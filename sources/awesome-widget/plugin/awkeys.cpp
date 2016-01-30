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
#include <QJSEngine>
#include <QRegExp>
#include <QThread>

#include "awdataaggregator.h"
#include "awdataengineaggregator.h"
#include "awdebug.h"
#include "awkeyoperations.h"
#include "awkeysaggregator.h"
#include "graphicalitem.h"
#include "version.h"


AWKeys::AWKeys(QObject *parent)
    : QObject(parent)
{
    qSetMessagePattern(LOG_FORMAT);
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
    for (auto metadata : getBuildData())
        qCDebug(LOG_AW) << metadata;

    // thread pool
    m_threadPool = new QThreadPool(this);

    aggregator = new AWKeysAggregator(this);
    dataAggregator = new AWDataAggregator(this);
    keyOperator = new AWKeyOperations(this);
    // update key data if required
    connect(keyOperator, SIGNAL(updateKeys(QStringList)), this,
            SLOT(reinitKeys(QStringList)));
    // transfer signal from AWDataAggregator object to QML ui
    connect(dataAggregator, SIGNAL(toolTipPainted(const QString)), this,
            SIGNAL(needToolTipToBeUpdated(const QString)));
    connect(this, SIGNAL(needToBeUpdated()), this, SLOT(updateTextData()));
}


AWKeys::~AWKeys()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    // core
    delete dataEngineAggregator;
    delete m_threadPool;
    delete aggregator;
    delete dataAggregator;
    delete keyOperator;
}


void AWKeys::initDataAggregator(const QVariantMap tooltipParams)
{
    qCDebug(LOG_AW) << "Tooltip parameters" << tooltipParams;

    dataAggregator->setParameters(tooltipParams);
}


void AWKeys::initKeys(const QString currentPattern, const int interval,
                      const int limit)
{
    qCDebug(LOG_AW) << "Pattern" << currentPattern;
    qCDebug(LOG_AW) << "Interval" << interval;
    qCDebug(LOG_AW) << "Queue limit" << limit;

    // init
    keyOperator->setPattern(currentPattern);
    if (dataEngineAggregator == nullptr) {
        dataEngineAggregator = new AWDataEngineAggregator(this, interval);
        connect(this, SIGNAL(dropSourceFromDataengine(QString)),
                dataEngineAggregator, SLOT(dropSource(QString)));
    } else
        dataEngineAggregator->setInterval(interval);
    m_threadPool->setMaxThreadCount(limit == 0 ? QThread::idealThreadCount()
                                               : limit);
    keyOperator->updateCache();

    return dataEngineAggregator->reconnectSources();
}


void AWKeys::setAggregatorProperty(const QString key, const QVariant value)
{
    qCDebug(LOG_AW) << "Key" << key;
    qCDebug(LOG_AW) << "Value" << value;

    aggregator->setProperty(key.toUtf8().constData(), value);
}


void AWKeys::setWrapNewLines(const bool wrap)
{
    qCDebug(LOG_AW) << "Is wrapping enabled" << wrap;

    m_wrapNewLines = wrap;
}


void AWKeys::updateCache()
{
    return keyOperator->updateCache();
}


QStringList AWKeys::dictKeys(const bool sorted, const QString regexp) const
{
    qCDebug(LOG_AW) << "Should be sorted" << sorted;
    qCDebug(LOG_AW) << "Filter" << regexp;

    QStringList allKeys = keyOperator->dictKeys();
    // sort if required
    if (sorted)
        allKeys.sort();

    return allKeys.filter(QRegExp(regexp));
}


QStringList AWKeys::getHddDevices() const
{
    QStringList devices = keyOperator->devices(QString("hdd"));
    // required by selector in the UI
    devices.insert(0, QString("disable"));
    devices.insert(0, QString("auto"));

    return devices;
}


QString AWKeys::infoByKey(QString key) const
{
    qCDebug(LOG_AW) << "Requested key" << key;

    return keyOperator->infoByKey(key);
}


// HACK this method requires to define tag value from bar from UI interface
QString AWKeys::valueByKey(QString key) const
{
    qCDebug(LOG_AW) << "Requested key" << key;

    return values.value(key.remove(QRegExp(QString("^bar[0-9]{1,}"))),
                        QString(""));
}


