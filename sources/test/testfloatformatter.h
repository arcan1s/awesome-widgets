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


#ifndef TESTFLOATFORMATTER_H
#define TESTFLOATFORMATTER_H

#include <QObject>
#include <QVariant>


class AWFloatFormatter;

class TestAWFloatFormatter : public QObject
{
    Q_OBJECT

private slots:
    // initialization
    void initTestCase();
    void cleanupTestCase();
    // test
    void test_values();
    void test_count();
    void test_fillChar();
    void test_format();
    void test_precision();
    void test_multiplier();
    void test_summand();
    void test_copy();

private:
    void doRandom();
    float getValue() const;
    AWFloatFormatter *formatter = nullptr;
};


#endif /* TESTFLOATFORMATTER_H */
