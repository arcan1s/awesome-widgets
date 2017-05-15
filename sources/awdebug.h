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


#ifndef AWDEBUG_H
#define AWDEBUG_H

#include <QLoggingCategory>

#include "version.h"

namespace AWDebug
{
const char LOG_FORMAT[] = "[%{time "
                          "process}][%{if-debug}DD%{endif}%{if-info}II%{endif}%"
                          "{if-warning}WW%{endif}%{if-critical}CC%{endif}%{if-"
                          "fatal}FF%{endif}][%{category}][%{function}] "
                          "%{message}";

QString getAboutText(const QString &_type);
QStringList getBuildData();
}


Q_DECLARE_LOGGING_CATEGORY(LOG_AW)
Q_DECLARE_LOGGING_CATEGORY(LOG_DBUS)
Q_DECLARE_LOGGING_CATEGORY(LOG_DP)
Q_DECLARE_LOGGING_CATEGORY(LOG_ESM)
Q_DECLARE_LOGGING_CATEGORY(LOG_ESS)
Q_DECLARE_LOGGING_CATEGORY(LOG_LIB)


#endif /* AWDEBUG_H */
