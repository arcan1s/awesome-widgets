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

#pragma once

#include <ksysguard/systemstats/SensorInfo.h>

#include <QHash>
#include <QObject>
#include <QSet>


namespace KSysGuard::SystemStats
{
class DBusInterface;
}

class AWDataEngineAggregator : public QObject
{
    Q_OBJECT

public:
    explicit AWDataEngineAggregator(QObject *_parent = nullptr);
    ~AWDataEngineAggregator() override;
    void connectSources();
    void disconnectSources();
    [[nodiscard]] static bool isValidSensor(const KSysGuard::SensorInfo &_sensor);
    void loadSources();

signals:
    void dataUpdated(const QHash<QString, KSysGuard::SensorInfo> &_sensors, const KSysGuard::SensorDataList &_data);
    void deviceAdded(const QString &_source);

public slots:
    void dropSource(const QString &_source);
    void sensorAdded(const QString &_sensor);
    void sensorRemoved(const QString &_sensor);
    void updateData(KSysGuard::SensorDataList _data);
    void updateSensors(const QHash<QString, KSysGuard::SensorInfo> &_sensors);

private:
    KSysGuard::SystemStats::DBusInterface *m_interface = nullptr;
    QHash<QString, KSysGuard::SensorInfo> m_sensors;
    QSet<QString> m_subscribed;
};
