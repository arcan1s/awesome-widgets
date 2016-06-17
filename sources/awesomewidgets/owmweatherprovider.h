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

#ifndef OWMWEATHERPROVIDER_H
#define OWMWEATHERPROVIDER_H

#include "abstractweatherprovider.h"

#define OWM_WEATHER_URL "http://arcanis.me/weather"
#define OWM_FORECAST_URL "http://arcanis.me/forecast"


class OWMWeatherProvider : public AbstractWeatherProvider
{
public:
    explicit OWMWeatherProvider(QObject *parent, const int number);
    virtual ~OWMWeatherProvider();
    void initUrl(const QString city, const QString country, const int);
    QVariantHash parse(const QVariantMap &json) const;
    QUrl url() const;

private:
    QVariantHash parseSingleJson(const QVariantMap &json) const;
    int m_ts = 0;
    QUrl m_url;
};


#endif /* OWMWEATHERPROVIDER_H */
