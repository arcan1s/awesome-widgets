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
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSettings>
#include <QStandardPaths>
#include <QTextCodec>
#include <QTime>

#include <pdebug/pdebug.h>

#include "version.h"


ExtScript::ExtScript(QWidget *parent, const QString scriptName, const QStringList directories, const bool debugCmd)
    : QDialog(parent),
      m_fileName(scriptName),
      m_dirs(directories),
      debug(debugCmd),
      ui(new Ui::ExtScript)
{
    m_name = m_fileName;
    readConfiguration();
    readJsonFilters();
    // init process
    process = new QProcess(this);
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(updateValue()));
    process->waitForFinished(0);
    // init ui
    ui->setupUi(this);
}


ExtScript::~ExtScript()
{
    if (debug) qDebug() << PDEBUG;

    process->kill();
    delete process;
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


QStringList ExtScript::filters()
{
    if (debug) qDebug() << PDEBUG;

    return m_filters;
}


bool ExtScript::hasOutput()
{
    if (debug) qDebug() << PDEBUG;

    return m_output;
}


int ExtScript::interval()
{
    if (debug) qDebug() << PDEBUG;

    return m_interval;
}


bool ExtScript::isActive()
{
    if (debug) qDebug() << PDEBUG;

    return m_active;
}


QString ExtScript::name()
{
    if (debug) qDebug() << PDEBUG;

    return m_name;
}


int ExtScript::number()
{
    if (debug) qDebug() << PDEBUG;

    return m_number;
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
    case nothing:
    default:
        value = QString("nothing");
        break;
    }

    return value;
}


QString ExtScript::tag()
{
    if (debug) qDebug() << PDEBUG;

    return QString("custom%1").arg(m_number);
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


void ExtScript::setFilters(const QStringList _filters)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Filters" << _filters;

    for (int i=0; i<_filters.count(); i++)
        updateFilter(_filters[i]);
}


void ExtScript::setHasOutput(const bool _state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << _state;

    m_output = _state;
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


void ExtScript::setNumber(int _number)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Number" << _number;
    if (_number == -1) {
        if (debug) qDebug() << PDEBUG << ":" << "Number is empty, generate new one";
        qsrand(QTime::currentTime().msec());
        _number = qrand() % 1000;
        if (debug) qDebug() << PDEBUG << ":" << "Generated number is" << _number;
    }

    m_number = _number;
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


QString ExtScript::applyFilters(QString _value)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Value" << _value;

    for (int i=0; i<m_filters.count(); i++) {
        if (debug) qDebug() << PDEBUG << ":" << "Found filter" << m_filters[i];
        QVariantMap filter = jsonFilters[m_filters[i]].toMap();
        if (filter.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not find filter in the json";
            continue;
        }
        for (int j=0; j<filter.keys().count(); j++)
            _value.replace(filter.keys()[j], filter[filter.keys()[j]].toString());
    }

    return _value;
}


void ExtScript::updateFilter(const QString _filter, const bool _add)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Filter" << _filter;
    if (debug) qDebug() << PDEBUG << ":" << "Should be added" << _add;

    if (_add) {
        if (m_filters.contains(_filter)) return;
        m_filters.append(_filter);
    } else {
        m_filters.removeOne(_filter);
    }
}


void ExtScript::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=m_dirs.count()-1; i>=0; i--) {
        if (!QDir(m_dirs[i]).entryList(QDir::Files).contains(m_fileName)) continue;
        QSettings settings(QString("%1/%2").arg(m_dirs[i]).arg(m_fileName), QSettings::IniFormat);

        settings.beginGroup(QString("Desktop Entry"));
        setName(settings.value(QString("Name"), m_name).toString());
        setComment(settings.value(QString("Comment"), m_comment).toString());
        setApiVersion(settings.value(QString("X-AW-ApiVersion"), m_apiVersion).toInt());
        setExecutable(settings.value(QString("Exec"), m_executable).toString());
        setPrefix(settings.value(QString("X-AW-Prefix"), m_prefix).toString());
        setActive(settings.value(QString("X-AW-Active"), QVariant(m_active)).toString() == QString("true"));
        setHasOutput(settings.value(QString("X-AW-Output"), QVariant(m_output)).toString() == QString("true"));
        setStrRedirect(settings.value(QString("X-AW-Redirect"), strRedirect()).toString());
        setInterval(settings.value(QString("X-AW-Interval"), m_interval).toInt());
        // api == 2
        setNumber(settings.value(QString("X-AW-Number"), m_number).toInt());
        // api == 3
        setFilters(settings.value(QString("X-AW-Filters"), m_filters).toString()
                                                                     .split(QChar(','), QString::SkipEmptyParts));
        settings.endGroup();
    }

    if (!m_output)
        setRedirect(stdout2stderr);

    // update for current API
    if ((m_apiVersion > 0) && (m_apiVersion < AWESAPI)) {
        setApiVersion(AWESAPI);
        writeConfiguration();
    }
}


