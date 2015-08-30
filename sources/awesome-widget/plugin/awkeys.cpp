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
#include <QDir>
#include <QInputDialog>
#include <QLocale>
#include <QNetworkInterface>
#include <QRegExp>
#include <QScriptEngine>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

#include "awactions.h"
#include "awdebug.h"
#include "awtooltip.h"
#include "extquotes.h"
#include "extscript.h"
#include "extupgrade.h"
#include "extweather.h"
#include "graphicalitem.h"


AWKeys::AWKeys(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW);

    // logging
    qSetMessagePattern(LOG_FORMAT);

    // backend
    graphicalItems = new ExtItemAggregator<GraphicalItem>(nullptr, QString("desktops"));
    extQuotes = new ExtItemAggregator<ExtQuotes>(nullptr, QString("quotes"));
    extScripts = new ExtItemAggregator<ExtScript>(nullptr, QString("scripts"));
    extUpgrade = new ExtItemAggregator<ExtUpgrade>(nullptr, QString("upgrade"));
    extWeather = new ExtItemAggregator<ExtWeather>(nullptr, QString("weather"));
    connect(this, SIGNAL(needToBeUpdated()), this, SLOT(dataUpdate()));
}


AWKeys::~AWKeys()
{
    qCDebug(LOG_AW);

    if (toolTip != nullptr)  delete toolTip;

    delete graphicalItems;
    delete extQuotes;
    delete extScripts;
    delete extUpgrade;
    delete extWeather;
}


void AWKeys::initKeys(const QString currentPattern)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Pattern" << currentPattern;

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
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Tooltip parameters" << tooltipParams;

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
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Is popup enabled" << popup;

    enablePopup = popup;
}


void AWKeys::setTranslateStrings(const bool translate)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Is translation enabled" << translate;

    translateStrings = translate;
}


void AWKeys::setWrapNewLines(const bool wrap)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Is wrapping enabled" << wrap;

    wrapNewLines = wrap;
}


QSize AWKeys::toolTipSize() const
{
    qCDebug(LOG_AW);
    if (toolTip == nullptr) return QSize();

    return toolTip->getSize();
}


void AWKeys::addDevice(const QString source)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Source" << source;

    QRegExp diskRegexp = QRegExp(QString("disk/(?:md|sd|hd)[a-z|0-9]_.*/Rate/(?:rblk)"));
    QRegExp mountRegexp = QRegExp(QString("partitions/.*/filllevel"));

    if (source.contains(diskRegexp)) {
        QString device = source;
        device.remove(QString("/Rate/rblk"));
        addKeyToCache(QString("Disk"), device);
    } else if (source.contains(mountRegexp)) {
        QString device = source;
        device.remove(QString("partitions")).remove(QString("/filllevel"));
        addKeyToCache(QString("Mount"), device);
    } else if (source.startsWith(QString("lmsensors"))) {
        addKeyToCache(QString("Temp"), source);
    }
}


