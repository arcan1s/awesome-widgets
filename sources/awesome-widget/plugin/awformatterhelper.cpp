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

#include "awformatterhelper.h"

#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"
#include "awdatetimeformatter.h"
#include "awfloatformatter.h"
#include "awnoformatter.h"


AWFormatterHelper::AWFormatterHelper(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_genericConfig = QString("%1/awesomewidgets/general.ini")
                          .arg(QStandardPaths::writableLocation(
                              QStandardPaths::GenericDataLocation));
#ifdef BUILD_FUTURE
    init();
#endif /* BUILD_FUTURE */
}


AWFormatterHelper::~AWFormatterHelper()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_formatters.clear();
}


QString AWFormatterHelper::convert(const QVariant &value,
                                   const QString name) const
{
    qCDebug(LOG_AW) << "Convert value" << value << "for" << name;

    return m_formatters.contains(name) ? m_formatters[name]->convert(value)
                                       : value.toString();
}


QStringList AWFormatterHelper::definedFormatters() const
{
    return m_formatters.keys();
}


AWFormatterHelper::FormatterClass
AWFormatterHelper::defineFormatterClass(const QString name) const
{
    qCDebug(LOG_AW) << "Define formatter class for" << name;

    QSettings settings(m_genericConfig, QSettings::IniFormat);

    settings.beginGroup(name);
    QString stringType
        = settings.value(QString("Type"), QString("NoFormat")).toString();
    settings.endGroup();

    FormatterClass formatter = FormatterClass::NoFormat;
    if (stringType == QString("Float"))
        formatter = FormatterClass::Float;
    else if (stringType == QString("DateTime"))
        formatter = FormatterClass::DateTime;
    else
        qCWarning(LOG_AW) << "Unknown formatter" << stringType;

    return formatter;
}


void AWFormatterHelper::init()
{
    QSettings settings(m_genericConfig, QSettings::IniFormat);

    settings.beginGroup(QString("Formatters"));
    QStringList keys = settings.childKeys();
    for (auto key : keys) {
        QString name = settings.value(key).toString();
        FormatterClass formatter = defineFormatterClass(name);
        qCInfo(LOG_AW) << "Found formatter" << name << "for key" << key
                       << "defined as" << static_cast<int>(formatter);

        switch (formatter) {
        case FormatterClass::Float:
            m_formatters[key]
                = new AWFloatFormatter(this, m_genericConfig, name);
            break;
        case FormatterClass::DateTime:
            m_formatters[key]
                = new AWDateTimeFormatter(this, m_genericConfig, name);
            break;
        case FormatterClass::NoFormat:
            m_formatters[key] = new AWNoFormatter(this, m_genericConfig, name);
            break;
        }
    }
    settings.endGroup();
}
