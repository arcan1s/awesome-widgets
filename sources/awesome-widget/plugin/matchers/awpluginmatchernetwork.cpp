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

#include "awpluginmatchernetwork.h"

#include <QRegularExpression>

#include "formatters/formatters.h"


QHash<QString, AWPluginFormaterInterface *> AWPluginMatcherNetwork::keys(const QString &_source, const KSysGuard::Unit,
                                                                         const AWPluginMatcherSettings &_settings) const
{
    auto type = _source.endsWith("download") ? "down" : "up";
    auto device = AWPluginMatcher::device(_source);
    auto index = _settings.network.indexOf(device);

    if (index == -1)
        return {};

    return {
        {QString("%1%2").arg(type).arg(index), AWPluginFormatterNet::instance()},
        {QString("%1kb%2").arg(type).arg(index), AWPluginFormatterMemory::instance()},
        {QString("%1units%2").arg(type).arg(index), AWPluginFormatterNetUnits::instance()},
    };
}


bool AWPluginMatcherNetwork::matches(const QString &_source) const
{
    static auto regexp = QRegularExpression("^network/.*/(download|upload)$");
    return _source.contains(regexp);
}
