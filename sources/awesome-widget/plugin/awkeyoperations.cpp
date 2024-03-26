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
#include <QRegularExpression>
#include <QThread>

#include "awcustomkeyshelper.h"
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


AWKeyOperations::AWKeyOperations(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_customKeys = new AWCustomKeysHelper(this);
    m_graphicalItems = new ExtItemAggregator<GraphicalItem>(nullptr, "desktops");
    m_extNetRequest = new ExtItemAggregator<ExtNetworkRequest>(nullptr, "requests");
    m_extQuotes = new ExtItemAggregator<ExtQuotes>(nullptr, "quotes");
    m_extScripts = new ExtItemAggregator<ExtScript>(nullptr, "scripts");
    m_extUpgrade = new ExtItemAggregator<ExtUpgrade>(nullptr, "upgrade");
    m_extWeather = new ExtItemAggregator<ExtWeather>(nullptr, "weather");
}


AWKeyOperations::~AWKeyOperations()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QStringList AWKeyOperations::devices(const QString &_type) const
{
    qCDebug(LOG_AW) << "Looking for type" << _type;

    return m_devices[_type];
}


QHash<QString, QStringList> AWKeyOperations::devices() const
{
    return m_devices;
}


void AWKeyOperations::updateCache()
{
    // update network and hdd list
    addKeyToCache("net");
}


