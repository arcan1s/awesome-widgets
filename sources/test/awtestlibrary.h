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


#ifndef AWTESTLIBRARY_H
#define AWTESTLIBRARY_H

#include <QPair>
#include <QStringList>


namespace AWTestLibrary
{
void init();
bool isKWinActive();
char randomChar();
QPair<QString, QString> randomFilenames();
int randomInt(const int _max = 100);
QString randomString(const int _min = 1, const int _max = 100);
QStringList randomStringList(const int _max = 100);
QStringList randomSelect(const QStringList &_available);
} // namespace AWTestLibrary


#endif /* AWTESTLIBRARY_H */
