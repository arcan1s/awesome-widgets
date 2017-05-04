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

#include "owmweatherprovider.h"

#include <QDateTime>
#include <QUrlQuery>

#include "awdebug.h"


OWMWeatherProvider::OWMWeatherProvider(QObject *parent, const int number)
    : AbstractWeatherProvider(parent, number)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


OWMWeatherProvider::~OWMWeatherProvider()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


void OWMWeatherProvider::initUrl(const QString &city, const QString &country,
                                 const int ts)
{
    qCDebug(LOG_LIB) << "Init query for" << city << country << "with ts" << ts;

    m_ts = ts;

    if (m_ts == 0)
        m_url = QUrl(OWM_WEATHER_URL);
    else
        m_url = QUrl(OWM_FORECAST_URL);
    QUrlQuery params;
    params.addQueryItem(QString("q"), QString("%1,%2").arg(city, country));
    params.addQueryItem(QString("units"), QString("metric"));
    m_url.setQuery(params);
}


QVariantHash OWMWeatherProvider::parse(const QVariantMap &json) const
{
    qCDebug(LOG_LIB) << "Parse json" << json;

    if (json[QString("cod")].toInt() != 200) {
        qCWarning(LOG_LIB) << "Invalid OpenWeatherMap return code"
                           << json[QString("cod")].toInt();
        return QVariantHash();
    }

    if (m_ts == 0) {
        return parseSingleJson(json);
    } else {
        QVariantList list = json[QString("list")].toList();
        return parseSingleJson(list.count() <= m_ts ? list.at(m_ts - 1).toMap()
                                                    : list.last().toMap());
    }
}


QUrl OWMWeatherProvider::url() const
{
    return m_url;
}


QVariantHash OWMWeatherProvider::parseSingleJson(const QVariantMap &json) const
{
    qCDebug(LOG_LIB) << "Single json data" << json;

    QVariantHash output;

    // weather status
    QVariantList weather = json[QString("weather")].toList();
    if (!weather.isEmpty()) {
        int id = weather.first().toMap()[QString("id")].toInt();
        output[QString("weatherId%1").arg(number())] = id;
    }

    // main data
    QVariantMap mainWeather = json[QString("main")].toMap();
    if (!weather.isEmpty()) {
        output[QString("humidity%1").arg(number())]
            = mainWeather[QString("humidity")].toFloat();
        output[QString("pressure%1").arg(number())]
            = mainWeather[QString("pressure")].toFloat();
        output[QString("temperature%1").arg(number())]
            = mainWeather[QString("temp")].toFloat();
    }

    // timestamp
    output[QString("timestamp%1").arg(number())]
        = QDateTime::fromTime_t(json[QString("dt")].toUInt()).toUTC();

    return output;
}
