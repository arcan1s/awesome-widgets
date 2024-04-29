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
#include "formatters/formatters.h"
#include "matchers/matchers.h"


AWDataEngineMapper::AWDataEngineMapper(QObject *_parent, AWFormatterHelper *_custom)
    : QObject(_parent)
    , m_customFormatters(_custom)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    // default formatters
    // network
    m_formatter["down"] = AWPluginFormatterNet::instance();
    m_formatter["downkb"] = AWPluginFormatterMemory::instance();
    m_formatter["downtot"] = AWPluginFormatterMemoryMB::instance();
    m_formatter["downtotkb"] = AWPluginFormatterMemory::instance();
    m_formatter["downunits"] = AWPluginFormatterNetUnits::instance();
    m_formatter["up"] = AWPluginFormatterNet::instance();
    m_formatter["upkb"] = AWPluginFormatterMemory::instance();
    m_formatter["uptot"] = AWPluginFormatterMemoryMB::instance();
    m_formatter["uptotkb"] = AWPluginFormatterMemory::instance();
    m_formatter["upunits"] = AWPluginFormatterNetUnits::instance();
}


AWPluginFormaterInterface *AWDataEngineMapper::formatter(const QString &_key) const
{
    qCDebug(LOG_AW) << "Get formatter for key" << _key;

    return m_formatter.value(_key, AWPluginFormatterNoFormat::instance());
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

    auto matchers = AWPluginMatchers::matchers;
    auto matcher = std::find_if(matchers.cbegin(), matchers.cend(),
                                [&_source](auto matcher) { return matcher->matches(_source); });
    if (matcher == matchers.cend())
        return {};

    auto foundKeys = (*matcher)->keys(_source, _units, m_settings);
    auto keys = foundKeys.keys(); // speedup a little bit

    // rewrite formatters for custom ones
    QStringList customFormattersKeys;
    if (m_customFormatters)
        customFormattersKeys = m_customFormatters->definedFormatters();
    qCInfo(LOG_AW) << "Looking for formatters" << keys << "in" << customFormattersKeys;
    for (auto &key : keys) {
        if (!customFormattersKeys.contains(key))
            continue;
        foundKeys[key] = AWPluginFormatterCustom::instance();
    }

    // check if keys were actually requested
    qCInfo(LOG_AW) << "Looking for keys" << keys << "in" << _keys;
    auto required = _keys.isEmpty()
                    || std::any_of(keys.cbegin(), keys.cend(), [&_keys](auto &key) { return _keys.contains(key); });
    if (!required)
        return {};

    // insert keys into memory
    for (auto [key, formatter] : foundKeys.asKeyValueRange()) {
        m_map.insert(_source, key);
        m_formatter[key] = formatter;
    }

    return keys;
}


void AWDataEngineMapper::setDevices(const AWPluginMatcherSettings &_settings)
{
    m_settings = _settings;
}
