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


QuotesSource::QuotesSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_extQuotes = new ExtItemAggregator<ExtQuotes>(nullptr, "quotes");
    m_extQuotes->initSockets();
    m_sources = getSources();
}


QuotesSource::~QuotesSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant QuotesSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    int ind = index(_source);
    auto service = _source;
    service.remove("quotes/");
    if (!m_values.contains(service)) {
        QVariantHash data = m_extQuotes->itemByTagNumber(ind)->run();
        for (auto &key : data.keys())
            m_values[key] = data[key];
    }
    QVariant value = m_values.take(service);
    return value;
}


QVariantMap QuotesSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    int ind = index(_source);
    QVariantMap data;
    if (_source.startsWith("quotes/pricechg")) {
        data["min"] = 0.0;
        data["max"] = 0.0;
        data["name"] = QString("Absolute prie changes for '%1'")
                           .arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data["type"] = "double";
        data["units"] = "";
    } else if (_source.startsWith("quotes/price")) {
        data["min"] = 0.0;
        data["max"] = 0.0;
        data["name"] = QString("Price for '%1'").arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data["type"] = "double";
        data["units"] = "";
    } else if (_source.startsWith("quotes/percpricechg")) {
        data["min"] = -100.0;
        data["max"] = 100.0;
        data["name"]
            = QString("Price changes for '%1'").arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data["type"] = "double";
        data["units"] = "";
    } else if (_source.startsWith("quotes/volumechg")) {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = QString("Absolute volume changes for '%1'")
                           .arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data["type"] = "int";
        data["units"] = "";
    } else if (_source.startsWith("quotes/volume")) {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = QString("Volume for '%1'").arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data["type"] = "int";
        data["units"] = "";
    } else if (_source.startsWith("quotes/percvolumechg")) {
        data["min"] = -100.0;
        data["max"] = 100.0;
        data["name"]
            = QString("Volume changes for '%1'").arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data["type"] = "double";
        data["units"] = "";
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
    for (auto &item : m_extQuotes->activeItems()) {
        sources.append(QString("quotes/%1").arg(item->tag("price")));
        sources.append(QString("quotes/%1").arg(item->tag("pricechg")));
        sources.append(QString("quotes/%1").arg(item->tag("percpricechg")));
        sources.append(QString("quotes/%1").arg(item->tag("volume")));
        sources.append(QString("quotes/%1").arg(item->tag("volumechg")));
        sources.append(QString("quotes/%1").arg(item->tag("percvolumechg")));
    }

    return sources;
}
