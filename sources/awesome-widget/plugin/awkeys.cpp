/***************************************************************************
 *   This file is part of awesome-widgets                                  *
 *                                                                         *
 *   awesome-widgets is free software: you can redistribute it and/or      *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   awesome-widgets is distributed in the hope that it will be useful,    *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with awesome-widgets. If not, see http://www.gnu.org/licenses/  *
 ***************************************************************************/

#include "awkeys.h"

#include <QDBusConnection>
#include <QDBusError>
#include <QRegExp>
#include <QThread>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>

#include "awdataaggregator.h"
#include "awdataengineaggregator.h"
#include "awdbusadaptor.h"
#include "awdebug.h"
#include "awkeycache.h"
#include "awkeyoperations.h"
#include "awkeysaggregator.h"
#include "awpatternfunctions.h"
#include "graphicalitem.h"


AWKeys::AWKeys(QObject *parent)
    : QObject(parent)
{
    qSetMessagePattern(LOG_FORMAT);
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
    for (auto metadata : getBuildData())
        qCDebug(LOG_AW) << metadata;

    // thread pool
    m_threadPool = new QThreadPool(this);

    m_aggregator = new AWKeysAggregator(this);
    m_dataAggregator = new AWDataAggregator(this);
    m_dataEngineAggregator = new AWDataEngineAggregator(this);
    m_keyOperator = new AWKeyOperations(this);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);

    createDBusInterface();

    // update key data if required
    connect(m_keyOperator, SIGNAL(updateKeys(QStringList)), this,
            SLOT(reinitKeys(QStringList)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateTextData()));
    // transfer signal from AWDataAggregator object to QML ui
    connect(m_dataAggregator, SIGNAL(toolTipPainted(const QString)), this,
            SIGNAL(needToolTipToBeUpdated(const QString)));
    connect(this, SIGNAL(dropSourceFromDataengine(QString)),
            m_dataEngineAggregator, SLOT(dropSource(QString)));
    // transfer signal from dataengine to update source list
    connect(m_dataEngineAggregator, SIGNAL(deviceAdded(const QString &)),
            m_keyOperator, SLOT(addDevice(const QString &)));
}


AWKeys::~AWKeys()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_timer->stop();
    delete m_timer;

    // delete dbus session
    long id = reinterpret_cast<long>(this);
    QDBusConnection::sessionBus().unregisterObject(QString("/%1").arg(id));
    QDBusConnection::sessionBus().unregisterService(AWDBUS_SERVICE);

    // core
    delete m_dataEngineAggregator;
    delete m_threadPool;
    delete m_aggregator;
    delete m_dataAggregator;
    delete m_keyOperator;
}


void AWKeys::initDataAggregator(const QVariantMap tooltipParams)
{
    qCDebug(LOG_AW) << "Tooltip parameters" << tooltipParams;

    // store parameters to generate m_requiredKeys
    m_tooltipParams = tooltipParams;
    m_dataAggregator->setParameters(m_tooltipParams);
}


void AWKeys::initKeys(const QString currentPattern, const int interval,
                      const int limit, const bool optimize)
{
    qCDebug(LOG_AW) << "Pattern" << currentPattern << "with interval"
                    << interval << "and queue limit" << limit
                    << "with optimization" << optimize;

    // init
    m_optimize = optimize;
    m_threadPool->setMaxThreadCount(limit == 0 ? QThread::idealThreadCount()
                                               : limit);
    // child objects
    m_aggregator->initFormatters();
    m_keyOperator->setPattern(currentPattern);
    m_keyOperator->updateCache();
    m_dataEngineAggregator->clear();
    m_dataEngineAggregator->initDataEngines(interval);

    // timer
    m_timer->setInterval(interval);
    m_timer->start();
}


void AWKeys::setAggregatorProperty(const QString key, const QVariant value)
{
    qCDebug(LOG_AW) << "Key" << key << "with value" << value;

    m_aggregator->setProperty(key.toUtf8().constData(), value);
}


void AWKeys::setWrapNewLines(const bool wrap)
{
    qCDebug(LOG_AW) << "Is wrapping enabled" << wrap;

    m_wrapNewLines = wrap;
}


void AWKeys::updateCache()
{
    return m_keyOperator->updateCache();
}


QStringList AWKeys::dictKeys(const bool sorted, const QString regexp) const
{
    qCDebug(LOG_AW) << "Should be sorted" << sorted << "and filter applied"
                    << regexp;

    // check if functions asked
    if (regexp == QString("functions"))
        return QString(STATIC_FUNCTIONS).split(QChar(','));

    QStringList allKeys = m_keyOperator->dictKeys();
    // sort if required
    if (sorted)
        allKeys.sort();

    return allKeys.filter(QRegExp(regexp));
}


