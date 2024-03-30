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

#include "customsource.h"

#include "awdebug.h"
#include "extscript.h"


CustomSource::CustomSource(QObject *_parent)
    : AbstractExtSysMonSource(_parent)
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_extScripts = new ExtItemAggregator<ExtScript>(this, "scripts");
    m_extScripts->initSockets();
}


QVariant CustomSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    // there are only one value
    return dataByItem(m_extScripts, _source).values().first();
}


QHash<QString, KSysGuard::SensorInfo *> CustomSource::sources() const
{
    auto result = QHash<QString, KSysGuard::SensorInfo *>();

    for (auto &item : m_extScripts->activeItems())
        result.insert(item->tag("custom"),
                      makeSensorInfo(QString("Custom command '%1' output").arg(item->uniq()), QVariant::String));

    return result;
}
