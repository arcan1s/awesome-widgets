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

#include "awpluginmatchersensors.h"

#include <QRegularExpression>

#include "formatters/formatters.h"


QHash<QString, AWPluginFormaterInterface *> AWPluginMatcherSensors::keys(const QString &_source, KSysGuard::Unit _units,
                                                                         const AWPluginMatcherSettings &_settings) const
{
    // temperature
    auto index = _settings.sensors.indexOf(_source);
    auto key = QString("temp%1").arg(index);

    if (index == -1)
        return {};

    if (_units == KSysGuard::UnitCelsius)
        return {{key, AWPluginFormatterTemperature::instance()}};
    return {{key, AWPluginFormatterInteger::instance()}};
}


bool AWPluginMatcherSensors::matches(const QString &_source) const
{
    static auto regexp = QRegularExpression("^cpu/cpu.*/temperature$");
    return _source.startsWith("lmsensors/") || _source == "cpu/all/averageTemperature" || _source.contains(regexp);
}
