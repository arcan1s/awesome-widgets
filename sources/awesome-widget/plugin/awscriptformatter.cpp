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


#include "awscriptformatter.h"

#include <QJSEngine>
#include <QSettings>

#include "awdebug.h"


AWScriptFormatter::AWScriptFormatter(QObject *parent, const QString filename,
                                     const QString section)
    : AWAbstractFormatter(parent, filename, section)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    init(filename, section);
    initProgram();
}


AWScriptFormatter::AWScriptFormatter(QObject *parent, const bool appendCode,
                                     const QString code, const bool hasReturn)
    : AWAbstractFormatter(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    setAppendCode(appendCode);
    setCode(code);
    setHasReturn(hasReturn);
    initProgram();
}


AWScriptFormatter::~AWScriptFormatter()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QString AWScriptFormatter::convert(const QVariant &value) const
{
    qCDebug(LOG_AW) << "Convert value" << value;

    // init engine
    QJSEngine engine;
    QJSValue fn = engine.evaluate(m_program);
    QJSValueList args = QJSValueList() << value.toString();
    QJSValue result = fn.call(args);

    if (result.isError()) {
        qCWarning(LOG_AW) << "Uncaught exception at line"
                          << result.property("lineNumber").toInt() << ":"
                          << result.toString();
        return QString();
    } else {
        return result.toString();
    }
}


bool AWScriptFormatter::appendCode() const
{
    return m_appendCode;
}


QString AWScriptFormatter::code() const
{
    return m_code;
}


bool AWScriptFormatter::hasReturn() const
{
    return m_hasReturn;
}


QString AWScriptFormatter::program() const
{
    return m_program;
}


void AWScriptFormatter::setAppendCode(const bool _appendCode)
{
    qCDebug(LOG_AW) << "Set append code" << _appendCode;

    m_appendCode = _appendCode;
}


void AWScriptFormatter::setCode(const QString _code)
{
    qCDebug(LOG_AW) << "Set code" << _code;

    m_code = _code;
}


void AWScriptFormatter::setHasReturn(const bool _hasReturn)
{
    qCDebug(LOG_AW) << "Set has return" << _hasReturn;

    m_hasReturn = _hasReturn;
}


void AWScriptFormatter::init(const QString filename, const QString section)
{
    qCDebug(LOG_AW) << "Looking for section" << section << "in" << filename;

    QSettings settings(filename, QSettings::IniFormat);

    settings.beginGroup(section);
    setAppendCode(settings.value(QString("AppendCode"), true).toBool());
    setCode(settings.value(QString("Code"), QString()).toString());
    setHasReturn(settings.value(QString("HasReturn"), false).toBool());
    settings.endGroup();
}


void AWScriptFormatter::initProgram()
{
    if (m_appendCode)
        m_program
            = QString("(function(value) { %1%2 })")
                  .arg(m_code)
                  .arg(m_hasReturn ? QString("") : QString("; return output;"));
    else
        m_program = m_code;

    qCInfo(LOG_AW) << "Create JS engine with code" << m_program;
}
