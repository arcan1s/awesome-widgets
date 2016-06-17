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


WeatherSource::WeatherSource(QObject *parent, const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    extWeather = new ExtItemAggregator<ExtWeather>(nullptr, QString("weather"));
    m_sources = getSources();
}


WeatherSource::~WeatherSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    delete extWeather;
}


QVariant WeatherSource::data(QString source)
{
    qCDebug(LOG_ESS) << "Source" << source;

    int ind = index(source);
    source.remove(QString("weather/"));
    if (!m_values.contains(source)) {
        QVariantHash data = extWeather->itemByTagNumber(ind)->run();
        for (auto key : data.keys())
            m_values[key] = data[key];
    }
    QVariant value = m_values.take(source);
    return value;
}


QVariantMap WeatherSource::initialData(QString source) const
{
    qCDebug(LOG_ESS) << "Source" << source;

    int ind = index(source);
    QVariantMap data;
    if (source.startsWith(QString("weather/weatherId"))) {
        data[QString("min")] = 0;
        data[QString("max")] = 1000;
        data[QString("name")]
            = QString("Numeric weather ID for '%1'")
                  .arg(extWeather->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("weather/weather"))) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")]
            = QString("ID string map for '%1'")
                  .arg(extWeather->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("weather/humidity"))) {
        data[QString("min")] = 0;
        data[QString("max")] = 100;
        data[QString("name")]
            = QString("Humidity for '%1'")
                  .arg(extWeather->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("%");
    } else if (source.startsWith(QString("weather/pressure"))) {
        data[QString("min")] = 0;
        data[QString("max")] = 0;
        data[QString("name")]
            = QString("Atmospheric pressure for '%1'")
                  .arg(extWeather->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("mb");
    } else if (source.startsWith(QString("weather/temperature"))) {
        data[QString("min")] = 0.0;
        data[QString("max")] = 0.0;
        data[QString("name")]
            = QString("Temperature for '%1'")
                  .arg(extWeather->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("float");
        data[QString("units")] = QString("°C");
    } else if (source.startsWith(QString("weather/timestamp"))) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")]
            = QString("Timestamp for '%1'")
                  .arg(extWeather->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
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
    for (auto item : extWeather->activeItems()) {
        sources.append(
            QString("weather/%1").arg(item->tag(QString("weatherId"))));
        sources.append(
            QString("weather/%1").arg(item->tag(QString("weather"))));
        sources.append(
            QString("weather/%1").arg(item->tag(QString("humidity"))));
        sources.append(
            QString("weather/%1").arg(item->tag(QString("pressure"))));
        sources.append(
            QString("weather/%1").arg(item->tag(QString("temperature"))));
        sources.append(
            QString("weather/%1").arg(item->tag(QString("timestamp"))));
    }

    return sources;
}
