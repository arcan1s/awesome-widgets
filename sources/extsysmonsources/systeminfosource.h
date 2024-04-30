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

#include <QObject>

#include "abstractextsysmonsource.h"


class SystemInfoSource : public AbstractExtSysMonSource
{
    Q_OBJECT

public:
    explicit SystemInfoSource(QObject *_parent);
    ~SystemInfoSource() override = default;
    [[nodiscard]] QVariant data(const QString &_source) override;
    [[nodiscard]] QHash<QString, KSysGuard::SensorInfo *> sources() const override;

private:
    // configuration and values
    [[nodiscard]] static QVariant fromDBusVariant(const QVariant &_value);
    [[nodiscard]] static double getCurrentBrightness();
    [[nodiscard]] static double getCurrentVolume();
    [[nodiscard]] static QVariant sendDBusRequest(const QString &_destination, const QString &_path,
                                                  const QString &_interface, const QString &_method,
                                                  const QVariantList &_args = {});
};
