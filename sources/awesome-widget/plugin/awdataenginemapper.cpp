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

#include "awdataenginemapper.h"

#include <QRegularExpression>

#include "awdebug.h"
#include "awformatterhelper.h"


AWDataEngineMapper::AWDataEngineMapper(QObject *_parent, AWFormatterHelper *_custom)
    : QObject(_parent)
    , m_customFormatters(_custom)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    // default formatters
    // memory
    m_formatter["mem"] = AWKeysAggregator::FormatterType::Float;
    m_formatter["memtotmb"] = AWKeysAggregator::FormatterType::MemMBFormat;
    m_formatter["memtotgb"] = AWKeysAggregator::FormatterType::MemGBFormat;
    // network
    m_formatter["down"] = AWKeysAggregator::FormatterType::NetSmartFormat;
    m_formatter["downkb"] = AWKeysAggregator::FormatterType::Integer;
    m_formatter["downtot"] = AWKeysAggregator::FormatterType::MemMBFormat;
    m_formatter["downtotkb"] = AWKeysAggregator::FormatterType::Integer;
    m_formatter["downunits"] = AWKeysAggregator::FormatterType::NetSmartUnits;
    m_formatter["up"] = AWKeysAggregator::FormatterType::NetSmartFormat;
    m_formatter["upkb"] = AWKeysAggregator::FormatterType::Integer;
    m_formatter["uptot"] = AWKeysAggregator::FormatterType::MemMBFormat;
    m_formatter["uptotkb"] = AWKeysAggregator::FormatterType::Integer;
    m_formatter["upunits"] = AWKeysAggregator::FormatterType::NetSmartUnits;
    // swap
    m_formatter["swap"] = AWKeysAggregator::FormatterType::Float;
    m_formatter["swaptotmb"] = AWKeysAggregator::FormatterType::MemMBFormat;
    m_formatter["swaptotgb"] = AWKeysAggregator::FormatterType::MemGBFormat;
}


AWKeysAggregator::FormatterType AWDataEngineMapper::formatter(const QString &_key) const
{
    qCDebug(LOG_AW) << "Get formatter for key" << _key;

    return m_formatter.value(_key, AWKeysAggregator::FormatterType::NoFormat);
}


QStringList AWDataEngineMapper::keysFromSource(const QString &_source) const
{
    qCDebug(LOG_AW) << "Search for source" << _source;

    return m_map.values(_source);
}


