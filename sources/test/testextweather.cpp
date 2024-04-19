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

#include "awtestlibrary.h"
#include "extweather.h"


void TestExtWeather::initTestCase()
{
    AWTestLibrary::init();
    extWeather = new ExtWeather(this);
    extWeather->setInterval(1);
    extWeather->setCity(city);
    extWeather->setCountry(country);
    extWeather->setNumber(0);
    extWeather->setProvider(ExtWeather::Provider::OWM);

    extWeather->run();
}


void TestExtWeather::cleanupTestCase() {}


void TestExtWeather::test_values()
{
    QCOMPARE(extWeather->interval(), 1);
    QCOMPARE(extWeather->number(), 0);
    QCOMPARE(extWeather->city(), city);
    QCOMPARE(extWeather->country(), country);
    QCOMPARE(extWeather->provider(), ExtWeather::Provider::OWM);
}


void TestExtWeather::test_runOWM()
{
    run();
}


void TestExtWeather::test_ts()
{
    extWeather->setTs(1);
    run();
}


void TestExtWeather::test_image()
{
    if (extWeather->jsonMapFile().isEmpty())
        QSKIP("No json map found for weather, skip image test");

    extWeather->setImage(true);
    // init spy
    QSignalSpy spy(extWeather, SIGNAL(dataReceived(const QVariantHash &)));
    auto firstValue = extWeather->run();

    // check values
    QVERIFY(spy.wait(5000));
    auto arguments = spy.takeFirst().at(0).toHash();
    QVERIFY(arguments[extWeather->tag("weather")].toString().startsWith("<img"));
}


void TestExtWeather::test_copy()
{
    auto newExtWeather = extWeather->copy("/dev/null", 1);

    QCOMPARE(newExtWeather->interval(), extWeather->interval());
    QCOMPARE(newExtWeather->city(), extWeather->city());
    QCOMPARE(newExtWeather->country(), extWeather->country());
    QCOMPARE(newExtWeather->ts(), extWeather->ts());
    QCOMPARE(newExtWeather->image(), extWeather->image());
    QCOMPARE(newExtWeather->provider(), extWeather->provider());
    QCOMPARE(newExtWeather->number(), 1);

    newExtWeather->deleteLater();
}


void TestExtWeather::run()
{
    // init spy
    QSignalSpy spy(extWeather, SIGNAL(dataReceived(const QVariantHash &)));
    auto firstValue = extWeather->run();

    // check values
    QVERIFY(spy.wait(5000));
    auto arguments = spy.takeFirst().at(0).toHash();
    QEXPECT_FAIL("", "WeatherID should not be 0", Continue);
    QCOMPARE(arguments[extWeather->tag("weatherId")].toInt(), 0);
    QVERIFY((arguments[extWeather->tag("humidity")].toInt() >= humidity.first)
            && (arguments[extWeather->tag("humidity")].toInt() <= humidity.second));
    QVERIFY((arguments[extWeather->tag("pressure")].toInt() > pressure.first)
            && (arguments[extWeather->tag("pressure")].toInt() < pressure.second));
    QVERIFY((arguments[extWeather->tag("temperature")].toDouble() > temp.first)
            && (arguments[extWeather->tag("temperature")].toDouble() < temp.second));
    // image should be only one symbol here
    if (extWeather->jsonMapFile().isEmpty())
        QSKIP("No json map found for weather, skip image test");
    QCOMPARE(arguments[extWeather->tag("weather")].toString().length(), 1);
}


QTEST_MAIN(TestExtWeather);
