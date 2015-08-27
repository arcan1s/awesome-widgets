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

#include <KWindowSystem>
#include <Plasma/DataContainer>

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QNetworkInterface>
#include <QProcessEnvironment>
#include <QRegExp>
#include <QTextCodec>
#include <QSettings>
#include <QStandardPaths>

#include <pdebug/pdebug.h>
#include <pdebug/pdebug-time.h>
#include <task/taskadds.h>

#include "extquotes.h"
#include "extscript.h"
#include "extupgrade.h"
#include "extweather.h"
#include "version.h"


ExtendedSysMon::ExtendedSysMon(QObject* parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)
    qInstallMessageHandler(debugString);

    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));

    setMinimumPollingInterval(333);
    readConfiguration();

    extQuotes = new ExtItemAggregator<ExtQuotes>(nullptr, QString("quotes"), debug);
    extScripts = new ExtItemAggregator<ExtScript>(nullptr, QString("scripts"), debug);
    extUpgrade = new ExtItemAggregator<ExtUpgrade>(nullptr, QString("upgrade"), debug);
    extWeather = new ExtItemAggregator<ExtWeather>(nullptr, QString("weather"), debug);
}


ExtendedSysMon::~ExtendedSysMon()
{
    if (debug) qDebug() << PDEBUG;

    delete extQuotes;
    delete extScripts;
    delete extUpgrade;
    delete extWeather;
}


QStringList ExtendedSysMon::getAllHdd() const
{
    if (debug) qDebug() << PDEBUG;

    QStringList allDevices = QDir(QString("/dev")).entryList(QDir::System, QDir::Name);
    QStringList devices = allDevices.filter(QRegExp(QString("^[hms]d[a-z]$")));
    for (int i=0; i<devices.count(); i++)
        devices[i] = QString("/dev/%1").arg(devices.at(i));

    if (debug) qDebug() << PDEBUG << ":" << "Device list" << devices;
    return devices;
}


QString ExtendedSysMon::getAutoGpu() const
{
    if (debug) qDebug() << PDEBUG;

    QString gpu = QString("disable");
    QFile moduleFile(QString("/proc/modules"));
    if (!moduleFile.open(QIODevice::ReadOnly)) return gpu;

    QString output = moduleFile.readAll();
    if (output.contains(QString("fglrx")))
        gpu = QString("ati");
    else if (output.contains(QString("nvidia")))
        gpu = QString("nvidia");

    if (debug) qDebug() << PDEBUG << ":" << "Device" << gpu;
    return gpu;
}


QString ExtendedSysMon::getAutoMpris() const
{
    if (debug) qDebug() << PDEBUG;

    QDBusMessage listServices = QDBusConnection::sessionBus().interface()->call(QDBus::BlockWithGui, QString("ListNames"));
    if (listServices.arguments().isEmpty()) return QString();
    QStringList arguments = listServices.arguments().first().toStringList();

    foreach(QString arg, arguments) {
        if (debug) qDebug() << PDEBUG << ":" << "Service found" << arg;
        if (!arg.startsWith(QString("org.mpris.MediaPlayer2."))) continue;
        QString service = arg;
        service.remove(QString("org.mpris.MediaPlayer2."));
        return service;
    }

    return QString();
}


QStringList ExtendedSysMon::sources() const
{
    if (debug) qDebug() << PDEBUG;

    QStringList source;
    source.append(QString("battery"));
    source.append(QString("custom"));
    source.append(QString("desktop"));
    source.append(QString("netdev"));
    source.append(QString("gpu"));
    source.append(QString("gputemp"));
    source.append(QString("hddtemp"));
    source.append(QString("pkg"));
    source.append(QString("player"));
    source.append(QString("ps"));
    source.append(QString("quotes"));
    source.append(QString("update"));
    source.append(QString("weather"));

    if (debug) qDebug() << PDEBUG << ":" << "Sources" << source;
    return source;
}


void ExtendedSysMon::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QString fileName = QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                              QString("plasma-dataengine-extsysmon.conf"));
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QSettings settings(fileName, QSettings::IniFormat);
    QHash<QString, QString> rawConfig;

    settings.beginGroup(QString("Configuration"));
    rawConfig[QString("ACPIPATH")] = settings.value(QString("ACPIPATH"), QString("/sys/class/power_supply/")).toString();
    rawConfig[QString("GPUDEV")] = settings.value(QString("GPUDEV"), QString("auto")).toString();
    rawConfig[QString("HDDDEV")] = settings.value(QString("HDDDEV"), QString("all")).toString();
    rawConfig[QString("HDDTEMPCMD")] = settings.value(QString("HDDTEMPCMD"), QString("sudo smartctl -a")).toString();
    rawConfig[QString("MPDADDRESS")] = settings.value(QString("MPDADDRESS"), QString("localhost")).toString();
    rawConfig[QString("MPDPORT")] = settings.value(QString("MPDPORT"), QString("6600")).toString();
    rawConfig[QString("MPRIS")] = settings.value(QString("MPRIS"), QString("auto")).toString();
    rawConfig[QString("PLAYER")] = settings.value(QString("PLAYER"), QString("mpris")).toString();
    settings.endGroup();

    configuration = updateConfiguration(rawConfig);
}


