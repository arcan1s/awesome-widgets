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


#include "testawtelemetryhandler.h"

#include <QtTest>

#include "awtelemetryhandler.h"
#include "awtestlibrary.h"


void TestAWTelemetryHandler::initTestCase()
{
    plugin = new AWTelemetryHandler(this, telemetryId);
    telemetryData = AWTestLibrary::randomString();
    telemetryGroup = AWTestLibrary::randomString();
}


void TestAWTelemetryHandler::cleanupTestCase()
{
    delete plugin;
}


void TestAWTelemetryHandler::test_put()
{
    QVERIFY(plugin->put(telemetryGroup, telemetryData));
}


void TestAWTelemetryHandler::test_get()
{
    QStringList output = plugin->get(telemetryGroup);

    QVERIFY(!output.isEmpty());
    QCOMPARE(QSet<QString>::fromList(output).count(), output.count());
    QVERIFY(output.contains(telemetryData));
}


void TestAWTelemetryHandler::test_getLast()
{
    QCOMPARE(telemetryData, plugin->getLast(telemetryGroup));
}


void TestAWTelemetryHandler::test_uploadTelemetry()
{
    QSignalSpy spy(plugin, SIGNAL(replyReceived(QString)));
    plugin->uploadTelemetry(telemetryValidGroup, telemetryData);

    QVERIFY(spy.wait(5000));
    QVariantList arguments = spy.takeFirst();

    QCOMPARE(arguments.at(0).toString(), QString("saved"));
}


QTEST_MAIN(TestAWTelemetryHandler);
