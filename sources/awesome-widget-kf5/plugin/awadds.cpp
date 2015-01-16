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
#include <QNetworkAccessManager>
#include <QNetworkInterface>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcessEnvironment>
#include <QSettings>
#include <QStandardPaths>
#include <QTextCodec>

#include <pdebug/pdebug.h>
#include <task/taskadds.h>

#include "graphicalitem.h"
#include "version.h"


AWAdds::AWAdds(QObject *parent)
    : QObject(parent)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));
}


AWAdds::~AWAdds()
{
    if (debug) qDebug() << PDEBUG;
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

    QString cmd = QString("grep -c ^processor /proc/cpuinfo");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed().toInt();
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
