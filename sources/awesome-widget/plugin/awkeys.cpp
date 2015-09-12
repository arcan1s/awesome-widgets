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

#include <QtConcurrent/QtConcurrent>
#include <QDir>
#include <QInputDialog>
#include <QNetworkInterface>
#include <QRegExp>
#include <QScriptEngine>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

#include "awdebug.h"
#include "awkeysaggregator.h"
#include "awdataaggregator.h"
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

    aggregator = new AWKeysAggregator(this);

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

    if (dataAggregator != nullptr)  delete dataAggregator;

    delete aggregator;
    delete graphicalItems;
    delete extQuotes;
    delete extScripts;
    delete extUpgrade;
    delete extWeather;
}


void AWKeys::initDataAggregator(const QVariantMap tooltipParams)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Tooltip parameters" << tooltipParams;

    if (dataAggregator != nullptr) {
        disconnect(dataAggregator, SIGNAL(toolTipPainted(QString)),
                   this, SIGNAL(needToolTipToBeUpdated(QString)));
        delete dataAggregator;
    }

    dataAggregator = new AWDataAggregator(this, tooltipParams);
    // transfer signal from AWDataAggregator object to QML ui
    connect(dataAggregator, SIGNAL(toolTipPainted(QString)),
            this, SIGNAL(needToolTipToBeUpdated(QString)));
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
}


void AWKeys::setAggregatorProperty(const QString key, const QVariant value)
{
    qCDebug(LOG_AW);

    aggregator->setProperty(key.toUtf8().constData(), value);
}



void AWKeys::setPopupEnabled(const bool popup)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Is popup enabled" << popup;

    enablePopup = popup;
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
    if (dataAggregator == nullptr) return QSize();

    return dataAggregator->getTooltipSize();
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
    for (int i=extWeather->activeItems().count()-1; i>=0; i--) {
        allKeys.append(extWeather->activeItems().at(i)->tag(QString("weatherId")));
        allKeys.append(extWeather->activeItems().at(i)->tag(QString("weather")));
        allKeys.append(extWeather->activeItems().at(i)->tag(QString("humidity")));
        allKeys.append(extWeather->activeItems().at(i)->tag(QString("pressure")));
        allKeys.append(extWeather->activeItems().at(i)->tag(QString("temperature")));
        allKeys.append(extWeather->activeItems().at(i)->tag(QString("timestamp")));
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
    for (int i=extUpgrade->activeItems().count()-1; i>=0; i--)
        allKeys.append(extUpgrade->activeItems().at(i)->tag(QString("pkgcount")));
    // quotes
    for (int i=extQuotes->activeItems().count()-1; i>=0; i--) {
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("ask")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("askchg")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("percaskchg")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("bid")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("bidchg")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("percbidchg")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("price")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("pricechg")));
        allKeys.append(extQuotes->activeItems().at(i)->tag(QString("percpricechg")));
    }
    // custom
    for (int i=extScripts->activeItems().count()-1; i>=0; i--)
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
    foreach(GraphicalItem *item, graphicalItems->items())
        graphicalItemsKeys.append(item->tag());
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


void AWKeys::dataUpdateReceived(const QString sourceName, const QVariantMap data)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Source" << sourceName;
    qCDebug(LOG_AW) << "Data" << data;

    if (sourceName == QString("update")) return emit(needToBeUpdated());

    QtConcurrent::run([this, sourceName, data]() {
        return setDataBySource(sourceName, data);
    });
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

    return values.value(key.remove(QRegExp(QString("^bar[0-9]{1,}"))), QString(""));
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


void AWKeys::dataUpdate()
{
    qCDebug(LOG_AW);

    calculateValues();
    calculateLambdas();
    emit(needTextToBeUpdated(parsePattern()));
    if (dataAggregator != nullptr) emit(dataAggregator->updateData(values));
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

    return reinitKeys();
}


void AWKeys::reinitKeys()
{
    qCDebug(LOG_AW);

    // init
    QStringList allKeys = dictKeys();

    // not documented feature - place all available tags
    pattern = pattern.replace(QString("$ALL"), [allKeys](){
        QStringList strings;
        foreach(QString tag, allKeys)
            strings.append(QString("%1: $%1").arg(tag));
        return strings.join(QString(" | "));
    }());

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

    // set key data to aggregator
    aggregator->setDevices([this]() {
        QHash<QString, QStringList> deviceList;
        deviceList[QString("disk")] = diskDevices;
        deviceList[QString("hdd")] = hddDevices;
        deviceList[QString("mount")] = mountDevices;
        deviceList[QString("net")] = networkDevices;
        deviceList[QString("temp")] = tempDevices;
        return deviceList;
    }());
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
    return loadKeysFromCache();
}


