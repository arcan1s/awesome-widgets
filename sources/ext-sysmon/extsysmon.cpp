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

#include <Plasma/DataContainer>
#include <KDE/KGlobal>
#include <KDE/KStandardDirs>
#include <KDE/KWindowSystem>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcessEnvironment>
#include <QRegExp>
#include <QTextCodec>
#include <QThread>

#include <extscript.h>
#include <pdebug/pdebug.h>
#include <task/taskadds.h>


ExtendedSysMon::ExtendedSysMon(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args);

    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    if (debugEnv == QString("yes"))
        debug = true;
    else
        debug = false;

    setMinimumPollingInterval(333);
    readConfiguration();
    initScripts();
}


QString ExtendedSysMon::getAllHdd()
{
    if (debug) qDebug() << PDEBUG;

    QStringList devices;
    QString cmd = QString("find /dev -name [hms]d[a-z]");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++)
        devices.append(qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]);

    if (debug) qDebug() << PDEBUG << ":" << "Device list" << devices;
    return devices.join(QChar(','));
}


QString ExtendedSysMon::getAutoGpu()
{
    if (debug) qDebug() << PDEBUG;

    QString gpu = QString("disable");
    QString cmd = QString("lspci");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    if (qoutput.toLower().contains("nvidia"))
        gpu = QString("nvidia");
    else if (qoutput.toLower().contains("radeon"))
        gpu = QString("ati");

    if (debug) qDebug() << PDEBUG << ":" << "Device" << gpu;
    return gpu;
}


QString ExtendedSysMon::getAutoMpris()
{
    if (debug) qDebug() << PDEBUG;

    QString mpris;
    QString cmd = QString("bash -c \"qdbus 'org.mpris.MediaPlayer2.*'\"");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    TaskResult process = runTask(cmd);

    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    if (qoutput.split(QChar('\n'))[0].split(QChar('.')).count() > 3)
        mpris = qoutput.split(QChar('\n'))[0].split(QChar('.'))[3];

    if (debug) qDebug() << PDEBUG << ":" << "Player found" << mpris;
    return mpris;
}


void ExtendedSysMon::initScripts()
{
    if (debug) qDebug() << PDEBUG;

    // create directory at $HOME
    QString localDir = KStandardDirs::locateLocal("data", "plasma_engine_extsysmon/scripts");
    if (KStandardDirs::makeDir(localDir))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = KGlobal::dirs()->findDirs("data", "plasma_engine_extsysmon/scripts");
    QStringList names;
    for (int i=0; i<dirs.count(); i++) {
        QStringList files = QDir(dirs[i]).entryList(QDir::Files, QDir::Name);
        for (int j=0; j<files.count(); j++) {
            if (files[j].endsWith(QString(".conf"))) continue;
            if (names.contains(files[j])) continue;
            if (debug) qDebug() << PDEBUG << ":" << "Found file" << files[j] << "in" << dirs[i];
            names.append(files[j]);
            externalScripts.append(new ExtScript(0, files[j], dirs, debug));
            times.append(1);
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
    source.append(QString("gpu"));
    source.append(QString("gputemp"));
    source.append(QString("hddtemp"));
    source.append(QString("pkg"));
    source.append(QString("player"));
    source.append(QString("ps"));

    if (debug) qDebug() << PDEBUG << ":" << "Sources" << source;
    return source;
}


void ExtendedSysMon::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    // pre-setup
    QMap<QString, QString> rawConfig;
    rawConfig[QString("ACPIPATH")] = QString("/sys/class/power_supply/");
    rawConfig[QString("GPUDEV")] = QString("auto");
    rawConfig[QString("HDDDEV")] = QString("all");
    rawConfig[QString("HDDTEMPCMD")] = QString("sudo hddtemp");
    rawConfig[QString("MPDADDRESS")] = QString("localhost");
    rawConfig[QString("MPDPORT")] = QString("6600");
    rawConfig[QString("MPRIS")] = QString("auto");
    rawConfig[QString("PKGCMD")] = QString("pacman -Qu");
    rawConfig[QString("PKGNULL")] = QString("0");
    rawConfig[QString("PLAYER")] = QString("mpris");

    QString fileName = KGlobal::dirs()->findResource("config", "extsysmon.conf");
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QFile configFile(fileName);
    if (!configFile.open(QIODevice::ReadOnly)) {
        configuration = updateConfiguration(rawConfig);
        return;
    }
    QString fileStr;
    QStringList value;
    while (true) {
        fileStr = QString(configFile.readLine()).trimmed();
        if ((fileStr.isEmpty()) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar('#')) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar(';')) && (!configFile.atEnd())) continue;
        if (fileStr.contains(QChar('='))) {
            value.clear();
            for (int i=1; i<fileStr.split(QChar('=')).count(); i++)
                value.append(fileStr.split(QChar('='))[i]);
            rawConfig[fileStr.split(QChar('='))[0]] = value.join(QChar('='));
        }
        if (configFile.atEnd()) break;
    }
    configFile.close();

    configuration = updateConfiguration(rawConfig);
}


