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
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"


bool AWKeyCache::addKeyToCache(const QString &_type, const QString &_key)
{
    qCDebug(LOG_AW) << "Key" << _key << "with type" << _type;

    QString fileName
        = QString("%1/awesomewidgets.ndx").arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    qCInfo(LOG_AW) << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);

    cache.beginGroup(_type);
    QStringList cachedValues;
    for (auto &number : cache.allKeys())
        cachedValues.append(cache.value(number).toString());

    if (_type == "hdd") {
        QStringList allDevices = QDir("/dev").entryList(QDir::System, QDir::Name);
        QStringList devices = allDevices.filter(QRegularExpression("^[hms]d[a-z]$"));
        for (auto &dev : devices) {
            QString device = QString("/dev/%1").arg(dev);
            if (cachedValues.contains(device))
                continue;
            qCInfo(LOG_AW) << "Found new key" << device << "for type" << _type;
            cachedValues.append(device);
            cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), device);
        }
    } else if (_type == "net") {
        QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
        for (auto &interface : rawInterfaceList) {
            QString device = interface.name();
            if (cachedValues.contains(device))
                continue;
            qCInfo(LOG_AW) << "Found new key" << device << "for type" << _type;
            cachedValues.append(device);
            cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), device);
        }
    } else {
        if (cachedValues.contains(_key))
            return false;
        qCInfo(LOG_AW) << "Found new key" << _key << "for type" << _type;
        cache.setValue(QString("%1").arg(cache.allKeys().count(), 3, 10, QChar('0')), _key);
    }
    cache.endGroup();

    cache.sync();
    return true;
}


QStringList AWKeyCache::getRequiredKeys(const QStringList &_keys, const QStringList &_bars, const QVariantMap &_tooltip,
                                        const QStringList &_userKeys, const QStringList &_allKeys)
{
    qCDebug(LOG_AW) << "Looking for required keys in" << _keys << _bars << "using tooltip settings" << _tooltip;

    // initial copy
    QSet<QString> used(_keys.cbegin(), _keys.cend());
    used.unite(QSet(_bars.cbegin(), _bars.cend()));
    used.unite(QSet(_userKeys.cbegin(), _userKeys.cend()));
    // insert keys from tooltip
    for (auto &key : _tooltip.keys()) {
        if ((key.endsWith("Tooltip")) && (_tooltip[key].toBool())) {
            key.remove("Tooltip");
            used << key;
        }
    }

    // insert depending keys, refer to AWKeys::calculateValues()
    // hddtotmb*
    for (auto &key : _allKeys.filter(QRegularExpression("^hddtotmb"))) {
        if (!used.contains(key))
            continue;
        key.remove("hddtotmb");
        int index = key.toInt();
        used << QString("hddfreemb%1").arg(index) << QString("hddmb%1").arg(index);
    }
    // hddtotgb*
    for (auto &key : _allKeys.filter(QRegularExpression("^hddtotgb"))) {
        if (!used.contains(key))
            continue;
        key.remove("hddtotgb");
        int index = key.toInt();
        used << QString("hddfreegb%1").arg(index) << QString("hddgb%1").arg(index);
    }
    // mem
    if (used.contains("mem"))
        used << "memmb"
             << "memtotmb";
    // memtotmb
    if (used.contains("memtotmb"))
        used << "memusedmb"
             << "memfreemb";
    // memtotgb
    if (used.contains("memtotgb"))
        used << "memusedgb"
             << "memfreegb";
    // swap
    if (used.contains("swap"))
        used << "swapmb"
             << "swaptotmb";
    // swaptotmb
    if (used.contains("swaptotmb"))
        used << "swapmb"
             << "swapfreemb";
    // memtotgb
    if (used.contains("swaptotgb"))
        used << "swapgb"
             << "swapfreegb";
    // network keys
    QStringList netKeys(
        {"up", "upkb", "uptot", "uptotkb", "upunits", "down", "downkb", "downtot", "downtotkb", "downunits"});
    for (auto &key : netKeys) {
        if (!used.contains(key))
            continue;
        QStringList filt = _allKeys.filter(QRegularExpression(QString("^%1[0-9]{1,}").arg(key)));
        for (auto &filtered : filt)
            used << filtered;
    }
    // netdev key
    if (std::any_of(netKeys.cbegin(), netKeys.cend(), [&used](const QString &key) { return used.contains(key); }))
        used << "netdev";

    // HACK append dummy if there are no other keys. This hack is required
    // because empty list leads to the same behaviour as skip checking
    if (used.isEmpty())
        used << "dummy";

    return used.values();
}


QHash<QString, QStringList> AWKeyCache::loadKeysFromCache()
{
    QString fileName
        = QString("%1/awesomewidgets.ndx").arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    qCInfo(LOG_AW) << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);

    QHash<QString, QStringList> devices;
    for (auto &group : cache.childGroups()) {
        cache.beginGroup(group);
        for (auto &key : cache.allKeys())
            devices[group].append(cache.value(key).toString());
        cache.endGroup();
    }

    return devices;
}
