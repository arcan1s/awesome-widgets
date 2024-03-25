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

#include "desktopsource.h"

#include <KWindowSystem/KWindowSystem>
#include <ksysguard/formatter/Unit.h>
#include <ksysguard/systemstats/SensorInfo.h>
#include <taskmanager/virtualdesktopinfo.h>

#include "awdebug.h"


DesktopSource::DesktopSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_vdi = new TaskManager::VirtualDesktopInfo(this);
}


DesktopSource::~DesktopSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_vdi->deleteLater();
}


QVariant DesktopSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    auto nativeIndex = m_vdi->position(m_vdi->currentDesktop());
    auto decrement = KWindowSystem::isPlatformX11() ? 1 : 0;
    auto current = nativeIndex - decrement;

    if (_source == "name") {
        return m_vdi->desktopNames().at(current);
    } else if (_source == "number") {
        return current + 1;
    } else if (_source == "names") {
        return m_vdi->desktopNames();
    } else if (_source == "count") {
        return m_vdi->numberOfDesktops();
    }

    return {};
}


KSysGuard::SensorInfo *DesktopSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    auto data = new KSysGuard::SensorInfo();
    if (_source == "name") {
        data->name = "Current desktop name";
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "number") {
        data->min = 0;
        data->name = "Current desktop number";
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "names") {
        data->name = "All desktops by name";
        data->variantType = QVariant::StringList;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "count") {
        data->min = 0;
        data->name = "Desktops count";
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitNone;
    }

    return data;
}


QStringList DesktopSource::sources() const
{
    QStringList sources;
    sources.append("name");
    sources.append("number");
    sources.append("names");
    sources.append("count");

    return sources;
}
