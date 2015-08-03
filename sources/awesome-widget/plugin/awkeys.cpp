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

#include <KI18n/KLocalizedString>

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QInputDialog>
#include <QNetworkInterface>
#include <QProcessEnvironment>
#include <QRegExp>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

#include <pdebug/pdebug.h>

#include "awactions.h"
#include "awtooltip.h"
#include "extquotes.h"
#include "extscript.h"
#include "extupgrade.h"
#include "extweather.h"
#include "graphicalitem.h"
#include "version.h"


AWKeys::AWKeys(QObject *parent)
    : QObject(parent)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));

    // backend
    graphicalItems = new ExtItemAggregator<GraphicalItem>(nullptr, QString("desktops"), debug);
    extQuotes = new ExtItemAggregator<ExtQuotes>(nullptr, QString("quotes"), debug);
    extScripts = new ExtItemAggregator<ExtScript>(nullptr, QString("scripts"), debug);
    extUpgrade = new ExtItemAggregator<ExtUpgrade>(nullptr, QString("upgrade"), debug);
    extWeather = new ExtItemAggregator<ExtWeather>(nullptr, QString("weather"), debug);
    connect(this, SIGNAL(needToBeUpdated()), this, SLOT(dataUpdate()));
}


AWKeys::~AWKeys()
{
    if (debug) qDebug() << PDEBUG;

    if (toolTip != nullptr)  delete toolTip;

    delete graphicalItems;
    delete extQuotes;
    delete extScripts;
    delete extUpgrade;
    delete extWeather;
}


void AWKeys::initKeys(const QString currentPattern)
{
    if (debug) qDebug() << PDEBUG;

    // clear
    keys.clear();
    foundBars.clear();
    foundKeys.clear();

    // init
    pattern = currentPattern;
    // update network and hdd list
    addKeyToCache(QString("Hdd"));
    addKeyToCache(QString("Network"));
    loadKeysFromCache();
    reinitKeys();
}


void AWKeys::initTooltip(const QVariantMap tooltipParams)
{
    if (debug) qDebug() << PDEBUG;

    if (toolTip != nullptr) {
        disconnect(toolTip, SIGNAL(toolTipPainted(QString)), this, SIGNAL(needToolTipToBeUpdated(QString)));
        delete toolTip;
    }

    toolTip = new AWToolTip(this, tooltipParams);
    // transfer signal from AWToolTip object to QML ui
    connect(toolTip, SIGNAL(toolTipPainted(QString)), this, SIGNAL(needToolTipToBeUpdated(QString)));
}


void AWKeys::setPopupEnabled(const bool popup)
{
    if (debug) qDebug() << PDEBUG;

    enablePopup = popup;
}


void AWKeys::setWrapNewLines(const bool wrap)
{
    if (debug) qDebug() << PDEBUG;

    wrapNewLines = wrap;
}


QString AWKeys::parsePattern() const
{
    if (debug) qDebug() << PDEBUG;
    if (keys.isEmpty()) return pattern;

    QString parsed = pattern;
    parsed.replace(QString("$$"), QString("$\\$\\"));
    for (int i=0; i<foundKeys.count(); i++)
        parsed.replace(QString("$%1").arg(foundKeys[i]), htmlValue(foundKeys[i]));
    for (int i=0; i<foundBars.count(); i++)
        parsed.replace(QString("$%1").arg(foundBars[i]), getItemByTag(foundBars[i])->image(valueByKey(foundBars[i]).toFloat()));
    parsed.replace(QString("$\\$\\"), QString("$$"));
    // wrap new lines if required
    if (wrapNewLines) parsed.replace(QString("\n"), QString("<br>"));

    return parsed;
}


QSize AWKeys::toolTipSize() const
{
    if (debug) qDebug() << PDEBUG;
    if (toolTip == nullptr) return QSize();

    return toolTip->getSize();
}


void AWKeys::addDevice(const QString source)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source" << source;

    QRegExp diskRegexp = QRegExp(QString("disk/(?:md|sd|hd)[a-z|0-9]_.*/Rate/(?:rblk)"));
    QRegExp mountRegexp = QRegExp(QString("partitions/.*/filllevel"));
    QRegExp tempRegexp = QRegExp(QString("lmsensors/.*"));

    if (diskRegexp.indexIn(source) > -1) {
        QString device = source;
        device.remove(QString("/Rate/rblk"));
        addKeyToCache(QString("Disk"), device);
    } else if (mountRegexp.indexIn(source) > -1) {
        QString device = source;
        device.remove(QString("partitions")).remove(QString("/filllevel"));
        addKeyToCache(QString("Mount"), device);
    } else if (tempRegexp.indexIn(source) > -1) {
        addKeyToCache(QString("Temp"), source);
    }
}


