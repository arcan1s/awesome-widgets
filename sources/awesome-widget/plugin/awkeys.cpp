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


AWKeys::AWKeys(QObject *_parent)
    : QObject(_parent)
{
    qSetMessagePattern(AWDebug::LOG_FORMAT);
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
    for (auto &metadata : AWDebug::getBuildData())
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
    connect(m_keyOperator, &AWKeyOperations::updateKeys, this, &AWKeys::reinitKeys);
    connect(m_timer, &QTimer::timeout, this, &AWKeys::updateTextData);

    // transfer signal from AWDataAggregator object to QML ui
    connect(m_dataAggregator, &AWDataAggregator::toolTipPainted, [this](const QString &_tooltip) { emit(needToolTipToBeUpdated(_tooltip)); });

    connect(this, &AWKeys::dropSourceFromDataengine, m_dataEngineAggregator, &AWDataEngineAggregator::dropSource);
    connect(m_dataEngineAggregator, &AWDataEngineAggregator::dataUpdated, this, &AWKeys::dataUpdated);
    // transfer signal from dataengine to update source list
    connect(m_dataEngineAggregator, &AWDataEngineAggregator::deviceAdded, m_keyOperator, &AWKeyOperations::addDevice);
}


AWKeys::~AWKeys()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_timer->stop();
    // delete dbus session
    auto id = reinterpret_cast<qlonglong>(this);
    QDBusConnection::sessionBus().unregisterObject(QString("/%1").arg(id));
}


void AWKeys::initDataAggregator(const QVariantMap &_tooltipParams)
{
    qCDebug(LOG_AW) << "Tooltip parameters" << _tooltipParams;

    // store parameters to generate m_requiredKeys
    m_tooltipParams = _tooltipParams;
    m_dataAggregator->setParameters(m_tooltipParams);
}


void AWKeys::initKeys(const QString &_currentPattern, const int _interval, const int _limit, const bool _optimize)
{
    qCDebug(LOG_AW) << "Pattern" << _currentPattern << "with interval" << _interval << "and queue limit" << _limit
                    << "with optimization" << _optimize;

    // init
    m_optimize = _optimize;
    m_threadPool->setMaxThreadCount(_limit == 0 ? QThread::idealThreadCount() : _limit);
    // child objects
    m_aggregator->initFormatters();
    m_keyOperator->setPattern(_currentPattern);
    m_keyOperator->updateCache();
    m_dataEngineAggregator->reconnectSources(_interval);

    // timer
    m_timer->setInterval(_interval);
    m_timer->start();
}


void AWKeys::setAggregatorProperty(const QString &_key, const QVariant &_value)
{
    qCDebug(LOG_AW) << "Key" << _key << "with value" << _value;

    m_aggregator->setProperty(_key.toUtf8().constData(), _value);
}


void AWKeys::setWrapNewLines(const bool _wrap)
{
    qCDebug(LOG_AW) << "Is wrapping enabled" << _wrap;

    m_wrapNewLines = _wrap;
}


void AWKeys::updateCache()
{
    return m_keyOperator->updateCache();
}


QStringList AWKeys::dictKeys(const bool _sorted, const QString &_regexp) const
{
    qCDebug(LOG_AW) << "Should be sorted" << _sorted << "and filter applied" << _regexp;

    // check if functions asked
    if (_regexp == "functions")
        return QString(STATIC_FUNCTIONS).split(',');
    // check if user defined keys asked
    if (_regexp == "userdefined")
        return m_keyOperator->userKeys();

    QStringList allKeys = m_keyOperator->dictKeys();
    // sort if required
    if (_sorted)
        allKeys.sort();

    return allKeys.filter(QRegularExpression(_regexp));
}


QVariantList AWKeys::getHddDevices() const
{
    QStringList hddDevices = m_keyOperator->devices("hdd");
    // required by selector in the UI
    hddDevices.insert(0, "disable");
    hddDevices.insert(0, "auto");

    // build model
    QVariantList devices;
    for (auto &device : hddDevices) {
        QVariantMap model;
        model["label"] = device;
        model["name"] = device;
        devices.append(model);
    }

    return devices;
}


QString AWKeys::infoByKey(const QString &_key) const
{
    qCDebug(LOG_AW) << "Requested info for key" << _key;

    return m_keyOperator->infoByKey(_key);
}


