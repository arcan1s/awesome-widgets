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

#include "awdebug.h"
#include "extupgrade.h"


UpgradeSource::UpgradeSource(QObject *parent, const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 0);
    qCDebug(LOG_ESM);

    extUpgrade = new ExtItemAggregator<ExtUpgrade>(nullptr, QString("upgrade"));
    m_sources = getSources();
}


UpgradeSource::~UpgradeSource()
{
    qCDebug(LOG_ESM);

    delete extUpgrade;
}


QVariant UpgradeSource::data(QString source)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    // there are only one value
    return extUpgrade->itemByTagNumber(index(source))->run().values().first();
}


QVariantMap UpgradeSource::initialData(QString source) const
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    QVariantMap data;
    data[QString("min")] = QString("");
    data[QString("max")] = QString("");
    data[QString("name")]
        = QString("Package manager '%1' metadata")
              .arg(extUpgrade->itemByTagNumber(index(source))->uniq());
    data[QString("type")] = QString("QString");
    data[QString("units")] = QString("");

    return data;
}


QStringList UpgradeSource::sources() const
{
    qCDebug(LOG_ESM);

    return m_sources;
}


QStringList UpgradeSource::getSources()
{
    qCDebug(LOG_ESM);

    QStringList sources;
    foreach (ExtUpgrade *item, extUpgrade->activeItems())
        sources.append(
            QString("upgrade/%1").arg(item->tag(QString("pkgcount"))));

    return sources;
}
