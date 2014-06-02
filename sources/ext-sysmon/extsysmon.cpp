/***************************************************************************
 *   This file is part of pytextmonitor                                    *
 *                                                                         *
 *   pytextmonitor is free software: you can redistribute it and/or        *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   pytextmonitor is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with pytextmonitor. If not, see http://www.gnu.org/licenses/    *
 ***************************************************************************/


#include "extsysmon.h"

#include <Plasma/DataContainer>
#include <KDE/KGlobal>
#include <KDE/KStandardDirs>
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QProcessEnvironment>
#include <QRegExp>
#include <QTextCodec>


ExtendedSysMon::ExtendedSysMon(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)

    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("PTM_DEBUG"), QString("no"));
    if (debugEnv == QString("yes"))
        debug = true;
    else
        debug = false;

    setMinimumPollingInterval(333);
    readConfiguration();
}


QString ExtendedSysMon::getAllHdd()
{
    if (debug) qDebug() << "[DE]" << "[getAllHdd]";
    QProcess command;
    QStringList devices;
    QString cmd = QString("find /dev -name [hms]d[a-z]");
    QString qoutput = QString("");
    if (debug) qDebug() << "[DE]" << "[getAllHdd]" << ":" << "Run cmd" << cmd;
    command.start(cmd);
    command.waitForFinished(-1);
    if (debug) qDebug() << "[DE]" << "[getAllHdd]" << ":" << "Cmd returns" << command.exitCode();
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
    for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++)
        devices.append(qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]);
    if (debug) qDebug() << "[DE]" << "[getAllHdd]" << ":" << "Device list" << devices;
    return devices.join(QChar(','));
}


QString ExtendedSysMon::getAutoGpu()
{
    if (debug) qDebug() << "[DE]" << "[getAutoGpu]";
    QProcess command;
    QString gpu = QString("disable");
    QString cmd = QString("lspci");
    QString qoutput = QString("");
    if (debug) qDebug() << "[DE]" << "[getAutoGpu]" << ":" << "Run cmd" << cmd;
    command.start(cmd);
    command.waitForFinished(-1);
    if (debug) qDebug() << "[DE]" << "[getAutoGpu]" << ":" << "Cmd returns" << command.exitCode();
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
    if (qoutput.toLower().contains("nvidia"))
        gpu = QString("nvidia");
    else if (qoutput.toLower().contains("radeon"))
        gpu = QString("ati");
    if (debug) qDebug() << "[DE]" << "[getAutoGpu]" << ":" << "Device" << gpu;
    return gpu;
}


QStringList ExtendedSysMon::sources() const
{
    if (debug) qDebug() << "[DE]" << "[sources]";
    QStringList source;
    source.append(QString("custom"));
    source.append(QString("gpu"));
    source.append(QString("gputemp"));
    source.append(QString("hddtemp"));
    source.append(QString("pkg"));
    source.append(QString("player"));
    source.append(QString("ps"));
    if (debug) qDebug() << "[DE]" << "[sources]" << ":" << "Sources" << source;
    return source;
}


void ExtendedSysMon::readConfiguration()
{
    if (debug) qDebug() << "[DE]" << "[readConfiguration]";
    // pre-setup
    QMap<QString, QString> rawConfig;
    rawConfig[QString("CUSTOM")] = QString("wget -qO- http://ifconfig.me/ip");
    rawConfig[QString("GPUDEV")] = QString("auto");
    rawConfig[QString("HDDDEV")] = QString("all");
    rawConfig[QString("HDDTEMPCMD")] = QString("sudo hddtemp");
    rawConfig[QString("MPDADDRESS")] = QString("localhost");
    rawConfig[QString("MPDPORT")] = QString("6600");
    rawConfig[QString("PKGCMD")] = QString("pacman -Qu");
    rawConfig[QString("PKGNULL")] = QString("0");
    rawConfig[QString("PLAYER")] = QString("amarok");

    QString fileName = KGlobal::dirs()->findResource("config", "extsysmon.conf");
    if (debug) qDebug() << "[DE]" << "[readConfiguration]" << ":" << "Configuration file" << fileName;
    QFile confFile(fileName);
    bool ok = confFile.open(QIODevice::ReadOnly);
    if (!ok) {
        configuration = updateConfiguration(rawConfig);
        return;
    }
    QString fileStr;
    QStringList value;
    while (true) {
        fileStr = QString(confFile.readLine()).trimmed();
        if (fileStr[0] == QChar('#')) continue;
        if (fileStr[0] == QChar(';')) continue;
        if (fileStr.contains(QChar('='))) {
            value.clear();
            for (int i=1; i<fileStr.split(QChar('=')).count(); i++)
                value.append(fileStr.split(QChar('='))[i]);
            rawConfig[fileStr.split(QChar('='))[0]] = value.join(QChar('='));
        }
        if (confFile.atEnd())
            break;
    }
    confFile.close();
    configuration = updateConfiguration(rawConfig);
}


