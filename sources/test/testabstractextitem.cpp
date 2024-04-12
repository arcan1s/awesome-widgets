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

#include "testabstractextitem.h"

#include <QtTest>

#include "awtestlibrary.h"
#include "extupgrade.h"


void TestAbstractExtItem::initTestCase()
{
    AWTestLibrary::init();
    auto names = AWTestLibrary::randomFilenames();
    filePath = names.first;
    writeFilePath = names.second;

    name = AWTestLibrary::randomString();
    comment = AWTestLibrary::randomString();
    socket = AWTestLibrary::randomString();

    extItem = new ExtUpgrade(nullptr, filePath);
    extItem->setActive(false);
    extItem->setApiVersion(1);
    extItem->setComment(comment);
    extItem->setCron(cron);
    extItem->setName(name);
    extItem->setNumber(-1);
    extItem->setSocket(socket);
}


void TestAbstractExtItem::cleanupTestCase()
{
    QFile::remove(filePath);
    delete extItem;
}


void TestAbstractExtItem::test_values()
{
    QCOMPARE(extItem->isActive(), false);
    QCOMPARE(extItem->apiVersion(), 1);
    QCOMPARE(extItem->comment(), comment);
    QCOMPARE(extItem->filePath(), filePath);
    QCOMPARE(extItem->name(), name);
    QVERIFY((extItem->number() > 0) && (extItem->number() < 1000));
    QCOMPARE(extItem->socket(), socket);
}


void TestAbstractExtItem::test_writableFile()
{
    QCOMPARE(extItem->writableConfig(), writeFilePath);
}


void TestAbstractExtItem::test_configuration()
{
    extItem->writeConfiguration();

    auto newExtItem = new ExtUpgrade(nullptr, writeFilePath);
    QCOMPARE(newExtItem->isActive(), extItem->isActive());
    QCOMPARE(newExtItem->comment(), extItem->comment());
    QCOMPARE(newExtItem->filePath(), writeFilePath);
    QCOMPARE(newExtItem->name(), extItem->name());
    QCOMPARE(newExtItem->number(), extItem->number());
    QCOMPARE(newExtItem->socket(), extItem->socket());
    QCOMPARE(newExtItem->cron(), extItem->cron());

    delete newExtItem;
}


void TestAbstractExtItem::test_bumpApi()
{
    extItem->bumpApi(100500);

    QCOMPARE(extItem->apiVersion(), 100500);
}


void TestAbstractExtItem::test_delete()
{
    auto newExtItem = new ExtUpgrade(nullptr, writeFilePath);

    QVERIFY(newExtItem->tryDelete());
    QVERIFY(!QFile::exists(writeFilePath));

    delete newExtItem;
}


void TestAbstractExtItem::test_copy()
{
    auto newExtItem = extItem->copy("/dev/null", 1);

    QCOMPARE(newExtItem->isActive(), extItem->isActive());
    QCOMPARE(newExtItem->apiVersion(), extItem->apiVersion());
    QCOMPARE(newExtItem->comment(), extItem->comment());
    QCOMPARE(newExtItem->name(), extItem->name());
    QCOMPARE(newExtItem->socket(), extItem->socket());
    QCOMPARE(newExtItem->cron(), extItem->cron());

    delete newExtItem;
}


QTEST_MAIN(TestAbstractExtItem);
