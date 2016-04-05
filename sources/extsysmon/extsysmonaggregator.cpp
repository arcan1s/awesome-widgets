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
#include "sources/batterysource.h"
#include "sources/customsource.h"
#include "sources/desktopsource.h"
#include "sources/gpuloadsource.h"
#include "sources/gputempsource.h"
#include "sources/hddtempsource.h"
#include "sources/loadsource.h"
#include "sources/networksource.h"
#include "sources/playersource.h"
#include "sources/processessource.h"
#include "sources/quotessource.h"
#include "sources/updatesource.h"
#include "sources/upgradesource.h"
#include "sources/weathersource.h"
#include "version.h"


ExtSysMonAggregator::ExtSysMonAggregator(QObject *parent,
                                         const QHash<QString, QString> config)
    : QObject(parent)
{
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    init(config);
}


ExtSysMonAggregator::~ExtSysMonAggregator()
{
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    m_map.clear();
}


QVariant ExtSysMonAggregator::data(const QString source) const
{
    qCDebug(LOG_ESM) << "Source" << source;

    return m_map[source]->data(source);
}


bool ExtSysMonAggregator::hasSource(const QString source) const
{
    qCDebug(LOG_ESM) << "Source" << source;

    return m_map.contains(source);
}


QVariantMap ExtSysMonAggregator::initialData(const QString source) const
{
    qCDebug(LOG_ESM) << "Source" << source;

    return hasSource(source) ? m_map[source]->initialData(source)
                             : QVariantMap();
}


QStringList ExtSysMonAggregator::sources() const
{
    return m_map.keys();
}


void ExtSysMonAggregator::init(const QHash<QString, QString> config)
{
    qCDebug(LOG_ESM) << "Configuration" << config;

    // battery
    AbstractExtSysMonSource *batteryItem
        = new BatterySource(this, QStringList() << config[QString("ACPIPATH")]);
    for (auto source : batteryItem->sources())
        m_map[source] = batteryItem;
    // custom
    AbstractExtSysMonSource *customItem = new CustomSource(this, QStringList());
    for (auto source : customItem->sources())
        m_map[source] = customItem;
    // desktop
    AbstractExtSysMonSource *desktopItem
        = new DesktopSource(this, QStringList());
    for (auto source : desktopItem->sources())
        m_map[source] = desktopItem;
    // gpu load
    AbstractExtSysMonSource *gpuLoadItem
        = new GPULoadSource(this, QStringList() << config[QString("GPUDEV")]);
    for (auto source : gpuLoadItem->sources())
        m_map[source] = gpuLoadItem;
    // gpu temperature
    AbstractExtSysMonSource *gpuTempItem = new GPUTemperatureSource(
        this, QStringList() << config[QString("GPUDEV")]);
    for (auto source : gpuTempItem->sources())
        m_map[source] = gpuTempItem;
    // hdd temperature
    AbstractExtSysMonSource *hddTempItem = new HDDTemperatureSource(
        this, QStringList() << config[QString("HDDDEV")]
                            << config[QString("HDDTEMPCMD")]);
    for (auto source : hddTempItem->sources())
        m_map[source] = hddTempItem;
    // network
    AbstractExtSysMonSource *networkItem
        = new NetworkSource(this, QStringList());
    for (auto source : networkItem->sources())
        m_map[source] = networkItem;
    // player
    AbstractExtSysMonSource *playerItem = new PlayerSource(
        this, QStringList()
                  << config[QString("PLAYER")] << config[QString("MPDADDRESS")]
                  << config[QString("MPDPORT")] << config[QString("MPRIS")]
                  << config[QString("PLAYERSYMBOLS")]);
    for (auto source : playerItem->sources())
        m_map[source] = playerItem;
    // processes
    AbstractExtSysMonSource *processesItem
        = new ProcessesSource(this, QStringList());
    for (auto source : processesItem->sources())
        m_map[source] = processesItem;
    // quotes
    AbstractExtSysMonSource *quotesItem = new QuotesSource(this, QStringList());
    for (auto source : quotesItem->sources())
        m_map[source] = quotesItem;
    // update
    AbstractExtSysMonSource *updateItem = new UpdateSource(this, QStringList());
    for (auto source : updateItem->sources())
        m_map[source] = updateItem;
    // upgrade
    AbstractExtSysMonSource *upgradeItem
        = new UpgradeSource(this, QStringList());
    for (auto source : upgradeItem->sources())
        m_map[source] = upgradeItem;
    // weather
    AbstractExtSysMonSource *weatherItem
        = new WeatherSource(this, QStringList());
    for (auto source : weatherItem->sources())
        m_map[source] = weatherItem;
#ifdef BUILD_TESTING
    // additional load source
    AbstractExtSysMonSource *loadItem = new LoadSource(this, QStringList());
    for (auto source : loadItem->sources())
        m_map[source] = loadItem;
#endif /* BUILD_TESTING */
}