void AWKeys::editItem(const QString type)
{
    qCDebug(LOG_AW) << "Item type" << type;

    return keyOperator->editItem(type);
}


void AWKeys::addDevice(const QString source)
{
    qCDebug(LOG_AW) << "Source" << source;

    return keyOperator->addDevice(source);
}


void AWKeys::dataUpdated(const QString &sourceName,
                         const Plasma::DataEngine::Data &data)
{
    // do not log these parameters
    if (sourceName == QString("update")) {
        qCInfo(LOG_AW) << "Update data";
        return emit(needToBeUpdated());
    }

    // run concurrent data update
    QtConcurrent::run(m_threadPool, this, &AWKeys::setDataBySource, sourceName,
                      data);
}


void AWKeys::reinitKeys(const QStringList currentKeys)
{
    qCDebug(LOG_AW) << "Update found keys by using list" << currentKeys;

    // append lists
    // bars
    m_foundBars = [currentKeys](const QString pattern) {
        QStringList selectedKeys;
        for (auto key : currentKeys)
            if ((key.startsWith(QString("bar")))
                && (pattern.contains(QString("$%1").arg(key)))) {
                qCInfo(LOG_AW) << "Found bar" << key;
                selectedKeys.append(key);
            }
        if (selectedKeys.isEmpty())
            qCWarning(LOG_AW) << "No bars found";
        return selectedKeys;
    }(keyOperator->pattern());

    // main key list
    m_foundKeys = [currentKeys](const QString pattern) {
        QStringList selectedKeys;
        for (auto key : currentKeys)
            if ((!key.startsWith(QString("bar")))
                && (pattern.contains(QString("$%1").arg(key)))) {
                qCInfo(LOG_AW) << "Found key" << key;
                selectedKeys.append(key);
            }
        if (selectedKeys.isEmpty())
            qCWarning(LOG_AW) << "No keys found";
        return selectedKeys;
    }(keyOperator->pattern());

    // lambdas
    m_foundLambdas = [](const QString pattern) {
        QStringList selectedKeys;
        // substring inside ${{ }} (with brackets) which should not contain ${{
        QRegularExpression lambdaRegexp(
            QString("\\$\\{\\{((?!\\$\\{\\{).)*?\\}\\}"));
        lambdaRegexp.setPatternOptions(
            QRegularExpression::DotMatchesEverythingOption);

        QRegularExpressionMatchIterator it = lambdaRegexp.globalMatch(pattern);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString lambda = match.captured();
            // drop brackets
            lambda.remove(QRegExp(QString("^\\$\\{\\{")));
            lambda.remove(QRegExp(QString("\\}\\}$")));
            // append
            qCInfo(LOG_AW) << "Found lambda" << lambda;
            selectedKeys.append(lambda);
        }
        if (selectedKeys.isEmpty())
            qCWarning(LOG_AW) << "No lambdas found";
        return selectedKeys;
    }(keyOperator->pattern());

    // set key data to aggregator
    aggregator->setDevices(keyOperator->devices());
}


void AWKeys::updateTextData()
{
    QFuture<QString> text = QtConcurrent::run(m_threadPool, [this]() {
        calculateValues();
        return parsePattern(keyOperator->pattern());
    });

    emit(needTextToBeUpdated(text));
    emit(dataAggregator->updateData(values));
}


