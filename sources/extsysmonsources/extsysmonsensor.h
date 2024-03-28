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

#include <ksysguard/systemstats/SensorObject.h>


class AbstractExtSysMonSource;
class QTimer;

class ExtSysMonSensor : public KSysGuard::SensorObject
{
    Q_OBJECT

public:
    explicit ExtSysMonSensor(KSysGuard::SensorContainer *_parent, const QString &_id, const QString &_name,
                             AbstractExtSysMonSource *_source);
    ~ExtSysMonSensor() override;
    void changeSubscription(bool _subscribed);
    void update();

private:
    void loadProperties();
    AbstractExtSysMonSource *m_source = nullptr;
    QTimer *m_timer = nullptr;
};
