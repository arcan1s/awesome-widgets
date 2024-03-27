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
#include <ksysguard/systemstats/SensorContainer.h>

#include "abstractextsysmonsource.h"


class AbstractExtSysMonSource;
class ExtSysMonSensor;

class ExtSysMonAggregator : public KSysGuard::SensorContainer
{
    Q_OBJECT

public:
    explicit ExtSysMonAggregator(const QString &_id, const QString &_name, KSysGuard::SensorPlugin *_parent,
                                 const QHash<QString, QString> &_config);

private:
    void createSensor(const QString &_id, const QString &_name, AbstractExtSysMonSource *_source);
    void init(const QHash<QString, QString> &_config);
};
