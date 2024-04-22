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

#include "awpluginformatternetunits.h"

#include <KI18n/KLocalizedString>


QString AWPluginFormatterNetUnits::format(const QVariant &_value, const QString &, const AWPluginFormatSettings &_settings) const
{
    auto value = _value.toDouble();
    return (value > MBinBytes) ? formatMB(_settings) : formatKB(_settings);
}


QString AWPluginFormatterNetUnits::formatKB(const AWPluginFormatSettings &_settings)
{
    return _settings.translate ? i18n("KB/s") : "KB/s";
}


QString AWPluginFormatterNetUnits::formatMB(const AWPluginFormatSettings &_settings)
{
    return _settings.translate ? i18n("MB/s") : "MB/s";
}
