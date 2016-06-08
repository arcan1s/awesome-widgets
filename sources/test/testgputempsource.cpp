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


#include "testgputempsource.h"

#include <QtTest>

#include "awtestlibrary.h"
#include "gputempsource.h"


void TestGPUTemperatureSource::initTestCase()
{
    device = GPUTemperatureSource::autoGpu();
    QVERIFY(!device.isEmpty());

    gputempSource = new GPUTemperatureSource(this, QStringList() << device);
}


void TestGPUTemperatureSource::cleanupTestCase()
{
    delete gputempSource;
}


void TestGPUTemperatureSource::test_sources()
{
    QCOMPARE(gputempSource->sources(), QStringList() << source);
}


void TestGPUTemperatureSource::test_gputemp()
{
    if (device == QString("disable"))
        QSKIP("Not supported device, test will be skipped");

    QSignalSpy spy(gputempSource, SIGNAL(dataReceived(const QVariantHash &)));
    float firstValue = gputempSource->data(source).toFloat();
    QCOMPARE(firstValue, 0.0f);

    QVERIFY(spy.wait(5000));
    QVariantHash arguments = spy.takeFirst().at(0).toHash();
    float secondValue = arguments[source].toFloat();
    QVERIFY((secondValue >= temp.first) && (secondValue <= temp.second));
}


QTEST_MAIN(TestGPUTemperatureSource);
