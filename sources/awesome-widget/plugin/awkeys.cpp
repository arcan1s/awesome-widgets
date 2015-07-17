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

#include <QBuffer>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QHBoxLayout>
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
#include "graphicalitem.h"
#include "version.h"


AWKeys::AWKeys(QObject *parent)
    : QObject(parent)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));

    // init dialog
    dialog = new QDialog(nullptr);
    widgetDialog = new QListWidget(dialog);
    dialogButtons = new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Close,
                                         Qt::Vertical, dialog);
    copyButton = dialogButtons->addButton(i18n("Copy"), QDialogButtonBox::ActionRole);
    createButton = dialogButtons->addButton(i18n("Create"), QDialogButtonBox::ActionRole);
    deleteButton = dialogButtons->addButton(i18n("Remove"), QDialogButtonBox::ActionRole);
    QHBoxLayout *layout = new QHBoxLayout(dialog);
    layout->addWidget(widgetDialog);
    layout->addWidget(dialogButtons);
    dialog->setLayout(layout);
    connect(dialogButtons, SIGNAL(clicked(QAbstractButton *)), this, SLOT(editItemButtonPressed(QAbstractButton *)));
    connect(dialogButtons, SIGNAL(rejected()), dialog, SLOT(reject()));
}


AWKeys::~AWKeys()
{
    if (debug) qDebug() << PDEBUG;

    delete toolTip;
    delete createButton;
    delete copyButton;
    delete deleteButton;
    delete dialogButtons;
    delete widgetDialog;
    delete dialog;

    graphicalItems.clear();
    extScripts.clear();
    extUpgrade.clear();
}


void AWKeys::initKeys()
{
    if (debug) qDebug() << PDEBUG;

    // clear
    extQuotes.clear();
    extScripts.clear();
    extUpgrade.clear();
    graphicalItems.clear();
    keys.clear();
    foundBars.clear();
    foundKeys.clear();

    // init
    extQuotes = getExtQuotes();
    extScripts = getExtScripts();
    extUpgrade = getExtUpgrade();
    graphicalItems = getGraphicalItems();
    // update network and hdd list
    addKeyToCache(QString("Hdd"));
    addKeyToCache(QString("Network"));
    loadKeysFromCache();
    reinitKeys();
}


