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

#include "awkeyoperations.h"

#include <QDir>
#include <QJSEngine>
#include <QRegExp>
#include <QThread>

#include "awdebug.h"
#include "awkeycache.h"
#include "awpatternfunctions.h"
#include "version.h"
// extensions
#include "extquotes.h"
#include "extscript.h"
#include "extupgrade.h"
#include "extweather.h"
#include "graphicalitem.h"


AWKeyOperations::AWKeyOperations(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


AWKeyOperations::~AWKeyOperations()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    // extensions
    delete graphicalItems;
    delete extQuotes;
    delete extScripts;
    delete extUpgrade;
    delete extWeather;
}


QStringList AWKeyOperations::devices(const QString type) const
{
    qCDebug(LOG_AW) << "Looking for type" << type;

    return m_devices[type];
}


QHash<QString, QStringList> AWKeyOperations::devices() const
{
    return m_devices;
}


void AWKeyOperations::updateCache()
{
    // update network and hdd list
    addKeyToCache(QString("hdd"));
    addKeyToCache(QString("net"));
}


QStringList AWKeyOperations::dictKeys() const
{
    QStringList allKeys;
    // weather
    for (int i = extWeather->activeItems().count() - 1; i >= 0; i--) {
        allKeys.append(
            extWeather->activeItems().at(i)->tag(QString("weatherId")));
        allKeys.append(
            extWeather->activeItems().at(i)->tag(QString("weather")));
        allKeys.append(
            extWeather->activeItems().at(i)->tag(QString("humidity")));
        allKeys.append(
            extWeather->activeItems().at(i)->tag(QString("pressure")));
        allKeys.append(
            extWeather->activeItems().at(i)->tag(QString("temperature")));
        allKeys.append(
            extWeather->activeItems().at(i)->tag(QString("timestamp")));
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
    for (int i = QThread::idealThreadCount() - 1; i >= 0; i--) {
        allKeys.append(QString("cpucl%1").arg(i));
        allKeys.append(QString("cpu%1").arg(i));
    }
    allKeys.append(QString("cpucl"));
    allKeys.append(QString("cpu"));
    // temperature
    for (int i = m_devices[QString("temp")].count() - 1; i >= 0; i--)
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
    for (int i = m_devices[QString("mount")].count() - 1; i >= 0; i--) {
        allKeys.append(QString("hddmb%1").arg(i));
        allKeys.append(QString("hddgb%1").arg(i));
        allKeys.append(QString("hddfreemb%1").arg(i));
        allKeys.append(QString("hddfreegb%1").arg(i));
        allKeys.append(QString("hddtotmb%1").arg(i));
        allKeys.append(QString("hddtotgb%1").arg(i));
        allKeys.append(QString("hdd%1").arg(i));
    }
    // hdd speed
    for (int i = m_devices[QString("disk")].count() - 1; i >= 0; i--) {
        allKeys.append(QString("hddr%1").arg(i));
        allKeys.append(QString("hddw%1").arg(i));
    }
    // hdd temp
    for (int i = m_devices[QString("hdd")].count() - 1; i >= 0; i--)
        allKeys.append(QString("hddtemp%1").arg(i));
    // network
    for (int i = m_devices[QString("net")].count() - 1; i >= 0; i--) {
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
    QStringList allBatteryDevices
        = QDir(QString("/sys/class/power_supply"))
              .entryList(QStringList() << QString("BAT*"),
                         QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (int i = allBatteryDevices.count() - 1; i >= 0; i--)
        allKeys.append(QString("bat%1").arg(i));
    allKeys.append(QString("bat"));
    // player
    allKeys.append(QString("album"));
    allKeys.append(QString("artist"));
    allKeys.append(QString("duration"));
    allKeys.append(QString("progress"));
    allKeys.append(QString("title"));
    allKeys.append(QString("dalbum"));
    allKeys.append(QString("dartist"));
    allKeys.append(QString("dtitle"));
    allKeys.append(QString("salbum"));
    allKeys.append(QString("sartist"));
    allKeys.append(QString("stitle"));
    // ps
    allKeys.append(QString("pscount"));
    allKeys.append(QString("pstotal"));
    allKeys.append(QString("ps"));
    // package manager
    for (int i = extUpgrade->activeItems().count() - 1; i >= 0; i--)
        allKeys.append(
            extUpgrade->activeItems().at(i)->tag(QString("pkgcount")));
    // quotes
    for (int i = extQuotes->activeItems().count() - 1; i >= 0; i--) {
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("ask")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("askchg")));
        allKeys.append(
            extQuotes->activeItems().at(i)->tag(QString("percaskchg")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("bid")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("bidchg")));
        allKeys.append(
            extQuotes->activeItems().at(i)->tag(QString("percbidchg")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("price")));
        allKeys.append(
            extQuotes->activeItems().at(i)->tag(QString("pricechg")));
        allKeys.append(
            extQuotes->activeItems().at(i)->tag(QString("percpricechg")));
    }
    // custom
    for (int i = extScripts->activeItems().count() - 1; i >= 0; i--)
        allKeys.append(extScripts->activeItems().at(i)->tag(QString("custom")));
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
    for (auto item : graphicalItems->items())
        graphicalItemsKeys.append(item->tag());
    graphicalItemsKeys.sort();
    for (int i = graphicalItemsKeys.count() - 1; i >= 0; i--)
        allKeys.append(graphicalItemsKeys.at(i));

    return allKeys;
}


// this method is required to provide GraphicalItem functions (e.g. paint()) to
// parent classes
GraphicalItem *AWKeyOperations::giByKey(const QString key) const
{
    qCDebug(LOG_AW) << "Looking for item" << key;

    return graphicalItems->itemByTag(key);
}


QString AWKeyOperations::infoByKey(QString key) const
{
    qCDebug(LOG_AW) << "Requested key" << key;

    key.remove(QRegExp(QString("^bar[0-9]{1,}")));
    if (key.startsWith(QString("custom")))
        return extScripts->itemByTagNumber(
                             key.remove(QString("custom")).toInt())
            ->uniq();
    else if (key.contains(QRegExp(QString("^hdd[rw]"))))
        return QString("%1").arg(m_devices[QString(
            "disk")][key.remove(QRegExp(QString("hdd[rw]"))).toInt()]);
    else if (key.contains(QRegExp(
                 QString("^hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb)"))))
        return QString("%1").arg(m_devices[QString(
            "mount")][key
                          .remove(QRegExp(QString(
                              "^hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb)")))
                          .toInt()]);
    else if (key.startsWith(QString("hddtemp")))
        return QString("%1").arg(
            m_devices[QString("hdd")][key.remove(QString("hddtemp")).toInt()]);
    else if (key.contains(QRegExp(QString("^(down|up)[0-9]"))))
        return QString("%1").arg(m_devices[QString(
            "net")][key.remove(QRegExp(QString("^(down|up)"))).toInt()]);
    else if (key.startsWith(QString("pkgcount")))
        return extUpgrade->itemByTagNumber(
                             key.remove(QString("pkgcount")).toInt())
            ->uniq();
    else if (key.contains(QRegExp(QString("(^|perc)(ask|bid|price)(chg|)"))))
        return extQuotes->itemByTagNumber(
                            key.remove(QRegExp(QString(
                                           "(^|perc)(ask|bid|price)(chg|)")))
                                .toInt())
            ->uniq();
    else if (key.contains(QRegExp(
                 QString("(weather|weatherId|humidity|pressure|temperature)"))))
        return extWeather
            ->itemByTagNumber(
                key
                    .remove(QRegExp(QString(
                        "(weather|weatherId|humidity|pressure|temperature)")))
                    .toInt())
            ->uniq();
    else if (key.startsWith(QString("temp")))
        return QString("%1").arg(
            m_devices[QString("temp")][key.remove(QString("temp")).toInt()]);

    return QString("(none)");
}


QString AWKeyOperations::pattern() const
{
    return m_pattern;
}


void AWKeyOperations::setPattern(const QString currentPattern)
{
    qCDebug(LOG_AW) << "Set pattern" << currentPattern;

    m_pattern = currentPattern;
}


void AWKeyOperations::editItem(const QString type)
{
    qCDebug(LOG_AW) << "Item type" << type;

    if (type == QString("graphicalitem")) {
        QStringList keys = dictKeys().filter(QRegExp(
            QString("^(cpu(?!cl).*|gpu$|mem$|swap$|hdd[0-9].*|bat.*)")));
        keys.sort();
        graphicalItems->setConfigArgs(keys);
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


void AWKeyOperations::addDevice(const QString &source)
{
    qCDebug(LOG_AW) << "Source" << source;

    QRegExp diskRegexp
        = QRegExp(QString("disk/(?:md|sd|hd)[a-z|0-9]_.*/Rate/(?:rblk)"));
    QRegExp mountRegexp = QRegExp(QString("partitions/.*/filllevel"));

    if (source.contains(diskRegexp)) {
        QString device = source;
        device.remove(QString("/Rate/rblk"));
        addKeyToCache(QString("disk"), device);
    } else if (source.contains(mountRegexp)) {
        QString device = source;
        device.remove(QString("partitions")).remove(QString("/filllevel"));
        addKeyToCache(QString("mount"), device);
    } else if (source.startsWith(QString("lmsensors"))) {
        addKeyToCache(QString("temp"), source);
    }
}


void AWKeyOperations::addKeyToCache(const QString type, const QString key)
{
    qCDebug(LOG_AW) << "Key" << key << "with type" << type;

    if (AWKeyCache::addKeyToCache(type, key)) {
        m_devices = AWKeyCache::loadKeysFromCache();
        reinitKeys();
    }
}


void AWKeyOperations::reinitKeys()
{
    // renew extensions
    // delete them if any
    delete graphicalItems;
    graphicalItems = nullptr;
    delete extQuotes;
    extQuotes = nullptr;
    delete extScripts;
    extScripts = nullptr;
    delete extUpgrade;
    extUpgrade = nullptr;
    delete extWeather;
    extWeather = nullptr;
    // create
    graphicalItems
        = new ExtItemAggregator<GraphicalItem>(nullptr, QString("desktops"));
    extQuotes = new ExtItemAggregator<ExtQuotes>(nullptr, QString("quotes"));
    extScripts = new ExtItemAggregator<ExtScript>(nullptr, QString("scripts"));
    extUpgrade = new ExtItemAggregator<ExtUpgrade>(nullptr, QString("upgrade"));
    extWeather = new ExtItemAggregator<ExtWeather>(nullptr, QString("weather"));

    // init
    QStringList allKeys = dictKeys();

    // apply aw_* functions
    m_pattern = AWPatternFunctions::insertAllKeys(m_pattern, allKeys);
    m_pattern = AWPatternFunctions::insertKeyCount(m_pattern, allKeys);
    m_pattern = AWPatternFunctions::insertKeyNames(m_pattern, allKeys);
    m_pattern = AWPatternFunctions::insertKeys(m_pattern, allKeys);
    // wrap templates
    m_pattern = AWPatternFunctions::expandTemplates(m_pattern);

    emit(updateKeys(allKeys));
}