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

#include "abstractquotesprovider.h"


class YahooQuotesProvider : public AbstractQuotesProvider
{
public:
    const char *YAHOO_QUOTES_URL = "https://query.yahooapis.com/v1/public/yql";
    const char *YAHOO_QUOTES_QUERY = "select * from yahoo.finance.quotes where symbol='%1'";

    explicit YahooQuotesProvider();
    ~YahooQuotesProvider() override = default;
    void initUrl(const QString &_asset) override;
    [[nodiscard]] QVariantHash parse(const QByteArray &_source) override;
    [[nodiscard]] QUrl url() const override;

private:
    double m_price = 0.0;
    QUrl m_url;
};
