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

#include "testextquotes.h"

#include <QtTest>

#include "awtestlibrary.h"
#include "extquotes.h"


void TestExtQuotes::initTestCase()
{
    AWTestLibrary::init();
    extQuotes = new ExtQuotes(nullptr);
    extQuotes->setInterval(1);
    extQuotes->setTicker(ticker);
    extQuotes->setNumber(0);

    extQuotes->run();
}


void TestExtQuotes::cleanupTestCase()
{
    delete extQuotes;
}


void TestExtQuotes::test_values()
{
    QCOMPARE(extQuotes->interval(), 1);
    QCOMPARE(extQuotes->number(), 0);
    QCOMPARE(extQuotes->ticker(), ticker);
}


void TestExtQuotes::test_run()
{
    // init spy
    QSignalSpy spy(extQuotes, SIGNAL(dataReceived(const QVariantHash &)));
    auto firstValue = extQuotes->run();

    // check values
    QVERIFY(spy.wait(5000));
    auto arguments = spy.takeFirst();
    for (auto &type : types)
        cache[type] = arguments.at(0).toHash()[extQuotes->tag(type)];

    for (auto &type : types) {
        QCOMPARE(firstValue[extQuotes->tag(type)].toDouble(), 0.0);
        QVERIFY((cache[type].toDouble() >= price.first) && (cache[type].toDouble() <= price.second));
    }
}


void TestExtQuotes::test_derivatives()
{
    // init spy
    QSignalSpy spy(extQuotes, SIGNAL(dataReceived(const QVariantHash &)));
    auto firstValue = extQuotes->run();

    // check values
    QVERIFY(spy.wait(5000));
    auto arguments = spy.takeFirst();
    QVariantHash values;
    for (auto &type : types)
        values[type] = arguments.at(0).toHash()[extQuotes->tag(type)];

    for (auto &type : types) {
        QCOMPARE(arguments.at(0).toHash()[extQuotes->tag(QString("%1chg").arg(type))].toDouble(),
                 (values[type].toDouble() - cache[type].toDouble()));
    }
}


void TestExtQuotes::test_copy()
{
    auto newExtQuotes = extQuotes->copy("/dev/null", 1);

    QCOMPARE(newExtQuotes->interval(), extQuotes->interval());
    QCOMPARE(newExtQuotes->ticker(), extQuotes->ticker());
    QCOMPARE(newExtQuotes->number(), 1);

    delete newExtQuotes;
}


QTEST_MAIN(TestExtQuotes);
