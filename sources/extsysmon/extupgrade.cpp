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

#include <pdebug/pdebug.h>

#include "version.h"


ExtUpgrade::ExtUpgrade(QWidget *parent, const QString upgradeName,
                       const QStringList directories, const bool debugCmd)
    : AbstractExtItem(parent, upgradeName, directories, debugCmd),
      debug(debugCmd),
      ui(new Ui::ExtUpgrade)
{
    readConfiguration();
    ui->setupUi(this);

    value[QString("value")] = 0;

    process = new QProcess(this);
    connect(process, SIGNAL(finished(int)), this, SLOT(updateValue()));
    process->waitForFinished(0);
}


ExtUpgrade::~ExtUpgrade()
{
    if (debug) qDebug() << PDEBUG;

    process->kill();
    delete process;
    delete ui;
}


QString ExtUpgrade::executable() const
{
    if (debug) qDebug() << PDEBUG;

    return m_executable;
}


int ExtUpgrade::null() const
{
    if (debug) qDebug() << PDEBUG;

    return m_null;
}


void ExtUpgrade::setExecutable(const QString _executable)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Executable" << _executable;

    m_executable = _executable;
}


void ExtUpgrade::setNull(const int _null)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Null lines" << _null;
    if (_null < 0) return;

    m_null = _null;
}


void ExtUpgrade::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;
    AbstractExtItem::readConfiguration();

    for (int i=directories().count()-1; i>=0; i--) {
        if (!QDir(directories()[i]).entryList(QDir::Files).contains(fileName())) continue;
        QSettings settings(QString("%1/%2").arg(directories()[i]).arg(fileName()), QSettings::IniFormat);

        settings.beginGroup(QString("Desktop Entry"));
        setExecutable(settings.value(QString("Exec"), m_executable).toString());
        setNull(settings.value(QString("X-AW-Null"), m_null).toInt());
        settings.endGroup();
    }

    // update for current API
    if ((apiVersion() > 0) && (apiVersion() < AWEUAPI)) {
        setApiVersion(AWEUAPI);
        writeConfiguration();
    }
}


QVariantMap ExtUpgrade::run()
{
    if (debug) qDebug() << PDEBUG;
    if (!isActive()) return value;

    if ((times == 1) && (process->state() == QProcess::NotRunning))
        process->start(QString("sh -c \"%1\"").arg(m_executable));
    else if (times >= interval())
        times = 0;
    times++;

    return value;
}


int ExtUpgrade::showConfiguration(const QVariant args)
{
    Q_UNUSED(args)
    if (debug) qDebug() << PDEBUG;

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->lineEdit_command->setText(m_executable);
    ui->checkBox_active->setCheckState(isActive() ? Qt::Checked : Qt::Unchecked);
    ui->spinBox_null->setValue(m_null);
    ui->spinBox_interval->setValue(interval());

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


void ExtUpgrade::writeConfiguration() const
{
    if (debug) qDebug() << PDEBUG;
    AbstractExtItem::writeConfiguration();

    QSettings settings(QString("%1/%2").arg(directories()[0]).arg(fileName()), QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("Exec"), m_executable);
    settings.setValue(QString("X-AW-Null"), m_null);
    settings.endGroup();

    settings.sync();
}


void ExtUpgrade::updateValue()
{
    if (debug) qDebug() << PDEBUG;

    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process->exitCode();
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process->readAllStandardError();

    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process->readAllStandardOutput()).trimmed();
    value[QString("value")] = qoutput.split(QChar('\n'), QString::SkipEmptyParts).count() - m_null;
}
