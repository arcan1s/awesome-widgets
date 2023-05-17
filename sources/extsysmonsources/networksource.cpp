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
#include <QTextCodec>

#include "awdebug.h"


NetworkSource::NetworkSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
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

    if (!m_values.contains(_source))
        run();
    return m_values.take(_source);
}


QVariantMap NetworkSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    QVariantMap data;
    if (_source == "network/current/name") {
        data["min"] = "";
        data["max"] = "";
        data["name"] = "Current network device name";
        data["type"] = "QString";
        data["units"] = "";
    } else if (_source == "network/current/ssid") {
        data["min"] = "";
        data["max"] = "";
        data["name"] = "Current SSID name";
        data["type"] = "QString";
        data["units"] = "";
    }

    return data;
}


void NetworkSource::run()
{
    m_values["network/current/name"] = NetworkSource::getCurrentDevice();
    m_process->start("iwgetid", QStringList() << "-r");
}


QStringList NetworkSource::sources() const
{
    QStringList sources;
    sources.append("network/current/name");
    sources.append("network/current/ssid");

    return sources;
}


void NetworkSource::updateSsid()
{
    qCInfo(LOG_ESS) << "Cmd returns" << m_process->exitCode();
    QString qdebug = QTextCodec::codecForMib(106)->toUnicode(m_process->readAllStandardError()).trimmed();
    qCInfo(LOG_ESS) << "Error" << qdebug;
    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(m_process->readAllStandardOutput()).trimmed();
    qCInfo(LOG_ESS) << "Output" << qoutput;

    m_values["network/current/ssid"] = qoutput;
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
