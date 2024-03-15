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
#include "extsysmonsensor.h"
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


ExtSysMonAggregator::ExtSysMonAggregator(const QString &_id, const QString &_name, KSysGuard::SensorPlugin *_parent,
                                         const QHash<QString, QString> &_config)
    : KSysGuard::SensorContainer(_id, _name, _parent)
{
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    init(_config);
}


void ExtSysMonAggregator::init(const QHash<QString, QString> &_config)
{
    qCDebug(LOG_ESM) << "Configuration" << _config;

    // battery
    auto batteryItem = new BatterySource(this, {_config["ACPIPATH"]});
    for (auto &source : batteryItem->sources())
        addObject(new ExtSysMonSensor(this, source, batteryItem));
    // custom
    auto customItem = new CustomSource(this, {});
    for (auto &source : customItem->sources())
        addObject(new ExtSysMonSensor(this, source, customItem));
    // desktop
    auto desktopItem = new DesktopSource(this, {});
    for (auto &source : desktopItem->sources())
        addObject(new ExtSysMonSensor(this, source, desktopItem));
    // gpu load
    auto gpuLoadItem = new GPULoadSource(this, {_config["GPUDEV"]});
    for (auto &source : gpuLoadItem->sources())
        addObject(new ExtSysMonSensor(this, source, gpuLoadItem));
    // gpu temperature
    auto gpuTempItem = new GPUTemperatureSource(this, {_config["GPUDEV"]});
    for (auto &source : gpuTempItem->sources())
        addObject(new ExtSysMonSensor(this, source, gpuTempItem));
    // hdd temperature
    auto hddTempItem
        = new HDDTemperatureSource(this, {_config["HDDDEV"], _config["HDDTEMPCMD"]});
    for (auto &source : hddTempItem->sources())
        addObject(new ExtSysMonSensor(this, source, hddTempItem));
    // network
    auto networkItem = new NetworkSource(this, QStringList());
    for (auto &source : networkItem->sources())
        addObject(new ExtSysMonSensor(this, source, networkItem));
    // player
    auto playerItem
        = new PlayerSource(this, {_config["PLAYER"], _config["MPDADDRESS"], _config["MPDPORT"],
                                  _config["MPRIS"], _config["PLAYERSYMBOLS"]});
    for (auto &source : playerItem->sources())
        addObject(new ExtSysMonSensor(this, source, playerItem));
    // processes
    auto processesItem = new ProcessesSource(this, {});
    for (auto &source : processesItem->sources())
        addObject(new ExtSysMonSensor(this, source, processesItem));
    // network request
    auto requestItem = new RequestSource(this, {});
    for (auto &source : requestItem->sources())
        addObject(new ExtSysMonSensor(this, source, requestItem));
    // quotes
    auto quotesItem = new QuotesSource(this, {});
    for (auto &source : quotesItem->sources())
        addObject(new ExtSysMonSensor(this, source, quotesItem));
    // system
    auto systemItem = new SystemInfoSource(this, {});
    for (auto &source : systemItem->sources())
        addObject(new ExtSysMonSensor(this, source, systemItem));
    // upgrade
    auto upgradeItem = new UpgradeSource(this, {});
    for (auto &source : upgradeItem->sources())
        addObject(new ExtSysMonSensor(this, source, upgradeItem));
    // weather
    auto weatherItem = new WeatherSource(this, {});
    for (auto &source : weatherItem->sources())
        addObject(new ExtSysMonSensor(this, source, weatherItem));
#ifdef BUILD_LOAD
    // additional load source
    auto loadItem = new LoadSource(this, QStringList());
    for (auto &source : loadItem->sources())
        addObject(new ExtSysMonSensor(this, source, loadItem));
#endif /* BUILD_LOAD */
}