QMap<QString, QString> ExtendedSysMon::updateConfiguration(const QMap<QString, QString> rawConfig)
{
    if (debug) qDebug() << "[DE]" << "[updateConfiguration]";
    QMap<QString, QString> config;
    QString key, value;
    // remove spaces and copy source map
    for (int i=0; i<rawConfig.keys().count(); i++) {
        key = rawConfig.keys()[i];
        value = rawConfig[key];
        key.remove(QChar(' '));
        if ((key != QString("CUSTOM")) && (key != QString("PKGCMD")))
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
    if ((config[QString("PLAYER")] != QString("amarok")) &&
        (config[QString("PLAYER")] != QString("clementine")) &&
        (config[QString("PLAYER")] != QString("mpd")) &&
        (config[QString("PLAYER")] != QString("qmmp")))
        config[QString("PLAYER")] = QString("amarok");

    for (int i=0; i<config.keys().count(); i++)
        if (debug) qDebug() << "[DE]" << "[updateConfiguration]" << ":" <<
            config.keys()[i] + QString("=") + config[config.keys()[i]];
    return config;
}


QString ExtendedSysMon::getCustomCmd(const QString cmd)
{
    if (debug) qDebug() << "[DE]" << "[getCustomCmd]";
    if (debug) qDebug() << "[DE]" << "[getCustomCmd]" << ":" << "Run function with cmd" << cmd;
    QProcess command;
    QString qoutput = QString("");
    if (debug) qDebug() << "[DE]" << "[getCustomCmd]" << ":" << "Run cmd" << QString("bash -c \"") + cmd + QString("\"");
    command.start(QString("bash -c \"") + cmd + QString("\""));
    command.waitForFinished(-1);
    if (debug) qDebug() << "[DE]" << "[getCustomCmd]" << ":" << "Cmd returns" << command.exitCode();
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput()).trimmed();
    if (debug) qDebug() << "[DE]" << "[getCustomCmd]" << ":" << "Return" << qoutput;
    return qoutput;
}


float ExtendedSysMon::getGpu(const QString device)
{
    if (debug) qDebug() << "[DE]" << "[getGpu]";
    if (debug) qDebug() << "[DE]" << "[getGpu]" << ":" << "Run function with device" << device;
    float gpu = 0.0;
    if ((device != QString("nvidia")) && (device != QString("ati")))
        return gpu;
    QProcess command;
    QString qoutput;

    if (device == QString("nvidia")) {
        QString cmd = QString("nvidia-smi -q -d UTILIZATION");
        if (debug) qDebug() << "[DE]" << "[getGpu]" << ":" << "Run cmd" << cmd;
        command.start(cmd);
        command.waitForFinished(-1);
        if (debug) qDebug() << "[DE]" << "[getGpu]" << ":" << "Cmd returns" << command.exitCode();
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("Gpu"))) {
                QString load = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .split(QChar(' '), QString::SkipEmptyParts)[2]
                        .remove(QChar('%'));
                gpu = load.toFloat();
            }
        }
    }
    else if (device == QString("ati")) {
        QString cmd = QString("aticonfig --od-getclocks");
        if (debug) qDebug() << "[DE]" << "[getGpu]" << ":" << "Run cmd" << cmd;
        command.start(cmd);
        command.waitForFinished(-1);
        if (debug) qDebug() << "[DE]" << "[getGpu]" << ":" << "Cmd returns" << command.exitCode();
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("load"))) {
                QString load = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .split(QChar(' '), QString::SkipEmptyParts)[3]
                        .remove(QChar('%'));
                gpu = load.toFloat();
            }
        }
    }
    if (debug) qDebug() << "[DE]" << "[getGpu]" << ":" << "Return" << gpu;
    return gpu;
}


