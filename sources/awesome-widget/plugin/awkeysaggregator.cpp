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
}


AWKeysAggregator::~AWKeysAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QString AWKeysAggregator::formater(const QVariant &data,
                                   const QString &key) const
{
    qCDebug(LOG_AW) << "Data" << data;
    qCDebug(LOG_AW) << "Key" << key;

    QString output;
    QLocale loc = m_translate ? QLocale::system() : QLocale::c();
    // case block
    switch (m_formater[key]) {
    case Float:
        output = QString("%1").arg(data.toFloat(), 5, 'f', 1);
        break;
    case FloatTwoSymbols:
        output = QString("%1").arg(data.toFloat(), 5, 'f', 2);
        break;
    case Integer:
        output = QString("%1").arg(data.toFloat(), 4, 'f', 0);
        break;
    case IntegerThree:
        output = QString("%1").arg(data.toFloat(), 3, 'f', 0);
        break;
    case List:
        output = data.toStringList().join(QChar(','));
        break;
    case ACFormat:
        output = data.toBool() ? m_acOnline : m_acOffline;
        break;
    case MemGBFormat:
        output
            = QString("%1").arg(data.toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
        break;
    case MemMBFormat:
        output = QString("%1").arg(data.toFloat() / 1024.0, 5, 'f', 0);
        break;
    case NetSmartFormat:
        output = [](const float value) {
            if (value > 1024.0)
                return QString("%1").arg(value / 1024.0, 4, 'f', 1);
            else
                return QString("%1").arg(value, 4, 'f', 0);
        }(data.toFloat());
        break;
    case NetSmartUnits:
        if (data.toFloat() > 1024.0)
            output = m_translate ? i18n("MB/s") : QString("MB/s");
        else
            output = m_translate ? i18n("KB/s") : QString("KB/s");
        break;
    case Quotes:
        // first cast
        output = QString("%1").arg(data.toDouble(), 0, 'f');
        output = output.rightJustified(8, QLatin1Char(' '), true);
        break;
    case Temperature:
        output = QString("%1").arg(temperature(data.toFloat()), 5, 'f', 1);
        break;
    case Time:
        output = data.toDateTime().toString();
        break;
    case TimeCustom:
        output = m_customTime;
        [&output, loc, this](const QDateTime dt) {
            foreach (QString key, timeKeys)
                output.replace(QString("$%1").arg(key), loc.toString(dt, key));
        }(data.toDateTime());
        break;
    case TimeISO:
        output = data.toDateTime().toString(Qt::ISODate);
        break;
    case TimeLong:
        output = loc.toString(data.toDateTime(), QLocale::LongFormat);
        break;
    case TimeShort:
        output = loc.toString(data.toDateTime(), QLocale::ShortFormat);
        break;
    case Uptime:
    case UptimeCustom:
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
            }(m_formater[key] == Uptime ? QString("$ddd$hhh$mmm")
                                        : m_customUptime,
              data.toFloat());
        break;
    case NoFormat:
    default:
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
                                             const QString &units)
{
    qCDebug(LOG_AW) << "Source" << source;
    qCDebug(LOG_AW) << "Units" << units;

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

    if (source == QString("battery/ac")) {
        // AC
        m_map[source] = QString("ac");
        m_formater[QString("ac")] = ACFormat;
    } else if (source.startsWith(QString("battery/"))) {
        // battery stats
        QString key = source;
        key.remove(QString("battery/"));
        m_map[source] = key;
        m_formater[key] = IntegerThree;
    } else if (source == QString("cpu/system/TotalLoad")) {
        // cpu
        m_map[source] = QString("cpu");
        m_formater[QString("cpu")] = Float;
    } else if (source.contains(cpuRegExp)) {
        // cpus
        QString key = source;
        key.remove(QString("cpu/")).remove(QString("/TotalLoad"));
        m_map[source] = key;
        m_formater[key] = Float;
    } else if (source == QString("cpu/system/AverageClock")) {
        // cpucl
        m_map[source] = QString("cpucl");
        m_formater[QString("cpucl")] = Integer;
    } else if (source.contains(cpuclRegExp)) {
        // cpucls
        QString key = source;
        key.remove(QString("cpu/cpu")).remove(QString("/clock"));
        key = QString("cpucl%1").arg(key);
        m_map[source] = key;
        m_formater[key] = Integer;
    } else if (source.startsWith(QString("custom"))) {
        // custom
        QString key = source;
        key.remove(QString("custom/"));
        m_map[source] = key;
        m_formater[key] = NoFormat;
    } else if (source == QString("desktop/current/name")) {
        // current desktop name
        m_map[source] = QString("desktop");
        m_formater[QString("desktop")] = NoFormat;
    } else if (source == QString("desktop/current/number")) {
        // current desktop number
        m_map[source] = QString("ndesktop");
        m_formater[QString("ndesktop")] = NoFormat;
    } else if (source == QString("desktop/total/number")) {
        // desktop count
        m_map[source] = QString("tdesktops");
        m_formater[QString("tdesktops")] = NoFormat;
    } else if (source.contains(hddrRegExp)) {
        // read speed
        QString device = source;
        device.remove(QString("/Rate/rblk"));
        int index = m_devices[QString("disk")].indexOf(device);
        if (index > -1) {
            QString key = QString("hddr%1").arg(index);
            m_map[source] = key;
            m_formater[key] = Integer;
        }
    } else if (source.contains(hddwRegExp)) {
        // write speed
        QString device = source;
        device.remove(QString("/Rate/wblk"));
        int index = m_devices[QString("disk")].indexOf(device);
        if (index > -1) {
            QString key = QString("hddw%1").arg(index);
            m_map[source] = key;
            m_formater[key] = Integer;
        }
    } else if (source == QString("gpu/load")) {
        // gpu load
        m_map[source] = QString("gpu");
        m_formater[QString("gpu")] = Float;
    } else if (source == QString("gpu/temperature")) {
        // gpu temperature
        m_map[source] = QString("gputemp");
        m_formater[QString("gputemp")] = Temperature;
    } else if (source.contains(mountFillRegExp)) {
        // fill level
        QString device = source;
        device.remove(QString("partitions")).remove(QString("/filllevel"));
        int index = m_devices[QString("mount")].indexOf(device);
        if (index > -1) {
            QString key = QString("hdd%1").arg(index);
            m_map[source] = key;
            m_formater[key] = Float;
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
            m_formater[key] = MemMBFormat;
            // gb
            key = QString("hddfreegb%1").arg(index);
            m_map.insertMulti(source, key);
            m_formater[key] = MemGBFormat;
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
            m_formater[key] = MemMBFormat;
            // gb
            key = QString("hddgb%1").arg(index);
            m_map.insertMulti(source, key);
            m_formater[key] = MemGBFormat;
        }
    } else if (source.startsWith(QString("hdd/temperature"))) {
        // hdd temperature
        QString device = source;
        device.remove(QString("hdd/temperature"));
        int index = m_devices[QString("hdd")].indexOf(device);
        if (index > -1) {
            QString key = QString("hddtemp%1").arg(index);
            m_map[source] = key;
            m_formater[key] = Temperature;
        }
    } else if (source.startsWith(QString("cpu/system/loadavg"))) {
        // load average
        QString time = source;
        time.remove(QString("cpu/system/loadavg"));
        QString key = QString("la%1").arg(time);
        m_map[source] = key;
        m_formater[key] = FloatTwoSymbols;
    } else if (source == QString("mem/physical/application")) {
        // app memory
        // mb
        m_map[source] = QString("memmb");
        m_formater[QString("memmb")] = MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("memgb"));
        m_formater[QString("memgb")] = MemGBFormat;
    } else if (source == QString("mem/physical/free")) {
        // free memory
        // mb
        m_map[source] = QString("memfreemb");
        m_formater[QString("memfreemb")] = MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("memfreegb"));
        m_formater[QString("memfreegb")] = MemGBFormat;
    } else if (source == QString("mem/physical/used")) {
        // used memory
        // mb
        m_map[source] = QString("memusedmb");
        m_formater[QString("memusedmb")] = MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("memusedgb"));
        m_formater[QString("memusedgb")] = MemGBFormat;
    } else if (source == QString("network/current/name")) {
        // network device
        m_map[source] = QString("netdev");
        m_formater[QString("netdev")] = NoFormat;
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
            m_formater[key] = Integer;
            // smart
            key = QString("%1%2").arg(type).arg(index);
            m_map.insertMulti(source, key);
            m_formater[key] = NetSmartFormat;
            // units
            key = QString("%1units%2").arg(type).arg(index);
            m_map.insertMulti(source, key);
            m_formater[key] = NetSmartUnits;
        }
    } else if (source.startsWith(QString("upgrade"))) {
        // package manager
        QString key = source;
        key.remove(QString("upgrade/"));
        m_map[source] = key;
        m_formater[key] = IntegerThree;
    } else if (source.startsWith(QString("player"))) {
        // player
        QString key = source;
        key.remove(QString("player/"));
        m_map[source] = key;
        m_formater[key] = NoFormat;
    } else if (source == QString("ps/running/count")) {
        // running processes count
        m_map[source] = QString("pscount");
        m_formater[QString("pscount")] = NoFormat;
    } else if (source == QString("ps/running/list")) {
        // list of running processes
        m_map[source] = QString("ps");
        m_formater[QString("ps")] = List;
    } else if (source == QString("ps/total/count")) {
        // total processes count
        m_map[source] = QString("pstotal");
        m_formater[QString("pstotal")] = NoFormat;
    } else if (source.startsWith(QString("quotes"))) {
        // quotes
        QString key = source;
        key.remove(QString("quotes/"));
        m_map[source] = key;
        m_formater[key] = Quotes;
    } else if (source == QString("mem/swap/free")) {
        // free swap
        // mb
        m_map[source] = QString("swapfreemb");
        m_formater[QString("swapfreemb")] = MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("swapfreegb"));
        m_formater[QString("swapfreegb")] = MemGBFormat;
    } else if (source == QString("mem/swap/used")) {
        // used swap
        // mb
        m_map[source] = QString("swapmb");
        m_formater[QString("swapmb")] = MemMBFormat;
        // gb
        m_map.insertMulti(source, QString("swapgb"));
        m_formater[QString("swapgb")] = MemGBFormat;
    } else if (source.startsWith(QString("lmsensors/"))) {
        // temperature
        int index = m_devices[QString("temp")].indexOf(source);
        // FIXME on DE initialization there are no units key
        if (units.isEmpty())
            return QStringList() << QString("temp%1").arg(index);
        if (index > -1) {
            QString key = QString("temp%1").arg(index);
            m_map[source] = key;
            m_formater[key] = units == QString("°C") ? Temperature : Integer;
        }
    } else if (source == QString("Local")) {
        // time
        m_map[source] = QString("time");
        m_formater[QString("time")] = Time;
        // custom time
        m_map.insertMulti(source, QString("ctime"));
        m_formater[QString("ctime")] = TimeCustom;
        // ISO time
        m_map.insertMulti(source, QString("isotime"));
        m_formater[QString("isotime")] = TimeISO;
        // long time
        m_map.insertMulti(source, QString("longtime"));
        m_formater[QString("longtime")] = TimeLong;
        // short time
        m_map.insertMulti(source, QString("shorttime"));
        m_formater[QString("shorttime")] = TimeShort;
    } else if (source == QString("system/uptime")) {
        // uptime
        m_map[source] = QString("uptime");
        m_formater[QString("uptime")] = Uptime;
        // custom uptime
        m_map.insertMulti(source, QString("cuptime"));
        m_formater[QString("cuptime")] = UptimeCustom;
    } else if (source.startsWith(QString("weather/temperature"))) {
        // temperature
        QString key = source;
        key.remove(QString("weather/"));
        m_map[source] = key;
        m_formater[key] = Temperature;
    } else if (source.startsWith(QString("weather/"))) {
        // other weather
        QString key = source;
        key.remove(QString("weather/"));
        m_map[source] = key;
        m_formater[key] = NoFormat;
    } else if (source.startsWith(QString("load/load"))) {
        // load source
        QString key = source;
        key.remove(QString("load/"));
        m_map[source] = key;
        m_formater[key] = Temperature;
    }

    return keysFromSource(source);
}


float AWKeysAggregator::temperature(const float temp) const
{
    qCDebug(LOG_AW) << "Temperature value" << temp;

    float converted = temp;
    if (m_tempUnits == QString("Celsius")) {
    } else if (m_tempUnits == QString("Fahrenheit")) {
        converted = temp * 9.0 / 5.0 + 32.0;
    } else if (m_tempUnits == QString("Kelvin")) {
        converted = temp + 273.15;
    } else if (m_tempUnits == QString("Reaumur")) {
        converted = temp * 0.8;
    } else if (m_tempUnits == QString("cm^-1")) {
        converted = (temp + 273.15) * 0.695;
    } else if (m_tempUnits == QString("kJ/mol")) {
        converted = (temp + 273.15) * 8.31;
    } else if (m_tempUnits == QString("kcal/mol")) {
        converted = (temp + 273.15) * 1.98;
    } else {
        qCWarning(LOG_AW) << "Invalid units" << m_tempUnits;
    }

    return converted;
}
