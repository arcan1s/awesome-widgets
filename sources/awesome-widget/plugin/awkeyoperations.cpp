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
// extensions
#include "extnetworkrequest.h"
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
    delete m_graphicalItems;
    delete m_extNetRequest;
    delete m_extQuotes;
    delete m_extScripts;
    delete m_extUpgrade;
    delete m_extWeather;
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
    for (auto item : m_extWeather->activeItems()) {
        allKeys.append(item->tag(QString("weatherId")));
        allKeys.append(item->tag(QString("weather")));
        allKeys.append(item->tag(QString("humidity")));
        allKeys.append(item->tag(QString("pressure")));
        allKeys.append(item->tag(QString("temperature")));
        allKeys.append(item->tag(QString("timestamp")));
    }
    // cpuclock & cpu
    for (int i = 0; i < QThread::idealThreadCount(); i++) {
        allKeys.append(QString("cpucl%1").arg(i));
        allKeys.append(QString("cpu%1").arg(i));
    }
    // temperature
    for (int i = 0; i < m_devices[QString("temp")].count(); i++)
        allKeys.append(QString("temp%1").arg(i));
    // hdd
    for (int i = 0; i < m_devices[QString("mount")].count(); i++) {
        allKeys.append(QString("hddmb%1").arg(i));
        allKeys.append(QString("hddgb%1").arg(i));
        allKeys.append(QString("hddfreemb%1").arg(i));
        allKeys.append(QString("hddfreegb%1").arg(i));
        allKeys.append(QString("hddtotmb%1").arg(i));
        allKeys.append(QString("hddtotgb%1").arg(i));
        allKeys.append(QString("hdd%1").arg(i));
    }
    // hdd speed
    for (int i = 0; i < m_devices[QString("disk")].count(); i++) {
        allKeys.append(QString("hddr%1").arg(i));
        allKeys.append(QString("hddw%1").arg(i));
    }
    // hdd temp
    for (int i = 0; i < m_devices[QString("hdd")].count(); i++)
        allKeys.append(QString("hddtemp%1").arg(i));
    // network
    for (int i = 0; i < m_devices[QString("net")].count(); i++) {
        allKeys.append(QString("downunits%1").arg(i));
        allKeys.append(QString("upunits%1").arg(i));
        allKeys.append(QString("downtotalkb%1").arg(i));
        allKeys.append(QString("downtotal%1").arg(i));
        allKeys.append(QString("downkb%1").arg(i));
        allKeys.append(QString("down%1").arg(i));
        allKeys.append(QString("uptotalkb%1").arg(i));
        allKeys.append(QString("uptotal%1").arg(i));
        allKeys.append(QString("upkb%1").arg(i));
        allKeys.append(QString("up%1").arg(i));
    }
    // battery
    QStringList allBatteryDevices
        = QDir(QString("/sys/class/power_supply"))
              .entryList(QStringList() << QString("BAT*"),
                         QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (int i = 0; i < allBatteryDevices.count(); i++)
        allKeys.append(QString("bat%1").arg(i));
    // package manager
    for (auto item : m_extUpgrade->activeItems())
        allKeys.append(item->tag(QString("pkgcount")));
    // quotes
    for (auto item : m_extQuotes->activeItems()) {
        allKeys.append(item->tag(QString("ask")));
        allKeys.append(item->tag(QString("askchg")));
        allKeys.append(item->tag(QString("percaskchg")));
        allKeys.append(item->tag(QString("bid")));
        allKeys.append(item->tag(QString("bidchg")));
        allKeys.append(item->tag(QString("percbidchg")));
        allKeys.append(item->tag(QString("price")));
        allKeys.append(item->tag(QString("pricechg")));
        allKeys.append(item->tag(QString("percpricechg")));
    }
    // custom
    for (auto item : m_extScripts->activeItems())
        allKeys.append(item->tag(QString("custom")));
    // network requests
    for (auto item : m_extNetRequest->activeItems())
        allKeys.append(item->tag(QString("response")));
    // bars
    for (auto item : m_graphicalItems->activeItems())
        allKeys.append(item->tag(QString("bar")));
    // static keys
    allKeys.append(QString(STATIC_KEYS).split(QChar(',')));

    // sort in valid order
    allKeys.sort();
    std::reverse(allKeys.begin(), allKeys.end());

    return allKeys;
}


// this method is required to provide GraphicalItem functions (e.g. paint()) to
// parent classes
GraphicalItem *AWKeyOperations::giByKey(const QString key) const
{
    qCDebug(LOG_AW) << "Looking for item" << key;

    return m_graphicalItems->itemByTag(key, QString("bar"));
}


