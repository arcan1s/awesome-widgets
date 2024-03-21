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

#include <ksysguard/formatter/Unit.h>
#include <ksysguard/systemstats/SensorInfo.h>

#include "awdebug.h"
#include "extweather.h"


WeatherSource::WeatherSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_extWeather = new ExtItemAggregator<ExtWeather>(nullptr, "weather");
    m_extWeather->initSockets();
    m_sources = getSources();
}


WeatherSource::~WeatherSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant WeatherSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    int ind = index(_source);
    if (!m_values.contains(_source)) {
        QVariantHash data = m_extWeather->itemByTagNumber(ind)->run();
        for (auto &key : data.keys())
            m_values[key] = data[key];
    }
    QVariant value = m_values.take(_source);
    return value;
}


KSysGuard::SensorInfo *WeatherSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    int ind = index(_source);
    auto data = new KSysGuard::SensorInfo();
    if (_source.startsWith("weatherId")) {
        data->min = 0;
        data->max = 1000;
        data->name = QString("Numeric weather ID for '%1'").arg(m_extWeather->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitNone;
    } else if (_source.startsWith("weather")) {
        data->name = QString("ID string map for '%1'").arg(m_extWeather->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    } else if (_source.startsWith("humidity")) {
        data->min = 0;
        data->max = 100;
        data->name = QString("Humidity for '%1'").arg(m_extWeather->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitPercent;
    } else if (_source.startsWith("pressure")) {
        data->min = 0;
        data->max = 0;
        data->name = QString("Atmospheric pressure for '%1'").arg(m_extWeather->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitNone;
    } else if (_source.startsWith("temperature")) {
        data->min = 0.0;
        data->max = 0.0;
        data->name = QString("Temperature for '%1'").arg(m_extWeather->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::Double;
        data->unit = KSysGuard::UnitCelsius;
    } else if (_source.startsWith("timestamp")) {
        data->name = QString("Timestamp for '%1'").arg(m_extWeather->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    }

    return data;
}


QStringList WeatherSource::sources() const
{
    return m_sources;
}


QStringList WeatherSource::getSources()
{
    QStringList sources;
    for (auto &item : m_extWeather->activeItems()) {
        sources.append(item->tag("weatherId"));
        sources.append(item->tag("weather"));
        sources.append(item->tag("humidity"));
        sources.append(item->tag("pressure"));
        sources.append(item->tag("temperature"));
        sources.append(item->tag("timestamp"));
    }

    return sources;
}
