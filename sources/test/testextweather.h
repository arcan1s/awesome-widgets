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


#ifndef TESTEXTWEATHER_H
#define TESTEXTWEATHER_H

#include <QObject>
#include <QPair>


class ExtWeather;

class TestExtWeather : public QObject
{
    Q_OBJECT

private slots:
    // initialization
    void initTestCase();
    void cleanupTestCase();
    // test
    void test_values();
    void test_runOWM();
    void test_ts();
    void test_image();
    void test_copy();

private:
    void run();
    ExtWeather *extWeather = nullptr;
    QString city = "London";
    QString country = "uk";
    // humidity is in percents
    QPair<int, int> humidity = QPair<int, int>(0, 100);
    // pressure should be about 1 atm
    QPair<float, float> pressure = QPair<float, float>(500.0f, 1500.0f);
    // dont know about temperature, but I suppose it will be between -40 and 40
    QPair<float, float> temp = QPair<float, float>(-40.0f, 40.0f);
};


#endif /* TESTEXTWEATHER_H */
