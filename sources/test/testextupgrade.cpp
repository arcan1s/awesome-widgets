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


#include "testextupgrade.h"

#include <QtTest>

#include "awtestlibrary.h"
#include "extupgrade.h"


void TestExtUpgrade::initTestCase()
{
    randomStrings = AWTestLibrary::randomStringList();
    cmd = QString("echo -e '%1'").arg(randomStrings.join(QString("\n")));

    extUpgrade = new ExtUpgrade(nullptr);
    extUpgrade->setInterval(1);
    extUpgrade->setExecutable(cmd);
    extUpgrade->setNumber(0);

    extUpgrade->run();
}


void TestExtUpgrade::cleanupTestCase()
{
    delete extUpgrade;
}


void TestExtUpgrade::test_values()
{
    QCOMPARE(extUpgrade->interval(), 1);
    QCOMPARE(extUpgrade->number(), 0);
    QCOMPARE(extUpgrade->executable(), cmd);
}


void TestExtUpgrade::test_run()
{
    // init spy
    QSignalSpy spy(extUpgrade, SIGNAL(dataReceived(const QVariantHash &)));
    QVariantHash firstValue = extUpgrade->run();
    QCOMPARE(firstValue[extUpgrade->tag(QString("pkgcount"))].toInt(), 0);

    // check values
    QVERIFY(spy.wait(5000));
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(
        arguments.at(0).toHash()[extUpgrade->tag(QString("pkgcount"))].toInt(),
        randomStrings.count());
}


void TestExtUpgrade::test_null()
{
    int null = AWTestLibrary::randomInt(randomStrings.count());
    extUpgrade->setNull(null);
    QSignalSpy spy(extUpgrade, SIGNAL(dataReceived(const QVariantHash &)));
    extUpgrade->run();

    // check values
    QVERIFY(spy.wait(5000));
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(
        arguments.at(0).toHash()[extUpgrade->tag(QString("pkgcount"))].toInt(),
        randomStrings.count() - null);
}


void TestExtUpgrade::test_filter()
{
    QSet<QString> filters;
    int count = AWTestLibrary::randomInt(randomStrings.count());
    for (int i = 0; i < count; i++) {
        int index = AWTestLibrary::randomInt(randomStrings.count());
        filters << randomStrings.at(index);
    }

    extUpgrade->setFilter(
        QString("(^%1$)").arg(filters.toList().join(QString("$|^"))));
    // init spy
    QSignalSpy spy(extUpgrade, SIGNAL(dataReceived(const QVariantHash &)));
    extUpgrade->run();

    // check values
    QVERIFY(spy.wait(5000));
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(
        arguments.at(0).toHash()[extUpgrade->tag(QString("pkgcount"))].toInt(),
        filters.count());
}


void TestExtUpgrade::test_copy()
{
    ExtUpgrade *newExtUpgrade = extUpgrade->copy(QString("/dev/null"), 1);

    QCOMPARE(newExtUpgrade->interval(), extUpgrade->interval());
    QCOMPARE(newExtUpgrade->executable(), extUpgrade->executable());
    QCOMPARE(newExtUpgrade->filter(), extUpgrade->filter());
    QCOMPARE(newExtUpgrade->null(), extUpgrade->null());
    QCOMPARE(newExtUpgrade->number(), 1);

    delete newExtUpgrade;
}


QTEST_MAIN(TestExtUpgrade);
