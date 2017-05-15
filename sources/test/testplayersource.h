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


#ifndef TESTPLAYERSOURCE_H
#define TESTPLAYERSOURCE_H

#include <QObject>


class PlayerSource;

class TestPlayerSource : public QObject
{
    Q_OBJECT

private slots:
    // initialization
    void initTestCase();
    void cleanupTestCase();
    // test
    void _test_sources(const PlayerSource *_source);
    void test_buildString();
    void test_stripString();
    void test_autoMpris();
    void test_mpd();
    void test_mpris();

private:
    QString mpdAddress = "localhost";
    int mpdPort = 6600;
};


#endif /* TESTPLAYERSOURCE_H */
