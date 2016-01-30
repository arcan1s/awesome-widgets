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


#ifndef AWPATTERNFUNCTIONS_H
#define AWPATTERNFUNCTIONS_H

#include <QString>
#include <QVariant>


namespace AWPatternFunctions
{
QString expandTemplates(QString code);
QVariantList findFunctionCalls(const QString function, const QString code);
QString insertKeyCount(QString code, const QStringList keys);
QString insertKeyNames(QString code, const QStringList keys);
QString insertKeys(QString code, const QStringList keys);
};


#endif /* AWPATTERNFUNCTIONS_H */
