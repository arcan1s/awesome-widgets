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

#include <ksysguard/formatter/Unit.h>
#include <ksysguard/systemstats/SensorInfo.h>

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
    if (!m_values.contains(_source)) {
        QVariantHash data = m_extQuotes->itemByTagNumber(ind)->run();
        for (auto &key : data.keys())
            m_values[key] = data[key];
    }
    QVariant value = m_values.take(_source);
    return value;
}


KSysGuard::SensorInfo *QuotesSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    int ind = index(_source);
    auto data = new KSysGuard::SensorInfo;
    if (_source.startsWith("pricechg")) {
        data->min = 0.0;
        data->max = 0.0;
        data->name = QString("Absolute price changes for '%1'").arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::Double;
        data->unit = KSysGuard::UnitNone;
    } else if (_source.startsWith("price")) {
        data->min = 0.0;
        data->max = 0.0;
        data->name = QString("Price for '%1'").arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::Double;
        data->unit = KSysGuard::UnitNone;
    } else if (_source.startsWith("percpricechg")) {
        data->min = -100.0;
        data->max = 100.0;
        data->name = QString("Price changes for '%1'").arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::Double;
        data->unit = KSysGuard::UnitPercent;
    } else if (_source.startsWith("volumechg")) {
        data->min = 0;
        data->max = 0;
        data->name = QString("Absolute volume changes for '%1'").arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitNone;
    } else if (_source.startsWith("volume")) {
        data->min = 0;
        data->max = 0;
        data->name = QString("Volume for '%1'").arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitNone;
    } else if (_source.startsWith("percvolumechg")) {
        data->min = -100.0;
        data->max = 100.0;
        data->name = QString("Volume changes for '%1'").arg(m_extQuotes->itemByTagNumber(ind)->uniq());
        data->variantType = QVariant::Double;
        data->unit = KSysGuard::UnitPercent;
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
        sources.append(item->tag("price"));
        sources.append(item->tag("pricechg"));
        sources.append(item->tag("percpricechg"));
        sources.append(item->tag("volume"));
        sources.append(item->tag("volumechg"));
        sources.append(item->tag("percvolumechg"));
    }

    return sources;
}
