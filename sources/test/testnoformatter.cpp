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


#include "testnoformatter.h"

#include <QtTest>

#include "awnoformatter.h"


void TestAWNoFormatter::initTestCase()
{
    formatter = new AWNoFormatter(nullptr);
}


void TestAWNoFormatter::cleanupTestCase()
{
    delete formatter;
}


void TestAWNoFormatter::test_values()
{
}


void TestAWNoFormatter::test_conversion()
{
    // integer
    int randomInt = rand();
    QCOMPARE(formatter->convert(randomInt), QString::number(randomInt));
    // float
    QWARN("Float conversion isn't tested here due to possible rounding errors");
    // string
    QString randomString = generateRandomString();
    QCOMPARE(formatter->convert(randomString), randomString);
}


void TestAWNoFormatter::test_copy()
{
    AWNoFormatter *newFormatter = formatter->copy(QString("/dev/null"), 1);

    QCOMPARE(newFormatter->number(), 1);

    delete newFormatter;
}


QString TestAWNoFormatter::generateRandomString()
{
    QString string;

    int diff = 'Z' - 'A';
    int count = rand() % 100 + 1;
    for (int i = 0; i < count; i++) {
        char c = 'A' + (rand() % diff);
        string += QChar(c);
    }

    return string;
}


QTEST_MAIN(TestAWNoFormatter);