QHash<QString, QString> ExtendedSysMon::updateConfiguration(QHash<QString, QString> rawConfig) const
{
    if (debug) qDebug() << PDEBUG;

    // gpudev
    if (rawConfig[QString("GPUDEV")] == QString("disable"))
        rawConfig[QString("GPUDEV")] = QString("disable");
    else if (rawConfig[QString("GPUDEV")] == QString("auto"))
        rawConfig[QString("GPUDEV")] = getAutoGpu();
    else if ((rawConfig[QString("GPUDEV")] != QString("ati")) &&
        (rawConfig[QString("GPUDEV")] != QString("nvidia")))
        rawConfig[QString("GPUDEV")] = getAutoGpu();
    // hdddev
    QStringList allHddDevices = getAllHdd();
    if (rawConfig[QString("HDDDEV")] == QString("all"))
        rawConfig[QString("HDDDEV")] = allHddDevices.join(QChar(','));
    else if (rawConfig[QString("HDDDEV")] == QString("disable"))
        rawConfig[QString("HDDDEV")] = QString("");
    else {
        QStringList deviceList = rawConfig[QString("HDDDEV")].split(QChar(','), QString::SkipEmptyParts);
        QStringList devices;
        QRegExp diskRegexp = QRegExp("^/dev/[hms]d[a-z]$");
        foreach(QString device, deviceList)
            if ((QFile::exists(device)) && (diskRegexp.indexIn(device) > -1))
                devices.append(device);
        if (devices.isEmpty())
            rawConfig[QString("HDDDEV")] = allHddDevices.join(QChar(','));
        else
            rawConfig[QString("HDDDEV")] = devices.join(QChar(','));
    }
    // player
    if ((rawConfig[QString("PLAYER")] != QString("mpd")) &&
        (rawConfig[QString("PLAYER")] != QString("mpris")) &&
        (rawConfig[QString("PLAYER")] != QString("disable")))
        rawConfig[QString("PLAYER")] = QString("mpris");

    foreach(QString key, rawConfig.keys())
        if (debug) qDebug() << PDEBUG << ":" << key << "=" << rawConfig[key];
    return rawConfig;
}


QVariantHash ExtendedSysMon::getBattery(const QString acpiPath) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "ACPI path" << acpiPath;

    QVariantHash battery;
    battery[QString("ac")] = false;
    battery[QString("bat")] = 0;

    // adaptor
    QFile acFile(QString("%1/AC/online").arg(acpiPath));
    if (acFile.open(QIODevice::ReadOnly)) {
        if (QString(acFile.readLine()).trimmed().toInt() == 1)
            battery[QString("ac")] = true;
    }
    acFile.close();

    // batterites
    float currentLevel = 0.0;
    float fullLevel = 0.0;
    QStringList batDevices = QDir(acpiPath).entryList(QStringList() << QString("BAT*"),
                                                      QDir::Dirs | QDir::NoDotAndDotDot,
                                                      QDir::Name);
    for (int i=0; i<batDevices.count(); i++) {
        QFile currentLevelFile(QString("%1/%2/energy_now").arg(acpiPath).arg(batDevices.at(i)));
        QFile fullLevelFile(QString("%1/%2/energy_full").arg(acpiPath).arg(batDevices.at(i)));
        if ((currentLevelFile.open(QIODevice::ReadOnly)) &&
            (fullLevelFile.open(QIODevice::ReadOnly))) {
            float batCurrent = QString(currentLevelFile.readLine()).trimmed().toFloat();
            float batFull = QString(fullLevelFile.readLine()).trimmed().toFloat();
            battery[QString("bat%1").arg(i)] = static_cast<int>(100 * batCurrent / batFull);
            currentLevel += batCurrent;
            fullLevel += batFull;
        }
        currentLevelFile.close();
        fullLevelFile.close();
    }
    battery[QString("bat")] = static_cast<int>(100 * currentLevel / fullLevel);

    return battery;
}


