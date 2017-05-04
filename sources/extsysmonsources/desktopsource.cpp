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

#include <KWindowSystem>

#include "awdebug.h"


DesktopSource::DesktopSource(QObject *parent, const QStringList &args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


DesktopSource::~DesktopSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant DesktopSource::data(const QString &source)
{
    qCDebug(LOG_ESS) << "Source" << source;

    int current = KWindowSystem::currentDesktop();
    int total = KWindowSystem::numberOfDesktops();

    if (source == QString("desktop/current/name")) {
        return KWindowSystem::desktopName(current);
    } else if (source == QString("desktop/current/number")) {
        return current;
    } else if (source == QString("desktop/total/name")) {
        QStringList desktops;
        for (int i = 1; i < total + 1; i++)
            desktops.append(KWindowSystem::desktopName(i));
        return desktops;
    } else if (source == QString("desktop/total/number")) {
        return total;
    }

    return QVariant();
}


QVariantMap DesktopSource::initialData(const QString &source) const
{
    qCDebug(LOG_ESS) << "Source" << source;

    QVariantMap data;
    if (source == QString("desktop/current/name")) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")] = QString("Current desktop name");
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    } else if (source == QString("desktop/current/number")) {
        data[QString("min")] = 0;
        data[QString("max")] = 0;
        data[QString("name")] = QString("Current desktop number");
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("");
    } else if (source == QString("desktop/total/name")) {
        data[QString("min")] = QStringList();
        data[QString("max")] = QStringList();
        data[QString("name")] = QString("All desktops by name");
        data[QString("type")] = QString("QStringList");
        data[QString("units")] = QString("");
    } else if (source == QString("desktop/total/number")) {
        data[QString("min")] = 0;
        data[QString("max")] = 0;
        data[QString("name")] = QString("Desktops count");
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("");
    }

    return data;
}


QStringList DesktopSource::sources() const
{
    QStringList sources;
    sources.append(QString("desktop/current/name"));
    sources.append(QString("desktop/current/number"));
    sources.append(QString("desktop/total/name"));
    sources.append(QString("desktop/total/number"));

    return sources;
}
