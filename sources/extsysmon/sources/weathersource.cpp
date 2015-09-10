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
    qCDebug(LOG_ESM);

    extWeather = new ExtItemAggregator<ExtWeather>(nullptr, QString("weather"));
    m_sources = getSources();
}


WeatherSource::~WeatherSource()
{
    qCDebug(LOG_ESM);

    delete extWeather;
}


QVariant WeatherSource::data(QString source)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    if (source.startsWith(QString("weather/weatherId")))
        values[source] = extWeather->itemByTagNumber(index(source))->run();
    QString base = QString(source).remove(QString("weather/"));
    return values[source][base];
}


QVariantMap WeatherSource::initialData(QString source) const
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    int ind = index(source);
    QVariantMap data;
    if (source.startsWith(QString("weather/weatherId"))) {
        data[QString("min")] = 0;
        data[QString("max")] = 1000;
        data[QString("name")] = QString("Numeric weather ID for '%1'").arg(extWeather->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("weather/weather"))) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")] = QString("ID string map for '%1'").arg(extWeather->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("weather/humidity"))) {
        data[QString("min")] = 0;
        data[QString("max")] = 100;
        data[QString("name")] = QString("Humidity for '%1'").arg(extWeather->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("%");
    } else if (source.startsWith(QString("weather/pressure"))) {
        data[QString("min")] = 0;
        data[QString("max")] = 0;
        data[QString("name")] = QString("Atmospheric pressure for '%1'").arg(extWeather->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("hPa");
    } else if (source.startsWith(QString("weather/temperature"))) {
        data[QString("min")] = 0.0;
        data[QString("max")] = 0.0;
        data[QString("name")] = QString("Temperature for '%1'").arg(extWeather->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("float");
        data[QString("units")] = QString("°C");
    }

    return data;
}


QStringList WeatherSource::sources() const
{
    qCDebug(LOG_ESM);

    return m_sources;
}


QStringList WeatherSource::getSources()
{
    qCDebug(LOG_ESM);

    QStringList sources;
    foreach(ExtWeather *item, extWeather->activeItems()) {
        sources.append(QString("weather/%1").arg(item->tag(QString("weatherId"))));
        sources.append(QString("weather/%1").arg(item->tag(QString("weather"))));
        sources.append(QString("weather/%1").arg(item->tag(QString("humidity"))));
        sources.append(QString("weather/%1").arg(item->tag(QString("pressure"))));
        sources.append(QString("weather/%1").arg(item->tag(QString("temperature"))));
    }

    return sources;
}
