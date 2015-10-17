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

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QRegExp>
#include <QSettings>
#include <QTextCodec>

#include "awdebug.h"
#include "version.h"


ExtUpgrade::ExtUpgrade(QWidget *parent, const QString upgradeName,
                       const QStringList directories)
    : AbstractExtItem(parent, upgradeName, directories)
    , ui(new Ui::ExtUpgrade)
{
    qCDebug(LOG_LIB);

    readConfiguration();
    ui->setupUi(this);
    translate();

    value[tag(QString("pkgcount"))] = 0;

    process = new QProcess(this);
    connect(process, SIGNAL(finished(int)), this, SLOT(updateValue()));
    process->waitForFinished(0);
}


ExtUpgrade::~ExtUpgrade()
{
    qCDebug(LOG_LIB);

    process->kill();
    delete process;
    delete ui;
}


ExtUpgrade *ExtUpgrade::copy(const QString _fileName, const int _number)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "File" << _fileName;
    qCDebug(LOG_LIB) << "Number" << _number;

    ExtUpgrade *item = new ExtUpgrade(static_cast<QWidget *>(parent()),
                                      _fileName, directories());
    item->setActive(isActive());
    item->setApiVersion(apiVersion());
    item->setComment(comment());
    item->setExecutable(executable());
    item->setFilter(filter());
    item->setInterval(interval());
    item->setName(name());
    item->setNumber(_number);
    item->setNull(null());

    return item;
}


QString ExtUpgrade::executable() const
{
    qCDebug(LOG_LIB);

    return m_executable;
}


QString ExtUpgrade::filter() const
{
    qCDebug(LOG_LIB);

    return m_filter;
}


int ExtUpgrade::null() const
{
    qCDebug(LOG_LIB);

    return m_null;
}


QString ExtUpgrade::uniq() const
{
    qCDebug(LOG_LIB);

    return m_executable;
}


void ExtUpgrade::setExecutable(const QString _executable)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Executable" << _executable;

    m_executable = _executable;
}


void ExtUpgrade::setFilter(const QString _filter)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Filter" << _filter;

    m_filter = _filter;
}


void ExtUpgrade::setNull(const int _null)
{
    qCDebug(LOG_LIB);
    qCDebug(LOG_LIB) << "Null lines" << _null;
    if (_null < 0)
        return;

    m_null = _null;
}


void ExtUpgrade::readConfiguration()
{
    qCDebug(LOG_LIB);
    AbstractExtItem::readConfiguration();

    for (int i = directories().count() - 1; i >= 0; i--) {
        if (!QDir(directories().at(i))
                 .entryList(QDir::Files)
                 .contains(fileName()))
            continue;
        QSettings settings(
            QString("%1/%2").arg(directories().at(i)).arg(fileName()),
            QSettings::IniFormat);

        settings.beginGroup(QString("Desktop Entry"));
        setExecutable(settings.value(QString("Exec"), m_executable).toString());
        setNull(settings.value(QString("X-AW-Null"), m_null).toInt());
        // api == 3
        setFilter(settings.value(QString("X-AW-Filter"), m_filter).toString());
        settings.endGroup();
    }

    // update for current API
    if ((apiVersion() > 0) && (apiVersion() < AWEUAPI)) {
        qCWarning(LOG_LIB) << "Bump API version from" << apiVersion() << "to"
                           << AWEUAPI;
        setApiVersion(AWEUAPI);
        writeConfiguration();
    }
}


QVariantHash ExtUpgrade::run()
{
    qCDebug(LOG_LIB);
    if (!isActive())
        return value;

    if ((times == 1) && (process->state() == QProcess::NotRunning)) {
        QString cmd = QString("sh -c \"%1\"").arg(m_executable);
        qCInfo(LOG_LIB) << "Run cmd" << cmd;
        process->start(cmd);
    } else if (times >= interval()) {
        times = 0;
    }
    times++;

    return value;
}


int ExtUpgrade::showConfiguration(const QVariant args)
{
    Q_UNUSED(args)
    qCDebug(LOG_LIB);

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->lineEdit_command->setText(m_executable);
    ui->lineEdit_filter->setText(m_filter);
    ui->checkBox_active->setCheckState(isActive() ? Qt::Checked
                                                  : Qt::Unchecked);
    ui->spinBox_null->setValue(m_null);
    ui->spinBox_interval->setValue(interval());

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setNumber(ui->label_numberValue->text().toInt());
    setApiVersion(AWEUAPI);
    setExecutable(ui->lineEdit_command->text());
    setFilter(ui->lineEdit_filter->text());
    setActive(ui->checkBox_active->checkState() == Qt::Checked);
    setNull(ui->spinBox_null->value());
    setInterval(ui->spinBox_interval->value());

    writeConfiguration();
    return ret;
}


void ExtUpgrade::writeConfiguration() const
{
    qCDebug(LOG_LIB);
    AbstractExtItem::writeConfiguration();

    QSettings settings(
        QString("%1/%2").arg(directories().first()).arg(fileName()),
        QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("Exec"), m_executable);
    settings.setValue(QString("X-AW-Filter"), m_filter);
    settings.setValue(QString("X-AW-Null"), m_null);
    settings.endGroup();

    settings.sync();
}


void ExtUpgrade::updateValue()
{
    qCDebug(LOG_LIB);

    qCInfo(LOG_LIB) << "Cmd returns" << process->exitCode();
    qCInfo(LOG_LIB) << "Error" << process->readAllStandardError();

    QString qoutput = QTextCodec::codecForMib(106)
                          ->toUnicode(process->readAllStandardOutput())
                          .trimmed();
    value[tag(QString("pkgcount"))] = [this](QString output) {
        return m_filter.isEmpty()
                   ? output.split(QChar('\n'), QString::SkipEmptyParts).count()
                         - m_null
                   : output.split(QChar('\n'), QString::SkipEmptyParts)
                         .filter(QRegExp(m_filter))
                         .count();
    }(qoutput);
}


void ExtUpgrade::translate()
{
    qCDebug(LOG_LIB);

    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_number->setText(i18n("Tag"));
    ui->label_command->setText(i18n("Command"));
    ui->label_filter->setText(i18n("Filter"));
    ui->checkBox_active->setText(i18n("Active"));
    ui->label_null->setText(i18n("Null"));
    ui->label_interval->setText(i18n("Interval"));
}
