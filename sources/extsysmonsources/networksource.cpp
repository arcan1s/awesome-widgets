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

#include "networksource.h"

#include <QNetworkInterface>
#include <QProcess>

#include "awdebug.h"


NetworkSource::NetworkSource(QObject *_parent)
    : AbstractExtSysMonSource(_parent)
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_process = new QProcess(nullptr);
    // fucking magic from http://doc.qt.io/qt-5/qprocess.html#finished
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this](int, QProcess::ExitStatus) { return updateSsid(); });
    m_process->waitForFinished(0);
}


NetworkSource::~NetworkSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_process->kill();
    m_process->deleteLater();
}


QVariant NetworkSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (_source == "device") {
        return NetworkSource::getCurrentDevice();
    } else if (_source == "ssid") {
        if (m_process->state() == QProcess::ProcessState::NotRunning)
            m_process->start("iwgetid", {"-r"});
        return m_currentSsid;
    }

    return {};
}


QHash<QString, KSysGuard::SensorInfo *> NetworkSource::sources() const
{
    auto result = QHash<QString, KSysGuard::SensorInfo *>();

    result.insert("device", makeSensorInfo("Current network device name", QVariant::String));
    result.insert("ssid", makeSensorInfo("Current SSID name", QVariant::String));

    return result;
}


void NetworkSource::updateSsid()
{
    qCInfo(LOG_ESS) << "Cmd returns" << m_process->exitCode();
    QString qdebug = QString::fromUtf8(m_process->readAllStandardError()).trimmed();
    qCInfo(LOG_ESS) << "Error" << qdebug;
    QString qoutput = QString::fromUtf8(m_process->readAllStandardOutput()).trimmed();
    qCInfo(LOG_ESS) << "Output" << qoutput;

    m_currentSsid = qoutput;
}


QString NetworkSource::getCurrentDevice()
{
    qCDebug(LOG_ESS) << "Get current devices";

    QString device = "lo";
    auto rawInterfaceList = QNetworkInterface::allInterfaces();
    qCInfo(LOG_ESS) << "Devices" << rawInterfaceList;

    for (auto &interface : rawInterfaceList) {
        if ((interface.flags().testFlag(QNetworkInterface::IsLoopBack))
            || (interface.flags().testFlag(QNetworkInterface::IsPointToPoint)))
            continue;
        if (interface.addressEntries().isEmpty())
            continue;
        device = interface.name();
        break;
    }

    return device;
}
