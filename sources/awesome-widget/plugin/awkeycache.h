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

#pragma once

#include <QString>
#include <QVariant>

#include <matchers/awpluginmatchersettings.h>


namespace AWKeyCache
{
bool addKeyToCache(const QString &_type, const QString &_key = "");
QStringList getRequiredKeys(const QStringList &_keys, const QStringList &_bars, const QVariantMap &_tooltip,
                            const QStringList &_userKeys, const QStringList &_allKeys);
AWPluginMatcherSettings loadKeysFromCache();
} // namespace AWKeyCache
