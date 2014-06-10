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
    setProcesses();
    initValues();
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


void ExtendedSysMon::initValues()
{
    QStringList sourceList = sources();
    for (int i=0; i<sourceList.count(); i++)
        updateSourceEvent(sourceList[i]);
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


void ExtendedSysMon::setProcesses()
{
    if (debug) qDebug() << "[DE]" << "[setProcesses]";
    // custom
    for (int i=0; i<configuration[QString("CUSTOM")].split(QString("@@"), QString::SkipEmptyParts).count(); i++) {
        processes[QString("custom")].append(new QProcess);
        connect(processes[QString("custom")][i], SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(setCustomCmd(int, QProcess::ExitStatus)));
    }
    // gpu
    processes[QString("gpu")].append(new QProcess);
    connect(processes[QString("gpu")][0], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setGpu(int, QProcess::ExitStatus)));
    // gputemp
    processes[QString("gputemp")].append(new QProcess);
    connect(processes[QString("gputemp")][0], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setGpuTemp(int, QProcess::ExitStatus)));
    // hddtemp
    for (int i=0; i<configuration[QString("HDDDEV")].split(QChar(','), QString::SkipEmptyParts).count(); i++) {
        processes[QString("hddtemp")].append(new QProcess);
        connect(processes[QString("hddtemp")][i], SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(setHddTemp(int, QProcess::ExitStatus)));
    }
    // pkg
    for (int i=0; i<configuration[QString("PKGCMD")].split(QString(","), QString::SkipEmptyParts).count(); i++) {
        processes[QString("pkg")].append(new QProcess);
        connect(processes[QString("pkg")][i], SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(setUpgradeInfo(int, QProcess::ExitStatus)));
    }
    // player
    processes[QString("player")].append(new QProcess);
    connect(processes[QString("player")][0], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setPlayer(int, QProcess::ExitStatus)));
    // ps
    // pscount && ps
    processes[QString("ps")].append(new QProcess);
    // pstotal
    processes[QString("ps")].append(new QProcess);
    connect(processes[QString("ps")][1], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setPs(int, QProcess::ExitStatus)));
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
        if ((key != QString("CUSTOM")) &&
            (key != QString("HDDTEMPCMD")) &&
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


void ExtendedSysMon::getCustomCmd(const QString cmd, const int number)
{
    if (debug) qDebug() << "[DE]" << "[getCustomCmd]";
    if (debug) qDebug() << "[DE]" << "[getCustomCmd]" << ":" << "Run function with cmd" << cmd;
    if (debug) qDebug() << "[DE]" << "[getCustomCmd]" << ":" << "Run function with number" << number;
    if (debug) qDebug() << "[DE]" << "[getCustomCmd]" << ":" << "Run cmd" << QString("bash -c \"") + cmd + QString("\"");
    processes[QString("custom")][number]->start(QString("bash -c \"") + cmd + QString("\""));
}


void ExtendedSysMon::setCustomCmd(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    if (debug) qDebug() << "[DE]" << "[setCustomCmd]";
    if (debug) qDebug() << "[DE]" << "[setCustomCmd]" << ":" << "Cmd returns" << exitCode;
    for (int i=0; i<processes[QString("custom")].count(); i++) {
        QString value = QString("");
        value = QTextCodec::codecForMib(106)->toUnicode(processes[QString("custom")][i]->readAllStandardOutput()).trimmed();
        if (!value.isEmpty()) {
            if (debug) qDebug() << "[DE]" << "[setCustomCmd]" << ":" << "Found data for cmd" << i;
            if (debug) qDebug() << "[DE]" << "[setCustomCmd]" << ":" << "Return" << value;
            QString source = QString("custom");
            QString key = QString("custom") + QString::number(i);
            setData(source, key, value);
        }
    }
}


void ExtendedSysMon::getGpu(const QString device)
{
    if (debug) qDebug() << "[DE]" << "[getGpu]";
    if (debug) qDebug() << "[DE]" << "[getGpu]" << ":" << "Run function with device" << device;
    if ((device != QString("nvidia")) && (device != QString("ati")))
        return;
    QString cmd = QString("");
    if (device == QString("nvidia"))
        cmd = QString("nvidia-smi -q -d UTILIZATION");
    else if (device == QString("ati"))
        cmd = QString("aticonfig --od-getclocks");
    if (debug) qDebug() << "[DE]" << "[getGpu]" << ":" << "Run cmd" << cmd;
    processes[QString("gpu")][0]->start(cmd);
}


void ExtendedSysMon::setGpu(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    if (debug) qDebug() << "[DE]" << "[setGpu]";
    if (debug) qDebug() << "[DE]" << "[setGpu]" << ":" << "Cmd returns" << exitCode;
    float value = 0.0;
    QString qoutput;
    if (configuration[QString("GPUDEV")] == QString("nvidia")) {
        qoutput = QTextCodec::codecForMib(106)->toUnicode(processes[QString("gpu")][0]->readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("Gpu"))) {
                QString load = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .split(QChar(' '), QString::SkipEmptyParts)[2]
                        .remove(QChar('%'));
                value = load.toFloat();
            }
        }
    }
    else if (configuration[QString("GPUDEV")] == QString("ati")) {
        qoutput = QTextCodec::codecForMib(106)->toUnicode(processes[QString("gpu")][0]->readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("load"))) {
                QString load = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .split(QChar(' '), QString::SkipEmptyParts)[3]
                        .remove(QChar('%'));
                value = load.toFloat();
            }
        }
    }
    if (debug) qDebug() << "[DE]" << "[setGpu]" << ":" << "Return" << value;
    QString source = QString("gpu");
    QString key = QString("GPU");
    setData(source, key, value);
}


