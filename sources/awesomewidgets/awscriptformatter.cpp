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

#include <QJSEngine>
#include <QSettings>

#include "awdebug.h"


AWScriptFormatter::AWScriptFormatter(QWidget *_parent, const QString &_filePath)
    : AWAbstractFormatter(_parent, _filePath)
    , ui(new Ui::AWScriptFormatter)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        AWScriptFormatter::readConfiguration();
    ui->setupUi(this);
    AWScriptFormatter::translate();
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
        qCWarning(LOG_LIB) << "Uncaught exception at line" << result.property("lineNumber").toInt() << ":"
                           << result.toString();
        return "";
    } else {
        return result.toString();
    }
}


AWScriptFormatter *AWScriptFormatter::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    auto *item = new AWScriptFormatter(dynamic_cast<QWidget *>(parent()), _fileName);
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
    initProgram();
}


void AWScriptFormatter::setCode(const QString &_code)
{
    qCDebug(LOG_LIB) << "Set code" << _code;

    m_code = _code;
    initProgram();
}


void AWScriptFormatter::setHasReturn(const bool _hasReturn)
{
    qCDebug(LOG_LIB) << "Set has return" << _hasReturn;

    m_hasReturn = _hasReturn;
    initProgram();
}


void AWScriptFormatter::readConfiguration()
{
    AWAbstractFormatter::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setAppendCode(settings.value("X-AW-AppendCode", appendCode()).toBool());
    setCode(settings.value("X-AW-Code", code()).toString());
    setHasReturn(settings.value("X-AW-HasReturn", hasReturn()).toBool());
    settings.endGroup();

    bumpApi(AW_FORMATTER_API);
}


int AWScriptFormatter::showConfiguration(const QVariant &_args)
{
    Q_UNUSED(_args)

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_typeValue->setText("Script");
    ui->checkBox_appendCode->setCheckState(appendCode() ? Qt::Checked : Qt::Unchecked);
    ui->checkBox_hasReturn->setCheckState(hasReturn() ? Qt::Checked : Qt::Unchecked);
    ui->textEdit_code->setPlainText(code());

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setApiVersion(AW_FORMATTER_API);
    setStrType(ui->label_typeValue->text());
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

    settings.beginGroup("Desktop Entry");
    settings.setValue("X-AW-AppendCode", appendCode());
    settings.setValue("X-AW-Code", code());
    settings.setValue("X-AW-HasReturn", hasReturn());
    settings.endGroup();

    settings.sync();
}


void AWScriptFormatter::initProgram()
{
    // init JS code
    if (appendCode())
        m_program = QString("(function(value) { %1%2 })").arg(code()).arg(hasReturn() ? "" : "; return output;");
    else
        m_program = code();

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
