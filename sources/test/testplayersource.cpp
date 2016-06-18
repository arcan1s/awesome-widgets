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
}


void TestPlayerSource::cleanupTestCase()
{
}


void TestPlayerSource::_test_sources(const PlayerSource *source)
{
    QVERIFY(source->sources().count() == 11);
}


void TestPlayerSource::test_buildString()
{
    QString randomString = AWTestLibrary::randomString(40);
    QString str = PlayerSource::buildString(QString(), randomString, 20);
    QCOMPARE(str.count(), 20);

    str = PlayerSource::buildString(str, randomString, 20);
    QCOMPARE(str.count(), 20);
    QCOMPARE(randomString.indexOf(str), 1);

    str = PlayerSource::buildString(QString(), AWTestLibrary::randomString(10),
                                    20);
    QCOMPARE(str.count(), 20);
}


void TestPlayerSource::test_stripString()
{
    QString str = PlayerSource::buildString(
        QString(), AWTestLibrary::randomString(40), 20);
    QCOMPARE(str.count(), 20);

    str = PlayerSource::buildString(QString(), AWTestLibrary::randomString(10),
                                    20);
    QCOMPARE(str.count(), 20);
}


void TestPlayerSource::test_autoMpris()
{
    QStringList args(QStringList() << QString("mpris") << mpdAddress
                                   << QString::number(mpdPort)
                                   << QString("auto") << QString::number(10));
    PlayerSource *source = new PlayerSource(this, args);

    QWARN("Will fail if no MPRIS supported player is run");
    QVERIFY(!source->getAutoMpris().isEmpty());
}


void TestPlayerSource::test_mpd()
{
    QStringList args(QStringList() << QString("mpd") << mpdAddress
                                   << QString::number(mpdPort)
                                   << QString("auto") << QString::number(10));
    PlayerSource *source = new PlayerSource(this, args);
    _test_sources(source);

    // init spy
    QSignalSpy spy(source, SIGNAL(dataReceived(const QVariantHash &)));
    QVariant firstValue = source->data(QString("player/title"));

    // check values
    QVERIFY(spy.wait(5000));
    QList<QVariant> arguments = spy.takeFirst();
    QVariantHash secondValue = arguments.at(0).toHash();

    // actually nothing to test here just print warning if no information found
    if (secondValue[QString("player/title")].toString() == QString("unknown")) {
        QWARN("No mpd found");
    } else {
        QVERIFY(secondValue[QString("player/progress")].toInt()
                < secondValue[QString("player/duration")].toInt());
    }
}


void TestPlayerSource::test_mpris()
{
    QStringList args(QStringList() << QString("mpris") << mpdAddress
                                   << QString::number(mpdPort)
                                   << QString("auto") << QString::number(10));
    PlayerSource *source = new PlayerSource(this, args);
    _test_sources(source);

    QString value = source->data(QString("player/title")).toString();
    int progress = source->data(QString("player/progress")).toInt();
    int duration = source->data(QString("player/duration")).toInt();

    // actually nothing to test here just print warning if no information found
    if (value == QString("unknown")) {
        QWARN("No mpris found");
    } else {
        QVERIFY(progress < duration);
    }
}


QTEST_MAIN(TestPlayerSource);
