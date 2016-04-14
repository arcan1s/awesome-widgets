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

#include <QtConcurrent/QtConcurrent>
#include <QJSEngine>
#include <QRegExp>
#include <QThread>

#include "awdataaggregator.h"
#include "awdataengineaggregator.h"
#include "awdebug.h"
#include "awkeycache.h"
#include "awkeyoperations.h"
#include "awkeysaggregator.h"
#include "awpatternfunctions.h"
#include "graphicalitem.h"
#include "version.h"


AWKeys::AWKeys(QObject *parent)
    : QObject(parent)
{
    qSetMessagePattern(LOG_FORMAT);
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
    for (auto metadata : getBuildData())
        qCDebug(LOG_AW) << metadata;

    // thread pool
    m_threadPool = new QThreadPool(this);

    aggregator = new AWKeysAggregator(this);
    dataAggregator = new AWDataAggregator(this);
    dataEngineAggregator = new AWDataEngineAggregator(this);
    keyOperator = new AWKeyOperations(this);

    // update key data if required
    connect(keyOperator, SIGNAL(updateKeys(QStringList)), this,
            SLOT(reinitKeys(QStringList)));
    // transfer signal from AWDataAggregator object to QML ui
    connect(dataAggregator, SIGNAL(toolTipPainted(const QString)), this,
            SIGNAL(needToolTipToBeUpdated(const QString)));
    connect(this, SIGNAL(needToBeUpdated()), this, SLOT(updateTextData()));
    connect(this, SIGNAL(dropSourceFromDataengine(QString)),
            dataEngineAggregator, SLOT(dropSource(QString)));
    // transfer signal from dataengine to update source list
    connect(dataEngineAggregator, SIGNAL(deviceAdded(const QString &)),
            keyOperator, SLOT(addDevice(const QString &)));
}


AWKeys::~AWKeys()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    // core
    delete dataEngineAggregator;
    delete m_threadPool;
    delete aggregator;
    delete dataAggregator;
    delete keyOperator;
}


void AWKeys::initDataAggregator(const QVariantMap tooltipParams)
{
    qCDebug(LOG_AW) << "Tooltip parameters" << tooltipParams;

    // store parameters to generate m_requiredKeys
    m_tooltipParams = tooltipParams;
    dataAggregator->setParameters(m_tooltipParams);
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
    keyOperator->setPattern(currentPattern);
    keyOperator->updateCache();
    dataEngineAggregator->clear();

    return dataEngineAggregator->initDataEngines(interval);
}


void AWKeys::setAggregatorProperty(const QString key, const QVariant value)
{
    qCDebug(LOG_AW) << "Key" << key << "with value" << value;

    aggregator->setProperty(key.toUtf8().constData(), value);
}


void AWKeys::setWrapNewLines(const bool wrap)
{
    qCDebug(LOG_AW) << "Is wrapping enabled" << wrap;

    m_wrapNewLines = wrap;
}


void AWKeys::updateCache()
{
    return keyOperator->updateCache();
}


QStringList AWKeys::dictKeys(const bool sorted, const QString regexp) const
{
    qCDebug(LOG_AW) << "Should be sorted" << sorted << "and filter applied"
                    << regexp;

    QStringList allKeys = keyOperator->dictKeys();
    // sort if required
    if (sorted)
        allKeys.sort();

    return allKeys.filter(QRegExp(regexp));
}


QStringList AWKeys::getHddDevices() const
{
    QStringList devices = keyOperator->devices(QString("hdd"));
    // required by selector in the UI
    devices.insert(0, QString("disable"));
    devices.insert(0, QString("auto"));

    return devices;
}


QString AWKeys::infoByKey(QString key) const
{
    qCDebug(LOG_AW) << "Requested info for key" << key;

    return keyOperator->infoByKey(key);
}


// HACK this method requires to define tag value from bar from UI interface
QString AWKeys::valueByKey(QString key) const
{
    qCDebug(LOG_AW) << "Requested value for key" << key;

    key.remove(QRegExp(QString("^bar[0-9]{1,}")));
    return aggregator->formater(values[key], key);
}


void AWKeys::editItem(const QString type)
{
    qCDebug(LOG_AW) << "Item type" << type;

    return keyOperator->editItem(type);
}


void AWKeys::dataUpdated(const QString &sourceName,
                         const Plasma::DataEngine::Data &data)
{
    // do not log these parameters
    if (sourceName == QString("update")) {
        qCInfo(LOG_AW) << "Update data";
        return emit(needToBeUpdated());
    }

    // run concurrent data update
    QtConcurrent::run(m_threadPool, this, &AWKeys::setDataBySource, sourceName,
                      data);
}