QVariantHash ExtendedSysMon::getCurrentDesktop() const
{
    if (debug) qDebug() << PDEBUG;

    int number = KWindowSystem::currentDesktop();
    int total = KWindowSystem::numberOfDesktops();
    QVariantHash currentDesktop;
    currentDesktop[QString("currentName")] = KWindowSystem::desktopName(number);
    currentDesktop[QString("currentNumber")] = number;
    currentDesktop[QString("list")] = QStringList();
    for (int i=1; i<total+1; i++)
        currentDesktop[QString("list")].toStringList().append(KWindowSystem::desktopName(i));
    currentDesktop[QString("number")] = total;

    return currentDesktop;
}


float ExtendedSysMon::getGpu(const QString device) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Device" << device;

    float value = 0.0;
    if ((device != QString("nvidia")) && (device != QString("ati")))
        return value;
    QString cmd = QString("");
    if (device == QString("nvidia"))
        cmd = QString("nvidia-smi -q -x");
    else if (device == QString("ati"))
        cmd = QString("aticonfig --od-getclocks");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    if (configuration[QString("GPUDEV")] == QString("nvidia"))
        foreach(QString str, qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
            if (!str.contains(QString("<gpu_util>"))) continue;
            QString load = str.remove(QString("<gpu_util>")).remove(QString("</gpu_util>"))
                              .remove(QChar('%'));
            value = load.toFloat();
            break;
        }
    else if (configuration[QString("GPUDEV")] == QString("ati"))
        foreach(QString str, qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
            if (!str.contains(QString("load"))) continue;
            QString load = str.split(QChar(' '), QString::SkipEmptyParts)[3]
                              .remove(QChar('%'));
            value = load.toFloat();
            break;
        }

    return value;
}


float ExtendedSysMon::getGpuTemp(const QString device) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Device" << device;

    float value = 0.0;
    if ((device != QString("nvidia")) && (device != QString("ati")))
        return value;
    QString cmd = QString("");
    if (device == QString("nvidia"))
        cmd = QString("nvidia-smi -q -x");
    else if (device == QString("ati"))
        cmd = QString("aticonfig --od-gettemperature");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    if (configuration[QString("GPUDEV")] == QString("nvidia"))
        foreach(QString str, qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
            if (!str.contains(QString("<gpu_temp>"))) continue;
            QString temp = str.remove(QString("<gpu_temp>")).remove(QString("C</gpu_temp>"));
            value = temp.toFloat();
            break;
        }
    else if (configuration[QString("GPUDEV")] == QString("ati"))
        foreach(QString str, qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
            if (!str.contains(QString("Temperature"))) continue;
            QString temp = str.split(QChar(' '), QString::SkipEmptyParts).at(4);
            value = temp.toFloat();
            break;
        }

    return value;
}


float ExtendedSysMon::getHddTemp(const QString cmd, const QString device) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    if (debug) qDebug() << PDEBUG << ":" << "Device" << device;

    float value = 0.0;
    TaskResult process = runTask(QString("%1 %2").arg(cmd).arg(device));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    bool smartctl = cmd.contains(QString("smartctl"));
    if (debug) qDebug() << PDEBUG << ":" << "Define smartctl" << smartctl;

    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    if (smartctl) {
        foreach(QString str, qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
            if (!str.startsWith(QString("194"))) continue;
            if (str.split(QChar(' '), QString::SkipEmptyParts).count() < 9) break;
            value = str.split(QChar(' '), QString::SkipEmptyParts).at(9).toFloat();
            break;
        }
    } else {
        if (qoutput.split(QChar(':'), QString::SkipEmptyParts).count() >= 3) {
            QString temp = qoutput.split(QChar(':'), QString::SkipEmptyParts).at(2);
            temp.remove(QChar(0260)).remove(QChar('C'));
            value = temp.toFloat();
        }
    }

    return value;
}


QString ExtendedSysMon::getNetworkDevice() const
{
    if (debug) qDebug() << PDEBUG;

    QString device = QString("lo");
    QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface interface, rawInterfaceList)
        if ((interface.flags().testFlag(QNetworkInterface::IsUp)) &&
            (!interface.flags().testFlag(QNetworkInterface::IsLoopBack)) &&
            (!interface.flags().testFlag(QNetworkInterface::IsPointToPoint))) {
            device = interface.name();
            break;
        }

    return device;
}