void AWKeys::initTooltip(const QVariantMap tooltipParams)
{
    if (debug) qDebug() << PDEBUG;

    if (toolTip != nullptr) delete toolTip;
    toolTip = new AWToolTip(this, tooltipParams);
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


bool AWKeys::isDebugEnabled()
{
    if (debug) qDebug() << PDEBUG;

    return debug;
}


QString AWKeys::parsePattern(const QString currentPattern)
{
    if (debug) qDebug() << PDEBUG;
    if (keys.isEmpty()) return currentPattern;

    // get key data
    if ((foundBars.isEmpty()) && (foundKeys.isEmpty())) {
        foundBars = findGraphicalItems(currentPattern);
        foundKeys = findKeys(currentPattern);
    }

    QString parsed = currentPattern;
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


QString AWKeys::toolTipImage()
{
    if(debug) qDebug() << PDEBUG;
    if (toolTip == nullptr) return QString("");

    if (keys.isEmpty()) return QString();

    QPixmap tooltip = toolTip->image();
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    tooltip.save(&buffer, "PNG");

    return QString("<img src=\"data:image/png;base64,%1\"/>").arg(QString(byteArray.toBase64()));
}


QSize AWKeys::toolTipSize()
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


QStringList AWKeys::dictKeys(const bool sorted)
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
    for (int i=extUpgrade.count()-1; i>=0; i--)
        allKeys.append(extUpgrade[i]->tag());
    // quotes
    for (int i=extQuotes.count()-1; i>=0; i--) {
        allKeys.append(extQuotes[i]->tag(QString("ask")));
        allKeys.append(extQuotes[i]->tag(QString("askchg")));
        allKeys.append(extQuotes[i]->tag(QString("percaskchg")));
        allKeys.append(extQuotes[i]->tag(QString("bid")));
        allKeys.append(extQuotes[i]->tag(QString("bidchg")));
        allKeys.append(extQuotes[i]->tag(QString("percbidchg")));
        allKeys.append(extQuotes[i]->tag(QString("price")));
        allKeys.append(extQuotes[i]->tag(QString("pricechg")));
        allKeys.append(extQuotes[i]->tag(QString("percpricechg")));
    }
    // custom
    for (int i=extScripts.count()-1; i>=0; i--)
        allKeys.append(extScripts[i]->tag());
    // desktop
    allKeys.append(QString("desktop"));
    allKeys.append(QString("ndesktop"));
    allKeys.append(QString("tdesktops"));
    // bars
    QStringList graphicalItemsKeys;
    for (int i=0; i<graphicalItems.count(); i++)
        graphicalItemsKeys.append(QString("%1%2").arg(graphicalItems[i]->name())
                                                 .arg(graphicalItems[i]->bar()));
    graphicalItemsKeys.sort();
    for (int i=graphicalItemsKeys.count()-1; i>=0; i--)
        allKeys.append(graphicalItemsKeys[i]);

    // sort if required
    if (sorted) allKeys.sort();

    return allKeys;
}


QStringList AWKeys::getHddDevices()
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

    if (sourceName == QString("update")) {
        emit(needToBeUpdated());
        return;
    }

    // checking
    if (!checkKeys(data)) return;
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
            } else {
                values[data.keys()[i]] = QString("%1").arg(data[data.keys()[i]].toFloat(), 3, 'f', 0);
                if (toolTip != nullptr) toolTip->setData(QString("batTooltip"), data[data.keys()[i]].toFloat(),
                                                         data[QString("ac")].toBool());
            }
        }
    } else if (sourceName == QString("cpu/system/TotalLoad")) {
        // cpu
        // notification
        if ((data[QString("value")].toFloat() >= 90.0) && (values[QString("cpu")].toFloat() < 90.0))
            AWActions::sendNotification(QString("event"), i18n("High CPU load"), enablePopup);
        // value
        values[QString("cpu")] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
        if (toolTip != nullptr) toolTip->setData(QString("cpuTooltip"), data[QString("value")].toFloat());
    } else if (sourceName.contains(cpuRegExp)) {
        // cpus
        QString number = sourceName;
        number.remove(QString("cpu/cpu")).remove(QString("/TotalLoad"));
        values[QString("cpu") + number] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
    } else if (sourceName == QString("cpu/system/AverageClock")) {
        // cpucl
        values[QString("cpucl")] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 0);
        if (toolTip != nullptr) toolTip->setData(QString("cpuclTooltip"), data[QString("value")].toFloat());
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
        float value = 100.0 * values[QString("memmb")].toFloat() / values[QString("memtotmb")].toFloat();
        // notification
        if ((value >= 90.0) && (values[QString("mem")].toFloat() < 90.0))
            AWActions::sendNotification(QString("event"), i18n("High memory usage"), enablePopup);
        // value
        values[QString("mem")] = QString("%1").arg(value, 5, 'f', 1);
        if (toolTip != nullptr) toolTip->setData(QString("memTooltip"), values[QString("mem")].toFloat());
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
            if (toolTip != nullptr) toolTip->setData(QString("downTooltip"), value);
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
            if (toolTip != nullptr) toolTip->setData(QString("upTooltip"), data[QString("value")].toFloat());
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
        if (toolTip != nullptr) toolTip->setData(QString("swapTooltip"), values[QString("swap")].toFloat());
    } else if (sourceName.contains(tempRegExp)) {
        // temperature devices
        for (int i=0; i<tempDevices.count(); i++)
            if (sourceName == tempDevices[i]) {
                float temp = data[QString("units")].toString() == QString("°C") ?
                    temperature(data[QString("value")].toFloat(), params[QString("tempUnits")].toString())
                    : data[QString("value")].toFloat();
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
    } else {
        if (debug) qDebug() << PDEBUG << ":" << "Source" << sourceName << "not found";
        emit(dropSourceFromDataengine(sourceName));
    }
}


void AWKeys::graphicalValueByKey()
{
    if (debug) qDebug() << PDEBUG;

    bool ok;
    QString tag = QInputDialog::getItem(0, i18n("Select tag"), i18n("Tag"),
                                        dictKeys(true), 0, false, &ok);

    if ((!ok) || (tag.isEmpty())) return;
    QString message = i18n("Tag: %1", tag);
    message += QString("<br>");
    message += i18n("Value: %1", valueByKey(tag));

    return AWActions::sendNotification(QString("tag"), message);
}


