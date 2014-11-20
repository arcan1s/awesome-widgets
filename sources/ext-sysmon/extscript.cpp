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

#include "extscript.h"
#include "ui_extscript.h"

#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QTextCodec>

#include <pdebug/pdebug.h>
#include <task/taskadds.h>


ExtScript::ExtScript(QWidget *parent, const QString scriptName, const QStringList directories, const bool debugCmd) :
    QDialog(parent),
    fileName(scriptName),
    dirs(directories),
    debug(debugCmd),
    ui(new Ui::ExtScript)
{
    _name = fileName;
    readConfiguration();
    ui->setupUi(this);
}


ExtScript::~ExtScript()
{
    if (debug) qDebug() << PDEBUG;

    delete ui;
}


QString ExtScript::getComment()
{
    if (debug) qDebug() << PDEBUG;

    return _comment;
}


QString ExtScript::getExec()
{
    if (debug) qDebug() << PDEBUG;

    return _exec;
}


QString ExtScript::getFileName()
{
    if (debug) qDebug() << PDEBUG;

    return fileName;
}


int ExtScript::getInterval()
{
    if (debug) qDebug() << PDEBUG;

    return _interval;
}


QString ExtScript::getName()
{
    if (debug) qDebug() << PDEBUG;

    return _name;
}


QString ExtScript::getPrefix()
{
    if (debug) qDebug() << PDEBUG;

    return _prefix;
}


ExtScript::Redirect ExtScript::getRedirect()
{
    if (debug) qDebug() << PDEBUG;

    return _redirect;
}


QString ExtScript::getStrRedirect()
{
    if (debug) qDebug() << PDEBUG;

    QString value;
    switch (_redirect) {
    case stdout2stderr:
        value = QString("stdout2stderr");
        break;
    case stderr2stdout:
        value = QString("stderr2stdout");
        break;
    default:
        value = QString("nothing");
        break;
    }

    return value;
}


bool ExtScript::hasOutput()
{
    if (debug) qDebug() << PDEBUG;

    return _output;
}


bool ExtScript::isActive()
{
    if (debug) qDebug() << PDEBUG;

    return _active;
}


void ExtScript::setActive(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    _active = state;
}


void ExtScript::setComment(const QString comment)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Comment" << comment;

    _comment = comment;
}


void ExtScript::setExec(const QString exec)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Executable" << exec;

    _exec = exec;
}


void ExtScript::setHasOutput(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    _output = state;
}


void ExtScript::setInterval(const int interval)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Interval" << interval;
    if (interval <= 0) return;

    _interval = interval;
}


void ExtScript::setName(const QString name)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Name" << name;

    _name = name;
}


void ExtScript::setPrefix(const QString prefix)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Prefix" << prefix;

    _prefix = prefix;
}


void ExtScript::setRedirect(const QString redirect)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Redirect" << redirect;

    if (redirect == QString("stdout2sdterr"))
        _redirect = stdout2stderr;
    else if (redirect == QString("stderr2sdtout"))
        _redirect = stderr2stdout;
    else
        _redirect = nothing;
}


void ExtScript::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=dirs.count()-1; i>=0; i--) {
        if (!QDir(dirs[i]).entryList(QDir::Files).contains(fileName)) continue;
        QSettings settings(dirs[i] + QDir::separator() + fileName, QSettings::IniFormat);
        settings.beginGroup(QString("Desktop Entry"));
        setName(settings.value(QString("Name"), _name).toString());
        setComment(settings.value(QString("Comment"), _comment).toString());
        setExec(settings.value(QString("Exec"), _exec).toString());
        setPrefix(settings.value(QString("X-AW-Prefix"), _prefix).toString());
        setActive(settings.value(QString("X-AW-Active"), QVariant(_active).toString()).toString() == QString("true"));
        setHasOutput(settings.value(QString("X-AW-Output"), QVariant(_output).toString()).toString() == QString("true"));
        setRedirect(settings.value(QString("X-AW-Redirect"), getStrRedirect()).toString());
        setInterval(settings.value(QString("X-AW-Interval"), _interval).toInt());
        settings.endGroup();
    }

    if (!_output)
        setRedirect(QString("stdout2stderr"));
}


ExtScript::ScriptData ExtScript::run(const int time)
{
    if (debug) qDebug() << PDEBUG;

    ScriptData response;
    response.active = _active;
    response.name = _name;
    response.refresh = false;
    if (!_active) return response;
    if (time != _interval) return response;
    response.refresh = true;

    QStringList cmdList;
    if (!_prefix.isEmpty())
        cmdList.append(_prefix);
    cmdList.append(_exec);
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmdList.join(QChar(' '));
    TaskResult process = runTask(cmdList.join(QChar(' ')));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

    QString info = QTextCodec::codecForMib(106)->toUnicode(process.error).trimmed();
    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    switch(_redirect) {
    case stdout2stderr:
        if (debug) qDebug() << PDEBUG << ":" << "Debug" << info;
        if (debug) qDebug() << PDEBUG << ":" << "Output" << qoutput;
        break;
    case stderr2stdout:
        response.output = info + QString("\t") + qoutput;
        break;
    default:
        if (debug) qDebug() << PDEBUG << ":" << "Debug" << info;
        response.output = qoutput;
        break;
    }
    if (!_output)
        response.output = QString::number(process.exitCode);

    return response;
}


void ExtScript::showConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    ui->lineEdit_name->setText(_name);
    ui->lineEdit_comment->setText(_comment);
    ui->lineEdit_command->setText(_exec);
    ui->lineEdit_prefix->setText(_prefix);
    if (_active)
        ui->checkBox_active->setCheckState(Qt::Checked);
    else
        ui->checkBox_active->setCheckState(Qt::Unchecked);
    if (_output)
        ui->checkBox_output->setCheckState(Qt::Checked);
    else
        ui->checkBox_output->setCheckState(Qt::Unchecked);
    ui->comboBox_redirect->setCurrentIndex((int)_redirect);
    ui->spinBox_interval->setValue(_interval);

    int ret = exec();
    if (ret != 1) return;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setExec(ui->lineEdit_command->text());
    setPrefix(ui->lineEdit_prefix->text());
    setActive(ui->checkBox_active->checkState() == Qt::Checked);
    setHasOutput(ui->checkBox_output->checkState() == Qt::Checked);
    setRedirect(ui->comboBox_redirect->currentText());
    setInterval(ui->spinBox_interval->value());

    writeConfiguration();
}


void ExtScript::tryDelete()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=0; i<dirs.count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << "Remove file" << dirs[i] + QDir::separator() + fileName <<
                               QFile::remove(dirs[i] + QDir::separator() + fileName);
}


void ExtScript::writeConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QSettings settings(dirs[0] + QDir::separator() + fileName, QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();
    settings.beginGroup(QString("Desktop Entry"));

    settings.setValue(QString("Encoding"), QString("UTF-8"));
    settings.setValue(QString("Name"), _name);
    settings.setValue(QString("Comment"), _comment);
    settings.setValue(QString("Exec"), _exec);
    settings.setValue(QString("X-AW-Prefix"), _prefix);
    settings.setValue(QString("X-AW-Active"), QVariant(_active).toString());
    settings.setValue(QString("X-AW-Output"), QVariant(_active).toString());
    settings.setValue(QString("X-AW-Redirect"), getStrRedirect());
    settings.setValue(QString("X-AW-Interval"), _interval);

    settings.endGroup();
    settings.sync();
}
