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

#include "awesomewidget.h"

#include <QtQml>

#include "awactions.h"
#include "awconfighelper.h"
#include "awkeys.h"


void AWPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.private.awesomewidget"));

    qmlRegisterType<AWActions>(uri, 1, 0, "AWActions");
    qmlRegisterType<AWConfigHelper>(uri, 1, 0, "AWConfigHelper");
    qmlRegisterType<AWKeys>(uri, 1, 0, "AWKeys");
}
