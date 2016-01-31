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

#include "awkeycache.h"

#include <QDir>
#include <QNetworkInterface>
#include <QRegExp>
#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"


bool AWKeyCache::addKeyToCache(const QString type, const QString key)
{
    qCDebug(LOG_AW) << "Key" << key << "with type" << type;

    QString fileName = QString("%1/awesomewidgets.ndx")
                           .arg(QStandardPaths::writableLocation(
                               QStandardPaths::GenericCacheLocation));
    qCInfo(LOG_AW) << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);

    cache.beginGroup(type);
    QStringList cachedValues;
    for (auto key : cache.allKeys())
        cachedValues.append(cache.value(key).toString());

    if (type == QString("hdd")) {
        QStringList allDevices
            = QDir(QString("/dev")).entryList(QDir::System, QDir::Name);
        QStringList devices
            = allDevices.filter(QRegExp(QString("^[hms]d[a-z]$")));
        for (auto dev : devices) {
            QString device = QString("/dev/%1").arg(dev);
            if (cachedValues.contains(device))
                continue;
            qCInfo(LOG_AW) << "Found new key" << device << "for type" << type;
            cache.setValue(
                QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')),
                device);
        }
    } else if (type == QString("net")) {
        QList<QNetworkInterface> rawInterfaceList
            = QNetworkInterface::allInterfaces();
        for (auto interface : rawInterfaceList) {
            QString device = interface.name();
            if (cachedValues.contains(device))
                continue;
            qCInfo(LOG_AW) << "Found new key" << device << "for type" << type;
            cache.setValue(
                QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')),
                device);
        }
    } else {
        if (cachedValues.contains(key))
            return false;
        qCInfo(LOG_AW) << "Found new key" << key << "for type" << type;
        cache.setValue(
            QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), key);
    }
    cache.endGroup();

    cache.sync();
    return true;
}


QHash<QString, QStringList> AWKeyCache::loadKeysFromCache()
{
    QString fileName = QString("%1/awesomewidgets.ndx")
                           .arg(QStandardPaths::writableLocation(
                               QStandardPaths::GenericCacheLocation));
    qCInfo(LOG_AW) << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);

    QHash<QString, QStringList> devices;
    for (auto group : cache.childGroups()) {
        cache.beginGroup(group);
        for (auto key : cache.allKeys())
            devices[group].append(cache.value(key).toString());
        cache.endGroup();
    }

    return devices;
}
