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

#include "awpluginformattertimecustom.h"

#include "awdebug.h"


QString AWPluginFormatterTimeCustom::format(const QVariant &_value, const QString &,
                                            const AWPluginFormatSettings &_settings) const
{
    auto value = QDateTime::fromSecsSinceEpoch(_value.toLongLong());
    return format(value, _settings.customTime, locale(_settings));
}


void AWPluginFormatterTimeCustom::load()
{
    m_timeKeys = QString(TIME_KEYS).split(',');
    m_timeKeys.sort();
    std::reverse(m_timeKeys.begin(), m_timeKeys.end());
}


QString AWPluginFormatterTimeCustom::format(const QDateTime &_value, QString _formatString,
                                            const QLocale &_locale) const
{
    for (auto &key : m_timeKeys)
        _formatString.replace(QString("$%1").arg(key), _locale.toString(_value, key));

    return _formatString;
}
