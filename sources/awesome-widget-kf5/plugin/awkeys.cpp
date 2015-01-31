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
#include <QHBoxLayout>
#include <QInputDialog>
#include <QNetworkInterface>
#include <QProcessEnvironment>
#include <QRegExp>
#include <QStandardPaths>
#include <QThread>

#include <pdebug/pdebug.h>

#include "awtooltip.h"
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

    dialog = new QDialog(0);
    widgetDialog = new QListWidget(dialog);
    dialogButtons = new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Close,
                                         Qt::Vertical, dialog);
    copyButton = dialogButtons->addButton(i18n("Copy"), QDialogButtonBox::ActionRole);
    createButton = dialogButtons->addButton(i18n("Create"), QDialogButtonBox::ActionRole);
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

    delete dialog;
}


void AWKeys::initKeys(const QString pattern,
                      const QMap<QString, QVariant> params,
                      const QMap<QString, QVariant> tooltipParams)
{
    if (debug) qDebug() << PDEBUG;

    // clear
    extScripts.clear();
    extUpgrade.clear();
    graphicalItems.clear();
    counts.clear();
    keys.clear();
    foundBars.clear();
    foundKeys.clear();
    toolTip = nullptr;

    // init
    extScripts = getExtScripts();
    extUpgrade = getExtUpgrade();
    graphicalItems = getGraphicalItems();
    counts = getCounts(params);
    keys = dictKeys();
    foundBars = findGraphicalItems(pattern);
    foundKeys = findKeys(pattern);
    toolTip = new AWToolTip(this, tooltipParams);

    ready = true;
}


bool AWKeys::isDebugEnabled()
{
    if (debug) qDebug() << PDEBUG;

    return debug;
}


bool AWKeys::isReady()
{
    if (debug) qDebug() << PDEBUG;

    return ready;
}


QString AWKeys::parsePattern(const QString pattern)
{
    if (debug) qDebug() << PDEBUG;

    QString parsed = pattern;
    parsed.replace(QString("$$"), QString("$\\$\\"));
    for (int i=0; i<foundKeys.count(); i++)
        parsed.replace(QString("$") + foundKeys[i], valueByKey(foundKeys[i]));
    for (int i=0; i<foundBars.count(); i++)
        parsed.replace(QString("$") + foundBars[i], getItemByTag(foundBars[i])->image(valueByKey(foundBars[i]).toFloat()));
    parsed.replace(QString("$\\$\\"), QString("$$"));

    return parsed;
}


QPixmap AWKeys::toolTipImage()
{
    if(debug) qDebug() << PDEBUG;

    return toolTip->image();
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
                if (data[QString("ac")].toBool())
                    values[QString("ac")] = params[QString("acOnline")].toString();
                else
                    values[QString("ac")] = params[QString("acOffline")].toString();
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
        values[QString("gpu")] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
    } else if (sourceName == QString("gputemp")) {
        // gpu temperature
        values[QString("gputemp")] = QString("%1").arg(
            temperature(data[QString("value")].toFloat(), params[QString("tempUnits")].toString()), 4, 'f', 1);
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
    }
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


void AWKeys::editItemButtonPressed(QAbstractButton *button)
{
    if (debug) qDebug() << PDEBUG;

    QListWidgetItem *item = widgetDialog->currentItem();
    if (dynamic_cast<QPushButton *>(button) == copyButton) {
        if (item == nullptr) return;
        QString current = item->text();
        switch (requestedItem) {
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
    } else if (dialogButtons->buttonRole(button) == QDialogButtonBox::AcceptRole) {
        if (item == nullptr) return;
        QString current = item->text();
        switch (requestedItem) {
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

    int number = 0;
    while (true) {
        bool exit = true;
        for (int i=0; i<graphicalItems.count(); i++)
            if (graphicalItems[i]->name() == QString("bar%1").arg(number)) {
                number++;
                exit = false;
                break;
            }
        if (exit) break;
    }
    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("plasma_applet_awesome-widget/desktops"),
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

    item->showConfiguration(bars);
    delete item;
}


void AWKeys::copyScript(const QString original)
{
    if (debug) qDebug() << PDEBUG;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("plasma_dataengine_extsysmon/scripts"),
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

    script->showConfiguration();
    delete script;
}


void AWKeys::copyUpgrade(const QString original)
{
    if (debug) qDebug() << PDEBUG;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("plasma_dataengine_extsysmon/upgrade"),
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
    ExtUpgrade *uprade = new ExtUpgrade(0, name, dirs, debug);
    if (originalItem != -1) {
        uprade->setActive(extUpgrade[originalItem]->isActive());
        uprade->setComment(extUpgrade[originalItem]->comment());
        uprade->setExecutable(extUpgrade[originalItem]->executable());
        uprade->setName(extUpgrade[originalItem]->name());
        uprade->setNull(extUpgrade[originalItem]->null());
    }

    uprade->showConfiguration();
    delete uprade;
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


QMap<QString, QVariant> AWKeys::getCounts(const QMap<QString, QVariant> params)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QVariant> awCounts;
    awCounts[QString("cpu")] = numberCpus();
    awCounts[QString("custom")] = extScripts.count();
    awCounts[QString("disk")] = params[QString("disk")].toString().split(QString("@@")).count();
    awCounts[QString("fan")] = params[QString("fanDevice")].toString().split(QString("@@")).count();
    awCounts[QString("hddtemp")] = params[QString("hdd")].toString().split(QString("@@")).count();
    awCounts[QString("mount")] = params[QString("mount")].toString().split(QString("@@")).count();
    awCounts[QString("pkg")] = extUpgrade.count();
    awCounts[QString("temp")] = params[QString("tempDevice")].toString().split(QString("@@")).count();

    return awCounts;
}


QList<ExtScript *> AWKeys::getExtScripts()
{
    if (debug) qDebug() << PDEBUG;

    QList<ExtScript *> externalScripts;
    // create directory at $HOME
    QString localDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
            QString("/plasma_dataengine_extsysmon/scripts");
    QDir localDirectory;
    if ((!localDirectory.exists(localDir)) && (localDirectory.mkpath(localDir)))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("plasma_dataengine_extsysmon/scripts"),
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
    QString localDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
            QString("/plasma_dataengine_extsysmon/upgrade");
    QDir localDirectory;
    if ((!localDirectory.exists(localDir)) && (localDirectory.mkpath(localDir)))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QString("plasma_dataengine_extsysmon/upgrade"),
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
    QString localDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
            QString("/plasma_applet_awesome-widget/desktops");
    QDir localDirectory;
    if (localDirectory.mkpath(localDir))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
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
