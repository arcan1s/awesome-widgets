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

#include <QColor>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>

#include <cmath>

#include "awdebug.h"


GraphicalItemHelper::GraphicalItemHelper(QObject *parent, QGraphicsScene *scene)
    : QObject(parent)
    , m_scene(scene)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


GraphicalItemHelper::~GraphicalItemHelper()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


void GraphicalItemHelper::setParameters(const QString active,
                                        const QString inactive, const int width,
                                        const int height, const int count)
{
    qCDebug(LOG_LIB) << "Use active color" << active << ", inactive" << inactive
                     << ", width" << width << ", height" << height << ", count"
                     << count;

    // put images to pens if any otherwise set pen colors
    // Images resize to content here as well
    if (active.startsWith(QString("/"))) {
        qCInfo(LOG_LIB) << "Found path, trying to load Pixmap from" << active;
        QPixmap pixmap = QPixmap(active);
        if (pixmap.isNull()) {
            qCInfo(LOG_LIB) << "Invalid pixmap found" << active;
            m_activePen.setColor(QColor(0, 0, 0, 130));
        } else {
            m_activePen.setBrush(QBrush(pixmap.scaled(width, height)));
        }
    } else {
        m_activePen.setColor(stringToColor(active));
    }
    if (inactive.startsWith(QString("/"))) {
        qCInfo(LOG_LIB) << "Found path, trying to load Pixmap from" << inactive;
        QPixmap pixmap = QPixmap(inactive);
        if (pixmap.isNull()) {
            qCInfo(LOG_LIB) << "Invalid pixmap found" << inactive;
            m_inactivePen.setColor(QColor(255, 255, 255, 130));
        } else {
            m_inactivePen.setBrush(QBrush(pixmap.scaled(width, height)));
        }
    } else {
        m_inactivePen.setColor(stringToColor(inactive));
    }
    m_width = width;
    m_height = height;
    m_count = count;
}


void GraphicalItemHelper::paintCircle(const float &percent)
{
    qCDebug(LOG_LIB) << "Paint with percent" << percent;

    m_activePen.setWidth(1);
    m_inactivePen.setWidth(1);
    QGraphicsEllipseItem *circle;
    // 16 is because of qt. From Qt documentation:
    // Returns the start angle for an ellipse segment in 16ths of a degree

    // inactive
    circle = m_scene->addEllipse(0.0, 0.0, m_width, m_height, m_inactivePen,
                                 m_inactivePen.brush());
    circle->setSpanAngle(-(1.0f - percent) * 360.0f * 16.0f);
    circle->setStartAngle(90.0f * 16.0f - percent * 360.0f * 16.0f);
    // active
    circle = m_scene->addEllipse(0.0, 0.0, m_width, m_height, m_activePen,
                                 m_activePen.brush());
    circle->setSpanAngle(-percent * 360.0f * 16.0f);
    circle->setStartAngle(90 * 16);
}


void GraphicalItemHelper::paintGraph(const float &value)
{
    qCDebug(LOG_LIB) << "Paint with value" << value;

    // refresh background image
    m_scene->setBackgroundBrush(m_inactivePen.brush());

    storeValue(value);

    // default norms
    float normX
        = static_cast<float>(m_width) / static_cast<float>(m_values.count());
    float normY = static_cast<float>(m_height - 1);
    // paint graph
    for (int i = 0; i < m_values.count() - 1; i++) {
        // some magic here
        float x1 = i * normX;
        float y1 = -fabs(m_values.at(i)) * normY + 0.5f;
        float x2 = (i + 1) * normX;
        float y2 = -fabs(m_values.at(i + 1)) * normY + 0.5f;
        m_scene->addLine(x1, y1, x2, y2, m_activePen);
    }
}


void GraphicalItemHelper::paintHorizontal(const float &percent)
{
    qCDebug(LOG_LIB) << "Paint with percent" << percent;

    m_activePen.setWidth(m_height);
    m_inactivePen.setWidth(m_height);
    // inactive
    m_scene->addLine(percent * m_width + 0.5 * m_height, 0.5 * m_height,
                     m_width + 0.5 * m_height, 0.5 * m_height, m_inactivePen);
    // active
    m_scene->addLine(-0.5 * m_height, 0.5 * m_height,
                     percent * m_width - 0.5 * m_height, 0.5 * m_height,
                     m_activePen);
}


void GraphicalItemHelper::paintVertical(const float &percent)
{
    qCDebug(LOG_LIB) << "Paint with percent" << percent;

    m_activePen.setWidth(m_height);
    m_inactivePen.setWidth(m_height);
    // inactive
    m_scene->addLine(0.5 * m_width, -0.5 * m_width, 0.5 * m_width,
                     (1.0 - percent) * m_height - 0.5 * m_width, m_inactivePen);
    // active
    m_scene->addLine(0.5 * m_width, (1.0 - percent) * m_height + 0.5 * m_width,
                     0.5 * m_width, m_height + 0.5 * m_width, m_activePen);
}


float GraphicalItemHelper::getPercents(const float &value, const float &min,
                                       const float &max)
{
    qCDebug(LOG_LIB) << "Get percent value from" << value;
    // newest Qt crashes here if value is nan
    if (isnan(value))
        return 0.0;

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


void GraphicalItemHelper::storeValue(const float &value)
{
    qCDebug(LOG_LIB) << "Save value to array" << value;

    if (m_values.count() == 0)
        m_values.append(1.0);
    else if (m_values.count() > m_count)
        m_values.removeFirst();

    m_values.append(value);
}
