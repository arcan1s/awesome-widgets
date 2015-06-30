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
#include <task/taskadds.h>

#include "extquotes.h"
#include "extscript.h"
#include "extupgrade.h"
#include "version.h"


ExtendedSysMon::ExtendedSysMon(QObject* parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)

    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));

    setMinimumPollingInterval(333);
    readConfiguration();
    initQuotes();
    initScripts();
    initUpgrade();
}


ExtendedSysMon::~ExtendedSysMon()
{
    if (debug) qDebug() << PDEBUG;

    externalQuotes.clear();
    externalScripts.clear();
    externalUpgrade.clear();
}


QString ExtendedSysMon::getAllHdd()
{
    if (debug) qDebug() << PDEBUG;

    QStringList allDevices = QDir(QString("/dev")).entryList(QDir::System, QDir::Name);
    QStringList devices = allDevices.filter(QRegExp(QString("^[hms]d[a-z]$")));
    for (int i=0; i<devices.count(); i++)
        devices[i] = QString("/dev/%1").arg(devices[i]);

    if (debug) qDebug() << PDEBUG << ":" << "Device list" << devices;
    return devices.join(QChar(','));
}


QString ExtendedSysMon::getAutoGpu()
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


QString ExtendedSysMon::getAutoMpris()
{
    if (debug) qDebug() << PDEBUG;

    QDBusMessage listServices = QDBusConnection::sessionBus().interface()->call(QDBus::BlockWithGui, QString("ListNames"));
    if (listServices.arguments().isEmpty()) return QString();
    QStringList arguments = listServices.arguments()[0].toStringList();

    for (int i=0; i<arguments.count(); i++) {
        if (debug) qDebug() << PDEBUG << ":" << "Service found" << arguments[i];
        if (!arguments[i].startsWith(QString("org.mpris.MediaPlayer2."))) continue;
        QString service = arguments[i];
        service.remove(QString("org.mpris.MediaPlayer2."));
        return service;
    }

    return QString();
}


void ExtendedSysMon::initQuotes()
{
    if (debug) qDebug() << PDEBUG;

    // create directory at $HOME and create dirs list
    QString localDir = QString("%1/awesomewidgets/quotes")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    QDir localDirectory;
    if ((!localDirectory.exists(localDir)) && (localDirectory.mkpath(localDir)))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("awesomewidgets/quotes"),
                                                 QStandardPaths::LocateDirectory);

    QStringList names;
    for (int i=0; i<dirs.count(); i++) {
        QStringList files = QDir(dirs[i]).entryList(QDir::Files, QDir::Name);
        for (int j=0; j<files.count(); j++) {
            if (!files[j].endsWith(QString(".desktop"))) continue;
            if (names.contains(files[j])) continue;
            if (debug) qDebug() << PDEBUG << ":" << "Found file" << files[j] << "in" << dirs[i];
            names.append(files[j]);
            externalQuotes.append(new ExtQuotes(0, files[j], dirs, debug));
        }
    }
}


void ExtendedSysMon::initScripts()
{
    if (debug) qDebug() << PDEBUG;

    // create directory at $HOME and create dirs list
    QString localDir = QString("%1/awesomewidgets/scripts")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    QDir localDirectory;
    if ((!localDirectory.exists(localDir)) && (localDirectory.mkpath(localDir)))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("awesomewidgets/scripts"),
                                                 QStandardPaths::LocateDirectory);

    QStringList names;
    for (int i=0; i<dirs.count(); i++) {
        QStringList files = QDir(dirs[i]).entryList(QDir::Files, QDir::Name);
        for (int j=0; j<files.count(); j++) {
            if (!files[j].endsWith(QString(".desktop"))) continue;
            if (names.contains(files[j])) continue;
            if (debug) qDebug() << PDEBUG << ":" << "Found file" << files[j] << "in" << dirs[i];
            names.append(files[j]);
            externalScripts.append(new ExtScript(0, files[j], dirs, debug));
        }
    }
}


