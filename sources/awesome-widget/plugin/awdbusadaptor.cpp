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

#include "awdbusadaptor.h"

#include "awdebug.h"
#include "awkeys.h"


AWDBusAdaptor::AWDBusAdaptor(AWKeys *parent)
    : QDBusAbstractAdaptor(parent)
    , m_plugin(parent)
{
    qCDebug(LOG_DBUS) << __PRETTY_FUNCTION__;
}


AWDBusAdaptor::~AWDBusAdaptor()
{
    qCDebug(LOG_DBUS) << __PRETTY_FUNCTION__;
}


qlonglong AWDBusAdaptor::whoAmI() const
{
    return reinterpret_cast<qlonglong>(m_plugin);
}
