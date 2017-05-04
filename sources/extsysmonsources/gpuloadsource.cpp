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

#include <QFile>
#include <QProcess>
#include <QTextCodec>

#include "awdebug.h"


GPULoadSource::GPULoadSource(QObject *parent, const QStringList &args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 1);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_device = args.at(0);

    m_process = new QProcess(nullptr);
    // fucking magic from http://doc.qt.io/qt-5/qprocess.html#finished
    connect(m_process,
            static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
                &QProcess::finished),
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
    QString gpu = QString("disable");
    QFile moduleFile(QString("/proc/modules"));
    if (!moduleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(LOG_AW) << "Could not open file as text"
                          << moduleFile.fileName();
        return gpu;
    }

    QString output = moduleFile.readAll();
    moduleFile.close();
    if (output.contains(QString("fglrx")))
        gpu = QString("ati");
    else if (output.contains(QString("nvidia")))
        gpu = QString("nvidia");

    qCInfo(LOG_ESM) << "Device" << gpu;
    return gpu;
}


QVariant GPULoadSource::data(const QString &source)
{
    qCDebug(LOG_ESS) << "Source" << source;

    if (source == QString("gpu/load"))
        run();

    return m_values[source];
}


QVariantMap GPULoadSource::initialData(const QString &source) const
{
    qCDebug(LOG_ESS) << "Source" << source;

    QVariantMap data;
    if (source == QString("gpu/load")) {
        data[QString("min")] = 0.0;
        data[QString("max")] = 100.0;
        data[QString("name")] = QString("GPU usage");
        data[QString("type")] = QString("float");
        data[QString("units")] = QString("%");
    }

    return data;
}


void GPULoadSource::run()
{
    if ((m_device != QString("nvidia")) && (m_device != QString("ati")))
        return;
    // build cmd
    QString cmd = m_device == QString("nvidia")
                      ? QString("nvidia-smi -q -x")
                      : QString("aticonfig --od-getclocks");
    qCInfo(LOG_ESS) << "cmd" << cmd;

    m_process->start(cmd);
}


QStringList GPULoadSource::sources() const
{
    QStringList sources;
    sources.append(QString("gpu/load"));

    return sources;
}


void GPULoadSource::updateValue()
{
    qCInfo(LOG_ESS) << "Cmd returns" << m_process->exitCode();
    QString qdebug = QTextCodec::codecForMib(106)
                         ->toUnicode(m_process->readAllStandardError())
                         .trimmed();
    qCInfo(LOG_ESS) << "Error" << qdebug;
    QString qoutput = QTextCodec::codecForMib(106)
                          ->toUnicode(m_process->readAllStandardOutput())
                          .trimmed();
    qCInfo(LOG_ESS) << "Output" << qoutput;

    if (m_device == QString("nvidia")) {
        for (auto &str : qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
            if (!str.contains(QString("<gpu_util>")))
                continue;
            QString load = str.remove(QString("<gpu_util>"))
                               .remove(QString("</gpu_util>"))
                               .remove(QChar('%'));
            m_values[QString("gpu/load")] = load.toFloat();
            break;
        }
    } else if (m_device == QString("ati")) {
        for (auto &str : qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
            if (!str.contains(QString("load")))
                continue;
            QString load
                = str.split(QChar(' '), QString::SkipEmptyParts)[3].remove(
                    QChar('%'));
            m_values[QString("gpu/load")] = load.toFloat();
            break;
        }
    }

    emit(dataReceived(m_values));
}
