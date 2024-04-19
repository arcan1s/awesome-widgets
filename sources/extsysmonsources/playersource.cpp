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

#include "playersource.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QProcess>

#include "awdebug.h"


PlayerSource::PlayerSource(QObject *_parent, QString _player, QString _mpdAddress, int _mpdPort, QString _mpris,
                           int _symbols)
    : AbstractExtSysMonSource(_parent)
    , m_mpdAddress(std::move(_mpdAddress))
    , m_mpdPort(_mpdPort)
    , m_mpris(std::move(_mpris))
    , m_player(std::move(_player))
    , m_symbols(_symbols)
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    connect(&m_mpdSocket, &QTcpSocket::connected, this, &PlayerSource::mpdSocketConnected);
    connect(&m_mpdSocket, &QTcpSocket::readyRead, this, &PlayerSource::mpdSocketReadyRead);
    connect(&m_mpdSocket, &QTcpSocket::bytesWritten, this, &PlayerSource::mpdSocketWritten);
    m_mpdCached = defaultInfo();
}


PlayerSource::~PlayerSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_mpdSocket.close();
}


QVariant PlayerSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (!m_values.contains(_source))
        run(); // syncronous update of all values

    return m_values.take(_source);
}


QString PlayerSource::getAutoMpris()
{
    auto listServices = QDBusConnection::sessionBus().interface()->call(QDBus::BlockWithGui, "ListNames");
    if (listServices.arguments().isEmpty()) {
        qCWarning(LOG_ESS) << "Could not find any DBus service";
        return "";
    }
    auto arguments = listServices.arguments().first().toStringList();

    for (auto &service : arguments) {
        if (!service.startsWith("org.mpris.MediaPlayer2."))
            continue;
        qCInfo(LOG_ESS) << "Service found" << service;
        service.remove("org.mpris.MediaPlayer2.");
        return service;
    }

    return "";
}


void PlayerSource::run()
{
    // initial data
    if (m_player == "mpd") {
        // mpd
        m_values = getPlayerMpdInfo();
    } else if (m_player == "mpris") {
        // players which supports mpris
        if (m_dbusMutex.tryLock()) {
            QString mpris = m_mpris == "auto" ? getAutoMpris() : m_mpris;
            m_values = getPlayerMprisInfo(mpris);
            m_dbusMutex.unlock();
        }
    }

    // dymanic properties
    // solid
    m_values["salbum"] = stripString(m_values["album"].toString(), m_symbols);
    m_values["sartist"] = stripString(m_values["artist"].toString(), m_symbols);
    m_values["stitle"] = stripString(m_values["title"].toString(), m_symbols);
    // dynamic
    m_values["dalbum"] = buildString(m_values["dalbum"].toString(), m_values["album"].toString(), m_symbols);
    m_values["dartist"] = buildString(m_values["dartist"].toString(), m_values["artist"].toString(), m_symbols);
    m_values["dtitle"] = buildString(m_values["dtitle"].toString(), m_values["title"].toString(), m_symbols);
}


QHash<QString, KSysGuard::SensorInfo *> PlayerSource::sources() const
{
    auto result = QHash<QString, KSysGuard::SensorInfo *>();

    result.insert("album", makeSensorInfo("Current song album", QMetaType::QString));
    result.insert("salbum",
                  makeSensorInfo(QString("Current song album (%1 symbols)").arg(m_symbols), QMetaType::QString));
    result.insert("dalbum", makeSensorInfo(QString("Current song album (%1 symbols, dynamic)").arg(m_symbols),
                                           QMetaType::QString));
    result.insert("artist", makeSensorInfo("Current song artist", QMetaType::QString));
    result.insert("sartist",
                  makeSensorInfo(QString("Current song artist (%1 symbols)").arg(m_symbols), QMetaType::QString));
    result.insert("dartist", makeSensorInfo(QString("Current song artist (%1 symbols, dynamic)").arg(m_symbols),
                                            QMetaType::QString));
    result.insert("duration", makeSensorInfo("Current song duration", QMetaType::Int, KSysGuard::UnitSecond));
    result.insert("progress", makeSensorInfo("Current song progress", QMetaType::Int, KSysGuard::UnitSecond));
    result.insert("title", makeSensorInfo("Current song title", QMetaType::QString));
    result.insert("stitle",
                  makeSensorInfo(QString("Current song title (%1 symbols)").arg(m_symbols), QMetaType::QString));
    result.insert("dtitle", makeSensorInfo(QString("Current song title (%1 symbols, dynamic)").arg(m_symbols),
                                           QMetaType::QString));

    return result;
}


QString PlayerSource::buildString(const QString &_current, const QString &_value, const int _s)
{
    qCDebug(LOG_ESS) << "Current value" << _current << "received" << _value << "will be stripped after" << _s;

    auto index = _value.indexOf(_current);
    if ((_current.isEmpty()) || ((index + _s + 1) > _value.length()))
        return QString("%1").arg(_value.left(_s), -_s, QLatin1Char(' '));
    else
        return QString("%1").arg(_value.mid(index + 1, _s), -_s, QLatin1Char(' '));
}