QString AWKeys::infoByKey(QString key)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Requested key" << key;

    key.remove(QRegExp(QString("^bar[0-9]{1,}")));
    if (key.startsWith(QString("custom")))
        for (int i=0; i<extScripts.count(); i++) {
            if (extScripts[i]->tag() != key) continue;
            return extScripts[i]->executable();
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
        for (int i=0; i<extUpgrade.count(); i++) {
            if (extUpgrade[i]->tag() != key) continue;
            return extUpgrade[i]->executable();
        }
    else if (key.contains(QRegExp(QString("(^|perc)(ask|bid|price)(chg|)"))))
        for (int i=0; i<extQuotes.count(); i++) {
            if (extQuotes[i]->number() != key.remove(QRegExp(QString("(^|perc)(ask|bid|price)(chg|)"))).toInt()) continue;
            return extQuotes[i]->ticker();
        }
    else if (key.startsWith(QString("temp")))
        return QString("%1").arg(tempDevices[key.remove(QString("temp")).toInt()]);

    return QString("(none)");
}


QString AWKeys::valueByKey(QString key)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Requested key" << key;

    key.remove(QRegExp(QString("^bar[0-9]{1,}")));

    return values[key];
}


void AWKeys::editItem(const QString type)
{
    if (debug) qDebug() << PDEBUG;

    widgetDialog->clear();
    if (type == QString("graphicalitem")) {
        requestedItem = RequestedGraphicalItem;
        for (int i=0; i<graphicalItems.count(); i++) {
            QListWidgetItem *item = new QListWidgetItem(graphicalItems[i]->fileName());
            QStringList tooltip;
            tooltip.append(i18n("Tag: %1", graphicalItems[i]->name() + graphicalItems[i]->bar()));
            tooltip.append(i18n("Comment: %1", graphicalItems[i]->comment()));
            item->setToolTip(tooltip.join(QChar('\n')));
            widgetDialog->addItem(item);
        }
    } else if (type == QString("extquotes")) {
        requestedItem = RequestedExtQuotes;
        for (int i=0; i<extQuotes.count(); i++) {
            QListWidgetItem *item = new QListWidgetItem(extQuotes[i]->fileName());
            QStringList tooltip;
            tooltip.append(i18n("Name: %1", extQuotes[i]->name()));
            tooltip.append(i18n("Comment: %1", extQuotes[i]->comment()));
            tooltip.append(i18n("Ticker: %1", extQuotes[i]->ticker()));
            item->setToolTip(tooltip.join(QChar('\n')));
            widgetDialog->addItem(item);
        }
    } else if (type == QString("extscript")) {
        requestedItem = RequestedExtScript;
        for (int i=0; i<extScripts.count(); i++) {
            QListWidgetItem *item = new QListWidgetItem(extScripts[i]->fileName());
            QStringList tooltip;
            tooltip.append(i18n("Name: %1", extScripts[i]->name()));
            tooltip.append(i18n("Comment: %1", extScripts[i]->comment()));
            tooltip.append(i18n("Exec: %1", extScripts[i]->executable()));
            item->setToolTip(tooltip.join(QChar('\n')));
            widgetDialog->addItem(item);
        }
    } else if (type == QString("extupgrade")) {
        requestedItem = RequestedExtUpgrade;
        for (int i=0; i<extUpgrade.count(); i++) {
            QListWidgetItem *item = new QListWidgetItem(extUpgrade[i]->fileName());
            QStringList tooltip;
            tooltip.append(i18n("Name: %1", extUpgrade[i]->name()));
            tooltip.append(i18n("Comment: %1", extUpgrade[i]->comment()));
            tooltip.append(i18n("Exec: %1", extUpgrade[i]->executable()));
            item->setToolTip(tooltip.join(QChar('\n')));
            widgetDialog->addItem(item);
        }
    }

    int ret = dialog->exec();
    if (debug) qDebug() << PDEBUG << ":" << "Dialog returns" << ret;
    requestedItem = Nothing;
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
}


