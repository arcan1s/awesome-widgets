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


#include "testabstractformatter.h"

#include <QtTest>

#include "awnoformatter.h"
#include "awtestlibrary.h"


void TestAbstractFormatter::initTestCase()
{
    AWTestLibrary::init();
    formatter = new AWNoFormatter(nullptr);
}


void TestAbstractFormatter::cleanupTestCase()
{
    delete formatter;
}


void TestAbstractFormatter::test_values()
{
}


void TestAbstractFormatter::test_type()
{
    QString type = AWTestLibrary::randomString();
    QEXPECT_FAIL("", "Will fail because of invalid format", Continue);
    formatter->setStrType(type);
    QCOMPARE(formatter->strType(), type);

    formatter->setStrType(QString("NoFormat"));
    QCOMPARE(formatter->strType(), QString("NoFormat"));
}


void TestAbstractFormatter::test_copy()
{
    AWNoFormatter *newFormatter = formatter->copy(QString("/dev/null"), 1);

    QCOMPARE(newFormatter->type(), formatter->type());
    QCOMPARE(newFormatter->name(), formatter->name());

    delete newFormatter;
}


QTEST_MAIN(TestAbstractFormatter);
