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


AWKeysAggregator::AWKeysAggregator(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    // default formatters
    // memory
    m_formatter[QString("mem")] = FormatterType::Float;
    m_formatter[QString("memtotmb")] = FormatterType::MemMBFormat;
    m_formatter[QString("memtotgb")] = FormatterType::MemGBFormat;
    // network
    m_formatter[QString("down")] = FormatterType::NetSmartFormat;
    m_formatter[QString("downkb")] = FormatterType::Integer;
    m_formatter[QString("downunits")] = FormatterType::NetSmartUnits;
    m_formatter[QString("up")] = FormatterType::NetSmartFormat;
    m_formatter[QString("upkb")] = FormatterType::Integer;
    m_formatter[QString("upunits")] = FormatterType::NetSmartUnits;
    // swap
    m_formatter[QString("swap")] = FormatterType::Float;
    m_formatter[QString("swaptotmb")] = FormatterType::MemMBFormat;
    m_formatter[QString("swaptotgb")] = FormatterType::MemGBFormat;

    m_customFormatters = new AWFormatterHelper(this);
}


AWKeysAggregator::~AWKeysAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    delete m_customFormatters;
}


QString AWKeysAggregator::formatter(const QVariant &data,
                                    const QString &key) const
{
    qCDebug(LOG_AW) << "Data" << data << "for key" << key;

    QString output;
    QLocale loc = m_translate ? QLocale::system() : QLocale::c();
    // case block
    switch (m_formatter[key]) {
    case FormatterType::Float:
        output = QString("%1").arg(data.toFloat(), 5, 'f', 1);
        break;
    case FormatterType::FloatTwoSymbols:
        output = QString("%1").arg(data.toFloat(), 5, 'f', 2);
        break;
    case FormatterType::Integer:
        output = QString("%1").arg(data.toFloat(), 4, 'f', 0);
        break;
    case FormatterType::IntegerFive:
        output = QString("%1").arg(data.toFloat(), 5, 'f', 0);
        break;
    case FormatterType::IntegerThree:
        output = QString("%1").arg(data.toFloat(), 3, 'f', 0);
        break;
    case FormatterType::List:
        output = data.toStringList().join(QChar(','));
        break;
    case FormatterType::ACFormat:
        output = data.toBool() ? m_acOnline : m_acOffline;
        break;
    case FormatterType::MemGBFormat:
        output
            = QString("%1").arg(data.toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
        break;
    case FormatterType::MemMBFormat:
        output = QString("%1").arg(data.toFloat() / 1024.0, 5, 'f', 0);
        break;
    case FormatterType::NetSmartFormat:
        output = [](const float value) {
            if (value > 1024.0)
                return QString("%1").arg(value / 1024.0, 4, 'f', 1);
            else
                return QString("%1").arg(value, 4, 'f', 0);
        }(data.toFloat());
        break;
    case FormatterType::NetSmartUnits:
        if (data.toFloat() > 1024.0)
            output = m_translate ? i18n("MB/s") : QString("MB/s");
        else
            output = m_translate ? i18n("KB/s") : QString("KB/s");
        break;
    case FormatterType::Quotes:
        // first cast
        output = QString("%1").arg(data.toDouble(), 0, 'f');
        output = output.rightJustified(8, QLatin1Char(' '), true);
        break;
    case FormatterType::Temperature:
        output = QString("%1").arg(temperature(data.toFloat()), 5, 'f', 1);
        break;
    case FormatterType::Time:
        output = data.toDateTime().toString();
        break;
    case FormatterType::TimeCustom:
        output = m_customTime;
        [&output, loc, this](const QDateTime dt) {
            for (auto key : timeKeys)
                output.replace(QString("$%1").arg(key), loc.toString(dt, key));
        }(data.toDateTime());
        break;
    case FormatterType::TimeISO:
        output = data.toDateTime().toString(Qt::ISODate);
        break;
    case FormatterType::TimeLong:
        output = loc.toString(data.toDateTime(), QLocale::LongFormat);
        break;
    case FormatterType::TimeShort:
        output = loc.toString(data.toDateTime(), QLocale::ShortFormat);
        break;
    case FormatterType::Uptime:
    case FormatterType::UptimeCustom:
        output =
            [](QString source, const int uptime) {
                int seconds = uptime - uptime % 60;
                int minutes = seconds / 60 % 60;
                int hours = ((seconds / 60) - minutes) / 60 % 24;
                int days = (((seconds / 60) - minutes) / 60 - hours) / 24;
                source.replace(QString("$dd"),
                               QString("%1").arg(days, 3, 10, QChar('0')));
                source.replace(QString("$d"), QString("%1").arg(days));
                source.replace(QString("$hh"),
                               QString("%1").arg(hours, 2, 10, QChar('0')));
                source.replace(QString("$h"), QString("%1").arg(hours));
                source.replace(QString("$mm"),
                               QString("%1").arg(minutes, 2, 10, QChar('0')));
                source.replace(QString("$m"), QString("%1").arg(minutes));
                return source;
            }(m_formatter[key] == FormatterType::Uptime
                  ? QString("$ddd$hhh$mmm")
                  : m_customUptime,
              static_cast<int>(data.toFloat()));
        break;
    case FormatterType::NoFormat:
        output = data.toString();
        break;
    case FormatterType::Custom:
        output = m_customFormatters->convert(data, key);
        break;
    }

    return output;
}


QStringList AWKeysAggregator::keysFromSource(const QString &source) const
{
    qCDebug(LOG_AW) << "Search for source" << source;

    return m_map.values(source);
}


void AWKeysAggregator::setAcOffline(const QString inactive)
{
    qCDebug(LOG_AW) << "Inactive AC string" << inactive;

    m_acOffline = inactive;
}


void AWKeysAggregator::setAcOnline(const QString active)
{
    qCDebug(LOG_AW) << "Active AC string" << active;

    m_acOnline = active;
}


void AWKeysAggregator::setCustomTime(const QString customTime)
{
    qCDebug(LOG_AW) << "Format" << customTime;

    m_customTime = customTime;
}


void AWKeysAggregator::setCustomUptime(const QString customUptime)
{
    qCDebug(LOG_AW) << "Format" << customUptime;

    m_customUptime = customUptime;
}


void AWKeysAggregator::setDevices(const QHash<QString, QStringList> devices)
{
    qCDebug(LOG_AW) << "Devices" << devices;

    m_devices = devices;
}


void AWKeysAggregator::setTempUnits(const QString units)
{
    qCDebug(LOG_AW) << "Units" << units;

    m_tempUnits = units;
}


void AWKeysAggregator::setTranslate(const bool translate)
{
    qCDebug(LOG_AW) << "Translate" << translate;

    m_translate = translate;
}


// HACK units required to define should the value be calculated as temperature
// or fan data
QStringList AWKeysAggregator::registerSource(const QString &source,
                                             const QString &units,
                                             const QStringList &keys)
{
    qCDebug(LOG_AW) << "Source" << source << "with units" << units;

    // regular expressions
    QRegExp cpuRegExp = QRegExp(QString("cpu/cpu.*/TotalLoad"));
    QRegExp cpuclRegExp = QRegExp(QString("cpu/cpu.*/clock"));
    QRegExp hddrRegExp = QRegExp(QString("disk/.*/Rate/rblk"));
    QRegExp hddwRegExp = QRegExp(QString("disk/.*/Rate/wblk"));
    QRegExp mountFillRegExp = QRegExp(QString("partitions/.*/filllevel"));
    QRegExp mountFreeRegExp = QRegExp(QString("partitions/.*/freespace"));
    QRegExp mountUsedRegExp = QRegExp(QString("partitions/.*/usedspace"));
    QRegExp netRegExp = QRegExp(
        QString("network/interfaces/.*/(receiver|transmitter)/data$"));
    QRegExp netTotalRegExp = QRegExp(
        QString("network/interfaces/.*/(receiver|transmitter)/dataTotal$"));

    if (source == QString("battery/ac")) {
        // AC
        m_map[source] = QString("ac");
        m_formatter[QString("ac")] = FormatterType::ACFormat;
    } else if (source.startsWith(QString("battery/"))) {
        // battery stats
        QString key = source;
        key.remove(QString("battery/"));
        m_map[source] = key;
        m_formatter[key] = FormatterType::IntegerThree;
    } else if (source == QString("cpu/system/TotalLoad")) {
        // cpu
        m_map[source] = QString("cpu");
        m_formatter[QString("cpu")] = FormatterType::Float;
    } else if (source.contains(cpuRegExp)) {
        // cpus
        QString key = source;
        key.remove(QString("cpu/")).remove(QString("/TotalLoad"));
        m_map[source] = key;
        m_formatter[key] = FormatterType::Float;
    } else if (source == QString("cpu/system/AverageClock")) {
        // cpucl
        m_map[source] = QString("cpucl");
        m_formatter[QString("cpucl")] = FormatterType::Integer;
    } else if (source.contains(cpuclRegExp)) {
        // cpucls
        QString key = source;
        key.remove(QString("cpu/cpu")).remove(QString("/clock"));
        key = QString("cpucl%1").arg(key);
        m_map[source] = key;
        m_formatter[key] = FormatterType::Integer;
    } else if (source.startsWith(QString("custom"))) {
        // custom
        QString key = source;
        key.remove(QString("custom/"));
        m_map[source] = key;
        m_formatter[key] = FormatterType::NoFormat;
    } else if (source == QString("desktop/current/name")) {
        // current desktop name
        m_map[source] = QString("desktop");
        m_formatter[QString("desktop")] = FormatterType::NoFormat;
    } else if (source == QString("desktop/current/number")) {
        // current desktop number
        m_map[source] = QString("ndesktop");
        m_formatter[QString("ndesktop")] = FormatterType::NoFormat;
    } else if (source == QString("desktop/total/number")) {
        // desktop count
        m_map[source] = QString("tdesktops");
        m_formatter[QString("tdesktops")] = FormatterType::NoFormat;
    } else if (source.contains(hddrRegExp)) {
        // read speed
        QString device = source;
        device.remove(QString("/Rate/rblk"));
        int index = m_devices[QString("disk")].indexOf(device);
        if (index > -1) {
            QString key = QString("hddr%1").arg(index);
            m_map[source] = key;
            m_formatter[key] = FormatterType::Integer;
        }
    } else if (source.contains(hddwRegExp)) {
        // write speed
        QString device = source;
        device.remove(QString("/Rate/wblk"));
        int index = m_devices[QString("disk")].indexOf(device);
        if (index > -1) {
            QString key = QString("hddw%1").arg(index);
            m_map[source] = key;
            m_formatter[key] = FormatterType::Integer;
        }
    } else if (source == QString("gpu/load")) {
        // gpu load
        m_map[source] = QString("gpu");
        m_formatter[QString("gpu")] = FormatterType::Float;
    } else if (source == QString("gpu/temperature")) {
        // gpu temperature
        m_map[source] = QString("gputemp");
        m_formatter[QString("gputemp")] = FormatterType::Temperature;
    } else if (source.contains(mountFillRegExp)) {
        // fill level
        QString device = source;
        device.remove(QString("partitions")).remove(QString("/filllevel"));
        int index = m_devices[QString("mount")].indexOf(device);
        if (index > -1) {
            QString key = QString("hdd%1").arg(index);
            m_map[source] = key;
            m_formatter[key] = FormatterType::Float;
            // additional keys
            m_formatter[QString("hddtotmb%1").arg(index)]
                = FormatterType::MemMBFormat;
            m_formatter[QString("hddtotgb%1").arg(index)]
                = FormatterType::MemGBFormat;
        }
    } else if (source.contains(mountFreeRegExp)) {
        // free space
        QString device = source;
        device.remove(QString("partitions")).remove(QString("/freespace"));
        int index = m_devices[QString("mount")].indexOf(device);
        if (index > -1) {
            // mb
            QString key = QString("hddfreemb%1").arg(index);
            m_map[source] = key;
            m_formatter[key] = FormatterType::MemMBFormat;
            // gb
            key = QString("hddfreegb%1").arg(index);
            m_map.insertMulti(source, key);
            m_formatter[key] = FormatterType::MemGBFormat;
        }
    } else if (source.contains(mountUsedRegExp)) {
        // used
        QString device = source;
        device.remove(QString("partitions")).remove(QString("/usedspace"));
        int index = m_devices[QString("mount")].indexOf(device);
        if (index > -1) {
            // mb
            QString key = QString("hddmb%1").arg(index);
            m_map[source] = key;
            m_formatter[key] = FormatterType::MemMBFormat;
            // gb
            key = QString("hddgb%1").arg(index);
            m_map.insertMulti(source, key);
            m_formatter[key] = FormatterType::MemGBFormat;
        }
    } else if (source.startsWith(QString("hdd/temperature"))) {
        // hdd temperature
        QString device = source;
        device.remove(QString("hdd/temperature"));
        int index = m_devices[QString("hdd")].indexOf(device);
        if (index > -1) {
            QString key = QString("hddtemp%1").arg(index);
            m_map[source] = key;
            m_formatter[key] = FormatterType::Temperature;
        }
    } else if (source.startsWith(QString("cpu/system/loadavg"))) {
        // load average
        QString time = source;
        time.remove(QString("cpu/system/loadavg"));
        QString key = QString("la%1").arg(time);
        m_map[source] = key;
        m_formatter[key] = FormatterType::FloatTwoSymbols;
    } else if (source == QString("mem/physical/application")) {
        // app memory
        // mb
        m_map[source] = QString("memmb");
        m_formatter[QString("memmb")] = FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("memgb"));
        m_formatter[QString("memgb")] = FormatterType::MemGBFormat;
    } else if (source == QString("mem/physical/free")) {
        // free memory
        // mb
        m_map[source] = QString("memfreemb");
        m_formatter[QString("memfreemb")] = FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("memfreegb"));
        m_formatter[QString("memfreegb")] = FormatterType::MemGBFormat;
    } else if (source == QString("mem/physical/used")) {
        // used memory
        // mb
        m_map[source] = QString("memusedmb");
        m_formatter[QString("memusedmb")] = FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("memusedgb"));
        m_formatter[QString("memusedgb")] = FormatterType::MemGBFormat;
    } else if (source == QString("network/current/name")) {
        // network device
        m_map[source] = QString("netdev");
        m_formatter[QString("netdev")] = FormatterType::NoFormat;
    } else if (source.contains(netRegExp)) {
        // network speed
        QString type = source.contains(QString("receiver")) ? QString("down")
                                                            : QString("up");
        int index
            = m_devices[QString("net")].indexOf(source.split(QChar('/'))[2]);
        if (index > -1) {
            // kb
            QString key = QString("%1kb%2").arg(type).arg(index);
            m_map[source] = key;
            m_formatter[key] = FormatterType::Integer;
            // smart
            key = QString("%1%2").arg(type).arg(index);
            m_map.insertMulti(source, key);
            m_formatter[key] = FormatterType::NetSmartFormat;
            // units
            key = QString("%1units%2").arg(type).arg(index);
            m_map.insertMulti(source, key);
            m_formatter[key] = FormatterType::NetSmartUnits;
        }
    } else if (source.contains(netTotalRegExp)) {
        // network data total
        QString type = source.contains(QString("receiver")) ? QString("down")
                                                            : QString("up");
        int index
            = m_devices[QString("net")].indexOf(source.split(QChar('/'))[2]);
        if (index > -1) {
            // kb
            QString key = QString("%1totalkb%2").arg(type).arg(index);
            m_map[source] = key;
            m_formatter[key] = FormatterType::Integer;
            // mb
            key = QString("%1total%2").arg(type).arg(index);
            m_map.insertMulti(source, key);
            m_formatter[key] = FormatterType::MemMBFormat;
        }
    } else if (source.startsWith(QString("upgrade"))) {
        // package manager
        QString key = source;
        key.remove(QString("upgrade/"));
        m_map[source] = key;
        m_formatter[key] = FormatterType::IntegerThree;
    } else if (source.startsWith(QString("player"))) {
        // player
        QString key = source;
        key.remove(QString("player/"));
        m_map[source] = key;
        m_formatter[key] = FormatterType::NoFormat;
    } else if (source == QString("ps/running/count")) {
        // running processes count
        m_map[source] = QString("pscount");
        m_formatter[QString("pscount")] = FormatterType::NoFormat;
    } else if (source == QString("ps/running/list")) {
        // list of running processes
        m_map[source] = QString("ps");
        m_formatter[QString("ps")] = FormatterType::List;
    } else if (source == QString("ps/total/count")) {
        // total processes count
        m_map[source] = QString("pstotal");
        m_formatter[QString("pstotal")] = FormatterType::NoFormat;
    } else if (source.startsWith(QString("quotes"))) {
        // quotes
        QString key = source;
        key.remove(QString("quotes/"));
        m_map[source] = key;
        m_formatter[key] = FormatterType::Quotes;
    } else if (source == QString("mem/swap/free")) {
        // free swap
        // mb
        m_map[source] = QString("swapfreemb");
        m_formatter[QString("swapfreemb")] = FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("swapfreegb"));
        m_formatter[QString("swapfreegb")] = FormatterType::MemGBFormat;
    } else if (source == QString("mem/swap/used")) {
        // used swap
        // mb
        m_map[source] = QString("swapmb");
        m_formatter[QString("swapmb")] = FormatterType::MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("swapgb"));
        m_formatter[QString("swapgb")] = FormatterType::MemGBFormat;
    } else if (source.startsWith(QString("lmsensors/"))) {
        // temperature
        int index = m_devices[QString("temp")].indexOf(source);
        // FIXME on DE initialization there are no units key
        if (units.isEmpty())
            return QStringList() << QString("temp%1").arg(index);
        if (index > -1) {
            QString key = QString("temp%1").arg(index);
            m_map[source] = key;
            m_formatter[key] = units == QString("°C")
                                   ? FormatterType::Temperature
                                   : FormatterType::Integer;
        }
    } else if (source == QString("Local")) {
        // time
        m_map[source] = QString("time");
        m_formatter[QString("time")] = FormatterType::Time;
        // custom time
        m_map.insertMulti(source, QString("ctime"));
        m_formatter[QString("ctime")] = FormatterType::TimeCustom;
        // ISO time
        m_map.insertMulti(source, QString("isotime"));
        m_formatter[QString("isotime")] = FormatterType::TimeISO;
        // long time
        m_map.insertMulti(source, QString("longtime"));
        m_formatter[QString("longtime")] = FormatterType::TimeLong;
        // short time
        m_map.insertMulti(source, QString("shorttime"));
        m_formatter[QString("shorttime")] = FormatterType::TimeShort;
    } else if (source == QString("system/uptime")) {
        // uptime
        m_map[source] = QString("uptime");
        m_formatter[QString("uptime")] = FormatterType::Uptime;
        // custom uptime
        m_map.insertMulti(source, QString("cuptime"));
        m_formatter[QString("cuptime")] = FormatterType::UptimeCustom;
    } else if (source.startsWith(QString("weather/temperature"))) {
        // temperature
        QString key = source;
        key.remove(QString("weather/"));
        m_map[source] = key;
        m_formatter[key] = FormatterType::Temperature;
    } else if (source.startsWith(QString("weather/"))) {
        // other weather
        QString key = source;
        key.remove(QString("weather/"));
        m_map[source] = key;
        m_formatter[key] = FormatterType::NoFormat;
    } else if (source.startsWith(QString("load/load"))) {
        // load source
        QString key = source;
        key.remove(QString("load/"));
        m_map[source] = key;
        m_formatter[key] = FormatterType::Temperature;
    }

    QStringList foundKeys = keysFromSource(source);

    // rewrite formatters for custom ones
    QStringList customFormattersKeys = m_customFormatters->definedFormatters();
    qCInfo(LOG_AW) << "Looking for fprmatters" << foundKeys << "in"
                   << customFormattersKeys;
    for (auto key : foundKeys) {
        if (!customFormattersKeys.contains(key))
            continue;
        m_formatter[key] = FormatterType::Custom;
    }

    // drop key from dictionary if no one user requested key required it
    qCInfo(LOG_AW) << "Looking for keys" << foundKeys << "in" << keys;
    bool required
        = keys.isEmpty() || std::any_of(foundKeys.cbegin(), foundKeys.cend(),
                                        [&keys](const QString &key) {
                                            return keys.contains(key);
                                        });
    if (!required) {
        m_map.remove(source);
        for (auto key : foundKeys)
            m_formatter.remove(key);
    }

    return keysFromSource(source);
}


float AWKeysAggregator::temperature(const float temp) const
{
    qCDebug(LOG_AW) << "Temperature value" << temp;

    float converted = temp;
    if (m_tempUnits == QString("Celsius")) {
    } else if (m_tempUnits == QString("Fahrenheit")) {
        converted = temp * 9.0f / 5.0f + 32.0f;
    } else if (m_tempUnits == QString("Kelvin")) {
        converted = temp + 273.15f;
    } else if (m_tempUnits == QString("Reaumur")) {
        converted = temp * 0.8f;
    } else if (m_tempUnits == QString("cm^-1")) {
        converted = (temp + 273.15f) * 0.695f;
    } else if (m_tempUnits == QString("kJ/mol")) {
        converted = (temp + 273.15f) * 8.31f;
    } else if (m_tempUnits == QString("kcal/mol")) {
        converted = (temp + 273.15f) * 1.98f;
    } else {
        qCWarning(LOG_AW) << "Invalid units" << m_tempUnits;
    }

    return converted;
}
