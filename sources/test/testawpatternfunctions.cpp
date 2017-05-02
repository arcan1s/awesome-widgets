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


#include "testawpatternfunctions.h"

#include <QtTest>

#include "awpatternfunctions.h"
#include "awtestlibrary.h"


void TestAWPatternFunctions::initTestCase()
{
    AWTestLibrary::init();
}


void TestAWPatternFunctions::cleanupTestCase()
{
}


void TestAWPatternFunctions::test_findFunctionCalls()
{
    QString name = QString("aw_%1").arg(AWTestLibrary::randomString(1, 10));
    QString code = AWTestLibrary::randomString(1, 20);
    QStringList args = AWTestLibrary::randomStringList(20);
    QString function = QString("$%1<%2>{{%3}}")
                           .arg(name)
                           .arg(args.join(QChar(',')))
                           .arg(code);

    QString pattern = AWTestLibrary::randomString() + function
                      + AWTestLibrary::randomString();

    QList<AWPatternFunctions::AWFunction> found
        = AWPatternFunctions::findFunctionCalls(name, pattern);
    QCOMPARE(found.count(), 1);

    QCOMPARE(found.at(0).args, args);
    QCOMPARE(found.at(0).body, code);
    QCOMPARE(found.at(0).what, function);
}


void TestAWPatternFunctions::test_findKeys()
{
    int count = AWTestLibrary::randomInt(200);
    QStringList allKeys;
    for (int i = 0; i < count; i++) {
        auto key = AWTestLibrary::randomString(1, 20);
        while (allKeys.indexOf(QRegExp(QString("^%1.*").arg(key))) != -1)
            key = AWTestLibrary::randomString(1, 20);
        allKeys.append(key);
    }

    auto keys = AWTestLibrary::randomSelect(allKeys);
    auto bars = AWTestLibrary::randomSelect(allKeys);
    std::for_each(bars.begin(), bars.end(),
                  [](QString &bar) { bar.prepend(QString("bar")); });
    QString pattern = QString("$%1 $%2")
                          .arg(keys.join(QString(" $")))
                          .arg(bars.join(QString(" $")));

    allKeys.append(bars);
    allKeys.sort();
    std::reverse(allKeys.begin(), allKeys.end());
    keys.sort();
    bars.sort();

    QStringList foundKeys
        = AWPatternFunctions::findKeys(pattern, allKeys, false);
    foundKeys.sort();
    QStringList foundBars
        = AWPatternFunctions::findKeys(pattern, allKeys, true);
    foundBars.sort();

    QCOMPARE(foundKeys, keys);
    QCOMPARE(foundBars, bars);
}


void TestAWPatternFunctions::test_findLambdas()
{
    QStringList lambdas = AWTestLibrary::randomStringList(20);
    QString pattern = AWTestLibrary::randomString()
                      + QString("${{%1}}").arg(lambdas.join(QString("}}${{")))
                      + AWTestLibrary::randomString();

    QCOMPARE(AWPatternFunctions::findLambdas(pattern), lambdas);
}


void TestAWPatternFunctions::test_expandTemplates()
{
    int firstValue = AWTestLibrary::randomInt();
    int secondValue = AWTestLibrary::randomInt();
    int result = firstValue + secondValue;
    QString code
        = QString("$template{{%1+%2}}").arg(firstValue).arg(secondValue);
    QString prefix = AWTestLibrary::randomString();
    QString pattern = prefix + code;

    QCOMPARE(AWPatternFunctions::expandTemplates(pattern),
             QString("%1%2").arg(prefix).arg(result));
}


QTEST_MAIN(TestAWPatternFunctions);
