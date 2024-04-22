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

#include <QJsonDocument>
#include <QSettings>

#include "awdebug.h"


AWJsonFormatter::AWJsonFormatter(QObject *_parent, const QString &_filePath)
    : AWAbstractFormatter(_parent, _filePath)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        AWJsonFormatter::readConfiguration();
}


QString AWJsonFormatter::convert(const QVariant &_value) const
{
    qCDebug(LOG_LIB) << "Convert value" << _value;

    // check if _value is string and parse first if required
    auto json = _value.userType() == QMetaType::QString ? QJsonDocument::fromJson(_value.toString().toUtf8())
                                                        : QJsonDocument::fromVariant(_value);
    auto converted = json.toVariant();
    for (auto &element : m_path)
        converted = getFromJson(converted, element);

    return converted.toString();
}


AWJsonFormatter *AWJsonFormatter::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    auto item = new AWJsonFormatter(parent(), _fileName);
    AWAbstractFormatter::copyDefaults(item);

    item->setNumber(_number);
    item->setPath(path());

    return item;
}


QString AWJsonFormatter::path() const
{
    return std::accumulate(m_path.cbegin(), m_path.cend(), QString(""),
                           [](auto acc, auto &value) { return QString("%1.%2").arg(acc, value.toString()); });
}


void AWJsonFormatter::setPath(const QString &_path)
{
    qCDebug(LOG_LIB) << "Path" << _path;

    m_path.clear();
    auto elements = _path.split('.', Qt::SkipEmptyParts);

    for (auto &element : elements) {
        bool ok;
        auto number = element.toInt(&ok);
        m_path.append(ok ? QVariant(number) : QVariant(element));
    }
}


void AWJsonFormatter::readConfiguration()
{
    AWAbstractFormatter::readConfiguration();

    QSettings settings(filePath(), QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setPath(settings.value("X-AW-Path", path()).toString());
    settings.endGroup();

    bumpApi(AW_FORMATTER_API);
}


int AWJsonFormatter::showConfiguration(QWidget *_parent, const QVariant &args)
{
    Q_UNUSED(args)

    auto dialog = new QDialog(_parent);
    auto ui = new Ui::AWJsonFormatter();
    ui->setupUi(dialog);

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText("Json");
    ui->lineEdit_path->setText(path());

    auto ret = dialog->exec();
    if (ret == 1) {
        setName(ui->lineEdit_name->text());
        setComment(ui->lineEdit_comment->text());
        setApiVersion(AW_FORMATTER_API);
        setStrType(ui->label_typeValue->text());
        setPath(ui->lineEdit_path->text());

        writeConfiguration();
    }

    dialog->deleteLater();
    delete ui;

    return ret;
}


void AWJsonFormatter::writeConfiguration() const
{
    AWAbstractFormatter::writeConfiguration();

    QSettings settings(writableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("X-AW-Path", path());
    settings.endGroup();

    settings.sync();
}


QVariant AWJsonFormatter::getFromJson(const QVariant &_value, const QVariant &_element)
{
    qCDebug(LOG_LIB) << "Looking for element" << _element << "in" << _value;

    if (_element.userType() == QMetaType::QString) {
        return getFromMap(_value, _element.toString());
    } else if (_element.userType() == QMetaType::Int) {
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
