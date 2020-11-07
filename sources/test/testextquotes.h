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


#ifndef TESTEXTQUOTES_H
#define TESTEXTQUOTES_H

#include <QObject>
#include <QVariant>


class ExtQuotes;

class TestExtQuotes : public QObject
{
    Q_OBJECT

private slots:
    // initialization
    void initTestCase();
    void cleanupTestCase();
    // test
    void test_values();
    void test_run();
    void test_derivatives();
    void test_copy();

private:
    ExtQuotes *extQuotes = nullptr;
    QVariantHash cache;
    QString ticker = "EURUSD";
    QStringList types = {"price", "volume"};
    // we assume that price will not be differ more than in 2 times
    QPair<double, double> price = QPair<double, double>(0.0, 2.0);
};


#endif /* TESTEXTQUOTES_H */