void AWKeys::reinitKeys(const QStringList currentKeys)
{
    qCDebug(LOG_AW) << "Update found keys by using list" << currentKeys;

    // append lists
    m_foundBars
        = AWPatternFunctions::findBars(keyOperator->pattern(), currentKeys);
    m_foundKeys
        = AWPatternFunctions::findKeys(keyOperator->pattern(), currentKeys);
    m_foundLambdas = AWPatternFunctions::findLambdas(keyOperator->pattern());
    // generate list of required keys for bars
    QStringList barKeys;
    for (auto bar : m_foundBars) {
        GraphicalItem *item = keyOperator->giByKey(bar);
        if (item->isCustom())
            item->setUsedKeys(
                AWPatternFunctions::findKeys(item->bar(), currentKeys));
        else
            item->setUsedKeys(QStringList() << item->bar());
        barKeys.append(item->usedKeys());
    }
    // get required keys
    m_requiredKeys
        = m_optimize ? AWKeyCache::getRequiredKeys(m_foundKeys, barKeys,
                                                   m_tooltipParams, currentKeys)
                     : QStringList();

    // set key data to aggregator
    aggregator->setDevices(keyOperator->devices());
}


void AWKeys::updateTextData()
{
    // do not do it in parallel to avoid race condition
    calculateValues();
    QString text = parsePattern(keyOperator->pattern());

    emit(needTextToBeUpdated(text));
    emit(dataAggregator->updateData(values));
}


// HACK this method is required since I could not define some values by using
// specified pattern. Usually they are values which depend on several others
void AWKeys::calculateValues()
{
    // hddtot*
    QStringList mountDevices = keyOperator->devices(QString("mount"));
    for (auto device : mountDevices) {
        int index = mountDevices.indexOf(device);
        values[QString("hddtotmb%1").arg(index)]
            = values[QString("hddfreemb%1").arg(index)].toFloat()
              + values[QString("hddmb%1").arg(index)].toFloat();
        values[QString("hddtotgb%1").arg(index)]
            = values[QString("hddfreegb%1").arg(index)].toFloat()
              + values[QString("hddgb%1").arg(index)].toFloat();
    }

    // memtot*
    values[QString("memtotmb")] = values[QString("memusedmb")].toInt()
                                  + values[QString("memfreemb")].toInt();
    values[QString("memtotgb")] = values[QString("memusedgb")].toFloat()
                                  + values[QString("memfreegb")].toFloat();
    // mem
    values[QString("mem")] = 100.0f * values[QString("memmb")].toFloat()
                             / values[QString("memtotmb")].toFloat();

    // up, down, upkb, downkb, upunits, downunits
    int netIndex = keyOperator->devices(QString("net"))
                       .indexOf(values[QString("netdev")].toString());
    values[QString("down")] = values[QString("down%1").arg(netIndex)];
    values[QString("downkb")] = values[QString("downkb%1").arg(netIndex)];
    values[QString("downunits")] = values[QString("downunits%1").arg(netIndex)];
    values[QString("up")] = values[QString("up%1").arg(netIndex)];
    values[QString("upkb")] = values[QString("upkb%1").arg(netIndex)];
    values[QString("upunits")] = values[QString("upunits%1").arg(netIndex)];

    // swaptot*
    values[QString("swaptotmb")] = values[QString("swapmb")].toInt()
                                   + values[QString("swapfreemb")].toInt();
    values[QString("swaptotgb")] = values[QString("swapgb")].toFloat()
                                   + values[QString("swapfreegb")].toFloat();
    // swap
    values[QString("swap")] = 100.0f * values[QString("swapmb")].toFloat()
                              / values[QString("swaptotmb")].toFloat();

    // lambdas
    for (auto key : m_foundLambdas)
        values[key] = AWPatternFunctions::expandLambdas(key, aggregator, values,
                                                        m_foundKeys);
}


QString AWKeys::parsePattern(QString pattern) const
{
    // screen sign
    pattern.replace(QString("$$"), QString(0x1d));

    // lambdas
    for (auto key : m_foundLambdas)
        pattern.replace(QString("${{%1}}").arg(key), values[key].toString());

    // main keys
    for (auto key : m_foundKeys)
        pattern.replace(QString("$%1").arg(key), [this](const QString &tag,
                                                        const QVariant &value) {
            QString strValue = aggregator->formater(value, tag);
            if ((!tag.startsWith(QString("custom")))
                && (!tag.startsWith(QString("weather"))))
                strValue.replace(QString(" "), QString("&nbsp;"));
            return strValue;
        }(key, values[key]));

    // bars
    for (auto bar : m_foundBars) {
        GraphicalItem *item = keyOperator->giByKey(bar);
        QString image
            = item->isCustom()
                  ? item->image(AWPatternFunctions::expandLambdas(
                        item->bar(), aggregator, values, item->usedKeys()))
                  : item->image(values[item->bar()]);
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
    QStringList tags = aggregator->keysFromSource(sourceName);
    if (tags.isEmpty())
        tags = aggregator->registerSource(
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
                      values[tag] = sourceName == QString("Local")
                                        ? data[QString("DateTime")]
                                        : data[QString("value")];
                  });
    m_mutex.unlock();
}
