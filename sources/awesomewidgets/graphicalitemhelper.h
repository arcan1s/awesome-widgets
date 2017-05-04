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

#ifndef GRAPHICALITEMHELPER_H
#define GRAPHICALITEMHELPER_H

#include <QColor>
#include <QObject>
#include <QPen>


class QGraphicsScene;

class GraphicalItemHelper : public QObject
{
    Q_OBJECT

public:
    explicit GraphicalItemHelper(QObject *parent = nullptr,
                                 QGraphicsScene *scene = nullptr);
    virtual ~GraphicalItemHelper();
    // parameters
    void setParameters(const QString &active, const QString &inactive,
                       const int width, const int height, const int count);
    // paint methods
    void paintBars(const float value);
    void paintCircle(const float percent);
    void paintGraph(const float value);
    void paintHorizontal(const float percent);
    void paintVertical(const float percent);
    // additional conversion methods
    float getPercents(const float value, const float min, const float max);
    bool isColor(const QString &input);
    QColor stringToColor(const QString &color);

private:
    void storeValue(const float value);
    QGraphicsScene *m_scene = nullptr;
    int m_count = 100;
    QPen m_activePen;
    QPen m_inactivePen;
    int m_width = 100;
    int m_height = 100;
    // list of values which will be used to store data for graph type only
    QList<float> m_values;
};


#endif /* GRAPHICALITEMHELPER_H */
