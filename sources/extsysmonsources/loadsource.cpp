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

#include <QTime>

#include "awdebug.h"


LoadSource::LoadSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    qsrand(static_cast<uint>(QTime::currentTime().msec()));
}


LoadSource::~LoadSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant LoadSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    return qrand();
}


QVariantMap LoadSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    QVariantMap data;
    if (_source.startsWith("load/load")) {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = "Simple sources for load tests";
        data["type"] = "int";
        data["units"] = "";
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
