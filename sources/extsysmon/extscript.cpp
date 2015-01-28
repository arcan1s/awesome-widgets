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

#include "version.h"


ExtScript::ExtScript(QWidget *parent, const QString scriptName, const QStringList directories, const bool debugCmd) :
    QDialog(parent),
    m_fileName(scriptName),
    m_dirs(directories),
    debug(debugCmd),
    ui(new Ui::ExtScript)
{
    m_name = m_fileName;
    readConfiguration();
    ui->setupUi(this);
}


ExtScript::~ExtScript()
{
    if (debug) qDebug() << PDEBUG;

    delete ui;
}


int ExtScript::apiVersion()
{
    if (debug) qDebug() << PDEBUG;

    return m_apiVersion;
}


QString ExtScript::comment()
{
    if (debug) qDebug() << PDEBUG;

    return m_comment;
}


QString ExtScript::executable()
{
    if (debug) qDebug() << PDEBUG;

    return m_executable;
}


QString ExtScript::fileName()
{
    if (debug) qDebug() << PDEBUG;

    return m_fileName;
}


int ExtScript::interval()
{
    if (debug) qDebug() << PDEBUG;

    return m_interval;
}


QString ExtScript::name()
{
    if (debug) qDebug() << PDEBUG;

    return m_name;
}


QString ExtScript::prefix()
{
    if (debug) qDebug() << PDEBUG;

    return m_prefix;
}


ExtScript::Redirect ExtScript::redirect()
{
    if (debug) qDebug() << PDEBUG;

    return m_redirect;
}


QString ExtScript::strRedirect()
{
    if (debug) qDebug() << PDEBUG;

    QString value;
    switch (m_redirect) {
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

    return m_output;
}


bool ExtScript::isActive()
{
    if (debug) qDebug() << PDEBUG;

    return m_active;
}


void ExtScript::setApiVersion(const int _apiVersion)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Version" << _apiVersion;

    m_apiVersion = _apiVersion;
}


void ExtScript::setActive(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    m_active = state;
}


void ExtScript::setComment(const QString _comment)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Comment" << _comment;

    m_comment = _comment;
}


void ExtScript::setExecutable(const QString _executable)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Executable" << _executable;

    m_executable = _executable;
}


void ExtScript::setHasOutput(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    m_output = state;
}


void ExtScript::setInterval(const int _interval)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Interval" << _interval;
    if (_interval <= 0) return;

    m_interval = _interval;
}


void ExtScript::setName(const QString _name)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Name" << _name;

    m_name = _name;
}


void ExtScript::setPrefix(const QString _prefix)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Prefix" << _prefix;

    m_prefix = _prefix;
}


void ExtScript::setRedirect(const Redirect _redirect)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Redirect" << _redirect;

    m_redirect = _redirect;
}


void ExtScript::setStrRedirect(const QString _redirect)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Redirect" << _redirect;

    if (_redirect == QString("stdout2sdterr"))
        m_redirect = stdout2stderr;
    else if (_redirect == QString("stderr2sdtout"))
        m_redirect = stderr2stdout;
    else
        m_redirect = nothing;
}


void ExtScript::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=m_dirs.count()-1; i>=0; i--) {
        if (!QDir(m_dirs[i]).entryList(QDir::Files).contains(m_fileName)) continue;
        QSettings settings(m_dirs[i] + QDir::separator() + m_fileName, QSettings::IniFormat);

        settings.beginGroup(QString("Desktop Entry"));
        setName(settings.value(QString("Name"), m_name).toString());
        setComment(settings.value(QString("Comment"), m_comment).toString());
        setApiVersion(settings.value(QString("X-AW-ApiVersion"), AWESAPI).toInt());
        setExecutable(settings.value(QString("Exec"), m_executable).toString());
        setPrefix(settings.value(QString("X-AW-Prefix"), m_prefix).toString());
        setActive(settings.value(QString("X-AW-Active"), QVariant(m_active)).toString() == QString("true"));
        setHasOutput(settings.value(QString("X-AW-Output"), QVariant(m_output)).toString() == QString("true"));
        setStrRedirect(settings.value(QString("X-AW-Redirect"), strRedirect()).toString());
        setInterval(settings.value(QString("X-AW-Interval"), m_interval).toInt());
        settings.endGroup();
    }

    if (!m_output)
        setRedirect(stdout2stderr);
}


ExtScript::ScriptData ExtScript::run(const int time)
{
    if (debug) qDebug() << PDEBUG;

    ScriptData response;
    response.active = m_active;
    response.name = m_name;
    response.refresh = false;
    if (!m_active) return response;
    if (time != m_interval) return response;
    response.refresh = true;

    QStringList cmdList;
    if (!m_prefix.isEmpty())
        cmdList.append(m_prefix);
    cmdList.append(m_executable);
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmdList.join(QChar(' '));
    TaskResult process = runTask(cmdList.join(QChar(' ')));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

    QString info = QString::number(process.exitCode) + QString(":") +
                   QTextCodec::codecForMib(106)->toUnicode(process.error).trimmed();
    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    switch (m_redirect) {
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

    return response;
}


void ExtScript::showConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    ui->lineEdit_name->setText(m_name);
    ui->lineEdit_comment->setText(m_comment);
    ui->lineEdit_command->setText(m_executable);
    ui->lineEdit_prefix->setText(m_prefix);
    if (m_active)
        ui->checkBox_active->setCheckState(Qt::Checked);
    else
        ui->checkBox_active->setCheckState(Qt::Unchecked);
    if (m_output)
        ui->checkBox_output->setCheckState(Qt::Checked);
    else
        ui->checkBox_output->setCheckState(Qt::Unchecked);
    ui->comboBox_redirect->setCurrentIndex(static_cast<int>(m_redirect));
    ui->spinBox_interval->setValue(m_interval);

    int ret = exec();
    if (ret != 1) return;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setApiVersion(AWESAPI);
    setExecutable(ui->lineEdit_command->text());
    setPrefix(ui->lineEdit_prefix->text());
    setActive(ui->checkBox_active->checkState() == Qt::Checked);
    setHasOutput(ui->checkBox_output->checkState() == Qt::Checked);
    setStrRedirect(ui->comboBox_redirect->currentText());
    setInterval(ui->spinBox_interval->value());

    writeConfiguration();
}


void ExtScript::tryDelete()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=0; i<m_dirs.count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << "Remove file" << m_dirs[i] + QDir::separator() + m_fileName <<
                               QFile::remove(m_dirs[i] + QDir::separator() + m_fileName);
}


void ExtScript::writeConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QSettings settings(m_dirs[0] + QDir::separator() + m_fileName, QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("Encoding"), QString("UTF-8"));
    settings.setValue(QString("Name"), m_name);
    settings.setValue(QString("Comment"), m_comment);
    settings.setValue(QString("Exec"), m_executable);
    settings.setValue(QString("X-AW-ApiVersion"), m_apiVersion);
    settings.setValue(QString("X-AW-Prefix"), m_prefix);
    settings.setValue(QString("X-AW-Active"), QVariant(m_active).toString());
    settings.setValue(QString("X-AW-Output"), QVariant(m_active).toString());
    settings.setValue(QString("X-AW-Redirect"), strRedirect());
    settings.setValue(QString("X-AW-Interval"), m_interval);
    settings.endGroup();

    settings.sync();
}
