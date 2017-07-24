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

#include "awkeysaggregator.h"

#include <KI18n/KLocalizedString>

#include <QDateTime>
#include <QLocale>
#include <QRegExp>

#include "awdebug.h"
#include "awformatterhelper.h"


AWKeysAggregator::AWKeysAggregator(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_customFormatters = new AWFormatterHelper(nullptr);

    // sort time keys
    m_timeKeys.sort();
    std::reverse(m_timeKeys.begin(), m_timeKeys.end());

    // default formatters
    // memory
    m_formatter["mem"] = FormatterType::Float;
    m_formatter["memtotmb"] = FormatterType::MemMBFormat;
    m_formatter["memtotgb"] = FormatterType::MemGBFormat;
    // network
    m_formatter["down"] = FormatterType::NetSmartFormat;
    m_formatter["downkb"] = FormatterType::Integer;
    m_formatter["downtot"] = FormatterType::MemMBFormat;
    m_formatter["downtotkb"] = FormatterType::Integer;
    m_formatter["downunits"] = FormatterType::NetSmartUnits;
    m_formatter["up"] = FormatterType::NetSmartFormat;
    m_formatter["upkb"] = FormatterType::Integer;
    m_formatter["uptot"] = FormatterType::MemMBFormat;
    m_formatter["uptotkb"] = FormatterType::Integer;
    m_formatter["upunits"] = FormatterType::NetSmartUnits;
    // swap
    m_formatter["swap"] = FormatterType::Float;
    m_formatter["swaptotmb"] = FormatterType::MemMBFormat;
    m_formatter["swaptotgb"] = FormatterType::MemGBFormat;
}


AWKeysAggregator::~AWKeysAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


void AWKeysAggregator::initFormatters()
{
    m_customFormatters->initItems();
}


