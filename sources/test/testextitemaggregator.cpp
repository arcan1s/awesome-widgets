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


#include "testextitemaggregator.h"

#include <QtTest>

#include "awnoformatter.h"
#include "awtestlibrary.h"
#include "extitemaggregator.h"


void TestExtItemAggregator::initTestCase()
{
    AWTestLibrary::init();
    aggregator = new ExtItemAggregator<AWNoFormatter>(nullptr, type);
}


void TestExtItemAggregator::cleanupTestCase()
{
    delete aggregator;
}


void TestExtItemAggregator::test_values()
{
    QCOMPARE(aggregator->type(), type);
    QCOMPARE(aggregator->uniqNumber(), 0);
    QCOMPARE(aggregator->items().count(), 0);
}


QTEST_MAIN(TestExtItemAggregator);