QString PlayerSource::stripString(const QString &_value, const int _s)
{
    qCDebug(LOG_ESS) << "New value" << _value << "will be stripped after" << _s;

    return _value.length() > _s ? QString("%1\u2026").arg(_value.left(_s - 1))
                                : _value.leftJustified(_s, QLatin1Char(' '));
}


bool PlayerSource::isMpdSocketConnected() const
{
    return (m_mpdSocket.state() == QAbstractSocket::ConnectedState);
}


void PlayerSource::mpdSocketConnected()
{
    qCDebug(LOG_ESS) << "MPD socket connected to" << m_mpdSocket.peerName() << "with state" << m_mpdSocket.state();
}


void PlayerSource::mpdSocketReadyRead()
{
    auto qoutput = QString::fromUtf8(m_mpdSocket.readAll()).trimmed();
    qCInfo(LOG_ESS) << "Output" << qoutput;

    // parse
    for (auto &str : qoutput.split('\n', Qt::SkipEmptyParts)) {
        if (str.split(": ", Qt::SkipEmptyParts).count() == 2) {
            // "Metadata: data"
            QString metadata = str.split(": ", Qt::SkipEmptyParts).first().toLower();
            QString data = str.split(": ", Qt::SkipEmptyParts).last().trimmed();
            // there are one more time...
            if ((metadata == "time") && (data.contains(':'))) {
                QStringList times = data.split(':');
                m_mpdCached["duration"] = times.at(0).toInt();
                m_mpdCached["progress"] = times.at(1).toInt();
            } else if (m_metadata.contains(metadata)) {
                m_mpdCached[metadata] = data;
            }
        }
    }

    emit(dataReceived(m_mpdCached));
}


void PlayerSource::mpdSocketWritten(const qint64 _bytes)
{
    qCDebug(LOG_ESS) << "Bytes written" << _bytes << "to" << m_mpdSocket.peerName();
}


QVariantHash PlayerSource::defaultInfo()
{
    QVariantHash info;
    info["album"] = "unknown";
    info["artist"] = "unknown";
    info["duration"] = 0;
    info["progress"] = 0;
    info["title"] = "unknown";

    return info;
}


QVariantHash PlayerSource::getPlayerMpdInfo()
{
    if (m_mpdSocket.state() == QAbstractSocket::UnconnectedState) {
        // connect to host
        qCInfo(LOG_ESS) << "Connect to" << m_mpdAddress << m_mpdPort;
        m_mpdSocket.connectToHost(m_mpdAddress, static_cast<quint16>(m_mpdPort));
    } else if (m_mpdSocket.state() == QAbstractSocket::ConnectedState) {
        // send request
        if (m_mpdSocket.write(MPD_STATUS_REQUEST) == -1)
            qCWarning(LOG_ESS) << "Could not write request to" << m_mpdSocket.peerName();
    }

    return m_mpdCached;
}


QVariantHash PlayerSource::getPlayerMprisInfo(const QString &_mpris)
{
    qCDebug(LOG_ESS) << "MPRIS" << _mpris;

    auto info = defaultInfo();
    if (_mpris.isEmpty())
        return info;

    auto bus = QDBusConnection::sessionBus();
    // comes from the following request:
    // qdbus org.mpris.MediaPlayer2.vlc /org/mpris/MediaPlayer2
    // org.freedesktop.DBus.Properties.Get org.mpris.MediaPlayer2.Player
    // Metadata
    // or the same but using dbus-send:
    // dbus-send --print-reply --session --dest=org.mpris.MediaPlayer2.vlc
    // /org/mpris/MediaPlayer2 org.freedesktop.DBus.Properties.Get
    // string:'org.mpris.MediaPlayer2.Player' string:'Metadata'
    auto args = QVariantList({"org.mpris.MediaPlayer2.Player", "Metadata"});
    auto request = QDBusMessage::createMethodCall(QString("org.mpris.MediaPlayer2.%1").arg(_mpris),
                                                  "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "Get");
    request.setArguments(args);
    auto response = bus.call(request, QDBus::BlockWithGui, REQUEST_TIMEOUT);
    if ((response.type() != QDBusMessage::ReplyMessage) || (response.arguments().isEmpty())) {
        qCWarning(LOG_ESS) << "Error message" << response.errorMessage();
    } else {
        // another portion of dirty magic
        auto map = qdbus_cast<QVariantHash>(
            response.arguments().first().value<QDBusVariant>().variant().value<QDBusArgument>());
        info["album"] = map.value("xesam:album", "unknown");
        // artist is array
        info["artist"] = map.value("xesam:artist", "unknown").toString();
        info["duration"] = map.value("mpris:length", 0).toInt() / (1000 * 1000);
        info["title"] = map.value("xesam:title", "unknown");
    }

    // position
    args[1] = "Position";
    request.setArguments(args);
    response = bus.call(request, QDBus::BlockWithGui);
    if ((response.type() != QDBusMessage::ReplyMessage) || (response.arguments().isEmpty())) {
        qCWarning(LOG_ESS) << "Error message" << response.errorMessage();
    } else {
        // this cast is simpler than the previous one ;)
        info["progress"] = response.arguments().first().value<QDBusVariant>().variant().toLongLong() / (1000 * 1000);
    }

    return info;
}