QVariantList AWKeys::getHddDevices() const
{
    QStringList hddDevices = m_keyOperator->devices(QString("hdd"));
    // required by selector in the UI
    hddDevices.insert(0, QString("disable"));
    hddDevices.insert(0, QString("auto"));

    // build model
    QVariantList devices;
    for (auto device : hddDevices) {
        QVariantMap model;
        model[QString("label")] = device;
        model[QString("name")] = device;
        devices.append(model);
    }

    return devices;
}


QString AWKeys::infoByKey(QString key) const
{
    qCDebug(LOG_AW) << "Requested info for key" << key;

    return m_keyOperator->infoByKey(key);
}


// HACK this method requires to define tag value from bar from UI interface
QString AWKeys::valueByKey(QString key) const
{
    qCDebug(LOG_AW) << "Requested value for key" << key;

    QString trueKey
        = key.startsWith(QString("bar")) ? m_keyOperator->infoByKey(key) : key;

    return m_aggregator->formatter(m_values[trueKey], trueKey);
}


void AWKeys::editItem(const QString type)
{
    qCDebug(LOG_AW) << "Item type" << type;

    return m_keyOperator->editItem(type);
}


void AWKeys::dataUpdated(const QString &sourceName,
                         const Plasma::DataEngine::Data &data)
{
    // run concurrent data update
    QtConcurrent::run(m_threadPool, this, &AWKeys::setDataBySource, sourceName,
                      data);
}


void AWKeys::reinitKeys(const QStringList currentKeys)
{
    qCDebug(LOG_AW) << "Update found keys by using list" << currentKeys;

    // append lists
    m_foundBars = AWPatternFunctions::findKeys(m_keyOperator->pattern(),
                                               currentKeys, true);
    m_foundKeys = AWPatternFunctions::findKeys(m_keyOperator->pattern(),
                                               currentKeys, false);
    m_foundLambdas = AWPatternFunctions::findLambdas(m_keyOperator->pattern());
    // generate list of required keys for bars
    QStringList barKeys;
    for (auto bar : m_foundBars) {
        GraphicalItem *item = m_keyOperator->giByKey(bar);
        if (item->isCustom())
            item->setUsedKeys(
                AWPatternFunctions::findKeys(item->bar(), currentKeys, false));
        else
            item->setUsedKeys(QStringList() << item->bar());
        barKeys.append(item->usedKeys());
    }
    // get required keys
    m_requiredKeys
        = m_optimize ? AWKeyCache::getRequiredKeys(m_foundKeys, barKeys,
                                                   m_tooltipParams, currentKeys)
                     : QStringList();

    // set key data to m_aggregator
    m_aggregator->setDevices(m_keyOperator->devices());
}


void AWKeys::updateTextData()
{
    // do not do it in parallel to avoid race condition
    m_mutex.lock();
    calculateValues();
    QString text = parsePattern(m_keyOperator->pattern());
    m_mutex.unlock();

    emit(needTextToBeUpdated(text));
    emit(m_dataAggregator->updateData(m_values));
}