QStringList AWKeys::dictKeys(const bool sorted) const
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
    for (int i=numberCpus()-1; i>=0; i--)
        allKeys.append(QString("cpucl%1").arg(i));
    allKeys.append(QString("cpucl"));
    // cpu
    for (int i=numberCpus()-1; i>=0; i--)
        allKeys.append(QString("cpu%1").arg(i));
    allKeys.append(QString("cpu"));
    // temperature
    for (int i=tempDevices.count()-1; i>=0; i--)
        allKeys.append(QString("temp%1").arg(i));
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
    for (int i=mountDevices.count()-1; i>=0; i--) {
        allKeys.append(QString("hddmb%1").arg(i));
        allKeys.append(QString("hddgb%1").arg(i));
        allKeys.append(QString("hddfreemb%1").arg(i));
        allKeys.append(QString("hddfreegb%1").arg(i));
        allKeys.append(QString("hddtotmb%1").arg(i));
        allKeys.append(QString("hddtotgb%1").arg(i));
        allKeys.append(QString("hdd%1").arg(i));
    }
    // hdd speed
    for (int i=diskDevices.count()-1; i>=0; i--) {
        allKeys.append(QString("hddr%1").arg(i));
        allKeys.append(QString("hddw%1").arg(i));
    }
    // hdd temp
    for (int i=hddDevices.count()-1; i>=0; i--)
        allKeys.append(QString("hddtemp%1").arg(i));
    // network
    for (int i=networkDevices.count()-1; i>=0; i--) {
        allKeys.append(QString("down%1").arg(i));
        allKeys.append(QString("up%1").arg(i));
    }
    allKeys.append(QString("down"));
    allKeys.append(QString("up"));
    allKeys.append(QString("netdev"));
    // battery
    allKeys.append(QString("ac"));
    QStringList allBatteryDevices = QDir(QString("/sys/class/power_supply"))
                                        .entryList(QStringList() << QString("BAT*"),
                                                   QDir::Dirs | QDir::NoDotAndDotDot,
                                                   QDir::Name);
    for (int i=allBatteryDevices.count()-1; i>=0; i--)
        allKeys.append(QString("bat%1").arg(i));
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
    for (int i=extUpgrade->items().count()-1; i>=0; i--) {
        if (!extUpgrade->items()[i]->isActive()) continue;
        allKeys.append(extUpgrade->items()[i]->tag(QString("pkgcount")));
    }
    // quotes
    for (int i=extQuotes->items().count()-1; i>=0; i--) {
        if (!extQuotes->items()[i]->isActive()) continue;
        allKeys.append(extQuotes->items()[i]->tag(QString("ask")));
        allKeys.append(extQuotes->items()[i]->tag(QString("askchg")));
        allKeys.append(extQuotes->items()[i]->tag(QString("percaskchg")));
        allKeys.append(extQuotes->items()[i]->tag(QString("bid")));
        allKeys.append(extQuotes->items()[i]->tag(QString("bidchg")));
        allKeys.append(extQuotes->items()[i]->tag(QString("percbidchg")));
        allKeys.append(extQuotes->items()[i]->tag(QString("price")));
        allKeys.append(extQuotes->items()[i]->tag(QString("pricechg")));
        allKeys.append(extQuotes->items()[i]->tag(QString("percpricechg")));
    }
    // custom
    for (int i=extScripts->items().count()-1; i>=0; i--) {
        if (!extScripts->items()[i]->isActive()) continue;
        allKeys.append(extScripts->items()[i]->tag(QString("custom")));
    }
    // desktop
    allKeys.append(QString("desktop"));
    allKeys.append(QString("ndesktop"));
    allKeys.append(QString("tdesktops"));
    // load average
    allKeys.append(QString("la15"));
    allKeys.append(QString("la5"));
    allKeys.append(QString("la1"));
    // weather
    for (int i=extWeather->items().count()-1; i>=0; i--) {
        if (!extWeather->items()[i]->isActive()) continue;
        allKeys.append(extWeather->items()[i]->tag(QString("weatherId")));
        allKeys.append(extWeather->items()[i]->tag(QString("weather")));
        allKeys.append(extWeather->items()[i]->tag(QString("humidity")));
        allKeys.append(extWeather->items()[i]->tag(QString("pressure")));
        allKeys.append(extWeather->items()[i]->tag(QString("temperature")));
        allKeys.append(extWeather->items()[i]->tag(QString("timestamp")));
    }
    // bars
    QStringList graphicalItemsKeys;
    for (int i=0; i<graphicalItems->items().count(); i++)
        graphicalItemsKeys.append(QString("%1%2").arg(graphicalItems->items()[i]->name())
                                                 .arg(graphicalItems->items()[i]->bar()));
    graphicalItemsKeys.sort();
    for (int i=graphicalItemsKeys.count()-1; i>=0; i--)
        allKeys.append(graphicalItemsKeys[i]);

    // sort if required
    if (sorted) allKeys.sort();

    return allKeys;
}


