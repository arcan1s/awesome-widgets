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


QuotesSource::QuotesSource(QObject *_parent)
    : AbstractExtSysMonSource(_parent)
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_extQuotes = new ExtItemAggregator<ExtQuotes>(this, "quotes");
    m_extQuotes->initSockets();
}


QVariant QuotesSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (!m_values.contains(_source))
        m_values = dataByItem(m_extQuotes, _source);

    return m_values.take(_source);
}


QHash<QString, KSysGuard::SensorInfo *> QuotesSource::sources() const
{
    auto result = QHash<QString, KSysGuard::SensorInfo *>();

    for (auto item : m_extQuotes->activeItems()) {
        result.insert(item->tag("pricechg"),
                      makeSensorInfo(QString("Absolute price changes for '%1'").arg(item->uniq()), QVariant::Double));
        result.insert(item->tag("price"),
                      makeSensorInfo(QString("Price for '%1'").arg(item->uniq()), QVariant::Double));
        result.insert(item->tag("percpricechg"), makeSensorInfo(QString("Price changes for '%1'").arg(item->uniq()),
                                                                QVariant::Double, KSysGuard::UnitPercent, 0, 100));
        result.insert(item->tag("volumechg"),
                      makeSensorInfo(QString("Absolute volume changes for '%1'").arg(item->uniq()), QVariant::Double));
        result.insert(item->tag("volume"),
                      makeSensorInfo(QString("Volume for '%1'").arg(item->uniq()), QVariant::Double));
        result.insert(item->tag("percvolumechg"), makeSensorInfo(QString("Volume changes for '%1'").arg(item->uniq()),
                                                                 QVariant::Double, KSysGuard::UnitPercent, 0, 100));
    }

    return result;
}
