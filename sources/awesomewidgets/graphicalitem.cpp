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
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSettings>

#include "awdebug.h"
#include "graphicalitemhelper.h"


GraphicalItem::GraphicalItem(QWidget *parent, const QString &filePath)
    : AbstractExtItem(parent, filePath)
    , ui(new Ui::GraphicalItem)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!filePath.isEmpty())
        readConfiguration();
    ui->setupUi(this);
    translate();

    connect(ui->checkBox_custom, SIGNAL(stateChanged(int)), this,
            SLOT(changeValue(int)));
    connect(ui->comboBox_type, SIGNAL(currentIndexChanged(int)), this,
            SLOT(changeCountState(int)));
    connect(ui->toolButton_activeColor, SIGNAL(clicked()), this,
            SLOT(changeColor()));
    connect(ui->toolButton_inactiveColor, SIGNAL(clicked()), this,
            SLOT(changeColor()));
}


GraphicalItem::~GraphicalItem()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    delete m_scene;
    delete ui;
    delete m_helper;
}


GraphicalItem *GraphicalItem::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    GraphicalItem *item
        = new GraphicalItem(static_cast<QWidget *>(parent()), _fileName);
    copyDefaults(item);
    item->setActiveColor(activeColor());
    item->setBar(bar());
    item->setCount(count());
    item->setCustom(isCustom());
    item->setDirection(direction());
    item->setItemHeight(itemHeight());
    item->setInactiveColor(inactiveColor());
    item->setMaxValue(maxValue());
    item->setMinValue(minValue());
    item->setNumber(_number);
    item->setType(type());
    item->setItemWidth(itemWidth());

    return item;
}


QString GraphicalItem::image(const QVariant &value)
{
    qCDebug(LOG_LIB) << "Value" << value;

    m_scene->clear();
    int scale[2] = {1, 1};
    float converted
        = m_helper->getPercents(value.toFloat(), minValue(), maxValue());

    // paint
    switch (m_type) {
    case Type::Vertical:
        m_helper->paintVertical(converted);
        // scale
        scale[1] = -2 * static_cast<int>(direction()) + 1;
        break;
    case Type::Circle:
        m_helper->paintCircle(converted);
        // scale
        scale[0] = -2 * static_cast<int>(direction()) + 1;
        break;
    case Type::Graph:
        m_helper->paintGraph(converted);
        scale[0] = -2 * static_cast<int>(direction()) + 1;
        scale[1] = -1;
        break;
    case Type::Bars:
        m_helper->paintBars(converted);
        scale[0] = -2 * static_cast<int>(direction()) + 1;
        scale[1] = -1;
        break;
    case Type::Horizontal:
        m_helper->paintHorizontal(converted);
        // scale
        scale[0] = -2 * static_cast<int>(direction()) + 1;
        break;
    }

    // convert
    QPixmap pixmap
        = m_view->grab().transformed(QTransform().scale(scale[0], scale[1]));
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    pixmap.save(&buffer, "PNG");
    QString url = QString("<img src=\"data:image/png;base64,%1\"/>")
                      .arg(QString(byteArray.toBase64()));

    return url;
}


void GraphicalItem::initScene()
{
    // cleanup
    delete m_helper;
    delete m_scene;

    // init scene
    m_scene = new QGraphicsScene();
    m_scene->setBackgroundBrush(QBrush(Qt::NoBrush));
    // init view
    m_view = new QGraphicsView(m_scene);
    m_view->setStyleSheet(QString("background: transparent"));
    m_view->setContentsMargins(0, 0, 0, 0);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->resize(m_width + 5, m_height + 5);

    // init helper
    m_helper = new GraphicalItemHelper(this, m_scene);
    m_helper->setParameters(activeColor(), inactiveColor(), itemWidth(),
                            itemHeight(), count());
}


QString GraphicalItem::bar() const
{
    return m_bar;
}


QString GraphicalItem::activeColor() const
{
    return m_activeColor;
}


int GraphicalItem::count() const
{
    return m_count;
}


QString GraphicalItem::inactiveColor() const
{
    return m_inactiveColor;
}


bool GraphicalItem::isCustom() const
{
    return m_custom;
}


int GraphicalItem::itemHeight() const
{
    return m_height;
}


