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

#include "yahooweatherprovider.h"

#include <QUrlQuery>

#include "awdebug.h"


YahooWeatherProvider::YahooWeatherProvider(QObject *parent, const int number)
    : AbstractWeatherProvider(parent, number)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


YahooWeatherProvider::~YahooWeatherProvider()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


void YahooWeatherProvider::initUrl(const QString city, const QString country,
                                   const int ts)
{
    qCDebug(LOG_LIB) << "Init query for" << city << country << "with ts" << ts;

    m_ts = ts;

    m_url = QUrl(YAHOO_WEATHER_URL);
    QUrlQuery params;
    params.addQueryItem(QString("format"), QString("json"));
    params.addQueryItem(QString("env"),
                        QString("store://datatables.org/alltableswithkeys"));
    params.addQueryItem(QString("q"),
                        QString(YAHOO_WEATHER_QUERY).arg(city, country));
    m_url.setQuery(params);
}


QVariantHash YahooWeatherProvider::parse(const QVariantMap &json) const
{
    qCDebug(LOG_LIB) << "Parse json" << json;

    QVariantMap jsonMap = json[QString("query")].toMap();
    if (jsonMap[QString("count")].toInt() != 1) {
        qCWarning(LOG_LIB) << "Found data count"
                           << json[QString("count")].toInt() << "is not 1";
        return QVariantHash();
    }
    QVariantMap results
        = jsonMap[QString("results")].toMap()[QString("channel")].toMap();
    QVariantMap item = results[QString("item")].toMap();
    QVariantMap atmosphere = results[QString("atmosphere")].toMap();

    return m_ts == 0 ? parseCurrent(item, atmosphere) : parseForecast(item);
}


QUrl YahooWeatherProvider::url() const
{
    return m_url;
}


QVariantHash
YahooWeatherProvider::parseCurrent(const QVariantMap &json,
                                   const QVariantMap &atmosphere) const
{
    qCDebug(LOG_LIB) << "Parse current weather from" << json;

    QVariantHash values;
    int id = json[QString("condition")].toMap()[QString("code")].toInt();
    values[QString("weatherId%1").arg(number())] = id;
    values[QString("temperature%1").arg(number())]
        = json[QString("condition")].toMap()[QString("temp")].toInt();
    values[QString("timestamp%1").arg(number())]
        = json[QString("condition")].toMap()[QString("date")].toString();
    values[QString("humidity%1").arg(number())]
        = atmosphere[QString("humidity")].toInt();
    // HACK temporary fix of invalid values on Yahoo! side
    values[QString("pressure%1").arg(number())] = static_cast<int>(
        atmosphere[QString("pressure")].toFloat() / 33.863753);

    return values;
}


QVariantHash YahooWeatherProvider::parseForecast(const QVariantMap &json) const
{
    qCDebug(LOG_LIB) << "Parse forecast from" << json;

    QVariantHash values;
    QVariantList weatherList = json[QString("forecast")].toList();
    QVariantMap weatherMap = weatherList.count() < m_ts
                                 ? weatherList.last().toMap()
                                 : weatherList.at(m_ts).toMap();
    int id = weatherMap[QString("code")].toInt();
    values[QString("weatherId%1").arg(number())] = id;
    values[QString("timestamp%1").arg(number())]
        = weatherMap[QString("date")].toString();
    // yahoo provides high and low temperatures. Lets calculate average one
    values[QString("temperature%1").arg(number())]
        = (weatherMap[QString("high")].toFloat()
           + weatherMap[QString("low")].toFloat())
          / 2.0;
    // ... and no forecast data for humidity and pressure
    values[QString("humidity%1").arg(number())] = 0;
    values[QString("pressure%1").arg(number())] = 0.0;

    return values;
}
