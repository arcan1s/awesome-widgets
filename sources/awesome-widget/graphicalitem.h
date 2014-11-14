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
        Circle
    };

    GraphicalItem(QObject *wid, const QString desktopName, const QStringList directories, const bool debugCmd = false);
    ~GraphicalItem();
    QString getImage(const float value);
    // get methods
    QString getName();
    QString getComment();
    QString getBar();
    QColor getActiveColor();
    QColor getInactiveColor();
    Type getType();
    Direction getDirection();
    int getHeight();
    int getWidth();
    // set methods
    void setName(const QString name);
    void setComment(const QString comment);
    void setBar(const QString bar);
    void setActiveColor(const QColor color);
    void setInactiveColor(const QColor color);
    void setType(const QString type);
    void setDirection(const QString direction);
    void setHeight(const int height);
    void setWidth(const int width);

public slots:
    void readConfiguration();
    void writeConfiguration();

private:
    QString fileName;
    QStringList dirs;
    bool debug;
    // properties
    QString _name = QString("none");
    QString _comment = QString("empty");
    QString _bar = QString("cpu");
    QColor _activeColor = QColor(255, 255, 255, 0);
    QColor _inactiveColor = QColor(255, 255, 255, 255);
    Type _type = Horizontal;
    Direction _direction = LeftToRight;
    int _height = 100;
    int _width = 100;
};


#endif /* GRAPHICALITEM_H */