float ExtendedSysMon::getGpuTemp(const QString device)
{
    if (debug) qDebug() << "[DE]" << "[getGpuTemp]";
    if (debug) qDebug() << "[DE]" << "[getGpuTemp]" << ":" << "Run function with device" << device;
    float gpuTemp = 0.0;
    if ((device != QString("nvidia")) && (device != QString("ati")))
        return gpuTemp;
    QProcess command;
    QString qoutput;

    if (device == QString("nvidia")) {
        QString cmd = QString("nvidia-smi -q -d TEMPERATURE");
        if (debug) qDebug() << "[DE]" << "[getGpuTemp]" << ":" << "Run cmd" << cmd;
        command.start(cmd);
        command.waitForFinished(-1);
        if (debug) qDebug() << "[DE]" << "[getGpuTemp]" << ":" << "Cmd returns" << command.exitCode();
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("Gpu"))) {
                QString temp = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .split(QChar(' '), QString::SkipEmptyParts)[2];
                gpuTemp = temp.toFloat();
            }
        }
    }
    else if (device == QString("ati")) {
        QString cmd = QString("aticonfig --od-gettemperature");
        if (debug) qDebug() << "[DE]" << "[getGpuTemp]" << ":" << "Run cmd" << cmd;
        command.start(cmd);
        command.waitForFinished(-1);
        if (debug) qDebug() << "[DE]" << "[getGpuTemp]" << ":" << "Cmd returns" << command.exitCode();
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("Temperature"))) {
                QString temp = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .split(QChar(' '), QString::SkipEmptyParts)[4];
                gpuTemp = temp.toFloat();
            }
        }
    }
    if (debug) qDebug() << "[DE]" << "[getGpuTemp]" << ":" << "Return" << gpuTemp;
    return gpuTemp;
}


float ExtendedSysMon::getHddTemp(const QString cmd, const QString device)
{
    if (debug) qDebug() << "[DE]" << "[getHddTemp]";
    if (debug) qDebug() << "[DE]" << "[getHddTemp]" << ":" << "Run function with cmd" << cmd;
    if (debug) qDebug() << "[DE]" << "[getHddTemp]" << ":" << "Run function with device" << device;
    float hddTemp = 0.0;
    QProcess command;
    QString qoutput = QString("");
    if (debug) qDebug() << "[DE]" << "[getHddTemp]" << ":" << "Run cmd" << cmd + QString(" ") + device;
    command.start(cmd + QString(" ") + device);
    command.waitForFinished(-1);
    if (debug) qDebug() << "[DE]" << "[getHddTemp]" << ":" << "Cmd returns" << command.exitCode();
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput()).trimmed();
    if (qoutput.split(QChar(':'), QString::SkipEmptyParts).count() >= 3) {
        QString temp = qoutput.split(QChar(':'), QString::SkipEmptyParts)[2];
        temp.remove(QChar(0260)).remove(QChar('C'));
        hddTemp = temp.toFloat();
    }
    if (debug) qDebug() << "[DE]" << "[getHddTemp]" << ":" << "Return" << hddTemp;
    return hddTemp;
}


