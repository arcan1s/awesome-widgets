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
#include <QFile>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"
#include "extsysmonaggregator.h"
#include "gpuloadsource.h"
#include "hddtempsource.h"


ExtendedSysMon::ExtendedSysMon(QObject *_parent, const QVariantList &_args)
    : KSysGuard::SensorPlugin(_parent, _args)
{
    qSetMessagePattern(AWDebug::LOG_FORMAT);
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;
    for (auto &metadata : AWDebug::getBuildData())
        qCDebug(LOG_ESM) << metadata;

    readConfiguration();

    addContainer(new ExtSysMonAggregator("extsysmon", "extsysmon", this, m_configuration));
}


ExtendedSysMon::~ExtendedSysMon()
{
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;
}


void ExtendedSysMon::update()
{
    //    m_aggregator->update();
}


void ExtendedSysMon::readConfiguration()
{
    QString fileName = QStandardPaths::locate(QStandardPaths::ConfigLocation, "plasma-dataengine-extsysmon.conf");
    qCInfo(LOG_ESM) << "Configuration file" << fileName;
    QSettings settings(fileName, QSettings::IniFormat);
    QHash<QString, QString> rawConfig;

    settings.beginGroup("Configuration");
    rawConfig["ACPIPATH"] = settings.value("ACPIPATH", "/sys/class/power_supply/").toString();
    rawConfig["GPUDEV"] = settings.value("GPUDEV", "auto").toString();
    rawConfig["HDDDEV"] = settings.value("HDDDEV", "all").toString();
    rawConfig["HDDTEMPCMD"] = settings.value("HDDTEMPCMD", "sudo smartctl -a").toString();
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

    // gpudev
    if (_rawConfig["GPUDEV"] == "disable")
        ;
    else if (_rawConfig["GPUDEV"] == "auto")
        _rawConfig["GPUDEV"] = GPULoadSource::autoGpu();
    else if ((_rawConfig["GPUDEV"] != "ati") && (_rawConfig["GPUDEV"] != "nvidia"))
        _rawConfig["GPUDEV"] = GPULoadSource::autoGpu();
    // hdddev
    QStringList allHddDevices = HDDTemperatureSource::allHdd();
    if (_rawConfig["HDDDEV"] == "all") {
        _rawConfig["HDDDEV"] = allHddDevices.join(',');
    } else if (_rawConfig["HDDDEV"] == "disable") {
        _rawConfig["HDDDEV"] = "";
    } else {
        QStringList deviceList = _rawConfig["HDDDEV"].split(',', Qt::SkipEmptyParts);
        QStringList devices;
        auto diskRegexp = QRegularExpression("^/dev/[hms]d[a-z]$");
        for (auto &device : deviceList)
            if ((QFile::exists(device)) && (device.contains(diskRegexp)))
                devices.append(device);
        if (devices.isEmpty())
            _rawConfig["HDDDEV"] = allHddDevices.join(',');
        else
            _rawConfig["HDDDEV"] = devices.join(',');
    }
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
