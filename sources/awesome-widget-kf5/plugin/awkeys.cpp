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

#include "awkeys.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QNetworkInterface>
#include <QProcessEnvironment>
#include <QRegExp>
#include <QStandardPaths>
#include <QThread>

#include <pdebug/pdebug.h>

#include "awtooltip.h"
#include "extscript.h"
#include "graphicalitem.h"
#include "version.h"


AWKeys::AWKeys(QObject *parent)
    : QObject(parent)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));

    initValues();
}


AWKeys::~AWKeys()
{
    if (debug) qDebug() << PDEBUG;
}


void AWKeys::initKeys(const QString pattern, const int maxTooltip)
{
    if (debug) qDebug() << PDEBUG;

    // clear
    counts.clear();
    keys.clear();
    foundBars.clear();
    foundKeys.clear();
    toolTip = nullptr;

    // init
    counts = getCounts();
    keys = dictKeys();
    foundBars = findGraphicalItems(pattern);
    foundKeys = findKeys(pattern);
    toolTip = new AWToolTip(this, maxTooltip);
}


void AWKeys::initValues()
{
    if (debug) qDebug() << PDEBUG;

    // clear
    extScripts.clear();
    graphicalItems.clear();

    // init
    extScripts = getExtScripts();
    graphicalItems = getGraphicalItems();
}


bool AWKeys::isDebugEnabled()
{
    if (debug) qDebug() << PDEBUG;

    return debug;
}


QString AWKeys::networkDevice(const QString custom)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Custom device" << custom;

    QString device = QString("lo");
    if (custom.isEmpty()) {
        QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
        for (int i=0; i<rawInterfaceList.count(); i++)
            if ((rawInterfaceList[i].flags().testFlag(QNetworkInterface::IsUp)) &&
                    (!rawInterfaceList[i].flags().testFlag(QNetworkInterface::IsLoopBack)) &&
                    (!rawInterfaceList[i].flags().testFlag(QNetworkInterface::IsPointToPoint))) {
                device = rawInterfaceList[i].name();
                break;
            }
    } else
        device = custom;

    return device;
}


int AWKeys::numberCpus()
{
    if (debug) qDebug() << PDEBUG;

    return QThread::idealThreadCount();
}


QString AWKeys::parsePattern(const QString pattern)
{
    if (debug) qDebug() << PDEBUG;

    QString parsed = pattern;
    parsed.replace(QString("$$"), QString("$\\$\\"));
    for (int i=0; i<foundKeys.count(); i++)
        parsed.replace(QString("$") + foundKeys[i], values[foundKeys[i]]);
    for (int i=0; i<foundBars.count(); i++) {
        QString key = foundBars[i];
        key.remove(QRegExp(QString("bar[0-9]{1,}")));
        parsed.replace(QString("$") + foundBars[i], getItemByTag(foundBars[i])->image(values[key].toFloat()));
    }
    parsed.replace(QString("$\\$\\"), QString("$$"));

    return parsed;
}


float AWKeys::temperature(const float temp, const QString units)
{
    if (debug) qDebug() << PDEBUG;

    float converted = temp;
    if (units == QString("Celsius"))
        ;
    else if (units == QString("Fahrenheit"))
        converted = temp * 9.0 / 5.0 + 32.0;
    else if (units == QString("Kelvin"))
        converted = temp + 273.15;
    else if (units == QString("Reaumur"))
        converted = temp * 0.8;
    else if (units == QString("cm^-1"))
        converted = (temp + 273.15) * 0.695;
    else if (units == QString("kJ/mol"))
        converted = (temp + 273.15) * 8.31;
    else if (units == QString("kcal/mol"))
        converted = (temp + 273.15) * 1.98;

    return converted;
}


