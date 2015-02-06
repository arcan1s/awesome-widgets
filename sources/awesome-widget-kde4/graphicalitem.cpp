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

#include "version.h"


GraphicalItem::GraphicalItem(QWidget *parent, const QString desktopName, const QStringList directories, const bool debugCmd)
    : QDialog(parent),
      m_fileName(desktopName),
      m_dirs(directories),
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


QString GraphicalItem::image(const float value)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Value" << value;
    if (m_bar == QString("none")) return QString("");

    QColor active = stringToColor(m_activeColor);
    QColor inactive = stringToColor(m_inactiveColor);
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
    view->resize(m_width + 5.0, m_height + 5.0);

    // paint
    switch(m_type) {
    case Vertical:
        pen.setWidth(m_width);
        // inactive
        pen.setColor(inactive);
        scene->addLine(0.5 * m_width, -0.5 * m_width, 0.5 * m_width,
                       (1.0 - percent) * m_height - 0.5 * m_width, pen);
        // active
        pen.setColor(active);
        scene->addLine(0.5 * m_width, (1.0 - percent) * m_height + 0.5 * m_width,
                       0.5 * m_width, m_height + 0.5 * m_width, pen);
        // scale
        scale[1] = -2 * static_cast<int>(m_direction) + 1;
        break;
    case Circle:
        QGraphicsEllipseItem *circle;
        pen.setWidth(1.0);
        // inactive
        pen.setColor(inactive);
        circle = scene->addEllipse(0.0, 0.0, m_width, m_height, pen, QBrush(inactive, Qt::SolidPattern));
        circle->setSpanAngle(- (1.0 - percent) * 360.0 * 16.0);
        circle->setStartAngle(90.0 * 16.0 - percent * 360.0 * 16.0);
        // active
        pen.setColor(active);
        circle = scene->addEllipse(0.0, 0.0, m_width, m_height, pen, QBrush(active, Qt::SolidPattern));
        circle->setSpanAngle(- percent * 360.0 * 16.0);
        circle->setStartAngle(90.0 * 16.0);
        // scale
        scale[0] = -2 * static_cast<int>(m_direction) + 1;
        break;
    default:
        pen.setWidth(m_height);
        // inactive
        pen.setColor(inactive);
        scene->addLine(percent * m_width + 0.5 * m_height, 0.5 * m_height,
                       m_width + 0.5 * m_height, 0.5 * m_height, pen);
        // active
        pen.setColor(active);
        scene->addLine(-0.5 * m_height, 0.5 * m_height,
                       percent * m_width - 0.5 * m_height, 0.5 * m_height, pen);
        // scale
        scale[0] = -2 * static_cast<int>(m_direction) + 1;
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


QString GraphicalItem::fileName()
{
    if (debug) qDebug() << PDEBUG;

    return m_fileName;
}


int GraphicalItem::apiVersion()
{
    if (debug) qDebug() << PDEBUG;

    return m_apiVersion;
}


QString GraphicalItem::name()
{
    if (debug) qDebug() << PDEBUG;

    return m_name;
}


QString GraphicalItem::comment()
{
    if (debug) qDebug() << PDEBUG;

    return m_comment;
}


QString GraphicalItem::bar()
{
    if (debug) qDebug() << PDEBUG;

    return m_bar;
}


QString GraphicalItem::activeColor()
{
    if (debug) qDebug() << PDEBUG;

    return m_activeColor;
}


QString GraphicalItem::inactiveColor()
{
    if (debug) qDebug() << PDEBUG;

    return m_inactiveColor;
}


GraphicalItem::Type GraphicalItem::type()
{
    if (debug) qDebug() << PDEBUG;

    return m_type;
}


QString GraphicalItem::strType()
{
    if (debug) qDebug() << PDEBUG;

    QString value;
    switch(m_type) {
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


GraphicalItem::Direction GraphicalItem::direction()
{
    if (debug) qDebug() << PDEBUG;

    return m_direction;
}


QString GraphicalItem::strDirection()
{
    if (debug) qDebug() << PDEBUG;

    QString value;
    switch (m_direction) {
    case RightToLeft:
        value = QString("RightToLeft");
        break;
    default:
        value = QString("LeftToRight");
        break;
    }

    return value;
}


int GraphicalItem::height()
{
    if (debug) qDebug() << PDEBUG;

    return m_height;
}


int GraphicalItem::width()
{
    if (debug) qDebug() << PDEBUG;

    return m_width;
}


void GraphicalItem::setApiVersion(const int _apiVersion)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Version" << _apiVersion;

    m_apiVersion = _apiVersion;
}


void GraphicalItem::setName(const QString _name)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Name" << _name;

    m_name = _name;
}


void GraphicalItem::setComment(const QString _comment)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Comment" << _comment;

    m_comment = _comment;
}


void GraphicalItem::setBar(const QString _bar)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Bar" << _bar;

    if ((!_bar.contains(QRegExp(QString("cpu(?!cl).*")))) &&
        (!_bar.contains(QRegExp(QString("gpu")))) &&
        (!_bar.contains(QRegExp(QString("mem")))) &&
        (!_bar.contains(QRegExp(QString("swap")))) &&
        (!_bar.contains(QRegExp(QString("hdd[0-9].*")))) &&
        (!_bar.contains(QRegExp(QString("bat.*")))))
        m_bar = QString("none");
    else
        m_bar = _bar;
}


void GraphicalItem::setActiveColor(const QString _color)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Color" << _color;

    m_activeColor = _color;
}


