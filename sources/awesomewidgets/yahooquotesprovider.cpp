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

#include "yahooquotesprovider.h"

#include <QJsonDocument>
#include <QUrlQuery>

#include "awdebug.h"


YahooQuotesProvider::YahooQuotesProvider()
    : AbstractQuotesProvider()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


void YahooQuotesProvider::initUrl(const QString &_asset)
{
    qCDebug(LOG_LIB) << "Init query for" << _asset;

    m_url = QUrl(YAHOO_QUOTES_URL);

    QUrlQuery params;
    params.addQueryItem("format", "json");
    params.addQueryItem("env", "store://datatables.org/alltableswithkeys");
    params.addQueryItem("q", QString(YAHOO_QUOTES_QUERY).arg(_asset));
    m_url.setQuery(params);
}


QVariantHash YahooQuotesProvider::parse(const QByteArray &_source)
{
    qCDebug(LOG_LIB) << "Parse json" << _source;

    QVariantHash values;

    QJsonParseError error{};
    auto jsonDoc = QJsonDocument::fromJson(_source, &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_LIB) << "Parse error" << error.errorString();
        return values;
    }
    auto jsonQuotes = jsonDoc.toVariant().toMap()["query"].toMap();
    jsonQuotes = jsonQuotes["results"].toMap()["quote"].toMap();

    // last trade
    auto price = jsonQuotes["LastTradePriceOnly"].toString().toDouble();
    values["pricechg"] = m_price == 0.0 ? 0.0 : price - m_price;
    values["percpricechg"] = 100.0 * values["pricechg"].toDouble() / price;
    values["price"] = price;
    m_price = price;
    // volume
    values["volume"] = 0;
    values["volumechg"] = 0;
    values["percvolumechg"] = 0.0;

    return values;
}


QUrl YahooQuotesProvider::url() const
{
    return m_url;
}
