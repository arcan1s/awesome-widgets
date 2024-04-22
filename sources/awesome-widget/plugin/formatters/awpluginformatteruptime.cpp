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

#include "awpluginformatteruptime.h"


QString AWPluginFormatterUptime::format(const QVariant &_value, const QString &, const AWPluginFormatSettings &_settings) const
{
    auto value = static_cast<long>(_value.toDouble());
    return format(value, _settings.customUptime);
}


QString AWPluginFormatterUptime::format(const long &_value, QString _formatString)
{
    auto seconds = _value - _value % 60;
    auto minutes = seconds / 60 % 60;
    auto hours = ((seconds / 60) - minutes) / 60 % 24;
    auto days = (((seconds / 60) - minutes) / 60 - hours) / 24;

    _formatString.replace("$dd", QString("%1").arg(days, 3, 10, QChar('0')));
    _formatString.replace("$d", QString("%1").arg(days));
    _formatString.replace("$hh", QString("%1").arg(hours, 2, 10, QChar('0')));
    _formatString.replace("$h", QString("%1").arg(hours));
    _formatString.replace("$mm", QString("%1").arg(minutes, 2, 10, QChar('0')));
    _formatString.replace("$m", QString("%1").arg(minutes));

    return _formatString;
}
