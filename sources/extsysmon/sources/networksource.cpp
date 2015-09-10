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


#include "networksource.h"

#include <QNetworkInterface>

#include "awdebug.h"


NetworkSource::NetworkSource(QObject *parent, const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 0);
    qCDebug(LOG_ESM);
}


NetworkSource::~NetworkSource()
{
    qCDebug(LOG_ESM);
}


QVariant NetworkSource::data(QString source)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    if (source == QString("network/current/name")) {
        QString device = QString("lo");
        QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
        qCInfo(LOG_ESM) << "Devices" << rawInterfaceList;
        foreach(QNetworkInterface interface, rawInterfaceList) {
            if ((interface.flags().testFlag(QNetworkInterface::IsLoopBack)) ||
                (interface.flags().testFlag(QNetworkInterface::IsPointToPoint)))
                continue;
            if (interface.addressEntries().isEmpty()) continue;
            device = interface.name();
            break;
        }
        return device;
    }

    return QVariant();
}


QVariantMap NetworkSource::initialData(QString source) const
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    QVariantMap data;
    if (source == QString("network/current/name")) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")] = QString("Current network device name");
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    }

    return data;
}


QStringList NetworkSource::sources() const
{
    qCDebug(LOG_ESM);

    QStringList sources;
    sources.append(QString("network/current/name"));

    return sources;
}
