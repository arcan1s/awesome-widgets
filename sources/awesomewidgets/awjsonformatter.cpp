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


#include "awjsonformatter.h"
#include "ui_awjsonformatter.h"

#include <KI18n/KLocalizedString>

#include <QJsonDocument>
#include <QSettings>

#include "awdebug.h"


AWJsonFormatter::AWJsonFormatter(QWidget *_parent, const QString &_filePath)
    : AWAbstractFormatter(_parent, _filePath)
    , ui(new Ui::AWJsonFormatter)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        AWJsonFormatter::readConfiguration();
    ui->setupUi(this);
    AWJsonFormatter::translate();
}


AWJsonFormatter::~AWJsonFormatter()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    delete ui;
}


QString AWJsonFormatter::convert(const QVariant &_value) const
{
    qCDebug(LOG_LIB) << "Convert value" << _value;

    // check if _value is string and parse first if required
    QJsonDocument json = _value.type() == QVariant::String ? QJsonDocument::fromJson(_value.toString().toUtf8())
                                                           : QJsonDocument::fromVariant(_value);
    QVariant converted = json.toVariant();
    for (auto &element : m_splittedPath)
        converted = getFromJson(converted, element);

    return converted.toString();
}


AWJsonFormatter *AWJsonFormatter::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    auto *item = new AWJsonFormatter(dynamic_cast<QWidget *>(parent()), _fileName);
    AWAbstractFormatter::copyDefaults(item);
    item->setNumber(_number);
    item->setPath(path());

    return item;
}


QString AWJsonFormatter::path() const
{
    return m_path;
}


void AWJsonFormatter::setPath(const QString &_path)
{
    qCDebug(LOG_LIB) << "Path" << _path;

    m_path = _path;
    initPath();
}


void AWJsonFormatter::readConfiguration()
{
    AWAbstractFormatter::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setPath(settings.value("X-AW-Path", path()).toString());
    settings.endGroup();

    bumpApi(AW_FORMATTER_API);
}


int AWJsonFormatter::showConfiguration(const QVariant &args)
{
    Q_UNUSED(args)

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText("Json");
    ui->lineEdit_path->setText(path());

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setApiVersion(AW_FORMATTER_API);
    setStrType(ui->label_typeValue->text());
    setPath(ui->lineEdit_path->text());

    writeConfiguration();
    return ret;
}


void AWJsonFormatter::writeConfiguration() const
{
    AWAbstractFormatter::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("X-AW-Path", path());
    settings.endGroup();

    settings.sync();
}


QVariant AWJsonFormatter::getFromJson(const QVariant &_value, const QVariant &_element)
{
    qCDebug(LOG_LIB) << "Looking for element" << _element << "in" << _value;

    if (_element.type() == QVariant::String) {
        return getFromMap(_value, _element.toString());
    } else if (_element.type() == QVariant::Int) {
        return getFromList(_value, _element.toInt());
    } else {
        qCWarning(LOG_LIB) << "Unknown type" << _element.typeName();
        return _value;
    }
}


QVariant AWJsonFormatter::getFromList(const QVariant &_value, const int _index)
{
    qCDebug(LOG_LIB) << "Looking for index" << _index << "in" << _value;

    return _value.toList()[_index];
}


QVariant AWJsonFormatter::getFromMap(const QVariant &_value, const QString &_key)
{
    qCDebug(LOG_LIB) << "Looking for key" << _key << "in" << _value;

    return _value.toMap()[_key];
}


void AWJsonFormatter::initPath()
{
    m_splittedPath.clear();
    QStringList splittedByDot = m_path.split('.', Qt::SkipEmptyParts);

    for (auto &element : splittedByDot) {
        bool ok;
        int number = element.toInt(&ok);
        m_splittedPath.append(ok ? QVariant(number) : QVariant(element));
    }
}


void AWJsonFormatter::translate()
{
    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_type->setText(i18n("Type"));
    ui->label_path->setText(i18n("Path"));
}
