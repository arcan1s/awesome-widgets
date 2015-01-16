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

#include <KI18n/KLocalizedString>
#include <KNotifications/KNotification>

#include <QDebug>
#include <QInputDialog>
#include <QProcessEnvironment>
#include <QSettings>
#include <QStandardPaths>

#include <pdebug/pdebug.h>

#include "awadds.h"
#include "graphicalitem.h"
#include "version.h"


AWAdds::AWAdds(QObject *parent)
    : QObject(parent)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));
}


AWAdds::~AWAdds()
{
    if (debug) qDebug() << PDEBUG;
}