void ExtendedSysMon::initUpgrade()
{
    if (debug) qDebug() << PDEBUG;

    // create directory at $HOME and create dirs list
    QString localDir = QString("%1/awesomewidgets/upgrade")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    QDir localDirectory;
    if ((!localDirectory.exists(localDir)) && (localDirectory.mkpath(localDir)))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("awesomewidgets/upgrade"),
                                                 QStandardPaths::LocateDirectory);

    QStringList names;
    for (int i=0; i<dirs.count(); i++) {
        QStringList files = QDir(dirs[i]).entryList(QDir::Files, QDir::Name);
        for (int j=0; j<files.count(); j++) {
            if (!files[j].endsWith(QString(".desktop"))) continue;
            if (names.contains(files[j])) continue;
            if (debug) qDebug() << PDEBUG << ":" << "Found file" << files[j] << "in" << dirs[i];
            names.append(files[j]);
            externalUpgrade.append(new ExtUpgrade(0, files[j], dirs, debug));
        }
    }
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
    QMap<QString, QString> rawConfig;

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


QMap<QString, QString> ExtendedSysMon::updateConfiguration(QMap<QString, QString> rawConfig)
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
    allHddDevices = getAllHdd().split(QChar(','), QString::SkipEmptyParts);
    if (rawConfig[QString("HDDDEV")] == QString("all"))
        rawConfig[QString("HDDDEV")] = allHddDevices.join(QChar(','));
    else if (rawConfig[QString("HDDDEV")] == QString("disable"))
        rawConfig[QString("HDDDEV")] = QString("");
    else {
        QStringList deviceList = rawConfig[QString("HDDDEV")].split(QChar(','), QString::SkipEmptyParts);
        QStringList devices;
        QRegExp diskRegexp = QRegExp("^/dev/[hms]d[a-z]$");
        for (int i=0; i<deviceList.count(); i++)
            if ((QFile::exists(deviceList[i])) &&
                (diskRegexp.indexIn(deviceList[i]) > -1))
                devices.append(deviceList[i]);
        if (devices.isEmpty())
            rawConfig[QString("HDDDEV")] = allHddDevices.join(QChar(','));
        else
            rawConfig[QString("HDDDEV")] = devices.join(QChar(','));
    }
    // player
    if ((rawConfig[QString("PLAYER")] != QString("mpd")) &&
        (rawConfig[QString("PLAYER")] != QString("mpris")))
        rawConfig[QString("PLAYER")] = QString("mpris");

    for (int i=0; i<rawConfig.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" <<
            rawConfig.keys()[i] + QString("=") + rawConfig[rawConfig.keys()[i]];
    return rawConfig;
}


QVariantMap ExtendedSysMon::getBattery(const QString acpiPath)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "ACPI path" << acpiPath;

    QVariantMap battery;
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
        QFile currentLevelFile(QString("%1/%2/energy_now").arg(acpiPath).arg(batDevices[i]));
        QFile fullLevelFile(QString("%1/%2/energy_full").arg(acpiPath).arg(batDevices[i]));
        if ((currentLevelFile.open(QIODevice::ReadOnly)) &&
            (fullLevelFile.open(QIODevice::ReadOnly))) {
            float batCurrent = QString(currentLevelFile.readLine()).trimmed().toFloat();
            float batFull = QString(fullLevelFile.readLine()).trimmed().toFloat();
            battery[QString("bat%1").arg(i)] = 100 * batCurrent / batFull;
            currentLevel += batCurrent;
            fullLevel += batFull;
        }
        currentLevelFile.close();
        fullLevelFile.close();
    }
    battery[QString("bat")] = 100 * currentLevel / fullLevel;

    return battery;
}


QVariantMap ExtendedSysMon::getCurrentDesktop()
{
    if (debug) qDebug() << PDEBUG;

    int number = KWindowSystem::currentDesktop();
    int total = KWindowSystem::numberOfDesktops();
    QVariantMap currentDesktop;
    currentDesktop[QString("currentName")] = KWindowSystem::desktopName(number);
    currentDesktop[QString("currentNumber")] = number;
    QStringList list;
    for (int i=1; i<total+1; i++)
        list.append(KWindowSystem::desktopName(i));
    currentDesktop[QString("list")] = list.join(QString(";;"));
    currentDesktop[QString("number")] = total;

    return currentDesktop;
}


float ExtendedSysMon::getGpu(const QString device)
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
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("<gpu_util>"))) {
                QString load = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .remove(QString("<gpu_util>"))
                        .remove(QString("</gpu_util>"))
                        .remove(QChar('%'));
                value = load.toFloat();
            }
        }
    else if (configuration[QString("GPUDEV")] == QString("ati"))
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("load"))) {
                QString load = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .split(QChar(' '), QString::SkipEmptyParts)[3]
                        .remove(QChar('%'));
                value = load.toFloat();
            }
        }

    return value;
}


