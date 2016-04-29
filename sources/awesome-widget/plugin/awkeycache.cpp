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


QStringList AWKeyCache::getRequiredKeys(const QStringList &keys,
                                        const QStringList &bars,
                                        const QVariantMap &tooltip,
                                        const QStringList &allKeys)
{
    qCDebug(LOG_AW) << "Looking for required keys in" << keys << bars
                    << "using tooltip settings" << tooltip;

    // initial copy
    QSet<QString> used = QSet<QString>::fromList(keys);
    used.unite(QSet<QString>::fromList(bars));
    // insert keys from tooltip
    for (auto key : tooltip.keys()) {
        if ((key.endsWith(QString("Tooltip"))) && (tooltip[key].toBool())) {
            key.remove(QString("Tooltip"));
            used << key;
        }
    }

    // insert depending keys, refer to AWKeys::calculateValues()
    // hddtotmb*
    for (auto key : allKeys.filter(QRegExp(QString("^hddtotmb")))) {
        if (!used.contains(key))
            continue;
        key.remove(QString("hddtotmb"));
        int index = key.toInt();
        used << QString("hddfreemb%1").arg(index)
             << QString("hddmb%1").arg(index);
    }
    // hddtotgb*
    for (auto key : allKeys.filter(QRegExp(QString("^hddtotgb")))) {
        if (!used.contains(key))
            continue;
        key.remove(QString("hddtotgb"));
        int index = key.toInt();
        used << QString("hddfreegb%1").arg(index)
             << QString("hddgb%1").arg(index);
    }
    // mem
    if (used.contains(QString("mem")))
        used << QString("memmb") << QString("memtotmb");
    // memtotmb
    if (used.contains(QString("memtotmb")))
        used << QString("memusedmb") << QString("memfreemb");
    // memtotgb
    if (used.contains(QString("memtotgb")))
        used << QString("memusedgb") << QString("memfreegb");
    // swap
    if (used.contains(QString("swap")))
        used << QString("swapmb") << QString("swaptotmb");
    // swaptotmb
    if (used.contains(QString("swaptotmb")))
        used << QString("swapmb") << QString("swapfreemb");
    // memtotgb
    if (used.contains(QString("swaptotgb")))
        used << QString("swapgb") << QString("swapfreegb");
    // network keys
    QStringList netKeys(QStringList()
                        << QString("up") << QString("upkb")
                        << QString("uptotal") << QString("uptotalkb")
                        << QString("upunits") << QString("down")
                        << QString("downkb") << QString("downtotal")
                        << QString("downtotalkb") << QString("downunits"));
    for (auto key : netKeys) {
        if (!used.contains(key))
            continue;
        QStringList filt
            = allKeys.filter(QRegExp(QString("^%1[0-9]{1,}").arg(key)));
        for (auto filtered : filt)
            used << filtered;
    }
    // netdev key
    if (std::any_of(netKeys.cbegin(), netKeys.cend(),
                    [&used](const QString &key) { return used.contains(key); }))
        used << QString("netdev");

    // HACK append dummy if there are no other keys. This hack is required
    // because empty list leads to the same behaviour as skip checking
    if (used.isEmpty())
        used << QString("dummy");

    return used.toList();
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
