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


#include "testscriptformatter.h"

#include <QtTest>

#include "awscriptformatter.h"
#include "awtestlibrary.h"


void TestAWScriptFormatter::initTestCase()
{
    formatter = new AWScriptFormatter(nullptr);
    formatter->setCode(fullCode);
    formatter->setAppendCode(false);
    formatter->setHasReturn(true);
}


void TestAWScriptFormatter::cleanupTestCase()
{
    delete formatter;
}


void TestAWScriptFormatter::test_values()
{
    QCOMPARE(formatter->code(), fullCode);
    QCOMPARE(formatter->appendCode(), false);
    QCOMPARE(formatter->hasReturn(), true);

    QCOMPARE(formatter->program(), fullCode);
}


void TestAWScriptFormatter::test_conversion()
{
    QString string = AWTestLibrary::randomString();
    QCOMPARE(formatter->convert(string), string);
}


void TestAWScriptFormatter::test_appendCode()
{
    // set
    formatter->setAppendCode(true);
    QCOMPARE(formatter->appendCode(), true);
    formatter->setCode(codeWithReturn);

    // test
    QCOMPARE(formatter->program(), fullCode);
    test_conversion();

    // reset
    formatter->setAppendCode(false);
}


void TestAWScriptFormatter::test_hasReturn()
{
    // set
    formatter->setHasReturn(false);
    QCOMPARE(formatter->hasReturn(), false);
    formatter->setCode(code);

    // test 1
    QEXPECT_FAIL("", "Should fail because appendCode set to false", Continue);
    QCOMPARE(formatter->program(), fullCode);

    // test 2
    formatter->setAppendCode(true);
    QCOMPARE(formatter->program(), fullCode);
    test_conversion();
}


void TestAWScriptFormatter::test_copy()
{
    AWScriptFormatter *newFormatter = formatter->copy(QString("/dev/null"), 1);

    QCOMPARE(newFormatter->appendCode(), formatter->appendCode());
    QCOMPARE(newFormatter->code(), formatter->code());
    QCOMPARE(newFormatter->hasReturn(), formatter->hasReturn());
    QCOMPARE(newFormatter->program(), formatter->program());
    QCOMPARE(newFormatter->number(), 1);

    delete newFormatter;
}


QTEST_MAIN(TestAWScriptFormatter);