QStringList AWKeys::getHddDevices() const
{
    if (debug) qDebug() << PDEBUG;

    QStringList devices = hddDevices;
    // required by ui interface
    devices.insert(0, QString("disable"));
    devices.insert(0, QString("auto"));

    return devices;
}


void AWKeys::setDataBySource(const QString sourceName, const QVariantMap data,
                             const QVariantMap params)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source" << sourceName;

    if (sourceName == QString("update")) return emit(needToBeUpdated());

    // checking
    if (keys.isEmpty()) return;

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
                // notification
                if ((values[QString("ac")] == params[QString("acOnline")].toString()) != data[QString("ac")].toBool()) {
                    if (data[QString("ac")].toBool())
                        AWActions::sendNotification(QString("event"), i18n("AC online"), enablePopup);
                    else
                        AWActions::sendNotification(QString("event"), i18n("AC offline"), enablePopup);
                }
                // value
                values[QString("ac")] = data.keys()[i];
                if (data[QString("ac")].toBool())
                    values[QString("ac")] = params[QString("acOnline")].toString();
                else
                    values[QString("ac")] = params[QString("acOffline")].toString();
            } else
                values[data.keys()[i]] = QString("%1").arg(data[data.keys()[i]].toFloat(), 3, 'f', 0);
        }
    } else if (sourceName == QString("cpu/system/TotalLoad")) {
        // cpu
        // notification
        if ((data[QString("value")].toFloat() >= 90.0) && (values[QString("cpu")].toFloat() < 90.0))
            AWActions::sendNotification(QString("event"), i18n("High CPU load"), enablePopup);
        // value
        values[QString("cpu")] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
    } else if (sourceName.contains(cpuRegExp)) {
        // cpus
        QString number = sourceName;
        number.remove(QString("cpu/cpu")).remove(QString("/TotalLoad"));
        values[QString("cpu") + number] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
    } else if (sourceName == QString("cpu/system/AverageClock")) {
        // cpucl
        values[QString("cpucl")] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 0);
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
        for (int i=0; i<diskDevices.count(); i++)
            if (diskDevices[i] == device) {
                values[QString("hddr%1").arg(i)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 0);
                break;
            }
    } else if (sourceName.contains(hddwRegExp)) {
        // write speed
        QString device = sourceName;
        device.remove(QString("/Rate/wblk"));
        for (int i=0; i<diskDevices.count(); i++)
            if (diskDevices[i] == device) {
                values[QString("hddw%1").arg(i)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 0);
                break;
            }
    } else if (sourceName == QString("gpu")) {
        // gpu load
        // notification
        if ((data[QString("value")].toFloat() >= 75.0) && (values[QString("gpu")].toFloat() < 75.0))
            AWActions::sendNotification(QString("event"), i18n("High GPU load"), enablePopup);
        // value
        values[QString("gpu")] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
    } else if (sourceName == QString("gputemp")) {
        // gpu temperature
        values[QString("gputemp")] = QString("%1").arg(
            temperature(data[QString("value")].toFloat(), params[QString("tempUnits")].toString()), 4, 'f', 1);
    } else if (sourceName.contains(mountFillRegExp)) {
        // fill level
        QString mount = sourceName;
        mount.remove(QString("partitions")).remove(QString("/filllevel"));
        for (int i=0; i<mountDevices.count(); i++)
            if (mountDevices[i] == mount) {
                if ((data[QString("value")].toFloat() >= 90.0) && (values[QString("hdd%1").arg(i)].toFloat() < 90.0))
                    AWActions::sendNotification(QString("event"), i18n("Free space on %1 less than 10%", mount), enablePopup);
                values[QString("hdd%1").arg(i)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
                break;
            }
    } else if (sourceName.contains(mountFreeRegExp)) {
        // free space
        QString mount = sourceName;
        mount.remove(QString("partitions")).remove(QString("/freespace"));
        for (int i=0; i<mountDevices.count(); i++)
            if (mountDevices[i] == mount) {
                values[QString("hddfreemb%1").arg(i)] = QString("%1").arg(
                    data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
                values[QString("hddfreegb%1").arg(i)] = QString("%1").arg(
                    data[QString("value")].toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
                break;
            }
    } else if (sourceName.contains(mountUsedRegExp)) {
        // used
        QString mount = sourceName;
        mount.remove(QString("partitions")).remove(QString("/usedspace"));
        for (int i=0; i<mountDevices.count(); i++)
            if (mountDevices[i] == mount) {
                values[QString("hddmb%1").arg(i)] = QString("%1").arg(
                    data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
                values[QString("hddgb%1").arg(i)] = QString("%1").arg(
                    data[QString("value")].toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
                // total
                values[QString("hddtotmb%1").arg(i)] = QString("%1").arg(
                    values[QString("hddfreemb%1").arg(i)].toInt() +
                    values[QString("hddmb%1").arg(i)].toInt());
                values[QString("hddtotgb%1").arg(i)] = QString("%1").arg(
                    values[QString("hddfreegb%1").arg(i)].toFloat() +
                    values[QString("hddgb%1").arg(i)].toFloat(), 5, 'f', 1);
                break;
            }
    } else if (sourceName == QString("hddtemp")) {
        // hdd temperature
        for (int i=0; i<data.keys().count(); i++)
            for (int j=0; j<hddDevices.count(); j++)
                if (hddDevices[j] == data.keys()[i]) {
                    values[QString("hddtemp%1").arg(j)] = QString("%1").arg(
                        temperature(data[data.keys()[i]].toFloat(), params[QString("tempUnits")].toString()), 4, 'f', 1);
                    break;
                }
    } else if (sourceName.startsWith(QString("cpu/system/loadavg"))) {
        // load average
        QString time = sourceName;
        time.remove(QString("cpu/system/loadavg"));
        values[QString("la%1").arg(time)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 2);
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
        float value = 100.0 * values[QString("memmb")].toFloat() / values[QString("memtotmb")].toFloat();
        // notification
        if ((value >= 90.0) && (values[QString("mem")].toFloat() < 90.0))
            AWActions::sendNotification(QString("event"), i18n("High memory usage"), enablePopup);
        // value
        values[QString("mem")] = QString("%1").arg(value, 5, 'f', 1);
    } else if (sourceName == QString("netdev")) {
        // network device
        // notification
        if (values[QString("netdev")] != data[QString("value")].toString())
            AWActions::sendNotification(QString("event"), i18n("Network device has been changed to %1",
                                                               data[QString("value")].toString()),
                                        enablePopup);
        // value
        values[QString("netdev")] = data[QString("value")].toString();
    } else if (sourceName.contains(netRecRegExp)) {
        // download speed
        QString device = sourceName;
        float value = data[QString("value")].toFloat();
        device.remove(QString("network/interfaces/")).remove(QString("/receiver/data"));
        QStringList allNetworkDevices = networkDevices;
        for (int i=0; i<allNetworkDevices.count(); i++) {
            if (allNetworkDevices[i] != device) continue;
            if (value > 1000.0)
                values[QString("down%1").arg(i)] = QString("%1").arg(value / 1024.0, 4, 'f', 1);
            else
                values[QString("down%1").arg(i)] = QString("%1").arg(value, 4, 'f', 0);
            break;
        }
        if (device == values[QString("netdev")]) {
            if (value > 1000.0)
                values[QString("down")] = QString("%1").arg(value / 1024.0, 4, 'f', 1);
            else
                values[QString("down")] = QString("%1").arg(value, 4, 'f', 0);
        }
    } else if (sourceName.contains(netTransRegExp)) {
        // upload speed
        QString device = sourceName;
        float value = data[QString("value")].toFloat();
        device.remove(QString("network/interfaces/")).remove(QString("/transmitter/data"));
        QStringList allNetworkDevices = networkDevices;
        for (int i=0; i<allNetworkDevices.count(); i++) {
            if (allNetworkDevices[i] != device) continue;
            if (value > 1000.0)
                values[QString("up%1").arg(i)] = QString("%1").arg(data[QString("value")].toFloat() / 1024.0, 4, 'f', 1);
            else
                values[QString("up%1").arg(i)] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 0);
            break;
        }
        if (device == values[QString("netdev")]) {
            if (value > 1000.0)
                values[QString("up")] = QString("%1").arg(value / 1024.0, 4, 'f', 1);
            else
                values[QString("up")] = QString("%1").arg(value, 4, 'f', 0);
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
    } else if (sourceName == QString("quotes")) {
        // quotes
        for (int i=0; i<data.keys().count(); i++)
            values[data.keys()[i]] = QString("%1").arg(data[data.keys()[i]].toFloat(), 7, 'f');
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
        float value = 100.0 * values[QString("swapmb")].toFloat() / values[QString("swaptotmb")].toFloat();
        // notification
        if ((value > 0.0) && (values[QString("swap")].toFloat() == 0.0))
            AWActions::sendNotification(QString("event"), i18n("Swap is used"), enablePopup);
        // value
        values[QString("swap")] = QString("%1").arg(value, 5, 'f', 1);
    } else if (sourceName.contains(tempRegExp)) {
        // temperature devices
        for (int i=0; i<tempDevices.count(); i++)
            if (sourceName == tempDevices[i]) {
                float temp = data[QString("units")].toString() == QString("°C") ?
                    temperature(data[QString("value")].toFloat(), params[QString("tempUnits")].toString()) :
                    data[QString("value")].toFloat();
                values[QString("temp%1").arg(i)] = QString("%1").arg(temp, 4, 'f', 1);
                break;
            }
    } else if (sourceName == QString("Local")) {
        // time
        values[QString("time")] = data[QString("DateTime")].toDateTime().toString(Qt::TextDate);
        values[QString("isotime")] = data[QString("DateTime")].toDateTime().toString(Qt::ISODate);
        values[QString("shorttime")] = data[QString("DateTime")].toDateTime().toString(Qt::SystemLocaleShortDate);
        values[QString("longtime")] = data[QString("DateTime")].toDateTime().toString(Qt::SystemLocaleLongDate);
        QStringList _timeKeys = getTimeKeys();
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
    } else if (sourceName == QString("weather")) {
        for (int i=0; i<data.keys().count(); i++) {
            if (data.keys()[i].startsWith(QString("weatherId")))
                values[data.keys()[i]] = QString("%1").arg(data[data.keys()[i]].toInt());
            else if (data.keys()[i].startsWith(QString("weather")))
                values[data.keys()[i]] = data[data.keys()[i]].toString();
            else if (data.keys()[i].startsWith(QString("humidity")))
                values[data.keys()[i]] = QString("%1").arg(data[data.keys()[i]].toInt(), 3);
            else if (data.keys()[i].startsWith(QString("pressure")))
                values[data.keys()[i]] = QString("%1").arg(data[data.keys()[i]].toFloat(), 0, 'f', 1);
            else if (data.keys()[i].startsWith(QString("temperature")))
                values[data.keys()[i]] = QString("%1").arg(
                    temperature(data[data.keys()[i]].toFloat(), params[QString("tempUnits")].toString()), 4, 'f', 1);
        }
    } else {
        if (debug) qDebug() << PDEBUG << ":" << "Source" << sourceName << "not found";
        emit(dropSourceFromDataengine(sourceName));
    }
}


void AWKeys::graphicalValueByKey() const
{
    if (debug) qDebug() << PDEBUG;

    bool ok;
    QString tag = QInputDialog::getItem(nullptr, i18n("Select tag"), i18n("Tag"),
                                        dictKeys(true), 0, false, &ok);

    if ((!ok) || (tag.isEmpty())) return;
    QString message = i18n("Tag: %1", tag);
    message += QString("<br>");
    message += i18n("Value: %1", valueByKey(tag));

    return AWActions::sendNotification(QString("tag"), message);
}


QString AWKeys::infoByKey(QString key) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Requested key" << key;

    key.remove(QRegExp(QString("^bar[0-9]{1,}")));
    if (key.startsWith(QString("custom")))
        for (int i=0; i<extScripts->items().count(); i++) {
            if (extScripts->items()[i]->tag(QString("custom")) != key) continue;
            return extScripts->items()[i]->executable();
        }
    else if (key.contains(QRegExp(QString("^hdd[rw]"))))
        return QString("%1").arg(diskDevices[key.remove(QRegExp(QString("hdd[rw]"))).toInt()]);
    else if (key.contains(QRegExp(QString("^hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb)"))))
        return QString("%1").arg(mountDevices[key.remove(QRegExp(QString("^hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb)"))).toInt()]);
    else if (key.startsWith(QString("hddtemp")))
        return QString("%1").arg(hddDevices[key.remove(QString("hddtemp")).toInt()]);
    else if (key.contains(QRegExp(QString("^(down|up)[0-9]"))))
        return QString("%1").arg(networkDevices[key.remove(QRegExp(QString("^(down|up)"))).toInt()]);
    else if (key.startsWith(QString("pkgcount")))
        for (int i=0; i<extUpgrade->items().count(); i++) {
            if (extUpgrade->items()[i]->tag(QString("pkgcount")) != key) continue;
            return extUpgrade->items()[i]->executable();
        }
    else if (key.contains(QRegExp(QString("(^|perc)(ask|bid|price)(chg|)"))))
        for (int i=0; i<extQuotes->items().count(); i++) {
            if (extQuotes->items()[i]->number() != key.remove(QRegExp(QString("(^|perc)(ask|bid|price)(chg|)"))).toInt()) continue;
            return extQuotes->items()[i]->ticker();
        }
    else if (key.startsWith(QString("temp")))
        return QString("%1").arg(tempDevices[key.remove(QString("temp")).toInt()]);

    return QString("(none)");
}


QString AWKeys::valueByKey(QString key) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Requested key" << key;

    key.remove(QRegExp(QString("^bar[0-9]{1,}")));

    return values[key];
}


void AWKeys::editItem(const QString type)
{
    if (debug) qDebug() << PDEBUG;

    if (type == QString("graphicalitem")) {
        QStringList bars;
        bars.append(keys.filter((QRegExp(QString("^cpu(?!cl).*")))));
        bars.append(keys.filter((QRegExp(QString("^gpu$")))));
        bars.append(keys.filter((QRegExp(QString("^mem$")))));
        bars.append(keys.filter((QRegExp(QString("^swap$")))));
        bars.append(keys.filter((QRegExp(QString("^hdd[0-9].*")))));
        bars.append(keys.filter((QRegExp(QString("^bat.*")))));
        graphicalItems->setConfigArgs(bars);
        return graphicalItems->editItems();
    } else if (type == QString("extquotes")) {
        return extQuotes->editItems();
    } else if (type == QString("extscript")) {
        return extScripts->editItems();
    } else if (type == QString("extupgrade")) {
        return extUpgrade->editItems();
    } else if (type == QString("extweather")) {
        return extWeather->editItems();
    }
}


void AWKeys::dataUpdate() const
{
    if (debug) qDebug() << PDEBUG;

    emit(needTextToBeUpdated(parsePattern()));
    if (toolTip != nullptr) emit(toolTip->updateData(values));
}


void AWKeys::loadKeysFromCache()
{
    if (debug) qDebug() << PDEBUG;

    QString fileName = QString("%1/awesomewidgets.ndx")
                       .arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    if (debug) qDebug() << PDEBUG << ":" << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);
    QStringList cachedKeys;

    cache.beginGroup(QString("Disk"));
    diskDevices.clear();
    cachedKeys = cache.allKeys();
    for (int i=0; i<cachedKeys.count(); i++)
        diskDevices.append(cache.value(cachedKeys[i]).toString());
    cache.endGroup();

    cache.beginGroup(QString("Hdd"));
    hddDevices.clear();
    cachedKeys = cache.allKeys();
    for (int i=0; i<cachedKeys.count(); i++)
        hddDevices.append(cache.value(cachedKeys[i]).toString());
    cache.endGroup();

    cache.beginGroup(QString("Mount"));
    mountDevices.clear();
    cachedKeys = cache.allKeys();
    for (int i=0; i<cachedKeys.count(); i++)
        mountDevices.append(cache.value(cachedKeys[i]).toString());
    cache.endGroup();

    cache.beginGroup(QString("Network"));
    networkDevices.clear();
    cachedKeys = cache.allKeys();
    for (int i=0; i<cachedKeys.count(); i++)
        networkDevices.append(cache.value(cachedKeys[i]).toString());
    cache.endGroup();

    cache.beginGroup(QString("Temp"));
    tempDevices.clear();
    cachedKeys = cache.allKeys();
    for (int i=0; i<cachedKeys.count(); i++)
        tempDevices.append(cache.value(cachedKeys[i]).toString());
    cache.endGroup();
}


void AWKeys::reinitKeys()
{
    if (debug) qDebug() << PDEBUG;

    keys = dictKeys();
    foundBars = findGraphicalItems();
    foundKeys = findKeys();
}


void AWKeys::addKeyToCache(const QString type, const QString key)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Key type" << type;
    if (debug) qDebug() << PDEBUG << ":" << "Key" << key;

    QString fileName = QString("%1/awesomewidgets.ndx").arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    if (debug) qDebug() << PDEBUG << ":" << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);

    cache.beginGroup(type);
    QStringList cachedValues;
    for (int i=0; i<cache.allKeys().count(); i++)
        cachedValues.append(cache.value(cache.allKeys()[i]).toString());

    if (type == QString("Hdd")) {
        QStringList allDevices = QDir(QString("/dev")).entryList(QDir::System, QDir::Name);
        QStringList devices = allDevices.filter(QRegExp(QString("^[hms]d[a-z]$")));
        for (int i=0; i<devices.count(); i++) {
            QString device = QString("/dev/%1").arg(devices[i]);
            if (cachedValues.contains(device)) continue;
            if (debug) qDebug() << PDEBUG << ":" << "Found new key" << device << "for type" << type;
            cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), device);
        }
    } else if (type == QString("Network")) {
        QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
        for (int i=0; i<rawInterfaceList.count(); i++) {
            QString device = rawInterfaceList[i].name();
            if (cachedValues.contains(device)) continue;
            if (debug) qDebug() << PDEBUG << ":" << "Found new key" << device << "for type" << type;
            cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), device);
        }
    } else {
        if (cachedValues.contains(key)) return;
        if (debug) qDebug() << PDEBUG << ":" << "Found new key" << key << "for type" << type;
        cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), key);
    }
    cache.endGroup();

    cache.sync();
    loadKeysFromCache();
    return reinitKeys();
}


