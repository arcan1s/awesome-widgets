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
#include "ui_graphicalitem.h"

#include <QBuffer>
#include <QColorDialog>
#include <QDebug>
#include <QDir>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSettings>

#include <pdebug/pdebug.h>


GraphicalItem::GraphicalItem(QWidget *parent, const QString desktopName, const QStringList directories, const bool debugCmd)
    : QDialog(parent),
      fileName(desktopName),
      dirs(directories),
      debug(debugCmd),
      ui(new Ui::GraphicalItem)
{
    readConfiguration();

    ui->setupUi(this);
    connect(ui->pushButton_activeColor, SIGNAL(clicked()), this, SLOT(changeColor()));
    connect(ui->pushButton_inactiveColor, SIGNAL(clicked()), this, SLOT(changeColor()));
}


GraphicalItem::~GraphicalItem()
{
    if (debug) qDebug() << PDEBUG;

    delete ui;
}


QString GraphicalItem::getImage(const float value)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Value" << value;
    if (_bar == QString("none")) return QString("");

    QColor active = stringToColor(_activeColor);
    QColor inactive = stringToColor(_inactiveColor);
    float percent = value / 100.0;
    int scale[2] = {1, 1};
    QPen pen = QPen();
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(QBrush(Qt::NoBrush));
    QGraphicsView *view = new QGraphicsView(scene);
    view->setStyleSheet(QString("background: transparent"));
    view->setContentsMargins(0, 0, 0, 0);
    view->setFrameShape(QFrame::NoFrame);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->resize(_width + 5.0, _height + 5.0);

    // paint
    switch(_type) {
    case Vertical:
        pen.setWidth(_width);
        // inactive
        pen.setColor(inactive);
        scene->addLine(0.5 * _width, -0.5 * _width, 0.5 * _width, (1.0 - percent) * _height - 0.5 * _width, pen);
        // active
        pen.setColor(active);
        scene->addLine(0.5 * _width, (1.0 - percent) * _height + 0.5 * _width, 0.5 * _width, _height + 0.5 * _width, pen);
        // scale
        scale[1] = -2 * (int)_direction + 1;
        break;
    case Circle:
        QGraphicsEllipseItem *circle;
        pen.setWidth(1.0);
        // inactive
        pen.setColor(inactive);
        circle = scene->addEllipse(0.0, 0.0, _width, _height, pen, QBrush(inactive, Qt::SolidPattern));
        circle->setSpanAngle(- (1.0 - percent) * 360.0 * 16.0);
        circle->setStartAngle(90.0 * 16.0 - percent * 360.0 * 16.0);
        // active
        pen.setColor(active);
        circle = scene->addEllipse(0.0, 0.0, _width, _height, pen, QBrush(active, Qt::SolidPattern));
        circle->setSpanAngle(- percent * 360.0 * 16.0);
        circle->setStartAngle(90.0 * 16.0);
        // scale
        scale[0] = -2 *(int)_direction + 1;
        break;
    default:
        pen.setWidth(_height);
        // inactive
        pen.setColor(inactive);
        scene->addLine(percent * _width + 0.5 * _height, 0.5 * _height, _width + 0.5 * _height, 0.5 * _height, pen);
        // active
        pen.setColor(active);
        scene->addLine(-0.5 * _height, 0.5 * _height, percent * _width - 0.5 * _height, 0.5 * _height, pen);
        // scale
        scale[0] = -2 * (int)_direction + 1;
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


QString GraphicalItem::getFileName()
{
    if (debug) qDebug() << PDEBUG;

    return fileName;
}


QString GraphicalItem::getName()
{
    if (debug) qDebug() << PDEBUG;

    return _name;
}


QString GraphicalItem::getComment()
{
    if (debug) qDebug() << PDEBUG;

    return _comment;
}


QString GraphicalItem::getBar()
{
    if (debug) qDebug() << PDEBUG;

    return _bar;
}


QString GraphicalItem::getActiveColor()
{
    if (debug) qDebug() << PDEBUG;

    return _activeColor;
}


QString GraphicalItem::getInactiveColor()
{
    if (debug) qDebug() << PDEBUG;

    return _inactiveColor;
}


GraphicalItem::Type GraphicalItem::getType()
{
    if (debug) qDebug() << PDEBUG;

    return _type;
}


QString GraphicalItem::getStrType()
{
    if (debug) qDebug() << PDEBUG;

    QString value;
    switch(_type) {
    case Vertical:
        value = QString("Vertical");
        break;
    case Circle:
        value = QString("Circle");
        break;
    default:
        value = QString("Horizontal");
        break;
    }\

    return value;
}


GraphicalItem::Direction GraphicalItem::getDirection()
{
    if (debug) qDebug() << PDEBUG;

    return _direction;
}


QString GraphicalItem::getStrDirection()
{
    if (debug) qDebug() << PDEBUG;

    QString value;
    switch (_direction) {
    case RightToLeft:
        value = QString("RightToLeft");
        break;
    default:
        value = QString("LeftToRight");
        break;
    }

    return value;
}


int GraphicalItem::getHeight()
{
    if (debug) qDebug() << PDEBUG;

    return _height;
}


int GraphicalItem::getWidth()
{
    if (debug) qDebug() << PDEBUG;

    return _width;
}


void GraphicalItem::setName(const QString name)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Name" << name;

    _name = name;
}