QStringList AWKeys::dictKeys()
{
    if (debug) qDebug() << PDEBUG;

    QStringList allKeys;
    // time
    allKeys.append(QString("time"));
    allKeys.append(QString("isotime"));
    allKeys.append(QString("shorttime"));
    allKeys.append(QString("longtime"));
    allKeys.append(QString("ctime"));
    // uptime
    allKeys.append(QString("uptime"));
    allKeys.append(QString("cuptime"));
    // cpuclock
    for (int i=counts[QString("cpu")].toInt()-1; i>=0; i--)
        allKeys.append(QString("cpucl") + QString::number(i));
    allKeys.append(QString("cpucl"));
    // cpu
    for (int i=counts[QString("cpu")].toInt()-1; i>=0; i--)
        allKeys.append(QString("cpu") + QString::number(i));
    allKeys.append(QString("cpu"));
    // temperature
    for (int i=counts[QString("temp")].toInt()-1; i>=0; i--)
        allKeys.append(QString("temp") + QString::number(i));
    for (int i=counts[QString("fan")].toInt()-1; i>=0; i--)
        allKeys.append(QString("fan") + QString::number(i));
    // gputemp
    allKeys.append(QString("gputemp"));
    // gpu
    allKeys.append(QString("gpu"));
    // memory
    allKeys.append(QString("memmb"));
    allKeys.append(QString("memgb"));
    allKeys.append(QString("memfreemb"));
    allKeys.append(QString("memfreegb"));
    allKeys.append(QString("memtotmb"));
    allKeys.append(QString("memtotgb"));
    allKeys.append(QString("memusedmb"));
    allKeys.append(QString("memusedgb"));
    allKeys.append(QString("mem"));
    // swap
    allKeys.append(QString("swapmb"));
    allKeys.append(QString("swapgb"));
    allKeys.append(QString("swapfreemb"));
    allKeys.append(QString("swapfreegb"));
    allKeys.append(QString("swaptotmb"));
    allKeys.append(QString("swaptotgb"));
    allKeys.append(QString("swap"));
    // hdd
    for (int i=counts[QString("mount")].toInt()-1; i>=0; i--) {
        allKeys.append(QString("hddmb") + QString::number(i));
        allKeys.append(QString("hddgb") + QString::number(i));
        allKeys.append(QString("hddfreemb") + QString::number(i));
        allKeys.append(QString("hddfreegb") + QString::number(i));
        allKeys.append(QString("hddtotmb") + QString::number(i));
        allKeys.append(QString("hddtotgb") + QString::number(i));
        allKeys.append(QString("hdd") + QString::number(i));
    }
    // hdd speed
    for (int i=counts[QString("disk")].toInt()-1; i>=0; i--) {
        allKeys.append(QString("hddr") + QString::number(i));
        allKeys.append(QString("hddw") + QString::number(i));
    }
    // hdd temp
    for (int i=counts[QString("hddtemp")].toInt()-1; i>=0; i--) {
        allKeys.append(QString("hddtemp") + QString::number(i));
        allKeys.append(QString("hddtemp") + QString::number(i));
    }
    // network
    allKeys.append(QString("down"));
    allKeys.append(QString("up"));
    allKeys.append(QString("netdev"));
    // battery
    allKeys.append(QString("ac"));
    for (int i=counts[QString("bat")].toInt()-1; i>=0; i--)
        allKeys.append(QString("bat") + QString::number(i));
    allKeys.append(QString("bat"));
    // player
    allKeys.append(QString("album"));
    allKeys.append(QString("artist"));
    allKeys.append(QString("duration"));
    allKeys.append(QString("progress"));
    allKeys.append(QString("title"));
    // ps
    allKeys.append(QString("pscount"));
    allKeys.append(QString("pstotal"));
    allKeys.append(QString("ps"));
    // package manager
    for (int i=counts[QString("pkg")].toInt()-1; i>=0; i--)
        allKeys.append(QString("pkgcount") + QString::number(i));
    // custom
    for (int i=counts[QString("custom")].toInt()-1; i>=0; i--)
        allKeys.append(QString("custom") + QString::number(i));
    // desktop
    allKeys.append(QString("desktop"));
    allKeys.append(QString("ndesktop"));
    allKeys.append(QString("tdesktops"));

    return allKeys;
}


