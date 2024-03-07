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

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QJsonDocument>
#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"


ExtScript::ExtScript(QObject *_parent, const QString &_filePath)
    : AbstractExtItem(_parent, _filePath)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        ExtScript::readConfiguration();
    readJsonFilters();

    m_values[tag("custom")] = "";

    m_process = new QProcess(nullptr);
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(updateValue()));
    m_process->waitForFinished(0);

    connect(this, SIGNAL(requestDataUpdate()), this, SLOT(startProcess()));
}


ExtScript::~ExtScript()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    disconnect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(updateValue()));
    m_process->kill();
    m_process->deleteLater();
    disconnect(this, SIGNAL(requestDataUpdate()), this, SLOT(startProcess()));
}


ExtScript *ExtScript::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    auto item = new ExtScript(parent(), _fileName);
    copyDefaults(item);
    item->setExecutable(executable());
    item->setNumber(_number);
    item->setRedirect(redirect());
    item->setFilters(filters());

    return item;
}


QString ExtScript::jsonFiltersFile()
{
    QString fileName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                              "awesomewidgets/scripts/awesomewidgets-extscripts-filters.json");
    qCInfo(LOG_LIB) << "Filters file" << fileName;

    return fileName;
}


QString ExtScript::executable() const
{
    return m_executable;
}


QStringList ExtScript::filters() const
{
    return m_filters;
}


ExtScript::Redirect ExtScript::redirect() const
{
    return m_redirect;
}


QString ExtScript::uniq() const
{
    return executable();
}


QString ExtScript::strRedirect() const
{
    QString value;
    switch (redirect()) {
    case Redirect::stdout2stderr:
        value = "stdout2stderr";
        break;
    case Redirect::stderr2stdout:
        value = "stderr2stdout";
        break;
    case Redirect::swap:
        value = "swap";
        break;
    case Redirect::nothing:
        value = "nothing";
        break;
    }

    return value;
}


void ExtScript::setExecutable(const QString &_executable)
{
    qCDebug(LOG_LIB) << "Executable" << _executable;

    m_executable = _executable;
}


void ExtScript::setFilters(const QStringList &_filters)
{
    qCDebug(LOG_LIB) << "Filters" << _filters;

    std::for_each(_filters.cbegin(), _filters.cend(),
                  [this](const QString &filter) { return updateFilter(filter, true); });
}


void ExtScript::setRedirect(const Redirect _redirect)
{
    qCDebug(LOG_LIB) << "Redirect" << static_cast<int>(_redirect);

    m_redirect = _redirect;
}


void ExtScript::setStrRedirect(const QString &_redirect)
{
    qCDebug(LOG_LIB) << "Redirect" << _redirect;

    if (_redirect == "stdout2sdterr")
        setRedirect(Redirect::stdout2stderr);
    else if (_redirect == "stderr2sdtout")
        setRedirect(Redirect::stderr2stdout);
    else if (_redirect == "swap")
        setRedirect(Redirect::swap);
    else
        setRedirect(Redirect::nothing);
}


QString ExtScript::applyFilters(QString _value) const
{
    qCDebug(LOG_LIB) << "Value" << _value;

    for (auto &filt : filters()) {
        qCInfo(LOG_LIB) << "Found filter" << filt;
        auto filter = m_jsonFilters[filt].toMap();
        if (filter.isEmpty()) {
            qCWarning(LOG_LIB) << "Could not find filter" << _value << "in the json";
            continue;
        }
        for (auto &f : filter.keys())
            _value.replace(f, filter[f].toString());
    }

    return _value;
}


void ExtScript::updateFilter(const QString &_filter, const bool _add)
{
    qCDebug(LOG_LIB) << "Should be added filters" << _add << "from" << _filter;

    if (_add) {
        if (filters().contains(_filter))
            return;
        m_filters.append(_filter);
    } else {
        m_filters.removeOne(_filter);
    }
}


void ExtScript::readConfiguration()
{
    AbstractExtItem::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setExecutable(settings.value("Exec", executable()).toString());
    setStrRedirect(settings.value("X-AW-Redirect", strRedirect()).toString());
    // api == 3
    setFilters(settings.value("X-AW-Filters", filters()).toString().split(',', Qt::SkipEmptyParts));
    settings.endGroup();

    bumpApi(AW_EXTSCRIPT_API);
}


void ExtScript::readJsonFilters()
{
    auto fileName = jsonFiltersFile();
    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(LOG_LIB) << "Could not open" << fileName;
        return;
    }
    QString jsonText = jsonFile.readAll();
    jsonFile.close();

    QJsonParseError error{};
    auto jsonDoc = QJsonDocument::fromJson(jsonText.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_LIB) << "Parse error" << error.errorString();
        return;
    }
    m_jsonFilters = jsonDoc.toVariant().toMap();

    qCInfo(LOG_LIB) << "Filters" << m_jsonFilters;
}