QVariantHash ExtendedSysMon::getPlayerInfo(const QString playerName, const QString mpdAddress,
                                          const QString mpdPort, QString mpris) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "player" << playerName;
    if (debug) qDebug() << PDEBUG << ":" << "MPD" << QString("%1:%2").arg(mpdAddress).arg(mpdPort);
    if (debug) qDebug() << PDEBUG << ":" << "MPRIS" << mpris;

    QVariantHash info;
    info[QString("album")] = QString("unknown");
    info[QString("artist")] = QString("unknown");
    info[QString("duration")] = QString("0");
    info[QString("progress")] = QString("0");
    info[QString("title")] = QString("unknown");

    if (playerName == QString("mpd"))
        // mpd
        return getPlayerMpdInfo(mpdAddress, mpdPort);
    else if (playerName == QString("mpris")) {
        // players which supports mpris
        if (mpris == QString("auto")) mpris = getAutoMpris();
        if (mpris.isEmpty()) return info;
        return getPlayerMprisInfo(mpris);
    }

    return info;
}


QVariantHash ExtendedSysMon::getPlayerMpdInfo(const QString mpdAddress, const QString mpdPort) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "MPD" << QString("%1:%2").arg(mpdAddress).arg(mpdPort);

    QVariantHash info;
    info[QString("album")] = QString("unknown");
    info[QString("artist")] = QString("unknown");
    info[QString("duration")] = QString("0");
    info[QString("progress")] = QString("0");
    info[QString("title")] = QString("unknown");

    QString cmd = QString("bash -c \"echo 'currentsong\nstatus\nclose' | curl --connect-timeout 1 -fsm 3 telnet://%1:%2\"")
                    .arg(mpdAddress)
                    .arg(mpdPort);
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    foreach(QString str, qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
        if (str.split(QString(": "), QString::SkipEmptyParts).count() > 1) {
            if (str.split(QString(": "), QString::SkipEmptyParts).first() == QString("Album"))
                info[QString("album")] = str.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
            else if (str.split(QString(": "), QString::SkipEmptyParts).first() == QString("Artist"))
                info[QString("artist")] = str.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
            else if (str.split(QString(": "), QString::SkipEmptyParts).first() == QString("time")) {
                info[QString("duration")] = str.split(QString(": "), QString::SkipEmptyParts)[1].trimmed().split(QString(":"))[0];
                info[QString("progress")] = str.split(QString(": "), QString::SkipEmptyParts)[1].trimmed().split(QString(":"))[1];
            } else if (str.split(QString(": "), QString::SkipEmptyParts).first() == QString("Title"))
                info[QString("title")] = str.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
        }
    }

    return info;
}


QVariantHash ExtendedSysMon::getPlayerMprisInfo(const QString mpris) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << "MPRIS" << mpris;

    QVariantHash info;
    info[QString("album")] = QString("unknown");
    info[QString("artist")] = QString("unknown");
    info[QString("duration")] = 0;
    info[QString("progress")] = 0;
    info[QString("title")] = QString("unknown");

    QDBusConnection bus = QDBusConnection::sessionBus();
    // comes from the following request:
    // qdbus org.mpris.MediaPlayer2.vlc /org/mpris/MediaPlayer2 org.freedesktop.DBus.Properties.Get org.mpris.MediaPlayer2.Player Metadata
    // or the same but using dbus-send:
    // dbus-send --print-reply --session --dest=org.mpris.MediaPlayer2.vlc /org/mpris/MediaPlayer2 org.freedesktop.DBus.Properties.Get string:'org.mpris.MediaPlayer2.Player' string:'Metadata'
    QVariantList args = QVariantList() << QString("org.mpris.MediaPlayer2.Player") << QString("Metadata");
    QDBusMessage request = QDBusMessage::createMethodCall(QString("org.mpris.MediaPlayer2.%1").arg(mpris),
                                                          QString("/org/mpris/MediaPlayer2"),
                                                          QString(""),
                                                          QString("Get"));
    request.setArguments(args);
    QDBusMessage response = bus.call(request, QDBus::BlockWithGui);
    if ((response.type() != QDBusMessage::ReplyMessage) || (response.arguments().isEmpty())) {
        if (debug) qDebug() << PDEBUG << ":" << "Error message" << response.errorMessage();
    } else {
        // another portion of dirty magic
        QVariantHash map = qdbus_cast<QVariantHash>(response.arguments().first()
                                                            .value<QDBusVariant>().variant()
                                                            .value<QDBusArgument>());
        info[QString("album")] = map.value(QString("xesam:album"), QString("unknown"));
        // artist is array
        info[QString("artist")] = map.value(QString("xesam:artist"), QString("unknown")).toString();
        info[QString("duration")] = map.value(QString("mpris:length"), 0).toInt() / (1000 * 1000);
        info[QString("title")] = map.value(QString("xesam:title"), QString("unknown"));
    }

    // position
    args[1] = QString("Position");
    request.setArguments(args);
    response = bus.call(request, QDBus::BlockWithGui);
    if ((response.type() != QDBusMessage::ReplyMessage) || (response.arguments().isEmpty())) {
        if (debug) qDebug() << PDEBUG << ":" << "Error message" << response.errorMessage();
    } else
        // this cast is simpler than the previous one ;)
        info[QString("progress")] =  response.arguments().first().value<QDBusVariant>()
                                             .variant().toLongLong() / (1000 * 1000);

    return info;
}


