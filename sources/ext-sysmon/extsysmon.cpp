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
#include <QFile>
#include <QProcess>
#include <QTextCodec>


ExtendedSysMon::ExtendedSysMon(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)

    setMinimumPollingInterval(333);
    readConfiguration();
}


QString ExtendedSysMon::getAllHdd()
{
    QProcess command;
    QStringList devices;
    QString qoutput = QString("");
    QString dev;
    command.start("find /dev -name [hs]d[a-z]");
    command.waitForFinished(-1);
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
    for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
        dev = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i];
        devices.append(dev);
    }
    return devices.join(QChar(','));
}


QString ExtendedSysMon::getAutoGpu()
{
    QProcess command;
    QString gpu = QString("");
    QString qoutput = QString("");
    command.start("lspci");
    command.waitForFinished(-1);
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
    if (qoutput.toLower().contains("nvidia"))
        gpu = QString("nvidia");
    else if (qoutput.toLower().contains("radeon"))
        gpu = QString("ati");
    return gpu;
}


QStringList ExtendedSysMon::sources() const
{
    QStringList source;
    source.append(QString("custom"));
    source.append(QString("gpu"));
    source.append(QString("gputemp"));
    source.append(QString("hddtemp"));
    source.append(QString("pkg"));
    source.append(QString("player"));
    source.append(QString("ps"));
    return source;
}


void ExtendedSysMon::readConfiguration()
{
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

    QString fileName = KGlobal::dirs()->findResource("config", "extsysmon.conf");
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
    return;
}


QMap<QString, QString> ExtendedSysMon::updateConfiguration(const QMap<QString, QString> rawConfig)
{
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
    if (config[QString("GPUDEV")] == QString("auto"))
        config[QString("GPUDEV")] = getAutoGpu();
    if (config[QString("HDDDEV")] == QString("all"))
        config[QString("HDDDEV")] = getAllHdd();
    for (int i=config[QString("PKGNULL")].split(QString(","), QString::SkipEmptyParts).count();
         i<config[QString("PKGCMD")].split(QString(","), QString::SkipEmptyParts).count()+1;
         i++)
        config[QString("PKGNULL")] += QString(",0");
    return config;
}


QString ExtendedSysMon::getCustomCmd(const QString cmd)
{
    QProcess command;
    QString qoutput = QString("");
    command.start(QString("bash -c \"") + cmd + QString("\""));
    command.waitForFinished(-1);
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput()).trimmed();
    return qoutput;
}


float ExtendedSysMon::getGpu(const QString device)
{
    float gpu = 0.0;
    if ((device != QString("nvidia")) && (device != QString("ati")))
        return gpu;
    QProcess command;
    QString qoutput;

    if (device == QString("nvidia")) {
        command.start(QString("nvidia-smi -q -d UTILIZATION"));
        command.waitForFinished(-1);
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
        command.start(QString("aticonfig --od-getclocks"));
        command.waitForFinished(-1);
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
    return gpu;
}


float ExtendedSysMon::getGpuTemp(const QString device)
{
    float gpuTemp = 0.0;
    if ((device != QString("nvidia")) && (device != QString("ati")))
        return gpuTemp;
    QProcess command;
    QString qoutput;
    if (device == QString("nvidia")) {
        command.start(QString("nvidia-smi -q -d TEMPERATURE"));
        command.waitForFinished(-1);
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
        command.start(QString("aticonfig --od-gettemperature"));
        command.waitForFinished(-1);
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i].contains(QString("Temperature"))) {
                QString temp = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]
                        .split(QChar(' '), QString::SkipEmptyParts)[4];
                gpuTemp = temp.toFloat();
            }
        }
    }
    return gpuTemp;
}


float ExtendedSysMon::getHddTemp(const QString cmd, const QString device)
{
    float hddTemp = 0.0;
    QProcess command;
    QString qoutput = QString("");
    command.start(cmd + QString(" ") + device);
    command.waitForFinished(-1);
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput()).trimmed();
    if (qoutput.split(QChar(':'), QString::SkipEmptyParts).count() >= 3) {
        QString temp = qoutput.split(QChar(':'), QString::SkipEmptyParts)[2];
        temp.remove(QChar(0260)).remove(QChar('C'));
        hddTemp = temp.toFloat();
    }
    return hddTemp;
}


