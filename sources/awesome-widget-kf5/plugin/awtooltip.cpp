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

#include "awtooltip.h"

#include <QDebug>
#include <QProcessEnvironment>

#include <pdebug/pdebug.h>


AWToolTip::AWToolTip(QObject *parent,
                     const int maxCount)
    : QObject(parent),
      m_maxCount(maxCount)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));
}


AWToolTip::~AWToolTip()
{
    if (debug) qDebug() << PDEBUG;
}


void AWToolTip::setData (const QString source, const float value, const bool ac)
{
    if (debug) qDebug() << PDEBUG;

    if (data[source].count() == 0)
        data[source].append(0.0);
    else if (data[source].count() > m_maxCount)
        data[source].takeFirst();

    if (ac)
        data[source].append(value);
    else
        data[source].append(-value);
}
