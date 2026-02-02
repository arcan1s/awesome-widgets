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

    auto fileName
        = QString("%1/awesomewidgets.ndx").arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    qCInfo(LOG_AW) << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);

    cache.beginGroup(_type);
    QStringList cachedValues;
    for (auto &number : cache.allKeys())
        cachedValues.append(cache.value(number).toString());

    if (_type == "net") {
        auto rawInterfaceList = QNetworkInterface::allInterfaces();
        for (auto &interface : rawInterfaceList) {
            auto device = interface.name();
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
    QSet used(_keys.cbegin(), _keys.cend());
    used.unite(QSet(_bars.cbegin(), _bars.cend()));
    used.unite(QSet(_userKeys.cbegin(), _userKeys.cend()));
    // insert keys from tooltip
    for (auto [key, value] : _tooltip.asKeyValueRange()) {
        if (key.endsWith("Tooltip") && value.toBool()) {
            auto local = key;
            local.remove("Tooltip");
            used << local;
        }
    }

    // insert keys which depend on others, refer to AWKeys::calculateValues()
    // network keys
    static QStringList netKeys(
        {"up", "upkb", "uptot", "uptotkb", "upunits", "down", "downkb", "downtot", "downtotkb", "downunits"});
    for (auto &key : netKeys) {
        if (!used.contains(key))
            continue;
        auto filt = _allKeys.filter(QRegularExpression(QString("^%1[0-9]{1,}").arg(key)));
        for (auto &filtered : filt)
            used << filtered;
    }
    // netdev key
    if (std::any_of(netKeys.cbegin(), netKeys.cend(), [&used](auto &key) { return used.contains(key); }))
        used << "netdev";
    // gpu memory keys
    static auto gpuMemoryCalculatedRegExp = QRegularExpression("^gpu(mem|freemb|freegb)");
    for (auto key : _keys.filter(gpuMemoryCalculatedRegExp)) {
        auto index = key.remove(gpuMemoryCalculatedRegExp);
        used << QString("gpuusedmb%1").arg(index) << QString("gputotmb%1").arg(index)
             << QString("gpuusedgb%1").arg(index) << QString("gputotgb%1").arg(index);
    }

    return used.values();
}


AWPluginMatcherSettings AWKeyCache::loadKeysFromCache()
{
    auto fileName
        = QString("%1/awesomewidgets.ndx").arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    qCInfo(LOG_AW) << "Cache file" << fileName;
    QSettings cache(fileName, QSettings::IniFormat);

    AWPluginMatcherSettings devices;
    QHash<QString, QStringList *> groups = {
        {"disk", &devices.disk},   {"gpu", &devices.gpu},      {"mount", &devices.mount},
        {"net", &devices.network}, {"temp", &devices.sensors},
    };

    for (auto [group, list] : groups.asKeyValueRange()) {
        cache.beginGroup(group);
        for (auto &key : cache.allKeys())
            list->append(cache.value(key).toString());
        cache.endGroup();
    }

    return devices;
}