QStringList ExtendedSysMon::getPlayerInfo(const QString playerName,
                                          const QString mpdAddress,
                                          const QString mpdPort)
{
    if (debug) qDebug() << "[DE]" << "[getPlayerInfo]";
    if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run function with player" << playerName;
    if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run function with MPD parameters" <<
        mpdAddress + QString(":") + mpdPort;
    QStringList info;
    // album
    info.append(QString("unknown"));
    // artist
    info.append(QString("unknown"));
    // progress
    info.append(QString("0"));
    // duration
    info.append(QString("0"));
    // title
    info.append(QString("unknown"));
    QProcess command;
    QString cmd;
    QString qoutput = QString("");
    QString qstr;
    if (playerName == QString("amarok")) {
        // amarok
        cmd = QString("qdbus org.kde.amarok /Player GetMetadata");
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run cmd" << cmd;
        command.start(cmd);
        command.waitForFinished(-1);
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Cmd returns" << command.exitCode();
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            qstr = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i];
            if (qstr.split(QString(": "), QString::SkipEmptyParts).count() > 1) {
                if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("album"))
                    info[0] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("artist"))
                    info[1] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("time"))
                    info[3] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("title"))
                    info[4] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
            }
        }
        cmd = QString("qdbus org.kde.amarok /Player PositionGet");
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run cmd" << cmd;
        command.start(cmd);
        command.waitForFinished(-1);
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Cmd returns" << command.exitCode();
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            qstr = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i];
            int time = qstr.toInt() / 1000;
            info[2] = QString::number(time);
        }
    }
    else if (playerName == QString("clementine")) {
        // clementine
        cmd = QString("qdbus org.mpris.clementine /Player GetMetadata");
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run cmd" << cmd;
        command.start(cmd);
        command.waitForFinished(-1);
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Cmd returns" << command.exitCode();
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            qstr = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i];
            if (qstr.split(QString(": "), QString::SkipEmptyParts).count() > 1) {
                if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("album"))
                    info[0] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("artist"))
                    info[1] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("time"))
                    info[3] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("title"))
                    info[4] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
            }
        }
        cmd = QString("qdbus org.mpris.clementine /Player PositionGet");
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run cmd" << cmd;
        command.start(cmd);
        command.waitForFinished(-1);
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Cmd returns" << command.exitCode();
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            qstr = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i];
            int time = qstr.toInt() / 1000;
            info[2] = QString::number(time);
        }
    }
    else if (playerName == QString("mpd")) {
        // mpd
        cmd = QString("bash -c \"echo 'currentsong\nstatus\nclose' | curl --connect-timeout 1 -fsm 3 telnet://") +
              mpdAddress + QString(":") + mpdPort + QString("\"");
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run cmd" << cmd;
        command.start(cmd);
        command.waitForFinished(-1);
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Cmd returns" << command.exitCode();
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            qstr = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i];
            if (qstr.split(QString(": "), QString::SkipEmptyParts).count() > 1) {
                if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("Album"))
                    info[0] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("Artist"))
                    info[1] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("time")) {
                    info[3] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed().split(QString(":"))[0];
                    info[2] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed().split(QString(":"))[1];
                }
                else if (qstr.split(QString(": "), QString::SkipEmptyParts)[0] == QString("Title"))
                    info[4] = qstr.split(QString(": "), QString::SkipEmptyParts)[1].trimmed();
            }
        }
    }
    else if (playerName == QString("qmmp")) {
        // qmmp
        cmd = QString("qmmp --status");
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run cmd" << cmd;
        command.start(cmd);
        command.waitForFinished(-1);
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Cmd returns" << command.exitCode();
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            qstr = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i];
            if ((qstr.split(QString(" = "), QString::SkipEmptyParts).count() > 1) || (qstr.at(0) == QChar('['))) {
                if (qstr.split(QString(" = "), QString::SkipEmptyParts)[0] == QString("ALBUM"))
                    info[0] = qstr.split(QString(" = "), QString::SkipEmptyParts)[1].trimmed();
                else if (qstr.split(QString(" = "), QString::SkipEmptyParts)[0] == QString("ARTIST"))
                    info[1] = qstr.split(QString(" = "), QString::SkipEmptyParts)[1].trimmed();
                else if ((qstr.at(0) == QChar('[')) && (!qstr.contains("[stopped]"))) {
                    QString time = qstr.split(QString(" "), QString::SkipEmptyParts)[2].trimmed();
                    info[2] = QString::number(time.split(QString("/"), QString::SkipEmptyParts)[0].split(QString(":"), QString::SkipEmptyParts)[0].toInt() * 60 +
                            time.split(QString("/"), QString::SkipEmptyParts)[0].split(QString(":"), QString::SkipEmptyParts)[1].toInt());
                    info[3] = QString::number(time.split(QString("/"), QString::SkipEmptyParts)[1].split(QString(":"), QString::SkipEmptyParts)[0].toInt() * 60 +
                            time.split(QString("/"), QString::SkipEmptyParts)[1].split(QString(":"), QString::SkipEmptyParts)[1].toInt());
                }
                else if (qstr.split(QString(" = "), QString::SkipEmptyParts)[0] == QString("TITLE"))
                    info[4] = qstr.split(QString(" = "), QString::SkipEmptyParts)[1].trimmed();
            }
        }
    }
    if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Return" << info;
    return info;
}


QStringList ExtendedSysMon::getPsStats()
{
    if (debug) qDebug() << "[DE]" << "[getPsStats]";
    int psCount = 0;
    QStringList psList;
    QString cmd;
    QProcess command;
    QString qoutput = QString("");
    cmd = QString("ps --no-headers -o command");
    if (debug) qDebug() << "[DE]" << "[getPsStats]" << ":" << "Run cmd" << cmd;
    command.start(cmd);
    command.waitForFinished(-1);
    if (debug) qDebug() << "[DE]" << "[getPsStats]" << ":" << "Cmd returns" << command.exitCode();
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput()).trimmed();
    for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++)
        if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i] != QString("ps --no-headers -o command")) {
            psCount++;
            psList.append(qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]);
        }
    QStringList psStats;
    psStats.append(QString::number(psCount));
    psStats.append(psList.join(QString(",")));
    cmd = QString("ps -e --no-headers -o command");
    if (debug) qDebug() << "[DE]" << "[getPsStats]" << ":" << "Run cmd" << cmd;
    command.start(cmd);
    command.waitForFinished(-1);
    if (debug) qDebug() << "[DE]" << "[getPsStats]" << ":" << "Cmd returns" << command.exitCode();
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput()).trimmed();
    int psTotal = qoutput.split(QChar('\n'), QString::SkipEmptyParts).count();
    psStats.append(QString::number(psTotal));
    if (debug) qDebug() << "[DE]" << "[getPsStats]" << ":" << "Return" << psStats;
    return psStats;
}


