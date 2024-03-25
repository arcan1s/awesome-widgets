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
#include "timesource.h"
#include "upgradesource.h"
#include "weathersource.h"


ExtSysMonAggregator::ExtSysMonAggregator(const QString &_id, const QString &_name, KSysGuard::SensorPlugin *_parent,
                                         const QHash<QString, QString> &_config)
    : KSysGuard::SensorContainer(_id, _name, _parent)
{
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    init(_config);
}


void ExtSysMonAggregator::createSensor(const QString &_id, const QString &_name, AbstractExtSysMonSource *_source)
{
    qCDebug(LOG_ESM) << "Register sensor" << _name << "with id" << _id;

    addObject(new ExtSysMonSensor(this, _id, _name, _source));
}


void ExtSysMonAggregator::init(const QHash<QString, QString> &_config)
{
    qCDebug(LOG_ESM) << "Configuration" << _config;

    // battery
    createSensor("battery", i18n("Battery"), new BatterySource(this, {_config["ACPIPATH"]}));
    // custom
    createSensor("custom", i18n("Scripts"), new CustomSource(this, {}));
    // desktop
    // FIXME causes segfault in kde libs
//    createSensor("desktop", i18n("Desktop"), new DesktopSource(this, {}));
    // gpu load
    createSensor("gpuload", i18n("GPU load"), new GPULoadSource(this, {_config["GPUDEV"]}));
    // gpu temperature
    createSensor("gputemp", i18n("GPU temperature"), new GPUTemperatureSource(this, {_config["GPUDEV"]}));
    // hdd temperature
    createSensor("hdd", i18n("HDD temperature"),
                 new HDDTemperatureSource(this, {_config["HDDDEV"], _config["HDDTEMPCMD"]}));
    // network
    createSensor("network", i18n("Network"), new NetworkSource(this, {}));
    // player
    createSensor("player", i18n("Music player"),
                 new PlayerSource(this, {_config["PLAYER"], _config["MPDADDRESS"], _config["MPDPORT"], _config["MPRIS"],
                                         _config["PLAYERSYMBOLS"]}));
    // processes
    createSensor("ps", i18n("Processes"), new ProcessesSource(this, {}));
    // network request
    createSensor("requests", i18n("Network requests"), new RequestSource(this, {}));
    // quotes
    createSensor("quotes", i18n("Quotes"), new QuotesSource(this, {}));
    // system
    createSensor("system", i18n("System"), new SystemInfoSource(this, {}));
    // current time
    createSensor("time", i18n("Time"), new TimeSource(this, {}));
    // upgrade
    createSensor("upgrade", i18n("Upgrades"), new UpgradeSource(this, {}));
    // weather
    createSensor("weather", i18n("Weather"), new WeatherSource(this, {}));
#ifdef BUILD_LOAD
    // additional load source
    createSensor("load", i18n("Load"), new LoadSource(this, {}));
#endif /* BUILD_LOAD */
}
