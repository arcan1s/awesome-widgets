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

#include "extsysmonaggregator.h"

#include "awdebug.h"
#include "batterysource.h"
#include "customsource.h"
#include "desktopsource.h"
#include "gpuloadsource.h"
#include "gputempsource.h"
#include "hddtempsource.h"
#include "loadsource.h"
#include "networksource.h"
#include "playersource.h"
#include "processessource.h"
#include "quotessource.h"
#include "requestsource.h"
#include "systeminfosource.h"
#include "upgradesource.h"
#include "weathersource.h"


ExtSysMonAggregator::ExtSysMonAggregator(QObject *_parent, const QHash<QString, QString> &_config)
    : QObject(_parent)
{
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    init(_config);
}


ExtSysMonAggregator::~ExtSysMonAggregator()
{
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    m_map.clear();
}


QVariant ExtSysMonAggregator::data(const QString &_source) const
{
    qCDebug(LOG_ESM) << "Source" << _source;

    return m_map[_source]->data(_source);
}


bool ExtSysMonAggregator::hasSource(const QString &_source) const
{
    qCDebug(LOG_ESM) << "Source" << _source;

    return m_map.contains(_source);
}


QVariantMap ExtSysMonAggregator::initialData(const QString &_source) const
{
    qCDebug(LOG_ESM) << "Source" << _source;

    return hasSource(_source) ? m_map[_source]->initialData(_source) : QVariantMap();
}


QStringList ExtSysMonAggregator::sources() const
{
    return m_map.keys();
}


void ExtSysMonAggregator::init(const QHash<QString, QString> &_config)
{
    qCDebug(LOG_ESM) << "Configuration" << _config;

    // battery
    AbstractExtSysMonSource *batteryItem = new BatterySource(this, QStringList() << _config["ACPIPATH"]);
    for (auto &source : batteryItem->sources())
        m_map[source] = batteryItem;
    // custom
    AbstractExtSysMonSource *customItem = new CustomSource(this, QStringList());
    for (auto &source : customItem->sources())
        m_map[source] = customItem;
    // desktop
    AbstractExtSysMonSource *desktopItem = new DesktopSource(this, QStringList());
    for (auto &source : desktopItem->sources())
        m_map[source] = desktopItem;
    // gpu load
    AbstractExtSysMonSource *gpuLoadItem = new GPULoadSource(this, QStringList({_config["GPUDEV"]}));
    for (auto &source : gpuLoadItem->sources())
        m_map[source] = gpuLoadItem;
    // gpu temperature
    AbstractExtSysMonSource *gpuTempItem = new GPUTemperatureSource(this, QStringList({_config["GPUDEV"]}));
    for (auto &source : gpuTempItem->sources())
        m_map[source] = gpuTempItem;
    // hdd temperature
    AbstractExtSysMonSource *hddTempItem
        = new HDDTemperatureSource(this, QStringList({_config["HDDDEV"], _config["HDDTEMPCMD"]}));
    for (auto &source : hddTempItem->sources())
        m_map[source] = hddTempItem;
    // network
    AbstractExtSysMonSource *networkItem = new NetworkSource(this, QStringList());
    for (auto &source : networkItem->sources())
        m_map[source] = networkItem;
    // player
    AbstractExtSysMonSource *playerItem
        = new PlayerSource(this, QStringList({_config["PLAYER"], _config["MPDADDRESS"], _config["MPDPORT"],
                                              _config["MPRIS"], _config["PLAYERSYMBOLS"]}));
    for (auto &source : playerItem->sources())
        m_map[source] = playerItem;
    // processes
    AbstractExtSysMonSource *processesItem = new ProcessesSource(this, QStringList());
    for (auto &source : processesItem->sources())
        m_map[source] = processesItem;
    // network request
    AbstractExtSysMonSource *requestItem = new RequestSource(this, QStringList());
    for (auto &source : requestItem->sources())
        m_map[source] = requestItem;
    // quotes
    AbstractExtSysMonSource *quotesItem = new QuotesSource(this, QStringList());
    for (auto &source : quotesItem->sources())
        m_map[source] = quotesItem;
    // system
    AbstractExtSysMonSource *systemItem = new SystemInfoSource(this, QStringList());
    for (auto &source : systemItem->sources())
        m_map[source] = systemItem;
    // upgrade
    AbstractExtSysMonSource *upgradeItem = new UpgradeSource(this, QStringList());
    for (auto &source : upgradeItem->sources())
        m_map[source] = upgradeItem;
    // weather
    AbstractExtSysMonSource *weatherItem = new WeatherSource(this, QStringList());
    for (auto &source : weatherItem->sources())
        m_map[source] = weatherItem;
#ifdef BUILD_LOAD
    // additional load source
    AbstractExtSysMonSource *loadItem = new LoadSource(this, QStringList());
    for (auto &source : loadItem->sources())
        m_map[source] = loadItem;
#endif /* BUILD_LOAD */
}
