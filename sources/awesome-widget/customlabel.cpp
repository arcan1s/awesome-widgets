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

#include "customlabel.h"

#include <QGraphicsSceneMouseEvent>

#include "awesome-widget.h"
#include <pdebug/pdebug.h>


CustomLabel::CustomLabel(AwesomeWidget *wid, const bool debugCmd)
    : Plasma::Label(wid),
      debug(debugCmd)
{
}


CustomLabel::~CustomLabel()
{
    if (debug) qDebug() << PDEBUG;
}


void CustomLabel::setPopupEnabled(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    enablePopup = state;
}


void CustomLabel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Get signal" << event->button();

    if ((enablePopup) && (event->button() == Qt::LeftButton))
        {}
    emit(Plasma::Label::mousePressEvent(event));
}