void ExtScript::readJsonFilters()
{
    if (debug) qDebug() << PDEBUG;

    QString fileName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                              QString("awesomewidgets/scripts/awesomewidgets-extscripts-filters.json"));
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QString jsonText = jsonFile.readAll();
    jsonFile.close();

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonText.toUtf8(), &error);
    if (debug) qDebug() << PDEBUG << ":" << "Json parse error" << error.errorString();
    if (error.error != QJsonParseError::NoError)
        return;
    jsonFilters = jsonDoc.toVariant().toMap();

    if (debug) qDebug() << PDEBUG << ":" << "Filters" << jsonFilters;
}


QString ExtScript::run()
{
    if (debug) qDebug() << PDEBUG;
    if (!m_active) return value;

    if ((times == 1) && (process->state() == QProcess::NotRunning)) {
        QStringList cmdList;
        if (!m_prefix.isEmpty()) cmdList.append(m_prefix);
        cmdList.append(m_executable);
        if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmdList.join(QChar(' '));
        process->start(cmdList.join(QChar(' ')));
    } else if (times >= m_interval)
        times = 0;
    times++;

    return value;
}


int ExtScript::showConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    ui->lineEdit_name->setText(m_name);
    ui->lineEdit_comment->setText(m_comment);
    ui->label_numberValue->setText(QString("%1").arg(m_number));
    ui->lineEdit_command->setText(m_executable);
    ui->lineEdit_prefix->setText(m_prefix);
    ui->checkBox_active->setCheckState(m_active ? Qt::Checked : Qt::Unchecked);
    ui->checkBox_output->setCheckState(m_output ? Qt::Checked : Qt::Unchecked);
    ui->comboBox_redirect->setCurrentIndex(static_cast<int>(m_redirect));
    ui->spinBox_interval->setValue(m_interval);
    // filters
    ui->checkBox_colorFilter->setCheckState(m_filters.contains(QString("color")) ? Qt::Checked : Qt::Unchecked);
    ui->checkBox_linesFilter->setCheckState(m_filters.contains(QString("newline")) ? Qt::Checked : Qt::Unchecked);
    ui->checkBox_spaceFilter->setCheckState(m_filters.contains(QString("space")) ? Qt::Checked : Qt::Unchecked);

    int ret = exec();
    if (ret != 1) return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setNumber(ui->label_numberValue->text().toInt());
    setApiVersion(AWESAPI);
    setExecutable(ui->lineEdit_command->text());
    setPrefix(ui->lineEdit_prefix->text());
    setActive(ui->checkBox_active->checkState() == Qt::Checked);
    setHasOutput(ui->checkBox_output->checkState() == Qt::Checked);
    setStrRedirect(ui->comboBox_redirect->currentText());
    setInterval(ui->spinBox_interval->value());
    // filters
    updateFilter(QString("color"), ui->checkBox_colorFilter->checkState() == Qt::Checked);
    updateFilter(QString("newline"), ui->checkBox_linesFilter->checkState() == Qt::Checked);
    updateFilter(QString("space"), ui->checkBox_spaceFilter->checkState() == Qt::Checked);

    writeConfiguration();
    return ret;
}


bool ExtScript::tryDelete()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=0; i<m_dirs.count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << "Remove file" << QString("%1/%2").arg(m_dirs[i]).arg(m_fileName) <<
                               QFile::remove(QString("%1/%2").arg(m_dirs[i]).arg(m_fileName));

    // check if exists
    for (int i=0; i<m_dirs.count(); i++)
        if (QFile::exists(QString("%1/%2").arg(m_dirs[i]).arg(m_fileName))) return false;
    return true;
}


void ExtScript::writeConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QSettings settings(QString("%1/%2").arg(m_dirs[0]).arg(m_fileName), QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("Encoding"), QString("UTF-8"));
    settings.setValue(QString("Name"), m_name);
    settings.setValue(QString("Comment"), m_comment);
    settings.setValue(QString("Exec"), m_executable);
    settings.setValue(QString("X-AW-ApiVersion"), m_apiVersion);
    settings.setValue(QString("X-AW-Prefix"), m_prefix);
    settings.setValue(QString("X-AW-Active"), QVariant(m_active).toString());
    settings.setValue(QString("X-AW-Output"), QVariant(m_output).toString());
    settings.setValue(QString("X-AW-Redirect"), strRedirect());
    settings.setValue(QString("X-AW-Interval"), m_interval);
    settings.setValue(QString("X-AW-Number"), m_number);
    settings.setValue(QString("X-AW-Filters"), m_filters.join(QChar(',')));
    settings.endGroup();

    settings.sync();
}


void ExtScript::updateValue()
{
    if (debug) qDebug() << PDEBUG;

    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process->exitCode();
    QString qdebug = QTextCodec::codecForMib(106)->toUnicode(process->readAllStandardError()).trimmed();
    if (debug) qDebug() << PDEBUG << ":" << "Error" << qdebug;
    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process->readAllStandardOutput()).trimmed();

    switch (m_redirect) {
    case stdout2stderr:
        if (debug) qDebug() << PDEBUG << ":" << "Debug" << qdebug;
        if (debug) qDebug() << PDEBUG << ":" << "Output" << qoutput;
        break;
    case stderr2stdout:
        value = QString("%1\t%2").arg(qdebug).arg(qoutput);
        break;
    case nothing:
    default:
        if (debug) qDebug() << PDEBUG << ":" << "Debug" << qdebug;
        value = qoutput;
        break;
    }

    // filters
    value = applyFilters(value);
}
