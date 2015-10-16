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
#include <QJSEngine>
#include <QNetworkInterface>
#include <QRegExp>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

#include "awdataaggregator.h"
#include "awdataengineaggregator.h"
#include "awdebug.h"
#include "awkeysaggregator.h"
#include "extquotes.h"
#include "extscript.h"
#include "extupgrade.h"
#include "extweather.h"
#include "graphicalitem.h"
#include "version.h"


AWKeys::AWKeys(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW);

    // logging
    qSetMessagePattern(LOG_FORMAT);

#ifdef BUILD_FUTURE
    // thread pool
    m_threadPool = new QThreadPool(this);
#endif /* BUILD_FUTURE */

    aggregator = new AWKeysAggregator(this);
    dataAggregator = new AWDataAggregator(this);
    // transfer signal from AWDataAggregator object to QML ui
    connect(dataAggregator, SIGNAL(toolTipPainted(const QString)),
            this, SIGNAL(needToolTipToBeUpdated(const QString)));
    connect(this, SIGNAL(needToBeUpdated()), this, SLOT(updateTextData()));
}


AWKeys::~AWKeys()
{
    qCDebug(LOG_AW);

    // extensions
    if (graphicalItems != nullptr) delete graphicalItems;
    if (extQuotes != nullptr) delete extQuotes;
    if (extScripts != nullptr) delete extScripts;
    if (extUpgrade != nullptr) delete extUpgrade;
    if (extWeather != nullptr) delete extWeather;

    // core
    if (dataEngineAggregator != nullptr) delete dataEngineAggregator;
    if (m_threadPool != nullptr) delete m_threadPool;
    delete aggregator;
    delete dataAggregator;
}


void AWKeys::initDataAggregator(const QVariantMap tooltipParams)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Tooltip parameters" << tooltipParams;

    dataAggregator->setParameters(tooltipParams);
}


void AWKeys::initKeys(const QString currentPattern, const int interval, const int limit)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Pattern" << currentPattern;
    qCDebug(LOG_AW) << "Interval" << interval;
    qCDebug(LOG_AW) << "Queue limit" << limit;

    // init
    m_pattern = currentPattern;
    if (dataEngineAggregator == nullptr) {
        dataEngineAggregator = new AWDataEngineAggregator(this, interval);
        connect(this, SIGNAL(dropSourceFromDataengine(QString)),
                dataEngineAggregator, SLOT(dropSource(QString)));
    } else
        dataEngineAggregator->setInterval(interval);
#ifdef BUILD_FUTURE
    m_threadPool->setMaxThreadCount(limit == 0 ? QThread::idealThreadCount() : limit);
#endif /* BUILD_FUTURE */
    updateCache();

    return dataEngineAggregator->reconnectSources();
}


void AWKeys::setAggregatorProperty(const QString key, const QVariant value)
{
    qCDebug(LOG_AW);

    aggregator->setProperty(key.toUtf8().constData(), value);
}


void AWKeys::setWrapNewLines(const bool wrap)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Is wrapping enabled" << wrap;

    m_wrapNewLines = wrap;
}


