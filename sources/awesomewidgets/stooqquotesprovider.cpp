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

#include <QTextCodec>
#include <QUrlQuery>

#include "awdebug.h"


StooqQuotesProvider::StooqQuotesProvider(QObject *_parent)
    : AbstractQuotesProvider(_parent)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


StooqQuotesProvider::~StooqQuotesProvider()
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


QVariantHash StooqQuotesProvider::parse(const QByteArray &_source,
                                        const QVariantHash &_oldValues) const
{
    qCDebug(LOG_LIB) << "Parse csv" << _source;

    QVariantHash values;

    QStringList sourceValues
        = QTextCodec::codecForMib(106)->toUnicode(_source).trimmed().split(',');
    if (sourceValues.count() != 2) {
        qCWarning(LOG_LIB) << "Parse error" << sourceValues;
        return values;
    }

    // extract old data
    auto oldPrice = _oldValues[tag("price")].toDouble();
    auto oldVolume = _oldValues[tag("volume")].toInt();

    // last trade
    auto price = sourceValues.at(0).toDouble();
    values[tag("pricechg")] = oldPrice == 0.0 ? 0.0 : price - oldPrice;
    values[tag("percpricechg")] = 100.0 * values[tag("pricechg")].toDouble() / price;
    values[tag("price")] = price;
    // volume
    auto volume = sourceValues.at(1).toInt();
    values[tag("volumechg")] = oldVolume == 0 ? 0 : volume - oldVolume;
    values[tag("percvolumechg")] = 100.0 * values[tag("volumechg")].toDouble() / volume;
    values[tag("volume")] = volume;

    return values;
}


QUrl StooqQuotesProvider::url() const
{
    return m_url;
}
