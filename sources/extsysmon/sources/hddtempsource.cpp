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

#include <QProcess>
#include <QTextCodec>

#include "awdebug.h"


HDDTemperatureSource::HDDTemperatureSource(QObject *parent,
                                           const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 2);
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    m_devices = args.at(0).split(QChar(','), QString::SkipEmptyParts);
    m_cmd = args.at(1);

    m_smartctl = m_cmd.contains(QString("smartctl"));
    qCInfo(LOG_ESM) << "Parse as smartctl" << m_smartctl;

    for (auto device : m_devices) {
        m_processes[device] = new QProcess(nullptr);
        // fucking magic from http://doc.qt.io/qt-5/qprocess.html#finished
        connect(m_processes[device],
                static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
                    &QProcess::finished),
                [this, device](int, QProcess::ExitStatus) {
                    return updateValue(device);
                });
        m_processes[device]->waitForFinished(0);
    }
}


HDDTemperatureSource::~HDDTemperatureSource()
{
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    for (auto device : m_devices) {
        m_processes[device]->kill();
        m_processes[device]->deleteLater();
    }
}


QVariant HDDTemperatureSource::data(QString source)
{
    qCDebug(LOG_ESM) << "Source" << source;

    QString device = source.remove(QString("hdd/temperature"));
    // run cmd
    if (m_processes[device]->state() == QProcess::NotRunning)
        m_processes[device]->start(QString("%1 %2").arg(m_cmd).arg(device));

    return m_values[device];
}


QVariantMap HDDTemperatureSource::initialData(QString source) const
{
    qCDebug(LOG_ESM) << "Source" << source;

    QString device = source.remove(QString("hdd/temperature"));
    QVariantMap data;
    data[QString("min")] = 0.0;
    data[QString("max")] = 0.0;
    data[QString("name")] = QString("HDD '%1' temperature").arg(device);
    data[QString("type")] = QString("float");
    data[QString("units")] = QString("°C");

    return data;
}


QStringList HDDTemperatureSource::sources() const
{
    QStringList sources;
    for (auto device : m_devices)
        sources.append(QString("hdd/temperature%1").arg(device));

    return sources;
}


void HDDTemperatureSource::updateValue(const QString &device)
{
    qCDebug(LOG_LIB) << "Called with device" << device;

    qCInfo(LOG_LIB) << "Cmd returns" << m_processes[device]->exitCode();
    QString qdebug
        = QTextCodec::codecForMib(106)
              ->toUnicode(m_processes[device]->readAllStandardError())
              .trimmed();
    qCInfo(LOG_LIB) << "Error" << qdebug;
    QString qoutput
        = QTextCodec::codecForMib(106)
              ->toUnicode(m_processes[device]->readAllStandardOutput())
              .trimmed();
    qCInfo(LOG_LIB) << "Output" << qoutput;

    // parse
    if (m_smartctl) {
        for (auto str : qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
            if (!str.startsWith(QString("194")))
                continue;
            if (str.split(QChar(' '), QString::SkipEmptyParts).count() < 9)
                continue;
            m_values[device] = str.split(QChar(' '), QString::SkipEmptyParts)
                                   .at(9)
                                   .toFloat();
            break;
        }
    } else {
        if (qoutput.split(QChar(':'), QString::SkipEmptyParts).count() >= 3) {
            QString temp
                = qoutput.split(QChar(':'), QString::SkipEmptyParts).at(2);
            temp.remove(QChar(0260)).remove(QChar('C'));
            m_values[device] = temp.toFloat();
        }
    }
}
