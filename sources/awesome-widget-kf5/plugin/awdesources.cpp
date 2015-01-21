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

#include "awdesources.h"

#include <QDebug>
#include <QProcessEnvironment>
#include <QRegExp>

#include <pdebug/pdebug.h>


AWDESources::AWDESources(QObject *parent, const QString pattern,
                         const QStringList foundKeys,
                         const QStringList foundBars,
                         const QMap<QString, QVariant> counts,
                         const QMap<QString, QVariant> paths,
                         const QMap<QString, QVariant> tooltipBools)
    : QObject(parent),
      m_pattern(pattern),
      m_foundKeys(foundKeys),
      m_foundBars(foundBars),
      m_counts(counts),
      m_paths(paths),
      m_tooltipBools(tooltipBools)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));
}


AWDESources::~AWDESources()
{
    if (debug) qDebug() << PDEBUG;
}


QStringList AWDESources::getSourcesForExtSystemMonitor()
{
    if (debug) qDebug() << PDEBUG;

    QStringList sources;
    QRegExp regExp;

    // battery
    regExp = QRegExp(QString("(^|bar[0-9].*)(ac|bat.*)"));
    if ((m_foundKeys.indexOf(regExp) > -1) ||
        (m_foundBars.indexOf(regExp) > -1) ||
        (m_tooltipBools[QString("batteryTooltip")].toBool()))
        sources.append(QString("battery"));
    // custom command
    regExp = QRegExp(QString("custom.*"));
    if (m_foundKeys.indexOf(regExp) > -1)
        sources.append(QString("custom"));
    // desktop
    regExp = QRegExp(QString(".*desktop.*"));
    if (m_foundKeys.indexOf(regExp) > -1)
        sources.append(QString("desktop"));
    // gpu
    regExp = QRegExp(QString("(^|bar[0-9].*)gpu"));
    if ((m_foundKeys.indexOf(regExp) > -1) ||
        (m_foundBars.indexOf(regExp) > -1))
        sources.append(QString("gpu"));
    // gpu temp
    regExp = QRegExp(QString("gputemp"));
    if (m_foundKeys.indexOf(regExp) > -1)
        sources.append(QString("gputemp"));
    // hdd temp
    regExp = QRegExp(QString("hddtemp.*"));
    if (m_foundKeys.indexOf(regExp) > -1)
        sources.append(QString("hddtemp"));
    // package manager
    regExp = QRegExp(QString("pkgcount.*"));
    if (m_foundKeys.indexOf(regExp) > -1)
        sources.append(QString("pkg"));
    // player
    regExp = QRegExp(QString("(album|artist|duration|progress|title)"));
    if (m_foundKeys.indexOf(regExp) > -1)
        sources.append(QString("player"));
    // ps
    regExp = QRegExp(QString("ps.*"));
    if (m_foundKeys.indexOf(regExp) > -1)
        sources.append(QString("ps"));
    // update == always true
    sources.append(QString("update"));

    return sources;
}


