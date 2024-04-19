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

#include "timesource.h"

#include "awdebug.h"


TimeSource::TimeSource(QObject *_parent)
    : AbstractExtSysMonSource(_parent)
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant TimeSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (_source == "now") {
        return QDateTime::currentSecsSinceEpoch();
    }

    return {};
}


QHash<QString, KSysGuard::SensorInfo *> TimeSource::sources() const
{
    auto result = QHash<QString, KSysGuard::SensorInfo *>();

    result.insert("now", makeSensorInfo("Current time", QMetaType::LongLong, KSysGuard::UnitSecond));

    return result;
}