QVariantHash ExtendedSysMon::getPsStats() const
{
    if (debug) qDebug() << PDEBUG;

    QStringList allDirectories = QDir(QString("/proc")).entryList(QDir::Dirs | QDir::NoDotAndDotDot,
                                                                  QDir::Name);
    QStringList directories = allDirectories.filter(QRegExp(QString("(\\d+)")));
    QStringList running;

    foreach(QString dir, directories) {
        QFile statusFile(QString("/proc/%1/status").arg(dir));
        if (!statusFile.open(QIODevice::ReadOnly)) continue;
        QFile cmdFile(QString("/proc/%1/cmdline").arg(dir));
        if (!cmdFile.open(QIODevice::ReadOnly)) continue;

        QString output = statusFile.readAll();
        if (output.contains(QString("running"))) running.append(cmdFile.readAll());
    }

    QVariantHash psStats;
    psStats[QString("pscount")] = running.count();
    psStats[QString("ps")] = running.join(QString(","));
    psStats[QString("pstotal")] = directories.count();

    return psStats;
}


bool ExtendedSysMon::sourceRequestEvent(const QString &source)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source" << source;

    return updateSourceEvent(source);
}


bool ExtendedSysMon::updateSourceEvent(const QString &source)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source" << source;

    if (source == QString("battery")) {
        QVariantHash battery = getBattery(configuration[QString("ACPIPATH")]);
        foreach(QString key, battery.keys()) setData(source, key, battery[key]);
    } else if (source == QString("custom")) {
        foreach(ExtScript *script, extScripts->items()) {
            QVariantHash data = script->run();
            foreach(QString key, data.keys()) setData(source, key, data[key]);
        }
    } else if (source == QString("desktop")) {
        QVariantHash desktop = getCurrentDesktop();
        foreach(QString key, desktop.keys()) setData(source, key, desktop[key]);
    } else if (source == QString("gpu")) {
        setData(source, QString("value"), getGpu(configuration[QString("GPUDEV")]));
    } else if (source == QString("gputemp")) {
        setData(source, QString("value"), getGpuTemp(configuration[QString("GPUDEV")]));
    } else if (source == QString("hddtemp")) {
        QStringList deviceList = configuration[QString("HDDDEV")].split(QChar(','), QString::SkipEmptyParts);
        QStringList allHddDevices = getAllHdd();
        foreach(QString device, allHddDevices)
            setData(source, device, deviceList.contains(device) ?
                    getHddTemp(configuration[QString("HDDTEMPCMD")], device) : 0.0);
    } else if (source == QString("netdev")) {
        setData(source, QString("value"), getNetworkDevice());
    } else if (source == QString("pkg")) {
        foreach(ExtUpgrade *upgrade, extUpgrade->items()) {
            QVariantHash data = upgrade->run();
            foreach(QString key, data.keys()) setData(source, key, data[key]);
        }
    } else if (source == QString("player")) {
        QVariantHash player = getPlayerInfo(configuration[QString("PLAYER")],
                                            configuration[QString("MPDADDRESS")],
                                            configuration[QString("MPDPORT")],
                                            configuration[QString("MPRIS")]);
        foreach(QString key, player.keys()) setData(source, key, player[key]);
    } else if (source == QString("ps")) {
        QVariantHash ps = getPsStats();
        foreach(QString key, ps.keys()) setData(source, key, ps[key]);
    } else if (source == QString("quotes")) {
        foreach(ExtQuotes *quote, extQuotes->items()) {
            QVariantHash data = quote->run();
            foreach(QString key, data.keys()) setData(source, key, data[key]);
        }
    } else if (source == QString("update")) {
        setData(source, QString("value"), true);
    } else if (source == QString("weather")) {
        foreach(ExtWeather *weather, extWeather->items()) {
            QVariantHash data = weather->run();
            foreach(QString key, data.keys()) setData(source, key, data[key]);
        }
    }

    return true;
}


K_EXPORT_PLASMA_DATAENGINE_WITH_JSON(extsysmon, ExtendedSysMon, "plasma-dataengine-extsysmon.json")

#include "extsysmon.moc"
