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

    m_interface = new KSysGuard::SystemStats::DBusInterface(
        KSysGuard::SystemStats::ServiceName, KSysGuard::SystemStats::ObjectPath, QDBusConnection::sessionBus(), this);

    connect(m_interface, SIGNAL(newSensorData(KSysGuard::SensorDataList)), this,
            SLOT(updateData(KSysGuard::SensorDataList)));
    connect(m_interface, SIGNAL(sensorMetaDataChanged(const QHash<QString, KSysGuard::SensorInfo> &)), this,
            SLOT(updateSensors(const QHash<QString, KSysGuard::SensorInfo> &)));
    connect(m_interface, SIGNAL(sensorAdded(const QString &)), this, SLOT(sensorAdded(const QString &)));
    connect(m_interface, SIGNAL(sensorRemoved(const QString &)), this, SLOT(sensorRemoved(const QString &)));

    loadSources();
}


AWDataEngineAggregator::~AWDataEngineAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    disconnectSources();
}


void AWDataEngineAggregator::disconnectSources()
{
    m_interface->unsubscribe(m_sensors.keys());
}


void AWDataEngineAggregator::loadSources()
{
    auto response = m_interface->allSensors();
    response.waitForFinished();

    auto sensors = response.value();
    updateSensors(sensors);
    for (auto &sensor : sensors.keys())
        sensorAdded(sensor);
}


void AWDataEngineAggregator::reconnectSources(const int interval)
{
    qCDebug(LOG_AW) << "Reconnect all sources with update interval" << interval;

    disconnectSources();
    m_interface->subscribe(m_sensors.keys());
}


void AWDataEngineAggregator::dropSource(const QString &_source)
{
    qCDebug(LOG_AW) << "Disconnect source" << _source;

    m_interface->unsubscribe({_source});
}


void AWDataEngineAggregator::sensorAdded(const QString &_sensor)
{
    qCDebug(LOG_AW) << "New sensor added" << _sensor;

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
    for (auto sensor = _sensors.cbegin(); sensor != _sensors.cend(); ++sensor)
        m_sensors.insert(sensor.key(), sensor.value());
}
