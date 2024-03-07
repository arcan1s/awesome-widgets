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


YahooWeatherProvider::YahooWeatherProvider(QObject *_parent)
    : AbstractWeatherProvider(_parent)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


YahooWeatherProvider::~YahooWeatherProvider()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


void YahooWeatherProvider::initUrl(const QString &_city, const QString &_country, const int ts)
{
    qCDebug(LOG_LIB) << "Init query for" << _city << _country << "with ts" << ts;

    m_ts = ts;

    m_url = QUrl(YAHOO_WEATHER_URL);
    QUrlQuery params;
    params.addQueryItem("format", "json");
    params.addQueryItem("env", "store://datatables.org/alltableswithkeys");
    params.addQueryItem("q", QString(YAHOO_WEATHER_QUERY).arg(_city, _country));
    m_url.setQuery(params);
}


QVariantHash YahooWeatherProvider::parse(const QVariantMap &_json) const
{
    qCDebug(LOG_LIB) << "Parse json" << _json;

    QVariantMap jsonMap = _json["query"].toMap();
    if (jsonMap["count"].toInt() != 1) {
        qCWarning(LOG_LIB) << "Found data count" << _json["count"].toInt() << "is not 1";
        return {};
    }
    QVariantMap results = jsonMap["results"].toMap()["channel"].toMap();
    QVariantMap item = results["item"].toMap();
    QVariantMap atmosphere = results["atmosphere"].toMap();

    return m_ts == 0 ? parseCurrent(item, atmosphere) : parseForecast(item);
}


QUrl YahooWeatherProvider::url() const
{
    return m_url;
}


QVariantHash YahooWeatherProvider::parseCurrent(const QVariantMap &_json, const QVariantMap &_atmosphere) const
{
    qCDebug(LOG_LIB) << "Parse current weather from" << _json;

    auto condition = _json["condition"].toMap();

    QVariantHash values;
    int id = _json["condition"].toMap()["code"].toInt();
    values[tag("weatherId")] = id;
    values[tag("temperature")] = condition["temp"].toInt();
    values[tag("timestamp")] = condition["date"].toString();
    values[tag("humidity")] = _atmosphere["humidity"].toInt();
    // HACK temporary fix of invalid values on Yahoo! side
    values[tag("pressure")] = static_cast<int>(_atmosphere["pressure"].toDouble() / 33.863753);

    return values;
}


QVariantHash YahooWeatherProvider::parseForecast(const QVariantMap &_json) const
{
    qCDebug(LOG_LIB) << "Parse forecast from" << _json;

    QVariantHash values;
    QVariantList weatherList = _json["forecast"].toList();
    QVariantMap weatherMap = weatherList.count() < m_ts ? weatherList.last().toMap() : weatherList.at(m_ts).toMap();
    int id = weatherMap["code"].toInt();
    values[tag("weatherId")] = id;
    values[tag("timestamp")] = weatherMap["date"].toString();
    // yahoo provides high and low temperatures. Lets calculate average one
    values[tag("temperature")] = (weatherMap["high"].toDouble() + weatherMap["low"].toDouble()) / 2.0;
    // ... and no forecast data for humidity and pressure
    values[tag("humidity")] = 0;
    values[tag("pressure")] = 0.0;

    return values;
}
