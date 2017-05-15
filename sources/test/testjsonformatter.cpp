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


#include "testjsonformatter.h"

#include <QtTest>

#include "awjsonformatter.h"
#include "awtestlibrary.h"


void TestAWJsonFormatter::initTestCase()
{
    AWTestLibrary::init();
    formatter = new AWJsonFormatter(nullptr);

    generate();
    formatter->setPath(path);
}


void TestAWJsonFormatter::cleanupTestCase()
{
    delete formatter;
}


void TestAWJsonFormatter::test_values()
{
    QCOMPARE(formatter->path(), path);
}


void TestAWJsonFormatter::test_conversion()
{
    QCOMPARE(formatter->convert(json), value);
}


void TestAWJsonFormatter::test_copy()
{
    AWJsonFormatter *newFormatter = formatter->copy("/dev/null", 1);

    QCOMPARE(newFormatter->path(), formatter->path());
    QCOMPARE(newFormatter->number(), 1);

    delete newFormatter;
}


void TestAWJsonFormatter::generate()
{
    value = AWTestLibrary::randomString();

    QVariantMap first;
    QString firstKey = AWTestLibrary::randomString();
    first[firstKey] = value;

    int listCount = AWTestLibrary::randomInt(5) + 1;
    int validCount = AWTestLibrary::randomInt(listCount);
    QVariantList second;
    for (int i = 0; i < listCount; i++) {
        if (i == validCount) {
            second.append(first);
        } else {
            QString key = AWTestLibrary::randomString();
            QString val = AWTestLibrary::randomString();
            QVariantMap dict;
            dict[key] = val;
            second.append(dict);
        }
    }

    QString thirdKey = AWTestLibrary::randomString();
    QVariantMap output;
    output[thirdKey] = second;

    json = output;
    path = QString("%1.%2.%3").arg(thirdKey).arg(validCount).arg(firstKey);
}


QTEST_MAIN(TestAWJsonFormatter);
