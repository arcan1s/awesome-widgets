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

#include "awpluginformattertemperature.h"


QString AWPluginFormatterTemperature::format(const QVariant &_value, const QString &,
                                             const AWPluginFormatSettings &_settings) const
{
    auto converted = convert(_value.toDouble(), _settings.tempUnits);
    return QString("%1").arg(converted, 5, 'f', 1);
}


double AWPluginFormatterTemperature::convert(const double &_value, const QString &_units)
{
    auto converted = _value;
    if (_units == "Celsius") {
    } else if (_units == "Fahrenheit") {
        converted = _value * 9.0f / 5.0 + 32.0;
    } else if (_units == "Kelvin") {
        converted = _value + 273.15;
    } else if (_units == "Reaumur") {
        converted = _value * 0.8;
    } else if (_units == "cm^-1") {
        converted = (_value + 273.15) * 0.695;
    } else if (_units == "kJ/mol") {
        converted = (_value + 273.15) * 8.31;
    } else if (_units == "kcal/mol") {
        converted = (_value + 273.15) * 1.98;
    }

    return converted;
}
