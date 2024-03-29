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


SystemInfoSource::SystemInfoSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant SystemInfoSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (!m_values.contains(_source))
        run();
    return m_values.take(_source);
}


void SystemInfoSource::run()
{
    m_values["brightness"] = SystemInfoSource::getCurrentBrightness();
    m_values["volume"] = SystemInfoSource::getCurrentVolume();
}


QHash<QString, KSysGuard::SensorInfo *> SystemInfoSource::sources() const
{
    auto result = QHash<QString, KSysGuard::SensorInfo *>();

    result.insert("brightness", makeSensorInfo("Screen brightness", QVariant::Double, KSysGuard::UnitPercent, 0, 100));
    result.insert("volume", makeSensorInfo("Master volume", QVariant::Double, KSysGuard::UnitNone));

    return result;
}


QVariant SystemInfoSource::fromDBusVariant(const QVariant &value)
{
    return value.value<QDBusVariant>().variant();
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
    auto currentMixer
        = fromDBusVariant(sendDBusRequest("org.kde.kmix", "/Mixers", "org.freedesktop.DBus.Properties", "Get",
                                          QVariantList({"org.kde.KMix.MixSet", "currentMasterMixer"})))
              .toString();

    if (currentMixer.isEmpty()) {
        qCWarning(LOG_ESS) << "Mixer is empty";
        return std::numeric_limits<double>::quiet_NaN();
    }
    currentMixer.replace(":", "_").replace(".", "_").replace("-", "_");

    // get capture device
    auto currentControl
        = fromDBusVariant(sendDBusRequest("org.kde.kmix", "/Mixers", "org.freedesktop.DBus.Properties", "Get",
                                          QVariantList({"org.kde.KMix.MixSet", "currentMasterControl"})))
              .toString();
    if (currentControl.isEmpty()) {
        qCWarning(LOG_ESS) << "Control is empty";
        return std::numeric_limits<double>::quiet_NaN();
    }
    currentControl.replace(":", "_").replace(".", "_").replace("-", "_");

    auto path = QString("/Mixers/%1/%2").arg(currentMixer, currentControl);
    return fromDBusVariant(sendDBusRequest("org.kde.kmix", path, "org.freedesktop.DBus.Properties", "Get",
                                           QVariantList({"org.kde.KMix.Control", "volume"})))
        .toDouble();
}


QVariant SystemInfoSource::sendDBusRequest(const QString &destination, const QString &path, const QString &interface,
                                           const QString &method, const QVariantList &args)
{
    qCDebug(LOG_ESS) << "Send dbus request" << destination << path << interface << method << args;

    auto bus = QDBusConnection::sessionBus();
    auto request = QDBusMessage::createMethodCall(destination, path, interface, method);
    if (!args.isEmpty())
        request.setArguments(args);

    auto response = bus.call(request, QDBus::BlockWithGui, REQUEST_TIMEOUT);

    if ((response.type() != QDBusMessage::ReplyMessage) || (response.arguments().isEmpty())) {
        qCWarning(LOG_ESS) << "Error message" << response.errorMessage();
        return QVariant();
    } else {
        return response.arguments().first();
    }
}
