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


class QGraphicsScene;

class GraphicalItemHelper : public QObject
{
public:
    explicit GraphicalItemHelper(QObject *parent = nullptr,
                                 QGraphicsScene *scene = nullptr);
    virtual ~GraphicalItemHelper();
    // parameters
    void setParameters(const QColor active, const QColor inactive,
                       const int width, const int height);
    // paint methods
    void paintCircle(const float &percent);
    void paintGraph(const QList<float> &value);
    void paintHorizontal(const float &percent);
    void paintVertical(const float &percent);
    // additional conversion methods
    QString colorToString(const QColor &color);
    float getPercents(const float &value, const float &min, const float &max);
    QColor stringToColor(const QString &color);

private:
    QGraphicsScene *m_scene = nullptr;
    QColor m_activeColor;
    QColor m_inactiveColor;
    int m_width;
    int m_height;
};


#endif /* GRAPHICALITEMHELPER_H */
