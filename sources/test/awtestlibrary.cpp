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


#include "awtestlibrary.h"

#include <KWindowSystem>
#include <QEventLoop>
#include <QSet>
#include <QStandardPaths>
#include <QTime>
#include <QtTest>


void AWTestLibrary::init()
{
    qsrand(static_cast<uint>(QTime::currentTime().msec()));
}


bool AWTestLibrary::isKWinActive()
{
    QSignalSpy spy(KWindowSystem::self(), SIGNAL(showingDesktopChanged(bool)));
    KWindowSystem::setShowingDesktop(true);
    spy.wait(5000);

    bool state = KWindowSystem::showingDesktop();
    KWindowSystem::setShowingDesktop(false);

    return state;
}


char AWTestLibrary::randomChar()
{
    return 'A' + (qrand() % static_cast<int>('Z' - 'A'));
}


double AWTestLibrary::randomDouble()
{
    return static_cast<double>(qrand()) / static_cast<double>(RAND_MAX);
}


QPair<QString, QString> AWTestLibrary::randomFilenames()
{
    QString fileName = QString("%1/").arg(
        QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    QString writeFileName = QString("%1/awesomewidgets/tmp/")
                                .arg(QStandardPaths::writableLocation(
                                    QStandardPaths::GenericDataLocation));

    QString name = randomString(1, 20);
    fileName += name;
    writeFileName += name;

    return QPair<QString, QString>(fileName, writeFileName);
}


int AWTestLibrary::randomInt(const int _max)
{
    return qrand() % _max;
}


QString AWTestLibrary::randomString(const int _min, const int _max)
{
    QString output;

    int count = _min + randomInt(_max - _min);
    for (int i = 0; i < count; i++)
        output += randomChar();

    return output;
}


QStringList AWTestLibrary::randomStringList(const int _max)
{
    QStringList output;

    int count = 1 + randomInt(_max);
    for (int i = 0; i < count; i++)
        output.append(randomString());

    return output;
}


QStringList AWTestLibrary::randomSelect(const QStringList &_available)
{
    QSet<QString> output;

    int count = 1 + randomInt(_available.count());
    for (int i = 0; i < count; i++) {
        int index = randomInt(_available.count());
        output << _available.at(index);
    }

    return output.toList();
}
