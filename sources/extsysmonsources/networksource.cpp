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


NetworkSource::NetworkSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


NetworkSource::~NetworkSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant NetworkSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (_source == "network/current/name") {
        QString device = "lo";
        QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
        qCInfo(LOG_ESS) << "Devices" << rawInterfaceList;
        for (auto &interface : rawInterfaceList) {
            if ((interface.flags().testFlag(QNetworkInterface::IsLoopBack))
                || (interface.flags().testFlag(QNetworkInterface::IsPointToPoint)))
                continue;
            if (interface.addressEntries().isEmpty())
                continue;
            device = interface.name();
            break;
        }
        return device;
    }

    return QVariant();
}


QVariantMap NetworkSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    QVariantMap data;
    if (_source == "network/current/name") {
        data["min"] = "";
        data["max"] = "";
        data["name"] = "Current network device name";
        data["type"] = "QString";
        data["units"] = "";
    }

    return data;
}


QStringList NetworkSource::sources() const
{
    QStringList sources;
    sources.append("network/current/name");

    return sources;
}
