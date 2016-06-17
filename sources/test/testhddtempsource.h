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


#ifndef TESTHDDTEMPSOURCE_H
#define TESTHDDTEMPSOURCE_H

#include <QObject>


class HDDTemperatureSource;

class TestHDDTemperatureSource : public QObject
{
    Q_OBJECT

private slots:
    // initialization
    void initTestCase();
    void cleanupTestCase();
    // test
    void test_sources();
    void test_hddtemp();
    void test_smartctl();

private:
    HDDTemperatureSource *hddtempSource = nullptr;
    HDDTemperatureSource *smartctlSource = nullptr;
    QStringList devices;
    QString hddtempCmd = QString("sudo hddtemp");
    QString smartctlCmd = QString("sudo smartctl -a");
    QPair<float, float> temp = QPair<float, float>(0.0f, 120.0f);
};


#endif /* TESTHDDTEMPSOURCE_H */
