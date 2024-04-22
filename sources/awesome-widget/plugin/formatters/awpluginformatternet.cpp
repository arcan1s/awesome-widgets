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

#include "awpluginformatternet.h"


QString AWPluginFormatterNet::format(const QVariant &_value, const QString &, const AWPluginFormatSettings &) const
{
    auto value = _value.toDouble();
    return (value > MBinBytes) ? formatMB(value) : formatKB(value);
}


QString AWPluginFormatterNet::formatKB(const double &_value)
{
    return QString("%1").arg(_value / KBinBytes, 4, 'f', 0);
}


QString AWPluginFormatterNet::formatMB(const double &_value)
{
    return QString("%1").arg(_value / MBinBytes, 4, 'f', 1);
}
