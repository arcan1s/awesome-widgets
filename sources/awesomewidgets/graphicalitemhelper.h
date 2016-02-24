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


class QGraphicsScene;

namespace GraphicalItemHelper
{
// paint methods
void paintCircle(const float &percent, const QColor &inactive,
                 const QColor &active, const int &width, const int &height,
                 QGraphicsScene *scene);
void paintGraph(const QList<float> &value, const QColor &active,
                const int &width, const int &height, QGraphicsScene *scene);
void paintHorizontal(const float &percent, const QColor &inactive,
                     const QColor &active, const int &width, const int &height,
                     QGraphicsScene *scene);
void paintVertical(const float &percent, const QColor &inactive,
                   const QColor &active, const int &width, const int &height,
                   QGraphicsScene *scene);
// additional conversion methods
QString colorToString(const QColor &color);
float getPercents(const float &value, const float &min, const float &max);
QColor stringToColor(const QString &color);
};


#endif /* GRAPHICALITEMHELPER_H */
