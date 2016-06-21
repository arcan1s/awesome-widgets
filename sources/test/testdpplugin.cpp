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


#include "testdpplugin.h"

#include <KWindowSystem>
#include <QtTest>

#include "awtestlibrary.h"
#include "dpadds.h"


void TestDPPlugin::initTestCase()
{
    plugin = new DPAdds(this);
}


void TestDPPlugin::cleanupTestCase()
{
    delete plugin;
}


void TestDPPlugin::test_desktops()
{
    int current = plugin->currentDesktop();
    int total = plugin->numberOfDesktops();
    QVERIFY(total != 0);
    QVERIFY(current < total);

    int number;
    if (total == 1)
        number = current;
    else
        number = current == (total - 1) ? current - 1 : current + 1;
    QSignalSpy spy(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)));
    plugin->setCurrentDesktop(number);
    QVERIFY(spy.wait(5000));
    QCOMPARE(plugin->currentDesktop(), number);

    plugin->setCurrentDesktop(current);
    QVERIFY(spy.wait(5000));
    QCOMPARE(plugin->currentDesktop(), current);
}


void TestDPPlugin::test_dictKeys()
{
    QCOMPARE(plugin->dictKeys().count(), 4);
    pattern += plugin->dictKeys().join(QString(" $"));
}


void TestDPPlugin::test_parsePattern()
{
    QString result = plugin->parsePattern(pattern, plugin->currentDesktop());
    QVERIFY(!result.isEmpty());
    QVERIFY(result != pattern);
    for (auto key : plugin->dictKeys())
        QVERIFY(!result.contains(key));
}


void TestDPPlugin::test_tooltipImage()
{
    QVariantMap data;
    data[QString("tooltipColor")] = QString("#000000");
    data[QString("tooltipType")] = QString("windows");
    data[QString("tooltipWidth")] = 300;
    plugin->setToolTipData(data);

    QString image = plugin->toolTipImage(plugin->currentDesktop());
    QVERIFY(image.startsWith(QString("<img src=\"")));
    QVERIFY(image.endsWith(QString("\"/>")));
}


QTEST_MAIN(TestDPPlugin);
