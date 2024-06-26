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

#include "abstractextitem.h"

#include <QDir>
#include <QLocalServer>
#include <QRandomGenerator>
#include <QSettings>
#include <QStandardPaths>

#include "abstractextitemaggregator.h"
#include "qcronscheduler.h"


AbstractExtItem::AbstractExtItem(QObject *_parent, const QString &_filePath)
    : QObject(_parent)
    , m_filePath(_filePath)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    qCDebug(LOG_LIB) << "Desktop name" << _filePath;

    m_name = m_filePath;

    m_scheduler = new QCronScheduler(this);
    connect(m_scheduler, &QCronScheduler::activated, this, &AbstractExtItem::requestDataUpdate);

    m_socket = new QLocalServer(this);
    connect(m_socket, &QLocalServer::newConnection, this, &AbstractExtItem::requestDataUpdate);
}


AbstractExtItem::~AbstractExtItem()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    m_scheduler->stop();
    m_scheduler->deleteLater();

    m_socket->close();
    m_socket->deleteLater();
}


void AbstractExtItem::bumpApi(const int _newVer)
{
    qCDebug(LOG_LIB) << "Bump API using new version" << _newVer;

    // update for current API
    if ((apiVersion() > 0) && (apiVersion() < _newVer)) {
        qCWarning(LOG_LIB) << "Bump API version from" << apiVersion() << "to" << _newVer;
        setApiVersion(_newVer);
        writeConfiguration();
    }
}


void AbstractExtItem::copyDefaults(AbstractExtItem *_other) const
{
    _other->setActive(isActive());
    _other->setApiVersion(apiVersion());
    _other->setComment(comment());
    _other->setCron(cron());
    _other->setInterval(interval());
    _other->setName(name());
    _other->setSocket(socket());
}


void AbstractExtItem::startTimer()
{
    if (!socket().isEmpty())
        // check if there is active socket setup
        return;
    else if (!cron().isEmpty())
        // check if there is active scheduler
        return;
    else if (m_times == 1)
        // check if it is time to update
        emit(requestDataUpdate());

    // update counter value
    if (m_times >= interval())
        m_times = 0;
    m_times++;
}


QString AbstractExtItem::writableConfig() const
{
    auto path = m_filePath;
    auto name = fileName();
    // extract subdirectory
    path.remove(path.length() - name.length() - 1, name.length() + 1);
    auto dir = QFileInfo(path).fileName();

    return QString("%1/awesomewidgets/%2/%3")
        .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation), dir, name);
}


int AbstractExtItem::apiVersion() const
{
    return m_apiVersion;
}


QString AbstractExtItem::comment() const
{
    return m_comment;
}


QString AbstractExtItem::cron() const
{
    return m_cron;
}


QString AbstractExtItem::fileName() const
{
    return QFileInfo(filePath()).fileName();
}


QString AbstractExtItem::filePath() const
{
    return m_filePath;
}


int AbstractExtItem::interval() const
{
    return m_interval;
}


bool AbstractExtItem::isActive() const
{
    return m_active;
}


QString AbstractExtItem::name() const
{
    return m_name;
}


int AbstractExtItem::number() const
{
    return m_number;
}


QString AbstractExtItem::socket() const
{
    return m_socketFile;
}


QString AbstractExtItem::tag(const QString &_type) const
{
    qCDebug(LOG_LIB) << "Tag type" << _type;

    return QString("%1%2").arg(_type).arg(number());
}


void AbstractExtItem::setApiVersion(const int _apiVersion)
{
    qCDebug(LOG_LIB) << "Version" << _apiVersion;

    m_apiVersion = _apiVersion;
}


void AbstractExtItem::setActive(const bool _state)
{
    qCDebug(LOG_LIB) << "State" << _state;

    m_active = _state;
}


void AbstractExtItem::setComment(const QString &_comment)
{
    qCDebug(LOG_LIB) << "Comment" << _comment;

    m_comment = _comment;
}


void AbstractExtItem::setCron(const QString &_cron)
{
    qCDebug(LOG_LIB) << "Cron string" << _cron;

    m_cron = _cron;
    if (m_cron.isEmpty()) { // disable cron timer
        m_scheduler->stop();
    } else {
        m_scheduler->parse(m_cron);
        m_scheduler->start();
    }
}


void AbstractExtItem::setInterval(const int _interval)
{
    qCDebug(LOG_LIB) << "Interval" << _interval;
    if (_interval <= 0)
        return;

    m_interval = _interval;
}


void AbstractExtItem::setName(const QString &_name)
{
    qCDebug(LOG_LIB) << "Name" << _name;

    m_name = _name;
}


void AbstractExtItem::setNumber(int _number)
{
    qCDebug(LOG_LIB) << "Number" << _number;
    auto generateNumber = (_number == -1);
    if (generateNumber) {
        _number = []() {
            qCWarning(LOG_LIB) << "Number is empty, generate new one";
            auto n = QRandomGenerator::global()->bounded(1000);
            qCInfo(LOG_LIB) << "Generated number is" << n;
            return n;
        }();
    }

    m_number = _number;
    if (generateNumber)
        writeConfiguration();
}


void AbstractExtItem::setSocket(const QString &_socket)
{
    qCDebug(LOG_LIB) << "Socket" << _socket;

    m_socketFile = _socket;
}


void AbstractExtItem::initSocket()
{
    // reload local socket
    m_socket->close();

    auto listening = m_socket->listen(m_socketFile);
    qCInfo(LOG_LIB) << "Server listening on" << m_socketFile << listening;
}


void AbstractExtItem::readConfiguration()
{
    QSettings settings(m_filePath, QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setName(settings.value("Name", name()).toString());
    setComment(settings.value("Comment", comment()).toString());
    setApiVersion(settings.value("X-AW-ApiVersion", apiVersion()).toInt());
    setActive(settings.value("X-AW-Active", isActive()).toBool());
    setInterval(settings.value("X-AW-Interval", interval()).toInt());
    setNumber(settings.value("X-AW-Number", number()).toInt());
    setCron(settings.value("X-AW-Schedule", cron()).toString());
    setSocket(settings.value("X-AW-Socket", socket()).toString());
    settings.endGroup();
}


bool AbstractExtItem::tryDelete() const
{
    auto status = QFile::remove(m_filePath);
    qCInfo(LOG_LIB) << "Remove file" << m_filePath << status;

    return status;
}


void AbstractExtItem::writeConfiguration() const
{
    QSettings settings(writableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("Encoding", "UTF-8");
    settings.setValue("Name", name());
    settings.setValue("Comment", comment());
    settings.setValue("X-AW-ApiVersion", apiVersion());
    settings.setValue("X-AW-Active", isActive());
    settings.setValue("X-AW-Interval", interval());
    settings.setValue("X-AW-Number", number());
    settings.setValue("X-AW-Schedule", cron());
    settings.setValue("X-AW-Socket", socket());
    settings.endGroup();

    settings.sync();
}
