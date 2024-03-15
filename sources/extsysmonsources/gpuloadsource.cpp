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
#include "gpuloadsource.h"

#include <ksysguard/formatter/Unit.h>
#include <ksysguard/systemstats/SensorInfo.h>

#include <QFile>
#include <QProcess>

#include "awdebug.h"


GPULoadSource::GPULoadSource(QObject *_parent, const QStringList &_args)
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


GPULoadSource::~GPULoadSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_process->kill();
    m_process->deleteLater();
}


QString GPULoadSource::autoGpu()
{
    QString gpu = "disable";
    QFile moduleFile("/proc/modules");
    if (!moduleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(LOG_AW) << "Could not open file as text" << moduleFile.fileName();
        return gpu;
    }

    QString output = moduleFile.readAll();
    moduleFile.close();
    if (output.contains("fglrx"))
        gpu = "ati";
    else if (output.contains("nvidia"))
        gpu = "nvidia";

    qCInfo(LOG_ESM) << "Device" << gpu;
    return gpu;
}


QVariant GPULoadSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (_source == "gpu/load")
        run();

    return m_values[_source];
}


KSysGuard::SensorInfo *GPULoadSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    auto data = new KSysGuard::SensorInfo();
    if (_source == "gpu/load") {
        data->min = 0.0;
        data->max = 100.0;
        data->name = "GPU usage";
        data->variantType = QVariant::Double;
        data->unit = KSysGuard::UnitPercent;
    }

    return data;
}


void GPULoadSource::run()
{
    if ((m_device != "nvidia") && (m_device != "ati"))
        return;
    // build cmd
    QString cmd = m_device == "nvidia" ? "nvidia-smi" : "aticonfig";
    auto args = m_device == "nvidia" ? QStringList({"-q", "-x"}) : QStringList({"--od-getclocks"});
    qCInfo(LOG_ESS) << "cmd" << cmd;

    m_process->start(cmd, args);
}


QStringList GPULoadSource::sources() const
{
    QStringList sources;
    sources.append("gpu/load");

    return sources;
}


void GPULoadSource::updateValue()
{
    qCInfo(LOG_ESS) << "Cmd returns" << m_process->exitCode();
    QString qdebug = QString::fromUtf8(m_process->readAllStandardError()).trimmed();
    qCInfo(LOG_ESS) << "Error" << qdebug;
    QString qoutput = QString::fromUtf8(m_process->readAllStandardOutput()).trimmed();
    qCInfo(LOG_ESS) << "Output" << qoutput;

    if (m_device == "nvidia") {
        for (auto &str : qoutput.split('\n', Qt::SkipEmptyParts)) {
            if (!str.contains("<gpu_util>"))
                continue;
            auto load = str.remove("<gpu_util>").remove("</gpu_util>").remove('%');
            m_values["gpu/load"] = load.toFloat();
            break;
        }
    } else if (m_device == "ati") {
        for (auto &str : qoutput.split('\n', Qt::SkipEmptyParts)) {
            if (!str.contains("load"))
                continue;
            QString load = str.split(' ', Qt::SkipEmptyParts)[3].remove('%');
            m_values["gpu/load"] = load.toFloat();
            break;
        }
    }

    emit(dataReceived(m_values));
}
