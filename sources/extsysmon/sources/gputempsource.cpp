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

#include <QProcess>
#include <QTextCodec>

#include "awdebug.h"


GPUTemperatureSource::GPUTemperatureSource(QObject *parent,
                                           const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 1);
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    m_device = args.at(0);

    m_process = new QProcess(nullptr);
    // fucking magic from http://doc.qt.io/qt-5/qprocess.html#finished
    connect(m_process,
            static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
                &QProcess::finished),
            [this](int, QProcess::ExitStatus) { return updateValue(); });
    m_process->waitForFinished(0);
}


GPUTemperatureSource::~GPUTemperatureSource()
{
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    m_process->kill();
    m_process->deleteLater();
}


QVariant GPUTemperatureSource::data(QString source)
{
    qCDebug(LOG_ESM) << "Source" << source;

    if (source == QString("gpu/temperature"))
        run();

    return m_value;
}


QVariantMap GPUTemperatureSource::initialData(QString source) const
{
    qCDebug(LOG_ESM) << "Source" << source;

    QVariantMap data;
    if (source == QString("gpu/temperature")) {
        data[QString("min")] = 0.0;
        data[QString("max")] = 0.0;
        data[QString("name")] = QString("GPU temperature");
        data[QString("type")] = QString("float");
        data[QString("units")] = QString("°C");
    }

    return data;
}


void GPUTemperatureSource::run()
{
    if ((m_device != QString("nvidia")) && (m_device != QString("ati")))
        return;
    // build cmd
    QString cmd = m_device == QString("nvidia")
                      ? QString("nvidia-smi -q -x")
                      : QString("aticonfig --od-gettemperature");
    qCInfo(LOG_ESM) << "cmd" << cmd;

    m_process->start(cmd);
}


QStringList GPUTemperatureSource::sources() const
{
    QStringList sources;
    sources.append(QString("gpu/temperature"));

    return sources;
}


void GPUTemperatureSource::updateValue()
{
    qCInfo(LOG_LIB) << "Cmd returns" << m_process->exitCode();
    QString qdebug = QTextCodec::codecForMib(106)
                         ->toUnicode(m_process->readAllStandardError())
                         .trimmed();
    qCInfo(LOG_LIB) << "Error" << qdebug;
    QString qoutput = QTextCodec::codecForMib(106)
                          ->toUnicode(m_process->readAllStandardOutput())
                          .trimmed();
    qCInfo(LOG_LIB) << "Output" << qoutput;

    if (m_device == QString("nvidia")) {
        for (auto str : qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
            if (!str.contains(QString("<gpu_temp>")))
                continue;
            QString temp = str.remove(QString("<gpu_temp>"))
                               .remove(QString("C</gpu_temp>"));
            m_value = temp.toFloat();
            break;
        }
    } else if (m_device == QString("ati")) {
        for (auto str : qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
            if (!str.contains(QString("Temperature")))
                continue;
            QString temp = str.split(QChar(' '), QString::SkipEmptyParts).at(4);
            m_value = temp.toFloat();
            break;
        }
    }
}
