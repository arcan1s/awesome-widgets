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

#include "awadds.h"

#include <KI18n/KLocalizedString>
#include <KNotifications/KNotification>

#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkInterface>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>
#include <QStandardPaths>
#include <QTextCodec>
#include <QThread>

#include <pdebug/pdebug.h>
//#include <task/taskadds.h>

#include "extscript.h"
#include "graphicalitem.h"
#include "version.h"


AWAdds::AWAdds(QObject *parent)
    : QObject(parent)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));

    initValues();
}


AWAdds::~AWAdds()
{
    if (debug) qDebug() << PDEBUG;
}


void AWAdds::checkUpdates()
{
    if (debug) qDebug() << PDEBUG;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(versionReplyRecieved(QNetworkReply *)));

    manager->get(QNetworkRequest(QUrl(VERSION_API)));
}


void AWAdds::initValues()
{
    if (debug) qDebug() << PDEBUG;

    // clear
    extScripts.clear();
    graphicalItems.clear();
    keys.clear();

    // init
    extScripts = getExtScripts();
    graphicalItems = getGraphicalItems();
    keys = dictKeys();
}


bool AWAdds::isDebugEnabled()
{
    if (debug) qDebug() << PDEBUG;

    return debug;
}


QString AWAdds::networkDevice(const QString custom)
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


int AWAdds::numberCpus()
{
    if (debug) qDebug() << PDEBUG;

    return QThread::idealThreadCount();
}


QString AWAdds::parsePattern(const QString pattern, const QMap<QString, QVariant> dict,
                             const QMap<QString, QVariant> values,
                             const QStringList foundKeys, const QStringList foundBars)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Dictionary" << dict;

    QString parsed = pattern;
    parsed.replace(QString("$$"), QString("$\\$\\"));
    for (int i=0; i<foundKeys.count(); i++)
        parsed.replace(QString("$") + foundKeys[i], values[foundKeys[i]]);
    for (int i=0; i<foundBars.count(); i++)
        parsed.replace(QString("$") + foundBars[i], getItemByTag(foundBars[i])->getImage(values[foundBars[i]].toFloat()));
    parsed.replace(QString("$\\$\\"), QString("$$"));

    return parsed;
}


float AWAdds::tempepature(const float temp, const QString units)
{
    if (debug) qDebug() << PDEBUG;

    float convertedTemp = temp;
    if (units == QString("Celsius"))
        ;
    else if (units == QString("Fahrenheit"))
        convertedTemp = temp * 9.0 / 5.0 + 32.0;
    else if (units == QString("Kelvin"))
        convertedTemp = temp + 273.15;
    else if (units == QString("Reaumur"))
        convertedTemp = temp * 0.8;
    else if (units == QString("cm^-1"))
        convertedTemp = (temp + 273.15) * 0.695;
    else if (units == QString("kJ/mol"))
        convertedTemp = (temp + 273.15) * 8.31;
    else if (units == QString("kcal/mol"))
        convertedTemp = (temp + 273.15) * 1.98;

    return convertedTemp;
}


QMap<QString, QVariant> AWAdds::counts()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QVariant> awCounts;
    awCounts[QString("cpu")] = numberCpus();
    awCounts[QString("custom")] = getExtScripts().count();
//    awCounts[QString("disk")] = configuration[QString("disk")].split(QString("@@")).count();
//    awCounts[QString("fan")] = configuration[QString("fanDevice")].split(QString("@@")).count();
//    awCounts[QString("hddtemp")] = configuration[QString("hdd")].split(QString("@@")).count();
//    awCounts[QString("mount")] = configuration[QString("mount")].split(QString("@@")).count();
//    awCounts[QString("pkg")] = deSettings[QString("PKGCMD")].split(QChar(',')).count();
//    awCounts[QString("temp")] = configuration[QString("tempDevice")].split(QString("@@")).count();
//    awCounts[QString("tooltip")] = 0;
//    awCounts[QString("tooltip")] += configuration[QString("cpuTooltip")].toInt();
//    awCounts[QString("tooltip")] += configuration[QString("cpuclTooltip")].toInt();
//    awCounts[QString("tooltip")] += configuration[QString("memTooltip")].toInt();
//    awCounts[QString("tooltip")] += configuration[QString("swapTooltip")].toInt();
//    awCounts[QString("tooltip")] += configuration[QString("downTooltip")].toInt();
//    awCounts[QString("tooltip")] += configuration[QString("batteryTooltip")].toInt();
//    awCounts[QString("tooltip")] = counts[QString("tooltip")] / 2;

    return awCounts;
}


