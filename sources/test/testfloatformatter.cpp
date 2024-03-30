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

#include "testfloatformatter.h"

#include <QRandomGenerator>
#include <QtTest>

#include "awfloatformatter.h"
#include "awtestlibrary.h"


void TestAWFloatFormatter::initTestCase()
{
    AWTestLibrary::init();
    formatter = new AWFloatFormatter(nullptr);
}


void TestAWFloatFormatter::cleanupTestCase()
{
    delete formatter;
}


void TestAWFloatFormatter::test_values() {}


void TestAWFloatFormatter::test_count()
{
    // assign
    auto count = 10 + AWTestLibrary::randomInt();
    formatter->setCount(count);
    QCOMPARE(formatter->count(), count);

    // test
    auto output = formatter->convert(QRandomGenerator::global()->generateDouble());
    QCOMPARE(output.length(), count);

    // reset
    formatter->setCount(0);
}


void TestAWFloatFormatter::test_fillChar()
{
    // assign
    auto c = AWTestLibrary::randomChar();
    formatter->setFillChar(c);
    QCOMPARE(formatter->fillChar(), QChar(c));
    formatter->setCount(101);

    // test
    auto output = formatter->convert(AWTestLibrary::randomInt());
    QVERIFY(output.startsWith(c));

    // reset
    formatter->setFillChar(QChar());
    formatter->setCount(0);
}


void TestAWFloatFormatter::test_forceWidth()
{
    // assign
    auto count = AWTestLibrary::randomInt(6);
    formatter->setForceWidth(true);
    formatter->setCount(count);
    QCOMPARE(formatter->forceWidth(), true);

    // test
    auto output = formatter->convert(QRandomGenerator::global()->generateDouble());
    QCOMPARE(output.length(), count);

    // reset
    formatter->setForceWidth(false);
    formatter->setCount(0);
}


void TestAWFloatFormatter::test_format()
{
    // assign
    QWARN("Lets assing 'z' formatter, it should cause a warning");
    formatter->setFormat('z');
    QCOMPARE(formatter->format(), 'f');
    formatter->setFormat('e');
    QCOMPARE(formatter->format(), 'e');

    // test
    auto output = formatter->convert(QRandomGenerator::global()->generateDouble());
    QVERIFY(output.contains('e'));

    // reset
    formatter->setFormat('f');
}


void TestAWFloatFormatter::test_precision()
{
    // assign
    auto precision = 1 + AWTestLibrary::randomInt(5);
    formatter->setPrecision(precision);
    QCOMPARE(formatter->precision(), precision);

    // test
    auto output = formatter->convert(QRandomGenerator::global()->generateDouble());
    output.remove("0.");
    QCOMPARE(output.length(), precision);

    // reset
    formatter->setPrecision(-1);
}


void TestAWFloatFormatter::test_multiplier()
{
    formatter->setPrecision(6);

    // assign
    auto multiplier = QRandomGenerator::global()->generateDouble();
    formatter->setMultiplier(multiplier);
    QCOMPARE(formatter->multiplier(), multiplier);

    // test
    auto value = QRandomGenerator::global()->generateDouble();
    QCOMPARE(formatter->convert(value), QString::number(value * multiplier, 'f', 6));

    // reset
    formatter->setMultiplier(1.0);
}


void TestAWFloatFormatter::test_summand()
{
    // assign
    auto summand = QRandomGenerator::global()->generateDouble();
    formatter->setSummand(summand);
    QCOMPARE(formatter->summand(), summand);

    // test
    auto value = QRandomGenerator::global()->generateDouble();
    QCOMPARE(formatter->convert(value), QString::number(value + summand, 'f', 6));

    // reset
    formatter->setSummand(1.0);
}


void TestAWFloatFormatter::test_copy()
{
    doRandom();
    auto newFormatter = formatter->copy("/dev/null", 1);

    QCOMPARE(newFormatter->count(), formatter->count());
    QCOMPARE(newFormatter->fillChar(), formatter->fillChar());
    QCOMPARE(newFormatter->forceWidth(), formatter->forceWidth());
    QCOMPARE(newFormatter->format(), formatter->format());
    QCOMPARE(newFormatter->multiplier(), formatter->multiplier());
    QCOMPARE(newFormatter->precision(), formatter->precision());
    QCOMPARE(newFormatter->summand(), formatter->summand());
    QCOMPARE(newFormatter->number(), 1);

    delete newFormatter;
}


void TestAWFloatFormatter::doRandom()
{
    formatter->setCount(AWTestLibrary::randomInt());
    formatter->setFillChar(AWTestLibrary::randomChar());
    formatter->setForceWidth(AWTestLibrary::randomInt() % 2);
    formatter->setFormat(AWTestLibrary::randomChar());
    formatter->setMultiplier(QRandomGenerator::global()->generateDouble());
    formatter->setPrecision(AWTestLibrary::randomInt());
    formatter->setSummand(QRandomGenerator::global()->generateDouble());
}


QTEST_MAIN(TestAWFloatFormatter);
