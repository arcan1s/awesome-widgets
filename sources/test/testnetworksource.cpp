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


#include "testnetworksource.h"

#include <QtTest>

#include "awtestlibrary.h"
#include "networksource.h"


void TestNetworkSource::initTestCase()
{
    AWTestLibrary::init();
    source = new NetworkSource(this, QStringList());
}


void TestNetworkSource::cleanupTestCase()
{
    delete source;
}


void TestNetworkSource::test_sources()
{
    QCOMPARE(source->sources(), QStringList({"device", "ssid"}));
}


void TestNetworkSource::test_values()
{
    QVERIFY(source->data("device").toString().length() > 0);
}


QTEST_MAIN(TestNetworkSource);
