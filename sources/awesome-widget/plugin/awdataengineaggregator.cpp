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

#include <QDBusConnection>
#include <ksysguard/formatter/Unit.h>
#include <ksysguard/systemstats/DBusInterface.h>

#include "awdebug.h"


AWDataEngineAggregator::AWDataEngineAggregator(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    qDBusRegisterMetaType<KSysGuard::SensorData>();
    qDBusRegisterMetaType<KSysGuard::SensorInfo>();
    qDBusRegisterMetaType<KSysGuard::SensorDataList>();
    qDBusRegisterMetaType<QHash<QString, KSysGuard::SensorInfo>>();

    m_interface = new KSysGuard::SystemStats::DBusInterface();

    connect(m_interface, &KSysGuard::SystemStats::DBusInterface::newSensorData, this, &AWDataEngineAggregator::updateData);
    connect(m_interface, &KSysGuard::SystemStats::DBusInterface::sensorMetaDataChanged, this, &AWDataEngineAggregator::updateSensors);
    connect(m_interface, &KSysGuard::SystemStats::DBusInterface::sensorAdded, this, &AWDataEngineAggregator::sensorAdded);
    connect(m_interface, &KSysGuard::SystemStats::DBusInterface::sensorRemoved, this, &AWDataEngineAggregator::sensorRemoved);

    loadSources();
}


AWDataEngineAggregator::~AWDataEngineAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    disconnectSources();
    m_interface->deleteLater();
}


void AWDataEngineAggregator::disconnectSources()
{
    m_interface->unsubscribe(m_sensors.keys());
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
}


void AWDataEngineAggregator::reconnectSources(const int interval)
{
    qCDebug(LOG_AW) << "Reconnect all sensors with update interval" << interval;

    disconnectSources();
    m_interface->subscribe(m_sensors.keys());
}


void AWDataEngineAggregator::dropSource(const QString &_source)
{
    qCDebug(LOG_AW) << "Disconnect sensor" << _source;

    m_interface->unsubscribe({_source});
}


void AWDataEngineAggregator::sensorAdded(const QString &_sensor)
{
    qCDebug(LOG_AW) << "New sensor added" << _sensor;

    // check if sensor is actually valid
    auto response = m_interface->sensors({_sensor});
    response.waitForFinished();
    auto info = response.value();
    if (info.count() != 1)
        return;

    qCWarning(LOG_AW) << info.keys();

    m_interface->subscribe({_sensor});
}


void AWDataEngineAggregator::sensorRemoved(const QString &_sensor)
{
    qCDebug(LOG_AW) << "Sensor" << _sensor << "has been removed";

    m_sensors.remove(_sensor);
    m_interface->unsubscribe({_sensor});
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