void AWKeys::calculateLambdas()
{
    qCDebug(LOG_AW);

    foreach(QString key, foundLambdas)
        values[key] = [this](QString key) {
            QScriptEngine engine;
            // apply $this values
            key.replace(QString("$this"), values[key]);
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
        }(key);
}


// HACK this method is required since I could not define some values by using
// specified pattern. Usually they are values which depend on several others
void AWKeys::calculateValues()
{
    qCDebug(LOG_AW);

    // hddtot*
    foreach(QString device, hddDevices) {
        int index = hddDevices.indexOf(device);
        values[QString("hddtotmb%1").arg(index)] = QString("%1").arg(
            values[QString("hddfreemb%1").arg(index)].toFloat() +
            values[QString("hddmb%1").arg(index)].toFloat(), 5, 'f', 0);
        values[QString("hddtotgb%1").arg(index)] = QString("%1").arg(
            values[QString("hddfreegb%1").arg(index)].toFloat() +
            values[QString("hddgb%1").arg(index)].toFloat(), 5, 'f', 1);
    }

    // memtot*
    values[QString("memtotmb")] = QString("%1").arg(
        values[QString("memusedmb")].toInt() + values[QString("memfreemb")].toInt(), 5);
    values[QString("memtotgb")] = QString("%1").arg(
        values[QString("memusedgb")].toFloat() + values[QString("memfreegb")].toFloat(), 5, 'f', 1);
    // mem
    values[QString("mem")] = QString("%1").arg(
        100.0 * values[QString("memmb")].toFloat() / values[QString("memtotmb")].toFloat(),
        5, 'f', 1);

    // up, down, upkb, downkb, upunits, downunits
    int netIndex = networkDevices.indexOf(values[QString("netdev")]);
    values[QString("down")] = values[QString("down%1").arg(netIndex)];
    values[QString("downkb")] = values[QString("downkb%1").arg(netIndex)];
    values[QString("downunits")] = values[QString("downunits%1").arg(netIndex)];
    values[QString("up")] = values[QString("up%1").arg(netIndex)];
    values[QString("upkb")] = values[QString("upkb%1").arg(netIndex)];
    values[QString("upunits")] = values[QString("upunits%1").arg(netIndex)];

    // swaptot*
    values[QString("swaptotmb")] = QString("%1").arg(
        values[QString("swapmb")].toInt() + values[QString("swapfreemb")].toInt(), 5);
    values[QString("swaptotgb")] = QString("%1").arg(
        values[QString("swapgb")].toFloat() + values[QString("swapfreegb")].toFloat(), 5, 'f', 1);
    // swap
    values[QString("swap")] = QString("%1").arg(
        100.0 * values[QString("swapmb")].toFloat() / values[QString("swaptotmb")].toFloat(),
        5, 'f', 1);
}


QString AWKeys::parsePattern() const
{
    qCDebug(LOG_AW);

    QString parsed = pattern;
    parsed.replace(QString("$$"), QString("$\\$\\"));

    // lambdas
    foreach(QString key, foundLambdas)
        parsed.replace(QString("${{%1}}").arg(key), values[key]);

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


void AWKeys::setDataBySource(const QString sourceName, const QVariantMap data)
{
    // check if data stream is locked
    lock = ((lock) && (queue > 0));
    if (lock) return;

    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Source" << sourceName;
    qCDebug(LOG_AW) << "Data" << data;

    // drop if limits are reached
    if (++queue > QUEUE_LIMIT) {
        qCWarning(LOG_AW) << "Messages queue" << queue-- << "more than limits" << QUEUE_LIMIT << ", lock";
        lock = true;
        return;
    }

    // first list init
    QStringList tags = aggregator->keyFromSource(sourceName);
    if (tags.isEmpty())
        tags = aggregator->registerSource(sourceName, data[QString("units")].toString());

    // update data or drop source if there are no matches
    if (tags.isEmpty()) {
        qCDebug(LOG_AW) << "Source" << sourceName << "not found";
        emit(dropSourceFromDataengine(sourceName));
    } else {
        // HACK workaround for time values which are stored in the different path
        QVariant value = sourceName == QString("Local") ? data[QString("DateTime")] : data[QString("value")];
        std::for_each(tags.cbegin(), tags.cend(), [this, value](const QString tag){
            values[tag] = aggregator->formater(value, tag);
        });
    }

    queue--;
}
