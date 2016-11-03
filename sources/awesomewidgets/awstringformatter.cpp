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


#include "awstringformatter.h"
#include "ui_awstringformatter.h"

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QSettings>

#include "awdebug.h"


AWStringFormatter::AWStringFormatter(QWidget *parent, const QString filePath)
    : AWAbstractFormatter(parent, filePath)
    , ui(new Ui::AWStringFormatter)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!filePath.isEmpty())
        readConfiguration();
    ui->setupUi(this);
    translate();
}


AWStringFormatter::~AWStringFormatter()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    delete ui;
}


QString AWStringFormatter::convert(const QVariant &_value) const
{
    qCDebug(LOG_LIB) << "Convert value" << _value;

    QString output = QString("%1").arg(_value.toString(), count(), fillChar());
    if (forceWidth())
        output = output.left(count());

    return output;
}


AWStringFormatter *AWStringFormatter::copy(const QString _fileName,
                                           const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    AWStringFormatter *item
        = new AWStringFormatter(static_cast<QWidget *>(parent()), _fileName);
    AWAbstractFormatter::copyDefaults(item);
    item->setCount(count());
    item->setFillChar(fillChar());
    item->setForceWidth(forceWidth());
    item->setNumber(_number);

    return item;
}


int AWStringFormatter::count() const
{
    return m_count;
}


QChar AWStringFormatter::fillChar() const
{
    return m_fillChar;
}


bool AWStringFormatter::forceWidth() const
{
    return m_forceWidth;
}


void AWStringFormatter::setCount(const int _count)
{
    qCDebug(LOG_LIB) << "Set width" << _count;

    m_count = _count;
}


void AWStringFormatter::setFillChar(const QChar _fillChar)
{
    qCDebug(LOG_LIB) << "Set char" << _fillChar;

    m_fillChar = _fillChar;
}


void AWStringFormatter::setForceWidth(const bool _forceWidth)
{
    qCDebug(LOG_LIB) << "Set force strip" << _forceWidth;

    m_forceWidth = _forceWidth;
}


void AWStringFormatter::readConfiguration()
{
    AWAbstractFormatter::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup(QString("Desktop Entry"));
    setCount(settings.value(QString("X-AW-Width"), count()).toInt());
    setFillChar(
        settings.value(QString("X-AW-FillChar"), fillChar()).toString().at(0));
    setForceWidth(
        settings.value(QString("X-AW-ForceWidth"), forceWidth()).toBool());
    settings.endGroup();

    bumpApi(AW_FORMATTER_API);
}


int AWStringFormatter::showConfiguration(const QVariant args)
{
    Q_UNUSED(args)

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText(QString("String"));
    ui->spinBox_width->setValue(count());
    ui->lineEdit_fill->setText(QString(fillChar()));
    ui->checkBox_forceWidth->setCheckState(forceWidth() ? Qt::Checked
                                                        : Qt::Unchecked);

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setApiVersion(AW_FORMATTER_API);
    setStrType(ui->label_typeValue->text());
    setCount(ui->spinBox_width->value());
    setFillChar(ui->lineEdit_fill->text().at(0));
    setForceWidth(ui->checkBox_forceWidth->checkState() == Qt::Checked);

    writeConfiguration();
    return ret;
}


void AWStringFormatter::writeConfiguration() const
{
    AWAbstractFormatter::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-Width"), count());
    settings.setValue(QString("X-AW-FillChar"), fillChar());
    settings.setValue(QString("X-AW-ForceWidth"), forceWidth());
    settings.endGroup();

    settings.sync();
}


void AWStringFormatter::translate()
{
    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_type->setText(i18n("Type"));
    ui->label_width->setText(i18n("Width"));
    ui->label_fill->setText(i18n("Fill char"));
    ui->checkBox_forceWidth->setText(i18n("Force width"));
}