void AWKeys::editItemButtonPressed(QAbstractButton *button)
{
    if (debug) qDebug() << PDEBUG;

    QListWidgetItem *item = widgetDialog->currentItem();
    if (dynamic_cast<QPushButton *>(button) == copyButton) {
        if (item == nullptr) return;
        QString current = item->text();
        switch (requestedItem) {
        case RequestedExtQuotes:
            copyQuotes(current);
            break;
        case RequestedExtScript:
            copyScript(current);
            break;
        case RequestedExtUpgrade:
            copyUpgrade(current);
            break;
        case RequestedGraphicalItem:
            copyBar(current);
            break;
        case Nothing:
        default:
            break;
        }
    } else if (dynamic_cast<QPushButton *>(button) == createButton) {
        switch (requestedItem) {
        case RequestedExtQuotes:
            copyQuotes(QString(""));
            break;
        case RequestedExtScript:
            copyScript(QString(""));
            break;
        case RequestedExtUpgrade:
            copyUpgrade(QString(""));
            break;
        case RequestedGraphicalItem:
            copyBar(QString(""));
            break;
        case Nothing:
        default:
            break;
        }
    } else if (dynamic_cast<QPushButton *>(button) == deleteButton) {
        if (item == nullptr) return;
        QString current = item->text();
        switch (requestedItem) {
        case RequestedExtQuotes:
            for (int i=0; i<extQuotes.count(); i++) {
                if (extQuotes[i]->fileName() != current) continue;
                if (extQuotes[i]->tryDelete()) {
                    widgetDialog->takeItem(widgetDialog->row(item));
                    extQuotes.clear();
                    extQuotes = getExtQuotes();
                }
                break;
            }
            break;
        case RequestedExtScript:
            for (int i=0; i<extScripts.count(); i++) {
                if (extScripts[i]->fileName() != current) continue;
                if (extScripts[i]->tryDelete()) {
                    widgetDialog->takeItem(widgetDialog->row(item));
                    extScripts.clear();
                    extScripts = getExtScripts();
                }
                break;
            }
            break;
        case RequestedExtUpgrade:
            for (int i=0; i<extUpgrade.count(); i++) {
                if (extUpgrade[i]->fileName() != current) continue;
                if (extUpgrade[i]->tryDelete()) {
                    widgetDialog->takeItem(widgetDialog->row(item));
                    extUpgrade.clear();
                    extUpgrade = getExtUpgrade();
                }
                break;
            }
            break;
        case RequestedGraphicalItem:
            for (int i=0; i<graphicalItems.count(); i++) {
                if (graphicalItems[i]->fileName() != current) continue;
                if (graphicalItems[i]->tryDelete()) {
                    widgetDialog->takeItem(widgetDialog->row(item));
                    graphicalItems.clear();
                    graphicalItems = getGraphicalItems();
                }
                break;
            }
            break;
        case Nothing:
        default:
            break;
        }
    } else if (dialogButtons->buttonRole(button) == QDialogButtonBox::AcceptRole) {
        if (item == nullptr) return;
        QString current = item->text();
        switch (requestedItem) {
        case RequestedExtQuotes:
            for (int i=0; i<extQuotes.count(); i++) {
                if (extQuotes[i]->fileName() != current) continue;
                extQuotes[i]->showConfiguration();
                break;
            }
            break;
        case RequestedExtScript:
            for (int i=0; i<extScripts.count(); i++) {
                if (extScripts[i]->fileName() != current) continue;
                extScripts[i]->showConfiguration();
                break;
            }
            break;
        case RequestedExtUpgrade:
            for (int i=0; i<extUpgrade.count(); i++) {
                if (extUpgrade[i]->fileName() != current) continue;
                extUpgrade[i]->showConfiguration();
                break;
            }
            break;
        case RequestedGraphicalItem:
            for (int i=0; i<graphicalItems.count(); i++) {
                if (graphicalItems[i]->fileName() != current) continue;
                graphicalItems[i]->showConfiguration();
                break;
            }
            break;
        case Nothing:
        default:
            break;
        }
    }
}