int GraphicalItem::itemWidth() const
{
    return m_width;
}


float GraphicalItem::maxValue() const
{
    return m_maxValue;
}


float GraphicalItem::minValue() const
{
    return m_minValue;
}


GraphicalItem::Type GraphicalItem::type() const
{
    return m_type;
}


QString GraphicalItem::strType() const
{
    QString value;
    switch (type()) {
    case Type::Vertical:
        value = QString("Vertical");
        break;
    case Type::Circle:
        value = QString("Circle");
        break;
    case Type::Graph:
        value = QString("Graph");
        break;
    case Type::Bars:
        value = QString("Bars");
        break;
    case Type::Horizontal:
        value = QString("Horizontal");
        break;
    }

    return value;
}


GraphicalItem::Direction GraphicalItem::direction() const
{
    return m_direction;
}


QString GraphicalItem::strDirection() const
{
    QString value;
    switch (direction()) {
    case Direction::RightToLeft:
        value = QString("RightToLeft");
        break;
    case Direction::LeftToRight:
        value = QString("LeftToRight");
        break;
    }

    return value;
}


QStringList GraphicalItem::usedKeys() const
{
    return m_usedKeys;
}


QString GraphicalItem::uniq() const
{
    return bar();
}


void GraphicalItem::setBar(const QString &_bar)
{
    qCDebug(LOG_LIB) << "Bar" << _bar;

    m_bar = _bar;
}


void GraphicalItem::setActiveColor(const QString &_color)
{
    qCDebug(LOG_LIB) << "Color" << _color;

    m_activeColor = _color;
}


void GraphicalItem::setCount(const int _count)
{
    qCDebug(LOG_LIB) << "Count" << _count;
    if (_count <= 1)
        return;

    m_count = _count;
}


void GraphicalItem::setCustom(const bool _custom)
{
    qCDebug(LOG_LIB) << "Use custom tag" << _custom;

    m_custom = _custom;
}


void GraphicalItem::setInactiveColor(const QString &_color)
{
    qCDebug(LOG_LIB) << "Color" << _color;

    m_inactiveColor = _color;
}


void GraphicalItem::setItemHeight(const int _height)
{
    qCDebug(LOG_LIB) << "Height" << _height;
    if (_height <= 0)
        return;

    m_height = _height;
}


void GraphicalItem::setItemWidth(const int _width)
{
    qCDebug(LOG_LIB) << "Width" << _width;
    if (_width <= 0)
        return;

    m_width = _width;
}


void GraphicalItem::setMaxValue(const float _value)
{
    qCDebug(LOG_LIB) << "Max value" << _value;

    m_maxValue = _value;
}


void GraphicalItem::setMinValue(const float _value)
{
    qCDebug(LOG_LIB) << "Min value" << _value;

    m_minValue = _value;
}


void GraphicalItem::setType(const Type _type)
{
    qCDebug(LOG_LIB) << "Type" << static_cast<int>(_type);

    m_type = _type;
}


void GraphicalItem::setStrType(const QString &_type)
{
    qCDebug(LOG_LIB) << "Type" << _type;

    if (_type == QString("Vertical"))
        setType(Type::Vertical);
    else if (_type == QString("Circle"))
        setType(Type::Circle);
    else if (_type == QString("Graph"))
        setType(Type::Graph);
    else if (_type == QString("Bars"))
        setType(Type::Bars);
    else
        setType(Type::Horizontal);
}


void GraphicalItem::setDirection(const Direction _direction)
{
    qCDebug(LOG_LIB) << "Direction" << static_cast<int>(_direction);

    m_direction = _direction;
}


void GraphicalItem::setStrDirection(const QString &_direction)
{
    qCDebug(LOG_LIB) << "Direction" << _direction;

    if (_direction == QString("RightToLeft"))
        setDirection(Direction::RightToLeft);
    else
        setDirection(Direction::LeftToRight);
}


void GraphicalItem::setUsedKeys(const QStringList &_usedKeys)
{
    qCDebug(LOG_LIB) << "Used keys" << _usedKeys;

    // remove dubs
    // HACK converting to set may break order
    usedKeys().clear();
    for (auto &key : _usedKeys) {
        if (usedKeys().contains(key))
            continue;
        usedKeys().append(key);
    }
}


