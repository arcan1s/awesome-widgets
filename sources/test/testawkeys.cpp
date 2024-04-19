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

#include "testawkeys.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QtTest>

#include "awkeys.h"
#include "awtestlibrary.h"
#include "version.h"


void TestAWKeys::initTestCase()
{
    AWTestLibrary::init();
    plugin = new AWKeys(this);

    // tooltip init
    QVariantMap tooltipSettings;
    tooltipSettings["tooltipNumber"] = 1000;
    tooltipSettings["useTooltipBackground"] = true;
    tooltipSettings["tooltipBackground"] = "#ffffff";
    tooltipSettings["cpuTooltip"] = true;
    tooltipSettings["cpuclTooltip"] = true;
    tooltipSettings["memTooltip"] = true;
    tooltipSettings["swapTooltip"] = true;
    tooltipSettings["downkbTooltip"] = true;
    tooltipSettings["upkbTooltip"] = true;
    tooltipSettings["batTooltip"] = true;
    tooltipSettings["cpuTooltipColor"] = "#ffffff";
    tooltipSettings["cpuclTooltipColor"] = "#ffffff";
    tooltipSettings["memTooltipColor"] = "#ffffff";
    tooltipSettings["swapTooltipColor"] = "#ffffff";
    tooltipSettings["downkbTooltipColor"] = "#ffffff";
    tooltipSettings["upkbTooltipColor"] = "#ffffff";
    tooltipSettings["batTooltipColor"] = "#ffffff";
    tooltipSettings["batInTooltipColor"] = "#ffffff";
    tooltipSettings["acOnline"] = "(*)";
    tooltipSettings["notify"] = false;
    plugin->initDataAggregator(tooltipSettings);

    // aggregator init
    plugin->setAggregatorProperty("acOffline", "( )");
    plugin->setAggregatorProperty("acOnline", "(*)");
    plugin->setAggregatorProperty("customTime", "$hh");
    plugin->setAggregatorProperty("customUptime", "$hh");
    plugin->setAggregatorProperty("tempUnits", "Celsius");
    plugin->setAggregatorProperty("translate", false);

    plugin->initKeys(pattern, interval, false);
}


void TestAWKeys::cleanupTestCase()
{
    delete plugin;
}


void TestAWKeys::test_dictKeys()
{
    auto keys = plugin->dictKeys();
    QVERIFY(!keys.isEmpty());

    auto sorted = plugin->dictKeys(true);
    QVERIFY(!sorted.isEmpty());
    QEXPECT_FAIL("", "Sorted and non-sorted lists should differ", Continue);
    QCOMPARE(keys, sorted);

    pattern = QString("$%1").arg(sorted.join("\n$"));
}


void TestAWKeys::test_pattern()
{
    plugin->initKeys(pattern, interval, false);
    QSignalSpy spy(plugin, SIGNAL(needTextToBeUpdated(const QString)));

    QVERIFY(spy.wait(5 * interval));
    QVERIFY(spy.wait(5 * interval));
    auto text = spy.takeFirst().at(0).toString();

    QEXPECT_FAIL("", "Pattern should be parsed", Continue);
    QCOMPARE(text, pattern);
    auto keys = plugin->dictKeys(true);
    for (auto &key : keys)
        QVERIFY(!text.contains(key));
}


void TestAWKeys::test_tooltip()
{
    if (!AWTestLibrary::isKWinActive())
        QSKIP("KWin inactive, skip tooltip test");

    QSignalSpy spy(plugin, SIGNAL(needToolTipToBeUpdated(const QString)));

    QVERIFY(spy.wait(5 * interval));
    auto text = spy.takeFirst().at(0).toString();
    QVERIFY(text.startsWith("<img"));
}


void TestAWKeys::test_wrapNewLines()
{
    QSignalSpy spy(plugin, SIGNAL(needTextToBeUpdated(const QString)));

    QVERIFY(spy.wait(5 * interval));
    auto text = spy.takeFirst().at(0).toString();
    QVERIFY(!text.contains("<br>") && text.contains("\n"));

    plugin->setWrapNewLines(true);
    QVERIFY(spy.wait(5 * interval));
    text = spy.takeFirst().at(0).toString();
    QVERIFY(text.contains("<br>") && !text.contains("\n"));
}


void TestAWKeys::test_infoByKey()
{
    auto notEmpty = 0;
    auto keys = plugin->dictKeys(true);
    for (auto &key : keys) {
        auto info = plugin->infoByKey(key);
        QVERIFY(!info.isEmpty());
        // append non-empty field count
        if (info != "(none)")
            notEmpty++;
    }
    QVERIFY(notEmpty > 0);
}


void TestAWKeys::test_valueByKey()
{
    auto notEmpty = 0;
    auto keys = plugin->dictKeys(true);
    for (auto &key : keys) {
        if (!plugin->valueByKey(key).isEmpty())
            notEmpty++;
    }
    QVERIFY(notEmpty > 0);
}


void TestAWKeys::test_dbus()
{
    // get id
    auto id = reinterpret_cast<qlonglong>(plugin);

    // create connection and message
    auto bus = QDBusConnection::sessionBus();

    // check if there is active sessions first
    auto sessions = QDBusMessage::createMethodCall(AWDBUS_SERVICE, AWDBUS_PATH, AWDBUS_SERVICE, "ActiveServices");
    auto sessionsResponse = bus.call(sessions, QDBus::BlockWithGui);
    if (sessionsResponse.arguments().isEmpty())
        QSKIP("No active sessions found, skip DBus tests");

    // dbus checks
    auto request = QDBusMessage::createMethodCall(QString("%1.i%2").arg(AWDBUS_SERVICE).arg(id), AWDBUS_PATH,
                                                  AWDBUS_SERVICE, "WhoAmI");
    // send message to dbus
    auto response = bus.call(request, QDBus::BlockWithGui);

    // parse result
    auto arguments = response.arguments();
    QVERIFY(!arguments.isEmpty());
    QCOMPARE(arguments.at(0).toLongLong(), id);
}


QTEST_MAIN(TestAWKeys);
