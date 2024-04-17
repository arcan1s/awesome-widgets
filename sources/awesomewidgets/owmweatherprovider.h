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


class OWMWeatherProvider : public AbstractWeatherProvider
{
public:
    // we are using own server to pass requests to OpenWeatherMap because it
    // requires specific APPID which belongs to developer not user
    const char *OWM_WEATHER_URL = "https://arcanis.me/weather";
    const char *OWM_FORECAST_URL = "https://arcanis.me/forecast";

    explicit OWMWeatherProvider();
    ~OWMWeatherProvider() override = default;
    void initUrl(const QString &_city, const QString &_country, int) override;
    [[nodiscard]] QVariantHash parse(const QVariantMap &_json) const override;
    [[nodiscard]] QUrl url() const override;

private:
    [[nodiscard]] static QVariantHash parseSingleJson(const QVariantMap &_json);
    int m_ts = 0;
    QUrl m_url;
};
