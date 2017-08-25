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

#include <QRegExp>

#include "awdebug.h"
#include "awformatterhelper.h"


AWDataEngineMapper::AWDataEngineMapper(QObject *_parent,
                                       AWFormatterHelper *_custom)
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


AWDataEngineMapper::~AWDataEngineMapper()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


AWKeysAggregator::FormatterType
AWDataEngineMapper::formatter(const QString &_key) const
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
QStringList AWDataEngineMapper::registerSource(const QString &_source,
                                               const QString &_units,
                                               const QStringList &_keys)
{
    qCDebug(LOG_AW) << "Source" << _source << "with units" << _units;

    // regular expressions
    QRegExp cpuRegExp = QRegExp("cpu/cpu.*/TotalLoad");
    QRegExp cpuclRegExp = QRegExp("cpu/cpu.*/clock");
    QRegExp hddrRegExp = QRegExp("disk/.*/Rate/rblk");
    QRegExp hddwRegExp = QRegExp("disk/.*/Rate/wblk");
    QRegExp mountFillRegExp = QRegExp("partitions/.*/filllevel");
    QRegExp mountFreeRegExp = QRegExp("partitions/.*/freespace");
    QRegExp mountUsedRegExp = QRegExp("partitions/.*/usedspace");
    QRegExp netRegExp
        = QRegExp("network/interfaces/.*/(receiver|transmitter)/data$");
    QRegExp netTotalRegExp
        = QRegExp("network/interfaces/.*/(receiver|transmitter)/dataTotal$");

    if (_source == "battery/ac") {
        // AC
        m_map[_source] = "ac";
        m_formatter["ac"] = AWKeysAggregator::FormatterType::ACFormat;
    } else if (_source.startsWith("battery/")) {
        // battery stats
        QString key = _source;
        key.remove("battery/");
        m_map[_source] = key;
        m_formatter[key] = _source.contains("rate")
                               ? AWKeysAggregator::FormatterType::Float
                               : AWKeysAggregator::FormatterType::IntegerThree;
    } else if (_source == "cpu/system/TotalLoad") {
        // cpu
        m_map[_source] = "cpu";
        m_formatter["cpu"] = AWKeysAggregator::FormatterType::Float;
    } else if (_source.contains(cpuRegExp)) {
        // cpus
        QString key = _source;
        key.remove("cpu/").remove("/TotalLoad");
        m_map[_source] = key;
        m_formatter[key] = AWKeysAggregator::FormatterType::Float;
    } else if (_source == "cpu/system/AverageClock") {
        // cpucl
        m_map[_source] = "cpucl";
        m_formatter["cpucl"] = AWKeysAggregator::FormatterType::Integer;
    } else if (_source.contains(cpuclRegExp)) {
        // cpucls
        QString key = _source;
        key.remove("cpu/cpu").remove("/clock");
        key = QString("cpucl%1").arg(key);
        m_map[_source] = key;
        m_formatter[key] = AWKeysAggregator::FormatterType::Integer;
    } else if (_source.startsWith("custom")) {
        // custom
        QString key = _source;
        key.remove("custom/");
        m_map[_source] = key;
        m_formatter[key] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source == "desktop/current/name") {
        // current desktop name
        m_map[_source] = "desktop";
        m_formatter["desktop"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source == "desktop/current/number") {
        // current desktop number
        m_map[_source] = "ndesktop";
        m_formatter["ndesktop"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source == "desktop/total/number") {
        // desktop count
        m_map[_source] = "tdesktops";
        m_formatter["tdesktops"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source.contains(hddrRegExp)) {
        // read speed
        QString device = _source;
        device.remove("/Rate/rblk");
        int index = m_devices["disk"].indexOf(device);
        if (index > -1) {
            QString key = QString("hddr%1").arg(index);
            m_map[_source] = key;
            m_formatter[key] = AWKeysAggregator::FormatterType::Integer;
        }
    } else if (_source.contains(hddwRegExp)) {
        // write speed
        QString device = _source;
        device.remove("/Rate/wblk");
        int index = m_devices["disk"].indexOf(device);
        if (index > -1) {
            QString key = QString("hddw%1").arg(index);
            m_map[_source] = key;
            m_formatter[key] = AWKeysAggregator::FormatterType::Integer;
        }
    } else if (_source == "gpu/load") {
        // gpu load
        m_map[_source] = "gpu";
        m_formatter["gpu"] = AWKeysAggregator::FormatterType::Float;
    } else if (_source == "gpu/temperature") {
        // gpu temperature
        m_map[_source] = "gputemp";
        m_formatter["gputemp"] = AWKeysAggregator::FormatterType::Temperature;
    } else if (_source.contains(mountFillRegExp)) {
        // fill level
        QString device = _source;
        device.remove("partitions").remove("/filllevel");
        int index = m_devices["mount"].indexOf(device);
        if (index > -1) {
            QString key = QString("hdd%1").arg(index);
            m_map[_source] = key;
            m_formatter[key] = AWKeysAggregator::FormatterType::Float;
            // additional keys
            m_formatter[QString("hddtotmb%1").arg(index)]
                = AWKeysAggregator::FormatterType::MemMBFormat;
            m_formatter[QString("hddtotgb%1").arg(index)]
                = AWKeysAggregator::FormatterType::MemGBFormat;
        }
    } else if (_source.contains(mountFreeRegExp)) {
        // free space
        QString device = _source;
        device.remove("partitions").remove("/freespace");
        int index = m_devices["mount"].indexOf(device);
        if (index > -1) {
            // mb
            QString key = QString("hddfreemb%1").arg(index);
            m_map[_source] = key;
            m_formatter[key] = AWKeysAggregator::FormatterType::MemMBFormat;
            // gb
            key = QString("hddfreegb%1").arg(index);
            m_map.insertMulti(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemGBFormat;
        }
    } else if (_source.contains(mountUsedRegExp)) {
        // used
        QString device = _source;
        device.remove("partitions").remove("/usedspace");
        int index = m_devices["mount"].indexOf(device);
        if (index > -1) {
            // mb
            QString key = QString("hddmb%1").arg(index);
            m_map[_source] = key;
            m_formatter[key] = AWKeysAggregator::FormatterType::MemMBFormat;
            // gb
            key = QString("hddgb%1").arg(index);
            m_map.insertMulti(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemGBFormat;
        }
    } else if (_source.startsWith("hdd/temperature")) {
        // hdd temperature
        QString device = _source;
        device.remove("hdd/temperature");
        int index = m_devices["hdd"].indexOf(device);
        if (index > -1) {
            QString key = QString("hddtemp%1").arg(index);
            m_map[_source] = key;
            m_formatter[key] = AWKeysAggregator::FormatterType::Temperature;
        }
    } else if (_source.startsWith("cpu/system/loadavg")) {
        // load average
        QString time = _source;
        time.remove("cpu/system/loadavg");
        QString key = QString("la%1").arg(time);
        m_map[_source] = key;
        m_formatter[key] = AWKeysAggregator::FormatterType::FloatTwoSymbols;
    } else if (_source == "mem/physical/application") {
        // app memory
        // mb
        m_map[_source] = "memmb";
        m_formatter["memmb"] = AWKeysAggregator::FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(_source, "memgb");
        m_formatter["memgb"] = AWKeysAggregator::FormatterType::MemGBFormat;
    } else if (_source == "mem/physical/free") {
        // free memory
        // mb
        m_map[_source] = "memfreemb";
        m_formatter["memfreemb"] = AWKeysAggregator::FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(_source, "memfreegb");
        m_formatter["memfreegb"] = AWKeysAggregator::FormatterType::MemGBFormat;
    } else if (_source == "mem/physical/used") {
        // used memory
        // mb
        m_map[_source] = "memusedmb";
        m_formatter["memusedmb"] = AWKeysAggregator::FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(_source, "memusedgb");
        m_formatter["memusedgb"] = AWKeysAggregator::FormatterType::MemGBFormat;
    } else if (_source == "network/current/name") {
        // network device
        m_map[_source] = "netdev";
        m_formatter["netdev"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source.startsWith("network/response")) {
        // network response
        QString key = _source;
        key.remove("network/");
        m_map[_source] = key;
        m_formatter[key] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source.contains(netRegExp)) {
        // network speed
        QString type = _source.contains("receiver") ? "down" : "up";
        int index = m_devices["net"].indexOf(_source.split('/')[2]);
        if (index > -1) {
            // kb
            QString key = QString("%1kb%2").arg(type).arg(index);
            m_map[_source] = key;
            m_formatter[key] = AWKeysAggregator::FormatterType::Integer;
            // smart
            key = QString("%1%2").arg(type).arg(index);
            m_map.insertMulti(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::NetSmartFormat;
            // units
            key = QString("%1units%2").arg(type).arg(index);
            m_map.insertMulti(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::NetSmartUnits;
        }
    } else if (_source.contains(netTotalRegExp)) {
        // network data total
        QString type = _source.contains("receiver") ? "down" : "up";
        int index = m_devices["net"].indexOf(_source.split('/')[2]);
        if (index > -1) {
            // kb
            QString key = QString("%1totkb%2").arg(type).arg(index);
            m_map[_source] = key;
            m_formatter[key] = AWKeysAggregator::FormatterType::Integer;
            // mb
            key = QString("%1tot%2").arg(type).arg(index);
            m_map.insertMulti(_source, key);
            m_formatter[key] = AWKeysAggregator::FormatterType::MemMBFormat;
        }
    } else if (_source.startsWith("upgrade")) {
        // package manager
        QString key = _source;
        key.remove("upgrade/");
        m_map[_source] = key;
        m_formatter[key] = AWKeysAggregator::FormatterType::IntegerThree;
    } else if (_source.startsWith("player")) {
        // player
        QString key = _source;
        key.remove("player/");
        m_map[_source] = key;
        m_formatter[key] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source == "ps/running/count") {
        // running processes count
        m_map[_source] = "pscount";
        m_formatter["pscount"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source == "ps/running/list") {
        // list of running processes
        m_map[_source] = "ps";
        m_formatter["ps"] = AWKeysAggregator::FormatterType::List;
    } else if (_source == "ps/total/count") {
        // total processes count
        m_map[_source] = "pstot";
        m_formatter["pstot"] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source.startsWith("quotes")) {
        // quotes
        QString key = _source;
        key.remove("quotes/");
        m_map[_source] = key;
        m_formatter[key] = AWKeysAggregator::FormatterType::Quotes;
    } else if (_source == "mem/swap/free") {
        // free swap
        // mb
        m_map[_source] = "swapfreemb";
        m_formatter["swapfreemb"]
            = AWKeysAggregator::FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(_source, "swapfreegb");
        m_formatter["swapfreegb"]
            = AWKeysAggregator::FormatterType::MemGBFormat;
    } else if (_source == "mem/swap/used") {
        // used swap
        // mb
        m_map[_source] = "swapmb";
        m_formatter["swapmb"] = AWKeysAggregator::FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(_source, "swapgb");
        m_formatter["swapgb"] = AWKeysAggregator::FormatterType::MemGBFormat;
    } else if (_source.startsWith("lmsensors/")) {
        // temperature
        int index = m_devices["temp"].indexOf(_source);
        // HACK on DE initialization there are no units key
        if (_units.isEmpty())
            return QStringList({QString("temp%1").arg(index)});
        if (index > -1) {
            QString key = QString("temp%1").arg(index);
            m_map[_source] = key;
            m_formatter[key]
                = _units == "°C" ? AWKeysAggregator::FormatterType::Temperature
                                 : AWKeysAggregator::FormatterType::Integer;
        }
    } else if (_source == "Local") {
        // time
        m_map[_source] = "time";
        m_formatter["time"] = AWKeysAggregator::FormatterType::Time;
        // custom time
        m_map.insertMulti(_source, "ctime");
        m_formatter["ctime"] = AWKeysAggregator::FormatterType::TimeCustom;
        // ISO time
        m_map.insertMulti(_source, "isotime");
        m_formatter["isotime"] = AWKeysAggregator::FormatterType::TimeISO;
        // long time
        m_map.insertMulti(_source, "longtime");
        m_formatter["longtime"] = AWKeysAggregator::FormatterType::TimeLong;
        // short time
        m_map.insertMulti(_source, "shorttime");
        m_formatter["shorttime"] = AWKeysAggregator::FormatterType::TimeShort;
        // timestamp
        m_map.insertMulti(_source, "tstime");
        m_formatter["tstime"] = AWKeysAggregator::FormatterType::Timestamp;
    } else if (_source == "system/uptime") {
        // uptime
        m_map[_source] = "uptime";
        m_formatter["uptime"] = AWKeysAggregator::FormatterType::Uptime;
        // custom uptime
        m_map.insertMulti(_source, "cuptime");
        m_formatter["cuptime"] = AWKeysAggregator::FormatterType::UptimeCustom;
    } else if (_source.startsWith("weather/temperature")) {
        // temperature
        QString key = _source;
        key.remove("weather/");
        m_map[_source] = key;
        m_formatter[key] = AWKeysAggregator::FormatterType::Temperature;
    } else if (_source.startsWith("weather/")) {
        // other weather
        QString key = _source;
        key.remove("weather/");
        m_map[_source] = key;
        m_formatter[key] = AWKeysAggregator::FormatterType::NoFormat;
    } else if (_source.startsWith("load/load")) {
        // load source
        QString key = _source;
        key.remove("load/");
        m_map[_source] = key;
        m_formatter[key] = AWKeysAggregator::FormatterType::Temperature;
    }

    QStringList foundKeys = keysFromSource(_source);

    // rewrite formatters for custom ones
    QStringList customFormattersKeys;
    if (m_customFormatters)
        customFormattersKeys = m_customFormatters->definedFormatters();
    qCInfo(LOG_AW) << "Looking for formatters" << foundKeys << "in"
                   << customFormattersKeys;
    for (auto &key : foundKeys) {
        if (!customFormattersKeys.contains(key))
            continue;
        m_formatter[key] = AWKeysAggregator::FormatterType::Custom;
    }

    // drop key from dictionary if no one user requested key required it
    qCInfo(LOG_AW) << "Looking for keys" << foundKeys << "in" << _keys;
    bool required
        = _keys.isEmpty() || std::any_of(foundKeys.cbegin(), foundKeys.cend(),
                                         [&_keys](const QString &key) {
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
