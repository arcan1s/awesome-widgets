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

#include <ksysguard/formatter/Unit.h>
#include <ksysguard/systemstats/SensorInfo.h>

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QProcess>

#include "awdebug.h"


PlayerSource::PlayerSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 5);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_player = _args.at(0);
    m_mpdAddress = _args.at(1);
    m_mpdPort = _args.at(2).toInt();
    m_mpris = _args.at(3);
    m_symbols = _args.at(4).toInt();

    connect(&m_mpdSocket, SIGNAL(connected()), this, SLOT(mpdSocketConnected()));
    connect(&m_mpdSocket, SIGNAL(readyRead()), this, SLOT(mpdSocketReadyRead()));
    connect(&m_mpdSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(mpdSocketWritten(const qint64)));
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
        run();
    QVariant value = m_values.take(_source);
    return value;
}


QString PlayerSource::getAutoMpris()
{
    QDBusMessage listServices = QDBusConnection::sessionBus().interface()->call(QDBus::BlockWithGui, "ListNames");
    if (listServices.arguments().isEmpty()) {
        qCWarning(LOG_ESS) << "Could not find any DBus service";
        return "";
    }
    QStringList arguments = listServices.arguments().first().toStringList();

    for (auto &arg : arguments) {
        if (!arg.startsWith("org.mpris.MediaPlayer2."))
            continue;
        qCInfo(LOG_ESS) << "Service found" << arg;
        QString service = arg;
        service.remove("org.mpris.MediaPlayer2.");
        return service;
    }

    return "";
}


KSysGuard::SensorInfo *PlayerSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    auto data = new KSysGuard::SensorInfo();
    if (_source == "player/album") {
        data->name = "Current song album";
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "player/salbum") {
        data->name = QString("Current song album (%1 symbols)").arg(m_symbols);
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "player/dalbum") {
        data->name = QString("Current song album (%1 symbols, dynamic)").arg(m_symbols);
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "player/artist") {
        data->name = "Current song artist";
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "player/sartist") {
        data->name = QString("Current song artist (%1 symbols)").arg(m_symbols);
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "player/dartist") {
        data->name = QString("Current song artist (%1 symbols, dynamic)").arg(m_symbols);
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "player/duration") {
        data->min = 0;
        data->max = 0;
        data->name = "Current song duration";
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitSecond;
    } else if (_source == "player/progress") {
        data->min = 0;
        data->max = 0;
        data->name = "Current song progress";
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitSecond;
    } else if (_source == "player/title") {
        data->name = "Current song title";
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "player/stitle") {
        data->name = QString("Current song title (%1 symbols)").arg(m_symbols);
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "player/dtitle") {
        data->name = QString("Current song title (%1 symbols, dynamic)").arg(m_symbols);
        data->variantType = QVariant::String;
        data->unit = KSysGuard::UnitNone;
    }

    return data;
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
    m_values["player/salbum"] = stripString(m_values["player/album"].toString(), m_symbols);
    m_values["player/sartist"] = stripString(m_values["player/artist"].toString(), m_symbols);
    m_values["player/stitle"] = stripString(m_values["player/title"].toString(), m_symbols);
    // dynamic
    m_values["player/dalbum"]
        = buildString(m_values["player/dalbum"].toString(), m_values["player/album"].toString(), m_symbols);
    m_values["player/dartist"]
        = buildString(m_values["player/dartist"].toString(), m_values["player/artist"].toString(), m_symbols);
    m_values["player/dtitle"]
        = buildString(m_values["player/dtitle"].toString(), m_values["player/title"].toString(), m_symbols);
}


QStringList PlayerSource::sources() const
{
    QStringList sources;
    sources.append("player/album");
    sources.append("player/dalbum");
    sources.append("player/salbum");
    sources.append("player/artist");
    sources.append("player/dartist");
    sources.append("player/sartist");
    sources.append("player/duration");
    sources.append("player/progress");
    sources.append("player/title");
    sources.append("player/dtitle");
    sources.append("player/stitle");

    return sources;
}


QString PlayerSource::buildString(const QString &_current, const QString &_value, const int _s)
{
    qCDebug(LOG_ESS) << "Current value" << _current << "received" << _value << "will be stripped after" << _s;

    int index = _value.indexOf(_current);
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
    QString qoutput = QString::fromUtf8(m_mpdSocket.readAll()).trimmed();
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
                m_mpdCached["player/duration"] = times.at(0).toInt();
                m_mpdCached["player/progress"] = times.at(1).toInt();
            } else if (m_metadata.contains(metadata)) {
                m_mpdCached[QString("player/%1").arg(metadata)] = data;
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
    info["player/album"] = "unknown";
    info["player/artist"] = "unknown";
    info["player/duration"] = 0;
    info["player/progress"] = 0;
    info["player/title"] = "unknown";

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
    QDBusMessage request
        = QDBusMessage::createMethodCall(QString("org.mpris.MediaPlayer2.%1").arg(_mpris), "/org/mpris/MediaPlayer2",
                                         "org.freedesktop.DBus.Properties", "Get");
    request.setArguments(args);
    auto response = bus.call(request, QDBus::BlockWithGui, REQUEST_TIMEOUT);
    if ((response.type() != QDBusMessage::ReplyMessage) || (response.arguments().isEmpty())) {
        qCWarning(LOG_ESS) << "Error message" << response.errorMessage();
    } else {
        // another portion of dirty magic
        auto map = qdbus_cast<QVariantHash>(
            response.arguments().first().value<QDBusVariant>().variant().value<QDBusArgument>());
        info["player/album"] = map.value("xesam:album", "unknown");
        // artist is array
        info["player/artist"] = map.value("xesam:artist", "unknown").toString();
        info["player/duration"] = map.value("mpris:length", 0).toInt() / (1000 * 1000);
        info["player/title"] = map.value("xesam:title", "unknown");
    }

    // position
    args[1] = "Position";
    request.setArguments(args);
    response = bus.call(request, QDBus::BlockWithGui);
    if ((response.type() != QDBusMessage::ReplyMessage) || (response.arguments().isEmpty())) {
        qCWarning(LOG_ESS) << "Error message" << response.errorMessage();
    } else {
        // this cast is simpler than the previous one ;)
        info["player/progress"]
            = response.arguments().first().value<QDBusVariant>().variant().toLongLong() / (1000 * 1000);
    }

    return info;
}
