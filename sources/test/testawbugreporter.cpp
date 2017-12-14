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


#include "testawbugreporter.h"

#include <QtTest>

#include "awbugreporter.h"
#include "awtestlibrary.h"


void TestAWBugReporter::initTestCase()
{
    AWTestLibrary::init();
    plugin = new AWBugReporter(this);
}


void TestAWBugReporter::cleanupTestCase()
{
    delete plugin;
}


void TestAWBugReporter::test_generateText()
{
    data = AWTestLibrary::randomStringList(4);
    QString output = plugin->generateText(data.at(0), data.at(1), data.at(2), data.at(3));

    for (auto &string : data)
        QVERIFY(output.contains(string));
}


void TestAWBugReporter::test_sendBugReport()
{
    QSignalSpy spy(plugin, SIGNAL(replyReceived(int, QString &)));
    plugin->sendBugReport(AWTestLibrary::randomString(),
                          plugin->generateText(data.at(0), data.at(1), data.at(2), data.at(3)));

    QVERIFY(spy.wait(5000));
    QVariantList arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).toInt() > 0);
    QVERIFY(!arguments.at(1).toString().isEmpty());
}


QTEST_MAIN(TestAWBugReporter);
