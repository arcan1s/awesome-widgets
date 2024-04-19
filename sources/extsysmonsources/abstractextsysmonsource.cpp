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

#include "abstractextsysmonsource.h"

#include "awdebug.h"


AbstractExtSysMonSource::AbstractExtSysMonSource(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


// This method returns -1 in case of invalid source name (like if there is no number)
int AbstractExtSysMonSource::index(const QString &_source)
{
    auto match = NUMBER_REGEX.match(_source);
    return match.hasMatch() ? match.captured().toInt() : -1;
}


KSysGuard::SensorInfo *AbstractExtSysMonSource::makeSensorInfo(const QString &_name, const QMetaType::Type _type,
                                                               const KSysGuard::Unit _unit, const double _min,
                                                               const double _max)
{
    auto info = new KSysGuard::SensorInfo();
    info->name = _name;
    info->variantType = static_cast<QVariant::Type>(_type);

    info->unit = _unit;

    info->min = _min;
    info->max = _max;

    return info;
}
