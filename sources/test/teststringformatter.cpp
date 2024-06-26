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

#include "teststringformatter.h"

#include <QtTest>

#include "awstringformatter.h"
#include "awtestlibrary.h"


void TestAWStringFormatter::initTestCase()
{
    AWTestLibrary::init();
    formatter = new AWStringFormatter(this);
}


void TestAWStringFormatter::cleanupTestCase() {}


void TestAWStringFormatter::test_values() {}


void TestAWStringFormatter::test_count()
{
    // assign
    auto count = 10 + AWTestLibrary::randomInt();
    formatter->setCount(count);
    QCOMPARE(formatter->count(), count);

    // test
    auto testString = AWTestLibrary::randomString();
    while (testString.length() > count)
        testString = AWTestLibrary::randomString();
    auto output = formatter->convert(testString);
    QCOMPARE(output.length(), count);

    // reset
    formatter->setCount(0);
}


void TestAWStringFormatter::test_fillChar()
{
    // assign
    auto c = AWTestLibrary::randomChar();
    formatter->setFillChar(c);
    QCOMPARE(formatter->fillChar(), QChar(c));
    formatter->setCount(101);

    // test
    auto output = formatter->convert(AWTestLibrary::randomString());
    QVERIFY(output.startsWith(c));

    // reset
    formatter->setFillChar(QChar());
    formatter->setCount(0);
}


void TestAWStringFormatter::test_forceWidth()
{
    // assign
    auto count = AWTestLibrary::randomInt();
    formatter->setForceWidth(true);
    formatter->setCount(count);
    QCOMPARE(formatter->forceWidth(), true);

    // test
    auto output = formatter->convert(AWTestLibrary::randomString());
    QCOMPARE(output.length(), count);

    // reset
    formatter->setForceWidth(false);
    formatter->setCount(0);
}


void TestAWStringFormatter::test_copy()
{
    doRandom();
    auto newFormatter = formatter->copy("/dev/null", 1);

    QCOMPARE(newFormatter->count(), formatter->count());
    QCOMPARE(newFormatter->fillChar(), formatter->fillChar());
    QCOMPARE(newFormatter->forceWidth(), formatter->forceWidth());
    QCOMPARE(newFormatter->number(), 1);

    newFormatter->deleteLater();
}


void TestAWStringFormatter::doRandom()
{
    formatter->setCount(AWTestLibrary::randomInt());
    formatter->setFillChar(AWTestLibrary::randomChar());
    formatter->setForceWidth(AWTestLibrary::randomInt() % 2);
}


QTEST_MAIN(TestAWStringFormatter);
