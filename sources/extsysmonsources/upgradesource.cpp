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


#include "upgradesource.h"

#include <ksysguard/formatter/Unit.h>
#include <ksysguard/systemstats/SensorInfo.h>

#include "awdebug.h"
#include "extupgrade.h"


UpgradeSource::UpgradeSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_extUpgrade = new ExtItemAggregator<ExtUpgrade>(nullptr, "upgrade");
    m_extUpgrade->initSockets();
    m_sources = getSources();
}


UpgradeSource::~UpgradeSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant UpgradeSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    // there are only one value
    return m_extUpgrade->itemByTagNumber(index(_source))->run().values().first();
}


KSysGuard::SensorInfo *UpgradeSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    auto data = new KSysGuard::SensorInfo();
    data->name = QString("Package manager '%1' metadata").arg(m_extUpgrade->itemByTagNumber(index(_source))->uniq());
    data->variantType = QVariant::String;
    data->unit = KSysGuard::UnitNone;

    return data;
}


QStringList UpgradeSource::sources() const
{
    return m_sources;
}


QStringList UpgradeSource::getSources()
{
    QStringList sources;
    for (auto &item : m_extUpgrade->activeItems())
        sources.append(QString("upgrade/%1").arg(item->tag("pkgcount")));

    return sources;
}