void AWKeys::copyBar(const QString original)
{
    if (debug) qDebug() << PDEBUG;

    QStringList tagList;
    for (int i=0; i<graphicalItems.count(); i++)
        tagList.append(graphicalItems[i]->name());
    int number = 0;
    while (tagList.contains(QString("bar%1").arg(number)))
        number++;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("awesomewidgets/desktops"),
                                                 QStandardPaths::LocateDirectory);
    bool ok;
    QString name = QInputDialog::getText(0, i18n("Enter file name"),
                                         i18n("File name"), QLineEdit::Normal,
                                         QString(""), &ok);
    if ((!ok) || (name.isEmpty())) return;
    if (!name.endsWith(QString(".desktop"))) name += QString(".desktop");
    QStringList bars;
    bars.append(keys.filter((QRegExp(QString("cpu(?!cl).*")))));
    bars.append(keys.filter((QRegExp(QString("^gpu$")))));
    bars.append(keys.filter((QRegExp(QString("^mem$")))));
    bars.append(keys.filter((QRegExp(QString("^swap$")))));
    bars.append(keys.filter((QRegExp(QString("^hdd[0-9].*")))));
    bars.append(keys.filter((QRegExp(QString("^bat.*")))));

    int originalItem = -1;
    for (int i=0; i<graphicalItems.count(); i++) {
        if ((graphicalItems[i]->fileName() != original) ||
            (graphicalItems[i]->fileName() != name))
            continue;
        originalItem = i;
        break;
    }
    GraphicalItem *item = new GraphicalItem(0, name, dirs, debug);
    item->setApiVersion(AWGIAPI);
    item->setName(QString("bar%1").arg(number));
    if (originalItem != -1) {
        item->setComment(graphicalItems[originalItem]->comment());
        item->setBar(graphicalItems[originalItem]->bar());
        item->setActiveColor(graphicalItems[originalItem]->activeColor());
        item->setInactiveColor(graphicalItems[originalItem]->inactiveColor());
        item->setType(graphicalItems[originalItem]->type());
        item->setDirection(graphicalItems[originalItem]->direction());
        item->setHeight(graphicalItems[originalItem]->height());
        item->setWidth(graphicalItems[originalItem]->width());
    }

    if (item->showConfiguration(bars) == 1) {
        graphicalItems.clear();
        graphicalItems = getGraphicalItems();
        QListWidgetItem *widgetItem = new QListWidgetItem(item->fileName());
        QStringList tooltip;
        tooltip.append(i18n("Tag: %1", item->name() + item->bar()));
        tooltip.append(i18n("Comment: %1", item->comment()));
        widgetItem->setToolTip(tooltip.join(QChar('\n')));
        widgetDialog->addItem(widgetItem);
        widgetDialog->sortItems();
    }
    delete item;
}


void AWKeys::copyQuotes(const QString original)
{
    if (debug) qDebug() << PDEBUG;

    QList<int> tagList;
    for (int i=0; i<extQuotes.count(); i++)
        tagList.append(extQuotes[i]->number());
    int number = 0;
    while (tagList.contains(number))
        number++;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("awesomewidgets/quotes"),
                                                 QStandardPaths::LocateDirectory);
    bool ok;
    QString name = QInputDialog::getText(0, i18n("Enter file name"),
                                         i18n("File name"), QLineEdit::Normal,
                                         QString(""), &ok);
    if ((!ok) || (name.isEmpty())) return;
    if (!name.endsWith(QString(".desktop"))) name += QString(".desktop");

    int originalItem = -1;
    for (int i=0; i<extQuotes.count(); i++) {
        if ((extQuotes[i]->fileName() != original) ||
            (extQuotes[i]->fileName() != name))
            continue;
        originalItem = i;
        break;
    }
    ExtQuotes *quotes = new ExtQuotes(0, name, dirs, debug);
    quotes->setApiVersion(AWEQAPI);
    quotes->setNumber(number);
    if (originalItem != -1) {
        quotes->setActive(extQuotes[originalItem]->isActive());
        quotes->setComment(extQuotes[originalItem]->comment());
        quotes->setInterval(extQuotes[originalItem]->interval());
        quotes->setName(extQuotes[originalItem]->name());
        quotes->setTicker(extQuotes[originalItem]->ticker());
    }

    if (quotes->showConfiguration() == 1) {
        extQuotes.clear();
        extQuotes = getExtQuotes();
        QListWidgetItem *widgetItem = new QListWidgetItem(quotes->fileName());
        QStringList tooltip;
        tooltip.append(i18n("Name: %1", quotes->name()));
        tooltip.append(i18n("Comment: %1", quotes->comment()));
        tooltip.append(i18n("Ticker: %1", quotes->ticker()));
        widgetItem->setToolTip(tooltip.join(QChar('\n')));
        widgetDialog->addItem(widgetItem);
        widgetDialog->sortItems();
    }
    delete quotes;
}