QMap<QString, QString> ExtendedSysMon::updateConfiguration(const QMap<QString, QString> rawConfig)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> config;
    QString key, value;
    // remove spaces and copy source map
    for (int i=0; i<rawConfig.keys().count(); i++) {
        key = rawConfig.keys()[i];
        value = rawConfig[key];
        key.remove(QChar(' '));
        if ((key != QString("HDDTEMPCMD")) &&
            (key != QString("PKGCMD")))
            value.remove(QChar(' '));
        config[key] = value;
    }
    // update values
    // gpudev
    if (config[QString("GPUDEV")] == QString("disable"))
        config[QString("GPUDEV")] = QString("disable");
    else if (config[QString("GPUDEV")] == QString("auto"))
        config[QString("GPUDEV")] = getAutoGpu();
    else if ((config[QString("GPUDEV")] != QString("ati")) &&
        (config[QString("GPUDEV")] != QString("nvidia")))
        config[QString("GPUDEV")] = getAutoGpu();
    // hdddev
    if (config[QString("HDDDEV")] == QString("all"))
        config[QString("HDDDEV")] = getAllHdd();
    else if (config[QString("HDDDEV")] == QString("disable"))
        config[QString("HDDDEV")] = QString("");
    else {
        QStringList deviceList = config[QString("HDDDEV")].split(QChar(','), QString::SkipEmptyParts);
        QStringList devices;
        QRegExp diskRegexp = QRegExp("/dev/[hms]d[a-z]$");
        for (int i=0; i<deviceList.count(); i++)
            if ((QFile::exists(deviceList[i])) &&
                (diskRegexp.indexIn(deviceList[i]) > -1))
                devices.append(deviceList[i]);
        if (devices.isEmpty())
            config[QString("HDDDEV")] = getAllHdd();
        else
            config[QString("HDDDEV")] = devices.join(QChar(','));
    }
    // pkgcmd
    for (int i=config[QString("PKGNULL")].split(QString(","), QString::SkipEmptyParts).count();
         i<config[QString("PKGCMD")].split(QString(","), QString::SkipEmptyParts).count()+1;
         i++)
        config[QString("PKGNULL")] += QString(",0");
    // player
    if ((config[QString("PLAYER")] != QString("mpd")) &&
        (config[QString("PLAYER")] != QString("mpris")))
        config[QString("PLAYER")] = QString("mpris");

    for (int i=0; i<config.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" <<
            config.keys()[i] + QString("=") + config[config.keys()[i]];
    return config;
}


