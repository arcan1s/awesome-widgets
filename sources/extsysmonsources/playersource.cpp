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
#include <QTextCodec>

#include "awdebug.h"


PlayerSource::PlayerSource(QObject *parent, const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 5);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_player = args.at(0);
    m_mpdAddress = args.at(1);
    m_mpdPort = args.at(2).toInt();
    m_mpris = args.at(3);
    m_symbols = args.at(4).toInt();

    connect(&m_mpdSocket, SIGNAL(connected()), this,
            SLOT(mpdSocketConnected()));
    connect(&m_mpdSocket, SIGNAL(readyRead()), this,
            SLOT(mpdSocketReadyRead()));
    connect(&m_mpdSocket, SIGNAL(bytesWritten(qint64)), this,
            SLOT(mpdSocketWritten(const qint64)));
    m_mpdCached = defaultInfo();
}


PlayerSource::~PlayerSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_mpdSocket.close();
}


QVariant PlayerSource::data(QString source)
{
    qCDebug(LOG_ESS) << "Source" << source;

    if (!m_values.contains(source))
        run();
    QVariant value = m_values.take(source);
    return value;
}


QString PlayerSource::getAutoMpris() const
{
    QDBusMessage listServices = QDBusConnection::sessionBus().interface()->call(
        QDBus::BlockWithGui, QString("ListNames"));
    if (listServices.arguments().isEmpty())
        return QString();
    QStringList arguments = listServices.arguments().first().toStringList();

    for (auto arg : arguments) {
        if (!arg.startsWith(QString("org.mpris.MediaPlayer2.")))
            continue;
        qCInfo(LOG_ESS) << "Service found" << arg;
        QString service = arg;
        service.remove(QString("org.mpris.MediaPlayer2."));
        return service;
    }

    return QString();
}


QVariantMap PlayerSource::initialData(QString source) const
{
    qCDebug(LOG_ESS) << "Source" << source;

    QVariantMap data;
    if (source == QString("player/album")) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")] = QString("Current song album");
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    } else if (source == QString("player/salbum")) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")]
            = QString("Current song album (%1 symbols)").arg(m_symbols);
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    } else if (source == QString("player/dalbum")) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")]
            = QString("Current song album (%1 symbols, dynamic)")
                  .arg(m_symbols);
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    } else if (source == QString("player/artist")) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")] = QString("Current song artist");
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    } else if (source == QString("player/sartist")) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")]
            = QString("Current song artist (%1 symbols)").arg(m_symbols);
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    } else if (source == QString("player/dartist")) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")]
            = QString("Current song artist (%1 symbols, dynamic)")
                  .arg(m_symbols);
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    } else if (source == QString("player/duration")) {
        data[QString("min")] = 0;
        data[QString("max")] = 0;
        data[QString("name")] = QString("Current song duration");
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("s");
    } else if (source == QString("player/progress")) {
        data[QString("min")] = 0;
        data[QString("max")] = 0;
        data[QString("name")] = QString("Current song progress");
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("s");
    } else if (source == QString("player/title")) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")] = QString("Current song title");
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    } else if (source == QString("player/stitle")) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")]
            = QString("Current song title (%1 symbols)").arg(m_symbols);
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    } else if (source == QString("player/dtitle")) {
        data[QString("min")] = QString("");
        data[QString("max")] = QString("");
        data[QString("name")]
            = QString("Current song title (%1 symbols, dynamic)")
                  .arg(m_symbols);
        data[QString("type")] = QString("QString");
        data[QString("units")] = QString("");
    }

    return data;
}


