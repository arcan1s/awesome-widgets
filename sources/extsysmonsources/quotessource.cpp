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


#include "quotessource.h"

#include "awdebug.h"
#include "extquotes.h"


QuotesSource::QuotesSource(QObject *parent, const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    extQuotes = new ExtItemAggregator<ExtQuotes>(nullptr, QString("quotes"));
    m_sources = getSources();
}


QuotesSource::~QuotesSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    delete extQuotes;
}


QVariant QuotesSource::data(QString source)
{
    qCDebug(LOG_ESS) << "Source" << source;

    int ind = index(source);
    source.remove(QString("quotes/"));
    if (!m_values.contains(source)) {
        QVariantHash data = extQuotes->itemByTagNumber(ind)->run();
        for (auto key : data.keys())
            m_values[key] = data[key];
    }
    QVariant value = m_values.take(source);
    return value;
}


QVariantMap QuotesSource::initialData(QString source) const
{
    qCDebug(LOG_ESS) << "Source" << source;

    int ind = index(source);
    QVariantMap data;
    if (source.startsWith(QString("quotes/askchg"))) {
        data[QString("min")] = 0.0;
        data[QString("max")] = 0.0;
        data[QString("name")]
            = QString("Absolute ask changes for '%1'")
                  .arg(extQuotes->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("double");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("quotes/ask"))) {
        data[QString("min")] = 0.0;
        data[QString("max")] = 0.0;
        data[QString("name")]
            = QString("Ask for '%1'")
                  .arg(extQuotes->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("double");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("quotes/percaskchg"))) {
        data[QString("min")] = -100.0;
        data[QString("max")] = 100.0;
        data[QString("name")]
            = QString("Ask changes for '%1'")
                  .arg(extQuotes->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("double");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("quotes/bidchg"))) {
        data[QString("min")] = 0.0;
        data[QString("max")] = 0.0;
        data[QString("name")]
            = QString("Absolute bid changes for '%1'")
                  .arg(extQuotes->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("double");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("quotes/bid"))) {
        data[QString("min")] = 0.0;
        data[QString("max")] = 0.0;
        data[QString("name")]
            = QString("Bid for '%1'")
                  .arg(extQuotes->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("double");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("quotes/percbidchg"))) {
        data[QString("min")] = -100.0;
        data[QString("max")] = 100.0;
        data[QString("name")]
            = QString("Bid changes for '%1'")
                  .arg(extQuotes->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("double");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("quotes/pricechg"))) {
        data[QString("min")] = 0.0;
        data[QString("max")] = 0.0;
        data[QString("name")]
            = QString("Absolute prie changes for '%1'")
                  .arg(extQuotes->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("double");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("quotes/price"))) {
        data[QString("min")] = 0.0;
        data[QString("max")] = 0.0;
        data[QString("name")]
            = QString("Price for '%1'")
                  .arg(extQuotes->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("double");
        data[QString("units")] = QString("");
    } else if (source.startsWith(QString("quotes/percpricechg"))) {
        data[QString("min")] = -100.0;
        data[QString("max")] = 100.0;
        data[QString("name")]
            = QString("Price changes for '%1'")
                  .arg(extQuotes->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("double");
        data[QString("units")] = QString("");
    }

    return data;
}


QStringList QuotesSource::sources() const
{
    return m_sources;
}


QStringList QuotesSource::getSources()
{
    QStringList sources;
    for (auto item : extQuotes->activeItems()) {
        sources.append(QString("quotes/%1").arg(item->tag(QString("ask"))));
        sources.append(QString("quotes/%1").arg(item->tag(QString("askchg"))));
        sources.append(
            QString("quotes/%1").arg(item->tag(QString("percaskchg"))));
        sources.append(QString("quotes/%1").arg(item->tag(QString("bid"))));
        sources.append(QString("quotes/%1").arg(item->tag(QString("bidchg"))));
        sources.append(
            QString("quotes/%1").arg(item->tag(QString("percbidchg"))));
        sources.append(QString("quotes/%1").arg(item->tag(QString("price"))));
        sources.append(
            QString("quotes/%1").arg(item->tag(QString("pricechg"))));
        sources.append(
            QString("quotes/%1").arg(item->tag(QString("percpricechg"))));
    }

    return sources;
}