void GraphicalItem::setInactiveColor(const QString _color)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Color" << _color;

    m_inactiveColor = _color;
}


void GraphicalItem::setType(const Type _type)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Type" << _type;

    m_type = _type;
}


void GraphicalItem::setStrType(const QString _type)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Type" << _type;

    if (_type == QString("Vertical"))
        setType(Vertical);
    else if (_type == QString("Circle"))
        setType(Circle);
    else
        setType(Horizontal);
}


void GraphicalItem::setDirection(const Direction _direction)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Direction" << _direction;

    m_direction = _direction;
}


void GraphicalItem::setStrDirection(const QString _direction)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Direction" << _direction;

    if (_direction == QString("RightToLeft"))
        setDirection(RightToLeft);
    else
        setDirection(LeftToRight);
}


void GraphicalItem::setHeight(const int _height)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Height" << _height;
    if (_height <= 0) return;

    m_height = _height;
}


void GraphicalItem::setWidth(const int _width)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Width" << _width;
    if (_width <= 0) return;

    m_width = _width;
}


void GraphicalItem::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=m_dirs.count()-1; i>=0; i--) {
        if (!QDir(m_dirs[i]).entryList(QDir::Files).contains(m_fileName)) continue;
        QSettings settings(m_dirs[i] + QDir::separator() + m_fileName, QSettings::IniFormat);

        settings.beginGroup(QString("Desktop Entry"));
        setName(settings.value(QString("Name"), m_name).toString());
        setComment(settings.value(QString("Comment"), m_comment).toString());
        setApiVersion(settings.value(QString("X-AW-ApiVersion"), AWGIAPI).toInt());
        setBar(settings.value(QString("X-AW-Value"), m_bar).toString());
        setActiveColor(settings.value(QString("X-AW-ActiveColor"), m_activeColor).toString());
        setInactiveColor(settings.value(QString("X-AW-InactiveColor"), m_inactiveColor).toString());
        setStrType(settings.value(QString("X-AW-Type"), strType()).toString());
        setStrDirection(settings.value(QString("X-AW-Direction"), strDirection()).toString());
        setHeight(settings.value(QString("X-AW-Height"), m_height).toInt());
        setWidth(settings.value(QString("X-AW-Width"), m_width).toInt());
        settings.endGroup();
    }
}


void GraphicalItem::showConfiguration(const QStringList tags)
{
    if (debug) qDebug() << PDEBUG;

    ui->label_nameValue->setText(m_name);
    ui->lineEdit_comment->setText(m_comment);
    ui->comboBox_value->addItems(tags);
    ui->comboBox_value->addItem(m_bar);
    ui->comboBox_value->setCurrentIndex(ui->comboBox_value->count() - 1);
    ui->pushButton_activeColor->setText(m_activeColor);
    ui->pushButton_inactiveColor->setText(m_inactiveColor);
    ui->comboBox_type->setCurrentIndex(static_cast<int>(m_type));
    ui->comboBox_direction->setCurrentIndex(static_cast<int>(m_direction));
    ui->spinBox_height->setValue(m_height);
    ui->spinBox_width->setValue(m_width);

    int ret = exec();
    if (ret != 1) return;

    setName(ui->label_nameValue->text());
    setComment(ui->lineEdit_comment->text());
    setApiVersion(AWGIAPI);
    setBar(ui->comboBox_value->currentText());
    setActiveColor(ui->pushButton_activeColor->text().remove(QChar('&')));
    setInactiveColor(ui->pushButton_inactiveColor->text().remove(QChar('&')));
    setStrType(ui->comboBox_type->currentText());
    setStrDirection(ui->comboBox_direction->currentText());
    setHeight(ui->spinBox_height->value());
    setWidth(ui->spinBox_width->value());

    writeConfiguration();
}


void GraphicalItem::tryDelete()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=0; i<m_dirs.count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << "Remove file" << m_dirs[i] + QDir::separator() + m_fileName <<
                               QFile::remove(m_dirs[i] + QDir::separator() + m_fileName);
}


void GraphicalItem::writeConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QSettings settings(m_dirs[0] + QDir::separator() + m_fileName, QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("Encoding"), QString("UTF-8"));
    settings.setValue(QString("Name"), m_name);
    settings.setValue(QString("Comment"), m_comment);
    settings.setValue(QString("X-AW-ApiVersion"), m_apiVersion);
    settings.setValue(QString("X-AW-Value"), m_bar);
    settings.setValue(QString("X-AW-ActiveColor"), m_activeColor);
    settings.setValue(QString("X-AW-InactiveColor"), m_inactiveColor);
    settings.setValue(QString("X-AW-Type"), strType());
    settings.setValue(QString("X-AW-Direction"), strDirection());
    settings.setValue(QString("X-AW-Height"), m_height);
    settings.setValue(QString("X-AW-Width"), m_width);
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
    dynamic_cast<QPushButton *>(sender())->setText(colorText.join(QChar(',')));
}


QColor GraphicalItem::stringToColor(const QString _color)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Color" << _color;

    QColor qcolor;
    QStringList listColor = _color.split(QChar(','));
    while (listColor.count() < 4)
        listColor.append(QString("0"));
    qcolor.setRed(listColor[0].toInt());
    qcolor.setGreen(listColor[1].toInt());
    qcolor.setBlue(listColor[2].toInt());
    qcolor.setAlpha(listColor[3].toInt());

    return qcolor;
}