QVariantHash ExtScript::run()
{
    if (m_process->state() != QProcess::NotRunning)
        return m_values;
    startTimer();

    return m_values;
}


int ExtScript::showConfiguration(QWidget *_parent, const QVariant &_args)
{
    Q_UNUSED(_args)

    auto dialog = new QDialog(_parent);
    auto ui = new Ui::ExtScript();
    ui->setupUi(dialog);
    translate(ui);

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->lineEdit_command->setText(executable());
    ui->checkBox_active->setCheckState(isActive() ? Qt::Checked : Qt::Unchecked);
    ui->comboBox_redirect->setCurrentIndex(static_cast<int>(redirect()));
    ui->lineEdit_schedule->setText(cron());
    ui->lineEdit_socket->setText(socket());
    ui->spinBox_interval->setValue(interval());
    // filters
    ui->checkBox_colorFilter->setCheckState(filters().contains("color") ? Qt::Checked : Qt::Unchecked);
    ui->checkBox_linesFilter->setCheckState(filters().contains("newline") ? Qt::Checked : Qt::Unchecked);
    ui->checkBox_spaceFilter->setCheckState(filters().contains("space") ? Qt::Checked : Qt::Unchecked);

    int ret = dialog->exec();
    if (ret == 1) {
        setName(ui->lineEdit_name->text());
        setComment(ui->lineEdit_comment->text());
        setNumber(ui->label_numberValue->text().toInt());
        setApiVersion(AW_EXTSCRIPT_API);
        setExecutable(ui->lineEdit_command->text());
        setActive(ui->checkBox_active->checkState() == Qt::Checked);
        setRedirect(static_cast<Redirect>(ui->comboBox_redirect->currentIndex()));
        setCron(ui->lineEdit_schedule->text());
        setSocket(ui->lineEdit_socket->text());
        setInterval(ui->spinBox_interval->value());
        // filters
        updateFilter("color", ui->checkBox_colorFilter->checkState() == Qt::Checked);
        updateFilter("newline", ui->checkBox_linesFilter->checkState() == Qt::Checked);
        updateFilter("space", ui->checkBox_spaceFilter->checkState() == Qt::Checked);

        writeConfiguration();
    }

    dialog->deleteLater();
    delete ui;

    return ret;
}


void ExtScript::writeConfiguration() const
{
    AbstractExtItem::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("Exec", executable());
    settings.setValue("X-AW-Redirect", strRedirect());
    settings.setValue("X-AW-Filters", filters().join(','));
    settings.endGroup();

    settings.sync();
}


void ExtScript::startProcess()
{
    qCInfo(LOG_LIB) << "Run cmd" << executable();
    m_process->start("sh", QStringList() << "-c" << executable());
}


void ExtScript::updateValue()
{
    qCInfo(LOG_LIB) << "Cmd returns" << m_process->exitCode();
    auto qdebug = QString::fromUtf8(m_process->readAllStandardError()).trimmed();
    qCInfo(LOG_LIB) << "Error" << qdebug;
    auto qoutput = QString::fromUtf8(m_process->readAllStandardOutput()).trimmed();
    qCInfo(LOG_LIB) << "Output" << qoutput;
    QString strValue;

    switch (redirect()) {
    case Redirect::stdout2stderr:
        break;
    case Redirect::stderr2stdout:
        strValue = QString("%1\n%2").arg(qdebug).arg(qoutput);
        break;
    case Redirect::swap:
        strValue = qdebug;
        break;
    case Redirect::nothing:
        strValue = qoutput;
        break;
    }

    // filters
    m_values[tag("custom")] = applyFilters(strValue);
    emit(dataReceived(m_values));
}


void ExtScript::translate(void *_ui)
{
    auto ui = reinterpret_cast<Ui::ExtScript *>(_ui);

    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_number->setText(i18n("Tag"));
    ui->label_command->setText(i18n("Command"));
    ui->checkBox_active->setText(i18n("Active"));
    ui->label_redirect->setText(i18n("Redirect"));
    ui->label_schedule->setText(i18n("Schedule"));
    ui->label_socket->setText(i18n("Socket"));
    ui->label_interval->setText(i18n("Interval"));
    ui->groupBox_filters->setTitle(i18n("Additional filters"));
    ui->checkBox_colorFilter->setText(i18n("Wrap colors"));
    ui->checkBox_linesFilter->setText(i18n("Wrap new lines"));
    ui->checkBox_spaceFilter->setText(i18n("Wrap spaces"));
}
