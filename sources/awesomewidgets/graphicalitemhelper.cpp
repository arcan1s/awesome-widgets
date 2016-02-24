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

#include "graphicalitemhelper.h"

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>

#include <math.h>

#include "awdebug.h"


void GraphicalItemHelper::paintCircle(const float &percent,
                                      const QColor &inactive,
                                      const QColor &active, const int &width,
                                      const int &height, QGraphicsScene *scene)
{
    QPen pen;
    pen.setWidth(1);
    QGraphicsEllipseItem *circle;

    // inactive
    pen.setColor(inactive);
    circle = scene->addEllipse(0.0, 0.0, width, height, pen,
                               QBrush(inactive, Qt::SolidPattern));
    circle->setSpanAngle(-(1.0f - percent) * 360.0f * 16.0f);
    circle->setStartAngle(90.0f * 16.0f - percent * 360.0f * 16.0f);
    // active
    pen.setColor(active);
    circle = scene->addEllipse(0.0, 0.0, width, height, pen,
                               QBrush(active, Qt::SolidPattern));
    circle->setSpanAngle(-percent * 360.0f * 16.0f);
    circle->setStartAngle(90.0f * 16.0f);
}


void GraphicalItemHelper::paintGraph(const QList<float> &value,
                                     const QColor &active, const int &width,
                                     const int &height, QGraphicsScene *scene)
{
    QPen pen;
    pen.setColor(active);

    // default norms
    float normX = static_cast<float>(width) / static_cast<float>(value.count());
    float normY = static_cast<float>(height) / (1.5f * 100.0f);
    // paint graph
    for (int i = 0; i < value.count() - 1; i++) {
        // some magic here
        float x1 = i * normX;
        float y1 = -fabs(value.at(i)) * normY + 5.0f;
        float x2 = (i + 1) * normX;
        float y2 = -fabs(value.at(i + 1)) * normY + 5.0f;
        scene->addLine(x1, y1, x2, y2, pen);
    }
}


void GraphicalItemHelper::paintHorizontal(const float &percent,
                                          const QColor &inactive,
                                          const QColor &active,
                                          const int &width, const int &height,
                                          QGraphicsScene *scene)
{
    QPen pen;
    pen.setWidth(height);
    // inactive
    pen.setColor(inactive);
    scene->addLine(percent * width + 0.5 * height, 0.5 * height,
                   width + 0.5 * height, 0.5 * height, pen);
    // active
    pen.setColor(active);
    scene->addLine(-0.5 * height, 0.5 * height, percent * width - 0.5 * height,
                   0.5 * height, pen);
}


void GraphicalItemHelper::paintVertical(const float &percent,
                                        const QColor &inactive,
                                        const QColor &active, const int &width,
                                        const int &height,
                                        QGraphicsScene *scene)
{
    QPen pen;
    pen.setWidth(width);
    // inactive
    pen.setColor(inactive);
    scene->addLine(0.5 * width, -0.5 * width, 0.5 * width,
                   (1.0 - percent) * height - 0.5 * width, pen);
    // active
    pen.setColor(active);
    scene->addLine(0.5 * width, (1.0 - percent) * height + 0.5 * width,
                   0.5 * width, height + 0.5 * width, pen);
}


QString GraphicalItemHelper::colorToString(const QColor &color)
{
    qCDebug(LOG_LIB) << "Color" << color;

    return QString("%1,%2,%3,%4")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(color.alpha());
}


float GraphicalItemHelper::getPercents(const float &value, const float &min,
                                       const float &max)
{
    qCDebug(LOG_LIB) << "Get percent value from" << value;

    return (value - min) / (max - min);
}


QColor GraphicalItemHelper::stringToColor(const QString &color)
{
    qCDebug(LOG_LIB) << "Color" << color;

    QColor qColor;
    QStringList listColor = color.split(QChar(','));
    while (listColor.count() < 4)
        listColor.append(QString("0"));
    qColor.setRed(listColor.at(0).toInt());
    qColor.setGreen(listColor.at(1).toInt());
    qColor.setBlue(listColor.at(2).toInt());
    qColor.setAlpha(listColor.at(3).toInt());

    return qColor;
}
