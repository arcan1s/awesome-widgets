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
#include "awtestlibrary.h"


void TestAWNoFormatter::initTestCase()
{
    AWTestLibrary::init();
    formatter = new AWNoFormatter(nullptr);
}


void TestAWNoFormatter::cleanupTestCase()
{
    delete formatter;
}


void TestAWNoFormatter::test_values() {}


void TestAWNoFormatter::test_conversion()
{
    // integer
    auto randomInt = AWTestLibrary::randomInt();
    QCOMPARE(formatter->convert(randomInt), QString::number(randomInt));
    // float
    QWARN("Float conversion isn't tested here due to possible rounding errors");
    // string
    auto randomString = AWTestLibrary::randomString();
    QCOMPARE(formatter->convert(randomString), randomString);
}


void TestAWNoFormatter::test_copy()
{
    auto newFormatter = formatter->copy("/dev/null", 1);

    QCOMPARE(newFormatter->number(), 1);

    delete newFormatter;
}


QTEST_MAIN(TestAWNoFormatter);
