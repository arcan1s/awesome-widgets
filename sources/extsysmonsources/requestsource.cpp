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


#include "requestsource.h"

#include "awdebug.h"
#include "extnetworkrequest.h"


RequestSource::RequestSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_extNetRequest
        = new ExtItemAggregator<ExtNetworkRequest>(nullptr, "requests");
    m_extNetRequest->initSockets();
    m_sources = getSources();
}


RequestSource::~RequestSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant RequestSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    int ind = index(_source);
    auto service = _source;
    service.remove("network/");
    if (!m_values.contains(service)) {
        QVariantHash data = m_extNetRequest->itemByTagNumber(ind)->run();
        for (auto &key : data.keys())
            m_values[key] = data[key];
    }
    QVariant value = m_values.take(service);
    return value;
}


QVariantMap RequestSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    int ind = index(_source);
    QVariantMap data;
    if (_source.startsWith("network/response")) {
        data["min"] = "";
        data["max"] = "";
        data["name"] = QString("Network response for %1")
                           .arg(m_extNetRequest->itemByTagNumber(ind)->uniq());
        data["type"] = "QString";
        data["units"] = "";
    }

    return data;
}


QStringList RequestSource::sources() const
{
    return m_sources;
}


QStringList RequestSource::getSources()
{
    QStringList sources;
    for (auto &item : m_extNetRequest->activeItems())
        sources.append(QString("network/%1").arg(item->tag("response")));

    return sources;
}
