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

#include "awtelemetryhandler.h"

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QStandardPaths>
#include <QUuid>

#include "awdebug.h"


AWTelemetryHandler::AWTelemetryHandler(QObject *_parent, const QString &_clientId)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_localFile = QString("%1/awesomewidgets/telemetry.ini")
                      .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));

    // override client id if any
    if (!_clientId.isEmpty())
        m_clientId = _clientId;
}


AWTelemetryHandler::~AWTelemetryHandler()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QStringList AWTelemetryHandler::get(const QString &_group) const
{
    qCDebug(LOG_AW) << "Get stored data for group" << _group;

    QStringList values;

    QSettings settings(m_localFile, QSettings::IniFormat);
    settings.beginGroup(_group);
    for (auto &key : settings.childKeys())
        values.append(settings.value(key).toString());
    settings.endGroup();

    return values;
}


QString AWTelemetryHandler::getLast(const QString &_group) const
{
    qCDebug(LOG_AW) << "Get last stored data for group" << _group;

    return get(_group).last();
}


void AWTelemetryHandler::init(const int _count, const bool _enableRemote, const QString &_clientId)
{
    qCDebug(LOG_AW) << "Init telemetry with count" << _count << "enable remote" << _enableRemote
                    << "client ID" << _clientId;

    m_storeCount = _count;
    m_uploadEnabled = _enableRemote;
    m_clientId = _clientId;
}


bool AWTelemetryHandler::put(const QString &_group, const QString &_value) const
{
    qCDebug(LOG_AW) << "Store data with group" << _group << "and value" << _value;

    QSettings settings(m_localFile, QSettings::IniFormat);
    settings.beginGroup(_group);
    // values will be stored as num=value inside specified group
    // load all values to memory
    QStringList saved;
    for (auto &key : settings.childKeys())
        saved.append(settings.value(key).toString());
    // check if this value is already saved
    if (saved.contains(_value)) {
        qCInfo(LOG_AW) << "Configuration" << _value << "for group" << _group << "is already saved";
        return false;
    }
    saved.append(_value);
    // remove old ones
    while (saved.count() > m_storeCount)
        saved.takeFirst();
    // clear group
    settings.remove("");
    // and save now
    for (auto &val : saved) {
        QString key = getKey(settings.childKeys().count());
        settings.setValue(key, val);
    }

    // sync settings
    settings.endGroup();
    settings.sync();
    // return status
    return (settings.status() == QSettings::NoError);
}


void AWTelemetryHandler::uploadTelemetry(const QString &_group, const QString &_value)
{
    qCDebug(LOG_AW) << "Upload data with group" << _group << "and value" << _value;
    if (!m_uploadEnabled) {
        qCInfo(LOG_AW) << "Upload disabled by configuration";
        return;
    }

    auto *manager = new QNetworkAccessManager(nullptr);
    connect(manager, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(telemetryReplyRecieved(QNetworkReply *)));

    QUrl url(REMOTE_TELEMETRY_URL);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // generate payload
    QVariantMap payload;
    payload["api"] = AW_TELEMETRY_API;
    payload["client_id"] = m_clientId;
    payload["metadata"] = _value;
    payload["type"] = _group;
    // convert to QByteArray to send request
    QByteArray data = QJsonDocument::fromVariant(payload).toJson(QJsonDocument::Compact);
    qCInfo(LOG_AW) << "Send request with body" << data.data() << "and size" << data.size();

    manager->post(request, data);
}


void AWTelemetryHandler::telemetryReplyRecieved(QNetworkReply *_reply)
{
    if (_reply->error() != QNetworkReply::NoError) {
        qCWarning(LOG_AW) << "An error occurs" << _reply->error() << "with message"
                          << _reply->errorString();
        return;
    }

    QJsonParseError error{};
    QJsonDocument jsonDoc = QJsonDocument::fromJson(_reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_AW) << "Parse error" << error.errorString();
        return;
    }
    _reply->deleteLater();

    // convert to map
    QVariantMap response = jsonDoc.toVariant().toMap();
    QString message = response["message"].toString();
    qCInfo(LOG_AW) << "Server reply on telemetry" << message;

    return emit(replyReceived(message));
}


QString AWTelemetryHandler::getKey(const int _count)
{
    qCDebug(LOG_AW) << "Get key for keys count" << _count;

    return QString("%1").arg(_count, 3, 10, QChar('0'));
}