QStringList AWKeys::dictKeys(const bool sorted, const QString regexp) const
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Should be sorted" << sorted;
    qCDebug(LOG_AW) << "Filter" << regexp;

    QStringList allKeys;
    // weather
    for (int i=extWeather->items().count()-1; i>=0; i--) {
        if (!extWeather->items().at(i)->isActive()) continue;
        allKeys.append(extWeather->items().at(i)->tag(QString("weatherId")));
        allKeys.append(extWeather->items().at(i)->tag(QString("weather")));
        allKeys.append(extWeather->items().at(i)->tag(QString("humidity")));
        allKeys.append(extWeather->items().at(i)->tag(QString("pressure")));
        allKeys.append(extWeather->items().at(i)->tag(QString("temperature")));
        allKeys.append(extWeather->items().at(i)->tag(QString("timestamp")));
    }
    // time
    allKeys.append(QString("time"));
    allKeys.append(QString("isotime"));
    allKeys.append(QString("shorttime"));
    allKeys.append(QString("longtime"));
    allKeys.append(QString("ctime"));
    // uptime
    allKeys.append(QString("uptime"));
    allKeys.append(QString("cuptime"));
    // cpuclock & cpu
    for (int i=QThread::idealThreadCount()-1; i>=0; i--) {
        allKeys.append(QString("cpucl%1").arg(i));
        allKeys.append(QString("cpu%1").arg(i));
    }
    allKeys.append(QString("cpucl"));
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
        allKeys.append(QString("downunits%1").arg(i));
        allKeys.append(QString("upunits%1").arg(i));
        allKeys.append(QString("downkb%1").arg(i));
        allKeys.append(QString("down%1").arg(i));
        allKeys.append(QString("upkb%1").arg(i));
        allKeys.append(QString("up%1").arg(i));
    }
    allKeys.append(QString("downunits"));
    allKeys.append(QString("upunits"));
    allKeys.append(QString("downkb"));
    allKeys.append(QString("down"));
    allKeys.append(QString("upkb"));
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
        if (!extUpgrade->items().at(i)->isActive()) continue;
        allKeys.append(extUpgrade->items().at(i)->tag(QString("pkgcount")));
    }
    // quotes
    for (int i=extQuotes->items().count()-1; i>=0; i--) {
        if (!extQuotes->items().at(i)->isActive()) continue;
        allKeys.append(extQuotes->items().at(i)->tag(QString("ask")));
        allKeys.append(extQuotes->items().at(i)->tag(QString("askchg")));
        allKeys.append(extQuotes->items().at(i)->tag(QString("percaskchg")));
        allKeys.append(extQuotes->items().at(i)->tag(QString("bid")));
        allKeys.append(extQuotes->items().at(i)->tag(QString("bidchg")));
        allKeys.append(extQuotes->items().at(i)->tag(QString("percbidchg")));
        allKeys.append(extQuotes->items().at(i)->tag(QString("price")));
        allKeys.append(extQuotes->items().at(i)->tag(QString("pricechg")));
        allKeys.append(extQuotes->items().at(i)->tag(QString("percpricechg")));
    }
    // custom
    for (int i=extScripts->items().count()-1; i>=0; i--) {
        if (!extScripts->items().at(i)->isActive()) continue;
        allKeys.append(extScripts->items().at(i)->tag(QString("custom")));
    }
    // desktop
    allKeys.append(QString("desktop"));
    allKeys.append(QString("ndesktop"));
    allKeys.append(QString("tdesktops"));
    // load average
    allKeys.append(QString("la15"));
    allKeys.append(QString("la5"));
    allKeys.append(QString("la1"));
    // bars
    QStringList graphicalItemsKeys;
    foreach(GraphicalItem *item, graphicalItems->items())
        graphicalItemsKeys.append(QString("%1%2").arg(item->name()).arg(item->bar()));
    graphicalItemsKeys.sort();
    for (int i=graphicalItemsKeys.count()-1; i>=0; i--)
        allKeys.append(graphicalItemsKeys.at(i));

    // sort if required
    if (sorted) allKeys.sort();

    return allKeys.filter(QRegExp(regexp));
}


QStringList AWKeys::getHddDevices() const
{
    qCDebug(LOG_AW);

    QStringList devices = hddDevices;
    // required by ui interface
    devices.insert(0, QString("disable"));
    devices.insert(0, QString("auto"));

    return devices;
}