QMap<QString, QVariant> ExtendedSysMon::getBattery(const QString acpiPath)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "ACPI path" << acpiPath;

    QMap<QString, QVariant> battery;
    battery[QString("ac")] = false;
    battery[QString("bat")] = 0;
    QFile acFile(acpiPath + QString("/AC/online"));
    if (acFile.open(QIODevice::ReadOnly)) {
        if (QString(acFile.readLine()).trimmed().toInt() == 1)
            battery[QString("ac")] = true;
    }
    acFile.close();
    // batterites
    QStringList allDevices = QDir(acpiPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    QStringList batDevices;
    QRegExp batRegexp = QRegExp(QString("BAT.*"));
    for (int i=0; i<allDevices.count(); i++)
        if (allDevices[i].contains(batRegexp))
            batDevices.append(allDevices[i]);
    for (int i=0; i<batDevices.count(); i++) {
        QFile batFile(acpiPath + QString("/") + batDevices[i] + QString("/capacity"));
        if (batFile.open(QIODevice::ReadOnly))
            battery[QString("bat") + QString::number(i)] = QString(batFile.readLine()).trimmed().toInt();
        batFile.close();
    }
    float number = 0.0;
    float average = 0.0;
    for (int i=0; i<battery.keys().count(); i++) {
        if (battery.keys()[i] == QString("ac")) continue;
        if (battery.keys()[i] == QString("bat")) continue;
        average += battery[battery.keys()[i]].toInt();
        number++;
    }
    battery[QString("bat")] = int(average / number);

    return battery;
}


QMap<QString, QVariant> ExtendedSysMon::getCurrentDesktop()
{
    if (debug) qDebug() << PDEBUG;

    int number = KWindowSystem::currentDesktop();
    int total = KWindowSystem::numberOfDesktops();
    QMap<QString, QVariant> currentDesktop;
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
    TaskResult process = runTask(QString("bash -c \"") + cmd + QString("\""));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

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
    TaskResult process = runTask(QString("bash -c \"") + cmd + QString("\""));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

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
    TaskResult process = runTask(cmd + QString(" ") + device);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    if (qoutput.split(QChar(':'), QString::SkipEmptyParts).count() >= 3) {
        QString temp = qoutput.split(QChar(':'), QString::SkipEmptyParts)[2];
        temp.remove(QChar(0260)).remove(QChar('C'));
        value = temp.toFloat();
    }

    return value;
}


QMap<QString, QVariant> ExtendedSysMon::getPlayerInfo(const QString playerName,
                                                      const QString mpdAddress,
                                                      const QString mpdPort,
                                                      QString mpris)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "player" << playerName;
    if (debug) qDebug() << PDEBUG << ":" << "MPD" << mpdAddress + QString(":") + mpdPort;
    if (debug) qDebug() << PDEBUG << ":" << "MPRIS" << mpris;

    QMap<QString, QVariant> info;
    info[QString("album")] = QString("unknown");
    info[QString("artist")] = QString("unknown");
    info[QString("duration")] = QString("0");
    info[QString("progress")] = QString("0");
    info[QString("title")] = QString("unknown");
    QString cmd;
    if (playerName == QString("mpd"))
        // mpd
        cmd = QString("bash -c \"echo 'currentsong\nstatus\nclose' | curl --connect-timeout 1 -fsm 3 telnet://%1:%2\"")
                .arg(mpdAddress)
                .arg(mpdPort);
    else if (playerName == QString("mpris")) {
        // players which supports mpris
        if (mpris == "auto")
            mpris = getAutoMpris();
        cmd = QString("bash -c \"qdbus org.mpris.%1 /Player GetMetadata && qdbus org.mpris.%1 /Player PositionGet\"")
                .arg(mpris);
    }
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    QString qstr = QString("");
    if (playerName == QString("mpd"))
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
    else if (playerName == QString("mpris"))
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            qstr = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i];
            if (qstr.split(QString(": "), QString::SkipEmptyParts).count() > 1) {
                if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("album"))
                    info[QString("album")] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("artist"))
                    info[QString("artist")] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("time"))
                    info[QString("duration")] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("title"))
                    info[QString("title")] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
            } else {
                int time = qstr.toInt() / 1000;
                info[QString("progress")] = QString::number(time);
            }
        }

    return info;
}


