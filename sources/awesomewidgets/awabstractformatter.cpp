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

#include "awabstractformatter.h"

#include <QSettings>

#include "awdebug.h"


AWAbstractFormatter::AWAbstractFormatter(QWidget *_parent, const QString &_filePath)
    : AbstractExtItem(_parent, _filePath)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


AWAbstractFormatter::~AWAbstractFormatter()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
}


void AWAbstractFormatter::copyDefaults(AbstractExtItem *_other) const
{
    AbstractExtItem::copyDefaults(_other);

    static_cast<AWAbstractFormatter *>(_other)->setType(type());
}


QString AWAbstractFormatter::uniq() const
{
    return QString("%1(%2)").arg(name()).arg(strType());
}


QString AWAbstractFormatter::strType() const
{
    QString value;
    switch (m_type) {
    case FormatterClass::DateTime:
        value = "DateTime";
        break;
    case FormatterClass::Float:
        value = "Float";
        break;
    case FormatterClass::List:
        value = "List";
        break;
    case FormatterClass::Script:
        value = "Script";
        break;
    case FormatterClass::String:
        value = "String";
        break;
    case FormatterClass::Json:
        value = "Json";
        break;
    case FormatterClass::NoFormat:
        value = "NoFormat";
        break;
    }

    return value;
}


AWAbstractFormatter::FormatterClass AWAbstractFormatter::type() const
{
    return m_type;
}


void AWAbstractFormatter::setStrType(const QString &_type)
{
    qCDebug(LOG_LIB) << "Type" << _type;

    if (_type == "DateTime")
        m_type = FormatterClass::DateTime;
    else if (_type == "Float")
        m_type = FormatterClass::Float;
    else if (_type == "List")
        m_type = FormatterClass::List;
    else if (_type == "Script")
        m_type = FormatterClass::Script;
    else if (_type == "String")
        m_type = FormatterClass::String;
    else if (_type == "Json")
        m_type = FormatterClass::Json;
    else
        m_type = FormatterClass::NoFormat;
}


void AWAbstractFormatter::setType(const AWAbstractFormatter::FormatterClass _type)
{
    qCDebug(LOG_LIB) << "Type" << static_cast<int>(_type);

    m_type = _type;
}


void AWAbstractFormatter::readConfiguration()
{
    AbstractExtItem::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setStrType(settings.value("X-AW-Type", strType()).toString());
    settings.endGroup();
}


void AWAbstractFormatter::writeConfiguration() const
{
    AbstractExtItem::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("X-AW-Type", strType());
    settings.endGroup();

    settings.sync();
}
