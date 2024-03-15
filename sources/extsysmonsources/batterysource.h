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

#ifndef BATTERYSOURCE_H
#define BATTERYSOURCE_H

#include <QDateTime>
#include <QObject>

#include "abstractextsysmonsource.h"


class BatterySource : public AbstractExtSysMonSource
{
    Q_OBJECT

public:
    const int TREND_LIMIT = 20;

    explicit BatterySource(QObject *_parent, const QStringList &_args);
    ~BatterySource() override;
    QStringList getSources();
    QVariant data(const QString &_source) override;
    [[nodiscard]] KSysGuard::SensorInfo *initialData(const QString &_source) const override;
    void run() override;
    [[nodiscard]] QStringList sources() const override;

private:
    static double approximate(const QList<int> &_trend);
    void calculateRates();
    // configuration and values
    int m_batteriesCount = 0;
    QString m_acpiPath;
    QStringList m_sources;
    QDateTime m_timestamp;
    QHash<int, QList<int>> m_trend;
    QVariantHash m_values;
};


#endif /* BATTERYSOURCE_H */