void AWKeys::setDataBySource(const QString sourceName, const QVariantMap data,
                             const QVariantMap params)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Source" << sourceName;
    qCDebug(LOG_AW) << "Data" << data;

    if (sourceName == QString("update")) return emit(needToBeUpdated());

    // regular expressions
    QRegExp cpuRegExp = QRegExp(QString("cpu/cpu.*/TotalLoad"));
    QRegExp cpuclRegExp = QRegExp(QString("cpu/cpu.*/clock"));
    QRegExp hddrRegExp = QRegExp(QString("disk/.*/Rate/rblk"));
    QRegExp hddwRegExp = QRegExp(QString("disk/.*/Rate/wblk"));
    QRegExp mountFillRegExp = QRegExp(QString("partitions/.*/filllevel"));
    QRegExp mountFreeRegExp = QRegExp(QString("partitions/.*/freespace"));
    QRegExp mountUsedRegExp = QRegExp(QString("partitions/.*/usedspace"));
    QRegExp netRegExp = QRegExp(QString("network/interfaces/.*/(receiver|transmitter)/data$"));

    if (sourceName == QString("battery")) {
        // battery
        foreach(QString key, data.keys()) {
            if (key == QString("ac")) {
                // notification
                if ((values[QString("ac")] == params[QString("acOnline")].toString()) != data[QString("ac")].toBool()) {
                    if (data[QString("ac")].toBool())
                        AWActions::sendNotification(QString("event"), i18n("AC online"), enablePopup);
                    else
                        AWActions::sendNotification(QString("event"), i18n("AC offline"), enablePopup);
                }
                // value
                if (data[QString("ac")].toBool())
                    values[QString("ac")] = params[QString("acOnline")].toString();
                else
                    values[QString("ac")] = params[QString("acOffline")].toString();
            } else
                values[key] = QString("%1").arg(data[key].toFloat(), 3, 'f', 0);
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
        foreach(QString key, data.keys()) values[key] = data[key].toString();
    } else if (sourceName == QString("desktop")) {
        // desktops
        values[QString("desktop")] = data[QString("currentName")].toString();
        values[QString("ndesktop")] = QString("%1").arg(data[QString("currentNumber")].toInt());
        values[QString("tdesktops")] = QString("%1").arg(data[QString("number")].toInt());
    } else if (sourceName.contains(hddrRegExp)) {
        // read speed
        QString device = sourceName;
        device.remove(QString("/Rate/rblk"));
        for (int i=0; i<diskDevices.count(); i++) {
            if (diskDevices.at(i) != device) continue;
            values[QString("hddr%1").arg(i)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 0);
            break;
        }
    } else if (sourceName.contains(hddwRegExp)) {
        // write speed
        QString device = sourceName;
        device.remove(QString("/Rate/wblk"));
        for (int i=0; i<diskDevices.count(); i++) {
            if (diskDevices.at(i) != device) continue;
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
        for (int i=0; i<mountDevices.count(); i++) {
            if (mountDevices.at(i) != mount) continue;
            if ((data[QString("value")].toFloat() >= 90.0) && (values[QString("hdd%1").arg(i)].toFloat() < 90.0))
                AWActions::sendNotification(QString("event"), i18n("Free space on %1 less than 10%", mount), enablePopup);
            values[QString("hdd%1").arg(i)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
            break;
        }
    } else if (sourceName.contains(mountFreeRegExp)) {
        // free space
        QString mount = sourceName;
        mount.remove(QString("partitions")).remove(QString("/freespace"));
        for (int i=0; i<mountDevices.count(); i++) {
            if (mountDevices.at(i) != mount) continue;
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
        for (int i=0; i<mountDevices.count(); i++) {
            if (mountDevices.at(i) != mount) continue;
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
        foreach(QString key, data.keys())
            for (int i=0; i<hddDevices.count(); i++) {
                if (hddDevices.at(i) != key) continue;
                values[QString("hddtemp%1").arg(i)] = QString("%1").arg(
                    temperature(data[key].toFloat(), params[QString("tempUnits")].toString()), 4, 'f', 1);
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
        if ((!isnan(value)) && (value >= 90.0) && (values[QString("mem")].toFloat() < 90.0))
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
    } else if (sourceName.contains(netRegExp)) {
        // network speed
        QString type = sourceName.contains(QString("receiver")) ? QString("down") : QString("up");
        // device name
        QString device = sourceName.split(QChar('/'))[2];
        // values
        float value = data[QString("value")].toFloat();
        QString simplifiedValue = value > 1000.0 ?
            QString("%1").arg(value / 1024.0, 4, 'f', 1) :
            QString("%1").arg(value, 4, 'f', 0);
        // units
        QString units;
        if (translateStrings)
            units = value > 1000.0 ? i18n("MB/s") : i18n("KB/s");
        else
            units = value > 1000.0 ? QString("MB/s") : QString("KB/s");
        // update
        for (int i=0; i<networkDevices.count(); i++) {
            if (networkDevices.at(i) != device) continue;
            values[QString("%1kb%2").arg(type).arg(i)] = QString("%1").arg(value,  4, 'f', 0);
            values[QString("%1%2").arg(type).arg(i)] = simplifiedValue;
            values[QString("%1units%2").arg(type).arg(i)] = units;
            break;
        }
        if (device == values[QString("netdev")]) {
            values[QString("%1kb").arg(type)] = QString("%1").arg(value, 4, 'f', 0);
            values[type] = simplifiedValue;
            values[QString("%1units").arg(type)] = units;
        }
    } else if (sourceName == QString("pkg")) {
        // package manager
        foreach(QString key, data.keys()) values[key] = QString("%1").arg(data[key].toInt(), 2);
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
        foreach(QString key, data.keys()) values[key] = QString("%1").arg(data[key].toFloat(), 7, 'f');
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
        if ((!isnan(value)) && (value > 0.0) && (values[QString("swap")].toFloat() == 0.0))
            AWActions::sendNotification(QString("event"), i18n("Swap is used"), enablePopup);
        // value
        values[QString("swap")] = QString("%1").arg(value, 5, 'f', 1);
    } else if (sourceName.startsWith(QString("lmsensors/"))) {
        // temperature devices
        for (int i=0; i<tempDevices.count(); i++) {
            if (sourceName != tempDevices.at(i)) continue;
            float temp = data[QString("units")].toString() == QString("°C") ?
                temperature(data[QString("value")].toFloat(), params[QString("tempUnits")].toString()) :
                data[QString("value")].toFloat();
            values[QString("temp%1").arg(i)] = QString("%1").arg(temp, 4, 'f', 1);
            break;
        }
    } else if (sourceName == QString("Local")) {
        // init locale
        QLocale loc = translateStrings ? QLocale::system() : QLocale::c();
        QDateTime dt = data[QString("DateTime")].toDateTime();
        // time
        values[QString("time")] = dt.toString(Qt::TextDate);
        values[QString("isotime")] = dt.toString(Qt::ISODate);
        values[QString("shorttime")] = loc.toString(dt, QLocale::ShortFormat);
        values[QString("longtime")] = loc.toString(dt, QLocale::LongFormat);
        values[QString("ctime")] = params[QString("customTime")].toString();
        foreach(QString key, timeKeys)
            values[QString("ctime")].replace(QString("$%1").arg(key), loc.toString(dt, key));
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
        foreach(QString key, data.keys()) {
            if (key.startsWith(QString("weatherId")))
                values[key] = QString("%1").arg(data[key].toInt(), 3);
            else if (key.startsWith(QString("weather")))
                values[key] = data[key].toString();
            else if (key.startsWith(QString("humidity")))
                values[key] = QString("%1").arg(data[key].toInt(), 3);
            else if (key.startsWith(QString("pressure")))
                values[key] = QString("%1").arg(data[key].toInt(), 4);
            else if (key.startsWith(QString("temperature")))
                values[key] = QString("%1").arg(
                    temperature(data[key].toFloat(), params[QString("tempUnits")].toString()), 4, 'f', 1);
        }
    } else {
        qCDebug(LOG_AW) << "Source" << sourceName << "not found";
        emit(dropSourceFromDataengine(sourceName));
    }
}


void AWKeys::graphicalValueByKey() const
{
    qCDebug(LOG_AW);

    bool ok;
    QString tag = QInputDialog::getItem(nullptr, i18n("Select tag"), i18n("Tag"),
                                        dictKeys(true), 0, true, &ok);

    if ((!ok) || (tag.isEmpty())) return;
    QString message = i18n("Tag: %1", tag);
    message += QString("<br>");
    message += i18n("Value: %1", valueByKey(tag));

    return AWActions::sendNotification(QString("tag"), message);
}


QString AWKeys::infoByKey(QString key) const
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Requested key" << key;

    key.remove(QRegExp(QString("^bar[0-9]{1,}")));
    if (key.startsWith(QString("custom")))
        foreach(ExtScript *item, extScripts->items()) {
            if (item->tag(QString("custom")) != key) continue;
            return item->executable();
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
        foreach(ExtUpgrade *item, extUpgrade->items()) {
            if (item->tag(QString("pkgcount")) != key) continue;
            return item->executable();
        }
    else if (key.contains(QRegExp(QString("(^|perc)(ask|bid|price)(chg|)"))))
        foreach(ExtQuotes *item, extQuotes->items()) {
            if (item->number() != key.remove(QRegExp(QString("(^|perc)(ask|bid|price)(chg|)"))).toInt()) continue;
            return item->ticker();
        }
    else if (key.startsWith(QString("temp")))
        return QString("%1").arg(tempDevices[key.remove(QString("temp")).toInt()]);

    return QString("(none)");
}


QString AWKeys::valueByKey(QString key) const
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Requested key" << key;

    return values[key.remove(QRegExp(QString("^bar[0-9]{1,}")))];
}


void AWKeys::editItem(const QString type)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Item type" << type;

    if (type == QString("graphicalitem")) {
        graphicalItems->setConfigArgs(dictKeys(true, QString("^(cpu(?!cl).*|gpu$|mem$|swap$|hdd[0-9].*|bat.*)")));
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
    qCDebug(LOG_AW);

    emit(needTextToBeUpdated(parsePattern()));
    if (toolTip != nullptr) emit(toolTip->updateData(values));
}


void AWKeys::loadKeysFromCache()
{
    qCDebug(LOG_AW);

    QString fileName = QString("%1/awesomewidgets.ndx")
                       .arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    qCInfo(LOG_AW) << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);

    cache.beginGroup(QString("Disk"));
    diskDevices.clear();
    foreach(QString key, cache.allKeys())
        diskDevices.append(cache.value(key).toString());
    cache.endGroup();

    cache.beginGroup(QString("Hdd"));
    hddDevices.clear();
    foreach(QString key, cache.allKeys())
        hddDevices.append(cache.value(key).toString());
    cache.endGroup();

    cache.beginGroup(QString("Mount"));
    mountDevices.clear();
    foreach(QString key, cache.allKeys())
        mountDevices.append(cache.value(key).toString());
    cache.endGroup();

    cache.beginGroup(QString("Network"));
    networkDevices.clear();
    foreach(QString key, cache.allKeys())
        networkDevices.append(cache.value(key).toString());
    cache.endGroup();

    cache.beginGroup(QString("Temp"));
    tempDevices.clear();
    foreach(QString key, cache.allKeys())
        tempDevices.append(cache.value(key).toString());
    cache.endGroup();
}


void AWKeys::reinitKeys()
{
    qCDebug(LOG_AW);

    // init
    QStringList allKeys = dictKeys();

    // append lists
    // bars
    foundBars = [allKeys](QString pattern) {
        QStringList selectedKeys;
        foreach(QString key, allKeys)
            if ((key.startsWith(QString("bar"))) &&
                (pattern.contains(QString("$%1").arg(key)))) {
                qCInfo(LOG_AW) << "Found bar" << key;
                selectedKeys.append(key);
            }
        if (selectedKeys.isEmpty()) qCWarning(LOG_AW) << "No bars found";
        return selectedKeys;
    }(pattern);

    // main key list
    foundKeys = [allKeys](QString pattern) {
        QStringList selectedKeys;
        foreach(QString key, allKeys)
            if ((!key.startsWith(QString("bar"))) &&
                (pattern.contains(QString("$%1").arg(key)))) {
                qCInfo(LOG_AW) << "Found key" << key;
                selectedKeys.append(key);
            }
        if (selectedKeys.isEmpty()) qCWarning(LOG_AW) << "No keys found";
        return selectedKeys;
    }(pattern);

    // lambdas
    foundLambdas = [](QString pattern) {
        QStringList selectedKeys;
        // substring inside ${{ }} (with brackets) which should not contain ${{
        QRegularExpression lambdaRegexp(QString("\\$\\{\\{((?!\\$\\{\\{).)*?\\}\\}"));
        lambdaRegexp.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

        QRegularExpressionMatchIterator it = lambdaRegexp.globalMatch(pattern);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString lambda = match.captured();
            // drop brakets
            lambda.remove(QRegExp(QString("^\\$\\{\\{")));
            lambda.remove(QRegExp(QString("\\}\\}$")));
            // append
            qCInfo(LOG_AW) << "Found lambda" << lambda;
            selectedKeys.append(lambda);
        }
        if (selectedKeys.isEmpty()) qCWarning(LOG_AW) << "No lambdas found";
        return selectedKeys;
    }(pattern);
}


void AWKeys::addKeyToCache(const QString type, const QString key)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Key type" << type;
    qCDebug(LOG_AW) << "Key" << key;

    QString fileName = QString("%1/awesomewidgets.ndx").arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    qCInfo(LOG_AW) << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);

    cache.beginGroup(type);
    QStringList cachedValues;
    foreach(QString key, cache.allKeys())
        cachedValues.append(cache.value(key).toString());

    if (type == QString("Hdd")) {
        QStringList allDevices = QDir(QString("/dev")).entryList(QDir::System, QDir::Name);
        QStringList devices = allDevices.filter(QRegExp(QString("^[hms]d[a-z]$")));
        foreach(QString dev, devices) {
            QString device = QString("/dev/%1").arg(dev);
            if (cachedValues.contains(device)) continue;
            qCInfo(LOG_AW) << "Found new key" << device << "for type" << type;
            cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), device);
        }
    } else if (type == QString("Network")) {
        QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
        foreach(QNetworkInterface interface, rawInterfaceList) {
            QString device = interface.name();
            if (cachedValues.contains(device)) continue;
            qCInfo(LOG_AW) << "Found new key" << device << "for type" << type;
            cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), device);
        }
    } else {
        if (cachedValues.contains(key)) return;
        qCInfo(LOG_AW) << "Found new key" << key << "for type" << type;
        cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), key);
    }
    cache.endGroup();

    cache.sync();
    loadKeysFromCache();
    return reinitKeys();
}


