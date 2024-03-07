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


#include "gputempsource.h"

#include <QFile>
#include <QProcess>

#include "awdebug.h"


GPUTemperatureSource::GPUTemperatureSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 1);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_device = _args.at(0);

    m_process = new QProcess(nullptr);
    // fucking magic from http://doc.qt.io/qt-5/qprocess.html#finished
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this](int, QProcess::ExitStatus) { return updateValue(); });
    m_process->waitForFinished(0);
}


GPUTemperatureSource::~GPUTemperatureSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_process->kill();
    m_process->deleteLater();
}


QVariant GPUTemperatureSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (_source == "gpu/temperature")
        run();

    return m_values[_source];
}


QVariantMap GPUTemperatureSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    QVariantMap data;
    if (_source == "gpu/temperature") {
        data["min"] = 0.0;
        data["max"] = 0.0;
        data["name"] = "GPU temperature";
        data["type"] = "float";
        data["units"] = "°C";
    }

    return data;
}


void GPUTemperatureSource::run()
{
    if ((m_device != "nvidia") && (m_device != "ati"))
        return;
    // build cmd
    QString cmd = m_device == "nvidia" ? "nvidia-smi" : "aticonfig";
    auto args = m_device == "nvidia" ? QStringList({"-q", "-x"}) : QStringList({"--od-gettemperature"});
    qCInfo(LOG_ESS) << "cmd" << cmd;

    m_process->start(cmd, args);
}


QStringList GPUTemperatureSource::sources() const
{
    QStringList sources;
    sources.append("gpu/temperature");

    return sources;
}


void GPUTemperatureSource::updateValue()
{
    qCInfo(LOG_ESS) << "Cmd returns" << m_process->exitCode();
    QString qdebug = QString::fromUtf8(m_process->readAllStandardError()).trimmed();
    qCInfo(LOG_ESS) << "Error" << qdebug;
    QString qoutput = QString::fromUtf8(m_process->readAllStandardOutput()).trimmed();
    qCInfo(LOG_ESS) << "Output" << qoutput;

    if (m_device == "nvidia") {
        for (auto &str : qoutput.split('\n', Qt::SkipEmptyParts)) {
            if (!str.contains("<gpu_temp>"))
                continue;
            QString temp = str.remove("<gpu_temp>").remove("C</gpu_temp>");
            m_values["gpu/temperature"] = temp.toFloat();
            break;
        }
    } else if (m_device == "ati") {
        for (auto &str : qoutput.split('\n', Qt::SkipEmptyParts)) {
            if (!str.contains("Temperature"))
                continue;
            QString temp = str.split(' ', Qt::SkipEmptyParts).at(4);
            m_values["gpu/temperature"] = temp.toFloat();
            break;
        }
    }

    emit(dataReceived(m_values));
}
