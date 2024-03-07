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
    QString randomString = AWTestLibrary::randomString(1, 40);
    QString str = PlayerSource::buildString("", randomString, 20);
    QCOMPARE(str.length(), 20);

    str = PlayerSource::buildString(str, randomString, 20);
    QCOMPARE(str.length(), 20);

    str = PlayerSource::buildString("", AWTestLibrary::randomString(1, 10), 20);
    QCOMPARE(str.length(), 20);
}


void TestPlayerSource::test_stripString()
{
    QString str = PlayerSource::buildString("", AWTestLibrary::randomString(1, 40), 20);
    QCOMPARE(str.length(), 20);

    str = PlayerSource::buildString("", AWTestLibrary::randomString(1, 10), 20);
    QCOMPARE(str.length(), 20);
}


void TestPlayerSource::test_autoMpris()
{
    QStringList args(QStringList() << "mpris" << mpdAddress << QString::number(mpdPort) << "auto"
                                   << QString::number(10));
    PlayerSource *source = new PlayerSource(this, args);

    bool empty = source->getAutoMpris().isEmpty();
    if (empty)
        QWARN("No MPRIS found, manual check required");
    else
        QVERIFY(!empty);
}


void TestPlayerSource::test_mpd()
{
    QStringList args(QStringList() << "mpd" << mpdAddress << QString::number(mpdPort) << "auto" << QString::number(10));
    PlayerSource *source = new PlayerSource(this, args);
    _test_sources(source);

    // init spy
    QSignalSpy spy(source, SIGNAL(dataReceived(const QVariantHash &)));
    QVariant firstValue = source->data("title");
    if (!source->isMpdSocketConnected())
        QSKIP("No mpd found");

    // check values
    QVERIFY(spy.wait(5000));
    QList<QVariant> arguments = spy.takeFirst();
    QVariantHash secondValue = arguments.at(0).toHash();

    // actually nothing to test here just print warning if no information found
    if (secondValue["title"].toString() == "unknown")
        QSKIP("No mpd found");

    QVERIFY(secondValue["progress"].toInt() < secondValue["duration"].toInt());
}


void TestPlayerSource::test_mpris()
{
    QStringList args(QStringList() << "mpris" << mpdAddress << QString::number(mpdPort) << "auto"
                                   << QString::number(10));
    PlayerSource *source = new PlayerSource(this, args);
    _test_sources(source);

    QString value = source->data("title").toString();
    int progress = source->data("progress").toInt();
    int duration = source->data("duration").toInt();

    // actually nothing to test here just print warning if no information found
    if (value == "unknown")
        QSKIP("No mpris found");

    QVERIFY(progress < duration);
}


QTEST_MAIN(TestPlayerSource);