QStringList AWKeys::extScriptsInfo()
{
    if (debug) qDebug() << PDEBUG;

    QStringList info;
    for (int i=0; i<extScripts.count(); i++) {
        info.append(extScripts[i]->fileName());
        info.append(extScripts[i]->name());
        info.append(extScripts[i]->comment());
        info.append(extScripts[i]->executable());
    }

    return info;
}


QStringList AWKeys::graphicalItemsInfo()
{
    if (debug) qDebug() << PDEBUG;

    QStringList info;
    for (int i=0; i<graphicalItems.count(); i++) {
        info.append(graphicalItems[i]->fileName());
        info.append(graphicalItems[i]->name() + graphicalItems[i]->bar());
        info.append(graphicalItems[i]->comment());
    }

    return info;
}


QStringList AWKeys::timeKeys()
{
    if (debug) qDebug() << PDEBUG;

    QStringList keys;
    keys.append(QString("dddd"));
    keys.append(QString("ddd"));
    keys.append(QString("dd"));
    keys.append(QString("d"));
    keys.append(QString("MMMM"));
    keys.append(QString("MMM"));
    keys.append(QString("MM"));
    keys.append(QString("M"));
    keys.append(QString("yyyy"));
    keys.append(QString("yy"));
    keys.append(QString("hh"));
    keys.append(QString("h"));
    keys.append(QString("mm"));
    keys.append(QString("m"));
    keys.append(QString("ss"));
    keys.append(QString("s"));

    return keys;
}