// HACK this method requires to define tag value from bar from UI interface
QString AWKeys::valueByKey(const QString &_key) const
{
    qCDebug(LOG_AW) << "Requested value for key" << _key;

    auto realKey = _key.startsWith("bar") ? m_keyOperator->infoByKey(_key) : _key;

    return m_aggregator->formatter(m_values[realKey], realKey, true);
}


void AWKeys::editItem(const QString &_type)
{
    qCDebug(LOG_AW) << "Item type" << _type;

    return m_keyOperator->editItem(_type);
}


void AWKeys::dataUpdated(const QHash<QString, KSysGuard::SensorInfo> &_sensors, const KSysGuard::SensorDataList &_data)
{
    for (auto &single : _data) {
        if (_sensors.contains(single.sensorProperty)) {
            setDataBySource(single.sensorProperty, _sensors.value(single.sensorProperty), single);
        }
        // TODO use QtConcurrent::map or something like that
        //        QtConcurrent::run(m_threadPool, this, &AWKeys::setDataBySource, "ss", sensor);
    }
}


void AWKeys::reinitKeys(const QStringList &_currentKeys)
{
    qCDebug(LOG_AW) << "Update found keys by using list" << _currentKeys;

    // append lists
    m_foundBars = AWPatternFunctions::findKeys(m_keyOperator->pattern(), _currentKeys, true);
    m_foundKeys = AWPatternFunctions::findKeys(m_keyOperator->pattern(), _currentKeys, false);
    m_foundLambdas = AWPatternFunctions::findLambdas(m_keyOperator->pattern());
    // generate list of required keys for bars
    QStringList barKeys;
    for (auto &bar : m_foundBars) {
        GraphicalItem *item = m_keyOperator->giByKey(bar);
        if (item->isCustom())
            item->setUsedKeys(AWPatternFunctions::findKeys(item->bar(), _currentKeys, false));
        else
            item->setUsedKeys(QStringList() << item->bar());
        barKeys.append(item->usedKeys());
    }
    // get required keys
    m_requiredKeys = m_optimize ? AWKeyCache::getRequiredKeys(m_foundKeys, barKeys, m_tooltipParams,
                                                              m_keyOperator->requiredUserKeys(), _currentKeys)
                                : QStringList();

    // set key data to m_aggregator
    m_aggregator->setDevices(m_keyOperator->devices());
}


void AWKeys::updateTextData()
{
    // do not do it in parallel to avoid race condition
    m_mutex.lock();
    calculateValues();
    auto text = parsePattern(m_keyOperator->pattern());
    // update tooltip values under lock
    m_dataAggregator->dataUpdate(m_values);
    m_mutex.unlock();

    emit(needTextToBeUpdated(text));
}


// HACK this method is required since I could not define some values by using
// specified pattern. Usually they are values which depend on several others
void AWKeys::calculateValues()
{
    // hddtot*
    QStringList mountDevices = m_keyOperator->devices("mount");
    for (auto &device : mountDevices) {
        int index = mountDevices.indexOf(device);
        m_values[QString("hddtotmb%1").arg(index)] = m_values[QString("hddfreemb%1").arg(index)].toDouble()
                                                     + m_values[QString("hddmb%1").arg(index)].toDouble();
        m_values[QString("hddtotgb%1").arg(index)] = m_values[QString("hddfreegb%1").arg(index)].toDouble()
                                                     + m_values[QString("hddgb%1").arg(index)].toDouble();
    }

    // memtot*
    m_values["memtotmb"] = m_values["memusedmb"].toLongLong() + m_values["memfreemb"].toLongLong();
    m_values["memtotgb"] = m_values["memusedgb"].toDouble() + m_values["memfreegb"].toDouble();
    // mem
    m_values["mem"] = 100.0f * m_values["memmb"].toDouble() / m_values["memtotmb"].toDouble();

    // up, down, upkb, downkb, upunits, downunits
    int netIndex = m_keyOperator->devices("net").indexOf(m_values["netdev"].toString());
    m_values["down"] = m_values[QString("down%1").arg(netIndex)];
    m_values["downkb"] = m_values[QString("downkb%1").arg(netIndex)];
    m_values["downtot"] = m_values[QString("downtot%1").arg(netIndex)];
    m_values["downtotkb"] = m_values[QString("downtotkb%1").arg(netIndex)];
    m_values["downunits"] = m_values[QString("downunits%1").arg(netIndex)];
    m_values["up"] = m_values[QString("up%1").arg(netIndex)];
    m_values["upkb"] = m_values[QString("upkb%1").arg(netIndex)];
    m_values["uptot"] = m_values[QString("uptot%1").arg(netIndex)];
    m_values["uptotkb"] = m_values[QString("uptotkb%1").arg(netIndex)];
    m_values["upunits"] = m_values[QString("upunits%1").arg(netIndex)];

    // swaptot*
    m_values["swaptotmb"] = m_values["swapmb"].toLongLong() + m_values["swapfreemb"].toLongLong();
    m_values["swaptotgb"] = m_values["swapgb"].toDouble() + m_values["swapfreegb"].toDouble();
    // swap
    m_values["swap"] = 100.0f * m_values["swapmb"].toDouble() / m_values["swaptotmb"].toDouble();

    // user defined keys
    for (auto &key : m_keyOperator->userKeys())
        m_values[key] = m_values[m_keyOperator->userKeySource(key)];

    // lambdas
    for (auto &key : m_foundLambdas)
        m_values[key] = AWPatternFunctions::expandLambdas(key, m_aggregator, m_values, m_foundKeys);
}


