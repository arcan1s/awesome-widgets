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
    m_isKwinActive = AWTestLibrary::isKWinActive();
}


void TestDPPlugin::cleanupTestCase()
{
    delete plugin;
}


void TestDPPlugin::test_desktops()
{
    if (!m_isKwinActive)
        QSKIP("KWin inactive, skip Destkop panel tests");

    int current = plugin->currentDesktop();
    int total = plugin->numberOfDesktops();
    QVERIFY(total != 0);
    QVERIFY(current <= total);

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
    if (!m_isKwinActive)
        QSKIP("KWin inactive, skip Destkop panel tests");

    QCOMPARE(plugin->dictKeys().count(), 4);
    pattern += plugin->dictKeys().join(QString(" $"));
}


void TestDPPlugin::test_infoByKey()
{
    if (!m_isKwinActive)
        QSKIP("KWin inactive, skip Destkop panel tests");

    // nothing to test here yet
    QVERIFY(true);
}


void TestDPPlugin::test_parsePattern()
{
    if (!m_isKwinActive)
        QSKIP("KWin inactive, skip Destkop panel tests");

    QString result = plugin->parsePattern(pattern, plugin->currentDesktop());
    QVERIFY(!result.isEmpty());
    QVERIFY(result != pattern);
    for (auto key : plugin->dictKeys())
        QVERIFY(!result.contains(key));
}


void TestDPPlugin::test_tooltipImage()
{
    if (!m_isKwinActive)
        QSKIP("KWin inactive, skip Destkop panel tests");

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
