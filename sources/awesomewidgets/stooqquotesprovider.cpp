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

#include "stooqquotesprovider.h"

#include <QUrlQuery>

#include "awdebug.h"


StooqQuotesProvider::StooqQuotesProvider()
    : AbstractQuotesProvider()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


void StooqQuotesProvider::initUrl(const QString &_asset)
{
    qCDebug(LOG_LIB) << "Init query for" << _asset;

    m_url = QUrl(STOOQ_QUOTES_URL);

    QUrlQuery params;
    params.addQueryItem("s", _asset);
    params.addQueryItem("f", "pv");
    params.addQueryItem("e", "csv");
    m_url.setQuery(params);
}


QVariantHash StooqQuotesProvider::parse(const QByteArray &_source)
{
    qCDebug(LOG_LIB) << "Parse csv" << _source;

    QVariantHash values;

    auto sourceValues = QString::fromUtf8(_source).trimmed().split(',');
    if (sourceValues.count() != 2) {
        qCWarning(LOG_LIB) << "Parse error" << sourceValues;
        return values;
    }

    // last trade
    auto price = sourceValues.at(0).toDouble();
    values["pricechg"] = m_price == 0.0 ? 0.0 : price - m_price;
    values["percpricechg"] = 100.0 * values["pricechg"].toDouble() / m_price;
    values["price"] = price;
    m_price = price;
    // volume
    auto volume = sourceValues.at(1).toInt();
    values["volumechg"] = m_volume == 0 ? 0 : volume - m_volume;
    values["percvolumechg"] = 100.0 * values["volumechg"].toDouble() / m_volume;
    values["volume"] = volume;
    m_volume = volume;

    return values;
}


QUrl StooqQuotesProvider::url() const
{
    return m_url;
}
