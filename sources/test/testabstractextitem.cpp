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
    auto names = AWTestLibrary::randomFilenames();
    fileName = names.first;
    writeFileName = names.second;

    extItem = new ExtUpgrade(nullptr, fileName);
    extItem->setActive(false);
    extItem->setApiVersion(1);
    extItem->setComment(comment);
    extItem->setName(name);
    extItem->setNumber(-1);
}


void TestAbstractExtItem::cleanupTestCase()
{
    QFile::remove(fileName);
    delete extItem;
}


void TestAbstractExtItem::test_values()
{
    QCOMPARE(extItem->isActive(), false);
    QCOMPARE(extItem->apiVersion(), 1);
    QCOMPARE(extItem->comment(), comment);
    QCOMPARE(extItem->fileName(), fileName);
    QCOMPARE(extItem->name(), name);
    QVERIFY((extItem->number() > 0) && (extItem->number() < 1000));
}


void TestAbstractExtItem::test_writtableFile()
{
    QCOMPARE(extItem->writtableConfig(), writeFileName);
}


void TestAbstractExtItem::test_configuration()
{
    extItem->writeConfiguration();

    ExtUpgrade *newExtItem = new ExtUpgrade(nullptr, writeFileName);
    QCOMPARE(newExtItem->isActive(), extItem->isActive());
    QCOMPARE(newExtItem->comment(), extItem->comment());
    QCOMPARE(newExtItem->fileName(), writeFileName);
    QCOMPARE(newExtItem->name(), extItem->name());
    QCOMPARE(newExtItem->number(), extItem->number());

    delete newExtItem;
}


void TestAbstractExtItem::test_bumpApi()
{
    extItem->bumpApi(100500);

    QCOMPARE(extItem->apiVersion(), 100500);
}


void TestAbstractExtItem::test_delete()
{
    ExtUpgrade *newExtItem = new ExtUpgrade(nullptr, writeFileName);

    QVERIFY(newExtItem->tryDelete());
    QVERIFY(!QFile::exists(writeFileName));

    delete newExtItem;
}


void TestAbstractExtItem::test_copy()
{
    ExtUpgrade *newExtItem = extItem->copy(QString("/dev/null"), 1);

    QCOMPARE(newExtItem->isActive(), extItem->isActive());
    QCOMPARE(newExtItem->apiVersion(), extItem->apiVersion());
    QCOMPARE(newExtItem->comment(), extItem->comment());
    QCOMPARE(newExtItem->name(), extItem->name());

    delete newExtItem;
}


QTEST_MAIN(TestAbstractExtItem);
