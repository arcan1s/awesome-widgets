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

#include "abstractextitem.h"


class GraphicalItemHelper;
class QGraphicsScene;
class QGraphicsView;

namespace Ui
{
class GraphicalItem;
}

class GraphicalItem : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(QString bar READ bar WRITE setBar)
    Q_PROPERTY(QString activeColor READ activeColor WRITE setActiveColor)
    Q_PROPERTY(bool custom READ isCustom WRITE setCustom)
    Q_PROPERTY(QString inactiveColor READ inactiveColor WRITE setInactiveColor)
    Q_PROPERTY(Type type READ type WRITE setType)
    Q_PROPERTY(Direction direction READ direction WRITE setDirection)
    Q_PROPERTY(int height READ height WRITE setHeight)
    Q_PROPERTY(float maxValue READ maxValue WRITE setMaxValue)
    Q_PROPERTY(float minValue READ minValue WRITE setMinValue)
    Q_PROPERTY(QStringList usedKeys READ usedKeys WRITE setUsedKeys)
    Q_PROPERTY(int width READ width WRITE setWidth)

public:
    enum Direction { LeftToRight = 0, RightToLeft };
    enum Type { Horizontal = 0, Vertical, Circle, Graph };

    explicit GraphicalItem(QWidget *parent = nullptr,
                           const QString desktopName = QString(),
                           const QStringList directories = QStringList());
    virtual ~GraphicalItem();
    GraphicalItem *copy(const QString _fileName, const int _number);
    QString image(const QVariant &value);
    // get methods
    QString bar() const;
    QString activeColor() const;
    QString inactiveColor() const;
    bool isCustom() const;
    float minValue() const;
    float maxValue() const;
    Type type() const;
    QString strType() const;
    Direction direction() const;
    QString strDirection() const;
    int height() const;
    QStringList usedKeys() const;
    int width() const;
    QString uniq() const;
    // set methods
    void setBar(const QString _bar = QString("cpu"));
    void setActiveColor(const QString _color = QString("0,0,0,130"));
    void setCustom(const bool _custom = false);
    void setInactiveColor(const QString _color = QString("255,255,255,130"));
    void setMinValue(const float _value = 0.0);
    void setMaxValue(const float _value = 100.0);
    void setType(const Type _type = Horizontal);
    void setStrType(const QString _type = QString("Horizontal"));
    void setDirection(const Direction _direction = LeftToRight);
    void setStrDirection(const QString _direction = QString("LeftToRight"));
    void setHeight(const int _height = 100);
    void setUsedKeys(const QStringList _usedKeys = QStringList());
    void setWidth(const int _width = 100);

public slots:
    void readConfiguration();
    QVariantHash run();
    int showConfiguration(const QVariant args = QVariant());
    void writeConfiguration() const;

private slots:
    void changeColor();
    void changeValue(const int state);

private:
    GraphicalItemHelper *m_helper = nullptr;
    QGraphicsScene *m_scene = nullptr;
    QGraphicsView *m_view = nullptr;
    Ui::GraphicalItem *ui;
    void initScene();
    void translate();
    // properties
    QString m_bar = QString("cpu");
    bool m_custom = false;
    QColor m_activeColor = QColor(0, 0, 0, 130);
    QColor m_inactiveColor = QColor(255, 255, 255, 130);
    float m_minValue = 0.0f;
    float m_maxValue = 100.0f;
    Type m_type = Horizontal;
    Direction m_direction = LeftToRight;
    int m_height = 100;
    QStringList m_usedKeys;
    int m_width = 100;
};


#endif /* GRAPHICALITEM_H */
