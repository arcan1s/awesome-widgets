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

#ifndef YAHOOWEATHERPROVIDER_H
#define YAHOOWEATHERPROVIDER_H

#include "abstractweatherprovider.h"

#define YAHOO_WEATHER_URL "https://query.yahooapis.com/v1/public/yql"
#define YAHOO_WEATHER_QUERY                                                    \
    "select * from weather.forecast where u='c' and woeid in (select woeid "   \
    "from geo.places(1) where text='%1, %2')"


class YahooWeatherProvider : public AbstractWeatherProvider
{
public:
    explicit YahooWeatherProvider(QObject *parent, const int number);
    virtual ~YahooWeatherProvider();
    void initUrl(const QString city, const QString country, const int);
    QVariantHash parse(const QVariantMap &json) const;
    QUrl url() const;

private:
    QVariantHash parseCurrent(const QVariantMap &json,
                              const QVariantMap &atmosphere) const;
    QVariantHash parseForecast(const QVariantMap &json) const;
    int m_ts = 0;
    QUrl m_url;
};


#endif /* YAHOOWEATHERPROVIDER_H */