QStringList AWDESources::getSourcesForSystemMonitor()
{
    if (debug) qDebug() << PDEBUG;

    QStringList sources;
    QRegExp regExp;

    // cpu
    regExp = QRegExp(QString("(^|bar[0-9].*)cpu(?!cl).*"));
    if ((m_foundKeys.indexOf(regExp) > -1) ||
        (m_foundBars.indexOf(regExp) > -1) ||
        (m_tooltipBools[QString("cpuTooltip")].toBool())) {
        sources.append(QString("cpu/system/TotalLoad"));
        for (int i=0; i<m_counts[QString("cpu")].toInt(); i++)
            sources.append(QString("cpu/cpu") + QString::number(i) + QString("/TotalLoad"));
    }
    // cpuclock
    regExp = QRegExp(QString("cpucl.*"));
    if ((m_foundKeys.indexOf(regExp) > -1) ||
        (m_foundBars.indexOf(regExp) > -1) ||
        (m_tooltipBools[QString("cpuclTooltip")].toBool())) {
        sources.append(QString("cpu/system/AverageClock"));
        for (int i=0; i<m_counts[QString("cpu")].toInt(); i++)
            sources.append(QString("cpu/cpu") + QString::number(i) + QString("/clock"));
    }
    // disk speed
    regExp = QRegExp(QString("hdd[rw].*"));
    if (m_foundKeys.indexOf(regExp) > -1)
        for (int i=0; i<m_paths[QString("disk")].toString().split(QString("@@")).count(); i++) {
            sources.append(m_paths[QString("disk")].toString().split(QString("@@"))[i] + QString("/Rate/rblk"));
            sources.append(m_paths[QString("disk")].toString().split(QString("@@"))[i] + QString("/Rate/wblk"));
        }
    // fan
    regExp = QRegExp(QString("fan.*"));
    if (m_foundKeys.indexOf(regExp) > -1)
        sources.append(m_paths[QString("fanDevice")].toString().split(QString("@@")));
    // mount
    regExp = QRegExp(QString("(^|bar[0-9].*)hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb).*"));
    if ((m_foundKeys.indexOf(regExp) > -1) ||
        (m_foundBars.indexOf(regExp) > -1))
        for (int i=0; i<m_paths[QString("mount")].toString().split(QString("@@")).count(); i++) {
            sources.append(QString("partitions") + m_paths[QString("mount")].toString().split(QString("@@"))[i] + QString("/filllevel"));
            sources.append(QString("partitions") + m_paths[QString("mount")].toString().split(QString("@@"))[i] + QString("/freespace"));
            sources.append(QString("partitions") + m_paths[QString("mount")].toString().split(QString("@@"))[i] + QString("/usedspace"));
        }
    // memory
    regExp = QRegExp(QString("(^|bar[0-9].*)mem.*"));
    if ((m_foundKeys.indexOf(regExp) > -1) ||
        (m_foundBars.indexOf(regExp) > -1) ||
        (m_tooltipBools[QString("memTooltip")].toBool())) {
        sources.append(QString("mem/physical/free"));
        sources.append(QString("mem/physical/used"));
        sources.append(QString("mem/physical/application"));
    }
    // network
    regExp = QRegExp(QString("(down|up|netdev)"));
    if ((m_foundKeys.indexOf(regExp) > -1) ||
        (m_tooltipBools[QString("downTooltip")].toBool())) {
        sources.append(QString("network/interfaces/") + m_paths[QString("netdev")].toString() + QString("/transmitter/data"));
        sources.append(QString("network/interfaces/") + m_paths[QString("netdev")].toString() + QString("/receiver/data"));
    }
    // swap
    regExp = QRegExp(QString("(^|bar[0-9].*)swap.*"));
    if ((m_foundKeys.indexOf(regExp) > -1) ||
        (m_foundBars.indexOf(regExp) > -1) ||
        (m_paths[QString("swapTooltip")].toBool())) {
        sources.append(QString("mem/swap/free"));
        sources.append(QString("mem/swap/used"));
    }
    // temp
    regExp = QRegExp(QString("temp.*"));
    if (m_foundKeys.indexOf(regExp) > -1)
        sources.append(m_paths[QString("tempDevice")].toString().split(QString("@@")));
    // uptime
    regExp = QRegExp(QString("(^|c)uptime"));
    if (m_foundKeys.indexOf(regExp) > -1)
        sources.append(QString("system/uptime"));

    return sources;
}


QStringList AWDESources::getSourcesForTimeMonitor()
{
    if (debug) qDebug() << PDEBUG;

    QStringList sources;
    QRegExp regExp;

    // time
    regExp = QRegExp(QString("(^|iso|short|long|c)time"));
    if (m_foundKeys.indexOf(regExp) > -1)
        sources.append(QString("Local"));

    return sources;
}

