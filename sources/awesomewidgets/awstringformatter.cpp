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

#include <QSettings>

#include "awdebug.h"


AWStringFormatter::AWStringFormatter(QObject *_parent, const QString &_filePath)
    : AWAbstractFormatter(_parent, _filePath)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        AWStringFormatter::readConfiguration();
}


QString AWStringFormatter::convert(const QVariant &_value) const
{
    qCDebug(LOG_LIB) << "Convert value" << _value;

    auto output = QString("%1").arg(_value.toString(), count(), fillChar());
    if (forceWidth())
        output = output.left(count());

    return output;
}


AWStringFormatter *AWStringFormatter::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    auto item = new AWStringFormatter(parent(), _fileName);
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


void AWStringFormatter::setFillChar(const QChar &_fillChar)
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

    QSettings settings(filePath(), QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setCount(settings.value("X-AW-Width", count()).toInt());
    setFillChar(settings.value("X-AW-FillChar", fillChar()).toString().at(0));
    setForceWidth(settings.value("X-AW-ForceWidth", forceWidth()).toBool());
    settings.endGroup();

    bumpApi(AW_FORMATTER_API);
}


int AWStringFormatter::showConfiguration(QWidget *_parent, const QVariant &_args)
{
    Q_UNUSED(_args)

    auto dialog = new QDialog(_parent);
    auto ui = new Ui::AWStringFormatter();
    ui->setupUi(dialog);

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText("String");
    ui->spinBox_width->setValue(count());
    ui->lineEdit_fill->setText(QString(fillChar()));
    ui->checkBox_forceWidth->setCheckState(forceWidth() ? Qt::Checked : Qt::Unchecked);

    auto ret = dialog->exec();
    if (ret == 1) {
        setName(ui->lineEdit_name->text());
        setComment(ui->lineEdit_comment->text());
        setApiVersion(AW_FORMATTER_API);
        setStrType(ui->label_typeValue->text());
        setCount(ui->spinBox_width->value());
        setFillChar(ui->lineEdit_fill->text().at(0));
        setForceWidth(ui->checkBox_forceWidth->checkState() == Qt::Checked);

        writeConfiguration();
    }

    dialog->deleteLater();
    delete ui;

    return ret;
}


void AWStringFormatter::writeConfiguration() const
{
    AWAbstractFormatter::writeConfiguration();

    QSettings settings(writableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("X-AW-Width", count());
    settings.setValue("X-AW-FillChar", fillChar());
    settings.setValue("X-AW-ForceWidth", forceWidth());
    settings.endGroup();

    settings.sync();
}
