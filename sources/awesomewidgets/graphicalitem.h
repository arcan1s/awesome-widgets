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
    Q_PROPERTY(int count READ count WRITE setCount)
    Q_PROPERTY(bool custom READ isCustom WRITE setCustom)
    Q_PROPERTY(QString inactiveColor READ inactiveColor WRITE setInactiveColor)
    Q_PROPERTY(int itemHeight READ itemHeight WRITE setItemHeight)
    Q_PROPERTY(int itemWidth READ itemWidth WRITE setItemWidth)
    Q_PROPERTY(Type type READ type WRITE setType)
    Q_PROPERTY(Direction direction READ direction WRITE setDirection)
    Q_PROPERTY(float maxValue READ maxValue WRITE setMaxValue)
    Q_PROPERTY(float minValue READ minValue WRITE setMinValue)
    Q_PROPERTY(QStringList usedKeys READ usedKeys WRITE setUsedKeys)

public:
    enum class Direction { LeftToRight = 0, RightToLeft = 1 };
    enum class Type {
        Horizontal = 0,
        Vertical = 1,
        Circle = 2,
        Graph = 3,
        Bars = 4
    };

    explicit GraphicalItem(QWidget *parent, const QString &filePath = QString());
    virtual ~GraphicalItem();
    GraphicalItem *copy(const QString &_fileName, const int _number);
    QString image(const QVariant &value);
    void initScene();
    // get methods
    QString bar() const;
    QString activeColor() const;
    int count() const;
    QString inactiveColor() const;
    bool isCustom() const;
    int itemHeight() const;
    int itemWidth() const;
    float minValue() const;
    float maxValue() const;
    Type type() const;
    QString strType() const;
    Direction direction() const;
    QString strDirection() const;
    QStringList usedKeys() const;
    QString uniq() const;
    // set methods
    void setBar(const QString &_bar = QString("cpu"));
    void setActiveColor(const QString &_color = QString("color://0,0,0,130"));
    void setCount(const int _count = 100);
    void setCustom(const bool _custom = false);
    void setInactiveColor(const QString &_color
                          = QString("color://255,255,255,130"));
    void setItemHeight(const int _height = 100);
    void setItemWidth(const int _width = 100);
    void setMinValue(const float _value = 0.0);
    void setMaxValue(const float _value = 100.0);
    void setType(const Type _type = Type::Horizontal);
    void setStrType(const QString &_type = QString("Horizontal"));
    void setDirection(const Direction _direction = Direction::LeftToRight);
    void setStrDirection(const QString &_direction = QString("LeftToRight"));
    void setUsedKeys(const QStringList &_usedKeys = QStringList());

public slots:
    void readConfiguration();
    QVariantHash run() { return QVariantHash(); };
    int showConfiguration(const QVariant &args = QVariant());
    void writeConfiguration() const;

private slots:
    void changeColor();
    void changeCountState(const int state);
    void changeValue(const int state);

private:
    GraphicalItemHelper *m_helper = nullptr;
    QGraphicsScene *m_scene = nullptr;
    QGraphicsView *m_view = nullptr;
    Ui::GraphicalItem *ui = nullptr;
    void translate();
    // properties
    QString m_bar = QString("cpu");
    int m_count = 100;
    bool m_custom = false;
    QString m_activeColor = QString("color://0,0,0,130");
    QString m_inactiveColor = QString("color://255,255,255,130");
    float m_minValue = 0.0f;
    float m_maxValue = 100.0f;
    Type m_type = Type::Horizontal;
    Direction m_direction = Direction::LeftToRight;
    int m_height = 100;
    QStringList m_usedKeys;
    int m_width = 100;
};


#endif /* GRAPHICALITEM_H */