void PlayerSource::run()
{
    // initial data
    if (m_player == QString("mpd")) {
        // mpd
        m_values = getPlayerMpdInfo();
    } else if (m_player == QString("mpris")) {
        // players which supports mpris
        if (m_dbusMutex.tryLock()) {
            QString mpris
                = m_mpris == QString("auto") ? getAutoMpris() : m_mpris;
            m_values = getPlayerMprisInfo(mpris);
            m_dbusMutex.unlock();
        }
    }

    // dymanic properties
    // solid
    m_values[QString("player/salbum")]
        = stripString(m_values[QString("player/album")].toString(), m_symbols);
    m_values[QString("player/sartist")]
        = stripString(m_values[QString("player/artist")].toString(), m_symbols);
    m_values[QString("player/stitle")]
        = stripString(m_values[QString("player/title")].toString(), m_symbols);
    // dynamic
    m_values[QString("player/dalbum")]
        = buildString(m_values[QString("player/dalbum")].toString(),
                      m_values[QString("player/album")].toString(), m_symbols);
    m_values[QString("player/dartist")]
        = buildString(m_values[QString("player/dartist")].toString(),
                      m_values[QString("player/artist")].toString(), m_symbols);
    m_values[QString("player/dtitle")]
        = buildString(m_values[QString("player/dtitle")].toString(),
                      m_values[QString("player/title")].toString(), m_symbols);
}


QStringList PlayerSource::sources() const
{
    QStringList sources;
    sources.append(QString("player/album"));
    sources.append(QString("player/dalbum"));
    sources.append(QString("player/salbum"));
    sources.append(QString("player/artist"));
    sources.append(QString("player/dartist"));
    sources.append(QString("player/sartist"));
    sources.append(QString("player/duration"));
    sources.append(QString("player/progress"));
    sources.append(QString("player/title"));
    sources.append(QString("player/dtitle"));
    sources.append(QString("player/stitle"));

    return sources;
}


QString PlayerSource::buildString(const QString &current, const QString &value,
                                  const int s)
{
    qCDebug(LOG_ESS) << "Current value" << current << "received" << value
                     << "will be stripped after" << s;

    int index = value.indexOf(current);
    if ((current.isEmpty()) || ((index + s + 1) > value.count()))
        return QString("%1").arg(value.left(s), s, QLatin1Char(' '));
    else
        return QString("%1").arg(value.mid(index + 1, s), s, QLatin1Char(' '));
}


QString PlayerSource::stripString(const QString &value, const int s)
{
    qCDebug(LOG_ESS) << "New value" << value << "will be stripped after" << s;

    return value.count() > s ? QString("%1\u2026").arg(value.left(s - 1))
                             : value.leftJustified(s, QLatin1Char(' '));
}


void PlayerSource::mpdSocketConnected()
{
    qCDebug(LOG_ESS) << "MPD socket connected to" << m_mpdSocket.peerName()
                     << "with state" << m_mpdSocket.state();
}


void PlayerSource::mpdSocketReadyRead()
{
    QString qoutput = QTextCodec::codecForMib(106)
                          ->toUnicode(m_mpdSocket.readAll())
                          .trimmed();
    qCInfo(LOG_ESS) << "Output" << qoutput;

    // parse
    for (auto str : qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
        if (str.split(QString(": "), QString::SkipEmptyParts).count() == 2) {
            // "Metadata: data"
            QString metadata = str.split(QString(": "), QString::SkipEmptyParts)
                                   .first()
                                   .toLower();
            QString data = str.split(QString(": "), QString::SkipEmptyParts)
                               .last()
                               .trimmed();
            // there are one more time...
            if ((metadata == QString("time")) && (data.contains(QChar(':')))) {
                QStringList times = data.split(QString(":"));
                m_mpdCached[QString("player/duration")] = times.at(0).toInt();
                m_mpdCached[QString("player/progress")] = times.at(1).toInt();
            } else if (m_metadata.contains(metadata)) {
                m_mpdCached[QString("player/%1").arg(metadata)] = data;
            }
        }
    }

    emit(dataReceived(m_mpdCached));
}


void PlayerSource::mpdSocketWritten(const qint64 bytes)
{
    qCDebug(LOG_ESS) << "Bytes written" << bytes << "to"
                     << m_mpdSocket.peerName();
}