void AWKeys::updateCache()
{
    qCDebug(LOG_AW);

    // update network and hdd list
    addKeyToCache(QString("hdd"));
    addKeyToCache(QString("net"));
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
    for (int i=m_devices[QString("temp")].count()-1; i>=0; i--)
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
    for (int i=m_devices[QString("mount")].count()-1; i>=0; i--) {
        allKeys.append(QString("hddmb%1").arg(i));
        allKeys.append(QString("hddgb%1").arg(i));
        allKeys.append(QString("hddfreemb%1").arg(i));
        allKeys.append(QString("hddfreegb%1").arg(i));
        allKeys.append(QString("hddtotmb%1").arg(i));
        allKeys.append(QString("hddtotgb%1").arg(i));
        allKeys.append(QString("hdd%1").arg(i));
    }
    // hdd speed
    for (int i=m_devices[QString("disk")].count()-1; i>=0; i--) {
        allKeys.append(QString("hddr%1").arg(i));
        allKeys.append(QString("hddw%1").arg(i));
    }
    // hdd temp
    for (int i=m_devices[QString("hdd")].count()-1; i>=0; i--)
        allKeys.append(QString("hddtemp%1").arg(i));
    // network
    for (int i=m_devices[QString("net")].count()-1; i>=0; i--) {
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
    QStringList allBatteryDevices = QDir(QString("/sys/class/power_supply")).
        entryList(QStringList() << QString("BAT*"),
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
    if (sorted)
        allKeys.sort();

    return allKeys.filter(QRegExp(regexp));
}


QStringList AWKeys::getHddDevices() const
{
    qCDebug(LOG_AW);

    QStringList devices = m_devices[QString("hdd")];
    // required by selector in the UI
    devices.insert(0, QString("disable"));
    devices.insert(0, QString("auto"));

    return devices;
}


QString AWKeys::infoByKey(QString key) const
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Requested key" << key;

    key.remove(QRegExp(QString("^bar[0-9]{1,}")));
    if (key.startsWith(QString("custom")))
        return extScripts->itemByTagNumber(key.remove(QString("custom")).toInt())->uniq();
    else if (key.contains(QRegExp(QString("^hdd[rw]"))))
        return QString("%1").arg(m_devices[QString("disk")][key.remove(QRegExp(QString("hdd[rw]"))).toInt()]);
    else if (key.contains(QRegExp(QString("^hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb)"))))
        return QString("%1").arg(m_devices[QString("mount")][key.remove(QRegExp(QString("^hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb)"))).toInt()]);
    else if (key.startsWith(QString("hddtemp")))
        return QString("%1").arg(m_devices[QString("hdd")][key.remove(QString("hddtemp")).toInt()]);
    else if (key.contains(QRegExp(QString("^(down|up)[0-9]"))))
        return QString("%1").arg(m_devices[QString("net")][key.remove(QRegExp(QString("^(down|up)"))).toInt()]);
    else if (key.startsWith(QString("pkgcount")))
        return extUpgrade->itemByTagNumber(key.remove(QString("pkgcount")).toInt())->uniq();
    else if (key.contains(QRegExp(QString("(^|perc)(ask|bid|price)(chg|)"))))
        return extQuotes->itemByTagNumber(key.remove(QRegExp(QString("(^|perc)(ask|bid|price)(chg|)"))).toInt())->uniq();
    else if (key.contains(QRegExp(QString("(weather|weatherId|humidity|pressure|temperature)"))))
        return extWeather->itemByTagNumber(key.remove(QRegExp(QString("(weather|weatherId|humidity|pressure|temperature)"))).toInt())->uniq();
    else if (key.startsWith(QString("temp")))
        return QString("%1").arg(m_devices[QString("temp")][key.remove(QString("temp")).toInt()]);

    return QString("(none)");
}


// HACK this method requires to define tag value from bar from UI interface
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


void AWKeys::addDevice(const QString source)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Source" << source;

    QRegExp diskRegexp = QRegExp(QString("disk/(?:md|sd|hd)[a-z|0-9]_.*/Rate/(?:rblk)"));
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


void AWKeys::dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Source" << sourceName;
    qCDebug(LOG_AW) << "Data" << data;

    if (sourceName == QString("update"))
        return emit(needToBeUpdated());

#ifdef BUILD_FUTURE
    // run concurrent data update
    QtConcurrent::run(m_threadPool, this, &AWKeys::setDataBySource, sourceName, data);
#else /* BUILD_FUTURE */
    return setDataBySource(sourceName, data);
#endif /* BUILD_FUTURE */
}


void AWKeys::loadKeysFromCache()
{
    qCDebug(LOG_AW);

    QString fileName = QString("%1/awesomewidgets.ndx").
        arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    qCInfo(LOG_AW) << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);

    foreach(QString group, cache.childGroups()) {
        cache.beginGroup(group);
        m_devices.remove(group);
        foreach(QString key, cache.allKeys())
            m_devices[group].append(cache.value(key).toString());
        cache.endGroup();
    }

    return reinitKeys();
}


