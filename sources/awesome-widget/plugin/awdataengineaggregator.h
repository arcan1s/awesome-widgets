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

#include <memory>

namespace KSysGuard::SystemStats
{
class DBusInterface;
}

class AWDataEngineAggregator : public QObject
{
    Q_OBJECT

public:
    ~AWDataEngineAggregator() override;

    AWDataEngineAggregator(AWDataEngineAggregator &) = delete;
    void operator=(const AWDataEngineAggregator &) = delete;

    [[nodiscard]] static AWDataEngineAggregator *instance(QObject *_client)
    {
        static auto instance = loadInstance();
        instance->registerClient(_client);
        return instance.get();
    };

    void connectSources();
    void disconnectSources();
    void dropSourceForClient(QObject *_client, const QString &_source);
    [[nodiscard]] inline bool isSubscriptionUnused(const QString &_source) const;
    [[nodiscard]] static bool isValidSensor(const KSysGuard::SensorInfo &_sensor);
    void loadSources();
    void registerClient(QObject *_client);
    void unregisterClient(QObject *_client);

signals:
    void dataUpdated(const QHash<QString, KSysGuard::SensorInfo> &_sensors, const KSysGuard::SensorDataList &_data);
    void deviceAdded(const QString &_source);

public slots:
    void dropSource(const QString &_source);
    void sensorAdded(const QString &_sensor);
    void sensorRemoved(const QString &_sensor);
    void updateData(const KSysGuard::SensorDataList &_data);
    void updateSensors(const QHash<QString, KSysGuard::SensorInfo> &_sensors);

protected:
    explicit AWDataEngineAggregator(QObject *_parent = nullptr);

    [[nodiscard]] static std::unique_ptr<AWDataEngineAggregator> loadInstance()
    {
        auto instance = new AWDataEngineAggregator();
        return std::unique_ptr<AWDataEngineAggregator>(instance);
    };

private:
    QSet<QObject *> m_clients;
    QHash<QString, QSet<QObject *>> m_droppedBy;
    KSysGuard::SystemStats::DBusInterface *m_interface = nullptr;
    QHash<QString, KSysGuard::SensorInfo> m_sensors;
    QSet<QString> m_subscribed;
};
