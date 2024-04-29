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

#include "awpluginmatcherdesktopnumber.h"

#include "formatters/formatters.h"


QHash<QString, AWPluginFormaterInterface *> AWPluginMatcherDesktopNumber::keys(const QString &, const KSysGuard::Unit,
                                                                               const AWPluginMatcherSettings &) const
{
    return {{"ndesktop", AWPluginFormatterNoFormat::instance()}};
}


bool AWPluginMatcherDesktopNumber::matches(const QString &_source) const
{
    return _source == "extsysmon/desktop/number";
}
