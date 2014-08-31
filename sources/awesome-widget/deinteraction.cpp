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

#include "awesome-widget.h"

#include <QRegExp>

#include <pdebug/pdebug.h>


void AwesomeWidget::connectToEngine()
{
    if (debug) qDebug() << PDEBUG;
    QRegExp regExp;

    // cpu
    regExp = QRegExp(QString("cpu.*"));
    if (foundKeys.indexOf(regExp) > -1) {
        sysmonEngine->connectSource(QString("cpu/system/TotalLoad"),
                                    this, configuration[QString("interval")].toInt());
        int numCpus = getNumberCpus();
        for (int i=0; i<numCpus; i++)
            sysmonEngine->connectSource(QString("cpu/cpu") + QString::number(i) + QString("/TotalLoad"),
                                        this, configuration[QString("interval")].toInt());
    }
    // cpuclock
    regExp = QRegExp(QString("cpucl.*"));
    if (foundKeys.indexOf(regExp) > -1) {
        sysmonEngine->connectSource(QString("cpu/system/AverageClock"),
                                    this, configuration[QString("interval")].toInt());
        int numCpus = getNumberCpus();
        for (int i=0; i<numCpus; i++)
            sysmonEngine->connectSource(QString("cpu/cpu") + QString::number(i) + QString("/clock"),
                                        this, configuration[QString("interval")].toInt());
    }
    // custom command
    regExp = QRegExp(QString("custom.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("custom"),
                                       this, configuration[QString("interval")].toInt());
    // desktop
    regExp = QRegExp(QString(".*desktop.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("desktop"),
                                       this, configuration[QString("interval")].toInt());
    // disk speed
    regExp = QRegExp(QString("hdd[rw].*"));
    if (foundKeys.indexOf(regExp) > -1)
        for (int i=0; i<configuration[QString("disk")].split(QString("@@")).count(); i++) {
            sysmonEngine->connectSource(configuration[QString("disk")].split(QString("@@"))[i] + QString("/Rate/rblk"),
                                        this, configuration[QString("interval")].toInt());
            sysmonEngine->connectSource(configuration[QString("disk")].split(QString("@@"))[i] + QString("/Rate/wblk"),
                                        this, configuration[QString("interval")].toInt());
        }
    // gpu
    regExp = QRegExp(QString("gpu"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("gpu"),
                                       this, configuration[QString("interval")].toInt());
    // gputemp
    regExp = QRegExp(QString("gputemp"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("gputemp"),
                                       this, configuration[QString("interval")].toInt());
    // mount
    regExp = QRegExp(QString("hdd([0-9]|mb|gb|totmb|totgb).*"));
    if (foundKeys.indexOf(regExp) > -1)
        for (int i=0; i<configuration[QString("mount")].split(QString("@@")).count(); i++) {
            sysmonEngine->connectSource(QString("partitions") + configuration[QString("mount")].split(QString("@@"))[i] + QString("/filllevel"),
                                        this, configuration[QString("interval")].toInt());
            sysmonEngine->connectSource(QString("partitions") + configuration[QString("mount")].split(QString("@@"))[i] + QString("/freespace"),
                                        this, configuration[QString("interval")].toInt());
            sysmonEngine->connectSource(QString("partitions") + configuration[QString("mount")].split(QString("@@"))[i] + QString("/usedspace"),
                                        this, configuration[QString("interval")].toInt());
        }
    // hddtemp
    regExp = QRegExp(QString("hddtemp.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("hddtemp"),
                                       this, configuration[QString("interval")].toInt());
    // memory
    regExp = QRegExp(QString("mem.*"));
    if (foundKeys.indexOf(regExp) > -1) {
        sysmonEngine->connectSource(QString("mem/physical/free"),
                                    this, configuration[QString("interval")].toInt());
        sysmonEngine->connectSource(QString("mem/physical/used"),
                                    this, configuration[QString("interval")].toInt());
        sysmonEngine->connectSource(QString("mem/physical/application"),
                                    this, configuration[QString("interval")].toInt());
    }
    // network
    regExp = QRegExp(QString("(down|up|netdev)"));
    if (foundKeys.indexOf(regExp) > -1) {
        sysmonEngine->connectSource(QString("network/interfaces/") + values[QString("netdev")] + QString("/transmitter/data"),
                                    this, configuration[QString("interval")].toInt());
        sysmonEngine->connectSource(QString("network/interfaces/") + values[QString("netdev")] + QString("/receiver/data"),
                                    this, configuration[QString("interval")].toInt());
    }
    // package manager
    regExp = QRegExp(QString("pkgcount.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("pkg"),
                                       this, 60*60*1000, Plasma::AlignToHour);
    // player
    regExp = QRegExp(QString("(album|artist|duration|progress|title)"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("player"),
                                       this, configuration[QString("interval")].toInt());
    // ps
    regExp = QRegExp(QString("ps.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("ps"),
                                       this, configuration[QString("interval")].toInt());
    // swap
    regExp = QRegExp(QString("swap.*"));
    if (foundKeys.indexOf(regExp) > -1) {
        sysmonEngine->connectSource(QString("mem/swap/free"),
                                    this, configuration[QString("interval")].toInt());
        sysmonEngine->connectSource(QString("mem/swap/used"),
                                    this, configuration[QString("interval")].toInt());
    }
    // temp
    regExp = QRegExp(QString("temp.*"));
    if (foundKeys.indexOf(regExp) > -1)
        for (int i=0; i<configuration[QString("tempDevice")].split(QString("@@")).count(); i++)
            sysmonEngine->connectSource(configuration[QString("tempDevice")].split(QString("@@"))[i],
                                        this, configuration[QString("interval")].toInt());
    // time
    regExp = QRegExp(QString("(^|iso|short|long|c)time"));
    if (foundKeys.indexOf(regExp) > -1)
        timeEngine->connectSource(QString("Local"),
                                  this, 1000);
    // uptime
    regExp = QRegExp(QString("(^|c)uptime"));
    if (foundKeys.indexOf(regExp) > -1)
        sysmonEngine->connectSource(QString("system/uptime"),
                                    this, configuration[QString("interval")].toInt());
}


void AwesomeWidget::dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Run function with source name" << sourceName;

    if (data.keys().count() == 0)
        return;
//    if (sourceName == QString("desktop")) {
//        currentDesktop = data[QString("currentNumber")].toInt();
//        if (desktopNames.isEmpty()) {
//            desktopNames = data[QString("list")].toString().split(QString(";;"));
//            reinit();
//        }
//        updateText();
//    }
}


void AwesomeWidget::disconnectFromEngine()
{
    if (debug) qDebug() << PDEBUG;
}