void GraphicalItem::readConfiguration()
{
    AbstractExtItem::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup(QString("Desktop Entry"));
    setCount(settings.value(QString("X-AW-Count"), count()).toInt());
    setCustom(settings.value(QString("X-AW-Custom"), isCustom()).toBool());
    setBar(settings.value(QString("X-AW-Value"), bar()).toString());
    setMaxValue(settings.value(QString("X-AW-Max"), maxValue()).toFloat());
    setMinValue(settings.value(QString("X-AW-Min"), minValue()).toFloat());
    setActiveColor(
        settings.value(QString("X-AW-ActiveColor"), activeColor()).toString());
    setInactiveColor(
        settings.value(QString("X-AW-InactiveColor"), inactiveColor())
            .toString());
    setStrType(settings.value(QString("X-AW-Type"), strType()).toString());
    setStrDirection(
        settings.value(QString("X-AW-Direction"), strDirection()).toString());
    setItemHeight(settings.value(QString("X-AW-Height"), itemHeight()).toInt());
    setItemWidth(settings.value(QString("X-AW-Width"), itemWidth()).toInt());
    // api == 5
    if (apiVersion() < 5) {
        QString prefix;
        prefix = activeColor().startsWith(QString("/")) ? QString("file://%1")
                                                        : QString("color://%1");
        m_activeColor = prefix.arg(activeColor());
        prefix = inactiveColor().startsWith(QString("/"))
                     ? QString("file://%1")
                     : QString("color://%1");
        m_inactiveColor = prefix.arg(inactiveColor());
    }
    settings.endGroup();

    bumpApi(AW_GRAPHITEM_API);
    initScene();
}


int GraphicalItem::showConfiguration(const QVariant &args)
{
    qCDebug(LOG_LIB) << "Combobox arguments" << args;
    QStringList tags = args.toStringList();

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->checkBox_custom->setChecked(isCustom());
    ui->comboBox_value->addItems(tags);
    if (isCustom()) {
        ui->lineEdit_customValue->setText(bar());
    } else {
        ui->comboBox_value->addItem(bar());
        ui->comboBox_value->setCurrentIndex(ui->comboBox_value->count() - 1);
    }
    ui->doubleSpinBox_max->setValue(maxValue());
    ui->doubleSpinBox_min->setValue(minValue());
    ui->spinBox_count->setValue(count());
    if (m_helper->isColor(activeColor()))
        ui->comboBox_activeImageType->setCurrentIndex(0);
    else
        ui->comboBox_activeImageType->setCurrentIndex(1);
    ui->lineEdit_activeColor->setText(activeColor());
    if (m_helper->isColor(inactiveColor()))
        ui->comboBox_inactiveImageType->setCurrentIndex(0);
    else
        ui->comboBox_inactiveImageType->setCurrentIndex(1);
    ui->lineEdit_inactiveColor->setText(inactiveColor());
    ui->comboBox_type->setCurrentIndex(static_cast<int>(type()));
    ui->comboBox_direction->setCurrentIndex(static_cast<int>(direction()));
    ui->spinBox_height->setValue(itemHeight());
    ui->spinBox_width->setValue(itemWidth());

    // update UI
    emit(ui->comboBox_type->currentIndexChanged(
        ui->comboBox_type->currentIndex()));
    emit(ui->checkBox_custom->stateChanged(ui->checkBox_custom->checkState()));

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setApiVersion(AW_GRAPHITEM_API);
    setCount(ui->spinBox_count->value());
    setCustom(ui->checkBox_custom->isChecked());
    setBar(m_custom ? ui->lineEdit_customValue->text()
                    : ui->comboBox_value->currentText());
    setMaxValue(ui->doubleSpinBox_max->value());
    setMinValue(ui->doubleSpinBox_min->value());
    setActiveColor(ui->lineEdit_activeColor->text());
    setInactiveColor(ui->lineEdit_inactiveColor->text());
    setType(static_cast<Type>(ui->comboBox_type->currentIndex()));
    setDirection(
        static_cast<Direction>(ui->comboBox_direction->currentIndex()));
    setItemHeight(ui->spinBox_height->value());
    setItemWidth(ui->spinBox_width->value());

    writeConfiguration();
    return ret;
}


