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
#include "ui_awscriptformatter.h"

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QJSEngine>
#include <QSettings>

#include "awdebug.h"


AWScriptFormatter::AWScriptFormatter(QWidget *parent, const QString filePath)
    : AWAbstractFormatter(parent, filePath)
    , ui(new Ui::AWScriptFormatter)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!filePath.isEmpty())
        readConfiguration();
    ui->setupUi(this);
    translate();
}


AWScriptFormatter::~AWScriptFormatter()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    delete ui;
}


QString AWScriptFormatter::convert(const QVariant &_value) const
{
    qCDebug(LOG_LIB) << "Convert value" << _value;

    // init engine
    QJSEngine engine;
    QJSValue fn = engine.evaluate(m_program);
    QJSValueList args = QJSValueList() << _value.toString();
    QJSValue result = fn.call(args);

    if (result.isError()) {
        qCWarning(LOG_LIB) << "Uncaught exception at line"
                           << result.property("lineNumber").toInt() << ":"
                           << result.toString();
        return QString();
    } else {
        return result.toString();
    }
}


AWScriptFormatter *AWScriptFormatter::copy(const QString _fileName,
                                           const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    AWScriptFormatter *item
        = new AWScriptFormatter(static_cast<QWidget *>(parent()), _fileName);
    AWAbstractFormatter::copyDefaults(item);
    item->setAppendCode(appendCode());
    item->setCode(code());
    item->setHasReturn(hasReturn());
    item->setNumber(_number);

    return item;
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
    qCDebug(LOG_LIB) << "Set append code" << _appendCode;

    m_appendCode = _appendCode;
}


void AWScriptFormatter::setCode(const QString _code)
{
    qCDebug(LOG_LIB) << "Set code" << _code;

    m_code = _code;
}


void AWScriptFormatter::setHasReturn(const bool _hasReturn)
{
    qCDebug(LOG_LIB) << "Set has return" << _hasReturn;

    m_hasReturn = _hasReturn;
}


void AWScriptFormatter::readConfiguration()
{
    AWAbstractFormatter::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup(QString("Desktop Entry"));
    setAppendCode(
        settings.value(QString("X-AW-AppendCode"), m_appendCode).toBool());
    setCode(settings.value(QString("X-AW-Code"), m_code).toString());
    setHasReturn(
        settings.value(QString("X-AW-HasReturn"), m_hasReturn).toBool());
    settings.endGroup();

    bumpApi(AWEFAPI);

    // init JS code
    initProgram();
}


int AWScriptFormatter::showConfiguration(const QVariant args)
{
    Q_UNUSED(args)

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText(QString("Script"));
    ui->checkBox_appendCode->setCheckState(m_appendCode ? Qt::Checked
                                                        : Qt::Unchecked);
    ui->checkBox_hasReturn->setCheckState(m_hasReturn ? Qt::Checked
                                                      : Qt::Unchecked);
    ui->textEdit_code->setPlainText(m_code);

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setType(ui->label_typeValue->text());
    setAppendCode(ui->checkBox_appendCode->checkState() == Qt::Checked);
    setHasReturn(ui->checkBox_hasReturn->checkState() == Qt::Checked);
    setCode(ui->textEdit_code->toPlainText());
    initProgram();

    writeConfiguration();
    return ret;
}


void AWScriptFormatter::writeConfiguration() const
{
    AWAbstractFormatter::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-AppendCode"), m_appendCode);
    settings.setValue(QString("X-AW-Code"), m_code);
    settings.setValue(QString("X-AW-HasReturn"), m_hasReturn);
    settings.endGroup();

    settings.sync();
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

    qCInfo(LOG_LIB) << "Create JS engine with code" << m_program;
}


void AWScriptFormatter::translate()
{
    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_type->setText(i18n("Type"));
    ui->checkBox_appendCode->setText(i18n("Append code"));
    ui->checkBox_hasReturn->setText(i18n("Has return"));
    ui->label_code->setText(i18n("Code"));
}
