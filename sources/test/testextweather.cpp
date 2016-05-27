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


#include "testextweather.h"

#include <QtTest>

#include "extweather.h"


void TestExtWeather::initTestCase()
{
    extWeather = new ExtWeather(nullptr);
    extWeather->setInterval(1);
    extWeather->setCity(city);
    extWeather->setCountry(country);
    extWeather->setNumber(0);

    extWeather->run();
}


void TestExtWeather::cleanupTestCase()
{
    delete extWeather;
}


void TestExtWeather::test_values()
{
    QCOMPARE(extWeather->interval(), 1);
    QCOMPARE(extWeather->number(), 0);
    QCOMPARE(extWeather->city(), city);
    QCOMPARE(extWeather->country(), country);
}


void TestExtWeather::test_run()
{
    // init spy
    QSignalSpy spy(extWeather, SIGNAL(dataReceived(const QVariantHash &)));
    QVariantHash firstValue = extWeather->run();

    // check values
    QVERIFY(spy.wait(5000));
    QVariantHash arguments = spy.takeFirst().at(0).toHash();
    QEXPECT_FAIL("", "WeatherID should not be 0", Continue);
    QCOMPARE(arguments[extWeather->tag(QString("weatherId"))].toInt(), 0);
    QVERIFY((arguments[extWeather->tag(QString("humidity"))].toInt()
             > humidity.first)
            && (arguments[extWeather->tag(QString("humidity"))].toInt()
                < humidity.second));
    QEXPECT_FAIL("", "https://yahoo.uservoice.com/forums/207813-us-weather/"
                     "suggestions/14209233-invalid-pressure-calculation",
                 Continue);
    QVERIFY((arguments[extWeather->tag(QString("pressure"))].toFloat()
             > pressure.first)
            && (arguments[extWeather->tag(QString("pressure"))].toInt()
                < pressure.second));
    QVERIFY((arguments[extWeather->tag(QString("temperature"))].toFloat()
             > temp.first)
            && (arguments[extWeather->tag(QString("temperature"))].toInt()
                < temp.second));
    // image should be only one symbol here
    QCOMPARE(arguments[extWeather->tag(QString("weather"))].toString().count(),
             1);
}


void TestExtWeather::test_ts()
{
    extWeather->setTs(1);
    // init spy
    QSignalSpy spy(extWeather, SIGNAL(dataReceived(const QVariantHash &)));
    QVariantHash firstValue = extWeather->run();

    // check values
    QVERIFY(spy.wait(5000));
    QVariantHash arguments = spy.takeFirst().at(0).toHash();
    QEXPECT_FAIL("", "WeatherID should not be 0", Continue);
    QCOMPARE(arguments[extWeather->tag(QString("weatherId"))].toInt(), 0);
    QCOMPARE(arguments[extWeather->tag(QString("humidity"))].toInt(), 0);
    QCOMPARE(arguments[extWeather->tag(QString("pressure"))].toFloat(), 0.0f);
    QVERIFY((arguments[extWeather->tag(QString("temperature"))].toFloat()
             > temp.first)
            && (arguments[extWeather->tag(QString("temperature"))].toInt()
                < temp.second));
    // image should be only one symbol here
    QCOMPARE(arguments[extWeather->tag(QString("weather"))].toString().count(),
             1);
}


void TestExtWeather::test_image()
{
    extWeather->setImage(true);
    // init spy
    QSignalSpy spy(extWeather, SIGNAL(dataReceived(const QVariantHash &)));
    QVariantHash firstValue = extWeather->run();

    // check values
    QVERIFY(spy.wait(5000));
    QVariantHash arguments = spy.takeFirst().at(0).toHash();
    QVERIFY(
        arguments[extWeather->tag(QString("weather"))].toString().startsWith(
            QString("<img")));
}


void TestExtWeather::test_copy()
{
    ExtWeather *newExtWeather = extWeather->copy(QString("/dev/null"), 1);

    QCOMPARE(newExtWeather->interval(), extWeather->interval());
    QCOMPARE(newExtWeather->city(), extWeather->city());
    QCOMPARE(newExtWeather->country(), extWeather->country());
    QCOMPARE(newExtWeather->ts(), extWeather->ts());
    QCOMPARE(newExtWeather->image(), extWeather->image());
    QCOMPARE(newExtWeather->number(), 1);

    delete newExtWeather;
}


QTEST_MAIN(TestExtWeather);