void ExtendedSysMon::getGpuTemp(const QString device)
{
    if (debug) qDebug() << "[DE]" << "[getGpuTemp]";
    if (debug) qDebug() << "[DE]" << "[getGpuTemp]" << ":" << "Run function with device" << device;
    if ((device != QString("nvidia")) && (device != QString("ati")))
        return;
    QString cmd = QString("");
    if (device == QString("nvidia"))
        cmd = QString("nvidia-smi -q -d TEMPERATURE");
    else if (device == QString("ati"))
        cmd = QString("aticonfig --od-gettemperature");
    if (debug) qDebug() << "[DE]" << "[getGpuTemp]" << ":" << "Run cmd" << cmd;
    processes[QString("gputemp")][0]->start(cmd);
}


void ExtendedSysMon::setGpuTemp(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    if (debug) qDebug() << "[DE]" << "[setGpuTemp]";
    if (debug) qDebug() << "[DE]" << "[setGpuTemp]" << ":" << "Cmd returns" << exitCode;
    float value = 0.0;
    QString qoutput;
    if (configuration[QString("GPUDEV")] == QString("nvidia")) {
        qoutput = QTextCodec::codecForMib(106)->toUnicode(processes[QString("gputemp")][0]->readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("Gpu"))) {
                QString temp = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .split(QChar(' '), QString::SkipEmptyParts)[2];
                value = temp.toFloat();
            }
        }
    }
    else if (configuration[QString("GPUDEV")] == QString("ati")) {
        qoutput = QTextCodec::codecForMib(106)->toUnicode(processes[QString("gputemp")][0]->readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("Temperature"))) {
                QString temp = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .split(QChar(' '), QString::SkipEmptyParts)[4];
                value = temp.toFloat();
            }
        }
    }
    if (debug) qDebug() << "[DE]" << "[setGpuTemp]" << ":" << "Return" << value;
    QString source = QString("gputemp");
    QString key = QString("GPUTemp");
    setData(source, key, value);
}


void ExtendedSysMon::getHddTemp(const QString cmd, const QString device, const int number)
{
    if (debug) qDebug() << "[DE]" << "[getHddTemp]";
    if (debug) qDebug() << "[DE]" << "[getHddTemp]" << ":" << "Run function with cmd" << cmd;
    if (debug) qDebug() << "[DE]" << "[getHddTemp]" << ":" << "Run function with device" << device;
    if (debug) qDebug() << "[DE]" << "[getHddTemp]" << ":" << "Run function with number" << number;
    if (debug) qDebug() << "[DE]" << "[getHddTemp]" << ":" << "Run cmd" << cmd + QString(" ") + device;
    processes[QString("hddtemp")][number]->start(cmd + QString(" ") + device);
}


void ExtendedSysMon::setHddTemp(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    if (debug) qDebug() << "[DE]" << "[setHddTemp]";
    if (debug) qDebug() << "[DE]" << "[setHddTemp]" << ":" << "Cmd returns" << exitCode;
    for (int i=0; i<processes[QString("hddtemp")].count(); i++) {
        float value = 0.0;
        QString qoutput = QString("");
        qoutput = QTextCodec::codecForMib(106)->toUnicode(processes[QString("hddtemp")][i]->readAllStandardOutput()).trimmed();
        if (qoutput.split(QChar(':'), QString::SkipEmptyParts).count() >= 3) {
            if (debug) qDebug() << "[DE]" << "[setHddTemp]" << ":" << "Found data for cmd" << i;
            QString temp = qoutput.split(QChar(':'), QString::SkipEmptyParts)[2];
            temp.remove(QChar(0260)).remove(QChar('C'));
            value = temp.toFloat();
            if (debug) qDebug() << "[DE]" << "[setHddTemp]" << ":" << "Return" << value;
            QString source = QString("hddtemp");
            QString key = configuration[QString("HDDDEV")].split(QChar(','), QString::SkipEmptyParts)[i];
            setData(source, key, value);
        }
    }
}


