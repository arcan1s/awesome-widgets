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
#include <QUrl>

#include <cmath>

#include "awdebug.h"


GraphicalItemHelper::GraphicalItemHelper(QObject *_parent, QGraphicsScene *_scene)
    : QObject(_parent)
    , m_scene(_scene)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


GraphicalItemHelper::~GraphicalItemHelper()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


void GraphicalItemHelper::setParameters(const QString &_active, const QString &_inactive, const int _width,
                                        const int _height, const int _count)
{
    qCDebug(LOG_LIB) << "Use active color" << _active << ", inactive" << _inactive << ", width" << _width << ", height"
                     << _height << ", count" << _count;

    // put images to pens if any otherwise set pen colors
    // Images resize to content here as well
    m_activePen.setBrush(parseBrush(_active, _width, _height, QColor(0, 0, 0, 130)));
    m_inactivePen.setBrush(parseBrush(_inactive, _width, _height, QColor(255, 255, 255, 130)));

    m_width = _width;
    m_height = _height;
    m_count = _count;
}


void GraphicalItemHelper::paintBars(const float _value)
{
    qCDebug(LOG_LIB) << "Paint with value" << _value;

    // refresh background image
    m_scene->setBackgroundBrush(m_inactivePen.brush());

    storeValue(_value);

    // default norms
    auto normX = static_cast<float>(m_width) / static_cast<float>(m_values.count());
    auto normY = static_cast<float>(m_height - 1);
    // paint graph
    for (auto i = 0; i < m_values.count(); ++i) {
        auto x = static_cast<float>(i) * normX;
        auto y = 0.5f;
        auto width = normX;
        auto height = m_values.at(i) * normY + 0.5f;
        m_scene->addRect(x, y, width, height, m_activePen, m_activePen.brush());
    }
}


void GraphicalItemHelper::paintCircle(const float _percent)
{
    qCDebug(LOG_LIB) << "Paint with percent" << _percent;

    m_activePen.setWidth(1);
    m_inactivePen.setWidth(1);
    // 16 is because of qt. From Qt documentation:
    // Returns the start angle for an ellipse segment in 16ths of a degree

    // inactive
    auto circle = m_scene->addEllipse(0.0, 0.0, m_width, m_height, m_inactivePen, m_inactivePen.brush());
    circle->setSpanAngle(static_cast<int>(-(1.0 - _percent) * 360.0 * 16.0));
    circle->setStartAngle(static_cast<int>(90.0 * 16.0 - _percent * 360.0 * 16.0));
    // active
    circle = m_scene->addEllipse(0.0, 0.0, m_width, m_height, m_activePen, m_activePen.brush());
    circle->setSpanAngle(static_cast<int>(-_percent * 360.0 * 16.0));
    circle->setStartAngle(90 * 16);
}


void GraphicalItemHelper::paintGraph(const float _value)
{
    qCDebug(LOG_LIB) << "Paint with value" << _value;

    // refresh background image
    m_scene->setBackgroundBrush(m_inactivePen.brush());

    storeValue(_value);

    // default norms
    auto normX = static_cast<float>(m_width) / static_cast<float>(m_values.count());
    auto normY = static_cast<float>(m_height - 1);
    // paint graph
    for (auto i = 0; i < m_values.count() - 1; ++i) {
        // some magic here
        auto value = static_cast<float>(i);
        auto x1 = value * normX;
        auto y1 = m_values.at(i) * normY + 0.5f;
        auto x2 = (value + 1) * normX;
        auto y2 = m_values.at(i + 1) * normY + 0.5f;
        m_scene->addLine(x1, y1, x2, y2, m_activePen);
    }
}


void GraphicalItemHelper::paintHorizontal(const float _percent)
{
    qCDebug(LOG_LIB) << "Paint with percent" << _percent;

    m_activePen.setWidth(m_height);
    m_inactivePen.setWidth(m_height);
    // inactive
    auto width = static_cast<float>(m_width);
    m_scene->addLine(_percent * width + 0.5 * m_height, 0.5 * m_height, m_width + 0.5 * m_height, 0.5 * m_height,
                     m_inactivePen);
    // active
    m_scene->addLine(-0.5 * m_height, 0.5 * m_height, _percent * width - 0.5 * m_height, 0.5 * m_height, m_activePen);
}


void GraphicalItemHelper::paintVertical(const float _percent)
{
    qCDebug(LOG_LIB) << "Paint with percent" << _percent;

    m_activePen.setWidth(m_height);
    m_inactivePen.setWidth(m_height);
    // inactive
    m_scene->addLine(0.5 * m_width, -0.5 * m_width, 0.5 * m_width, (1.0 - _percent) * m_height - 0.5 * m_width,
                     m_inactivePen);
    // active
    m_scene->addLine(0.5 * m_width, (1.0 - _percent) * m_height + 0.5 * m_width, 0.5 * m_width,
                     m_height + 0.5 * m_width, m_activePen);
}


float GraphicalItemHelper::getPercents(const float _value, const float _min, const float _max)
{
    qCDebug(LOG_LIB) << "Get percent value from" << _value;
    // newest Qt crashes here if value is nan
    if (std::isnan(_value))
        return 0.0;

    return (_value - _min) / (_max - _min);
}


bool GraphicalItemHelper::isColor(const QString &_input)
{
    qCDebug(LOG_LIB) << "Define input type in" << _input;

    return _input.startsWith("color://");
}


QColor GraphicalItemHelper::stringToColor(const QString &_color)
{
    qCDebug(LOG_LIB) << "Color" << _color;

    auto listColor = _color.split(',');
    while (listColor.count() < 4)
        listColor.append("0");
    // remove prefix
    listColor[0].remove("color://");
    // init color
    QColor qColor;
    qColor.setRed(listColor.at(0).toInt());
    qColor.setGreen(listColor.at(1).toInt());
    qColor.setBlue(listColor.at(2).toInt());
    qColor.setAlpha(listColor.at(3).toInt());

    return qColor;
}


QBrush GraphicalItemHelper::parseBrush(const QString &_input, const int _width, const int _height,
                                       const QColor &_default)
{
    qCDebug(LOG_LIB) << "Input select" << _input;

    if (isColor(_input)) {
        return stringToColor(_input);
    } else {
        qCInfo(LOG_LIB) << "Found path, trying to load Pixmap from" << _input;
        auto pixmap = QPixmap(QUrl(_input).toLocalFile());
        if (!pixmap.isNull())
            return pixmap.scaled(_width, _height);
    }

    qCWarning(LOG_LIB) << "Invalid pixmap found" << _input;
    return _default;
}


void GraphicalItemHelper::storeValue(const float _value)
{
    qCDebug(LOG_LIB) << "Save value to array" << _value;

    if (m_values.count() == 0)
        m_values.append(1.0);
    else if (m_values.count() > m_count)
        m_values.removeFirst();

    m_values.append(_value);
}
