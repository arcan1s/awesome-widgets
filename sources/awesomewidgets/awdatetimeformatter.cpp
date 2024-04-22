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

#include "awdatetimeformatter.h"
#include "ui_awdatetimeformatter.h"

#include <QDateTime>
#include <QSettings>

#include "awdebug.h"


AWDateTimeFormatter::AWDateTimeFormatter(QObject *_parent, const QString &_filePath)
    : AWAbstractFormatter(_parent, _filePath)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        AWDateTimeFormatter::readConfiguration();
}


QString AWDateTimeFormatter::convert(const QVariant &_value) const
{
    qCDebug(LOG_LIB) << "Convert value" << _value;

    return m_locale.toString(_value.toDateTime(), m_format);
}


AWDateTimeFormatter *AWDateTimeFormatter::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    auto item = new AWDateTimeFormatter(parent(), _fileName);
    AWAbstractFormatter::copyDefaults(item);

    item->setFormat(format());
    item->setTranslateString(translateString());
    item->setNumber(_number);

    return item;
}


QString AWDateTimeFormatter::format() const
{
    return m_format;
}


bool AWDateTimeFormatter::translateString() const
{
    return m_translate;
}


void AWDateTimeFormatter::setFormat(const QString &_format)
{
    qCDebug(LOG_LIB) << "Set format" << _format;

    m_format = _format;
}


void AWDateTimeFormatter::setTranslateString(const bool _translate)
{
    qCDebug(LOG_LIB) << "Set translate string" << _translate;

    m_translate = _translate;
    initLocale();
}


void AWDateTimeFormatter::readConfiguration()
{
    AWAbstractFormatter::readConfiguration();

    QSettings settings(filePath(), QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setFormat(settings.value("X-AW-Format", format()).toString());
    setTranslateString(settings.value("X-AW-Translate", translateString()).toBool());
    settings.endGroup();

    bumpApi(AW_FORMATTER_API);
}


int AWDateTimeFormatter::showConfiguration(QWidget *_parent, const QVariant &_args)
{
    Q_UNUSED(_args)

    auto dialog = new QDialog(_parent);
    auto ui = new Ui::AWDateTimeFormatter();
    ui->setupUi(dialog);

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText("DateTime");
    ui->lineEdit_format->setText(format());
    ui->checkBox_translate->setCheckState(translateString() ? Qt::Checked : Qt::Unchecked);

    auto ret = dialog->exec();
    if (ret == 1) {
        setName(ui->lineEdit_name->text());
        setComment(ui->lineEdit_comment->text());
        setApiVersion(AW_FORMATTER_API);
        setStrType(ui->label_typeValue->text());
        setFormat(ui->lineEdit_format->text());
        setTranslateString(ui->checkBox_translate->checkState() == Qt::Checked);

        writeConfiguration();
    }

    dialog->deleteLater();
    delete ui;

    return ret;
}


void AWDateTimeFormatter::writeConfiguration() const
{
    AWAbstractFormatter::writeConfiguration();

    QSettings settings(writableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("X-AW-Format", format());
    settings.setValue("X-AW-Translate", translateString());
    settings.endGroup();

    settings.sync();
}


void AWDateTimeFormatter::initLocale()
{
    m_locale = m_translate ? QLocale::system() : QLocale::c();
}
