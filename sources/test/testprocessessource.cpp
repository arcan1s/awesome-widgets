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


#include "testprocessessource.h"

#include <QtTest>

#include "awtestlibrary.h"
#include "processessource.h"


void TestProcessesSource::initTestCase()
{
    source = new ProcessesSource(this, QStringList());
}


void TestProcessesSource::cleanupTestCase()
{
    delete source;
}


void TestProcessesSource::test_sources()
{
    QCOMPARE(source->sources().count(), 3);
    // FIXME there is segfault here sometimes o_0
//    QVERIFY(std::all_of(
//        source->sources().cbegin(), source->sources().cend(),
//        [](const QString &src) { return src.startsWith(QString("ps/")); }));
}


void TestProcessesSource::test_values()
{
    QVERIFY(source->data(QString("ps/running/count")).toInt() > 0);
    QVERIFY(source->data(QString("ps/running/list")).toStringList().count() > 0);
    QVERIFY(source->data(QString("ps/total/count")).toInt() > 0);
}


QTEST_MAIN(TestProcessesSource);
