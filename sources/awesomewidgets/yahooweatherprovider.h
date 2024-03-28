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

#pragma once

#include "abstractweatherprovider.h"


class YahooWeatherProvider : public AbstractWeatherProvider
{
    Q_OBJECT

public:
    const char *YAHOO_WEATHER_URL = "https://query.yahooapis.com/v1/public/yql";
    const char *YAHOO_WEATHER_QUERY = "select * from weather.forecast where "
                                      "u='c' and woeid in (select woeid from "
                                      "geo.places(1) where text='%1, %2')";

    explicit YahooWeatherProvider(QObject *_parent);
    ~YahooWeatherProvider() override;
    void initUrl(const QString &_city, const QString &_country, int) override;
    [[nodiscard]] QVariantHash parse(const QVariantMap &_json) const override;
    [[nodiscard]] QUrl url() const override;

private:
    [[nodiscard]] QVariantHash parseCurrent(const QVariantMap &_json, const QVariantMap &_atmosphere) const;
    [[nodiscard]] QVariantHash parseForecast(const QVariantMap &_json) const;
    int m_ts = 0;
    QUrl m_url;
};
