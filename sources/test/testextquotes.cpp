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

#include "extquotes.h"


void TestExtQuotes::initTestCase()
{
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
    QVariantHash firstValue = extQuotes->run();

    // check values
    QVERIFY(spy.wait(5000));
    QList<QVariant> arguments = spy.takeFirst();
    cache[QString("ask")]
        = arguments.at(0).toHash()[extQuotes->tag(QString("ask"))];
    cache[QString("bid")]
        = arguments.at(0).toHash()[extQuotes->tag(QString("bid"))];
    cache[QString("price")]
        = arguments.at(0).toHash()[extQuotes->tag(QString("price"))];

    QCOMPARE(firstValue[extQuotes->tag(QString("ask"))].toDouble(), 0.0);
    QCOMPARE(firstValue[extQuotes->tag(QString("bid"))].toDouble(), 0.0);
    QCOMPARE(firstValue[extQuotes->tag(QString("price"))].toDouble(), 0.0);
    for (auto type : types) {
        qDebug() << "Test type" << type;
        QVERIFY((cache[type].toDouble() > price.first)
                && (cache[type].toDouble() < price.second));
    }
}


void TestExtQuotes::test_derivatives()
{
    // init spy
    QSignalSpy spy(extQuotes, SIGNAL(dataReceived(const QVariantHash &)));
    QVariantHash firstValue = extQuotes->run();

    // check values
    QVERIFY(spy.wait(5000));
    QList<QVariant> arguments = spy.takeFirst();
    QVariantHash values;
    values[QString("ask")]
        = arguments.at(0).toHash()[extQuotes->tag(QString("ask"))];
    values[QString("bid")]
        = arguments.at(0).toHash()[extQuotes->tag(QString("bid"))];
    values[QString("price")]
        = arguments.at(0).toHash()[extQuotes->tag(QString("price"))];

    for (auto type : types) {
        qDebug() << "Test type" << type;
        QCOMPARE(arguments.at(0)
                     .toHash()[extQuotes->tag(QString("%1chg").arg(type))]
                     .toDouble(),
                 (values[type].toDouble() - cache[type].toDouble()));
        QWARN("Possible round error");
        QCOMPARE(arguments.at(0)
                     .toHash()[extQuotes->tag(QString("perc%1chg").arg(type))]
                     .toDouble(),
                 100.0 * (values[type].toDouble() - cache[type].toDouble())
                     / values[type].toDouble());
    }
}


void TestExtQuotes::test_copy()
{
    ExtQuotes *newExtQuotes = extQuotes->copy(QString("/dev/null"), 1);

    QCOMPARE(newExtQuotes->interval(), extQuotes->interval());
    QCOMPARE(newExtQuotes->ticker(), extQuotes->ticker());
    QCOMPARE(newExtQuotes->number(), 1);

    delete newExtQuotes;
}


QTEST_MAIN(TestExtQuotes);