QStringList AWAdds::dictKeys()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QVariant> awCounts = counts();
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
    for (int i=awCounts[QString("cpu")].toInt()-1; i>=0; i--)
        allKeys.append(QString("cpucl") + QString::number(i));
    allKeys.append(QString("cpucl"));
    // cpu
    for (int i=awCounts[QString("cpu")].toInt()-1; i>=0; i--)
        allKeys.append(QString("cpu") + QString::number(i));
    allKeys.append(QString("cpu"));
    // temperature
    for (int i=awCounts[QString("temp")].toInt()-1; i>=0; i--)
        allKeys.append(QString("temp") + QString::number(i));
    for (int i=awCounts[QString("fan")].toInt()-1; i>=0; i--)
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
    for (int i=awCounts[QString("mount")].toInt()-1; i>=0; i--) {
        allKeys.append(QString("hddmb") + QString::number(i));
        allKeys.append(QString("hddgb") + QString::number(i));
        allKeys.append(QString("hddfreemb") + QString::number(i));
        allKeys.append(QString("hddfreegb") + QString::number(i));
        allKeys.append(QString("hddtotmb") + QString::number(i));
        allKeys.append(QString("hddtotgb") + QString::number(i));
        allKeys.append(QString("hdd") + QString::number(i));
    }
    // hdd speed
    for (int i=awCounts[QString("disk")].toInt()-1; i>=0; i--) {
        allKeys.append(QString("hddr") + QString::number(i));
        allKeys.append(QString("hddw") + QString::number(i));
    }
    // hdd temp
    for (int i=awCounts[QString("hddtemp")].toInt()-1; i>=0; i--) {
        allKeys.append(QString("hddtemp") + QString::number(i));
        allKeys.append(QString("hddtemp") + QString::number(i));
    }
    // network
    allKeys.append(QString("down"));
    allKeys.append(QString("up"));
    allKeys.append(QString("netdev"));
    // battery
    allKeys.append(QString("ac"));
    for (int i=awCounts[QString("bat")].toInt()-1; i>=0; i--)
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
    for (int i=awCounts[QString("pkg")].toInt()-1; i>=0; i--)
        allKeys.append(QString("pkgcount") + QString::number(i));
    // custom
    for (int i=awCounts[QString("custom")].toInt()-1; i>=0; i--)
        allKeys.append(QString("custom") + QString::number(i));
    // desktop
    allKeys.append(QString("desktop"));
    allKeys.append(QString("ndesktop"));
    allKeys.append(QString("tdesktops"));

    return allKeys;
}


QStringList AWAdds::timeKeys()
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


QStringList AWAdds::findGraphicalItems(const QString pattern)
{
    if (debug) qDebug() << PDEBUG;

    QStringList orderedKeys;
    for (int i=0; i<graphicalItems.count(); i++)
        orderedKeys.append(graphicalItems[i]->getName() + graphicalItems[i]->getBar());
    orderedKeys.sort();

    QStringList selectedKeys;
    for (int i=orderedKeys.count()-1; i>=0; i--)
        if (pattern.contains(QString("$") + orderedKeys[i])) {
            if (debug) qDebug() << PDEBUG << ":" << "Found key" << orderedKeys[i];
            selectedKeys.append(orderedKeys[i]);
        }

    return selectedKeys;
}


QString AWAdds::findKeys(const QString pattern)
{
    QStringList selectedKeys;
    for (int i=0; i<keys.count(); i++)
        if (pattern.contains(QString("$") + keys[i])) {
            if (debug) qDebug() << PDEBUG << ":" << "Found key" << keys[i];
            selectedKeys.append(keys[i]);
        }

    return selectedKeys;
}


void AWAdds::runCmd(const QString cmd)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Cmd" << cmd;

    QProcess command;
    sendNotification(QString("Info"), i18n("Run %1", cmd));

    command.startDetached(cmd);
}


void AWAdds::sendNotification(const QString eventId, const QString message)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Event" << eventId;
    if (debug) qDebug() << PDEBUG << ":" << "Message" << message;

    KNotification *notification = KNotification::event(eventId, QString("Awesome Widget ::: ") + eventId, message);
    notification->setComponentName(QString("plasma-applet-org.kde.plasma.awesome-widget"));
}


void AWAdds::showReadme()
{
    if (debug) qDebug() << PDEBUG;

    QDesktopServices::openUrl(QString(HOMEPAGE));
}


QMap<QString, QVariant> AWAdds::readDataEngineConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QString fileName = QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("plasma-dataengine-extsysmon.conf"));
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QSettings settings(fileName, QSettings::IniFormat);
    QMap<QString, QVariant> rawConfig;

    settings.beginGroup(QString("Configuration"));
    rawConfig[QString("ACPIPATH")] = settings.value(QString("ACPIPATH"), QString("/sys/class/power_supply/"));
    rawConfig[QString("GPUDEV")] = settings.value(QString("GPUDEV"), QString("auto"));
    rawConfig[QString("HDDDEV")] = settings.value(QString("HDDDEV"), QString("all"));
    rawConfig[QString("HDDTEMPCMD")] = settings.value(QString("HDDTEMPCMD"), QString("sudo hddtemp"));
    rawConfig[QString("MPDADDRESS")] = settings.value(QString("MPDADDRESS"), QString("localhost"));
    rawConfig[QString("MPDPORT")] = settings.value(QString("MPDPORT"), QString("6600"));
    rawConfig[QString("MPRIS")] = settings.value(QString("MPRIS"), QString("auto"));
    rawConfig[QString("PKGCMD")] = settings.value(QString("PKGCMD"), QString("pacman -Qu"));
    rawConfig[QString("PKGNULL")] = settings.value(QString("PKGNULL"), QString("0"));
    rawConfig[QString("PLAYER")] = settings.value(QString("PLAYER"), QString("mpris"));
    settings.endGroup();

    return updateDataEngineConfiguration(rawConfig);
}