float ExtendedSysMon::getGpuTemp(const QString device)
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
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("<gpu_temp>"))) {
                QString temp = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .remove(QString("<gpu_temp>"))
                        .remove(QString("C</gpu_temp>"));
                value = temp.toFloat();
            }
        }
    else if (configuration[QString("GPUDEV")] == QString("ati"))
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("Temperature"))) {
                QString temp = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .split(QChar(' '), QString::SkipEmptyParts)[4];
                value = temp.toFloat();
            }
        }

    return value;
}


float ExtendedSysMon::getHddTemp(const QString cmd, const QString device)
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
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (!qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].startsWith(QString("194"))) continue;
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].split(QChar(' '), QString::SkipEmptyParts).count() < 9) break;
            value = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].split(QChar(' '), QString::SkipEmptyParts)[9].toFloat();
            break;
        }
    } else {
        if (qoutput.split(QChar(':'), QString::SkipEmptyParts).count() >= 3) {
            QString temp = qoutput.split(QChar(':'), QString::SkipEmptyParts)[2];
            temp.remove(QChar(0260)).remove(QChar('C'));
            value = temp.toFloat();
        }
    }

    return value;
}


QString ExtendedSysMon::getNetworkDevice()
{
    if (debug) qDebug() << PDEBUG;

    QString device = QString("lo");
    QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
    for (int i=0; i<rawInterfaceList.count(); i++)
        if ((rawInterfaceList[i].flags().testFlag(QNetworkInterface::IsUp)) &&
            (!rawInterfaceList[i].flags().testFlag(QNetworkInterface::IsLoopBack)) &&
            (!rawInterfaceList[i].flags().testFlag(QNetworkInterface::IsPointToPoint))) {
            device = rawInterfaceList[i].name();
            break;
        }

    return device;
}


QVariantMap ExtendedSysMon::getPlayerInfo(const QString playerName, const QString mpdAddress,
                                          const QString mpdPort, QString mpris)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "player" << playerName;
    if (debug) qDebug() << PDEBUG << ":" << "MPD" << QString("%1:%2").arg(mpdAddress).arg(mpdPort);
    if (debug) qDebug() << PDEBUG << ":" << "MPRIS" << mpris;

    QVariantMap info;
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


QVariantMap ExtendedSysMon::getPlayerMpdInfo(const QString mpdAddress, const QString mpdPort)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "MPD" << QString("%1:%2").arg(mpdAddress).arg(mpdPort);

    QVariantMap info;
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
    QString qstr = QString("");
    for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
        qstr = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i];
        if (qstr.split(QString(": "), QString::SkipEmptyParts).count() > 1) {
            if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("Album"))
                info[QString("album")] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
            else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("Artist"))
                info[QString("artist")] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
            else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("time")) {
                info[QString("duration")] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed().split(QString(":"))[0];
                info[QString("progress")] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed().split(QString(":"))[1];
            } else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("Title"))
                info[QString("title")] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
        }
    }

    return info;
}


QVariantMap ExtendedSysMon::getPlayerMprisInfo(const QString mpris)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << "MPRIS" << mpris;

    QVariantMap info;
    info[QString("album")] = QString("unknown");
    info[QString("artist")] = QString("unknown");
    info[QString("duration")] = 0;
    info[QString("progress")] = 0;
    info[QString("title")] = QString("unknown");

    // init
    QDBusArgument arg;
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage response, request;
    QVariantMap map;

    // general information
    request = QDBusMessage::createMethodCall(QString("org.mpris.MediaPlayer2.%1").arg(mpris),
                                             QString("/Player"),
                                             QString(""),
                                             QString("GetMetadata"));
    response = bus.call(request, QDBus::BlockWithGui);
    if (response.arguments().size() == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Error message" << response.errorMessage();
    } else {
        arg = response.arguments()[0].value<QDBusArgument>();
        arg >> map;
        info[QString("album")] = map[QString("album")];
        info[QString("artist")] = map[QString("artist")];
        info[QString("duration")] = map[QString("time")];
        info[QString("title")] = map[QString("title")];
    }

    // position
    request = QDBusMessage::createMethodCall(QString("org.mpris.MediaPlayer2.%1").arg(mpris),
                                             QString("/Player"),
                                             QString(""),
                                             QString("PositionGet"));
    response = bus.call(request, QDBus::BlockWithGui);
    if (response.arguments().size() == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Error message" << response.errorMessage();
    } else
        info[QString("progress")] = response.arguments()[0].toInt() / 1000;

    return info;
}


