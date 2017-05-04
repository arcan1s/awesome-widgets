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


#include "loadsource.h"

#include "awdebug.h"


LoadSource::LoadSource(QObject *parent, const QStringList &args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


LoadSource::~LoadSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant LoadSource::data(const QString &source)
{
    qCDebug(LOG_ESS) << "Source" << source;

    auto data = source;
    data.remove("load/load");
    return data.toInt();
}


QVariantMap LoadSource::initialData(const QString &source) const
{
    qCDebug(LOG_ESS) << "Source" << source;

    QVariantMap data;
    if (source.startsWith(QString("load/load"))) {
        data[QString("min")] = 0;
        data[QString("max")] = 0;
        data[QString("name")] = QString("Simple sources for load tests");
        data[QString("type")] = QString("int");
        data[QString("units")] = QString("");
    }

    return data;
}


QStringList LoadSource::sources() const
{
    QStringList sources;
    for (int i = 0; i < 1000; i++)
        sources.append(QString("load/load%1").arg(i));

    return sources;
}
