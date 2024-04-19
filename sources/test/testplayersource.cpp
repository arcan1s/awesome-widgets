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

#include "testplayersource.h"

#include <QtTest>

#include "awtestlibrary.h"
#include "playersource.h"


void TestPlayerSource::initTestCase()
{
    AWTestLibrary::init();
}


void TestPlayerSource::cleanupTestCase() {}


void TestPlayerSource::_test_sources(const PlayerSource *_source)
{
    QVERIFY(_source->sources().count() == 11);
}


void TestPlayerSource::test_buildString()
{
    auto randomString = AWTestLibrary::randomString(1, 40);
    auto str = PlayerSource::buildString("", randomString, 20);
    QCOMPARE(str.length(), 20);

    str = PlayerSource::buildString(str, randomString, 20);
    QCOMPARE(str.length(), 20);

    str = PlayerSource::buildString("", AWTestLibrary::randomString(1, 10), 20);
    QCOMPARE(str.length(), 20);
}


void TestPlayerSource::test_stripString()
{
    auto str = PlayerSource::buildString("", AWTestLibrary::randomString(1, 40), 20);
    QCOMPARE(str.length(), 20);

    str = PlayerSource::buildString("", AWTestLibrary::randomString(1, 10), 20);
    QCOMPARE(str.length(), 20);
}


void TestPlayerSource::test_autoMpris()
{
    auto source = new PlayerSource(this, "mpris", mpdAddress, mpdPort, "auto", 10);

    auto empty = source->getAutoMpris().isEmpty();
    source->deleteLater();

    if (empty)
        qWarning("No MPRIS found, manual check required");
    else
        QVERIFY(!empty);
}


void TestPlayerSource::test_mpd()
{
    auto source = new PlayerSource(this, "mpd", mpdAddress, mpdPort, "auto", 10);
    _test_sources(source);

    // init spy
    QSignalSpy spy(source, SIGNAL(dataReceived(const QVariantHash &)));
    auto firstValue = source->data("title");
    if (!source->isMpdSocketConnected())
        QSKIP("No mpd found");

    // check values
    QVERIFY(spy.wait(5000));
    auto arguments = spy.takeFirst();
    auto secondValue = arguments.at(0).toHash();

    // actually nothing to test here just print warning if no information found
    if (secondValue["title"].toString() == "unknown")
        QSKIP("No mpd found");

    QVERIFY(secondValue["progress"].toInt() < secondValue["duration"].toInt());

    source->deleteLater();
}


void TestPlayerSource::test_mpris()
{
    auto source = new PlayerSource(this, "mpris", mpdAddress, mpdPort, "auto", 10);
    _test_sources(source);

    auto value = source->data("title").toString();
    auto progress = source->data("progress").toInt();
    auto duration = source->data("duration").toInt();

    // actually nothing to test here just print warning if no information found
    if (value == "unknown")
        QSKIP("No mpris found");

    QVERIFY(progress < duration);

    source->deleteLater();
}


QTEST_MAIN(TestPlayerSource);