void AWKeys::createDBusInterface()
{
    // get this object id
    auto id = reinterpret_cast<qlonglong>(this);

    // create session
    QDBusConnection instanceBus = QDBusConnection::sessionBus();
    // HACK we are going to use different services because it binds to
    // application
    if (instanceBus.registerService(QString("%1.i%2").arg(AWDBUS_SERVICE).arg(id))) {
        if (!instanceBus.registerObject(AWDBUS_PATH, new AWDBusAdaptor(this), QDBusConnection::ExportAllContents))
            qCWarning(LOG_AW) << "Could not register DBus object, last error" << instanceBus.lastError().message();
    } else {
        qCWarning(LOG_AW) << "Could not register DBus service, last error" << instanceBus.lastError().message();
    }

    // and same instance but for id independent service
    QDBusConnection commonBus = QDBusConnection::sessionBus();
    if (commonBus.registerService(AWDBUS_SERVICE))
        commonBus.registerObject(AWDBUS_PATH, new AWDBusAdaptor(this), QDBusConnection::ExportAllContents);
}


QString AWKeys::parsePattern(QString _pattern) const
{
    // screen sign
    _pattern.replace("$$", QChar(0x1d));

    // lambdas
    for (auto &key : m_foundLambdas)
        _pattern.replace(QString("${{%1}}").arg(key), m_values[key].toString());

    // main keys
    for (auto &key : m_foundKeys)
        _pattern.replace(QString("$%1").arg(key), m_aggregator->formatter(m_values[key], key, true));

    // bars
    for (auto &bar : m_foundBars) {
        GraphicalItem *item = m_keyOperator->giByKey(bar);
        QString image = item->isCustom() ? item->image(
                            AWPatternFunctions::expandLambdas(item->bar(), m_aggregator, m_values, item->usedKeys()))
                                         : item->image(m_values[item->bar()]);
        _pattern.replace(QString("$%1").arg(bar), image);
    }

    // prepare strings
    _pattern.replace(QChar(0x1d), "$");
    if (m_wrapNewLines)
        _pattern.replace("\n", "<br>");

    return _pattern;
}


void AWKeys::setDataBySource(const QString &_source, const KSysGuard::SensorInfo &_sensor,
                             const KSysGuard::SensorData &_data)
{
    qCDebug(LOG_AW) << "Source" << _source << _sensor.name << "with data" << _data.payload;

    // first list init
    auto tags = m_aggregator->keysFromSource(_source);
    if (tags.isEmpty())
        tags = m_aggregator->registerSource(_source, _sensor.unit, m_requiredKeys);

    // update data or drop source if there are no matches and exit
    if (tags.isEmpty()) {
        qCInfo(LOG_AW) << "Sensor" << _source << "not found";
        return emit(dropSourceFromDataengine(_source));
    }

    m_mutex.lock();
    // HACK workaround for time values which are stored in the different path
    std::for_each(tags.cbegin(), tags.cend(), [this, &_data](const QString &tag) { m_values[tag] = _data.payload; });
    m_mutex.unlock();
}
