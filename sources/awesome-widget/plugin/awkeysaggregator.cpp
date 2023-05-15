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

#include "awdataenginemapper.h"
#include "awdebug.h"
#include "awformatterhelper.h"
#include "version.h"


AWKeysAggregator::AWKeysAggregator(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_customFormatters = new AWFormatterHelper(nullptr);
    m_mapper = new AWDataEngineMapper(this, m_customFormatters);

    // sort time keys
    m_timeKeys = QString(TIME_KEYS).split(',');
    m_timeKeys.sort();
    std::reverse(m_timeKeys.begin(), m_timeKeys.end());
}


AWKeysAggregator::~AWKeysAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


void AWKeysAggregator::initFormatters()
{
    m_customFormatters->initItems();
}


QString AWKeysAggregator::formatter(const QVariant &_data, const QString &_key, bool replaceSpace) const
{
    qCDebug(LOG_AW) << "Data" << _data << "for key" << _key;

    QString output;
    QLocale loc = m_translate ? QLocale::system() : QLocale::c();
    // case block
    switch (m_mapper->formatter(_key)) {
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
        output = QString("%1").arg(_data.toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
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
        [&output, loc, this](const QDateTime &dt) {
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
        output = QString("%1").arg(_data.toDateTime().toMSecsSinceEpoch() / 1000.0, 10, 'f', 0);
        break;
    case FormatterType::Uptime:
    case FormatterType::UptimeCustom:
        output =
            [](QString source, const int uptime) {
                int seconds = uptime - uptime % 60;
                int minutes = seconds / 60 % 60;
                int hours = ((seconds / 60) - minutes) / 60 % 24;
                int days = (((seconds / 60) - minutes) / 60 - hours) / 24;
                source.replace("$dd", QString("%1").arg(days, 3, 10, QChar('0')));
                source.replace("$d", QString("%1").arg(days));
                source.replace("$hh", QString("%1").arg(hours, 2, 10, QChar('0')));
                source.replace("$h", QString("%1").arg(hours));
                source.replace("$mm", QString("%1").arg(minutes, 2, 10, QChar('0')));
                source.replace("$m", QString("%1").arg(minutes));
                return source;
            }(m_mapper->formatter(_key) == FormatterType::Uptime ? "$ddd$hhh$mmm" : m_customUptime,
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

    // replace spaces to non-breakable ones
    replaceSpace &= (!_key.startsWith("custom") && (!_key.startsWith("weather")));
    if (replaceSpace)
        output.replace(" ", "&nbsp;");

    return output;
}


QStringList AWKeysAggregator::keysFromSource(const QString &_source) const
{
    qCDebug(LOG_AW) << "Search for source" << _source;

    return m_mapper->keysFromSource(_source);
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

    m_mapper->setDevices(_devices);
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


QStringList AWKeysAggregator::registerSource(const QString &_source, const QString &_units, const QStringList &_keys)
{
    qCDebug(LOG_AW) << "Source" << _source << "with units" << _units;

    return m_mapper->registerSource(_source, _units, _keys);
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
