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

#include "awdataenginemapper.h"
#include "awdebug.h"
#include "awformatterhelper.h"


AWKeysAggregator::AWKeysAggregator(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_settings.customFormatters = new AWFormatterHelper(this);
    m_mapper = new AWDataEngineMapper(this, m_settings.customFormatters);
}


void AWKeysAggregator::initFormatters()
{
    m_settings.customFormatters->initItems();
}


QString AWKeysAggregator::acOffline() const
{
    return m_settings.acOffline;
}


QString AWKeysAggregator::acOnline() const
{
    return m_settings.acOnline;
}


QString AWKeysAggregator::customTime() const
{
    return m_settings.customTime;
}


QString AWKeysAggregator::customUptime() const
{
    return m_settings.customUptime;
}


QString AWKeysAggregator::formatter(const QVariant &_data, const QString &_key, bool replaceSpace) const
{
    qCDebug(LOG_AW) << "Data" << _data << "for key" << _key;

    auto output = m_mapper->formatter(_key)->format(_data, _key, m_settings);
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


QString AWKeysAggregator::tempUnits() const
{
    return m_settings.tempUnits;
}


bool AWKeysAggregator::translate() const
{
    return m_settings.translate;
}


void AWKeysAggregator::setAcOffline(const QString &_inactive)
{
    qCDebug(LOG_AW) << "Inactive AC string" << _inactive;

    m_settings.acOffline = _inactive;
}


void AWKeysAggregator::setAcOnline(const QString &_active)
{
    qCDebug(LOG_AW) << "Active AC string" << _active;

    m_settings.acOnline = _active;
}


void AWKeysAggregator::setCustomTime(const QString &_customTime)
{
    qCDebug(LOG_AW) << "Format" << _customTime;

    m_settings.customTime = _customTime;
}


void AWKeysAggregator::setCustomUptime(const QString &_customUptime)
{
    qCDebug(LOG_AW) << "Format" << _customUptime;

    m_settings.customUptime = _customUptime;
}


void AWKeysAggregator::setDevices(const QHash<QString, QStringList> &_devices)
{
    qCDebug(LOG_AW) << "Devices" << _devices;

    m_mapper->setDevices(_devices);
}


void AWKeysAggregator::setTempUnits(const QString &_units)
{
    qCDebug(LOG_AW) << "Units" << _units;

    m_settings.tempUnits = _units;
}


void AWKeysAggregator::setTranslate(const bool _translate)
{
    qCDebug(LOG_AW) << "Translate" << _translate;

    m_settings.translate = _translate;
}


QStringList AWKeysAggregator::registerSource(const QString &_source, const KSysGuard::Unit _units,
                                             const QStringList &_keys)
{
    qCDebug(LOG_AW) << "Source" << _source << "with units" << _units;

    return m_mapper->registerSource(_source, _units, _keys);
}
