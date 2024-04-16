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

#include <QSettings>

#include "awdebug.h"


ExtUpgrade::ExtUpgrade(QObject *_parent, const QString &_filePath)
    : AbstractExtItem(_parent, _filePath)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        ExtUpgrade::readConfiguration();

    m_values[tag("pkgcount")] = 0;

    m_process = new QProcess(nullptr);
    connect(m_process, &QProcess::finished, [this]() { return updateValue(); });
    m_process->waitForFinished(0);

    connect(this, &ExtUpgrade::requestDataUpdate, this, &ExtUpgrade::startProcess);
}


ExtUpgrade::~ExtUpgrade()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    m_process->kill();
    m_process->deleteLater();
    disconnect(this, &ExtUpgrade::requestDataUpdate, this, &ExtUpgrade::startProcess);
}


ExtUpgrade *ExtUpgrade::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    auto item = new ExtUpgrade(parent(), _fileName);
    copyDefaults(item);

    item->setExecutable(executable());
    item->setFilter(filter());
    item->setNumber(_number);
    item->setNull(null());

    return item;
}


QString ExtUpgrade::executable() const
{
    return m_executable;
}


QString ExtUpgrade::filter() const
{
    return m_filter;
}


int ExtUpgrade::null() const
{
    return m_null;
}


QString ExtUpgrade::uniq() const
{
    return executable();
}


void ExtUpgrade::setExecutable(const QString &_executable)
{
    qCDebug(LOG_LIB) << "Executable" << _executable;

    m_executable = _executable;
}


void ExtUpgrade::setFilter(const QString &_filter)
{
    qCDebug(LOG_LIB) << "Filter" << _filter;

    m_filter = _filter;
}


void ExtUpgrade::setNull(const int _null)
{
    qCDebug(LOG_LIB) << "Null lines" << _null;
    if (_null < 0)
        return;

    m_null = _null;
}


void ExtUpgrade::readConfiguration()
{
    AbstractExtItem::readConfiguration();

    QSettings settings(filePath(), QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setExecutable(settings.value("Exec", executable()).toString());
    setNull(settings.value("X-AW-Null", null()).toInt());
    // api == 3
    setFilter(settings.value("X-AW-Filter", filter()).toString());
    settings.endGroup();

    bumpApi(AW_EXTUPGRADE_API);
}


QVariantHash ExtUpgrade::run()
{
    if (m_process->state() != QProcess::NotRunning)
        return m_values;
    startTimer();

    return m_values;
}


int ExtUpgrade::showConfiguration(QWidget *_parent, const QVariant &_args)
{
    Q_UNUSED(_args)

    auto dialog = new QDialog(_parent);
    auto ui = new Ui::ExtUpgrade();
    ui->setupUi(dialog);
    translate(ui);

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->lineEdit_command->setText(executable());
    ui->lineEdit_filter->setText(filter());
    ui->checkBox_active->setCheckState(isActive() ? Qt::Checked : Qt::Unchecked);
    ui->spinBox_null->setValue(null());
    ui->lineEdit_schedule->setText(cron());
    ui->lineEdit_socket->setText(socket());
    ui->spinBox_interval->setValue(interval());

    auto ret = dialog->exec();
    if (ret == 1) {
        setName(ui->lineEdit_name->text());
        setComment(ui->lineEdit_comment->text());
        setNumber(ui->label_numberValue->text().toInt());
        setApiVersion(AW_EXTUPGRADE_API);
        setExecutable(ui->lineEdit_command->text());
        setFilter(ui->lineEdit_filter->text());
        setActive(ui->checkBox_active->checkState() == Qt::Checked);
        setNull(ui->spinBox_null->value());
        setCron(ui->lineEdit_schedule->text());
        setSocket(ui->lineEdit_socket->text());
        setInterval(ui->spinBox_interval->value());

        writeConfiguration();
    }

    dialog->deleteLater();
    delete ui;

    return ret;
}


void ExtUpgrade::writeConfiguration() const
{
    AbstractExtItem::writeConfiguration();

    QSettings settings(writableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("Exec", executable());
    settings.setValue("X-AW-Filter", filter());
    settings.setValue("X-AW-Null", null());
    settings.endGroup();

    settings.sync();
}


void ExtUpgrade::startProcess()
{
    qCInfo(LOG_LIB) << "Run cmd" << executable();
    m_process->start("sh", QStringList() << "-c" << executable());
}


void ExtUpgrade::updateValue()
{
    qCInfo(LOG_LIB) << "Cmd returns" << m_process->exitCode();
    qCInfo(LOG_LIB) << "Error" << m_process->readAllStandardError();

    auto qoutput = QString::fromUtf8(m_process->readAllStandardOutput()).trimmed();
    m_values[tag("pkgcount")] = [this](const QString &output) {
        return filter().isEmpty() ? output.split('\n', Qt::SkipEmptyParts).count() - null()
                                  : output.split('\n', Qt::SkipEmptyParts).filter(QRegularExpression(filter())).count();
    }(qoutput);

    emit(dataReceived(m_values));
}


void ExtUpgrade::translate(void *_ui)
{
    auto ui = reinterpret_cast<Ui::ExtUpgrade *>(_ui);

    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_number->setText(i18n("Tag"));
    ui->label_command->setText(i18n("Command"));
    ui->label_filter->setText(i18n("Filter"));
    ui->checkBox_active->setText(i18n("Active"));
    ui->label_null->setText(i18n("Null"));
    ui->label_socket->setText(i18n("Socket"));
    ui->label_schedule->setText(i18n("Schedule"));
    ui->label_interval->setText(i18n("Interval"));
}
