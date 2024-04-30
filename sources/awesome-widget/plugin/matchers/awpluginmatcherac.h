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

#include "awpluginmatcher.h"


class AWPluginMatcherAC : public AWPluginMatcher<AWPluginMatcherAC>
{
public:
    [[nodiscard]] QHash<QString, AWPluginFormaterInterface *> keys(const QString &, KSysGuard::Unit,
                                                                   const AWPluginMatcherSettings &) const override;
    [[nodiscard]] bool matches(const QString &_source) const override;
};