QVariantMap ExtendedSysMon::getPsStats()
{
    if (debug) qDebug() << PDEBUG;

    QStringList allDirectories = QDir(QString("/proc")).entryList(QDir::Dirs | QDir::NoDotAndDotDot,
                                                                  QDir::Name);
    QStringList directories = allDirectories.filter(QRegExp(QString("(\\d+)")));
    QStringList running;

    for (int i=0; i<directories.count(); i++) {
        QFile statusFile(QString("/proc/%1/status").arg(directories[i]));
        if (!statusFile.open(QIODevice::ReadOnly)) continue;
        QFile cmdFile(QString("/proc/%1/cmdline").arg(directories[i]));
        if (!cmdFile.open(QIODevice::ReadOnly)) continue;

        QString output = statusFile.readAll();
        if (output.contains(QString("running"))) running.append(cmdFile.readAll());
    }

    QVariantMap psStats;
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
        QVariantMap battery = getBattery(configuration[QString("ACPIPATH")]);
        for (int i=0; i<battery.keys().count(); i++)
            setData(source, battery.keys()[i], battery[battery.keys()[i]]);
    } else if (source == QString("custom")) {
        for (int i=0; i<externalScripts.count(); i++)
            setData(source, externalScripts[i]->tag(), externalScripts[i]->run());
    } else if (source == QString("desktop")) {
        QVariantMap desktop = getCurrentDesktop();
        for (int i=0; i<desktop.keys().count(); i++)
            setData(source, desktop.keys()[i], desktop[desktop.keys()[i]]);
    } else if (source == QString("gpu")) {
        setData(source, QString("value"), getGpu(configuration[QString("GPUDEV")]));
    } else if (source == QString("gputemp")) {
        setData(source, QString("value"), getGpuTemp(configuration[QString("GPUDEV")]));
    } else if (source == QString("hddtemp")) {
        // fill empty list
        for (int i=0; i<allHddDevices.count(); i++)
            setData(source, allHddDevices[i], 0.0);
        QStringList deviceList = configuration[QString("HDDDEV")].split(QChar(','), QString::SkipEmptyParts);
        for (int i=0; i<deviceList.count(); i++) {
            setData(source, deviceList[i],
                    getHddTemp(configuration[QString("HDDTEMPCMD")], deviceList[i]));
        }
    } else if (source == QString("netdev")) {
        setData(source, QString("value"), getNetworkDevice());
    } else if (source == QString("pkg")) {
        for (int i=0; i<externalUpgrade.count(); i++)
            setData(source, externalUpgrade[i]->tag(), externalUpgrade[i]->run());
    } else if (source == QString("player")) {
        QVariantMap player = getPlayerInfo(configuration[QString("PLAYER")],
                                           configuration[QString("MPDADDRESS")],
                                           configuration[QString("MPDPORT")],
                                           configuration[QString("MPRIS")]);
        for (int i=0; i<player.keys().count(); i++)
            setData(source, player.keys()[i], player[player.keys()[i]]);
    } else if (source == QString("ps")) {
        QVariantMap ps = getPsStats();
        for (int i=0; i<ps.keys().count(); i++)
            setData(source, ps.keys()[i], ps[ps.keys()[i]]);
    } else if (source == QString("quotes")) {
        for (int i=0; i<externalQuotes.count(); i++) {
            QMap<QString, float> data = externalQuotes[i]->run();
            setData(source, externalQuotes[i]->tag(QString("ask")), data[QString("ask")]);
            setData(source, externalQuotes[i]->tag(QString("askchg")), data[QString("askchg")]);
            setData(source, externalQuotes[i]->tag(QString("percaskchg")), data[QString("percaskchg")]);
            setData(source, externalQuotes[i]->tag(QString("bid")), data[QString("bid")]);
            setData(source, externalQuotes[i]->tag(QString("bidchg")), data[QString("bidchg")]);
            setData(source, externalQuotes[i]->tag(QString("percbidchg")), data[QString("percbidchg")]);
            setData(source, externalQuotes[i]->tag(QString("price")), data[QString("price")]);
            setData(source, externalQuotes[i]->tag(QString("pricechg")), data[QString("pricechg")]);
            setData(source, externalQuotes[i]->tag(QString("percpricechg")), data[QString("percpricechg")]);
        }
    } else if (source == QString("update")) {
        setData(source, QString("value"), true);
    }

    return true;
}


K_EXPORT_PLASMA_DATAENGINE_WITH_JSON(extsysmon, ExtendedSysMon, "plasma-dataengine-extsysmon.json")

#include "extsysmon.moc"