void AWKeys::reinitKeys()
{
    qCDebug(LOG_AW);

    // renew extensions
    // delete them if any
    if (graphicalItems != nullptr) delete graphicalItems;
    if (extQuotes != nullptr) delete extQuotes;
    if (extScripts != nullptr) delete extScripts;
    if (extUpgrade != nullptr) delete extUpgrade;
    if (extWeather != nullptr) delete extWeather;
    // create
    graphicalItems = new ExtItemAggregator<GraphicalItem>(nullptr, QString("desktops"));
    extQuotes = new ExtItemAggregator<ExtQuotes>(nullptr, QString("quotes"));
    extScripts = new ExtItemAggregator<ExtScript>(nullptr, QString("scripts"));
    extUpgrade = new ExtItemAggregator<ExtUpgrade>(nullptr, QString("upgrade"));
    extWeather = new ExtItemAggregator<ExtWeather>(nullptr, QString("weather"));

    // init
    QStringList allKeys = dictKeys();

#ifdef BUILD_TESTING
    // not documented feature - place all available tags
    m_pattern = m_pattern.replace(QString("$ALL"), [allKeys]() {
        QStringList strings;
        foreach(QString tag, allKeys)
            strings.append(QString("%1: $%1").arg(tag));
        return strings.join(QString(" | "));
    }());
#endif /* BUILD_TESTING */

    // append lists
    // bars
    m_foundBars = [allKeys](QString pattern) {
        QStringList selectedKeys;
        foreach(QString key, allKeys)
            if ((key.startsWith(QString("bar"))) &&
                (pattern.contains(QString("$%1").arg(key)))) {
                qCInfo(LOG_AW) << "Found bar" << key;
                selectedKeys.append(key);
            }
        if (selectedKeys.isEmpty())
            qCWarning(LOG_AW) << "No bars found";
        return selectedKeys;
    }(m_pattern);

    // main key list
    m_foundKeys = [allKeys](QString pattern) {
        QStringList selectedKeys;
        foreach(QString key, allKeys)
            if ((!key.startsWith(QString("bar"))) &&
                (pattern.contains(QString("$%1").arg(key)))) {
                qCInfo(LOG_AW) << "Found key" << key;
                selectedKeys.append(key);
            }
        if (selectedKeys.isEmpty())
            qCWarning(LOG_AW) << "No keys found";
        return selectedKeys;
    }(m_pattern);

    // lambdas
    m_foundLambdas = [](QString pattern) {
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
        if (selectedKeys.isEmpty())
            qCWarning(LOG_AW) << "No lambdas found";
        return selectedKeys;
    }(m_pattern);

    // set key data to aggregator
    aggregator->setDevices(m_devices);
}


void AWKeys::updateTextData()
{
    qCDebug(LOG_AW);

    calculateValues();
    emit(needTextToBeUpdated(parsePattern(m_pattern)));
    emit(dataAggregator->updateData(values));
}


void AWKeys::addKeyToCache(const QString type, const QString key)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Key type" << type;
    qCDebug(LOG_AW) << "Key" << key;

    QString fileName = QString("%1/awesomewidgets.ndx").
        arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    qCInfo(LOG_AW) << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);

    cache.beginGroup(type);
    QStringList cachedValues;
    foreach(QString key, cache.allKeys())
        cachedValues.append(cache.value(key).toString());

    if (type == QString("hdd")) {
        QStringList allDevices = QDir(QString("/dev")).entryList(QDir::System, QDir::Name);
        QStringList devices = allDevices.filter(QRegExp(QString("^[hms]d[a-z]$")));
        foreach(QString dev, devices) {
            QString device = QString("/dev/%1").arg(dev);
            if (cachedValues.contains(device))
                continue;
            qCInfo(LOG_AW) << "Found new key" << device << "for type" << type;
            cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), device);
        }
    } else if (type == QString("net")) {
        QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
        foreach(QNetworkInterface interface, rawInterfaceList) {
            QString device = interface.name();
            if (cachedValues.contains(device))
                continue;
            qCInfo(LOG_AW) << "Found new key" << device << "for type" << type;
            cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), device);
        }
    } else {
        if (cachedValues.contains(key))
            return;
        qCInfo(LOG_AW) << "Found new key" << key << "for type" << type;
        cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), key);
    }
    cache.endGroup();

    cache.sync();
    return loadKeysFromCache();
}


