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

#include "customlabel.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCodec>

#include "awesome-widget.h"
#include <pdebug/pdebug.h>
#include <task/taskadds.h>


CustomLabel::CustomLabel(AwesomeWidget *wid, const bool debugCmd)
    : QLabel(0),
      widget(wid),
      debug(debugCmd)
{
    setWordWrap(false);
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


void CustomLabel::mousePressEvent(QMouseEvent *event)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Get signal" << event->button();

    if ((enablePopup) && (event->button() == Qt::LeftButton)) {
        QString cmd, text;
        TaskResult process;

        // kernel
        cmd = QString("uname -rsm");
        if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
        process = runTask(cmd);
        if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
        if (process.exitCode != 0)
            if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
        text += QString("Kernel: %1\n").arg(QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed());
        // hostname
        cmd = QString("uname -n");
        if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
        process = runTask(cmd);
        if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
        if (process.exitCode != 0)
            if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
        text += QString("Hostname: %1\n").arg(QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed());
        // whoami
        cmd = QString("whoami");
        if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
        process = runTask(cmd);
        if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
        if (process.exitCode != 0)
            if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
        text += QString("Whoami: %1\n").arg(QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed());
        // uptime
        cmd = QString("uptime");
        if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
        process = runTask(cmd);
        if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
        if (process.exitCode != 0)
            if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
        text += QString("Uptime: %1\n").arg(QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed());

        widget->sendNotification(QString("system"), text);
    }
    emit(QLabel::mousePressEvent(event));
}
