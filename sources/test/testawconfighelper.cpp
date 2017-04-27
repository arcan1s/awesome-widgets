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


#include "testawconfighelper.h"

#include <QtTest>

#include "awconfighelper.h"
#include "awtestlibrary.h"


void TestAWConfigHelper::initTestCase()
{
    AWTestLibrary::init();
    plugin = new AWConfigHelper(this);
}


void TestAWConfigHelper::cleanupTestCase()
{
    delete plugin;
}


void TestAWConfigHelper::test_configurationDirectory()
{
    QVERIFY(!plugin->configurationDirectory().isEmpty());
}


void TestAWConfigHelper::test_exportConfiguration()
{
    QStringList keys = AWTestLibrary::randomStringList();
    for (auto key : keys)
        map[key] = AWTestLibrary::randomString();
    filename = AWTestLibrary::randomFilenames().first;

    QVERIFY(plugin->exportConfiguration(&map, filename));
}


void TestAWConfigHelper::test_importConfiguration()
{
    QVariantMap imported
        = plugin->importConfiguration(filename, true, true, true);
    QVariantMap converted;
    for (auto key : map.keys())
        converted[key] = map.value(key);

    QCOMPARE(imported, converted);
}


void TestAWConfigHelper::test_readDataEngineConfiguration()
{
    deConfig = plugin->readDataEngineConfiguration();
    QVERIFY(!deConfig.isEmpty());
}


void TestAWConfigHelper::test_writeDataEngineConfiguration()
{
    QVERIFY(plugin->writeDataEngineConfiguration(deConfig));
    QCOMPARE(plugin->readDataEngineConfiguration(), deConfig);
}


QTEST_MAIN(TestAWConfigHelper);
