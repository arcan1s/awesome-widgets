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


#ifndef TESTAWKEYS_H
#define TESTAWKEYS_H

#include <QObject>


class AWKeys;

class TestAWKeys : public QObject
{
    Q_OBJECT

private slots:
    // initialization
    void initTestCase();
    void cleanupTestCase();
    // test
    void test_hddDevices();
    void test_dictKeys();
    void test_pattern();
    void test_tooltip();
    void test_wrapNewLines();
    void test_infoByKey();
    void test_valueByKey();
    void test_dbus();

private:
    AWKeys *plugin = nullptr;
    QString pattern;
    int interval = 1000;
};


#endif /* TESTAWKEYS_H */
