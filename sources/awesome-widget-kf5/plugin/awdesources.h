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


#ifndef AWDESOURCES_H
#define AWDESOURCES_H

#include <QMap>
#include <QObject>
#include <QVariant>


class AWDESources : public QObject
{
    Q_OBJECT

public:
    AWDESources(QObject *parent = 0,
                const QString pattern = QString(""),
                const QStringList foundKeys = QStringList(),
                const QStringList foundBars = QStringList(),
                const QMap<QString, QVariant> counts = QMap<QString, QVariant>(),
                const QMap<QString, QVariant> paths = QMap<QString, QVariant>(),
                const QMap<QString, QVariant> tooltipBools = QMap<QString, QVariant>());
    ~AWDESources();

    QStringList getSourcesForExtSystemMonitor();
    QStringList getSourcesForSystemMonitor();
    QStringList getSourcesForTimeMonitor();

private:
    // variables
    bool debug = false;
    QString m_pattern;
    QStringList m_foundKeys, m_foundBars;
    QMap<QString, QVariant> m_counts, m_paths, m_tooltipBools;
};


#endif /* AWDESOURCES_H */
