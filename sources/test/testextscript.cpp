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


#include "testextscript.h"

#include <QtTest>

#include "awtestlibrary.h"
#include "extscript.h"


void TestExtScript::initTestCase()
{
    AWTestLibrary::init();
    randomString = AWTestLibrary::randomString();

    extScript = new ExtScript(nullptr);
    extScript->setInterval(1);
    extScript->setExecutable(randomString);
    extScript->setNumber(0);
    extScript->setRedirect(ExtScript::Redirect::stderr2stdout);
    extScript->setPrefix(QString("echo"));

    extScript->run();
}


void TestExtScript::cleanupTestCase()
{
    delete extScript;
}


void TestExtScript::test_values()
{
    QCOMPARE(extScript->interval(), 1);
    QCOMPARE(extScript->number(), 0);
    QCOMPARE(extScript->executable(), randomString);
    QCOMPARE(extScript->strRedirect(), QString("stderr2stdout"));
    QCOMPARE(extScript->prefix(), QString("echo"));
}


void TestExtScript::test_run()
{
    // init spy
    QSignalSpy spy(extScript, SIGNAL(dataReceived(const QVariantHash &)));
    QVariantHash firstValue = extScript->run();

    // check values
    QVERIFY(spy.wait(5000));
    QList<QVariant> arguments = spy.takeFirst();

    QCOMPARE(firstValue[extScript->tag(QString("custom"))].toString(),
             QString(""));
    QCOMPARE(
        arguments.at(0).toHash()[extScript->tag(QString("custom"))].toString(),
        QString("\n%1").arg(randomString));
}


void TestExtScript::test_filters()
{
    if (extScript->jsonFiltersFile().isEmpty())
        QSKIP("No json filters found for scripts, skip fitlers test");

    extScript->setFilters(QStringList() << QString("newline"));
    // init spy
    QSignalSpy spy(extScript, SIGNAL(dataReceived(const QVariantHash &)));
    extScript->run();

    // check values
    QVERIFY(spy.wait(5000));
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(
        arguments.at(0).toHash()[extScript->tag(QString("custom"))].toString(),
        QString("<br>%1").arg(randomString));
}


void TestExtScript::test_copy()
{
    ExtScript *newExtScript = extScript->copy(QString("/dev/null"), 1);

    QCOMPARE(newExtScript->interval(), extScript->interval());
    QCOMPARE(newExtScript->executable(), extScript->executable());
    QCOMPARE(newExtScript->strRedirect(), extScript->strRedirect());
    QCOMPARE(newExtScript->prefix(), extScript->prefix());
    QCOMPARE(newExtScript->filters(), extScript->filters());
    QCOMPARE(newExtScript->number(), 1);

    delete newExtScript;
}


QTEST_MAIN(TestExtScript);