QString AWKeys::parsePattern() const
{
    qCDebug(LOG_AW);

    QString parsed = pattern;
    parsed.replace(QString("$$"), QString("$\\$\\"));

    // lambdas
    foreach(QString key, foundLambdas)
        parsed.replace(QString("${{%1}}").arg(key), [this](QString key) {
            QScriptEngine engine;
            foreach(QString lambdaKey, foundKeys)
                key.replace(QString("$%1").arg(lambdaKey), values[lambdaKey]);
            qCInfo(LOG_AW) << "Expression" << key;
            QScriptValue result = engine.evaluate(key);
            if (engine.hasUncaughtException()) {
                int line = engine.uncaughtExceptionLineNumber();
                qCWarning(LOG_AW) << "Uncaught exception at line" << line << ":" << result.toString();
                return QString();
            } else
                return result.toString();
        }(key));

    // main keys
    foreach(QString key, foundKeys)
        parsed.replace(QString("$%1").arg(key), [](QString key, QString value) {
            if ((!key.startsWith(QString("custom"))) &&
                (!key.startsWith(QString("weather"))))
                value.replace(QString(" "), QString("&nbsp;"));
            return value;
        }(key, values[key]));

    // bars
    foreach(QString bar, foundBars)
        parsed.replace(QString("$%1").arg(bar),
                       graphicalItems->itemByTag(bar)->image(valueByKey(bar).toFloat()));

    // prepare strings
    parsed.replace(QString("$\\$\\"), QString("$$"));
    if (wrapNewLines) parsed.replace(QString("\n"), QString("<br>"));

    return parsed;
}


float AWKeys::temperature(const float temp, const QString units) const
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Temperature value" << temp;
    qCDebug(LOG_AW) << "Temperature units" << units;

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
