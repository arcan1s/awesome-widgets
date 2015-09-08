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

#include "extsysmonaggregator.h"

#include "awdebug.h"


ExtSysMonAggregator::ExtSysMonAggregator(QObject *parent, const QHash<QString, QString> config)
    : QObject(parent)
{
    qCDebug(LOG_ESM);

    init(config);
}


ExtSysMonAggregator::~ExtSysMonAggregator()
{
    qCDebug(LOG_ESM);

    m_map.clear();
}


QVariant ExtSysMonAggregator::data(const QString source) const
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    return m_map.contains(source) ? m_map[source]->data(source) : QVariant();
}


AbstractExtSysMonSource *ExtSysMonAggregator::engine(const QString source)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    return m_map.contains(source) ? m_map[source] : nullptr;
}


QVariantMap ExtSysMonAggregator::initialData(const QString source) const
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    return m_map.contains(source) ? m_map[source]->initialData(source) : QVariantMap();
}


QStringList ExtSysMonAggregator::sources() const
{
    qCDebug(LOG_ESM);

    return m_map.keys();
}


void ExtSysMonAggregator::init(const QHash<QString, QString> config)
{
    qCDebug(LOG_ESM);
}
