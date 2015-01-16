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
#include <QDialog>

namespace Ui {
class GraphicalItem;
}

class GraphicalItem : public QDialog
{
    Q_OBJECT

public:
    enum Direction {
        LeftToRight = 0,
        RightToLeft
    };
    enum Type {
        Horizontal = 0,
        Vertical,
        Circle
    };

    explicit GraphicalItem(QWidget *parent = 0, const QString desktopName = QString(),
                           const QStringList directories = QStringList(), const bool debugCmd = false);
    ~GraphicalItem();
    QString getImage(const float value);
    // get methods
    QString getFileName();
    QString getName();
    QString getComment();
    QString getBar();
    QString getActiveColor();
    QString getInactiveColor();
    Type getType();
    QString getStrType();
    Direction getDirection();
    QString getStrDirection();
    int getHeight();
    int getWidth();
    // set methods
    void setName(const QString name = QString("none"));
    void setComment(const QString comment = QString("empty"));
    void setBar(const QString bar = QString("cpu"));
    void setActiveColor(const QString color = QString("0,0,0,130"));
    void setInactiveColor(const QString color = QString("255,255,255,130"));
    void setType(const QString type = QString("Horizontal"));
    void setDirection(const QString direction = QString("LeftToRight"));
    void setHeight(const int height = 100);
    void setWidth(const int width = 100);

public slots:
    void readConfiguration();
    void showConfiguration(const QStringList tags = QStringList());
    void tryDelete();
    void writeConfiguration();

private slots:
    void changeColor();

private:
    QColor stringToColor(const QString color);
    QString fileName;
    QStringList dirs;
    bool debug;
    Ui::GraphicalItem *ui;
    // properties
    QString _name = QString("none");
    QString _comment = QString("empty");
    QString _bar = QString("cpu");
    QString _activeColor = QString("0,0,0,130");
    QString _inactiveColor = QString("255,255,255,130");
    Type _type = Horizontal;
    Direction _direction = LeftToRight;
    int _height = 100;
    int _width = 100;
};

#endif /* GRAPHICALITEM_H */
