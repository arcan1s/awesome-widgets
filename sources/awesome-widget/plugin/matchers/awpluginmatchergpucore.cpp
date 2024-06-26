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

#include "awpluginmatchergpucore.h"

#include <QRegularExpression>

#include "formatters/formatters.h"


QHash<QString, AWPluginFormaterInterface *> AWPluginMatcherGPUCore::keys(const QString &_source, const KSysGuard::Unit,
                                                                         const AWPluginMatcherSettings &_settings) const
{
    auto index = AWPluginMatcher::index(_source, _settings.gpu);
    if (index == -1)
        return {};

    return {{QString("gpu%1").arg(index), AWPluginFormatterFloat::instance()}};
}


bool AWPluginMatcherGPUCore::matches(const QString &_source) const
{
    static auto regexp = QRegularExpression("^gpu/gpu.*/usage$");
    return _source.contains(regexp);
}