void ExtendedSysMon::getPlayerInfo(const QString playerName,
                                   const QString mpdAddress,
                                   const QString mpdPort)
{
    if (debug) qDebug() << "[DE]" << "[getPlayerInfo]";
    if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run function with player" << playerName;
    if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run function with MPD parameters" <<
        mpdAddress + QString(":") + mpdPort;
    QString cmd;
    if (playerName == QString("amarok")) {
        // amarok
        cmd = QString("bash -c \"qdbus org.kde.amarok /Player GetMetadata && qdbus org.kde.amarok /Player PositionGet\"");
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run cmd" << cmd;
        processes[QString("player")][0]->start(cmd);
    }
    else if (playerName == QString("clementine")) {
        // clementine
        cmd = QString("bash -c \"qdbus org.mpris.clementine /Player GetMetadata && qdbus org.mpris.clementine /Player PositionGet\"");
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run cmd" << cmd;
        processes[QString("player")][0]->start(cmd);
    }
    else if (playerName == QString("mpd")) {
        // mpd
        cmd = QString("bash -c \"echo 'currentsong\nstatus\nclose' | curl --connect-timeout 1 -fsm 3 telnet://") +
              mpdAddress + QString(":") + mpdPort + QString("\"");
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run cmd" << cmd;
        processes[QString("player")][0]->start(cmd);
    }
    else if (playerName == QString("qmmp")) {
        // qmmp
        cmd = QString("qmmp --status");
        if (debug) qDebug() << "[DE]" << "[getPlayerInfo]" << ":" << "Run cmd" << cmd;
        processes[QString("player")][0]->start(cmd);
    }
}


void ExtendedSysMon::setPlayer(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    if (debug) qDebug() << "[DE]" << "[setPlayer]";
    if (debug) qDebug() << "[DE]" << "[setPlayer]" << ":" << "Cmd returns" << exitCode;
    QString playerName = configuration[QString("PLAYER")];
    QString qoutput = QString("");
    QString qstr = QString("");
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
    if (playerName == QString("amarok")) {
        qoutput = QTextCodec::codecForMib(106)->toUnicode(processes[QString("player")][0]->readAllStandardOutput());
        if (!qoutput.isEmpty()) {
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
                else {
                    int time = qstr.toInt() / 1000;
                    info[2] = QString::number(time);
                }
            }
        }
    }
    else if (playerName == QString("clementine")) {
        qoutput = QTextCodec::codecForMib(106)->toUnicode(processes[QString("player")][0]->readAllStandardOutput());
        if (!qoutput.isEmpty()) {
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
                else {
                    int time = qstr.toInt() / 1000;
                    info[2] = QString::number(time);
                }
            }
        }
    }
    else if (playerName == QString("mpd")) {
        qoutput = QTextCodec::codecForMib(106)->toUnicode(processes[QString("player")][0]->readAllStandardOutput());
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
        qoutput = QTextCodec::codecForMib(106)->toUnicode(processes[QString("player")][0]->readAllStandardOutput());
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
    QString source = QString("player");
    QString key;
    key = QString("album");
    setData(source, key, info[0]);
    key = QString("artist");
    setData(source, key, info[1]);
    key = QString("progress");
    setData(source, key, info[2]);
    key = QString("duration");
    setData(source, key, info[3]);
    key = QString("title");
    setData(source, key, info[4]);
}


void ExtendedSysMon::getPsStats()
{
    if (debug) qDebug() << "[DE]" << "[getPsStats]";
    QString cmd;
    cmd = QString("ps --no-headers -o command");
    if (debug) qDebug() << "[DE]" << "[getPsStats]" << ":" << "Run cmd" << cmd;
    processes[QString("ps")][0]->start(cmd);
    cmd = QString("ps -e --no-headers -o command");
    if (debug) qDebug() << "[DE]" << "[getPsStats]" << ":" << "Run cmd" << cmd;
    processes[QString("ps")][1]->start(cmd);
}


