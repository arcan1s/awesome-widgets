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

#pragma once

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
    enum class Type { Horizontal = 0, Vertical = 1, Circle = 2, Graph = 3, Bars = 4 };

    explicit GraphicalItem(QObject *_parent = nullptr, const QString &_filePath = "");
    ~GraphicalItem() override;
    GraphicalItem *copy(const QString &_fileName, int _number) override;
    QString image(const QVariant &value);
    void initScene();
    // get methods
    [[nodiscard]] QString bar() const;
    [[nodiscard]] QString activeColor() const;
    [[nodiscard]] int count() const;
    [[nodiscard]] QString inactiveColor() const;
    [[nodiscard]] bool isCustom() const;
    [[nodiscard]] int itemHeight() const;
    [[nodiscard]] int itemWidth() const;
    [[nodiscard]] float minValue() const;
    [[nodiscard]] float maxValue() const;
    [[nodiscard]] Type type() const;
    [[nodiscard]] QString strType() const;
    [[nodiscard]] Direction direction() const;
    [[nodiscard]] QString strDirection() const;
    [[nodiscard]] QStringList usedKeys() const;
    [[nodiscard]] QString uniq() const override;
    // set methods
    void setBar(const QString &_bar);
    void setActiveColor(const QString &_color);
    void setCount(int _count);
    void setCustom(bool _custom);
    void setInactiveColor(const QString &_color);
    void setItemHeight(int _height);
    void setItemWidth(int _width);
    void setMinValue(float _value);
    void setMaxValue(float _value);
    void setType(Type _type);
    void setStrType(const QString &_type);
    void setDirection(Direction _direction);
    void setStrDirection(const QString &_direction);
    void setUsedKeys(const QStringList &_usedKeys);

public slots:
    void readConfiguration() override;
    QVariantHash run() override { return {}; };
    int showConfiguration(QWidget *_parent, const QVariant &_args) override;
    void writeConfiguration() const override;

private slots:
    void changeColor(Ui::GraphicalItem *_ui);
    static void changeCountState(Ui::GraphicalItem *_ui, int _state);
    static void changeValue(Ui::GraphicalItem *_ui, int _state);

private:
    GraphicalItemHelper *m_helper = nullptr;
    QGraphicsScene *m_scene = nullptr;
    QGraphicsView *m_view = nullptr;
    void translate(void *_ui) override;
    // properties
    QString m_bar = "cpu";
    int m_count = 100;
    bool m_custom = false;
    QString m_activeColor = "color://0,0,0,130";
    QString m_inactiveColor = "color://255,255,255,130";
    float m_minValue = 0.0f;
    float m_maxValue = 100.0f;
    Type m_type = Type::Horizontal;
    Direction m_direction = Direction::LeftToRight;
    int m_height = 100;
    QStringList m_usedKeys;
    int m_width = 100;
};
