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


#include "ksystemstatssource.h"

#include <QDBusConnection>
#include <systemstats/DBusInterface.h>

#include "awdebug.h"


KSystemStatsSource::KSystemStatsSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    qDBusRegisterMetaType<KSysGuard::SensorData>();
    qDBusRegisterMetaType<KSysGuard::SensorInfo>();
    qDBusRegisterMetaType<KSysGuard::SensorDataList>();
    qDBusRegisterMetaType<QHash<QString, KSysGuard::SensorInfo>>();

    m_interface = new KSysGuard::SystemStats::DBusInterface(
        KSysGuard::SystemStats::ServiceName, KSysGuard::SystemStats::ObjectPath, QDBusConnection::sessionBus(), this);
    connect(m_interface, SIGNAL(newSensorData(KSysGuard::SensorDataList)), this,
            SLOT(updateData(KSysGuard::SensorDataList)));
    connect(m_interface, SIGNAL(sensorAdded(const QString &)), this, SLOT(sensorAdded(const QString &)));
    connect(m_interface, SIGNAL(sensorMetaDataChanged(const QHash<QString, KSysGuard::SensorInfo> &)), this,
            SLOT(updateSensor(const QHash<QString, KSysGuard::SensorInfo> &)));
    connect(m_interface, SIGNAL(sensorRemoved(const QString &)), this, SLOT(sensorRemoved(const QString &)));

    loadSources();
    m_interface->subscribe(m_sources.keys());
}


KSystemStatsSource::~KSystemStatsSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant KSystemStatsSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    return m_values.value(_source, {});
}


QVariantMap KSystemStatsSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    return m_sources.value(_source, QVariantMap()).toMap();
}


void KSystemStatsSource::run() {}


QStringList KSystemStatsSource::sources() const
{
    return m_sources.keys();
}


void KSystemStatsSource::sensorAdded(const QString &_sensor)
{
    qCDebug(LOG_ESS) << "Sensor" << _sensor << "added";

    auto sensors = QStringList(_sensor);

    auto response = m_interface->sensors(sensors);
    response.waitForFinished();
    auto metadata = response.value();

    updateSensor(metadata);

    m_interface->subscribe(sensors);
    emit(sourceAdded(_sensor));
}


void KSystemStatsSource::sensorRemoved(const QString &_sensor)
{
    qCDebug(LOG_ESS) << "Sensor" << _sensor << "removed";

    m_values.remove(_sensor);
    m_sources.remove(_sensor);
    emit(sourceRemoved(_sensor));
}


void KSystemStatsSource::updateData(KSysGuard::SensorDataList _data)
{
    for (auto &change : _data)
        m_values[change.sensorProperty] = change.payload;
}


void KSystemStatsSource::updateSensor(const QHash<QString, KSysGuard::SensorInfo> &_sensor)
{
    qCDebug(LOG_ESS) << "Sensors" << _sensor.keys() << "were updated";

    for (auto it = _sensor.constBegin(); it != _sensor.constEnd(); it++) {
        auto value = it.value();

        QVariantMap data;
        data["min"] = value.min;
        data["max"] = value.max;
        data["name"] = value.name;
        data["type"] = value.variantType;
        data["units"] = value.unit;

        m_sources[it.key()] = data;
    }
}


void KSystemStatsSource::loadSources()
{
    auto response = m_interface->allSensors();
    response.waitForFinished();
    auto metadata = response.value();

    updateSensor(metadata);
}
