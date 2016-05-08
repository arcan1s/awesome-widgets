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

#include <QDateTime>
#include <QSettings>

#include "awdebug.h"


AWDateTimeFormatter::AWDateTimeFormatter(QObject *parent,
                                         const QString filename,
                                         const QString section)
    : AWAbstractFormatter(parent, filename, section)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    init(filename, section);
}


AWDateTimeFormatter::AWDateTimeFormatter(QObject *parent, const QString format)
    : AWAbstractFormatter(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    setFormat(format);
}


AWDateTimeFormatter::~AWDateTimeFormatter()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QString AWDateTimeFormatter::convert(const QVariant &value) const
{
    qCDebug(LOG_AW) << "Convert value" << value;

    return value.toDateTime().toString(m_format);
}


QString AWDateTimeFormatter::format() const
{
    return m_format;
}


void AWDateTimeFormatter::setFormat(const QString _format)
{
    qCDebug(LOG_AW) << "Set format" << _format;

    m_format = _format;
}


void AWDateTimeFormatter::init(const QString filename, const QString section)
{
    qCDebug(LOG_AW) << "Looking for section" << section << "in" << filename;

    QSettings settings(filename, QSettings::IniFormat);

    settings.beginGroup(section);
    setFormat(settings.value(QString("Format"), QString()).toString());
    settings.endGroup();
}
