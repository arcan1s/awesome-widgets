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


AWKeysAggregator::AWKeysAggregator(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    // default formaters
    // memory
    m_formater[QString("mem")] = FormaterType::Float;
    m_formater[QString("memtotmb")] = FormaterType::MemMBFormat;
    m_formater[QString("memtotgb")] = FormaterType::MemGBFormat;
    // network
    m_formater[QString("down")] = FormaterType::NetSmartFormat;
    m_formater[QString("downkb")] = FormaterType::Integer;
    m_formater[QString("downunits")] = FormaterType::NetSmartUnits;
    m_formater[QString("up")] = FormaterType::NetSmartFormat;
    m_formater[QString("upkb")] = FormaterType::Integer;
    m_formater[QString("upunits")] = FormaterType::NetSmartUnits;
    // swap
    m_formater[QString("swap")] = FormaterType::Float;
    m_formater[QString("swaptotmb")] = FormaterType::MemMBFormat;
    m_formater[QString("swaptotgb")] = FormaterType::MemGBFormat;
}


AWKeysAggregator::~AWKeysAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QString AWKeysAggregator::formater(const QVariant &data,
                                   const QString &key) const
{
    qCDebug(LOG_AW) << "Data" << data << "for key" << key;

    QString output;
    QLocale loc = m_translate ? QLocale::system() : QLocale::c();
    // case block
    switch (m_formater[key]) {
    case FormaterType::Float:
        output = QString("%1").arg(data.toFloat(), 5, 'f', 1);
        break;
    case FormaterType::FloatTwoSymbols:
        output = QString("%1").arg(data.toFloat(), 5, 'f', 2);
        break;
    case FormaterType::Integer:
        output = QString("%1").arg(data.toFloat(), 4, 'f', 0);
        break;
    case FormaterType::IntegerFive:
        output = QString("%1").arg(data.toFloat(), 5, 'f', 0);
        break;
    case FormaterType::IntegerThree:
        output = QString("%1").arg(data.toFloat(), 3, 'f', 0);
        break;
    case FormaterType::List:
        output = data.toStringList().join(QChar(','));
        break;
    case FormaterType::ACFormat:
        output = data.toBool() ? m_acOnline : m_acOffline;
        break;
    case FormaterType::MemGBFormat:
        output
            = QString("%1").arg(data.toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
        break;
    case FormaterType::MemMBFormat:
        output = QString("%1").arg(data.toFloat() / 1024.0, 5, 'f', 0);
        break;
    case FormaterType::NetSmartFormat:
        output = [](const float value) {
            if (value > 1024.0)
                return QString("%1").arg(value / 1024.0, 4, 'f', 1);
            else
                return QString("%1").arg(value, 4, 'f', 0);
        }(data.toFloat());
        break;
    case FormaterType::NetSmartUnits:
        if (data.toFloat() > 1024.0)
            output = m_translate ? i18n("MB/s") : QString("MB/s");
        else
            output = m_translate ? i18n("KB/s") : QString("KB/s");
        break;
    case FormaterType::Quotes:
        // first cast
        output = QString("%1").arg(data.toDouble(), 0, 'f');
        output = output.rightJustified(8, QLatin1Char(' '), true);
        break;
    case FormaterType::Temperature:
        output = QString("%1").arg(temperature(data.toFloat()), 5, 'f', 1);
        break;
    case FormaterType::Time:
        output = data.toDateTime().toString();
        break;
    case FormaterType::TimeCustom:
        output = m_customTime;
        [&output, loc, this](const QDateTime dt) {
            for (auto key : timeKeys)
                output.replace(QString("$%1").arg(key), loc.toString(dt, key));
        }(data.toDateTime());
        break;
    case FormaterType::TimeISO:
        output = data.toDateTime().toString(Qt::ISODate);
        break;
    case FormaterType::TimeLong:
        output = loc.toString(data.toDateTime(), QLocale::LongFormat);
        break;
    case FormaterType::TimeShort:
        output = loc.toString(data.toDateTime(), QLocale::ShortFormat);
        break;
    case FormaterType::Uptime:
    case FormaterType::UptimeCustom:
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
            }(m_formater[key] == FormaterType::Uptime ? QString("$ddd$hhh$mmm")
                                                      : m_customUptime,
              static_cast<int>(data.toFloat()));
        break;
    case FormaterType::NoFormat:
        output = data.toString();
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
        m_formater[QString("ac")] = FormaterType::ACFormat;
    } else if (source.startsWith(QString("battery/"))) {
        // battery stats
        QString key = source;
        key.remove(QString("battery/"));
        m_map[source] = key;
        m_formater[key] = FormaterType::IntegerThree;
    } else if (source == QString("cpu/system/TotalLoad")) {
        // cpu
        m_map[source] = QString("cpu");
        m_formater[QString("cpu")] = FormaterType::Float;
    } else if (source.contains(cpuRegExp)) {
        // cpus
        QString key = source;
        key.remove(QString("cpu/")).remove(QString("/TotalLoad"));
        m_map[source] = key;
        m_formater[key] = FormaterType::Float;
    } else if (source == QString("cpu/system/AverageClock")) {
        // cpucl
        m_map[source] = QString("cpucl");
        m_formater[QString("cpucl")] = FormaterType::Integer;
    } else if (source.contains(cpuclRegExp)) {
        // cpucls
        QString key = source;
        key.remove(QString("cpu/cpu")).remove(QString("/clock"));
        key = QString("cpucl%1").arg(key);
        m_map[source] = key;
        m_formater[key] = FormaterType::Integer;
    } else if (source.startsWith(QString("custom"))) {
        // custom
        QString key = source;
        key.remove(QString("custom/"));
        m_map[source] = key;
        m_formater[key] = FormaterType::NoFormat;
    } else if (source == QString("desktop/current/name")) {
        // current desktop name
        m_map[source] = QString("desktop");
        m_formater[QString("desktop")] = FormaterType::NoFormat;
    } else if (source == QString("desktop/current/number")) {
        // current desktop number
        m_map[source] = QString("ndesktop");
        m_formater[QString("ndesktop")] = FormaterType::NoFormat;
    } else if (source == QString("desktop/total/number")) {
        // desktop count
        m_map[source] = QString("tdesktops");
        m_formater[QString("tdesktops")] = FormaterType::NoFormat;
    } else if (source.contains(hddrRegExp)) {
        // read speed
        QString device = source;
        device.remove(QString("/Rate/rblk"));
        int index = m_devices[QString("disk")].indexOf(device);
        if (index > -1) {
            QString key = QString("hddr%1").arg(index);
            m_map[source] = key;
            m_formater[key] = FormaterType::Integer;
        }
    } else if (source.contains(hddwRegExp)) {
        // write speed
        QString device = source;
        device.remove(QString("/Rate/wblk"));
        int index = m_devices[QString("disk")].indexOf(device);
        if (index > -1) {
            QString key = QString("hddw%1").arg(index);
            m_map[source] = key;
            m_formater[key] = FormaterType::Integer;
        }
    } else if (source == QString("gpu/load")) {
        // gpu load
        m_map[source] = QString("gpu");
        m_formater[QString("gpu")] = FormaterType::Float;
    } else if (source == QString("gpu/temperature")) {
        // gpu temperature
        m_map[source] = QString("gputemp");
        m_formater[QString("gputemp")] = FormaterType::Temperature;
    } else if (source.contains(mountFillRegExp)) {
        // fill level
        QString device = source;
        device.remove(QString("partitions")).remove(QString("/filllevel"));
        int index = m_devices[QString("mount")].indexOf(device);
        if (index > -1) {
            QString key = QString("hdd%1").arg(index);
            m_map[source] = key;
            m_formater[key] = FormaterType::Float;
            // additional keys
            m_formater[QString("hddtotmb%1").arg(index)]
                = FormaterType::MemMBFormat;
            m_formater[QString("hddtotgb%1").arg(index)]
                = FormaterType::MemGBFormat;
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
            m_formater[key] = FormaterType::MemMBFormat;
            // gb
            key = QString("hddfreegb%1").arg(index);
            m_map.insertMulti(source, key);
            m_formater[key] = FormaterType::MemGBFormat;
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
            m_formater[key] = FormaterType::MemMBFormat;
            // gb
            key = QString("hddgb%1").arg(index);
            m_map.insertMulti(source, key);
            m_formater[key] = FormaterType::MemGBFormat;
        }
    } else if (source.startsWith(QString("hdd/temperature"))) {
        // hdd temperature
        QString device = source;
        device.remove(QString("hdd/temperature"));
        int index = m_devices[QString("hdd")].indexOf(device);
        if (index > -1) {
            QString key = QString("hddtemp%1").arg(index);
            m_map[source] = key;
            m_formater[key] = FormaterType::Temperature;
        }
    } else if (source.startsWith(QString("cpu/system/loadavg"))) {
        // load average
        QString time = source;
        time.remove(QString("cpu/system/loadavg"));
        QString key = QString("la%1").arg(time);
        m_map[source] = key;
        m_formater[key] = FormaterType::FloatTwoSymbols;
    } else if (source == QString("mem/physical/application")) {
        // app memory
        // mb
        m_map[source] = QString("memmb");
        m_formater[QString("memmb")] = FormaterType::MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("memgb"));
        m_formater[QString("memgb")] = FormaterType::MemGBFormat;
    } else if (source == QString("mem/physical/free")) {
        // free memory
        // mb
        m_map[source] = QString("memfreemb");
        m_formater[QString("memfreemb")] = FormaterType::MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("memfreegb"));
        m_formater[QString("memfreegb")] = FormaterType::MemGBFormat;
    } else if (source == QString("mem/physical/used")) {
        // used memory
        // mb
        m_map[source] = QString("memusedmb");
        m_formater[QString("memusedmb")] = FormaterType::MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("memusedgb"));
        m_formater[QString("memusedgb")] = FormaterType::MemGBFormat;
    } else if (source == QString("network/current/name")) {
        // network device
        m_map[source] = QString("netdev");
        m_formater[QString("netdev")] = FormaterType::NoFormat;
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
            m_formater[key] = FormaterType::Integer;
            // smart
            key = QString("%1%2").arg(type).arg(index);
            m_map.insertMulti(source, key);
            m_formater[key] = FormaterType::NetSmartFormat;
            // units
            key = QString("%1units%2").arg(type).arg(index);
            m_map.insertMulti(source, key);
            m_formater[key] = FormaterType::NetSmartUnits;
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
            m_formater[key] = FormaterType::Integer;
            // mb
            key = QString("%1total%2").arg(type).arg(index);
            m_map.insertMulti(source, key);
            m_formater[key] = FormaterType::MemMBFormat;
        }
    } else if (source.startsWith(QString("upgrade"))) {
        // package manager
        QString key = source;
        key.remove(QString("upgrade/"));
        m_map[source] = key;
        m_formater[key] = FormaterType::IntegerThree;
    } else if (source.startsWith(QString("player"))) {
        // player
        QString key = source;
        key.remove(QString("player/"));
        m_map[source] = key;
        m_formater[key] = FormaterType::NoFormat;
    } else if (source == QString("ps/running/count")) {
        // running processes count
        m_map[source] = QString("pscount");
        m_formater[QString("pscount")] = FormaterType::NoFormat;
    } else if (source == QString("ps/running/list")) {
        // list of running processes
        m_map[source] = QString("ps");
        m_formater[QString("ps")] = FormaterType::List;
    } else if (source == QString("ps/total/count")) {
        // total processes count
        m_map[source] = QString("pstotal");
        m_formater[QString("pstotal")] = FormaterType::NoFormat;
    } else if (source.startsWith(QString("quotes"))) {
        // quotes
        QString key = source;
        key.remove(QString("quotes/"));
        m_map[source] = key;
        m_formater[key] = FormaterType::Quotes;
    } else if (source == QString("mem/swap/free")) {
        // free swap
        // mb
        m_map[source] = QString("swapfreemb");
        m_formater[QString("swapfreemb")] = FormaterType::MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("swapfreegb"));
        m_formater[QString("swapfreegb")] = FormaterType::MemGBFormat;
    } else if (source == QString("mem/swap/used")) {
        // used swap
        // mb
        m_map[source] = QString("swapmb");
        m_formater[QString("swapmb")] = FormaterType::MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("swapgb"));
        m_formater[QString("swapgb")] = FormaterType::MemGBFormat;
    } else if (source.startsWith(QString("lmsensors/"))) {
        // temperature
        int index = m_devices[QString("temp")].indexOf(source);
        // FIXME on DE initialization there are no units key
        if (units.isEmpty())
            return QStringList() << QString("temp%1").arg(index);
        if (index > -1) {
            QString key = QString("temp%1").arg(index);
            m_map[source] = key;
            m_formater[key] = units == QString("°C") ? FormaterType::Temperature
                                                     : FormaterType::Integer;
        }
    } else if (source == QString("Local")) {
        // time
        m_map[source] = QString("time");
        m_formater[QString("time")] = FormaterType::Time;
        // custom time
        m_map.insertMulti(source, QString("ctime"));
        m_formater[QString("ctime")] = FormaterType::TimeCustom;
        // ISO time
        m_map.insertMulti(source, QString("isotime"));
        m_formater[QString("isotime")] = FormaterType::TimeISO;
        // long time
        m_map.insertMulti(source, QString("longtime"));
        m_formater[QString("longtime")] = FormaterType::TimeLong;
        // short time
        m_map.insertMulti(source, QString("shorttime"));
        m_formater[QString("shorttime")] = FormaterType::TimeShort;
    } else if (source == QString("system/uptime")) {
        // uptime
        m_map[source] = QString("uptime");
        m_formater[QString("uptime")] = FormaterType::Uptime;
        // custom uptime
        m_map.insertMulti(source, QString("cuptime"));
        m_formater[QString("cuptime")] = FormaterType::UptimeCustom;
    } else if (source.startsWith(QString("weather/temperature"))) {
        // temperature
        QString key = source;
        key.remove(QString("weather/"));
        m_map[source] = key;
        m_formater[key] = FormaterType::Temperature;
    } else if (source.startsWith(QString("weather/"))) {
        // other weather
        QString key = source;
        key.remove(QString("weather/"));
        m_map[source] = key;
        m_formater[key] = FormaterType::NoFormat;
    } else if (source.startsWith(QString("load/load"))) {
        // load source
        QString key = source;
        key.remove(QString("load/"));
        m_map[source] = key;
        m_formater[key] = FormaterType::Temperature;
    }

    // drop key from dictionary if no one user requested key required it
    QStringList foundKeys = keysFromSource(source);
    qCInfo(LOG_AW) << "Looking for keys" << foundKeys << "in" << keys;
    // this source is required if list is empty (which means skip checking)
    // or if key in required key list
    bool required
        = keys.isEmpty() || std::any_of(foundKeys.cbegin(), foundKeys.cend(),
                                        [&keys](const QString &key) {
                                            return keys.contains(key);
                                        });
    if (!required)
        m_map.remove(source);

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