QString AWKeyOperations::infoByKey(QString key) const
{
    qCDebug(LOG_AW) << "Requested key" << key;

    QString stripped = key;
    stripped.remove(QRegExp(QString("\\d+")));
    QString output;

    if (key.startsWith(QString("bar"))) {
        AbstractExtItem *item = m_graphicalItems->itemByTag(key, stripped);
        if (item)
            output = item->uniq();
    } else if (key.startsWith(QString("custom"))) {
        AbstractExtItem *item = m_extScripts->itemByTag(key, stripped);
        if (item)
            output = item->uniq();
    } else if (key.contains(QRegExp(QString("^hdd[rw]")))) {
        output = m_devices[QString("disk")]
                          [key.remove(QRegExp(QString("hdd[rw]"))).toInt()];
    } else if (key.contains(QRegExp(
                   QString("^hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb)")))) {
        output
            = m_devices[QString("mount")]
                       [key
                            .remove(QRegExp(QString(
                                "^hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb)")))
                            .toInt()];
    } else if (key.startsWith(QString("hddtemp"))) {
        output
            = m_devices[QString("hdd")][key.remove(QString("hddtemp")).toInt()];
    } else if (key.contains(QRegExp(QString("^(down|up)[0-9]")))) {
        output = m_devices[QString("net")]
                          [key.remove(QRegExp(QString("^(down|up)"))).toInt()];
    } else if (key.startsWith(QString("pkgcount"))) {
        AbstractExtItem *item = m_extUpgrade->itemByTag(key, stripped);
        if (item)
            output = item->uniq();
    } else if (key.contains(
                   QRegExp(QString("(^|perc)(ask|bid|price)(chg|)")))) {
        AbstractExtItem *item = m_extQuotes->itemByTag(key, stripped);
        if (item)
            output = item->uniq();
    } else if (key.contains(QRegExp(QString(
                   "(weather|weatherId|humidity|pressure|temperature)")))) {
        AbstractExtItem *item = m_extWeather->itemByTag(key, stripped);
        if (item)
            output = item->uniq();
    } else if (key.startsWith(QString("temp"))) {
        output
            = m_devices[QString("temp")][key.remove(QString("temp")).toInt()];
    } else if (key.startsWith(QString("response"))) {
        AbstractExtItem *item = m_extNetRequest->itemByTag(key, stripped);
        if (item)
            output = item->uniq();
    } else {
        output = QString("(none)");
    }

    return output;
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
        m_graphicalItems->setConfigArgs(keys);
        return m_graphicalItems->editItems();
    } else if (type == QString("extnetworkrequest")) {
        return m_extNetRequest->editItems();
    } else if (type == QString("extquotes")) {
        return m_extQuotes->editItems();
    } else if (type == QString("extscript")) {
        return m_extScripts->editItems();
    } else if (type == QString("extupgrade")) {
        return m_extUpgrade->editItems();
    } else if (type == QString("extweather")) {
        return m_extWeather->editItems();
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
    delete m_graphicalItems;
    m_graphicalItems = nullptr;
    delete m_extNetRequest;
    m_extNetRequest = nullptr;
    delete m_extQuotes;
    m_extQuotes = nullptr;
    delete m_extScripts;
    m_extScripts = nullptr;
    delete m_extUpgrade;
    m_extUpgrade = nullptr;
    delete m_extWeather;
    m_extWeather = nullptr;
    // create
    m_graphicalItems
        = new ExtItemAggregator<GraphicalItem>(nullptr, QString("desktops"));
    m_extNetRequest = new ExtItemAggregator<ExtNetworkRequest>(
        nullptr, QString("requests"));
    m_extQuotes = new ExtItemAggregator<ExtQuotes>(nullptr, QString("quotes"));
    m_extScripts
        = new ExtItemAggregator<ExtScript>(nullptr, QString("scripts"));
    m_extUpgrade
        = new ExtItemAggregator<ExtUpgrade>(nullptr, QString("upgrade"));
    m_extWeather
        = new ExtItemAggregator<ExtWeather>(nullptr, QString("weather"));

    // init
    QStringList allKeys = dictKeys();

    // apply aw_* functions
    m_pattern = AWPatternFunctions::insertAllKeys(m_pattern, allKeys);
    m_pattern = AWPatternFunctions::insertKeyCount(m_pattern, allKeys);
    m_pattern = AWPatternFunctions::insertKeyNames(m_pattern, allKeys);
    m_pattern = AWPatternFunctions::insertKeys(m_pattern, allKeys);
    m_pattern = AWPatternFunctions::insertMacros(m_pattern);
    // wrap templates
    m_pattern = AWPatternFunctions::expandTemplates(m_pattern);

    emit(updateKeys(allKeys));
}
