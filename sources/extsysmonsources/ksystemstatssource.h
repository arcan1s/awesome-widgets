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

#ifndef KSYSTEMSTATSSOURCE_H
#define KSYSTEMSTATSSOURCE_H

#include <systemstats/SensorInfo.h>

#include "abstractextsysmonsource.h"


namespace KSysGuard::SystemStats
{
class DBusInterface;
}


class KSystemStatsSource : public AbstractExtSysMonSource
{
    Q_OBJECT

public:
    explicit KSystemStatsSource(QObject *_parent, const QStringList &_args);
    ~KSystemStatsSource() override;
    QVariant data(const QString &_source) override;
    [[nodiscard]] QVariantMap initialData(const QString &_source) const override;
    void run() override;
    [[nodiscard]] QStringList sources() const override;

public slots:
    void sensorAdded(const QString &_sensor);
    void sensorRemoved(const QString &_sensor);
    void updateData(KSysGuard::SensorDataList _data);
    void updateSensor(const QHash<QString, KSysGuard::SensorInfo> &_sensor);

private:
    KSysGuard::SystemStats::DBusInterface *m_interface = nullptr;
    QVariantHash m_values;
    QVariantHash m_sources;
    void loadSources();
};


#endif /* KSYSTEMSTATSSOURCE_H */
