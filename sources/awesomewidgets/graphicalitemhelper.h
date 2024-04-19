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
#include <QObject>
#include <QPen>


class QGraphicsScene;

class GraphicalItemHelper : public QObject
{
    Q_OBJECT

public:
    explicit GraphicalItemHelper(QObject *_parent = nullptr, QGraphicsScene *_scene = nullptr);
    ~GraphicalItemHelper() override;
    // parameters
    void setParameters(const QString &_active, const QString &_inactive, int _width, int _height, int _count);
    // paint methods
    void paintBars(float _value);
    void paintCircle(float _percent);
    void paintGraph(float _value);
    void paintHorizontal(float _percent);
    void paintVertical(float _percent);
    // additional conversion methods
    static double getPercents(double _value, double _min, double _max);
    static bool isColor(const QString &_input);
    static QColor stringToColor(const QString &_color);

private:
    static QBrush parseBrush(const QString &_input, int _width, int _height, const QColor &_default);
    void storeValue(float _value);
    QGraphicsScene *m_scene = nullptr;
    int m_count = 100;
    QPen m_activePen;
    QPen m_inactivePen;
    int m_width = 100;
    int m_height = 100;
    // list of values which will be used to store data for graph type only
    QList<float> m_values;
};
