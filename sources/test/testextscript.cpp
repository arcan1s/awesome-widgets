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

#include "extscript.h"
#include "version.h"


void TestExtScript::initTestCase()
{
    extScript = new ExtScript(nullptr);
    extScript->setInterval(1);
    extScript->setExecutable(QString("hello world"));
    extScript->setRedirect(ExtScript::Redirect::stderr2stdout);
    extScript->setPrefix(QString("echo"));

    while (true)
        qDebug() <<extScript->run();
}


void TestExtScript::cleanupTestCase()
{
    delete extScript;
}


void TestExtScript::test_values()
{
    QCOMPARE(extScript->interval(), 1);
    QCOMPARE(extScript->executable(), QString("hello world"));
    QCOMPARE(extScript->prefix(), QString("echo"));
}


void TestExtScript::test_firstRun()
{
    QVariantHash firstValue = extScript->run();
    qDebug() << firstValue;
    QTest::qSleep(20000);
    QCOMPARE(firstValue[extScript->tag(QString("custom"))].toString(), QString(""));
    QTest::qSleep(20000);
}


void TestExtScript::test_secondRun()
{
    QVariantHash secondValue = extScript->run();
    qDebug() << secondValue;
    QCOMPARE(secondValue[extScript->tag(QString("custom"))].toString(), QString("hello world"));
}


QTEST_MAIN(TestExtScript);
