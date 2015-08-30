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

#include <KI18n/KLocalizedString>

#include <QBuffer>
#include <QColorDialog>
#include <QDir>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSettings>

#include "awdebug.h"
#include "version.h"


GraphicalItem::GraphicalItem(QWidget *parent, const QString desktopName,
                             const QStringList directories)
    : AbstractExtItem(parent, desktopName, directories),
      ui(new Ui::GraphicalItem)
{
    qCDebug(LOG_LIB);

    readConfiguration();
    ui->setupUi(this);
    translate();

    connect(ui->pushButton_activeColor, SIGNAL(clicked()), this, SLOT(changeColor()));
    connect(ui->pushButton_inactiveColor, SIGNAL(clicked()), this, SLOT(changeColor()));
}


GraphicalItem::~GraphicalItem()
{
    qCDebug(LOG_LIB);

    delete ui;
}


GraphicalItem *GraphicalItem::copy(const QString _fileName, const int _number)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "File" << _fileName;
    qCDebug(LOG_LIB) << "Number" << _number;

    GraphicalItem *item = new GraphicalItem(static_cast<QWidget *>(parent()), _fileName, directories());
    item->setActive(isActive());
    item->setActiveColor(activeColor());
    item->setApiVersion(apiVersion());
    item->setBar(bar());
    item->setComment(comment());
    item->setDirection(direction());
    item->setHeight(height());
    item->setInactiveColor(inactiveColor());
    item->setInterval(interval());
    item->setName(QString("bar%1").arg(_number));
    item->setNumber(_number);
    item->setType(type());
    item->setWidth(width());

    return item;
}


QString GraphicalItem::image(const float value) const
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Value" << value;
    if (m_bar == QString("none")) return QString("");

    QColor active = stringToColor(m_activeColor);
    QColor inactive = stringToColor(m_inactiveColor);
    float percent = value / 100.0;
    int scale[2] = { 1, 1 };
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
    switch (m_type) {
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
    QPixmap pixmap = view->grab().transformed(QTransform().scale(scale[0], scale[1]));
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    pixmap.save(&buffer, "PNG");
    QString url = QString("<img src=\"data:image/png;base64,%1\"/>").arg(QString(byteArray.toBase64()));
    delete view;
    delete scene;

    return url;
}


QString GraphicalItem::bar() const
{
    qCDebug(LOG_LIB);

    return m_bar;
}


QString GraphicalItem::activeColor() const
{
    qCDebug(LOG_LIB);

    return m_activeColor;
}


QString GraphicalItem::inactiveColor() const
{
    qCDebug(LOG_LIB);

    return m_inactiveColor;
}


QString GraphicalItem::tag() const
{
    qCDebug(LOG_LIB);

    return name() + m_bar;
}


GraphicalItem::Type GraphicalItem::type() const
{
    qCDebug(LOG_LIB);

    return m_type;
}


QString GraphicalItem::strType() const
{
    qCDebug(LOG_LIB);

    QString value;
    switch (m_type) {
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


GraphicalItem::Direction GraphicalItem::direction() const
{
    qCDebug(LOG_LIB);

    return m_direction;
}


QString GraphicalItem::strDirection() const
{
    qCDebug(LOG_LIB);

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


int GraphicalItem::height() const
{
    qCDebug(LOG_LIB);

    return m_height;
}


int GraphicalItem::width() const
{
    qCDebug(LOG_LIB);

    return m_width;
}


QString GraphicalItem::uniq() const
{
    qCDebug(LOG_LIB);

    return m_bar;
}


void GraphicalItem::setBar(const QString _bar)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Bar" << _bar;

    if (!_bar.contains(QRegExp(QString("^(cpu(?!cl).*|gpu$|mem$|swap$|hdd[0-9].*|bat.*)")))) {
        qCWarning(LOG_LIB) << "Unsupported bar type" << _bar;
        m_bar = QString("none");
    } else
        m_bar = _bar;
}


void GraphicalItem::setActiveColor(const QString _color)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Color" << _color;

    m_activeColor = _color;
}


void GraphicalItem::setInactiveColor(const QString _color)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Color" << _color;

    m_inactiveColor = _color;
}


void GraphicalItem::setType(const Type _type)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Type" << _type;

    m_type = _type;
}


void GraphicalItem::setStrType(const QString _type)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Type" << _type;

    if (_type == QString("Vertical"))
        setType(Vertical);
    else if (_type == QString("Circle"))
        setType(Circle);
    else
        setType(Horizontal);
}


