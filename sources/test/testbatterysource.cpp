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

#include "testbatterysource.h"

#include <QtTest>

#include <cmath>

#include "awtestlibrary.h"
#include "batterysource.h"


void TestBatterySource::initTestCase()
{
    AWTestLibrary::init();
    source = new BatterySource(this, acpiPath);
}


void TestBatterySource::cleanupTestCase()
{
    delete source;
}


void TestBatterySource::test_sources()
{
    QVERIFY(source->sources().count() >= 6);
}


void TestBatterySource::test_battery()
{
    if (source->sources().count() == 6)
        QSKIP("No battery found, test will be skipped");

    auto batteries = source->sources().keys();
    std::for_each(batteries.cbegin(), batteries.cend(), [this](auto bat) {
        auto value = source->data(bat);
        if (bat == "ac")
            QCOMPARE(value.type(), QVariant::Bool);
        else if (bat.startsWith("batrate") || bat.startsWith("batleft"))
            ;
        else
            QVERIFY((value.toFloat() >= battery.first) || (std::isnan(value.toFloat())));
    });
}


QTEST_MAIN(TestBatterySource);