void AWKeys::copyScript(const QString original)
{
    if (debug) qDebug() << PDEBUG;

    QList<int> tagList;
    for (int i=0; i<extScripts.count(); i++)
        tagList.append(extScripts[i]->number());
    int number = 0;
    while (tagList.contains(number))
        number++;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("awesomewidgets/scripts"),
                                                 QStandardPaths::LocateDirectory);
    bool ok;
    QString name = QInputDialog::getText(0, i18n("Enter file name"),
                                         i18n("File name"), QLineEdit::Normal,
                                         QString(""), &ok);
    if ((!ok) || (name.isEmpty())) return;
    if (!name.endsWith(QString(".desktop"))) name += QString(".desktop");

    int originalItem = -1;
    for (int i=0; i<extScripts.count(); i++) {
        if ((extScripts[i]->fileName() != original) ||
            (extScripts[i]->fileName() != name))
            continue;
        originalItem = i;
        break;
    }
    ExtScript *script = new ExtScript(0, name, dirs, debug);
    script->setApiVersion(AWESAPI);
    script->setNumber(number);
    if (originalItem != -1) {
        script->setActive(extScripts[originalItem]->isActive());
        script->setComment(extScripts[originalItem]->comment());
        script->setExecutable(extScripts[originalItem]->executable());
        script->setHasOutput(extScripts[originalItem]->hasOutput());
        script->setInterval(extScripts[originalItem]->interval());
        script->setName(extScripts[originalItem]->name());
        script->setPrefix(extScripts[originalItem]->prefix());
        script->setRedirect(extScripts[originalItem]->redirect());
    }

    if (script->showConfiguration() == 1) {
        extScripts.clear();
        extScripts = getExtScripts();
        QListWidgetItem *widgetItem = new QListWidgetItem(script->fileName());
        QStringList tooltip;
        tooltip.append(i18n("Name: %1", script->name()));
        tooltip.append(i18n("Comment: %1", script->comment()));
        tooltip.append(i18n("Exec: %1", script->executable()));
        widgetItem->setToolTip(tooltip.join(QChar('\n')));
        widgetDialog->addItem(widgetItem);
        widgetDialog->sortItems();
    }
    delete script;
}


void AWKeys::copyUpgrade(const QString original)
{
    if (debug) qDebug() << PDEBUG;

    QList<int> tagList;
    for (int i=0; i<extUpgrade.count(); i++)
        tagList.append(extUpgrade[i]->number());
    int number = 0;
    while (tagList.contains(number))
        number++;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("awesomewidgets/upgrade"),
                                                 QStandardPaths::LocateDirectory);
    bool ok;
    QString name = QInputDialog::getText(0, i18n("Enter file name"),
                                         i18n("File name"), QLineEdit::Normal,
                                         QString(""), &ok);
    if ((!ok) || (name.isEmpty())) return;
    if (!name.endsWith(QString(".desktop"))) name += QString(".desktop");

    int originalItem = -1;
    for (int i=0; i<extUpgrade.count(); i++) {
        if ((extUpgrade[i]->fileName() != original) ||
            (extUpgrade[i]->fileName() != name))
            continue;
        originalItem = i;
        break;
    }
    ExtUpgrade *upgrade = new ExtUpgrade(0, name, dirs, debug);
    upgrade->setApiVersion(AWEUAPI);
    upgrade->setNumber(number);
    if (originalItem != -1) {
        upgrade->setActive(extUpgrade[originalItem]->isActive());
        upgrade->setComment(extUpgrade[originalItem]->comment());
        upgrade->setExecutable(extUpgrade[originalItem]->executable());
        upgrade->setName(extUpgrade[originalItem]->name());
        upgrade->setNull(extUpgrade[originalItem]->null());
        upgrade->setInterval(extUpgrade[originalItem]->interval());
    }

    if (upgrade->showConfiguration() == 1) {
        extUpgrade.clear();
        extUpgrade = getExtUpgrade();
        QListWidgetItem *widgetItem = new QListWidgetItem(upgrade->fileName());
        QStringList tooltip;
        tooltip.append(i18n("Name: %1", upgrade->name()));
        tooltip.append(i18n("Comment: %1", upgrade->comment()));
        tooltip.append(i18n("Exec: %1", upgrade->executable()));
        widgetItem->setToolTip(tooltip.join(QChar('\n')));
        widgetDialog->addItem(widgetItem);
        widgetDialog->sortItems();
    }
    delete upgrade;
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