// HACK units required to define should the value be calculated as temperature
// or fan data
QStringList AWDataEngineMapper::registerSource(const QString &_source, const KSysGuard::Unit _units,
                                               const QStringList &_keys)
{
    qCDebug(LOG_AW) << "Source" << _source << "with units" << _units;

    // regular expressions
    static auto cpuRegExp = QRegularExpression("^cpu/cpu.*/usage$");
    static auto cpuclRegExp = QRegularExpression("^cpu/cpu.*/frequency$");
    static auto cpuTempRegExp = QRegularExpression("^cpu/cpu.*/temperature$");
    static auto gpuRegExp = QRegularExpression("^gpu/gpu.*/usage$");
    static auto gpuTempRegExp = QRegularExpression("^gpu/gpu.*/temperature$");
    static auto hddrRegExp = QRegularExpression("^disk/.*/read$");
    static auto hddwRegExp = QRegularExpression("^disk/.*/write$");
    static auto mountFillRegExp = QRegularExpression("^disk/.*/usedPercent$");
    static auto mountFreeRegExp = QRegularExpression("^disk/.*/free$");
    static auto mountUsedRegExp = QRegularExpression("^disk/.*/used$");
    static auto netRegExp = QRegularExpression("^network/.*/(download|upload)$");
    static auto netTotalRegExp = QRegularExpression("^network/.*/(totalDownload|totalUpload)$");

    if (_source == "extsysmon/battery/ac") {
        // AC
        m_map.insert(_source, "ac");
        m_formatter["ac"] = AWKeysAggregator::FormatterType::ACFormat;
    } else if (_source.startsWith("extsysmon/battery/")) {
        // battery stats
        auto key = _source;
        key.remove("extsysmon/battery/");
        m_map.insert(_source, key);
        m_formatter[key] = _source.contains("rate") ? AWKeysAggregator::FormatterType::Float
                                                    : AWKeysAggregator::FormatterType::IntegerThree;
    } else if (_source == "cpu/all/usage") {
        // cpu
        m_map.insert(_source, "cpu");
        m_formatter["cpu"] = AWKeysAggregator::FormatterType::Float;
    } else if (_source.contains(cpuRegExp)) {
        // cpus
        auto key = _source;
        key.remove("cpu/").remove("/usage");
        m_map.insert(_source, key);
        m_formatter[key] = AWKeysAggregator::FormatterType::Float;
    } else if (_source == "cpu/all/averageFrequency") {
        // cpucl
        m_map.insert(_source, "cpucl");
        m_formatter["cpucl"] = AWKeysAggregator::FormatterType::Integer;
    } else if (_source.contains(cpuclRegExp)) {
        // cpucls
        auto key = _source;
        key.remove("cpu/cpu").remove("/frequency");
        key = QString("cpucl%1").arg(key);
        m_map.insert(_source, key);
        m_formatter[key] = AWKeysAggregator::FormatterType::Integer;
    } else if (_source.startsWith("extsysmon/custom")) {
        // custom
        auto key = _source;
        key.remove("extsysmon/custom/");
        m_map.insert(_source, key);
        m_formatter[key] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source == "extsysmon/desktop/name") {
        // current desktop name
        m_map.insert(_source, "desktop");
        m_formatter["desktop"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source == "extsysmon/desktop/number") {
        // current desktop number
        m_map.insert(_source, "ndesktop");
        m_formatter["ndesktop"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source == "extsysmon/desktop/count") {
        // desktop count
        m_map.insert(_source, "tdesktops");
        m_formatter["tdesktops"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source.contains(hddrRegExp)) {
        // read speed
        auto device = _source;
        device.remove("disk/").remove("/read");
        auto index = m_devices["disk"].indexOf(device);
        if (index > -1) {
            QString key = QString("hddr%1").arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemKBFormat;
        }
    } else if (_source.contains(hddwRegExp)) {
        // write speed
        auto device = _source;
        device.remove("disk/").remove("/write");
        auto index = m_devices["disk"].indexOf(device);
        if (index > -1) {
            QString key = QString("hddw%1").arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemKBFormat;
        }
    } else if (_source == "gpu/all/usage") {
        // gpu load
        m_map.insert(_source, "gpu");
        m_formatter["gpu"] = AWKeysAggregator::FormatterType::Float;
    } else if (_source.contains(gpuRegExp)) {
        // gpus
        auto device = _source;
        device.remove("gpu/").remove("/usage");
        auto index = m_devices["gpu"].indexOf(device);
        if (index > -1) {
            auto key = QString("gpu%1").arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::Float;
        }
    } else if (_source.contains(gpuTempRegExp)) {
        // gpus temps
        auto device = _source;
        device.remove("gpu/").remove("/temperature");
        auto index = m_devices["gpu"].indexOf(device);
        if (index > -1) {
            auto key = QString("gputemp%1").arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::Temperature;
        }
    } else if (_source.contains(mountFillRegExp)) {
        // fill level
        auto device = _source;
        device.remove("disk/").remove("/usedPercent");
        auto index = m_devices["mount"].indexOf(device);
        if (index > -1) {
            auto key = QString("hdd%1").arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::Float;
            // additional keys
            m_formatter[QString("hddtotmb%1").arg(index)] = AWKeysAggregator::FormatterType::MemMBFormat;
            m_formatter[QString("hddtotgb%1").arg(index)] = AWKeysAggregator::FormatterType::MemGBFormat;
        }
    } else if (_source.contains(mountFreeRegExp)) {
        // free space
        auto device = _source;
        device.remove("disk/").remove("/free");
        auto index = m_devices["mount"].indexOf(device);
        if (index > -1) {
            // mb
            auto key = QString("hddfreemb%1").arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemMBFormat;
            // gb
            key = QString("hddfreegb%1").arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemGBFormat;
        }
    } else if (_source.contains(mountUsedRegExp)) {
        // used
        auto device = _source;
        device.remove("disk/").remove("/used");
        auto index = m_devices["mount"].indexOf(device);
        if (index > -1) {
            // mb
            auto key = QString("hddmb%1").arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemMBFormat;
            // gb
            key = QString("hddgb%1").arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemGBFormat;
        }
    } else if (_source.startsWith("cpu/loadaverages/loadaverage")) {
        // load average
        auto time = _source;
        time.remove("cpu/loadaverages/loadaverage");
        auto key = QString("la%1").arg(time);
        m_map.insert(_source, key);
        m_formatter[key] = AWKeysAggregator::FormatterType::FloatTwoSymbols;
    } else if (_source == "memory/physical/application") {
        // app memory
        // mb
        m_map.insert(_source, "memmb");
        m_formatter["memmb"] = AWKeysAggregator::FormatterType::MemMBFormat;
        // gb
        m_map.insert(_source, "memgb");
        m_formatter["memgb"] = AWKeysAggregator::FormatterType::MemGBFormat;
    } else if (_source == "memory/physical/free") {
        // free memory
        // mb
        m_map.insert(_source, "memfreemb");
        m_formatter["memfreemb"] = AWKeysAggregator::FormatterType::MemMBFormat;
        // gb
        m_map.insert(_source, "memfreegb");
        m_formatter["memfreegb"] = AWKeysAggregator::FormatterType::MemGBFormat;
    } else if (_source == "memory/physical/used") {
        // used memory
        // mb
        m_map.insert(_source, "memusedmb");
        m_formatter["memusedmb"] = AWKeysAggregator::FormatterType::MemMBFormat;
        // gb
        m_map.insert(_source, "memusedgb");
        m_formatter["memusedgb"] = AWKeysAggregator::FormatterType::MemGBFormat;
    } else if (_source == "extsysmon/network/device") {
        // network device
        m_map.insert(_source, "netdev");
        m_formatter["netdev"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source == "extsysmon/network/ssid") {
        // current ssid
        m_map.insert(_source, "ssid");
        m_formatter["ssid"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source.startsWith("extsysmon/requests/response")) {
        // network response
        auto key = _source;
        key.remove("extsysmon/requests/");
        m_map.insert(_source, key);
        m_formatter[key] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source.contains(netRegExp)) {
        // network speed
        auto type = _source.endsWith("download") ? "down" : "up";
        auto index = m_devices["net"].indexOf(_source.split('/')[1]);
        if (index > -1) {
            // kb
            auto key = QString("%1kb%2").arg(type).arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemKBFormat;
            // smart
            key = QString("%1%2").arg(type).arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::NetSmartFormat;
            // units
            key = QString("%1units%2").arg(type).arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::NetSmartUnits;
        }
    } else if (_source.contains(netTotalRegExp)) {
        // network data total
        auto type = _source.endsWith("Download") ? "down" : "up";
        auto index = m_devices["net"].indexOf(_source.split('/')[1]);
        if (index > -1) {
            // kb
            auto key = QString("%1totkb%2").arg(type).arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemKBFormat;
            // mb
            key = QString("%1tot%2").arg(type).arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemMBFormat;
        }
    } else if (_source.startsWith("extsysmon/upgrade")) {
        // package manager
        auto key = _source;
        key.remove("extsysmon/upgrade/");
        m_map.insert(_source, key);
        m_formatter[key] = AWKeysAggregator::FormatterType::IntegerThree;
    } else if (_source.startsWith("extsysmon/player")) {
        // player
        auto key = _source;
        key.remove("extsysmon/player/");
        m_map.insert(_source, key);
        m_formatter[key] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source == "extsysmon/ps/running") {
        // running processes count
        m_map.insert(_source, "pscount");
        m_formatter["pscount"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source == "extsysmon/ps/list") {
        // list of running processes
        m_map.insert(_source, "ps");
        m_formatter["ps"] = AWKeysAggregator::FormatterType::List;
    } else if (_source == "extsysmon/ps/count") {
        // total processes count
        m_map.insert(_source, "pstot");
        m_formatter["pstot"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source.startsWith("extsysmon/quotes")) {
        // quotes
        auto key = _source;
        key.remove("extsysmon/quotes/");
        m_map.insert(_source, key);
        m_formatter[key] = AWKeysAggregator::FormatterType::Quotes;
    } else if (_source == "memory/swap/free") {
        // free swap
        // mb
        m_map.insert(_source, "swapfreemb");
        m_formatter["swapfreemb"] = AWKeysAggregator::FormatterType::MemMBFormat;
        // gb
        m_map.insert(_source, "swapfreegb");
        m_formatter["swapfreegb"] = AWKeysAggregator::FormatterType::MemGBFormat;
    } else if (_source == "memory/swap/used") {
        // used swap
        // mb
        m_map.insert(_source, "swapmb");
        m_formatter["swapmb"] = AWKeysAggregator::FormatterType::MemMBFormat;
        // gb
        m_map.insert(_source, "swapgb");
        m_formatter["swapgb"] = AWKeysAggregator::FormatterType::MemGBFormat;
    } else if (_source.startsWith("lmsensors/") || _source.contains(cpuTempRegExp)
               || _source == "cpu/all/averageTemperature") {
        // temperature
        auto index = m_devices["temp"].indexOf(_source);
        // HACK on DE initialization there are no units key
        if (_units == KSysGuard::UnitInvalid)
            return QStringList({QString("temp%1").arg(index)});
        if (index > -1) {
            auto key = QString("temp%1").arg(index);
            m_map.insert(_source, key);
            m_formatter[key] = _units == KSysGuard::UnitCelsius ? AWKeysAggregator::FormatterType::Temperature
                                                                : AWKeysAggregator::FormatterType::Integer;
        }
    } else if (_source == "extsysmon/time/now") {
        // time
        m_map.insert(_source, "time");
        m_formatter["time"] = AWKeysAggregator::FormatterType::Time;
        // custom time
        m_map.insert(_source, "ctime");
        m_formatter["ctime"] = AWKeysAggregator::FormatterType::TimeCustom;
        // ISO time
        m_map.insert(_source, "isotime");
        m_formatter["isotime"] = AWKeysAggregator::FormatterType::TimeISO;
        // long time
        m_map.insert(_source, "longtime");
        m_formatter["longtime"] = AWKeysAggregator::FormatterType::TimeLong;
        // short time
        m_map.insert(_source, "shorttime");
        m_formatter["shorttime"] = AWKeysAggregator::FormatterType::TimeShort;
        // timestamp
        m_map.insert(_source, "tstime");
        m_formatter["tstime"] = AWKeysAggregator::FormatterType::Timestamp;
    } else if (_source == "extsysmon/system/brightness") {
        m_map.insert(_source, "brightness");
        m_formatter["brightness"] = AWKeysAggregator::FormatterType::IntegerThree;
    } else if (_source == "extsysmon/system/volume") {
        m_map.insert(_source, "volume");
        m_formatter["volume"] = AWKeysAggregator::FormatterType::IntegerThree;
    } else if (_source == "os/system/uptime") {
        // uptime
        m_map.insert(_source, "uptime");
        m_formatter["uptime"] = AWKeysAggregator::FormatterType::Uptime;
        // custom uptime
        m_map.insert(_source, "cuptime");
        m_formatter["cuptime"] = AWKeysAggregator::FormatterType::UptimeCustom;
    } else if (_source.startsWith("extsysmon/weather/temperature")) {
        // temperature
        auto key = _source;
        key.remove("extsysmon/weather/");
        m_map.insert(_source, key);
        m_formatter[key] = AWKeysAggregator::FormatterType::Temperature;
    } else if (_source.startsWith("extsysmon/weather/")) {
        // other weather
        auto key = _source;
        key.remove("extsysmon/weather/");
        m_map.insert(_source, key);
        m_formatter[key] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source.startsWith("extsysmon/load/load")) {
        // load source
        auto key = _source;
        key.remove("extsysmon/load/");
        m_map.insert(_source, key);
        m_formatter[key] = AWKeysAggregator::FormatterType::Temperature;
    }

    auto foundKeys = keysFromSource(_source);

    // rewrite formatters for custom ones
    QStringList customFormattersKeys;
    if (m_customFormatters)
        customFormattersKeys = m_customFormatters->definedFormatters();
    qCInfo(LOG_AW) << "Looking for formatters" << foundKeys << "in" << customFormattersKeys;
    for (auto &key : foundKeys) {
        if (!customFormattersKeys.contains(key))
            continue;
        m_formatter[key] = AWKeysAggregator::FormatterType::Custom;
    }

    // drop key from dictionary if no one user requested key required it
    qCInfo(LOG_AW) << "Looking for keys" << foundKeys << "in" << _keys;
    auto required = _keys.isEmpty() || std::any_of(foundKeys.cbegin(), foundKeys.cend(), [&_keys](auto &key) {
                        return _keys.contains(key);
                    });
    if (!required) {
        m_map.remove(_source);
        for (auto &key : foundKeys)
            m_formatter.remove(key);
    }

    return keysFromSource(_source);
}


void AWDataEngineMapper::setDevices(const QHash<QString, QStringList> &_devices)
{
    qCDebug(LOG_AW) << "Devices" << _devices;

    m_devices = _devices;
}
