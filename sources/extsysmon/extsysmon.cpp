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

#include <QFile>
#include <QRegExp>
#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"
#include "extsysmonaggregator.h"
#include "gputempsource.h"
#include "hddtempsource.h"


ExtendedSysMon::ExtendedSysMon(QObject *parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)
    qSetMessagePattern(AWDebug::LOG_FORMAT);
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;
    for (auto &metadata : AWDebug::getBuildData())
        qCDebug(LOG_ESM) << metadata;

    setMinimumPollingInterval(333);
    readConfiguration();

    // init aggregator
    m_aggregator = new ExtSysMonAggregator(this, m_configuration);
    for (auto &source : m_aggregator->sources())
        setData(source, m_aggregator->initialData(source));
}


ExtendedSysMon::~ExtendedSysMon()
{
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    delete m_aggregator;
}


QStringList ExtendedSysMon::sources() const
{
    return m_aggregator->sources();
}


bool ExtendedSysMon::sourceRequestEvent(const QString &source)
{
    qCDebug(LOG_ESM) << "Source" << source;

    return updateSourceEvent(source);
}


bool ExtendedSysMon::updateSourceEvent(const QString &source)
{
    qCDebug(LOG_ESM) << "Source" << source;

    if (m_aggregator->hasSource(source)) {
        QVariant data = m_aggregator->data(source);
        if (data.isNull())
            return false;
        setData(source, QString("value"), data);
    } else {
        qCWarning(LOG_ESM) << "Unknown source" << source;
        return false;
    }

    return true;
}


void ExtendedSysMon::readConfiguration()
{
    QString fileName
        = QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                 QString("plasma-dataengine-extsysmon.conf"));
    qCInfo(LOG_ESM) << "Configuration file" << fileName;
    QSettings settings(fileName, QSettings::IniFormat);
    QHash<QString, QString> rawConfig;

    settings.beginGroup(QString("Configuration"));
    rawConfig[QString("ACPIPATH")]
        = settings
              .value(QString("ACPIPATH"), QString("/sys/class/power_supply/"))
              .toString();
    rawConfig[QString("GPUDEV")]
        = settings.value(QString("GPUDEV"), QString("auto")).toString();
    rawConfig[QString("HDDDEV")]
        = settings.value(QString("HDDDEV"), QString("all")).toString();
    rawConfig[QString("HDDTEMPCMD")]
        = settings.value(QString("HDDTEMPCMD"), QString("sudo smartctl -a"))
              .toString();
    rawConfig[QString("MPDADDRESS")]
        = settings.value(QString("MPDADDRESS"), QString("localhost"))
              .toString();
    rawConfig[QString("MPDPORT")]
        = settings.value(QString("MPDPORT"), QString("6600")).toString();
    rawConfig[QString("MPRIS")]
        = settings.value(QString("MPRIS"), QString("auto")).toString();
    rawConfig[QString("PLAYER")]
        = settings.value(QString("PLAYER"), QString("mpris")).toString();
    rawConfig[QString("PLAYERSYMBOLS")]
        = settings.value(QString("PLAYERSYMBOLS"), QString("10")).toString();
    settings.endGroup();

    m_configuration = updateConfiguration(rawConfig);
}


QHash<QString, QString>
ExtendedSysMon::updateConfiguration(QHash<QString, QString> rawConfig) const
{
    qCDebug(LOG_ESM) << "Raw configuration" << rawConfig;

    // gpudev
    if (rawConfig[QString("GPUDEV")] == QString("disable"))
        rawConfig[QString("GPUDEV")] = QString("disable");
    else if (rawConfig[QString("GPUDEV")] == QString("auto"))
        rawConfig[QString("GPUDEV")] = GPUTemperatureSource::autoGpu();
    else if ((rawConfig[QString("GPUDEV")] != QString("ati"))
             && (rawConfig[QString("GPUDEV")] != QString("nvidia")))
        rawConfig[QString("GPUDEV")] = GPUTemperatureSource::autoGpu();
    // hdddev
    QStringList allHddDevices = HDDTemperatureSource::allHdd();
    if (rawConfig[QString("HDDDEV")] == QString("all")) {
        rawConfig[QString("HDDDEV")] = allHddDevices.join(QChar(','));
    } else if (rawConfig[QString("HDDDEV")] == QString("disable")) {
        rawConfig[QString("HDDDEV")] = QString("");
    } else {
        QStringList deviceList = rawConfig[QString("HDDDEV")].split(
            QChar(','), QString::SkipEmptyParts);
        QStringList devices;
        QRegExp diskRegexp = QRegExp("^/dev/[hms]d[a-z]$");
        for (auto &device : deviceList)
            if ((QFile::exists(device)) && (device.contains(diskRegexp)))
                devices.append(device);
        if (devices.isEmpty())
            rawConfig[QString("HDDDEV")] = allHddDevices.join(QChar(','));
        else
            rawConfig[QString("HDDDEV")] = devices.join(QChar(','));
    }
    // player
    if ((rawConfig[QString("PLAYER")] != QString("mpd"))
        && (rawConfig[QString("PLAYER")] != QString("mpris"))
        && (rawConfig[QString("PLAYER")] != QString("disable")))
        rawConfig[QString("PLAYER")] = QString("mpris");
    // player symbols
    if (rawConfig[QString("PLAYERSYMBOLS")].toInt() <= 0)
        rawConfig[QString("PLAYERSYMBOLS")] = QString("10");

    for (auto &key : rawConfig.keys())
        qCInfo(LOG_ESM) << key << "=" << rawConfig[key];
    return rawConfig;
}


K_EXPORT_PLASMA_DATAENGINE_WITH_JSON(extsysmon, ExtendedSysMon,
                                     "plasma-dataengine-extsysmon.json")

#include "extsysmon.moc"
