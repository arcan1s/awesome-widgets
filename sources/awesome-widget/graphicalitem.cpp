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

#include "graphicalitem.h"

#include <QBuffer>
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>


#include <pdebug/pdebug.h>


GraphicalItem::GraphicalItem(QObject *wid, const QString assignName, const QString tag, const bool debugCmd)
    : QObject(wid),
      name(assignName),
      debug(debugCmd)
{
    if (debug) qDebug() << PDEBUG << ":" << "Name" << name;
    parseTag(tag);
}


GraphicalItem::~GraphicalItem()
{
    if (debug) qDebug() << PDEBUG;
}


QString GraphicalItem::getImage(const float value)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Value" << value;
    if (bar == QString("none")) return QString("");

    float percent = value / 100.0;
    int scale[2] = {1, 1};
    QPen pen = QPen();
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(QBrush(Qt::NoBrush));
    QGraphicsView *view = new QGraphicsView(scene);
    view->setStyleSheet(QString("background: transparent"));
    view->setContentsMargins(0, 0, 0, 0);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->resize(width + 5.0, height + 5.0);

    // paint
    switch(type) {
    case Vertical:
        pen.setWidth(width);
        // inactive
        pen.setColor(inactiveColor);
        scene->addLine(0.5 * width, 0.0, 0.5 * width, height, pen);
        // active
        pen.setColor(activeColor);
        scene->addLine(0.5 * width, (1.0 - percent) * height + 0.5 * width, 0.5 * width, height + 0.5 * width, pen);
        // scale
        scale[1] = (int)direction;
        break;
    case Circle:
        QGraphicsEllipseItem *circle;
        pen.setWidth(1.0);
        // inactive
        pen.setColor(inactiveColor);
        circle = scene->addEllipse(0.0, 0.0, width, height, pen, QBrush(inactiveColor, Qt::SolidPattern));
        circle->setSpanAngle((1.0 - percent) * 360.0 * 16.0);
        circle->setStartAngle(180.0 * 16.0 - (1.0 - percent) * 360.0 * 16.0);
        // active
        pen.setColor(activeColor);
        circle = scene->addEllipse(0.0, 0.0, width, height, pen, QBrush(activeColor, Qt::SolidPattern));
        circle->setSpanAngle(percent * 360.0 * 16.0);
        circle->setStartAngle(180.0 * 16.0);
        // scale
        scale[0] = (int)direction;
        break;
    default:
        pen.setWidth(height);
        // inactive
        pen.setColor(inactiveColor);
        scene->addLine(0.0, 0.5 * height, width, 0.5 * height, pen);
        // active
        pen.setColor(activeColor);
        scene->addLine(-0.5 * height, 0.5 * height, percent * width - 0.5 * height, 0.5 * height, pen);
        // scale
        scale[0] = (int)direction;
        break;
    }

    // convert
    QPixmap pixmap = QPixmap::grabWidget(view).transformed(QTransform().scale(scale[0], scale[1]));
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    pixmap.save(&buffer, "PNG");
    QString url = QString("<img src=\"data:image/png;base64,%1\"/>").arg(QString(byteArray.toBase64()));
    delete view;
    delete scene;

    return url;
}


QString GraphicalItem::getName()
{
    if (debug) qDebug() << PDEBUG;

    return name;
}


QString GraphicalItem::getBar()
{
    if (debug) qDebug() << PDEBUG;

    return bar;
}


QColor GraphicalItem::getActiveColor()
{
    if (debug) qDebug() << PDEBUG;

    return activeColor;
}


QColor GraphicalItem::getInactiveColor()
{
    if (debug) qDebug() << PDEBUG;

    return inactiveColor;
}


GraphicalItem::Type GraphicalItem::getType()
{
    if (debug) qDebug() << PDEBUG;

    return type;
}


GraphicalItem::Direction GraphicalItem::getDirection()
{
    if (debug) qDebug() << PDEBUG;

    return direction;
}


int GraphicalItem::getHeight()
{
    if (debug) qDebug() << PDEBUG;

    return height;
}


int GraphicalItem::getWidth()
{
    if (debug) qDebug() << PDEBUG;

    return width;
}


QList<int> GraphicalItem::parseColor(const QString stringColor)
{
    if (debug) qDebug() << PDEBUG;

    QList<int> color;
    color.append(0);
    color.append(0);
    color.append(0);
    color.append(0);

    QStringList colors = stringColor.split(QChar(','));
    for (int i=0; i<colors.count(); i++) {
        if (i == 4) break;
        color[i] = colors[i].toInt();
    }

    return color;
}


void GraphicalItem::parseTag(const QString tag)
{
    if (debug) qDebug() << PDEBUG;

    QStringList parsedTag = tag.split(QChar(';'));
    for (int i=0; i<parsedTag.count(); i++)
        parsedTag[i].remove(QChar('$')).remove(QChar('{')).remove(QChar('}'));

    if (parsedTag.count() > 0)
        bar = parsedTag[0];
    if (parsedTag.count() > 1) {
        QList<int> color = parseColor(parsedTag[1]);
        activeColor = QColor(color[0], color[1], color[2], color[3]);
    }
    if (parsedTag.count() > 2) {
        QList<int> color = parseColor(parsedTag[2]);
        inactiveColor = QColor(color[0], color[1], color[2], color[3]);
    }
    if (parsedTag.count() > 3)
        switch(parsedTag[3].toInt()) {
        case Vertical:
            type = Vertical;
            break;
        case Circle:
            type = Circle;
            break;
        default:
            type = Horizontal;
            break;
        }
    if (parsedTag.count() > 4)
        switch (parsedTag[4].toInt()) {
        case RightToLeft:
            direction = RightToLeft;
            break;
        default:
            direction = LeftToRight;
            break;
        }
    if (parsedTag.count() > 5)
        height = parsedTag[5].toInt();
    if (parsedTag.count() > 6)
        width = parsedTag[6].toInt();

    if ((!bar.contains(QRegExp(QString("cpu(?!cl).*")))) &&
        (!bar.contains(QRegExp(QString("gpu")))) &&
        (!bar.contains(QRegExp(QString("mem")))) &&
        (!bar.contains(QRegExp(QString("swap")))) &&
        (!bar.contains(QRegExp(QString("hdd[0-9].*")))) &&
        (!bar.contains(QRegExp(QString("bat.*")))))
        bar = QString("none");

    if (debug) qDebug() << PDEBUG << ":" << "Bar" << bar;
    if (debug) qDebug() << PDEBUG << ":" << "Active color" << activeColor;
    if (debug) qDebug() << PDEBUG << ":" << "Inactive color" << inactiveColor;
    if (debug) qDebug() << PDEBUG << ":" << "Type" << type;
    if (debug) qDebug() << PDEBUG << ":" << "Direction" << direction;
    if (debug) qDebug() << PDEBUG << ":" << "Size" << height << width;
}
