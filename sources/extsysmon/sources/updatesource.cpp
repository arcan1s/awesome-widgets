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


#include "updatesource.h"

#include "awdebug.h"


UpdateSource::UpdateSource(QObject *parent, const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 0);
    qCDebug(LOG_ESM);
}


UpdateSource::~UpdateSource()
{
    qCDebug(LOG_ESM);
}


QVariant UpdateSource::data(QString source)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    return true;
}


QVariantMap UpdateSource::initialData(QString source) const
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    QVariantMap data;
    if (source == QString("desktop/current/name")) {
        data[QString("min")] = true;
        data[QString("max")] = true;
        data[QString("name")] = QString("Simple value which is always true");
        data[QString("type")] = QString("bool");
        data[QString("units")] = QString("");
    }

    return data;
}


QStringList UpdateSource::sources() const
{
    qCDebug(LOG_ESM);

    QStringList sources;
    sources.append(QString("update"));

    return sources;
}