QVariantHash PlayerSource::defaultInfo() const
{
    QVariantHash info;
    info[QString("player/album")] = QString("unknown");
    info[QString("player/artist")] = QString("unknown");
    info[QString("player/duration")] = 0;
    info[QString("player/progress")] = 0;
    info[QString("player/title")] = QString("unknown");

    return info;
}


QVariantHash PlayerSource::getPlayerMpdInfo()
{
    if (m_mpdSocket.state() == QAbstractSocket::UnconnectedState) {
        // connect to host
        qCInfo(LOG_ESS) << "Connect to" << m_mpdAddress << m_mpdPort;
        m_mpdSocket.connectToHost(m_mpdAddress, m_mpdPort);
    } else if (m_mpdSocket.state() == QAbstractSocket::ConnectedState) {
        // send request
        if (m_mpdSocket.write(MPD_STATUS_REQUEST) == -1)
            qCWarning(LOG_ESS) << "Could not write request to"
                               << m_mpdSocket.peerName();
    }

    return m_mpdCached;
}


QVariantHash PlayerSource::getPlayerMprisInfo(const QString mpris) const
{
    qCDebug(LOG_ESS) << "MPRIS" << mpris;

    QVariantHash info = defaultInfo();
    if (mpris.isEmpty())
        return info;

    QDBusConnection bus = QDBusConnection::sessionBus();
    // comes from the following request:
    // qdbus org.mpris.MediaPlayer2.vlc /org/mpris/MediaPlayer2
    // org.freedesktop.DBus.Properties.Get org.mpris.MediaPlayer2.Player
    // Metadata
    // or the same but using dbus-send:
    // dbus-send --print-reply --session --dest=org.mpris.MediaPlayer2.vlc
    // /org/mpris/MediaPlayer2 org.freedesktop.DBus.Properties.Get
    // string:'org.mpris.MediaPlayer2.Player' string:'Metadata'
    QVariantList args = QVariantList()
                        << QString("org.mpris.MediaPlayer2.Player")
                        << QString("Metadata");
    QDBusMessage request = QDBusMessage::createMethodCall(
        QString("org.mpris.MediaPlayer2.%1").arg(mpris),
        QString("/org/mpris/MediaPlayer2"), QString(""), QString("Get"));
    request.setArguments(args);
    QDBusMessage response
        = bus.call(request, QDBus::BlockWithGui, REQUEST_TIMEOUT);
    if ((response.type() != QDBusMessage::ReplyMessage)
        || (response.arguments().isEmpty())) {
        qCWarning(LOG_ESS) << "Error message" << response.errorMessage();
    } else {
        // another portion of dirty magic
        QVariantHash map
            = qdbus_cast<QVariantHash>(response.arguments()
                                           .first()
                                           .value<QDBusVariant>()
                                           .variant()
                                           .value<QDBusArgument>());
        info[QString("player/album")]
            = map.value(QString("xesam:album"), QString("unknown"));
        // artist is array
        info[QString("player/artist")]
            = map.value(QString("xesam:artist"), QString("unknown")).toString();
        info[QString("player/duration")]
            = map.value(QString("mpris:length"), 0).toInt() / (1000 * 1000);
        info[QString("player/title")]
            = map.value(QString("xesam:title"), QString("unknown"));
    }

    // position
    args[1] = QString("Position");
    request.setArguments(args);
    response = bus.call(request, QDBus::BlockWithGui);
    if ((response.type() != QDBusMessage::ReplyMessage)
        || (response.arguments().isEmpty())) {
        qCWarning(LOG_ESS) << "Error message" << response.errorMessage();
    } else {
        // this cast is simpler than the previous one ;)
        info[QString("player/progress")] = response.arguments()
                                               .first()
                                               .value<QDBusVariant>()
                                               .variant()
                                               .toLongLong()
                                           / (1000 * 1000);
    }

    return info;
}
