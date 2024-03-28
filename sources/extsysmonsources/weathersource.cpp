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

#include "weathersource.h"

#include "awdebug.h"
#include "extweather.h"


WeatherSource::WeatherSource(QObject *_parent)
    : AbstractExtSysMonSource(_parent)
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_extWeather = new ExtItemAggregator<ExtWeather>(nullptr, "weather");
    m_extWeather->initSockets();
}


QVariant WeatherSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (!m_values.contains(_source))
        m_values = dataByItem(m_extWeather, _source);

    return m_values.take(_source);
}


QHash<QString, KSysGuard::SensorInfo *> WeatherSource::sources() const
{
    auto result = QHash<QString, KSysGuard::SensorInfo *>();

    for (auto &item : m_extWeather->activeItems()) {
        result.insert(item->tag("weatherId"), makeSensorInfo(QString("Numeric weather ID for '%1'").arg(item->uniq()),
                                                             QVariant::Int, KSysGuard::UnitNone, 0, 1000));
        result.insert(item->tag("weather"),
                      makeSensorInfo(QString("ID string map for '%1'").arg(item->uniq()), QVariant::String));
        result.insert(item->tag("humidity"), makeSensorInfo(QString("Humidity for '%1'").arg(item->uniq()),
                                                            QVariant::Int, KSysGuard::UnitPercent, 0, 100));
        result.insert(item->tag("pressure"),
                      makeSensorInfo(QString("Atmospheric pressure for '%1'").arg(item->uniq()), QVariant::Int));
        result.insert(item->tag("temperature"), makeSensorInfo(QString("Temperature for '%1'").arg(item->uniq()),
                                                               QVariant::Double, KSysGuard::UnitCelsius));
        result.insert(item->tag("timestamp"), makeSensorInfo(QString("Timestamp for '%1'").arg(item->uniq()),
                                                             QVariant::DateTime, KSysGuard::UnitNone));
    }

    return result;
}
