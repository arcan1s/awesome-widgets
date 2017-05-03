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
#include <QSettings>
#include <QStandardPaths>
#include <QTime>

#include "abstractextitemaggregator.h"
#include "awdebug.h"
#include "qcronscheduler.h"


AbstractExtItem::AbstractExtItem(QWidget *parent, const QString filePath)
    : QDialog(parent)
    , m_fileName(filePath)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    qCDebug(LOG_LIB) << "Desktop name" << filePath;

    m_name = m_fileName;
}


AbstractExtItem::~AbstractExtItem()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (m_socket) {
        m_socket->close();
        m_socket->removeServer(socket());
        delete m_socket;
    }
}


void AbstractExtItem::bumpApi(const int _newVer)
{
    qCDebug(LOG_LIB) << "Bump API using new version" << _newVer;

    // update for current API
    if ((apiVersion() > 0) && (apiVersion() < _newVer)) {
        qCWarning(LOG_LIB) << "Bump API version from" << apiVersion() << "to"
                           << _newVer;
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


QString AbstractExtItem::writtableConfig() const
{
    QString path = m_fileName;
    QString name = QFileInfo(path).fileName();
    path.remove(path.count() - name.count() - 1, name.count() + 1);
    QString dir = QFileInfo(path).fileName();

    return QString("%1/awesomewidgets/%2/%3")
        .arg(QStandardPaths::writableLocation(
            QStandardPaths::GenericDataLocation))
        .arg(dir)
        .arg(name);
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
    return m_fileName;
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


QString AbstractExtItem::tag(const QString _type) const
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


void AbstractExtItem::setComment(const QString _comment)
{
    qCDebug(LOG_LIB) << "Comment" << _comment;

    m_comment = _comment;
}


void AbstractExtItem::setCron(const QString _cron)
{
    qCDebug(LOG_LIB) << "Cron string" << _cron;
    // deinit module first
    if (m_scheduler) {
        disconnect(m_scheduler, SIGNAL(activated()), this,
                   SIGNAL(requestDataUpdate()));
        delete m_scheduler;
    }

    m_cron = _cron;
    if (cron().isEmpty())
        return;

    // init scheduler
    m_scheduler = new QCronScheduler(this);
    m_scheduler->parse(cron());
    connect(m_scheduler, SIGNAL(activated()), this,
            SIGNAL(requestDataUpdate()));
}


void AbstractExtItem::setInterval(const int _interval)
{
    qCDebug(LOG_LIB) << "Interval" << _interval;
    if (_interval <= 0)
        return;

    m_interval = _interval;
}


void AbstractExtItem::setName(const QString _name)
{
    qCDebug(LOG_LIB) << "Name" << _name;

    m_name = _name;
}


void AbstractExtItem::setNumber(int _number)
{
    qCDebug(LOG_LIB) << "Number" << _number;
    bool generateNumber = (_number == -1);
    if (generateNumber) {
        _number = []() {
            qCWarning(LOG_LIB) << "Number is empty, generate new one";
            // we suppose that currentTIme().msec() is always valid time
            qsrand(static_cast<uint>(QTime::currentTime().msec()));
            int n = qrand() % 1000;
            qCInfo(LOG_LIB) << "Generated number is" << n;
            return n;
        }();
    }

    m_number = _number;
    if (generateNumber)
        writeConfiguration();
}


void AbstractExtItem::setSocket(const QString _socket)
{
    qCDebug(LOG_LIB) << "Socket" << _socket;
    // remove old socket first
    deinitSocket();

    m_socketFile = _socket;
}


void AbstractExtItem::deinitSocket()
{
    if (!m_socket)
        return;

    m_socket->close();
    m_socket->removeServer(socket());
    delete m_socket;
    disconnect(m_socket, SIGNAL(newConnection()), this,
               SLOT(newConnectionReceived()));
}


void AbstractExtItem::initSocket()
{
    // remove old socket first
    deinitSocket();

    m_socket = new QLocalServer(this);
    bool listening = m_socket->listen(socket());
    qCInfo(LOG_LIB) << "Server listening on" << socket() << listening;
    connect(m_socket, SIGNAL(newConnection()), this,
            SLOT(newConnectionReceived()));
}


void AbstractExtItem::readConfiguration()
{
    QSettings settings(m_fileName, QSettings::IniFormat);

    settings.beginGroup(QString("Desktop Entry"));
    setName(settings.value(QString("Name"), name()).toString());
    setComment(settings.value(QString("Comment"), comment()).toString());
    setApiVersion(
        settings.value(QString("X-AW-ApiVersion"), apiVersion()).toInt());
    setActive(
        settings.value(QString("X-AW-Active"), QVariant(isActive())).toString()
        == QString("true"));
    setInterval(settings.value(QString("X-AW-Interval"), interval()).toInt());
    setNumber(settings.value(QString("X-AW-Number"), number()).toInt());
    setCron(settings.value(QString("X-AW-Schedule"), cron()).toString());
    setSocket(settings.value(QString("X-AW-Socket"), socket()).toString());
    settings.endGroup();
}


bool AbstractExtItem::tryDelete() const
{
    bool status = QFile::remove(m_fileName);
    qCInfo(LOG_LIB) << "Remove file" << m_fileName << status;

    return status;
}


void AbstractExtItem::writeConfiguration() const
{
    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("Encoding"), QString("UTF-8"));
    settings.setValue(QString("Name"), name());
    settings.setValue(QString("Comment"), comment());
    settings.setValue(QString("X-AW-ApiVersion"), apiVersion());
    settings.setValue(QString("X-AW-Active"), QVariant(isActive()).toString());
    settings.setValue(QString("X-AW-Interval"),
                      cron().isEmpty() ? QVariant(interval())
                                       : QVariant(cron()));
    settings.setValue(QString("X-AW-Number"), number());
    settings.setValue(QString("X-AW-Schedule"), cron());
    settings.setValue(QString("X-AW-Socket"), socket());
    settings.endGroup();

    settings.sync();
}


void AbstractExtItem::newConnectionReceived()
{
    emit(requestDataUpdate());
}
