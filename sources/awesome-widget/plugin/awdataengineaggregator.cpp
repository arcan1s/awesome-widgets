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

#include "awdataengineaggregator.h"

#include <ksysguard/formatter/Unit.h>
#include <ksysguard/systemstats/DBusInterface.h>

#include <QDBusConnection>

#include "awdebug.h"


AWDataEngineAggregator::AWDataEngineAggregator(QObject *_parent)
    : QObject(_parent)
    , m_interface(new KSysGuard::SystemStats::DBusInterface())
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    qDBusRegisterMetaType<KSysGuard::SensorData>();
    qDBusRegisterMetaType<KSysGuard::SensorInfo>();
    qDBusRegisterMetaType<KSysGuard::SensorDataList>();
    qDBusRegisterMetaType<QHash<QString, KSysGuard::SensorInfo>>();

    connect(m_interface, &KSysGuard::SystemStats::DBusInterface::newSensorData, this,
            &AWDataEngineAggregator::updateData);
    connect(m_interface, &KSysGuard::SystemStats::DBusInterface::sensorMetaDataChanged, this,
            &AWDataEngineAggregator::updateSensors);
    connect(m_interface, &KSysGuard::SystemStats::DBusInterface::sensorAdded, this,
            &AWDataEngineAggregator::sensorAdded);
    connect(m_interface, &KSysGuard::SystemStats::DBusInterface::sensorRemoved, this,
            &AWDataEngineAggregator::sensorRemoved);

    loadSources();
}


AWDataEngineAggregator::~AWDataEngineAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    disconnectSources();
    m_interface->deleteLater();
}


void AWDataEngineAggregator::connectSources()
{
    auto keys = m_sensors.keys();
    auto newKeys = QSet(keys.cbegin(), keys.cend()) - m_subscribed;

    m_interface->subscribe(newKeys.values()).waitForFinished();
    m_subscribed.unite(newKeys);
}


void AWDataEngineAggregator::disconnectSources()
{
    m_interface->unsubscribe(m_subscribed.values()).waitForFinished();
    m_subscribed.clear();
}


bool AWDataEngineAggregator::isValidSensor(const KSysGuard::SensorInfo &_sensor)
{
    return _sensor.unit != KSysGuard::UnitInvalid;
}


void AWDataEngineAggregator::loadSources()
{
    auto response = m_interface->allSensors();
    response.waitForFinished();

    auto sensors = response.value();
    updateSensors(sensors);
    connectSources();

    for (auto &sensor : m_sensors.keys())
        emit(deviceAdded(sensor));
}


void AWDataEngineAggregator::dropSource(const QString &_source)
{
    qCDebug(LOG_AW) << "Disconnect sensor" << _source;

    if (m_subscribed.contains(_source)) {
        m_interface->unsubscribe({_source}).waitForFinished();
        m_subscribed.remove(_source);
    }
}


void AWDataEngineAggregator::sensorAdded(const QString &_sensor)
{
    qCDebug(LOG_AW) << "New sensor added" << _sensor;

    // check if sensor is actually valid
    auto response = m_interface->sensors({_sensor});
    response.waitForFinished();

    auto info = response.value().value(_sensor);
    if (!isValidSensor(info))
        return;

    m_sensors[_sensor] = info;
    dropSource(_sensor); // force reconnect
    if (!m_subscribed.contains(_sensor)) {
        m_interface->subscribe({_sensor}).waitForFinished();
        m_subscribed.insert(_sensor);
    }

    // notify about new device
    emit(deviceAdded(_sensor));
}


void AWDataEngineAggregator::sensorRemoved(const QString &_sensor)
{
    qCDebug(LOG_AW) << "Sensor" << _sensor << "has been removed";

    m_sensors.remove(_sensor);
    dropSource(_sensor);
}


void AWDataEngineAggregator::updateData(KSysGuard::SensorDataList _data)
{
    emit(dataUpdated(m_sensors, _data));
}


void AWDataEngineAggregator::updateSensors(const QHash<QString, KSysGuard::SensorInfo> &_sensors)
{
    for (auto sensor = _sensors.cbegin(); sensor != _sensors.cend(); ++sensor) {
        if (!isValidSensor(sensor.value()))
            continue;
        m_sensors.insert(sensor.key(), sensor.value());
    }
}
