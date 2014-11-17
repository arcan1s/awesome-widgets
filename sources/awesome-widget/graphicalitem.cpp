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
    view->resize(_width + 5.0, _height + 5.0);

    // paint
    switch(_type) {
    case Vertical:
        pen.setWidth(_width);
        // inactive
        pen.setColor(_inactiveColor);
        scene->addLine(0.5 * _width, 0.0, 0.5 * _width, _height, pen);
        // active
        pen.setColor(_activeColor);
        scene->addLine(0.5 * _width, (1.0 - percent) * _height + 0.5 * _width, 0.5 * _width, _height + 0.5 * _width, pen);
        // scale
        scale[1] = (int)_direction;
        break;
    case Circle:
        QGraphicsEllipseItem *circle;
        pen.setWidth(1.0);
        // inactive
        pen.setColor(_inactiveColor);
        circle = scene->addEllipse(0.0, 0.0, _width, _height, pen, QBrush(_inactiveColor, Qt::SolidPattern));
        circle->setSpanAngle((1.0 - percent) * 360.0 * 16.0);
        circle->setStartAngle(180.0 * 16.0 - (1.0 - percent) * 360.0 * 16.0);
        // active
        pen.setColor(_activeColor);
        circle = scene->addEllipse(0.0, 0.0, _width, _height, pen, QBrush(_activeColor, Qt::SolidPattern));
        circle->setSpanAngle(percent * 360.0 * 16.0);
        circle->setStartAngle(180.0 * 16.0);
        // scale
        scale[0] = (int)_direction;
        break;
    default:
        pen.setWidth(_height);
        // inactive
        pen.setColor(_inactiveColor);
        scene->addLine(0.0, 0.5 * _height, _width, 0.5 * _height, pen);
        // active
        pen.setColor(_activeColor);
        scene->addLine(-0.5 * _height, 0.5 * _height, percent * _width - 0.5 * _height, 0.5 * _height, pen);
        // scale
        scale[0] = (int)_direction;
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


QColor GraphicalItem::getActiveColor()
{
    if (debug) qDebug() << PDEBUG;

    return _activeColor;
}


QColor GraphicalItem::getInactiveColor()
{
    if (debug) qDebug() << PDEBUG;

    return _inactiveColor;
}


GraphicalItem::Type GraphicalItem::getType()
{
    if (debug) qDebug() << PDEBUG;

    return _type;
}


GraphicalItem::Direction GraphicalItem::getDirection()
{
    if (debug) qDebug() << PDEBUG;

    return _direction;
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


void GraphicalItem::setActiveColor(const QColor color)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Color" << color;

    _activeColor = color;
}


void GraphicalItem::setInactiveColor(const QColor color)
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
        QStringList childKeys = settings.childKeys();
        for (int i=0; i<childKeys.count(); i++) {
            if (childKeys[i] == QString("Name")) {
                setName(settings.value(childKeys[i]).toString());
            } else if (childKeys[i] == QString("Comment")) {
                setComment(settings.value(childKeys[i]).toString());
            } else if (childKeys[i] == QString("X-AW-Value")) {
                setBar(settings.value(childKeys[i]).toString());
            } else if (childKeys[i] == QString("X-AW-ActiveColor")) {
                QStringList color = settings.value(childKeys[i]).toString().split(QChar(','));
                setActiveColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt(), color[3].toInt()));
            } else if (childKeys[i] == QString("X-AW-InactiveColor")) {
                QStringList color = settings.value(childKeys[i]).toString().split(QChar(','));
                setInactiveColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt(), color[3].toInt()));
            } else if (childKeys[i] == QString("X-AW-Type")) {
                setType(settings.value(childKeys[i]).toString());
            } else if (childKeys[i] == QString("X-AW-Direction")) {
                setDirection(settings.value(childKeys[i]).toString());
            } else if (childKeys[i] == QString("X-AW-Height")) {
                setHeight(settings.value(childKeys[i]).toInt());
            } else if (childKeys[i] == QString("X-AW-Width")) {
                setWidth(settings.value(childKeys[i]).toInt());
            }
        }
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
    ui->pushButton_activeColor->setText(QString("%1,%2,%3,%4")
                                        .arg(_activeColor.red()).arg(_activeColor.green())
                                        .arg(_activeColor.blue()).arg(_activeColor.alpha()));
    ui->pushButton_inactiveColor->setText(QString("%1,%2,%3,%4")
                                          .arg(_inactiveColor.red()).arg(_inactiveColor.green())
                                          .arg(_inactiveColor.blue()).arg(_inactiveColor.alpha()));
    ui->comboBox_type->setCurrentIndex((int)_type);
    ui->comboBox_direction->setCurrentIndex((int)_direction);
    ui->spinBox_height->setValue(_height);
    ui->spinBox_width->setValue(_width);

    int ret = exec();
    if (ret == 0) {
        QStringList colorText;
        setName(ui->label_nameValue->text());
        setComment(ui->label_comment->text());
        setBar(ui->comboBox_value->currentText());
        colorText = ui->pushButton_activeColor->text().split(QChar(','));
        setActiveColor(QColor(colorText[0].toInt(), colorText[1].toInt(),
                              colorText[2].toInt(), colorText[3].toInt()));
        colorText = ui->pushButton_inactiveColor->text().split(QChar(','));
        setInactiveColor(QColor(colorText[0].toInt(), colorText[1].toInt(),
                                colorText[2].toInt(), colorText[3].toInt()));
        setType(ui->comboBox_type->currentText());
        setDirection(ui->comboBox_direction->currentText());
        setHeight(ui->spinBox_height->value());
        setWidth(ui->spinBox_width->value());
        writeConfiguration();
    }
}