// HACK this method is required since I could not define some values by using
// specified pattern. Usually they are values which depend on several others
void AWKeys::calculateValues()
{
    qCDebug(LOG_AW);

    // hddtot*
    foreach(QString device, m_devices[QString("mount")]) {
        int index = m_devices[QString("mount")].indexOf(device);
        values[QString("hddtotmb%1").arg(index)] = QString("%1").
            arg(values[QString("hddfreemb%1").arg(index)].toFloat()
                + values[QString("hddmb%1").arg(index)].toFloat(), 5, 'f', 0);
        values[QString("hddtotgb%1").arg(index)] = QString("%1").
            arg(values[QString("hddfreegb%1").arg(index)].toFloat()
                + values[QString("hddgb%1").arg(index)].toFloat(), 5, 'f', 1);
    }

    // memtot*
    values[QString("memtotmb")] = QString("%1").
        arg(values[QString("memusedmb")].toInt() + values[QString("memfreemb")].toInt(), 5);
    values[QString("memtotgb")] = QString("%1").
        arg(values[QString("memusedgb")].toFloat() + values[QString("memfreegb")].toFloat(), 5, 'f', 1);
    // mem
    values[QString("mem")] = QString("%1").
        arg(100.0 * values[QString("memmb")].toFloat() / values[QString("memtotmb")].toFloat(),
            5, 'f', 1);

    // up, down, upkb, downkb, upunits, downunits
    int netIndex = m_devices[QString("net")].indexOf(values[QString("netdev")]);
    values[QString("down")] = values[QString("down%1").arg(netIndex)];
    values[QString("downkb")] = values[QString("downkb%1").arg(netIndex)];
    values[QString("downunits")] = values[QString("downunits%1").arg(netIndex)];
    values[QString("up")] = values[QString("up%1").arg(netIndex)];
    values[QString("upkb")] = values[QString("upkb%1").arg(netIndex)];
    values[QString("upunits")] = values[QString("upunits%1").arg(netIndex)];

    // swaptot*
    values[QString("swaptotmb")] = QString("%1").
        arg(values[QString("swapmb")].toInt() + values[QString("swapfreemb")].toInt(), 5);
    values[QString("swaptotgb")] = QString("%1").
        arg(values[QString("swapgb")].toFloat() + values[QString("swapfreegb")].toFloat(), 5, 'f', 1);
    // swap
    values[QString("swap")] = QString("%1").
        arg(100.0 * values[QString("swapmb")].toFloat() / values[QString("swaptotmb")].toFloat(),
            5, 'f', 1);

    // lambdas
    foreach(QString key, m_foundLambdas)
        values[key] = [this](QString key) {
            QJSEngine engine;
            // apply $this values
            key.replace(QString("$this"), values[key]);
            foreach(QString lambdaKey, m_foundKeys)
                key.replace(QString("$%1").arg(lambdaKey), values[lambdaKey]);
            qCInfo(LOG_AW) << "Expression" << key;
            QJSValue result = engine.evaluate(key);
            if (result.isError()) {
                qCWarning(LOG_AW) << "Uncaught exception at line" << result.property("lineNumber").toInt()
                                  << ":" << result.toString();
                return QString();
            } else {
                return result.toString();
            }
        }(key);
}


QString AWKeys::parsePattern(QString pattern) const
{
    qCDebug(LOG_AW);

    // screen sign
    pattern.replace(QString("$$"), QString("$\\$\\"));

    // lambdas
    foreach(QString key, m_foundLambdas)
        pattern.replace(QString("${{%1}}").arg(key), values[key]);

    // main keys
    foreach(QString key, m_foundKeys)
        pattern.replace(QString("$%1").arg(key), [](QString key, QString value) {
            if ((!key.startsWith(QString("custom")))
                && (!key.startsWith(QString("weather"))))
                value.replace(QString(" "), QString("&nbsp;"));
            return value;
        }(key, values[key]));

    // bars
    foreach(QString bar,m_foundBars) {
        GraphicalItem *item = graphicalItems->itemByTag(bar);
        QString key = bar;
        key.remove(QRegExp(QString("^bar[0-9]{1,}")));
        if (item->type() == GraphicalItem::Graph)
            pattern.replace(QString("$%1").arg(bar), item->image([](const QList<float> data) {
                return QVariant::fromValue<QList<float>>(data);
            }(dataAggregator->getData(key))));
        else
            pattern.replace(QString("$%1").arg(bar), item->image(values[key]));
    }

    // prepare strings
    pattern.replace(QString("$\\$\\"), QString("$$"));
    if (m_wrapNewLines)
        pattern.replace(QString("\n"), QString("<br>"));

    return pattern;
}


void AWKeys::setDataBySource(const QString &sourceName, const QVariantMap &data)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Source" << sourceName;
    qCDebug(LOG_AW) << "Data" << data;

    // first list init
    QStringList tags = aggregator->keysFromSource(sourceName);
    if (tags.isEmpty())
        tags = aggregator->registerSource(sourceName, data[QString("units")].toString());

    // update data or drop source if there are no matches
    if (tags.isEmpty()) {
        qCDebug(LOG_AW) << "Source" << sourceName << "not found";
        emit(dropSourceFromDataengine(sourceName));
    } else {
#ifdef BUILD_FUTURE
        m_mutex.lock();
#endif /* BUILD_FUTURE */
        // HACK workaround for time values which are stored in the different path
        QVariant value = sourceName == QString("Local") ? data[QString("DateTime")] : data[QString("value")];
        std::for_each(tags.cbegin(), tags.cend(), [this, value](const QString tag) {
            values[tag] = aggregator->formater(value, tag);
        });
#ifdef BUILD_FUTURE
        m_mutex.unlock();
#endif /* BUILD_FUTURE */
    }
}