void ExtendedSysMon::setPs(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    if (debug) qDebug() << "[DE]" << "[setPs]";
    if (debug) qDebug() << "[DE]" << "[setPs]" << ":" << "Cmd returns" << exitCode;
    QString qoutput = QString("");
    for (int i=0; i<processes[QString("ps")].count(); i++) {
        qoutput = QTextCodec::codecForMib(106)->toUnicode(processes[QString("ps")][i]->readAllStandardOutput()).trimmed();
        if (!qoutput.isEmpty()) {
            if (debug) qDebug() << "[DE]" << "[setPs]" << ":" << "Found data for cmd" << i;
            if (i == 0) {
                // pscount && ps
                QStringList psList;
                for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++)
                    if (!qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].
                        contains(QString("ps ")))
                        psList.append(qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]);
                QString source = QString("ps");
                QString key = QString("psCount");
                setData(source, key, psList.count());
                key = QString("ps");
                setData(source, key, psList.join(QString(",")));
            }
            else if (i == 1) {
                // pstotal
                QString source = QString("ps");
                QString key = QString("psTotal");
                setData(source, key, qoutput.split(QChar('\n'), QString::SkipEmptyParts).count());
            }
        }
    }
}


void ExtendedSysMon::getUpgradeInfo(const QString pkgCommand, const int number)
{
    if (debug) qDebug() << "[DE]" << "[getUpgradeInfo]";
    if (debug) qDebug() << "[DE]" << "[getUpgradeInfo]" << ":" << "Run function with cmd" << pkgCommand;
    if (debug) qDebug() << "[DE]" << "[getUpgradeInfo]" << ":" << "Run function with number" << number;
    QString cmd = QString("bash -c \"") + pkgCommand + QString(" | wc -l\"");
    if (debug) qDebug() << "[DE]" << "[getUpgradeInfo]" << ":" << "Run cmd" << cmd;
    processes[QString("pkg")][number]->start(cmd);
}


void ExtendedSysMon::setUpgradeInfo(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    if (debug) qDebug() << "[DE]" << "[setUpgradeInfo]";
    if (debug) qDebug() << "[DE]" << "[setUpgradeInfo]" << ":" << "Cmd returns" << exitCode;
    for (int i=0; i<processes[QString("pkg")].count(); i++) {
        int pkgNull = 0;
        int value = 0;
        QString qoutput = QString("");
        qoutput = QTextCodec::codecForMib(106)->toUnicode(processes[QString("pkg")][i]->readAllStandardOutput()).trimmed();
        if (!qoutput.isEmpty()) {
            if (debug) qDebug() << "[DE]" << "[setUpgradeInfo]" << ":" << "Found data for cmd" << i;
            pkgNull = configuration[QString("PKGNULL")].split(QString(","), QString::SkipEmptyParts)[i].toInt();
            value = qoutput.toInt();
            if (debug) qDebug() << "[DE]" << "[setUpgradeInfo]" << ":" << "Return" << value;
            QString source = QString("pkg");
            QString key = QString("pkgCount") + QString::number(i);
            setData(source, key, value - pkgNull);
        }
    }
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
    if (source == QString("custom")) {
        for (int i=0; i<configuration[QString("CUSTOM")].split(QString("@@"), QString::SkipEmptyParts).count(); i++)
            getCustomCmd(configuration[QString("CUSTOM")].split(QString("@@"), QString::SkipEmptyParts)[i], i);
    }
    else if (source == QString("gpu")) {
        getGpu(configuration[QString("GPUDEV")]);
    }
    else if (source == QString("gputemp")) {
        getGpuTemp(configuration[QString("GPUDEV")]);
    }
    else if (source == QString("hddtemp")) {
        QStringList deviceList = configuration[QString("HDDDEV")].split(QChar(','), QString::SkipEmptyParts);
        for (int i=0; i<deviceList.count(); i++)
            getHddTemp(configuration[QString("HDDTEMPCMD")], deviceList[i], i);
    }
    else if (source == QString("pkg")) {
        for (int i=0; i<configuration[QString("PKGCMD")].split(QString(","), QString::SkipEmptyParts).count(); i++)
            getUpgradeInfo(configuration[QString("PKGCMD")].split(QString(","), QString::SkipEmptyParts)[i], i);
    }
    else if (source == QString("player")) {
        getPlayerInfo(configuration[QString("PLAYER")],
                      configuration[QString("MPDADDRESS")],
                      configuration[QString("MPDPORT")]);
    }
    else if (source == QString("ps")) {
        getPsStats();
    }
    return true;
}


K_EXPORT_PLASMA_DATAENGINE(extsysmon, ExtendedSysMon)

#include "extsysmon.moc"
