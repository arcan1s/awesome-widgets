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

#include <KX11Extras>

#include "awdebug.h"


DesktopSource::DesktopSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


DesktopSource::~DesktopSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant DesktopSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    int current = KX11Extras::currentDesktop();
    int total = KX11Extras::numberOfDesktops();

    if (_source == "desktop/current/name") {
        return KX11Extras::desktopName(current);
    } else if (_source == "desktop/current/number") {
        return current;
    } else if (_source == "desktop/total/name") {
        QStringList desktops;
        for (int i = 1; i < total + 1; i++)
            desktops.append(KX11Extras::desktopName(i));
        return desktops;
    } else if (_source == "desktop/total/number") {
        return total;
    }

    return QVariant();
}


QVariantMap DesktopSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    QVariantMap data;
    if (_source == "desktop/current/name") {
        data["min"] = "";
        data["max"] = "";
        data["name"] = "Current desktop name";
        data["type"] = "QString";
        data["units"] = "";
    } else if (_source == "desktop/current/number") {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = "Current desktop number";
        data["type"] = "integer";
        data["units"] = "";
    } else if (_source == "desktop/total/name") {
        data["min"] = QStringList();
        data["max"] = QStringList();
        data["name"] = "All desktops by name";
        data["type"] = "QStringList";
        data["units"] = "";
    } else if (_source == "desktop/total/number") {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = "Desktops count";
        data["type"] = "integer";
        data["units"] = "";
    }

    return data;
}


QStringList DesktopSource::sources() const
{
    QStringList sources;
    sources.append("desktop/current/name");
    sources.append("desktop/current/number");
    sources.append("desktop/total/name");
    sources.append("desktop/total/number");

    return sources;
}