void GraphicalItem::writeConfiguration() const
{
    AbstractExtItem::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-Value"), bar());
    settings.setValue(QString("X-AW-Count"), count());
    settings.setValue(QString("X-AW-Custom"), isCustom());
    settings.setValue(QString("X-AW-Max"), maxValue());
    settings.setValue(QString("X-AW-Min"), minValue());
    settings.setValue(QString("X-AW-ActiveColor"), activeColor());
    settings.setValue(QString("X-AW-InactiveColor"), inactiveColor());
    settings.setValue(QString("X-AW-Type"), strType());
    settings.setValue(QString("X-AW-Direction"), strDirection());
    settings.setValue(QString("X-AW-Height"), itemHeight());
    settings.setValue(QString("X-AW-Width"), itemWidth());
    settings.endGroup();

    settings.sync();
}


void GraphicalItem::changeColor()
{
    QLineEdit *lineEdit;
    int state;
    if (sender() == ui->toolButton_activeColor) {
        lineEdit = ui->lineEdit_activeColor;
        state = ui->comboBox_activeImageType->currentIndex();
    } else {
        lineEdit = ui->lineEdit_inactiveColor;
        state = ui->comboBox_inactiveImageType->currentIndex();
    }
    qCInfo(LOG_LIB) << "Using state" << state << "and lineEdit" << lineEdit;

    QString outputColor;
    if (state == 0) {
        QColor color = m_helper->stringToColor(lineEdit->text());
        QColor newColor = QColorDialog::getColor(
            color, this, tr("Select color"), QColorDialog::ShowAlphaChannel);
        if (!newColor.isValid())
            return;
        qCInfo(LOG_LIB) << "Selected color" << newColor;

        QStringList colorText;
        colorText.append(QString("%1").arg(newColor.red()));
        colorText.append(QString("%1").arg(newColor.green()));
        colorText.append(QString("%1").arg(newColor.blue()));
        colorText.append(QString("%1").arg(newColor.alpha()));

        outputColor = QString("color://%1").arg(colorText.join(QChar(',')));
    } else if (state == 1) {
        QString path = lineEdit->text();
        QString directory = QFileInfo(path).absolutePath();
        outputColor = QFileDialog::getOpenFileUrl(
                          this, tr("Select path"), directory,
                          tr("Images (*.png *.bpm *.jpg);;All files (*.*)"))
                          .toString();

        qCInfo(LOG_LIB) << "Selected path" << outputColor;
    }

    if (outputColor.isEmpty()) {
        qCWarning(LOG_LIB) << "Empty color selected, skipping";
        return;
    }
    return lineEdit->setText(outputColor);
}


void GraphicalItem::changeCountState(const int state)
{
    qCDebug(LOG_LIB) << "Current state is" << state;

    // 3 is magic number. Actually 3 is Graph mode
    ui->widget_count->setHidden(state != 3);
}


void GraphicalItem::changeValue(const int state)
{
    qCDebug(LOG_LIB) << "Current state is" << state;

    ui->widget_value->setHidden(state != Qt::Unchecked);
    ui->widget_customValue->setHidden(state == Qt::Unchecked);
}


void GraphicalItem::translate()
{
    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_number->setText(i18n("Tag"));
    ui->checkBox_custom->setText(i18n("Use custom formula"));
    ui->label_value->setText(i18n("Value"));
    ui->label_customValue->setText(i18n("Value"));
    ui->label_max->setText(i18n("Max value"));
    ui->label_min->setText(i18n("Min value"));
    ui->label_activeImageType->setText(i18n("Active filling type"));
    ui->label_inactiveImageType->setText(i18n("Inctive filling type"));
    ui->label_type->setText(i18n("Type"));
    ui->label_count->setText(i18n("Points count"));
    ui->label_direction->setText(i18n("Direction"));
    ui->label_height->setText(i18n("Height"));
    ui->label_width->setText(i18n("Width"));

    ui->comboBox_activeImageType->clear();
    ui->comboBox_activeImageType->addItem(i18n("color"));
    ui->comboBox_activeImageType->addItem(i18n("image"));
    ui->comboBox_inactiveImageType->clear();
    ui->comboBox_inactiveImageType->addItem(i18n("color"));
    ui->comboBox_inactiveImageType->addItem(i18n("image"));
}
