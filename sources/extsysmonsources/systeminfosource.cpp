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


SystemInfoSource::~SystemInfoSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant SystemInfoSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (!m_values.contains(_source))
        run();
    return m_values.take(_source);
}


QVariantMap SystemInfoSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    QVariantMap data;
    if (_source == "system/brightness") {
        data["min"] = 0.0;
        data["max"] = 100.0;
        data["name"] = "Screen brightness";
        data["type"] = "float";
        data["units"] = "%";
    } else if (_source == "system/volume") {
        data["min"] = 0.0;
        data["max"] = 100.0;
        data["name"] = "Master volume";
        data["type"] = "float";
        data["units"] = "%";
    }

    return data;
}


void SystemInfoSource::run()
{
    m_values["system/brightness"] = SystemInfoSource::getCurrentBrightness();
    m_values["system/volume"] = SystemInfoSource::getCurrentVolume();
}


QStringList SystemInfoSource::sources() const
{
    QStringList sources;
    sources.append("system/brightness");
    sources.append("system/volume");

    return sources;
}


QVariant SystemInfoSource::fromDBusVariant(const QVariant &value)
{
    return value.value<QDBusVariant>().variant();
}


float SystemInfoSource::getCurrentBrightness()
{
    qCDebug(LOG_ESS) << "Get current brightness";

    auto maxBrightness
        = sendDBusRequest("org.kde.Solid.PowerManagement", "/org/kde/Solid/PowerManagement/Actions/BrightnessControl",
                          "org.kde.Solid.PowerManagement.Actions.BrightnessControl", "brightnessMax")
              .toFloat();
    auto brightness
        = sendDBusRequest("org.kde.Solid.PowerManagement", "/org/kde/Solid/PowerManagement/Actions/BrightnessControl",
                          "org.kde.Solid.PowerManagement.Actions.BrightnessControl", "brightness")
              .toFloat();

    return std::round(100.0f * brightness / maxBrightness);
}


float SystemInfoSource::getCurrentVolume()
{
    qCDebug(LOG_ESS) << "Get current volume";

    // current device first
    auto currentMixer
        = fromDBusVariant(sendDBusRequest("org.kde.kmix", "/Mixers", "org.freedesktop.DBus.Properties", "Get",
                                          QVariantList({"org.kde.KMix.MixSet", "currentMasterMixer"})))
              .toString();

    if (currentMixer.isEmpty()) {
        qCWarning(LOG_ESS) << "Mixer is empty";
        return std::numeric_limits<float>::quiet_NaN();
    }
    currentMixer.replace(":", "_").replace(".", "_").replace("-", "_");

    // get capture device
    auto currentControl
        = fromDBusVariant(sendDBusRequest("org.kde.kmix", "/Mixers", "org.freedesktop.DBus.Properties", "Get",
                                          QVariantList({"org.kde.KMix.MixSet", "currentMasterControl"})))
              .toString();
    if (currentControl.isEmpty()) {
        qCWarning(LOG_ESS) << "Control is empty";
        return std::numeric_limits<float>::quiet_NaN();
    }
    currentControl.replace(":", "_").replace(".", "_").replace("-", "_");

    auto path = QString("/Mixers/%1/%2").arg(currentMixer).arg(currentControl);
    return fromDBusVariant(sendDBusRequest("org.kde.kmix", path, "org.freedesktop.DBus.Properties", "Get",
                                           QVariantList({"org.kde.KMix.Control", "volume"})))
        .toFloat();
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