void AWKeys::setDataBySource(const QString sourceName,
                             const QMap<QString, QVariant> data,
                             const QMap<QString, QVariant> params)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source" << sourceName;

    // regular expressions
    QRegExp cpuRegExp = QRegExp(QString("cpu/cpu.*/TotalLoad"));
    QRegExp cpuclRegExp = QRegExp(QString("cpu/cpu.*/clock"));
    QRegExp hddrRegExp = QRegExp(QString("disk/.*/Rate/rblk"));
    QRegExp hddwRegExp = QRegExp(QString("disk/.*/Rate/wblk"));
    QRegExp mountFillRegExp = QRegExp(QString("partitions/.*/filllevel"));
    QRegExp mountFreeRegExp = QRegExp(QString("partitions/.*/freespace"));
    QRegExp mountUsedRegExp = QRegExp(QString("partitions/.*/usedspace"));
    QRegExp netRecRegExp = QRegExp(QString("network/interfaces/.*/receiver/data"));
    QRegExp netTransRegExp = QRegExp(QString("network/interfaces/.*/transmitter/data"));
    QRegExp tempRegExp = QRegExp(QString("lmsensors/.*"));

    if (sourceName == QString("battery")) {
        // battery
        for (int i=0; i<data.keys().count(); i++) {
            if (data.keys()[i] == QString("ac")) {
                values[QString("ac")] = data.keys()[i];
//                 if (data[QString("ac")].toBool())
//                     values[QString("ac")] = configuration[QString("acOnline")];
//                 else
//                     values[QString("ac")] = configuration[QString("acOffline")];
            } else {
                values[data.keys()[i]] = QString("%1").arg(data[data.keys()[i]].toFloat(), 3, 'f', 0);
                toolTip->setData(QString("bat"), data[data.keys()[i]].toFloat(), data[QString("ac")].toBool());
            }
        }
    } else if (sourceName == QString("cpu/system/TotalLoad")) {
        // cpu
        values[QString("cpu")] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
        toolTip->setData(QString("cpu"), data[QString("value")].toFloat());
    } else if (sourceName.contains(cpuRegExp)) {
        // cpus
        QString number = sourceName;
        number.remove(QString("cpu/cpu")).remove(QString("/TotalLoad"));
        values[QString("cpu") + number] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
    } else if (sourceName == QString("cpu/system/AverageClock")) {
        // cpucl
        values[QString("cpucl")] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 0);
        toolTip->setData(QString("cpucl"), data[QString("value")].toFloat());
    } else if (sourceName.contains(cpuclRegExp)) {
        // cpucls
        QString number = sourceName;
        number.remove(QString("cpu/cpu")).remove(QString("/clock"));
        values[QString("cpucl") + number] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 0);
    } else if (sourceName == QString("custom")) {
        // custom
        for (int i=0; i<data.keys().count(); i++)
            values[data.keys()[i]] = data[data.keys()[i]].toString();
    } else if (sourceName == QString("desktop")) {
        // desktops
        values[QString("desktop")] = data[QString("currentName")].toString();
        values[QString("ndesktop")] = QString("%1").arg(data[QString("currentNumber")].toInt());
        values[QString("tdesktops")] = QString("%1").arg(data[QString("number")].toInt());
    } else if (sourceName.contains(hddrRegExp)) {
        // read speed
        QString device = sourceName;
        device.remove(QString("/Rate/rblk"));
        for (int i=0; i<counts[QString("disk")].toInt(); i++)
            if (params[QString("disk")].toString().split(QString("@@"))[i] == device) {
                values[QString("hddr") + QString::number(i)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 0);
                break;
            }
    } else if (sourceName.contains(hddwRegExp)) {
        // write speed
        QString device = sourceName;
        device.remove(QString("/Rate/wblk"));
        for (int i=0; i<counts[QString("disk")].toInt(); i++)
            if (params[QString("disk")].toString().split(QString("@@"))[i] == device) {
                values[QString("hddw") + QString::number(i)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 0);
                break;
            }
    } else if (sourceName == QString("gpu")) {
        // gpu load
        values[QString("gpu")] = QString("%1").arg(data[QString("GPU")].toFloat(), 5, 'f', 1);
    } else if (sourceName == QString("gputemp")) {
        // gpu temperature
        values[QString("gputemp")] = QString("%1").arg(
            temperature(data[QString("GPUTemp")].toFloat(),params[QString("tempUnits")].toString()), 4, 'f', 1);
    } else if (sourceName.contains(mountFillRegExp)) {
        // fill level
        QString mount = sourceName;
        mount.remove(QString("partitions")).remove(QString("/filllevel"));
        for (int i=0; i<counts[QString("mount")].toInt(); i++)
            if (params[QString("mount")].toString().split(QString("@@"))[i] == mount) {
                values[QString("hdd") + QString::number(i)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
                break;
            }
    } else if (sourceName.contains(mountFreeRegExp)) {
        // free space
        QString mount = sourceName;
        mount.remove(QString("partitions")).remove(QString("/freespace"));
        for (int i=0; i<counts[QString("mount")].toInt(); i++)
            if (params[QString("mount")].toString().split(QString("@@"))[i] == mount) {
                values[QString("hddfreemb") + QString::number(i)] = QString("%1").arg(
                    data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
                values[QString("hddfreegb") + QString::number(i)] = QString("%1").arg(
                    data[QString("value")].toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
                break;
            }
    } else if (sourceName.contains(mountUsedRegExp)) {
        // used
        QString mount = sourceName;
        mount.remove(QString("partitions")).remove(QString("/usedspace"));
        for (int i=0; i<counts[QString("mount")].toInt(); i++)
            if (params[QString("mount")].toString().split(QString("@@"))[i] == mount) {
                values[QString("hddmb") + QString::number(i)] = QString("%1").arg(
                    data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
                values[QString("hddgb") + QString::number(i)] = QString("%1").arg(
                    data[QString("value")].toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
                // total
                values[QString("hddtotmb") + QString::number(i)] = QString("%1").arg(
                    values[QString("hddfreemb") + QString::number(i)].toInt() +
                    values[QString("hddmb") + QString::number(i)].toInt());
                values[QString("hddtotgb") + QString::number(i)] = QString("%1").arg(
                    values[QString("hddfreegb") + QString::number(i)].toFloat() +
                    values[QString("hddgb") + QString::number(i)].toFloat(), 5, 'f', 1);
                break;
            }
    } else if (sourceName == QString("hddtemp")) {
        // hdd temperature
        for (int i=0; i<data.keys().count(); i++)
            for (int j=0; j<counts[QString("hddtemp")].toInt(); j++)
                if (data.keys()[i] == params[QString("hdd")].toString().split(QString("@@"))[j]) {
                    values[QString("hddtemp") + QString::number(j)] = QString("%1").arg(
                        temperature(data[data.keys()[i]].toFloat(), params[QString("tempUnits")].toString()), 4, 'f', 1);
                    break;
                }
    } else if (sourceName == QString("mem/physical/application")) {
        // app memory
        values[QString("memmb")] = QString("%1").arg(data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
        values[QString("memgb")] = QString("%1").arg(data[QString("value")].toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
    } else if (sourceName == QString("mem/physical/free")) {
        // free memory
        values[QString("memfreemb")] = QString("%1").arg(data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
        values[QString("memfreegb")] = QString("%1").arg(data[QString("value")].toFloat() / (1024.0 * 1024.0), 4, 'f', 1);
    } else if (sourceName == QString("mem/physical/used")) {
        // used memory
        values[QString("memusedmb")] = QString("%1").arg(data[QString("value")].toFloat() / 1024.0, 0, 'f', 0);
        values[QString("memusedgb")] = QString("%1").arg(data[QString("value")].toFloat() / (1024.0 * 1024.0), 4, 'f', 1);
        // total
        values[QString("memtotmb")] = QString("%1").arg(
            values[QString("memusedmb")].toInt() + values[QString("memfreemb")].toInt(), 5);
        values[QString("memtotgb")] = QString("%1").arg(
            values[QString("memusedgb")].toFloat() + values[QString("memfreegb")].toFloat(), 4, 'f', 1);
        // percentage
        values[QString("mem")] = QString("%1").arg(
            100.0 * values[QString("memmb")].toFloat() / values[QString("memtotmb")].toFloat(), 5, 'f', 1);
        toolTip->setData(QString("mem"), values[QString("mem")].toFloat());
    } else if (sourceName == QString("netdev")) {
        // network device
        values[QString("netdev")] = data[QString("value")].toString();
    } else if (sourceName.contains(netRecRegExp)) {
        // download speed
        QString device = sourceName;
        device.remove(QString("network/interfaces/")).remove(QString("/receiver/data"));
        if (device == networkDevice(params[QString("customNetdev")].toString())) {
            values[QString("down")] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 0);
            toolTip->setData(QString("down"), data[QString("value")].toFloat());
        }
    } else if (sourceName.contains(netTransRegExp)) {
        // upload speed
        QString device = sourceName;
        device.remove(QString("network/interfaces/")).remove(QString("/transmitter/data"));
        if (device == networkDevice(params[QString("customNetdev")].toString())) {
            values[QString("up")] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 0);
            toolTip->setData(QString("up"), data[QString("value")].toFloat());
        }
    } else if (sourceName == QString("pkg")) {
        // package manager
        for (int i=0; i<data.keys().count(); i++)
            values[data.keys()[i]] = QString("%1").arg(data[data.keys()[i]].toInt(), 2);
    } else if (sourceName == QString("player")) {
        // player
        values[QString("album")] = data[QString("album")].toString();
        values[QString("artist")] = data[QString("artist")].toString();
        values[QString("duration")] = data[QString("duration")].toString();
        values[QString("progress")] = data[QString("progress")].toString();
        values[QString("title")] = data[QString("title")].toString();
    } else if (sourceName == QString("ps")) {
        // ps
        values[QString("ps")] = data[QString("ps")].toString();
        values[QString("pscount")] = QString("%1").arg(data[QString("pscount")].toInt(), 2);
        values[QString("pstotal")] = QString("%1").arg(data[QString("pstotal")].toInt(), 3);
    } else if (sourceName == QString("mem/swap/free")) {
        // free swap
        values[QString("swapfreemb")] = QString("%1").arg(data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
        values[QString("swapfreegb")] = QString("%1").arg(data[QString("value")].toFloat() / (1024.0 * 1024.0), 4, 'f', 1);
    } else if (sourceName == QString("mem/swap/used")) {
        // used swap
        values[QString("swapmb")] = QString("%1").arg(data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
        values[QString("swapgb")] = QString("%1").arg(data[QString("value")].toFloat() / (1024.0 * 1024.0), 4, 'f', 1);
        // total
        values[QString("swaptotmb")] = QString("%1").arg(
            values[QString("swapmb")].toInt() + values[QString("swapfreemb")].toInt(), 5);
        values[QString("swaptotgb")] = QString("%1").arg(
            values[QString("swapgb")].toFloat() + values[QString("swapfreegb")].toFloat(), 4, 'f', 1);
        // percentage
        values[QString("swap")] = QString("%1").arg(
            100.0 * values[QString("swapmb")].toFloat() / values[QString("swaptotmb")].toFloat(), 5, 'f', 1);
        toolTip->setData(QString("swap"), values[QString("swap")].toFloat());
    } else if (sourceName.contains(tempRegExp)) {
        // temperature devices
        if (data[QString("units")].toString() == QString("rpm")) {
            for (int i=0; i<counts[QString("fan")].toInt(); i++)
                if (sourceName == params[QString("fanDevice")].toString().split(QString("@@"))[i]) {
                    values[QString("fan") + QString::number(i)] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 1);
                    break;
                }
        } else {
            for (int i=0; i<counts[QString("temp")].toInt(); i++)
                if (sourceName == params[QString("tempDevice")].toString().split(QString("@@"))[i]) {
                    values[QString("temp") + QString::number(i)] = QString("%1").arg(
                        temperature(data[QString("value")].toFloat(), params[QString("tempUnits")].toString()), 4, 'f', 1);
                    break;
                }
        }
    } else if (sourceName == QString("Local")) {
        // time
        values[QString("time")] = data[QString("DateTime")].toDateTime().toString(Qt::TextDate);
        values[QString("isotime")] = data[QString("DateTime")].toDateTime().toString(Qt::ISODate);
        values[QString("shorttime")] = data[QString("DateTime")].toDateTime().toString(Qt::SystemLocaleShortDate);
        values[QString("longtime")] = data[QString("DateTime")].toDateTime().toString(Qt::SystemLocaleLongDate);
        QStringList _timeKeys = timeKeys();
        values[QString("ctime")] = params[QString("customTime")].toString();
        for (int i=0; i<_timeKeys.count(); i++)
            values[QString("ctime")].replace(QString("$") + _timeKeys[i],
                                             data[QString("DateTime")].toDateTime().toString(_timeKeys[i]));
    } else if (sourceName == QString("system/uptime")) {
        // uptime
        int uptime = data[QString("value")].toFloat();
        int seconds = uptime - uptime % 60;
        int minutes = seconds / 60 % 60;
        int hours = ((seconds / 60) - minutes) / 60 % 24;
        int days = (((seconds / 60) - minutes) / 60 - hours) / 24;
        values[QString("uptime")] = QString("%1d%2h%3m").arg(days, 3).arg(hours, 2).arg(minutes, 2);
        values[QString("cuptime")] = params[QString("customUptime")].toString();
        values[QString("cuptime")].replace(QString("$dd"), QString("%1").arg(days, 3, 10, QChar('0')));
        values[QString("cuptime")].replace(QString("$d"), QString("%1").arg(days));
        values[QString("cuptime")].replace(QString("$hh"), QString("%1").arg(hours, 2, 10, QChar('0')));
        values[QString("cuptime")].replace(QString("$h"), QString("%1").arg(hours));
        values[QString("cuptime")].replace(QString("$mm"), QString("%1").arg(minutes, 2, 10, QChar('0')));
        values[QString("cuptime")].replace(QString("$m"), QString("%1").arg(minutes));
    }
}


QMap<QString, QVariant> AWKeys::getCounts()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QVariant> awCounts;
    awCounts[QString("cpu")] = numberCpus();
    awCounts[QString("custom")] = getExtScripts().count();
//    awCounts[QString("disk")] = configuration[QString("disk")].split(QString("@@")).count();
//    awCounts[QString("fan")] = configuration[QString("fanDevice")].split(QString("@@")).count();
//    awCounts[QString("hddtemp")] = configuration[QString("hdd")].split(QString("@@")).count();
//    awCounts[QString("mount")] = configuration[QString("mount")].split(QString("@@")).count();
//    awCounts[QString("pkg")] = deSettings[QString("PKGCMD")].split(QChar(',')).count();
//    awCounts[QString("temp")] = configuration[QString("tempDevice")].split(QString("@@")).count();
//    awCounts[QString("tooltip")] = 0;
//    awCounts[QString("tooltip")] += configuration[QString("cpuTooltip")].toInt();
//    awCounts[QString("tooltip")] += configuration[QString("cpuclTooltip")].toInt();
//    awCounts[QString("tooltip")] += configuration[QString("memTooltip")].toInt();
//    awCounts[QString("tooltip")] += configuration[QString("swapTooltip")].toInt();
//    awCounts[QString("tooltip")] += configuration[QString("downTooltip")].toInt();
//    awCounts[QString("tooltip")] += configuration[QString("batteryTooltip")].toInt();
//    awCounts[QString("tooltip")] = counts[QString("tooltip")] / 2;

    return awCounts;
}


QStringList AWKeys::findGraphicalItems(const QString pattern)
{
    if (debug) qDebug() << PDEBUG;

    QStringList orderedKeys;
    for (int i=0; i<graphicalItems.count(); i++)
        orderedKeys.append(graphicalItems[i]->name() + graphicalItems[i]->bar());
    orderedKeys.sort();

    QStringList selectedKeys;
    for (int i=orderedKeys.count()-1; i>=0; i--)
        if (pattern.contains(QString("$") + orderedKeys[i])) {
            if (debug) qDebug() << PDEBUG << ":" << "Found key" << orderedKeys[i];
            selectedKeys.append(orderedKeys[i]);
        }

    return selectedKeys;
}


QStringList AWKeys::findKeys(const QString pattern)
{
    QStringList selectedKeys;
    for (int i=0; i<keys.count(); i++)
        if (pattern.contains(QString("$") + keys[i])) {
            if (debug) qDebug() << PDEBUG << ":" << "Found key" << keys[i];
            selectedKeys.append(keys[i]);
        }

    return selectedKeys;
}


QList<ExtScript *> AWKeys::getExtScripts()
{
    if (debug) qDebug() << PDEBUG;

    QList<ExtScript *> externalScripts;
    // create directory at $HOME
    QString localDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
            QString("/plasma_engine_extsysmon/scripts");
    QDir localDirectory;
    if (localDirectory.mkpath(localDir))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::DataLocation,
                                                 QString("plasma_engine_extsysmon/scripts"),
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

    return externalScripts;
}


QList<GraphicalItem *> AWKeys::getGraphicalItems()
{
    if (debug) qDebug() << PDEBUG;

    QList<GraphicalItem *> items;
    // create directory at $HOME
    QString localDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
            QString("/plasma_applet_awesome-widget/desktops");
    QDir localDirectory;
    if (localDirectory.mkpath(localDir))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::DataLocation,
                                                 QString("plasma_applet_awesome-widget/desktops"),
                                                 QStandardPaths::LocateDirectory);
    QStringList names;
    for (int i=0; i<dirs.count(); i++) {
        QStringList files = QDir(dirs[i]).entryList(QDir::Files, QDir::Name);
        for (int j=0; j<files.count(); j++) {
            if (!files[j].endsWith(QString(".desktop"))) continue;
            if (names.contains(files[j])) continue;
            if (debug) qDebug() << PDEBUG << ":" << "Found file" << files[j] << "in" << dirs[i];
            names.append(files[j]);
            items.append(new GraphicalItem(0, files[j], dirs, debug));
        }
    }

    return items;
}


GraphicalItem *AWKeys::getItemByTag(const QString tag)
{
    if (debug) qDebug() << PDEBUG;

    GraphicalItem *item = nullptr;
    for (int i=0; i< graphicalItems.count(); i++) {
        if ((graphicalItems[i]->name() + graphicalItems[i]->bar()) != tag) continue;
        item = graphicalItems[i];
        break;
    }

    return item;
}