void GraphicalItem::tryDelete()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=0; i<dirs.count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << "Remove file" << fileName << QFile::remove(fileName);
}


void GraphicalItem::writeConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QSettings settings(dirs[0] + QDir::separator() + fileName, QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();
    settings.beginGroup(QString("Desktop Entry"));

    QString strValue;
    settings.setValue(QString("Encoding"), QString("UTF-8"));
    settings.setValue(QString("Name"), _name);
    settings.setValue(QString("Comment"), _comment);
    settings.setValue(QString("X-AW-Value"), _bar);
    settings.setValue(QString("X-AW-ActiveColor"), QString("%1,%2,%3,%4")
                      .arg(_activeColor.red()).arg(_activeColor.green())
                      .arg(_activeColor.blue()).arg(_activeColor.alpha()));
    settings.setValue(QString("X-AW-InactiveColor"), QString("%1,%2,%3,%4")
                      .arg(_inactiveColor.red()).arg(_inactiveColor.green())
                      .arg(_inactiveColor.blue()).arg(_inactiveColor.alpha()));
    switch(_type) {
    case Vertical:
        strValue = QString("Vertical");
        break;
    case Circle:
        strValue = QString("Circle");
        break;
    default:
        strValue = QString("Horizontal");
        break;
    }
    settings.setValue(QString("X-AW-Type"), strValue);
    switch (_direction) {
    case RightToLeft:
        strValue = QString("RightToLeft");
        break;
    default:
        strValue = QString("LeftToRight");
        break;
    }
    settings.setValue(QString("X-AW-Direction"), strValue);
    settings.setValue(QString("X-AW-Height"), _height);
    settings.setValue(QString("X-AW-Width"), _width);

    settings.endGroup();
    settings.sync();
}


void GraphicalItem::changeColor()
{
    if (debug) qDebug() << PDEBUG;

    QStringList colorText = ((QPushButton *)sender())->text().split(QChar(','));
    QColor color = QColor(colorText[0].toInt(), colorText[1].toInt(),
                          colorText[2].toInt(), colorText[3].toInt());
    QColor newColor = QColorDialog::getColor(color, 0, i18n("Select color"),
                                             QColorDialog::ShowAlphaChannel);

    colorText.clear();
    colorText.append(QString("%1").arg(newColor.red()));
    colorText.append(QString("%1").arg(newColor.green()));
    colorText.append(QString("%1").arg(newColor.blue()));
    colorText.append(QString("%1").arg(newColor.alpha()));
    ((QPushButton *)sender())->setText(colorText.join(QChar(',')));
}
