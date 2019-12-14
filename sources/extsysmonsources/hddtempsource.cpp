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


#include "hddtempsource.h"

#include <QDir>
#include <QProcess>
#include <QTextCodec>

#include "awdebug.h"


HDDTemperatureSource::HDDTemperatureSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 2);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_devices = _args.at(0).split(',', QString::SkipEmptyParts);
    m_cmd = _args.at(1);

    m_smartctl = m_cmd.contains("smartctl");
    qCInfo(LOG_ESS) << "Parse as smartctl" << m_smartctl;

    for (auto &device : m_devices) {
        m_processes[device] = new QProcess(nullptr);
        // fucking magic from http://doc.qt.io/qt-5/qprocess.html#finished
        connect(m_processes[device], QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [=](int, QProcess::ExitStatus) { return updateValue(device); });
        m_processes[device]->waitForFinished(0);
    }
}


HDDTemperatureSource::~HDDTemperatureSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    for (auto &device : m_devices) {
        m_processes[device]->kill();
        m_processes[device]->deleteLater();
    }
}


QStringList HDDTemperatureSource::allHdd()
{
    QStringList allDevices = QDir("/dev").entryList(QDir::System, QDir::Name);
    QStringList devices = allDevices.filter(QRegExp("^[hms]d[a-z]$"));
    for (int i = 0; i < devices.count(); i++)
        devices[i] = QString("/dev/%1").arg(devices.at(i));

    qCInfo(LOG_ESS) << "Device list" << devices;
    return devices;
}


QVariant HDDTemperatureSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    QString device = _source;
    device.remove("hdd/temperature");
    // run cmd
    if (m_processes[device]->state() == QProcess::NotRunning)
        m_processes[device]->start(QString("%1 %2").arg(m_cmd).arg(device));

    return m_values[device];
}


QVariantMap HDDTemperatureSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    QString device = _source;
    device.remove("hdd/temperature");
    QVariantMap data;
    data["min"] = 0.0;
    data["max"] = 0.0;
    data["name"] = QString("HDD '%1' temperature").arg(device);
    data["type"] = "float";
    data["units"] = "°C";

    return data;
}


QStringList HDDTemperatureSource::sources() const
{
    QStringList sources;
    for (auto &device : m_devices)
        sources.append(QString("hdd/temperature%1").arg(device));

    return sources;
}


void HDDTemperatureSource::updateValue(const QString &_device)
{
    qCDebug(LOG_ESS) << "Called with device" << _device;

    qCInfo(LOG_ESS) << "Cmd returns" << m_processes[_device]->exitCode();
    QString qdebug = QTextCodec::codecForMib(106)
                         ->toUnicode(m_processes[_device]->readAllStandardError())
                         .trimmed();
    qCInfo(LOG_ESS) << "Error" << qdebug;
    QString qoutput = QTextCodec::codecForMib(106)
                          ->toUnicode(m_processes[_device]->readAllStandardOutput())
                          .trimmed();
    qCInfo(LOG_ESS) << "Output" << qoutput;

    // parse
    if (m_smartctl) {
        QStringList lines = qoutput.split('\n', QString::SkipEmptyParts);
        for (auto &str : lines) {
            if (!str.startsWith("194"))
                continue;
            if (str.split(' ', QString::SkipEmptyParts).count() < 9)
                continue;
            m_values[_device] = str.split(' ', QString::SkipEmptyParts).at(9).toFloat();
            break;
        }
    } else {
        QStringList lines = qoutput.split(':', QString::SkipEmptyParts);
        if (lines.count() >= 3) {
            QString temp = lines.at(2);
            temp.remove(QChar(0260)).remove('C');
            m_values[_device] = temp.toFloat();
        }
    }

    emit(dataReceived(m_values));
}
