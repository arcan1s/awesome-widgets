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
    Q_PROPERTY(int apiVersion READ apiVersion WRITE setApiVersion)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString comment READ comment WRITE setComment)
    Q_PROPERTY(QString bar READ bar WRITE setBar)
    Q_PROPERTY(QString activeColor READ activeColor WRITE setActiveColor)
    Q_PROPERTY(QString inactiveColor READ inactiveColor WRITE setInactiveColor)
    Q_PROPERTY(Type type READ type WRITE setType)
    Q_PROPERTY(Direction direction READ direction WRITE setDirection)
    Q_PROPERTY(int height READ height WRITE setHeight)
    Q_PROPERTY(int width READ width WRITE setWidth)

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

    explicit GraphicalItem(QWidget *parent = nullptr, const QString desktopName = QString(),
                           const QStringList directories = QStringList(),
                           const bool debugCmd = false);
    ~GraphicalItem();
    QString fileName();
    QString image(const float value);
    // get methods
    int apiVersion();
    QString name();
    QString comment();
    QString bar();
    QString activeColor();
    QString inactiveColor();
    Type type();
    QString strType();
    Direction direction();
    QString strDirection();
    int height();
    int width();
    // set methods
    void setApiVersion(const int _apiVersion = 0);
    void setName(const QString _name = QString("none"));
    void setComment(const QString _comment = QString("empty"));
    void setBar(const QString _bar = QString("cpu"));
    void setActiveColor(const QString _color = QString("0,0,0,130"));
    void setInactiveColor(const QString _color = QString("255,255,255,130"));
    void setType(const Type _type = Horizontal);
    void setStrType(const QString _type = QString("Horizontal"));
    void setDirection(const Direction _direction = LeftToRight);
    void setStrDirection(const QString _direction = QString("LeftToRight"));
    void setHeight(const int _height = 100);
    void setWidth(const int _width = 100);

public slots:
    void readConfiguration();
    int showConfiguration(const QStringList tags = QStringList());
    bool tryDelete();
    void writeConfiguration();

private slots:
    void changeColor();

private:
    QColor stringToColor(const QString _color);
    QString m_fileName;
    QStringList m_dirs;
    bool debug;
    Ui::GraphicalItem *ui;
    // properties
    int m_apiVersion = 0;
    QString m_name = QString("none");
    QString m_comment = QString("empty");
    QString m_bar = QString("cpu");
    QString m_activeColor = QString("0,0,0,130");
    QString m_inactiveColor = QString("255,255,255,130");
    Type m_type = Horizontal;
    Direction m_direction = LeftToRight;
    int m_height = 100;
    int m_width = 100;
};


#endif /* GRAPHICALITEM_H */
