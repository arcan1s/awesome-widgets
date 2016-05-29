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


#include "awtestlibrary.h"

#include <QSet>


char AWTestLibrary::randomChar()
{
    return 'A' + (rand() % static_cast<int>('Z' - 'A'));
}


double AWTestLibrary::randomDouble()
{
    return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}


int AWTestLibrary::randomInt(const int max)
{
    return rand() % max;
}


QString AWTestLibrary::randomString(const int max)
{
    QString output;

    int count = 1 + randomInt(max);
    for (int i = 0; i < count; i++)
        output += QChar(randomChar());

    return output;
}


QStringList AWTestLibrary::randomStringList(const int max)
{
    QStringList output;

    int count = 1 + randomInt(max);
    for (int i = 0; i < count; i++)
        output.append(randomString());

    return output;
}


QStringList AWTestLibrary::randomSelect(const QStringList available)
{
    QSet<QString> output;

    int count = 1 + randomInt(available.count());
    for (int i = 0; i < count; i++) {
        int index = randomInt(available.count());
        output << available.at(index);
    }

    return output.toList();
}