bool AWKeys::checkKeys(const QVariantMap data)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Data" << data;

    return (data.count() != 0);
}


QString AWKeys::htmlValue(QString key)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Requested key" << key;

    QString value = values[key];
    if (!key.startsWith(QString("custom")))
        value.replace(QString(" "), QString("&nbsp;"));
    return value;
}


int AWKeys::numberCpus()
{
    if (debug) qDebug() << PDEBUG;

    return QThread::idealThreadCount();
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


QStringList AWKeys::findGraphicalItems(const QString pattern)
{
    if (debug) qDebug() << PDEBUG;

    QStringList orderedKeys;
    for (int i=0; i<graphicalItems.count(); i++)
        orderedKeys.append(graphicalItems[i]->name() + graphicalItems[i]->bar());
    orderedKeys.sort();

    QStringList selectedKeys;
    for (int i=orderedKeys.count()-1; i>=0; i--)
        if (pattern.contains(QString("$%1").arg(orderedKeys[i]))) {
            if (debug) qDebug() << PDEBUG << ":" << "Found key" << orderedKeys[i];
            selectedKeys.append(orderedKeys[i]);
        }

    return selectedKeys;
}


QStringList AWKeys::findKeys(const QString pattern)
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


QList<ExtQuotes *> AWKeys::getExtQuotes()
{
    if (debug) qDebug() << PDEBUG;

    QList<ExtQuotes *> externalQuotes;
    // create directory at $HOME
    QString localDir = QString("%1/awesomewidgets/quotes")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    QDir localDirectory;
    if ((!localDirectory.exists(localDir)) && (localDirectory.mkpath(localDir)))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("awesomewidgets/quotes"),
                                                 QStandardPaths::LocateDirectory);
    QStringList names;
    for (int i=0; i<dirs.count(); i++) {
        QStringList files = QDir(dirs[i]).entryList(QDir::Files, QDir::Name);
        for (int j=0; j<files.count(); j++) {
            if (!files[j].endsWith(QString(".desktop"))) continue;
            if (names.contains(files[j])) continue;
            if (debug) qDebug() << PDEBUG << ":" << "Found file" << files[j] << "in" << dirs[i];
            names.append(files[j]);
            externalQuotes.append(new ExtQuotes(0, files[j], dirs, debug));
        }
    }

    return externalQuotes;
}


QList<ExtScript *> AWKeys::getExtScripts()
{
    if (debug) qDebug() << PDEBUG;

    QList<ExtScript *> externalScripts;
    // create directory at $HOME
    QString localDir = QString("%1/awesomewidgets/scripts")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    QDir localDirectory;
    if ((!localDirectory.exists(localDir)) && (localDirectory.mkpath(localDir)))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("awesomewidgets/scripts"),
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


QList<ExtUpgrade *> AWKeys::getExtUpgrade()
{
    if (debug) qDebug() << PDEBUG;

    QList<ExtUpgrade *> externalUpgrade;
    // create directory at $HOME
    QString localDir = QString("%1/awesomewidgets/upgrade")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    QDir localDirectory;
    if ((!localDirectory.exists(localDir)) && (localDirectory.mkpath(localDir)))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("awesomewidgets/upgrade"),
                                                 QStandardPaths::LocateDirectory);
    QStringList names;
    for (int i=0; i<dirs.count(); i++) {
        QStringList files = QDir(dirs[i]).entryList(QDir::Files, QDir::Name);
        for (int j=0; j<files.count(); j++) {
            if (!files[j].endsWith(QString(".desktop"))) continue;
            if (names.contains(files[j])) continue;
            if (debug) qDebug() << PDEBUG << ":" << "Found file" << files[j] << "in" << dirs[i];
            names.append(files[j]);
            externalUpgrade.append(new ExtUpgrade(0, files[j], dirs, debug));
        }
    }

    return externalUpgrade;
}


QList<GraphicalItem *> AWKeys::getGraphicalItems()
{
    if (debug) qDebug() << PDEBUG;

    QList<GraphicalItem *> items;
    // create directory at $HOME
    QString localDir = QString("%1/awesomewidgets/desktops")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    QDir localDirectory;
    if (localDirectory.mkpath(localDir))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("awesomewidgets/desktops"),
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


QStringList AWKeys::getTimeKeys()
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
