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


RequestSource::RequestSource(QObject *parent, const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_extNetRequest = new ExtItemAggregator<ExtNetworkRequest>(
        nullptr, QString("requests"));
    m_extNetRequest->initSockets();
    m_sources = getSources();
}


RequestSource::~RequestSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    delete m_extNetRequest;
}


QVariant RequestSource::data(QString source)
{
    qCDebug(LOG_ESS) << "Source" << source;

    int ind = index(source);
    source.remove(QString("network/"));
    if (!m_values.contains(source)) {
        QVariantHash data = m_extNetRequest->itemByTagNumber(ind)->run();
        for (auto key : data.keys())
            m_values[key] = data[key];
    }
    QVariant value = m_values.take(source);
    return value;
}


QVariantMap RequestSource::initialData(QString source) const
{
    qCDebug(LOG_ESS) << "Source" << source;

    int ind = index(source);
    QVariantMap data;
    if (source.startsWith(QString("network/response"))) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")]
            = QString("Network response for %1")
                  .arg(m_extNetRequest->itemByTagNumber(ind)->uniq());
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
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
    for (auto item : m_extNetRequest->activeItems())
        sources.append(
            QString("network/%1").arg(item->tag(QString("response"))));

    return sources;
}
