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

#include <QtTest>

#include "awkeys.h"
#include "awtestlibrary.h"


void TestAWKeys::initTestCase()
{
    plugin = new AWKeys(this);

    // tooltip init
    QVariantMap tooltipSettings;
    tooltipSettings[QString("tooltipNumber")] = 1000;
    tooltipSettings[QString("useTooltipBackground")] = true;
    tooltipSettings[QString("tooltipBackground")] = QString("#ffffff");
    tooltipSettings[QString("cpuTooltip")] = true;
    tooltipSettings[QString("cpuclTooltip")] = true;
    tooltipSettings[QString("memTooltip")] = true;
    tooltipSettings[QString("swapTooltip")] = true;
    tooltipSettings[QString("downkbTooltip")] = true;
    tooltipSettings[QString("upkbTooltip")] = true;
    tooltipSettings[QString("batTooltip")] = true;
    tooltipSettings[QString("cpuTooltipColor")] = QString("#ffffff");
    tooltipSettings[QString("cpuclTooltipColor")] = QString("#ffffff");
    tooltipSettings[QString("memTooltipColor")] = QString("#ffffff");
    tooltipSettings[QString("swapTooltipColor")] = QString("#ffffff");
    tooltipSettings[QString("downkbTooltipColor")] = QString("#ffffff");
    tooltipSettings[QString("upkbTooltipColor")] = QString("#ffffff");
    tooltipSettings[QString("batTooltipColor")] = QString("#ffffff");
    tooltipSettings[QString("batInTooltipColor")] = QString("#ffffff");
    tooltipSettings[QString("acOnline")] = QString("(*)");
    tooltipSettings[QString("notify")] = false;
    plugin->initDataAggregator(tooltipSettings);

    // aggregator init
    plugin->setAggregatorProperty("acOffline", QString("( )"));
    plugin->setAggregatorProperty("acOnline", QString("(*)"));
    plugin->setAggregatorProperty("customTime", QString("$hh"));
    plugin->setAggregatorProperty("customUptime", QString("$hh"));
    plugin->setAggregatorProperty("tempUnits", QString("Celsius"));
    plugin->setAggregatorProperty("translate", false);

    plugin->initKeys(pattern, interval, 0, false);
}


void TestAWKeys::cleanupTestCase()
{
    delete plugin;
}


void TestAWKeys::test_hddDevices()
{
    QVERIFY(plugin->getHddDevices().count() > 2);
}


void TestAWKeys::test_dictKeys()
{
    QStringList keys = plugin->dictKeys();
    QVERIFY(!keys.isEmpty());

    QStringList sorted = plugin->dictKeys(true);
    QVERIFY(!sorted.isEmpty());
    QEXPECT_FAIL("", "Sorted and non-sorted lists should differ", Continue);
    QCOMPARE(keys, sorted);

    pattern = QString("$%1").arg(sorted.join(QString("\n$")));
}


void TestAWKeys::test_pattern()
{
    plugin->initKeys(pattern, interval, 0, false);
    QSignalSpy spy(plugin, SIGNAL(needTextToBeUpdated(const QString)));

    QVERIFY(spy.wait(5 * interval));
    QVERIFY(spy.wait(5 * interval));
    QString text = spy.takeFirst().at(0).toString();

    QEXPECT_FAIL("", "Pattern should be parsed", Continue);
    QCOMPARE(text, pattern);
    QStringList keys = plugin->dictKeys(true);
    for (auto key : keys)
        QVERIFY(!text.contains(key));
}


void TestAWKeys::test_tooltip()
{
    QSignalSpy spy(plugin, SIGNAL(needToolTipToBeUpdated(const QString)));

    QVERIFY(spy.wait(5 * interval));
    QString text = spy.takeFirst().at(0).toString();
    QVERIFY(text.startsWith(QString("<img")));
}


void TestAWKeys::test_wrapNewLines()
{
    QSignalSpy spy(plugin, SIGNAL(needTextToBeUpdated(const QString)));

    QVERIFY(spy.wait(5 * interval));
    QString text = spy.takeFirst().at(0).toString();
    QVERIFY(!text.contains("<br>") && text.contains("\n"));

    plugin->setWrapNewLines(true);
    QVERIFY(spy.wait(5 * interval));
    text = spy.takeFirst().at(0).toString();
    QVERIFY(text.contains("<br>") && !text.contains("\n"));
}


void TestAWKeys::test_infoByKey()
{
    int notEmpty = 0;
    QStringList keys = plugin->dictKeys(true);
    for (auto key : keys) {
        QString info = plugin->infoByKey(key);
        QVERIFY(!info.isEmpty());
        // append non-empty field count
        if (info != QString("(none)"))
            notEmpty++;
    }
    QVERIFY(notEmpty > 0);
}


void TestAWKeys::test_valueByKey()
{
    int notEmpty = 0;
    QStringList keys = plugin->dictKeys(true);
    for (auto key : keys) {
        if (!plugin->valueByKey(key).isEmpty())
            notEmpty++;
    }
    QVERIFY(notEmpty > 0);
}


QTEST_MAIN(TestAWKeys);
