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


#include "testdatetimeformatter.h"

#include <QtTest>

#include "awdatetimeformatter.h"
#include "awtestlibrary.h"
#include "version.h"


void TestAWDateTimeFormatter::initTestCase()
{
    AWTestLibrary::init();
    format
        = AWTestLibrary::randomSelect(QString(TIME_KEYS).split(',')).join(' ');

    formatter = new AWDateTimeFormatter(nullptr);
    formatter->setFormat(format);
}


void TestAWDateTimeFormatter::cleanupTestCase()
{
    delete formatter;
}


void TestAWDateTimeFormatter::test_values()
{
    QCOMPARE(formatter->format(), format);
}


void TestAWDateTimeFormatter::test_conversion()
{
    QDateTime now = QDateTime::currentDateTime();
    QCOMPARE(formatter->convert(now), now.toString(format));
}


void TestAWDateTimeFormatter::test_copy()
{
    formatter->setTranslateString(false);
    AWDateTimeFormatter *newFormatter = formatter->copy("/dev/null", 1);

    QCOMPARE(newFormatter->format(), formatter->format());
    QCOMPARE(newFormatter->translateString(), formatter->translateString());
    QCOMPARE(newFormatter->number(), 1);

    delete newFormatter;
}


QTEST_MAIN(TestAWDateTimeFormatter);
