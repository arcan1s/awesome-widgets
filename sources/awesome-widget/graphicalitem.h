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

#ifndef GRAPHICALITEM_H
#define GRAPHICALITEM_H

#include <QColor>
#include <QObject>


class GraphicalItem : public QObject
{
    Q_OBJECT

public:
    enum Direction {
        RightToLeft = -1,
        LeftToRight = 1
    };
    enum Type {
        Horizontal = 0,
        Vertical,
        Ring
    };

    GraphicalItem(QObject *wid, const QString assignName, const QString tag, const bool debugCmd = false);
    ~GraphicalItem();
    QString getImage(const float value);
    QString getName();
    QString getBar();
    QColor getActiveColor();
    QColor getInactiveColor();
    Type getType();
    Direction getDirection();
    int getHeight();
    int getWidth();

public slots:
    void parseTag(const QString tag);

private:
    QList<int> parseColor(const QString stringColor);
    QString name;
    bool debug;
    // properties
    QString bar = QString("cpu");
    QColor activeColor = QColor(QString("#ffffff"));
    QColor inactiveColor = QColor(QString("#000000"));
    Type type = Horizontal;
    Direction direction;
    int height = 100;
    int width = 100;
};


#endif /* GRAPHICALITEM_H */
