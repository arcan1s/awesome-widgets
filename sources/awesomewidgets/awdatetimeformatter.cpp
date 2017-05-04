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

#include <KI18n/KLocalizedString>

#include <QDateTime>
#include <QDir>
#include <QSettings>

#include "awdebug.h"


AWDateTimeFormatter::AWDateTimeFormatter(QWidget *parent,
                                         const QString &filePath)
    : AWAbstractFormatter(parent, filePath)
    , ui(new Ui::AWDateTimeFormatter)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!filePath.isEmpty())
        readConfiguration();
    ui->setupUi(this);
    translate();
}


AWDateTimeFormatter::~AWDateTimeFormatter()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    delete ui;
}


QString AWDateTimeFormatter::convert(const QVariant &_value) const
{
    qCDebug(LOG_LIB) << "Convert value" << _value;

    return m_locale.toString(_value.toDateTime(), m_format);
}


AWDateTimeFormatter *AWDateTimeFormatter::copy(const QString &_fileName,
                                               const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    AWDateTimeFormatter *item
        = new AWDateTimeFormatter(static_cast<QWidget *>(parent()), _fileName);
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

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup(QString("Desktop Entry"));
    setFormat(settings.value(QString("X-AW-Format"), format()).toString());
    setTranslateString(
        settings.value(QString("X-AW-Translate"), translateString()).toBool());
    settings.endGroup();

    bumpApi(AW_FORMATTER_API);
}


int AWDateTimeFormatter::showConfiguration(const QVariant &args)
{
    Q_UNUSED(args)

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText(QString("DateTime"));
    ui->lineEdit_format->setText(format());
    ui->checkBox_translate->setCheckState(translateString() ? Qt::Checked
                                                            : Qt::Unchecked);

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setApiVersion(AW_FORMATTER_API);
    setStrType(ui->label_typeValue->text());
    setFormat(ui->lineEdit_format->text());
    setTranslateString(ui->checkBox_translate->checkState() == Qt::Checked);

    writeConfiguration();
    return ret;
}


void AWDateTimeFormatter::writeConfiguration() const
{
    AWAbstractFormatter::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-Format"), format());
    settings.setValue(QString("X-AW-Translate"), translateString());
    settings.endGroup();

    settings.sync();
}


void AWDateTimeFormatter::initLocale()
{
    m_locale = m_translate ? QLocale::system() : QLocale::c();
}


void AWDateTimeFormatter::translate()
{
    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_type->setText(i18n("Type"));
    ui->label_format->setText(i18n("Format"));
    ui->checkBox_translate->setText(i18n("Translate strings"));
}