void GraphicalItem::setDirection(const Direction _direction)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Direction" << _direction;

    m_direction = _direction;
}


void GraphicalItem::setStrDirection(const QString _direction)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Direction" << _direction;

    if (_direction == QString("RightToLeft"))
        setDirection(RightToLeft);
    else
        setDirection(LeftToRight);
}


void GraphicalItem::setHeight(const int _height)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Height" << _height;
    if (_height <= 0) return;

    m_height = _height;
}


void GraphicalItem::setWidth(const int _width)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Width" << _width;
    if (_width <= 0) return;

    m_width = _width;
}


void GraphicalItem::readConfiguration()
{
    qCDebug(LOG_LIB);
    AbstractExtItem::readConfiguration();

    for (int i=directories().count()-1; i>=0; i--) {
        if (!QDir(directories().at(i)).entryList(QDir::Files).contains(fileName())) continue;
        QSettings settings(QString("%1/%2").arg(directories().at(i)).arg(fileName()), QSettings::IniFormat);

        settings.beginGroup(QString("Desktop Entry"));
        setBar(settings.value(QString("X-AW-Value"), m_bar).toString());
        setActiveColor(settings.value(QString("X-AW-ActiveColor"), m_activeColor).toString());
        setInactiveColor(settings.value(QString("X-AW-InactiveColor"), m_inactiveColor).toString());
        setStrType(settings.value(QString("X-AW-Type"), strType()).toString());
        setStrDirection(settings.value(QString("X-AW-Direction"), strDirection()).toString());
        setHeight(settings.value(QString("X-AW-Height"), m_height).toInt());
        setWidth(settings.value(QString("X-AW-Width"), m_width).toInt());
        // api == 2
        if (apiVersion() < 2) setNumber(bar().remove(QString("bar")).toInt());
        settings.endGroup();
    }

    // update for current API
    if ((apiVersion() > 0) && (apiVersion() < AWGIAPI)) {
        qCWarning(LOG_LIB) << "Bump API version from" << apiVersion() << "to" << AWGIAPI;
        setApiVersion(AWGIAPI);
        writeConfiguration();
    }
}


QVariantHash GraphicalItem::run()
{
    qCDebug(LOG_LIB);

    // required by abstract class
    return QVariantHash();
}


int GraphicalItem::showConfiguration(const QVariant args)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Combobox arguments" << args;
    QStringList tags = args.toStringList();

    ui->label_nameValue->setText(name());
    ui->lineEdit_comment->setText(comment());
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
    if (ret != 1) return ret;

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
    return ret;
}


void GraphicalItem::writeConfiguration() const
{
    qCDebug(LOG_LIB);
    AbstractExtItem::writeConfiguration();

    QSettings settings(QString("%1/%2").arg(directories().first()).arg(fileName()), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
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
    qCDebug(LOG_LIB);

    QColor color = stringToColor((static_cast<QPushButton *>(sender()))->text());
    QColor newColor = QColorDialog::getColor(color, this, tr("Select color"),
                                             QColorDialog::ShowAlphaChannel);
    if (!newColor.isValid()) return;
    qCInfo(LOG_LIB) << "Selected color" << newColor;

    QStringList colorText;
    colorText.append(QString("%1").arg(newColor.red()));
    colorText.append(QString("%1").arg(newColor.green()));
    colorText.append(QString("%1").arg(newColor.blue()));
    colorText.append(QString("%1").arg(newColor.alpha()));

    return static_cast<QPushButton *>(sender())->setText(colorText.join(QChar(',')));
}


QColor GraphicalItem::stringToColor(const QString _color) const
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Color" << _color;

    QColor qcolor;
    QStringList listColor = _color.split(QChar(','));
    while (listColor.count() < 4)
        listColor.append(QString("0"));
    qcolor.setRed(listColor.at(0).toInt());
    qcolor.setGreen(listColor.at(1).toInt());
    qcolor.setBlue(listColor.at(2).toInt());
    qcolor.setAlpha(listColor.at(3).toInt());

    return qcolor;
}


void GraphicalItem::translate()
{
    qCDebug(LOG_LIB);

    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_value->setText(i18n("Value"));
    ui->label_activeColor->setText(i18n("Active color"));
    ui->label_inactiveColor->setText(i18n("Inactive color"));
    ui->label_type->setText(i18n("Type"));
    ui->label_direction->setText(i18n("Direction"));
    ui->label_height->setText(i18n("Height"));
    ui->label_width->setText(i18n("Width"));
}
