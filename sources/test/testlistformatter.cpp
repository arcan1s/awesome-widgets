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

#include "testlistformatter.h"

#include <QtTest>

#include "awlistformatter.h"
#include "awtestlibrary.h"


void TestAWListFormatter::initTestCase()
{
    AWTestLibrary::init();
    separator = AWTestLibrary::randomString(9, 10);

    formatter = new AWListFormatter(this);
    formatter->setSeparator(separator);
}


void TestAWListFormatter::cleanupTestCase() {}


void TestAWListFormatter::test_values()
{
    QCOMPARE(formatter->filter(), QString());
    QCOMPARE(formatter->separator(), separator);
    QCOMPARE(formatter->isSorted(), false);
}


void TestAWListFormatter::test_conversion()
{
    auto value = AWTestLibrary::randomStringList();
    QCOMPARE(formatter->convert(value), value.join(separator));
}


void TestAWListFormatter::test_sorted()
{
    formatter->setSorted(true);
    auto value = AWTestLibrary::randomStringList();
    auto received = formatter->convert(value);

    value.sort();
    QCOMPARE(received, value.join(separator));
}


void TestAWListFormatter::test_filter()
{
    auto value = AWTestLibrary::randomStringList();
    auto filters = AWTestLibrary::randomSelect(value);
    value.sort();
    formatter->setFilter(QString("(^%1$)").arg(filters.join("$|^")));

    QCOMPARE(formatter->convert(value).split(separator).count(), filters.count());
}


void TestAWListFormatter::test_copy()
{
    auto newFormatter = formatter->copy("/dev/null", 1);

    QCOMPARE(newFormatter->number(), 1);
    QCOMPARE(newFormatter->filter(), formatter->filter());
    QCOMPARE(newFormatter->separator(), formatter->separator());
    QCOMPARE(newFormatter->isSorted(), formatter->isSorted());

    newFormatter->deleteLater();
}


QTEST_MAIN(TestAWListFormatter);