QString AWKeysAggregator::formatter(const QVariant &_data,
                                    const QString &_key) const
{
    qCDebug(LOG_AW) << "Data" << _data << "for key" << _key;

    QString output;
    QLocale loc = m_translate ? QLocale::system() : QLocale::c();
    // case block
    switch (m_formatter[_key]) {
    case FormatterType::Float:
        output = QString("%1").arg(_data.toFloat(), 5, 'f', 1);
        break;
    case FormatterType::FloatTwoSymbols:
        output = QString("%1").arg(_data.toFloat(), 5, 'f', 2);
        break;
    case FormatterType::Integer:
        output = QString("%1").arg(_data.toFloat(), 4, 'f', 0);
        break;
    case FormatterType::IntegerFive:
        output = QString("%1").arg(_data.toFloat(), 5, 'f', 0);
        break;
    case FormatterType::IntegerThree:
        output = QString("%1").arg(_data.toFloat(), 3, 'f', 0);
        break;
    case FormatterType::List:
        output = _data.toStringList().join(',');
        break;
    case FormatterType::ACFormat:
        output = _data.toBool() ? m_acOnline : m_acOffline;
        break;
    case FormatterType::MemGBFormat:
        output
            = QString("%1").arg(_data.toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
        break;
    case FormatterType::MemMBFormat:
        output = QString("%1").arg(_data.toFloat() / 1024.0, 5, 'f', 0);
        break;
    case FormatterType::NetSmartFormat:
        output = [](const float value) {
            if (value > 1024.0)
                return QString("%1").arg(value / 1024.0, 4, 'f', 1);
            else
                return QString("%1").arg(value, 4, 'f', 0);
        }(_data.toFloat());
        break;
    case FormatterType::NetSmartUnits:
        if (_data.toFloat() > 1024.0)
            output = m_translate ? i18n("MB/s") : "MB/s";
        else
            output = m_translate ? i18n("KB/s") : "KB/s";
        break;
    case FormatterType::Quotes:
        // first cast
        output = QString("%1").arg(_data.toDouble(), 0, 'f');
        output = output.rightJustified(8, QLatin1Char(' '), true);
        break;
    case FormatterType::Temperature:
        output = QString("%1").arg(temperature(_data.toFloat()), 5, 'f', 1);
        break;
    case FormatterType::Time:
        output = _data.toDateTime().toString();
        break;
    case FormatterType::TimeCustom:
        output = m_customTime;
        [&output, loc, this](const QDateTime dt) {
            for (auto &key : m_timeKeys)
                output.replace(QString("$%1").arg(key), loc.toString(dt, key));
        }(_data.toDateTime());
        break;
    case FormatterType::TimeISO:
        output = _data.toDateTime().toString(Qt::ISODate);
        break;
    case FormatterType::TimeLong:
        output = loc.toString(_data.toDateTime(), QLocale::LongFormat);
        break;
    case FormatterType::TimeShort:
        output = loc.toString(_data.toDateTime(), QLocale::ShortFormat);
        break;
    case FormatterType::Timestamp:
        output = QString("%1").arg(
            _data.toDateTime().toMSecsSinceEpoch() / 1000.0, 10, 'f', 0);
        break;
    case FormatterType::Uptime:
    case FormatterType::UptimeCustom:
        output =
            [](QString source, const int uptime) {
                int seconds = uptime - uptime % 60;
                int minutes = seconds / 60 % 60;
                int hours = ((seconds / 60) - minutes) / 60 % 24;
                int days = (((seconds / 60) - minutes) / 60 - hours) / 24;
                source.replace("$dd",
                               QString("%1").arg(days, 3, 10, QChar('0')));
                source.replace("$d", QString("%1").arg(days));
                source.replace("$hh",
                               QString("%1").arg(hours, 2, 10, QChar('0')));
                source.replace("$h", QString("%1").arg(hours));
                source.replace("$mm",
                               QString("%1").arg(minutes, 2, 10, QChar('0')));
                source.replace("$m", QString("%1").arg(minutes));
                return source;
            }(m_formatter[_key] == FormatterType::Uptime ? "$ddd$hhh$mmm"
                                                         : m_customUptime,
              static_cast<int>(_data.toFloat()));
        break;
    case FormatterType::NoFormat:
        output = _data.toString();
        break;
    case FormatterType::Custom:
        if (m_customFormatters)
            output = m_customFormatters->convert(_data, _key);
        break;
    }

    return output;
}


QStringList AWKeysAggregator::keysFromSource(const QString &_source) const
{
    qCDebug(LOG_AW) << "Search for source" << _source;

    return m_map.values(_source);
}


void AWKeysAggregator::setAcOffline(const QString &_inactive)
{
    qCDebug(LOG_AW) << "Inactive AC string" << _inactive;

    m_acOffline = _inactive;
}


void AWKeysAggregator::setAcOnline(const QString &_active)
{
    qCDebug(LOG_AW) << "Active AC string" << _active;

    m_acOnline = _active;
}


void AWKeysAggregator::setCustomTime(const QString &_customTime)
{
    qCDebug(LOG_AW) << "Format" << _customTime;

    m_customTime = _customTime;
}


void AWKeysAggregator::setCustomUptime(const QString &_customUptime)
{
    qCDebug(LOG_AW) << "Format" << _customUptime;

    m_customUptime = _customUptime;
}


void AWKeysAggregator::setDevices(const QHash<QString, QStringList> &_devices)
{
    qCDebug(LOG_AW) << "Devices" << _devices;

    m_devices = _devices;
}


void AWKeysAggregator::setTempUnits(const QString &_units)
{
    qCDebug(LOG_AW) << "Units" << _units;

    m_tempUnits = _units;
}


void AWKeysAggregator::setTranslate(const bool _translate)
{
    qCDebug(LOG_AW) << "Translate" << _translate;

    m_translate = _translate;
}


// HACK units required to define should the value be calculated as temperature
// or fan data
QStringList AWKeysAggregator::registerSource(const QString &_source,
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
        m_formatter["ac"] = FormatterType::ACFormat;
    } else if (_source.startsWith("battery/")) {
        // battery stats
        QString key = _source;
        key.remove("battery/");
        m_map[_source] = key;
        m_formatter[key] = FormatterType::IntegerThree;
    } else if (_source == "cpu/system/TotalLoad") {
        // cpu
        m_map[_source] = "cpu";
        m_formatter["cpu"] = FormatterType::Float;
    } else if (_source.contains(cpuRegExp)) {
        // cpus
        QString key = _source;
        key.remove("cpu/").remove("/TotalLoad");
        m_map[_source] = key;
        m_formatter[key] = FormatterType::Float;
    } else if (_source == "cpu/system/AverageClock") {
        // cpucl
        m_map[_source] = "cpucl";
        m_formatter["cpucl"] = FormatterType::Integer;
    } else if (_source.contains(cpuclRegExp)) {
        // cpucls
        QString key = _source;
        key.remove("cpu/cpu").remove("/clock");
        key = QString("cpucl%1").arg(key);
        m_map[_source] = key;
        m_formatter[key] = FormatterType::Integer;
    } else if (_source.startsWith("custom")) {
        // custom
        QString key = _source;
        key.remove("custom/");
        m_map[_source] = key;
        m_formatter[key] = FormatterType::NoFormat;
    } else if (_source == "desktop/current/name") {
        // current desktop name
        m_map[_source] = "desktop";
        m_formatter["desktop"] = FormatterType::NoFormat;
    } else if (_source == "desktop/current/number") {
        // current desktop number
        m_map[_source] = "ndesktop";
        m_formatter["ndesktop"] = FormatterType::NoFormat;
    } else if (_source == "desktop/total/number") {
        // desktop count
        m_map[_source] = "tdesktops";
        m_formatter["tdesktops"] = FormatterType::NoFormat;
    } else if (_source.contains(hddrRegExp)) {
        // read speed
        QString device = _source;
        device.remove("/Rate/rblk");
        int index = m_devices["disk"].indexOf(device);
        if (index > -1) {
            QString key = QString("hddr%1").arg(index);
            m_map[_source] = key;
            m_formatter[key] = FormatterType::Integer;
        }
    } else if (_source.contains(hddwRegExp)) {
        // write speed
        QString device = _source;
        device.remove("/Rate/wblk");
        int index = m_devices["disk"].indexOf(device);
        if (index > -1) {
            QString key = QString("hddw%1").arg(index);
            m_map[_source] = key;
            m_formatter[key] = FormatterType::Integer;
        }
    } else if (_source == "gpu/load") {
        // gpu load
        m_map[_source] = "gpu";
        m_formatter["gpu"] = FormatterType::Float;
    } else if (_source == "gpu/temperature") {
        // gpu temperature
        m_map[_source] = "gputemp";
        m_formatter["gputemp"] = FormatterType::Temperature;
    } else if (_source.contains(mountFillRegExp)) {
        // fill level
        QString device = _source;
        device.remove("partitions").remove("/filllevel");
        int index = m_devices["mount"].indexOf(device);
        if (index > -1) {
            QString key = QString("hdd%1").arg(index);
            m_map[_source] = key;
            m_formatter[key] = FormatterType::Float;
            // additional keys
            m_formatter[QString("hddtotmb%1").arg(index)]
                = FormatterType::MemMBFormat;
            m_formatter[QString("hddtotgb%1").arg(index)]
                = FormatterType::MemGBFormat;
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
            m_formatter[key] = FormatterType::MemMBFormat;
            // gb
            key = QString("hddfreegb%1").arg(index);
            m_map.insertMulti(_source, key);
            m_formatter[key] = FormatterType::MemGBFormat;
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
            m_formatter[key] = FormatterType::MemMBFormat;
            // gb
            key = QString("hddgb%1").arg(index);
            m_map.insertMulti(_source, key);
            m_formatter[key] = FormatterType::MemGBFormat;
        }
    } else if (_source.startsWith("hdd/temperature")) {
        // hdd temperature
        QString device = _source;
        device.remove("hdd/temperature");
        int index = m_devices["hdd"].indexOf(device);
        if (index > -1) {
            QString key = QString("hddtemp%1").arg(index);
            m_map[_source] = key;
            m_formatter[key] = FormatterType::Temperature;
        }
    } else if (_source.startsWith("cpu/system/loadavg")) {
        // load average
        QString time = _source;
        time.remove("cpu/system/loadavg");
        QString key = QString("la%1").arg(time);
        m_map[_source] = key;
        m_formatter[key] = FormatterType::FloatTwoSymbols;
    } else if (_source == "mem/physical/application") {
        // app memory
        // mb
        m_map[_source] = "memmb";
        m_formatter["memmb"] = FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(_source, "memgb");
        m_formatter["memgb"] = FormatterType::MemGBFormat;
    } else if (_source == "mem/physical/free") {
        // free memory
        // mb
        m_map[_source] = "memfreemb";
        m_formatter["memfreemb"] = FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(_source, "memfreegb");
        m_formatter["memfreegb"] = FormatterType::MemGBFormat;
    } else if (_source == "mem/physical/used") {
        // used memory
        // mb
        m_map[_source] = "memusedmb";
        m_formatter["memusedmb"] = FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(_source, "memusedgb");
        m_formatter["memusedgb"] = FormatterType::MemGBFormat;
    } else if (_source == "network/current/name") {
        // network device
        m_map[_source] = "netdev";
        m_formatter["netdev"] = FormatterType::NoFormat;
    } else if (_source.startsWith("network/response")) {
        // network response
        QString key = _source;
        key.remove("network/");
        m_map[_source] = key;
        m_formatter[key] = FormatterType::NoFormat;
    } else if (_source.contains(netRegExp)) {
        // network speed
        QString type = _source.contains("receiver") ? "down" : "up";
        int index = m_devices["net"].indexOf(_source.split('/')[2]);
        if (index > -1) {
            // kb
            QString key = QString("%1kb%2").arg(type).arg(index);
            m_map[_source] = key;
            m_formatter[key] = FormatterType::Integer;
            // smart
            key = QString("%1%2").arg(type).arg(index);
            m_map.insertMulti(_source, key);
            m_formatter[key] = FormatterType::NetSmartFormat;
            // units
            key = QString("%1units%2").arg(type).arg(index);
            m_map.insertMulti(_source, key);
            m_formatter[key] = FormatterType::NetSmartUnits;
        }
    } else if (_source.contains(netTotalRegExp)) {
        // network data total
        QString type = _source.contains("receiver") ? "down" : "up";
        int index = m_devices["net"].indexOf(_source.split('/')[2]);
        if (index > -1) {
            // kb
            QString key = QString("%1totkb%2").arg(type).arg(index);
            m_map[_source] = key;
            m_formatter[key] = FormatterType::Integer;
            // mb
            key = QString("%1tot%2").arg(type).arg(index);
            m_map.insertMulti(_source, key);
            m_formatter[key] = FormatterType::MemMBFormat;
        }
    } else if (_source.startsWith("upgrade")) {
        // package manager
        QString key = _source;
        key.remove("upgrade/");
        m_map[_source] = key;
        m_formatter[key] = FormatterType::IntegerThree;
    } else if (_source.startsWith("player")) {
        // player
        QString key = _source;
        key.remove("player/");
        m_map[_source] = key;
        m_formatter[key] = FormatterType::NoFormat;
    } else if (_source == "ps/running/count") {
        // running processes count
        m_map[_source] = "pscount";
        m_formatter["pscount"] = FormatterType::NoFormat;
    } else if (_source == "ps/running/list") {
        // list of running processes
        m_map[_source] = "ps";
        m_formatter["ps"] = FormatterType::List;
    } else if (_source == "ps/total/count") {
        // total processes count
        m_map[_source] = "pstot";
        m_formatter["pstot"] = FormatterType::NoFormat;
    } else if (_source.startsWith("quotes")) {
        // quotes
        QString key = _source;
        key.remove("quotes/");
        m_map[_source] = key;
        m_formatter[key] = FormatterType::Quotes;
    } else if (_source == "mem/swap/free") {
        // free swap
        // mb
        m_map[_source] = "swapfreemb";
        m_formatter["swapfreemb"] = FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(_source, "swapfreegb");
        m_formatter["swapfreegb"] = FormatterType::MemGBFormat;
    } else if (_source == "mem/swap/used") {
        // used swap
        // mb
        m_map[_source] = "swapmb";
        m_formatter["swapmb"] = FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(_source, "swapgb");
        m_formatter["swapgb"] = FormatterType::MemGBFormat;
    } else if (_source.startsWith("lmsensors/")) {
        // temperature
        int index = m_devices["temp"].indexOf(_source);
        // HACK on DE initialization there are no units key
        if (_units.isEmpty())
            return QStringList({QString("temp%1").arg(index)});
        if (index > -1) {
            QString key = QString("temp%1").arg(index);
            m_map[_source] = key;
            m_formatter[key] = _units == "°C" ? FormatterType::Temperature
                                              : FormatterType::Integer;
        }
    } else if (_source == "Local") {
        // time
        m_map[_source] = "time";
        m_formatter["time"] = FormatterType::Time;
        // custom time
        m_map.insertMulti(_source, "ctime");
        m_formatter["ctime"] = FormatterType::TimeCustom;
        // ISO time
        m_map.insertMulti(_source, "isotime");
        m_formatter["isotime"] = FormatterType::TimeISO;
        // long time
        m_map.insertMulti(_source, "longtime");
        m_formatter["longtime"] = FormatterType::TimeLong;
        // short time
        m_map.insertMulti(_source, "shorttime");
        m_formatter["shorttime"] = FormatterType::TimeShort;
        // timestamp
        m_map.insertMulti(_source, "tstime");
        m_formatter["tstime"] = FormatterType::Timestamp;
    } else if (_source == "system/uptime") {
        // uptime
        m_map[_source] = "uptime";
        m_formatter["uptime"] = FormatterType::Uptime;
        // custom uptime
        m_map.insertMulti(_source, "cuptime");
        m_formatter["cuptime"] = FormatterType::UptimeCustom;
    } else if (_source.startsWith("weather/temperature")) {
        // temperature
        QString key = _source;
        key.remove("weather/");
        m_map[_source] = key;
        m_formatter[key] = FormatterType::Temperature;
    } else if (_source.startsWith("weather/")) {
        // other weather
        QString key = _source;
        key.remove("weather/");
        m_map[_source] = key;
        m_formatter[key] = FormatterType::NoFormat;
    } else if (_source.startsWith("load/load")) {
        // load source
        QString key = _source;
        key.remove("load/");
        m_map[_source] = key;
        m_formatter[key] = FormatterType::Temperature;
    }

    QStringList foundKeys = keysFromSource(_source);

    // rewrite formatters for custom ones
    QStringList customFormattersKeys;
    if (m_customFormatters)
        customFormattersKeys = m_customFormatters->definedFormatters();
    qCInfo(LOG_AW) << "Looking for fprmatters" << foundKeys << "in"
                   << customFormattersKeys;
    for (auto &key : foundKeys) {
        if (!customFormattersKeys.contains(key))
            continue;
        m_formatter[key] = FormatterType::Custom;
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


float AWKeysAggregator::temperature(const float temp) const
{
    qCDebug(LOG_AW) << "Temperature value" << temp;

    float converted = temp;
    if (m_tempUnits == "Celsius") {
    } else if (m_tempUnits == "Fahrenheit") {
        converted = temp * 9.0f / 5.0f + 32.0f;
    } else if (m_tempUnits == "Kelvin") {
        converted = temp + 273.15f;
    } else if (m_tempUnits == "Reaumur") {
        converted = temp * 0.8f;
    } else if (m_tempUnits == "cm^-1") {
        converted = (temp + 273.15f) * 0.695f;
    } else if (m_tempUnits == "kJ/mol") {
        converted = (temp + 273.15f) * 8.31f;
    } else if (m_tempUnits == "kcal/mol") {
        converted = (temp + 273.15f) * 1.98f;
    } else {
        qCWarning(LOG_AW) << "Invalid units" << m_tempUnits;
    }

    return converted;
}
