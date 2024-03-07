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

#include "extsysmon.h"

#include <KPluginFactory>

#include <QDBusMetaType>
#include <QFile>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"
#include "extsysmonaggregator.h"


ExtendedSysMon::ExtendedSysMon(QObject *_parent, const QVariantList &_args)
    : KSysGuard::SensorPlugin(_parent, _args)
{
    qSetMessagePattern(AWDebug::LOG_FORMAT);
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;
    for (auto &metadata : AWDebug::getBuildData())
        qCDebug(LOG_ESM) << metadata;

    readConfiguration();
    addContainer(new ExtSysMonAggregator("extsysmon", "Extended system monitor", this, m_configuration));
}


void ExtendedSysMon::readConfiguration()
{
    auto fileName = QStandardPaths::locate(QStandardPaths::ConfigLocation, "plasma-dataengine-extsysmon.conf");
    qCInfo(LOG_ESM) << "Configuration file" << fileName;
    QSettings settings(fileName, QSettings::IniFormat);
    QHash<QString, QString> rawConfig;

    settings.beginGroup("Configuration");
    rawConfig["ACPIPATH"] = settings.value("ACPIPATH", "/sys/class/power_supply/").toString();
    rawConfig["MPDADDRESS"] = settings.value("MPDADDRESS", "localhost").toString();
    rawConfig["MPDPORT"] = settings.value("MPDPORT", "6600").toString();
    rawConfig["MPRIS"] = settings.value("MPRIS", "auto").toString();
    rawConfig["PLAYER"] = settings.value("PLAYER", "mpris").toString();
    rawConfig["PLAYERSYMBOLS"] = settings.value("PLAYERSYMBOLS", "10").toString();
    settings.endGroup();

    m_configuration = updateConfiguration(rawConfig);
}


QHash<QString, QString> ExtendedSysMon::updateConfiguration(QHash<QString, QString> _rawConfig)
{
    qCDebug(LOG_ESM) << "Raw configuration" << _rawConfig;

    // player
    if ((_rawConfig["PLAYER"] != "mpd") && (_rawConfig["PLAYER"] != "mpris") && (_rawConfig["PLAYER"] != "disable"))
        _rawConfig["PLAYER"] = "mpris";
    // player symbols
    if (_rawConfig["PLAYERSYMBOLS"].toInt() <= 0)
        _rawConfig["PLAYERSYMBOLS"] = "10";

    for (auto &key : _rawConfig.keys())
        qCInfo(LOG_ESM) << key << "=" << _rawConfig[key];
    return _rawConfig;
}


K_PLUGIN_CLASS_WITH_JSON(ExtendedSysMon, "metadata.json")

#include "extsysmon.moc"