void GraphicalItem::setComment(const QString comment)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Comment" << comment;

    _comment = comment;
}


void GraphicalItem::setBar(const QString bar)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Bar" << bar;

    _bar = bar;
    if ((!_bar.contains(QRegExp(QString("cpu(?!cl).*")))) &&
        (!_bar.contains(QRegExp(QString("gpu")))) &&
        (!_bar.contains(QRegExp(QString("mem")))) &&
        (!_bar.contains(QRegExp(QString("swap")))) &&
        (!_bar.contains(QRegExp(QString("hdd[0-9].*")))) &&
        (!_bar.contains(QRegExp(QString("bat.*")))))
        _bar = QString("none");
}


void GraphicalItem::setActiveColor(const QString color)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Color" << color;

    _activeColor = color;
}


void GraphicalItem::setInactiveColor(const QString color)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Color" << color;

    _inactiveColor = color;
}


void GraphicalItem::setType(const QString type)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Type" << type;

    if (type == QString("Vertical"))
        _type = Vertical;
    else if (type == QString("Circle"))
        _type = Circle;
    else
        _type = Horizontal;
}


void GraphicalItem::setDirection(const QString direction)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Direction" << direction;

    if (direction == QString("RightToLeft"))
        _direction = RightToLeft;
    else
        _direction = LeftToRight;
}


void GraphicalItem::setHeight(const int height)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Height" << height;
    if (height <= 0) return;

    _height = height;
}


void GraphicalItem::setWidth(const int width)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Width" << width;
    if (width <= 0) return;

    _width = width;
}


void GraphicalItem::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=dirs.count()-1; i>=0; i--) {
        if (!QDir(dirs[i]).entryList(QDir::Files).contains(fileName)) continue;
        QSettings settings(dirs[i] + QDir::separator() + fileName, QSettings::IniFormat);
        settings.beginGroup(QString("Desktop Entry"));
        setName(settings.value(QString("Name"), _name).toString());
        setComment(settings.value(QString("Comment"), _comment).toString());
        setBar(settings.value(QString("X-AW-Value"), _bar).toString());
        setActiveColor(settings.value(QString("X-AW-ActiveColor"), _activeColor).toString());
        setInactiveColor(settings.value(QString("X-AW-InactiveColor"), _inactiveColor).toString());
        setType(settings.value(QString("X-AW-Type"), getStrType()).toString());
        setDirection(settings.value(QString("X-AW-Direction"), getStrDirection()).toString());
        setHeight(settings.value(QString("X-AW-Height"), QString::number(_height)).toInt());
        setWidth(settings.value(QString("X-AW-Width"), QString::number(_width)).toInt());
        settings.endGroup();
    }
}