int ExtendedSysMon::getUpgradeInfo(const QString pkgCommand, const int pkgNull)
{
    if (debug) qDebug() << "[DE]" << "[getUpgradeInfo]";
    if (debug) qDebug() << "[DE]" << "[getUpgradeInfo]" << ":" << "Run function with cmd" << pkgCommand;
    if (debug) qDebug() << "[DE]" << "[getUpgradeInfo]" << ":" << "Run function with number of null lines" << pkgNull;
    int count = 0;
    QString cmd = QString("bash -c \"") + pkgCommand + QString("\"");
    QProcess command;
    QString qoutput = QString("");
    if (debug) qDebug() << "[DE]" << "[getUpgradeInfo]" << ":" << "Run cmd" << cmd;
    command.start(cmd);
    command.waitForFinished(-1);
    if (debug) qDebug() << "[DE]" << "[getUpgradeInfo]" << ":" << "Cmd returns" << command.exitCode();
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput()).trimmed();
    count = qoutput.split(QChar('\n'), QString::SkipEmptyParts).count();
    if (debug) qDebug() << "[DE]" << "[getUpgradeInfo]" << ":" << "Return" << (count - pkgNull);
    return (count - pkgNull);
}


bool ExtendedSysMon::sourceRequestEvent(const QString &name)
{
    if (debug) qDebug() << "[DE]" << "[sourceRequestEvent]";
    if (debug) qDebug() << "[DE]" << "[sourceRequestEvent]" << ":" << "Run function with source name" << name;
    return updateSourceEvent(name);
}


bool ExtendedSysMon::updateSourceEvent(const QString &source)
{
    if (debug) qDebug() << "[DE]" << "[updateSourceEvent]";
    if (debug) qDebug() << "[DE]" << "[updateSourceEvent]" << ":" << "Run function with source name" << source;
    QString key;
    if (source == QString("custom")) {
        for (int i=0; i<configuration[QString("CUSTOM")].split(QString("@@"), QString::SkipEmptyParts).count(); i++) {
            key = QString("custom") + QString::number(i);
            QString value = getCustomCmd(configuration[QString("CUSTOM")].split(QString("@@"), QString::SkipEmptyParts)[i]);
            setData(source, key, value);
        }
    }
    else if (source == QString("gpu")) {
        key = QString("GPU");
        float value = getGpu(configuration[QString("GPUDEV")]);
        setData(source, key, value);
    }
    else if (source == QString("gputemp")) {
        key = QString("GPUTemp");
        float value = getGpuTemp(configuration[QString("GPUDEV")]);
        setData(source, key, value);
    }
    else if (source == QString("hddtemp")) {
        QStringList deviceList = configuration[QString("HDDDEV")].split(QChar(','), QString::SkipEmptyParts);
        for (int i=0; i<deviceList.count(); i++) {
            float value = getHddTemp(configuration[QString("HDDTEMPCMD")],
                                     deviceList[i]);
            setData(source, deviceList[i], value);
        }
    }
    else if (source == QString("pkg")) {
        for (int i=0; i<configuration[QString("PKGCMD")].split(QString(","), QString::SkipEmptyParts).count(); i++) {
            key = QString("pkgCount") + QString::number(i);
            int value = getUpgradeInfo(configuration[QString("PKGCMD")].split(QString(","), QString::SkipEmptyParts)[i],
                    configuration[QString("PKGNULL")].split(QString(","), QString::SkipEmptyParts)[i].toInt());
            setData(source, key, value);
        }
    }
    else if (source == QString("player")) {
        QStringList value;
        value = getPlayerInfo(configuration[QString("PLAYER")],
                              configuration[QString("MPDADDRESS")],
                              configuration[QString("MPDPORT")]);
        key = QString("album");
        setData(source, key, value[0]);
        key = QString("artist");
        setData(source, key, value[1]);
        key = QString("progress");
        setData(source, key, value[2]);
        key = QString("duration");
        setData(source, key, value[3]);
        key = QString("title");
        setData(source, key, value[4]);
    }
    else if (source == QString("ps")) {
        QStringList value = getPsStats();
        key = QString("psCount");
        setData(source, key, value[0].toInt());
        key = QString("ps");
        setData(source, key, value[1]);
        key = QString("psTotal");
        setData(source, key, value[2].toInt());
    }
    return true;
}


K_EXPORT_PLASMA_DATAENGINE(extsysmon, ExtendedSysMon)

#include "extsysmon.moc"