QMap<QString, QVariant> ExtendedSysMon::getPsStats()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QVariant> psStats;
    QString cmd, qoutput;
    cmd = QString("ps --no-headers -o command");
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

    qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    QStringList psList;
    for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++)
        if (!qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("ps ")))
            psList.append(qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]);
    psStats[QString("psCount")] = psList.count();
    psStats[QString("ps")] = psList.join(QString(","));

    cmd = QString("ps -e --no-headers -o command");
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

    qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    psStats[QString("psTotal")] = qoutput.split(QChar('\n'), QString::SkipEmptyParts).count();

    return psStats;
}


int ExtendedSysMon::getUpgradeInfo(const QString cmd)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;

    TaskResult process = runTask(QString("bash -c \"") + cmd + QString("\""));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    int count = 0;
    for (int i=0; i<qoutput.split(QChar('\n')).count(); i++)
        if (!qoutput.isEmpty())
            count++;

    return count;
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
        QMap<QString, QVariant> battery = getBattery(configuration[QString("ACPIPATH")]);
        setData(source, QString("ac"), battery[QString("ac")].toBool());
        for (int i=0; i<battery.keys().count(); i++) {
            if (battery.keys()[i] == QString("ac")) continue;
            setData(source, battery.keys()[i], battery[battery.keys()[i]].toInt());
        }
    } else if (source == QString("custom")) {
        for (int i=0; i<externalScripts.count(); i++) {
            ExtScript::ScriptData data = externalScripts[i]->run(times[i]);
            if (!data.active) return true;
            if (data.refresh) {
                times[i] = 1;
                setData(source, QString("custom") + QString::number(i), data.output);
            } else
                times[i]++;
        }
    } else if (source == QString("desktop")) {
        QMap<QString, QVariant> desktop = getCurrentDesktop();
        for (int i=0; i<desktop.keys().count(); i++)
            setData(source, desktop.keys()[i], desktop[desktop.keys()[i]]);
    } else if (source == QString("gpu")) {
        setData(source, QString("GPU"), getGpu(configuration[QString("GPUDEV")]));
    } else if (source == QString("gputemp")) {
        setData(source, QString("GPUTemp"), getGpuTemp(configuration[QString("GPUDEV")]));
    } else if (source == QString("hddtemp")) {
        QStringList deviceList = configuration[QString("HDDDEV")].split(QChar(','), QString::SkipEmptyParts);
        for (int i=0; i<deviceList.count(); i++)
            setData(source, deviceList[i],
                    getHddTemp(configuration[QString("HDDTEMPCMD")], deviceList[i]));
    } else if (source == QString("pkg")) {
        for (int i=0; i<configuration[QString("PKGCMD")].split(QString(","), QString::SkipEmptyParts).count(); i++)
            setData(source, QString("pkgCount") + QString::number(i),
                    getUpgradeInfo(configuration[QString("PKGCMD")].split(QString(","), QString::SkipEmptyParts)[i]));
    } else if (source == QString("player")) {
        QMap<QString, QVariant> player = getPlayerInfo(configuration[QString("PLAYER")],
                configuration[QString("MPDADDRESS")],
                configuration[QString("MPDPORT")],
                configuration[QString("MPRIS")]);
        for (int i=0; i<player.keys().count(); i++)
            setData(source, player.keys()[i], player[player.keys()[i]]);
    } else if (source == QString("ps")) {
        QMap<QString, QVariant> ps = getPsStats();
        for (int i=0; i<ps.keys().count(); i++)
            setData(source, ps.keys()[i], ps[ps.keys()[i]]);
    }

    return true;
}


K_EXPORT_PLASMA_DATAENGINE(extsysmon, ExtendedSysMon)

#include "extsysmon.moc"
