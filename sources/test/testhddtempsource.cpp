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


#include "testhddtempsource.h"

#include <QtTest>

#include "awtestlibrary.h"
#include "hddtempsource.h"


void TestHDDTemperatureSource::initTestCase()
{
    AWTestLibrary::init();
    devices = HDDTemperatureSource::allHdd();

    hddtempSource = new HDDTemperatureSource(this, QStringList() << devices.join(',') << hddtempCmd);
    smartctlSource = new HDDTemperatureSource(this, QStringList() << devices.join(',') << smartctlCmd);
}


void TestHDDTemperatureSource::cleanupTestCase()
{
    delete hddtempSource;
    delete smartctlSource;
}


void TestHDDTemperatureSource::test_sources()
{
    if (devices.isEmpty())
        QSKIP("No hdd devices found, test will be skipped");

    std::for_each(devices.begin(), devices.end(), [](QString &device) { device.prepend("hdd/temperature"); });

    QCOMPARE(hddtempSource->sources(), devices);
    QCOMPARE(smartctlSource->sources(), devices);
}


void TestHDDTemperatureSource::test_hddtemp()
{
    if (devices.isEmpty())
        QSKIP("No hdd devices found, test will be skipped");

    std::for_each(devices.begin(), devices.end(), [this](QString device) {
        QSignalSpy spy(hddtempSource, SIGNAL(dataReceived(const QVariantHash &)));
        float firstValue = hddtempSource->data(device).toFloat();

        QVERIFY(spy.wait(5000));
        QVariantHash arguments = spy.takeFirst().at(0).toHash();
        device.remove("hdd/temperature");
        float secondValue = arguments[device].toFloat();

        QCOMPARE(firstValue, 0.0f);
        QVERIFY((secondValue >= temp.first) && (secondValue <= temp.second));
    });
}


void TestHDDTemperatureSource::test_smartctl()
{
    if (devices.isEmpty())
        QSKIP("No hdd devices found, test will be skipped");

    std::for_each(devices.begin(), devices.end(), [this](QString &device) {
        QSignalSpy spy(smartctlSource, SIGNAL(dataReceived(const QVariantHash &)));
        float firstValue = smartctlSource->data(device).toFloat();

        QVERIFY(spy.wait(5000));
        QVariantHash arguments = spy.takeFirst().at(0).toHash();
        device.remove("hdd/temperature");
        float secondValue = arguments[device].toFloat();

        QCOMPARE(firstValue, 0.0f);
        QVERIFY((secondValue >= temp.first) && (secondValue <= temp.second));
    });
}


QTEST_MAIN(TestHDDTemperatureSource);
