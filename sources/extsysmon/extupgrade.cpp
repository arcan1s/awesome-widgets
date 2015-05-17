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

#include "extupgrade.h"
#include "ui_extupgrade.h"

#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QTextCodec>
#include <QTime>

#include <pdebug/pdebug.h>
#include <task/taskadds.h>

#include "version.h"


ExtUpgrade::ExtUpgrade(QWidget *parent, const QString upgradeName, const QStringList directories, const bool debugCmd)
    : QDialog(parent),
      m_fileName(upgradeName),
      m_dirs(directories),
      debug(debugCmd),
      ui(new Ui::ExtUpgrade)
{
    m_name = m_fileName;
    readConfiguration();
    ui->setupUi(this);
}


ExtUpgrade::~ExtUpgrade()
{
    if (debug) qDebug() << PDEBUG;

    delete ui;
}


int ExtUpgrade::apiVersion()
{
    if (debug) qDebug() << PDEBUG;

    return m_apiVersion;
}


QString ExtUpgrade::comment()
{
    if (debug) qDebug() << PDEBUG;

    return m_comment;
}


QString ExtUpgrade::executable()
{
    if (debug) qDebug() << PDEBUG;

    return m_executable;
}


QString ExtUpgrade::fileName()
{
    if (debug) qDebug() << PDEBUG;

    return m_fileName;
}


int ExtUpgrade::interval()
{
    if (debug) qDebug() << PDEBUG;

    return m_interval;
}


QString ExtUpgrade::name()
{
    if (debug) qDebug() << PDEBUG;

    return m_name;
}


int ExtUpgrade::null()
{
    if (debug) qDebug() << PDEBUG;

    return m_null;
}


int ExtUpgrade::number()
{
    if (debug) qDebug() << PDEBUG;

    return m_number;
}


QString ExtUpgrade::tag()
{
    if (debug) qDebug() << PDEBUG;

    return QString("pkgcount%1").arg(m_number);
}


bool ExtUpgrade::isActive()
{
    if (debug) qDebug() << PDEBUG;

    return m_active;
}


void ExtUpgrade::setApiVersion(const int _apiVersion)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Version" << _apiVersion;

    m_apiVersion = _apiVersion;
}


void ExtUpgrade::setActive(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    m_active = state;
}


void ExtUpgrade::setComment(const QString _comment)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Comment" << _comment;

    m_comment = _comment;
}


void ExtUpgrade::setExecutable(const QString _executable)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Executable" << _executable;

    m_executable = _executable;
}


void ExtUpgrade::setInterval(const int _interval)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Interval" << _interval;

    m_interval = _interval;
}


void ExtUpgrade::setName(const QString _name)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Name" << _name;

    m_name = _name;
}


void ExtUpgrade::setNull(const int _null)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Null lines" << _null;
    if (_null < 0) return;

    m_null = _null;
}


void ExtUpgrade::setNumber(int _number)
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


void ExtUpgrade::readConfiguration()
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
        setActive(settings.value(QString("X-AW-Active"), QVariant(m_active)).toString() == QString("true"));
        setNull(settings.value(QString("X-AW-Null"), m_null).toInt());
        setInterval(settings.value(QString("X-AW-Interval"), m_interval).toInt());
        // api == 2
        setNumber(settings.value(QString("X-AW-Number"), m_number).toInt());
        settings.endGroup();
    }

    // update for current API
    if ((m_apiVersion > 0) && (m_apiVersion < AWEUAPI)) {
        setApiVersion(AWEUAPI);
        writeConfiguration();
    }
}


int ExtUpgrade::run()
{
    if (debug) qDebug() << PDEBUG;
    if (!m_active) return value;

    if (times == 1) {
        TaskResult process = runTask(QString("bash -c \"%1\"").arg(m_executable));
        if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
        if (process.exitCode != 0)
            if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

        QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
        value = qoutput.split(QChar('\n'), QString::SkipEmptyParts).count() - m_null;
    }

    // update value
    if (times >= m_interval) times = 0;
    times++;

    return value;
}


int ExtUpgrade::showConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    ui->lineEdit_name->setText(m_name);
    ui->lineEdit_comment->setText(m_comment);
    ui->label_numberValue->setText(QString("%1").arg(m_number));
    ui->lineEdit_command->setText(m_executable);
    ui->checkBox_active->setCheckState(m_active ? Qt::Checked : Qt::Unchecked);
    ui->spinBox_null->setValue(m_null);
    ui->spinBox_interval->setValue(m_interval);

    int ret = exec();
    if (ret != 1) return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setNumber(ui->label_numberValue->text().toInt());
    setApiVersion(AWEUAPI);
    setExecutable(ui->lineEdit_command->text());
    setActive(ui->checkBox_active->checkState() == Qt::Checked);
    setNull(ui->spinBox_null->value());
    setInterval(ui->spinBox_interval->value());

    writeConfiguration();
    return ret;
}


bool ExtUpgrade::tryDelete()
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


void ExtUpgrade::writeConfiguration()
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
    settings.setValue(QString("X-AW-Active"), QVariant(m_active).toString());
    settings.setValue(QString("X-AW-Null"), m_null);
    settings.setValue(QString("X-AW-Interval"), m_interval);
    settings.setValue(QString("X-AW-Number"), m_number);
    settings.endGroup();

    settings.sync();
}