QMap<QString, QVariant> AWAdds::updateDataEngineConfiguration(QMap<QString, QVariant> rawConfig)
{
    if (debug) qDebug() << PDEBUG;

    for (int i=rawConfig[QString("PKGNULL")].toString().split(QString(","), QString::SkipEmptyParts).count();
         i<rawConfig[QString("PKGCMD")].toString().split(QString(","), QString::SkipEmptyParts).count()+1;
         i++)
        rawConfig[QString("PKGNULL")].toString() += QString(",0");

    for (int i=0; i<rawConfig.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" <<
            rawConfig.keys()[i] << QString("=") << rawConfig[rawConfig.keys()[i]];
    return rawConfig;
}


void AWAdds::writeDataEngineConfiguration(const QMap<QString, QVariant> configuration)
{
    if (debug) qDebug() << PDEBUG;

    QString fileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QString("/plasma-dataengine-extsysmon.conf");
    QSettings settings(fileName, QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Configuration"));
    settings.setValue(QString("GPUDEV"), configuration[QString("GPUDEV")]);
    settings.setValue(QString("HDDDEV"), configuration[QString("HDDDEV")]);
    settings.setValue(QString("HDDTEMPCMD"), configuration[QString("HDDTEMPCMD")]);
    settings.setValue(QString("MPDADDRESS"), configuration[QString("MPDADDRESS")]);
    settings.setValue(QString("MPDPORT"), configuration[QString("MPDPORT")]);
    settings.setValue(QString("MPRIS"), configuration[QString("MPRIS")]);
    settings.setValue(QString("PKGCMD"), configuration[QString("PKGCMD")]);
    settings.setValue(QString("PKGNULL"), configuration[QString("PKGNULL")]);
    settings.setValue(QString("PLAYER"), configuration[QString("PLAYER")]);
    settings.endGroup();

    settings.sync();
}


void AWAdds::showUpdates(QString version)
{
    if (debug) qDebug() << PDEBUG;

    QString text;
    text += i18n("Current version : %1", QString(VERSION)) + QString("\n");
    text += i18n("New version : %1", version) + QString("\n\n");
    text += i18n("Click \"Ok\" to download");

    int select = QMessageBox::information(0, i18n("There are updates"), text, QMessageBox::Ok | QMessageBox::Cancel);
    switch(select) {
    case QMessageBox::Ok:
        QDesktopServices::openUrl(QString(RELEASES) + version);
        break;
    }
}


void AWAdds::versionReplyRecieved(QNetworkReply *reply)
{
    if (debug) qDebug() << PDEBUG;

    QString answer = reply->readAll();
    if (!answer.contains(QString("tag_name"))) return;
    QString version = QString(VERSION);
    if (debug) qDebug() << PDEBUG << answer;
    for (int i=0; i<answer.split(QString("tag_name")).count(); i++) {
        version = answer.split(QString("tag_name"))[1].split(QChar(','))[0];
        version.remove(QChar('"'));
        version.remove(QChar(':'));
        version.remove(QString("V."));
        break;
    }

    int old_major = QString(VERSION).split(QChar('.'))[0].toInt();
    int old_minor = QString(VERSION).split(QChar('.'))[1].toInt();
    int old_patch = QString(VERSION).split(QChar('.'))[2].toInt();
    int new_major = QString(version).split(QChar('.'))[0].toInt();
    int new_minor = QString(version).split(QChar('.'))[1].toInt();
    int new_patch = QString(version).split(QChar('.'))[2].toInt();
    if ((old_major < new_major) ||
        ((old_major == new_major) && (old_minor < new_minor)) ||
        ((old_major == new_major) && (old_minor == new_minor) && (old_patch < new_patch)))
        showUpdates(version);
}


QList<ExtScript *> AWAdds::getExtScripts()
{
    if (debug) qDebug() << PDEBUG;

    QList<ExtScript *> externalScripts;
    // create directory at $HOME
    QString localDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
            QString("/plasma_engine_extsysmon/scripts");
    QDir localDirectory;
    if (localDirectory.mkdir(localDir))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::DataLocation,
                                                 QString("plasma_engine_extsysmon/scripts"),
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


QList<GraphicalItem *> AWAdds::getGraphicalItems()
{
    if (debug) qDebug() << PDEBUG;

    QList<GraphicalItem *> items;
    // create directory at $HOME
    QString localDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
            QString("/plasma_applet_awesome-widget/desktops");
    QDir localDirectory;
    if (localDirectory.mkdir(localDir))
        if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::DataLocation,
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


GraphicalItem *AWAdds::getItemByTag(const QString tag)
{
    if (debug) qDebug() << PDEBUG;

    GraphicalItem *item = nullptr;
    for (int i=0; i< graphicalItems.count(); i++) {
        if ((graphicalItems[i]->getName() + graphicalItems[i]->getBar()) != tag) continue;
        item = graphicalItems[i];
        break;
    }

    return item;
}
