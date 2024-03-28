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

#include "testdesktopsource.h"

#include <QtTest>

#include "awtestlibrary.h"
#include "desktopsource.h"


void TestDesktopSource::initTestCase()
{
    AWTestLibrary::init();
    source = new DesktopSource(this);
    m_isKwinActive = AWTestLibrary::isKWinActive();
}


void TestDesktopSource::cleanupTestCase()
{
    delete source;
}


void TestDesktopSource::test_sources()
{
    QCOMPARE(source->sources().count(), 3);
}


void TestDesktopSource::test_values()
{
    if (!m_isKwinActive)
        QSKIP("KWin inactive, skip desktop tests");

    QVERIFY(source->data("name").toString().length() > 0);
    QVERIFY(source->data("number").toInt() >= 0);
    QVERIFY(source->data("count").toInt() > 0);
}


QTEST_MAIN(TestDesktopSource);