QStringList AWKeyOperations::dictKeys() const
{
    QStringList allKeys;
    // weather
    for (auto &item : m_extWeather->activeItems()) {
        allKeys.append(item->tag("weatherId"));
        allKeys.append(item->tag("weather"));
        allKeys.append(item->tag("humidity"));
        allKeys.append(item->tag("pressure"));
        allKeys.append(item->tag("temperature"));
        allKeys.append(item->tag("timestamp"));
    }
    // cpuclock & cpu
    for (auto i = 0; i < QThread::idealThreadCount(); i++) {
        allKeys.append(QString("cpucl%1").arg(i));
        allKeys.append(QString("cpu%1").arg(i));
    }
    // temperature
    for (auto i = 0; i < m_devices["temp"].count(); i++)
        allKeys.append(QString("temp%1").arg(i));
    // gpu
    for (auto i = 0; i < m_devices["gpu"].count(); i++) {
        allKeys.append(QString("gpu%1").arg(i));
        allKeys.append(QString("gputemp%1").arg(i));
    }
    // hdd
    for (auto i = 0; i < m_devices["mount"].count(); i++) {
        allKeys.append(QString("hddmb%1").arg(i));
        allKeys.append(QString("hddgb%1").arg(i));
        allKeys.append(QString("hddfreemb%1").arg(i));
        allKeys.append(QString("hddfreegb%1").arg(i));
        allKeys.append(QString("hddtotmb%1").arg(i));
        allKeys.append(QString("hddtotgb%1").arg(i));
        allKeys.append(QString("hdd%1").arg(i));
    }
    // hdd speed
    for (auto i = 0; i < m_devices["disk"].count(); i++) {
        allKeys.append(QString("hddr%1").arg(i));
        allKeys.append(QString("hddw%1").arg(i));
    }
    // network
    for (auto i = 0; i < m_devices["net"].count(); i++) {
        allKeys.append(QString("downunits%1").arg(i));
        allKeys.append(QString("upunits%1").arg(i));
        allKeys.append(QString("downtotkb%1").arg(i));
        allKeys.append(QString("downtot%1").arg(i));
        allKeys.append(QString("downkb%1").arg(i));
        allKeys.append(QString("down%1").arg(i));
        allKeys.append(QString("uptotkb%1").arg(i));
        allKeys.append(QString("uptot%1").arg(i));
        allKeys.append(QString("upkb%1").arg(i));
        allKeys.append(QString("up%1").arg(i));
    }
    // battery
    auto allBatteryDevices
        = QDir("/sys/class/power_supply")
              .entryList(QStringList({"BAT*"}), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (int i = 0; i < allBatteryDevices.count(); i++) {
        allKeys.append(QString("bat%1").arg(i));
        allKeys.append(QString("batleft%1").arg(i));
        allKeys.append(QString("batnow%1").arg(i));
        allKeys.append(QString("batrate%1").arg(i));
        allKeys.append(QString("battotal%1").arg(i));
    }
    // package manager
    for (auto &item : m_extUpgrade->activeItems())
        allKeys.append(item->tag("pkgcount"));
    // quotes
    for (auto &item : m_extQuotes->activeItems()) {
        allKeys.append(item->tag("price"));
        allKeys.append(item->tag("pricechg"));
        allKeys.append(item->tag("percpricechg"));
        allKeys.append(item->tag("volume"));
        allKeys.append(item->tag("volumechg"));
        allKeys.append(item->tag("percvolumechg"));
    }
    // custom
    for (auto &item : m_extScripts->activeItems())
        allKeys.append(item->tag("custom"));
    // network requests
    for (auto &item : m_extNetRequest->activeItems())
        allKeys.append(item->tag("response"));
    // bars
    for (auto &item : m_graphicalItems->activeItems())
        allKeys.append(item->tag("bar"));
    // user defined keys
    allKeys.append(m_customKeys->keys());
    // static keys
    allKeys.append(QString(STATIC_KEYS).split(','));

    // sort in valid order
    allKeys.sort();
    std::reverse(allKeys.begin(), allKeys.end());

    return allKeys;
}


// this method is required to provide GraphicalItem functions (e.g. paint()) to
// parent classes
GraphicalItem *AWKeyOperations::giByKey(const QString &_key) const
{
    qCDebug(LOG_AW) << "Looking for item" << _key;

    return m_graphicalItems->itemByTag(_key, "bar");
}


QStringList AWKeyOperations::requiredUserKeys() const
{
    return m_customKeys->refinedSources();
}


QStringList AWKeyOperations::userKeys() const
{
    return m_customKeys->keys();
}


QString AWKeyOperations::userKeySource(const QString &_key) const
{
    return m_customKeys->source(_key);
}


QString AWKeyOperations::infoByKey(const QString &_key) const
{
    qCDebug(LOG_AW) << "Requested key" << _key;

    auto stripped = _key;
    stripped.remove(QRegularExpression("\\d+"));
    QString output;

    if (_key.startsWith("bar")) {
        auto *item = m_graphicalItems->itemByTag(_key, stripped);
        if (item)
            output = item->uniq();
    } else if (_key.startsWith("custom")) {
        auto *item = m_extScripts->itemByTag(_key, stripped);
        if (item)
            output = item->uniq();
    } else if (_key.contains(QRegularExpression("^hdd[rw]"))) {
        auto index = _key;
        index.remove(QRegularExpression("hdd[rw]"));
        output = m_devices["disk"][index.toInt()];
    } else if (_key.contains(QRegularExpression("^hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb)"))) {
        auto index = _key;
        index.remove(QRegularExpression("^hdd(|mb|gb|freemb|freegb|totmb|totgb)"));
        output = m_devices["mount"][index.toInt()];
    } else if (_key.contains(QRegularExpression("^(down|up)[0-9]"))) {
        auto index = _key;
        index.remove(QRegularExpression("^(down|up)"));
        output = m_devices["net"][index.toInt()];
    } else if (_key.startsWith("pkgcount")) {
        auto *item = m_extUpgrade->itemByTag(_key, stripped);
        if (item)
            output = item->uniq();
    } else if (_key.contains(QRegularExpression("(^|perc)(ask|bid|price)(chg|)"))) {
        auto *item = m_extQuotes->itemByTag(_key, stripped);
        if (item)
            output = item->uniq();
    } else if (_key.contains(QRegularExpression("(weather|weatherId|humidity|pressure|temperature)"))) {
        auto *item = m_extWeather->itemByTag(_key, stripped);
        if (item)
            output = item->uniq();
    } else if (_key.startsWith("temp")) {
        auto index = _key;
        index.remove("temp");
        output = m_devices["temp"][index.toInt()];
    } else if (_key.startsWith("response")) {
        auto *item = m_extNetRequest->itemByTag(_key, stripped);
        if (item)
            output = item->uniq();
    } else {
        output = "(none)";
    }

    return output;
}


QString AWKeyOperations::pattern() const
{
    return m_pattern;
}


void AWKeyOperations::setPattern(const QString &_currentPattern)
{
    qCDebug(LOG_AW) << "Set pattern" << _currentPattern;

    m_pattern = _currentPattern;
}


void AWKeyOperations::editItem(const QString &_type)
{
    qCDebug(LOG_AW) << "Item type" << _type;

    if (_type == "graphicalitem") {
        QStringList keys = dictKeys().filter(QRegularExpression("^(cpu(?!cl).*|gpu$|mem$|swap$|hdd[0-9].*|bat.*)"));
        keys.sort();
        m_graphicalItems->setConfigArgs(keys);
        return m_graphicalItems->editItems();
    } else if (_type == "extnetworkrequest") {
        return m_extNetRequest->editItems();
    } else if (_type == "extquotes") {
        return m_extQuotes->editItems();
    } else if (_type == "extscript") {
        return m_extScripts->editItems();
    } else if (_type == "extupgrade") {
        return m_extUpgrade->editItems();
    } else if (_type == "extweather") {
        return m_extWeather->editItems();
    }
}


void AWKeyOperations::addDevice(const QString &_source)
{
    qCDebug(LOG_AW) << "Source" << _source;

    auto diskRegexp = QRegularExpression("^disk/.*/read$");
    auto mountRegexp = QRegularExpression("^disk/.*/usedPercent$");
    auto cpuTempRegExp = QRegularExpression("^cpu/cpu.*/temperature$");
    auto gpuRegExp = QRegularExpression("^gpu/gpu.*/usage$");

    if (_source.contains(diskRegexp)) {
        auto device = _source;
        device.remove("disk/").remove("/read");
        addKeyToCache("disk", device);
    } else if (_source.contains(mountRegexp)) {
        auto device = _source;
        device.remove("disk/").remove("/usedPercent");
        addKeyToCache("mount", device);
    } else if (_source.startsWith("lmsensors") || _source.contains(cpuTempRegExp)
               || _source == "cpu/all/averageTemperature") {
        addKeyToCache("temp", _source);
    } else if (_source.contains(gpuRegExp)) {
        auto device = _source;
        device.remove("gpu/").remove("/usage");
        addKeyToCache("gpu", device);
    }
}


void AWKeyOperations::addKeyToCache(const QString &_type, const QString &_key)
{
    qCDebug(LOG_AW) << "Key" << _key << "with type" << _type;

    if (AWKeyCache::addKeyToCache(_type, _key)) {
        m_devices = AWKeyCache::loadKeysFromCache();
        reinitKeys();
    }
}


void AWKeyOperations::reinitKeys()
{
    m_customKeys->initItems();
    m_graphicalItems->initItems();
    m_extNetRequest->initItems();
    m_extQuotes->initItems();
    m_extScripts->initItems();
    m_extUpgrade->initItems();
    m_extWeather->initItems();

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
