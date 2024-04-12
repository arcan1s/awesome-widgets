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


OWMWeatherProvider::OWMWeatherProvider(QObject *_parent)
    : AbstractWeatherProvider(_parent)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


OWMWeatherProvider::~OWMWeatherProvider()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


void OWMWeatherProvider::initUrl(const QString &_city, const QString &_country, const int ts)
{
    qCDebug(LOG_LIB) << "Init query for" << _city << _country << "with ts" << ts;

    m_ts = ts;

    if (m_ts == 0)
        m_url = QUrl(OWM_WEATHER_URL);
    else
        m_url = QUrl(OWM_FORECAST_URL);

    QUrlQuery params;
    params.addQueryItem("q", QString("%1,%2").arg(_city, _country));
    params.addQueryItem("units", "metric");
    m_url.setQuery(params);
}


QVariantHash OWMWeatherProvider::parse(const QVariantMap &_json) const
{
    qCDebug(LOG_LIB) << "Parse json" << _json;

    if (_json["cod"].toInt() != 200) {
        qCWarning(LOG_LIB) << "Invalid OpenWeatherMap return code" << _json["cod"].toInt();
        return {};
    }

    if (m_ts == 0) {
        return parseSingleJson(_json);
    } else {
        QVariantList list = _json["list"].toList();
        return parseSingleJson(list.count() <= m_ts ? list.at(m_ts - 1).toMap() : list.last().toMap());
    }
}


QUrl OWMWeatherProvider::url() const
{
    return m_url;
}


QVariantHash OWMWeatherProvider::parseSingleJson(const QVariantMap &_json) const
{
    qCDebug(LOG_LIB) << "Single json data" << _json;

    QVariantHash output;

    // weather status
    auto weather = _json["weather"].toList();
    if (!weather.isEmpty()) {
        int id = weather.first().toMap()["id"].toInt();
        output[tag("weatherId")] = id;
    }

    // main data
    auto mainWeather = _json["main"].toMap();
    if (!weather.isEmpty()) {
        output[tag("humidity")] = mainWeather["humidity"].toDouble();
        output[tag("pressure")] = mainWeather["pressure"].toDouble();
        output[tag("temperature")] = mainWeather["temp"].toDouble();
    }

    // timestamp
    output[tag("timestamp")] = QDateTime::fromSecsSinceEpoch(_json["dt"].toUInt()).toUTC();

    return output;
}