// HACK this method is required since I could not define some values by using
// specified pattern. Usually they are values which depend on several others
void AWKeys::calculateValues()
{
    // hddtot*
    QStringList mountDevices = keyOperator->devices(QString("mount"));
    for (auto device : mountDevices) {
        int index = mountDevices.indexOf(device);
        values[QString("hddtotmb%1").arg(index)] = QString("%1").arg(
            values[QString("hddfreemb%1").arg(index)].toFloat()
                + values[QString("hddmb%1").arg(index)].toFloat(),
            5, 'f', 0);
        values[QString("hddtotgb%1").arg(index)] = QString("%1").arg(
            values[QString("hddfreegb%1").arg(index)].toFloat()
                + values[QString("hddgb%1").arg(index)].toFloat(),
            5, 'f', 1);
    }

    // memtot*
    values[QString("memtotmb")]
        = QString("%1").arg(values[QString("memusedmb")].toInt()
                                + values[QString("memfreemb")].toInt(),
                            5);
    values[QString("memtotgb")]
        = QString("%1").arg(values[QString("memusedgb")].toFloat()
                                + values[QString("memfreegb")].toFloat(),
                            5, 'f', 1);
    // mem
    values[QString("mem")]
        = QString("%1").arg(100.0 * values[QString("memmb")].toFloat()
                                / values[QString("memtotmb")].toFloat(),
                            5, 'f', 1);

    // up, down, upkb, downkb, upunits, downunits
    int netIndex = keyOperator->devices(QString("net"))
                       .indexOf(values[QString("netdev")]);
    values[QString("down")] = values[QString("down%1").arg(netIndex)];
    values[QString("downkb")] = values[QString("downkb%1").arg(netIndex)];
    values[QString("downunits")] = values[QString("downunits%1").arg(netIndex)];
    values[QString("up")] = values[QString("up%1").arg(netIndex)];
    values[QString("upkb")] = values[QString("upkb%1").arg(netIndex)];
    values[QString("upunits")] = values[QString("upunits%1").arg(netIndex)];

    // swaptot*
    values[QString("swaptotmb")]
        = QString("%1").arg(values[QString("swapmb")].toInt()
                                + values[QString("swapfreemb")].toInt(),
                            5);
    values[QString("swaptotgb")]
        = QString("%1").arg(values[QString("swapgb")].toFloat()
                                + values[QString("swapfreegb")].toFloat(),
                            5, 'f', 1);
    // swap
    values[QString("swap")]
        = QString("%1").arg(100.0 * values[QString("swapmb")].toFloat()
                                / values[QString("swaptotmb")].toFloat(),
                            5, 'f', 1);

    // lambdas
    for (auto key : m_foundLambdas)
        values[key] = [this](QString key) {
            QJSEngine engine;
            // apply $this values
            key.replace(QString("$this"), values[key]);
            for (auto lambdaKey : m_foundKeys)
                key.replace(QString("$%1").arg(lambdaKey), values[lambdaKey]);
            qCInfo(LOG_AW) << "Expression" << key;
            QJSValue result = engine.evaluate(key);
            if (result.isError()) {
                qCWarning(LOG_AW) << "Uncaught exception at line"
                                  << result.property("lineNumber").toInt()
                                  << ":" << result.toString();
                return QString();
            } else {
                return result.toString();
            }
        }(key);
}


QString AWKeys::parsePattern(QString pattern) const
{
    // screen sign
    pattern.replace(QString("$$"), QString("$\\$\\"));

    // lambdas
    for (auto key : m_foundLambdas)
        pattern.replace(QString("${{%1}}").arg(key), values[key]);

    // main keys
    for (auto key : m_foundKeys)
        pattern.replace(QString("$%1").arg(key),
                        [](QString key, QString value) {
                            if ((!key.startsWith(QString("custom")))
                                && (!key.startsWith(QString("weather"))))
                                value.replace(QString(" "), QString("&nbsp;"));
                            return value;
                        }(key, values[key]));

    // bars
    for (auto bar : m_foundBars) {
        GraphicalItem *item = keyOperator->giByKey(bar);
        QString key = bar;
        key.remove(QRegExp(QString("^bar[0-9]{1,}")));
        if (item->type() == GraphicalItem::Graph)
            pattern.replace(QString("$%1").arg(bar),
                            item->image([](const QList<float> data) {
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
    qCDebug(LOG_AW) << "Source" << sourceName;
    qCDebug(LOG_AW) << "Data" << data;

    // first list init
    QStringList tags = aggregator->keysFromSource(sourceName);
    if (tags.isEmpty())
        tags = aggregator->registerSource(sourceName,
                                          data[QString("units")].toString());

    // update data or drop source if there are no matches
    if (tags.isEmpty()) {
        qCDebug(LOG_AW) << "Source" << sourceName << "not found";
        emit(dropSourceFromDataengine(sourceName));
    } else {
        m_mutex.lock();
        // HACK workaround for time values which are stored in the different
        // path
        QVariant value = sourceName == QString("Local")
                             ? data[QString("DateTime")]
                             : data[QString("value")];
        std::for_each(tags.cbegin(), tags.cend(),
                      [this, value](const QString tag) {
                          values[tag] = aggregator->formater(value, tag);
                      });
        m_mutex.unlock();
    }
}