// HACK this method is required since I could not define some values by using
// specified pattern. Usually they are values which depend on several others
void AWKeys::calculateValues()
{
    // hddtot*
    QStringList mountDevices = m_keyOperator->devices(QString("mount"));
    for (auto device : mountDevices) {
        int index = mountDevices.indexOf(device);
        m_values[QString("hddtotmb%1").arg(index)]
            = m_values[QString("hddfreemb%1").arg(index)].toFloat()
              + m_values[QString("hddmb%1").arg(index)].toFloat();
        m_values[QString("hddtotgb%1").arg(index)]
            = m_values[QString("hddfreegb%1").arg(index)].toFloat()
              + m_values[QString("hddgb%1").arg(index)].toFloat();
    }

    // memtot*
    m_values[QString("memtotmb")] = m_values[QString("memusedmb")].toInt()
                                    + m_values[QString("memfreemb")].toInt();
    m_values[QString("memtotgb")] = m_values[QString("memusedgb")].toFloat()
                                    + m_values[QString("memfreegb")].toFloat();
    // mem
    m_values[QString("mem")] = 100.0f * m_values[QString("memmb")].toFloat()
                               / m_values[QString("memtotmb")].toFloat();

    // up, down, upkb, downkb, upunits, downunits
    int netIndex = m_keyOperator->devices(QString("net"))
                       .indexOf(m_values[QString("netdev")].toString());
    m_values[QString("down")] = m_values[QString("down%1").arg(netIndex)];
    m_values[QString("downkb")] = m_values[QString("downkb%1").arg(netIndex)];
    m_values[QString("downtotal")]
        = m_values[QString("downtotal%1").arg(netIndex)];
    m_values[QString("downtotalkb")]
        = m_values[QString("downtotalkb%1").arg(netIndex)];
    m_values[QString("downunits")]
        = m_values[QString("downunits%1").arg(netIndex)];
    m_values[QString("up")] = m_values[QString("up%1").arg(netIndex)];
    m_values[QString("upkb")] = m_values[QString("upkb%1").arg(netIndex)];
    m_values[QString("uptotal")] = m_values[QString("uptotal%1").arg(netIndex)];
    m_values[QString("uptotalkb")]
        = m_values[QString("uptotalkb%1").arg(netIndex)];
    m_values[QString("upunits")] = m_values[QString("upunits%1").arg(netIndex)];

    // swaptot*
    m_values[QString("swaptotmb")] = m_values[QString("swapmb")].toInt()
                                     + m_values[QString("swapfreemb")].toInt();
    m_values[QString("swaptotgb")]
        = m_values[QString("swapgb")].toFloat()
          + m_values[QString("swapfreegb")].toFloat();
    // swap
    m_values[QString("swap")] = 100.0f * m_values[QString("swapmb")].toFloat()
                                / m_values[QString("swaptotmb")].toFloat();

    // lambdas
    for (auto key : m_foundLambdas)
        m_values[key] = AWPatternFunctions::expandLambdas(
            key, m_aggregator, m_values, m_foundKeys);
}


void AWKeys::createDBusInterface()
{
    // get this object id
    long id = reinterpret_cast<long>(this);

    // create session
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.registerService(AWDBUS_SERVICE))
        qCWarning(LOG_AW) << "Could not register DBus service, last error"
                          << bus.lastError().message();
    if (!bus.registerObject(QString("/%1").arg(id), new AWDBusAdaptor(this),
                            QDBusConnection::ExportAllContents))
        qCWarning(LOG_AW) << "Could not register DBus object, last error"
                          << bus.lastError().message();
}


QString AWKeys::parsePattern(QString pattern) const
{
    // screen sign
    pattern.replace(QString("$$"), QString(0x1d));

    // lambdas
    for (auto key : m_foundLambdas)
        pattern.replace(QString("${{%1}}").arg(key), m_values[key].toString());

    // main keys
    for (auto key : m_foundKeys)
        pattern.replace(QString("$%1").arg(key), [this](const QString &tag,
                                                        const QVariant &value) {
            QString strValue = m_aggregator->formatter(value, tag);
            if ((!tag.startsWith(QString("custom")))
                && (!tag.startsWith(QString("weather"))))
                strValue.replace(QString(" "), QString("&nbsp;"));
            return strValue;
        }(key, m_values[key]));

    // bars
    for (auto bar : m_foundBars) {
        GraphicalItem *item = m_keyOperator->giByKey(bar);
        QString image
            = item->isCustom()
                  ? item->image(AWPatternFunctions::expandLambdas(
                        item->bar(), m_aggregator, m_values, item->usedKeys()))
                  : item->image(m_values[item->bar()]);
        pattern.replace(QString("$%1").arg(bar), image);
    }

    // prepare strings
    pattern.replace(QString(0x1d), QString("$"));
    if (m_wrapNewLines)
        pattern.replace(QString("\n"), QString("<br>"));

    return pattern;
}


void AWKeys::setDataBySource(const QString &sourceName, const QVariantMap &data)
{
    qCDebug(LOG_AW) << "Source" << sourceName << "with data" << data;

    // first list init
    QStringList tags = m_aggregator->keysFromSource(sourceName);
    if (tags.isEmpty())
        tags = m_aggregator->registerSource(
            sourceName, data[QString("units")].toString(), m_requiredKeys);

    // update data or drop source if there are no matches and exit
    if (tags.isEmpty()) {
        qCInfo(LOG_AW) << "Source" << sourceName << "not found";
        return emit(dropSourceFromDataengine(sourceName));
    }

    m_mutex.lock();
    // HACK workaround for time values which are stored in the different path
    std::for_each(tags.cbegin(), tags.cend(),
                  [this, &data, &sourceName](const QString &tag) {
                      m_values[tag] = sourceName == QString("Local")
                                          ? data[QString("DateTime")]
                                          : data[QString("value")];
                  });
    m_mutex.unlock();
}