QStringList ExtendedSysMon::getPlayerInfo(const QString playerName,
                                          const QString mpdAddress,
                                          const QString mpdPort)
{
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
    if ((playerName != QString("amarok")) &&
            (playerName != QString("mpd")) &&
            (playerName != QString("qmmp")))
        return info;
    QProcess command;
    QString qoutput = QString("");
    QString qstr;
    if (playerName == QString("amarok")) {
        // amarok
        command.start("qdbus org.kde.amarok /Player GetMetadata");
        command.waitForFinished(-1);
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
        command.start("qdbus org.kde.amarok /Player PositionGet");
        command.waitForFinished(-1);
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            qstr = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i];
            int time = qstr.toInt() / 1000;
            info[2] = QString::number(time);
        }
    }
    else if (playerName == QString("clementine")) {
        // clementine
        command.start("qdbus org.kde.clementine /Player GetMetadata");
        command.waitForFinished(-1);
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
        command.start("qdbus org.kde.clementine /Player PositionGet");
        command.waitForFinished(-1);
        qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
        for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++) {
            qstr = qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i];
            int time = qstr.toInt() / 1000;
            info[2] = QString::number(time);
        }
    }
    else if (playerName == QString("mpd")) {
        // mpd
        command.start(QString("bash -c \"echo 'currentsong\nstatus\nclose' | curl --connect-timeout 1 -fsm 3 telnet://") +
                      mpdAddress + QString(":") + mpdPort + QString("\""));
        command.waitForFinished(-1);
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
        command.start("qmmp --status");
        command.waitForFinished(-1);
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
    return info;
}


QStringList ExtendedSysMon::getPsStats()
{
    int psCount = 0;
    QStringList psList;
    QProcess command;
    QString qoutput = QString("");
    command.start(QString("ps --no-headers -o command"));
    command.waitForFinished(-1);
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput()).trimmed();
    for (int i=0; i<qoutput.split(QChar('\n'), QString::SkipEmptyParts).count(); i++)
        if (qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i] != QString("ps --no-headers -o command")) {
            psCount++;
            psList.append(qoutput.split(QChar('\n'), QString::SkipEmptyParts)[i]);
        }
    QStringList psStats;
    psStats.append(QString::number(psCount));
    psStats.append(psList.join(QString(",")));
    command.start(QString("ps -e --no-headers -o command"));
    command.waitForFinished(-1);
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput()).trimmed();
    int psTotal = qoutput.split(QChar('\n'), QString::SkipEmptyParts).count();
    psStats.append(QString::number(psTotal));
    return psStats;
}


int ExtendedSysMon::getUpgradeInfo(const QString pkgCommand, const int pkgNull)
{
    int count = 0;
    QProcess command;
    QString qoutput = QString("");
    command.start(QString("bash -c \"") + pkgCommand + QString("\""));
    command.waitForFinished(-1);
    qoutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput()).trimmed();
    count = qoutput.split(QChar('\n'), QString::SkipEmptyParts).count();
    return (count - pkgNull);
}


bool ExtendedSysMon::sourceRequestEvent(const QString &name)
{
    return updateSourceEvent(name);
}


bool ExtendedSysMon::updateSourceEvent(const QString &source)
{
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
        // amarok
        value = getPlayerInfo(QString("amarok"));
        key = QString("amarok_album");
        setData(source, key, value[0]);
        key = QString("amarok_artist");
        setData(source, key, value[1]);
        key = QString("amarok_progress");
        setData(source, key, value[2]);
        key = QString("amarok_duration");
        setData(source, key, value[3]);
        key = QString("amarok_title");
        setData(source, key, value[4]);
        // clementine
        value = getPlayerInfo(QString("clementine"));
        key = QString("clementine_album");
        setData(source, key, value[0]);
        key = QString("clementine_artist");
        setData(source, key, value[1]);
        key = QString("clementine_progress");
        setData(source, key, value[2]);
        key = QString("clementine_duration");
        setData(source, key, value[3]);
        key = QString("clementine_title");
        setData(source, key, value[4]);
        // mpd
        value = getPlayerInfo(QString("mpd"),
                              configuration[QString("MPDADDRESS")],
                              configuration[QString("MPDPORT")]);
        key = QString("mpd_album");
        setData(source, key, value[0]);
        key = QString("mpd_artist");
        setData(source, key, value[1]);
        key = QString("mpd_progress");
        setData(source, key, value[2]);
        key = QString("mpd_duration");
        setData(source, key, value[3]);
        key = QString("mpd_title");
        setData(source, key, value[4]);
        // qmmp
        value = getPlayerInfo(QString("qmmp"));
        key = QString("qmmp_album");
        setData(source, key, value[0]);
        key = QString("qmmp_artist");
        setData(source, key, value[1]);
        key = QString("qmmp_progress");
        setData(source, key, value[2]);
        key = QString("qmmp_duration");
        setData(source, key, value[3]);
        key = QString("qmmp_title");
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
