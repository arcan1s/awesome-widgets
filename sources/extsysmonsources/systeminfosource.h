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

#ifndef SYSTEMINFOSOURCE_H
#define SYSTEMINFOSOURCE_H

#include <QObject>

#include "abstractextsysmonsource.h"


class SystemInfoSource : public AbstractExtSysMonSource
{
    Q_OBJECT

public:
    explicit SystemInfoSource(QObject *_parent, const QStringList &_args);
    ~SystemInfoSource() override;
    QVariant data(const QString &_source) override;
    [[nodiscard]] KSysGuard::SensorInfo *initialData(const QString &_source) const override;
    void run() override;
    [[nodiscard]] QStringList sources() const override;

private:
    // configuration and values
    QVariantHash m_values;
    static QVariant fromDBusVariant(const QVariant &value);
    static double getCurrentBrightness();
    static double getCurrentVolume();
    static QVariant sendDBusRequest(const QString &destination, const QString &path, const QString &interface,
                                    const QString &method, const QVariantList &args = QVariantList());
};


#endif /* SYSTEMINFOSOURCE_H */