void GraphicalItem::showConfiguration(const QStringList tags)
{
    if (debug) qDebug() << PDEBUG;

    ui->label_nameValue->setText(_name);
    ui->lineEdit_comment->setText(_comment);
    ui->comboBox_value->addItems(tags);
    ui->comboBox_value->addItem(_bar);
    ui->comboBox_value->setCurrentIndex(ui->comboBox_value->count() - 1);
    ui->pushButton_activeColor->setText(_activeColor);
    ui->pushButton_inactiveColor->setText(_inactiveColor);
    ui->comboBox_type->setCurrentIndex((int)_type);
    ui->comboBox_direction->setCurrentIndex((int)_direction);
    ui->spinBox_height->setValue(_height);
    ui->spinBox_width->setValue(_width);

    int ret = exec();
    if (ret != 1) return;
    setName(ui->label_nameValue->text());
    setComment(ui->lineEdit_comment->text());
    setBar(ui->comboBox_value->currentText());
    setActiveColor(ui->pushButton_activeColor->text().remove(QChar('&')));
    setInactiveColor(ui->pushButton_inactiveColor->text().remove(QChar('&')));
    setType(ui->comboBox_type->currentText());
    setDirection(ui->comboBox_direction->currentText());
    setHeight(ui->spinBox_height->value());
    setWidth(ui->spinBox_width->value());

    writeConfiguration();
}


void GraphicalItem::tryDelete()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=0; i<dirs.count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << "Remove file" << dirs[i] + QDir::separator() + fileName <<
                               QFile::remove(dirs[i] + QDir::separator() + fileName);
}


void GraphicalItem::writeConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QSettings settings(dirs[0] + QDir::separator() + fileName, QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();
    settings.beginGroup(QString("Desktop Entry"));

    settings.setValue(QString("Encoding"), QString("UTF-8"));
    settings.setValue(QString("Name"), _name);
    settings.setValue(QString("Comment"), _comment);
    settings.setValue(QString("X-AW-Value"), _bar);
    settings.setValue(QString("X-AW-ActiveColor"), _activeColor);
    settings.setValue(QString("X-AW-InactiveColor"), _inactiveColor);
    settings.setValue(QString("X-AW-Type"), getStrType());
    settings.setValue(QString("X-AW-Direction"), getStrDirection());
    settings.setValue(QString("X-AW-Height"), _height);
    settings.setValue(QString("X-AW-Width"), _width);

    settings.endGroup();
    settings.sync();
}


void GraphicalItem::changeColor()
{
    if (debug) qDebug() << PDEBUG;

    QColor color = stringToColor(((QPushButton *)sender())->text());
    QColor newColor = QColorDialog::getColor(color, 0, i18n("Select color"),
                                             QColorDialog::ShowAlphaChannel);
    if (!newColor.isValid()) return;

    QStringList colorText;
    colorText.append(QString("%1").arg(newColor.red()));
    colorText.append(QString("%1").arg(newColor.green()));
    colorText.append(QString("%1").arg(newColor.blue()));
    colorText.append(QString("%1").arg(newColor.alpha()));
    ((QPushButton *)sender())->setText(colorText.join(QChar(',')));
}


QColor GraphicalItem::stringToColor(const QString color)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Color" << color;

    QColor qcolor;
    QStringList listColor = color.split(QChar(','));
    while (listColor.count() < 4)
        listColor.append(QString("0"));
    qcolor.setRed(listColor[0].toInt());
    qcolor.setGreen(listColor[1].toInt());
    qcolor.setBlue(listColor[2].toInt());
    qcolor.setAlpha(listColor[3].toInt());

    return qcolor;
}
