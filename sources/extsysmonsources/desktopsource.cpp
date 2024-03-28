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

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>

#include "awdebug.h"


DesktopSource::DesktopSource(QObject *_parent)
    : AbstractExtSysMonSource(_parent)
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant DesktopSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    auto current = getDBusProperty({"org.kde.KWin.VirtualDesktopManager", "current"}).toString();
    auto desktops
        = extractDesktops(getDBusProperty({"org.kde.KWin.VirtualDesktopManager", "desktops"}).value<QDBusArgument>());

    if (_source == "name") {
        return desktops[current].second;
    } else if (_source == "number") {
        return desktops[current].first + 1;
    } else if (_source == "count") {
        return desktops.count();
    }

    return {};
}


QHash<QString, KSysGuard::SensorInfo *> DesktopSource::sources() const
{
    auto result = QHash<QString, KSysGuard::SensorInfo *>();

    result.insert("name", makeSensorInfo("Current desktop name", QVariant::String));
    result.insert("number", makeSensorInfo("Current desktop number", QVariant::Int));
    result.insert("count", makeSensorInfo("Desktops count", QVariant::Int));

    return result;
}


QHash<QString, QPair<int, QString>> DesktopSource::extractDesktops(const QDBusArgument &_result)
{
    QHash<QString, QPair<int, QString>> result;

    _result.beginArray();
    while (!_result.atEnd()) {
        _result.beginStructure();

        int index;
        QString uuid, name;
        _result >> index >> uuid >> name;

        _result.endStructure();

        result[uuid] = {index, name};
    }
    _result.endArray();

    return result;
}


QVariant DesktopSource::getDBusProperty(const QVariantList &_args)
{
    qCDebug(LOG_ESS) << "Get VDI property" << _args;

    auto bus = QDBusConnection::sessionBus();
    auto request
        = QDBusMessage::createMethodCall(KWinDBusAdapter, VDIDBusPath, PropertyDBusInterface, PropertyDBusMethod);
    request.setArguments(_args);

    auto response = bus.call(request, QDBus::BlockWithGui, REQUEST_TIMEOUT);

    if ((response.type() != QDBusMessage::ReplyMessage) || (response.arguments().isEmpty())) {
        qCWarning(LOG_ESS) << "Error message" << response.errorMessage();
        return {};
    } else {
        auto value = response.arguments().first();
        return value.value<QDBusVariant>().variant();
    }
}
