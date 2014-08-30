/***************************************************************************
 *   This file is part of pytextmonitor                                    *
 *                                                                         *
 *   pytextmonitor is free software: you can redistribute it and/or        *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   pytextmonitor is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with pytextmonitor. If not, see http://www.gnu.org/licenses/    *
 ***************************************************************************/

#include "awesome-widget.h"


#include <pdebug/pdebug.h>


void AwesomeWidget::connectToEngine()
{
    if (debug) qDebug() << PDEBUG;
}


void AwesomeWidget::dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Run function with source name" << sourceName;

    if (data.keys().count() == 0)
        return;
//    if (sourceName == QString("desktop")) {
//        currentDesktop = data[QString("currentNumber")].toInt();
//        if (desktopNames.isEmpty()) {
//            desktopNames = data[QString("list")].toString().split(QString(";;"));
//            reinit();
//        }
//        updateText();
//    }
}


void AwesomeWidget::disconnectFromEngine()
{
    if (debug) qDebug() << PDEBUG;
}
