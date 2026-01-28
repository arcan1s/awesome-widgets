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

#include "systeminfosource.h"

#include <ksysguard/formatter/Unit.h>
#include <ksysguard/systemstats/SensorInfo.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>

#include <cmath>

#include "awdebug.h"


SystemInfoSource::SystemInfoSource(QObject *_parent)
    : AbstractExtSysMonSource(_parent)
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant SystemInfoSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (_source == "brightness") {
        return SystemInfoSource::getCurrentBrightness();
    } else if (_source == "volume") {
        return SystemInfoSource::getCurrentVolume();
    }

    return {};
}


QHash<QString, KSysGuard::SensorInfo *> SystemInfoSource::sources() const
{
    auto result = QHash<QString, KSysGuard::SensorInfo *>();

    result.insert("brightness", makeSensorInfo("Screen brightness", QMetaType::Double, KSysGuard::UnitPercent, 0, 100));
    result.insert("volume", makeSensorInfo("Master volume", QMetaType::Double, KSysGuard::UnitNone));

    return result;
}


QVariant SystemInfoSource::fromDBusVariant(const QVariant &_value)
{
    return _value.value<QDBusVariant>().variant();
}


double SystemInfoSource::getCurrentBrightness()
{
    qCDebug(LOG_ESS) << "Get current brightness";

    auto maxBrightness
        = sendDBusRequest("org.kde.Solid.PowerManagement", "/org/kde/Solid/PowerManagement/Actions/BrightnessControl",
                          "org.kde.Solid.PowerManagement.Actions.BrightnessControl", "brightnessMax")
              .toDouble();
    auto brightness
        = sendDBusRequest("org.kde.Solid.PowerManagement", "/org/kde/Solid/PowerManagement/Actions/BrightnessControl",
                          "org.kde.Solid.PowerManagement.Actions.BrightnessControl", "brightness")
              .toDouble();

    return std::round(100.0 * brightness / maxBrightness);
}


double SystemInfoSource::getCurrentVolume()
{
    qCDebug(LOG_ESS) << "Get current volume";

    // current device first
    auto currentMixer = fromDBusVariant(sendDBusRequest("org.kde.kmix", "/Mixers", "org.freedesktop.DBus.Properties",
                                                        "Get", {"org.kde.KMix.MixSet", "currentMasterMixer"}))
                            .toString();

    if (currentMixer.isEmpty()) {
        qCWarning(LOG_ESS) << "Mixer is empty";
        return std::numeric_limits<double>::quiet_NaN();
    }
    currentMixer.replace(":", "_").replace(".", "_").replace("-", "_");

    // get capture device
    auto currentControl = fromDBusVariant(sendDBusRequest("org.kde.kmix", "/Mixers", "org.freedesktop.DBus.Properties",
                                                          "Get", {"org.kde.KMix.MixSet", "currentMasterControl"}))
                              .toString();
    if (currentControl.isEmpty()) {
        qCWarning(LOG_ESS) << "Control is empty";
        return std::numeric_limits<double>::quiet_NaN();
    }
    currentControl.replace(":", "_").replace(".", "_").replace("-", "_");

    auto path = QString("/Mixers/%1/%2").arg(currentMixer, currentControl);
    return fromDBusVariant(sendDBusRequest("org.kde.kmix", path, "org.freedesktop.DBus.Properties", "Get",
                                           {"org.kde.KMix.Control", "volume"}))
        .toDouble();
}


QVariant SystemInfoSource::sendDBusRequest(const QString &_destination, const QString &_path, const QString &_interface,
                                           const QString &_method, const QVariantList &_args)
{
    qCDebug(LOG_ESS) << "Send dbus request" << _destination << _path << _interface << _method << _args;

    auto bus = QDBusConnection::sessionBus();
    auto request = QDBusMessage::createMethodCall(_destination, _path, _interface, _method);
    if (!_args.isEmpty())
        request.setArguments(_args);

    auto response = bus.call(request, QDBus::BlockWithGui, REQUEST_TIMEOUT);

    if (response.type() != QDBusMessage::ReplyMessage || response.arguments().isEmpty()) {
        qCWarning(LOG_ESS) << "Error" << response.errorName() << "with message" << response.errorMessage();
        return {};
    } else {
        return response.arguments().first();
    }
}