QString AWKeys::htmlValue(QString key) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Requested key" << key;

    QString value = values[key];
    if (!key.startsWith(QString("custom")))
        value.replace(QString(" "), QString("&nbsp;"));
    return value;
}


int AWKeys::numberCpus() const
{
    if (debug) qDebug() << PDEBUG;

    return QThread::idealThreadCount();
}


float AWKeys::temperature(const float temp, const QString units) const
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


QStringList AWKeys::findGraphicalItems() const
{
    if (debug) qDebug() << PDEBUG;

    QStringList orderedKeys;
    for (int i=0; i<graphicalItems->items().count(); i++)
        orderedKeys.append(graphicalItems->items()[i]->name() + graphicalItems->items()[i]->bar());
    orderedKeys.sort();

    QStringList selectedKeys;
    for (int i=orderedKeys.count()-1; i>=0; i--)
        if (pattern.contains(QString("$%1").arg(orderedKeys[i]))) {
            if (debug) qDebug() << PDEBUG << ":" << "Found key" << orderedKeys[i];
            selectedKeys.append(orderedKeys[i]);
        }

    return selectedKeys;
}


QStringList AWKeys::findKeys() const
{
    QStringList selectedKeys;
    for (int i=0; i<keys.count(); i++) {
        if (keys[i].startsWith(QString("bar"))) continue;
        if (pattern.contains(QString("$%1").arg(keys[i]))) {
            if (debug) qDebug() << PDEBUG << ":" << "Found key" << keys[i];
            selectedKeys.append(keys[i]);
        }
    }

    return selectedKeys;
}


GraphicalItem *AWKeys::getItemByTag(const QString tag) const
{
    if (debug) qDebug() << PDEBUG;

    GraphicalItem *item = nullptr;
    for (int i=0; i< graphicalItems->items().count(); i++) {
        if ((graphicalItems->items()[i]->name() + graphicalItems->items()[i]->bar()) != tag) continue;
        item = graphicalItems->items()[i];
        break;
    }

    return item;
}


QStringList AWKeys::getTimeKeys() const
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
