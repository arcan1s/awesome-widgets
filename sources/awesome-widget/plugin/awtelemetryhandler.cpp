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
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QStandardPaths>
#include <QUuid>

#include "awdebug.h"


AWTelemetryHandler::AWTelemetryHandler(QObject *parent, const QString clientId)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_localFile = QString("%1/awesomewidgets/telemetry.ini")
                      .arg(QStandardPaths::writableLocation(
                          QStandardPaths::GenericDataLocation));

    // override client id if any
    if (!clientId.isEmpty())
        m_clientId = clientId;
}


AWTelemetryHandler::~AWTelemetryHandler()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QStringList AWTelemetryHandler::get(const QString group) const
{
    qCDebug(LOG_AW) << "Get stored data for group" << group;

    QStringList values;

    QSettings settings(m_localFile, QSettings::IniFormat);
    settings.beginGroup(group);
    for (auto key : settings.childKeys())
        values.append(settings.value(key).toString());
    settings.endGroup();

    return values;
}


QString AWTelemetryHandler::getLast(const QString group) const
{
    qCDebug(LOG_AW) << "Get last stored data for group" << group;

    return get(group).last();
}


void AWTelemetryHandler::init(const int count, const bool enableRemote,
                              const QString clientId)
{
    qCDebug(LOG_AW) << "Init telemetry with count" << count << "enable remote"
                    << enableRemote << "client ID" << clientId;

    m_storeCount = count;
    m_uploadEnabled = enableRemote;
    m_clientId = clientId;
}


bool AWTelemetryHandler::put(const QString group, const QString value) const
{
    qCDebug(LOG_AW) << "Store data with group" << group << "and value" << value;

    QSettings settings(m_localFile, QSettings::IniFormat);
    settings.beginGroup(group);
    // values will be stored as num=value inside specified group
    // load all values to memory
    QStringList saved;
    for (auto key : settings.childKeys())
        saved.append(settings.value(key).toString());
    // check if this value is already saved
    if (saved.contains(value)) {
        qCInfo(LOG_AW) << "Configuration" << value << "for group" << group
                       << "is already saved";
        return false;
    }
    saved.append(value);
    // remove old ones
    while (saved.count() > m_storeCount)
        saved.takeFirst();
    // clear group
    settings.remove(QString(""));
    // and save now
    for (auto value : saved) {
        QString key = getKey(settings.childKeys().count());
        settings.setValue(key, value);
    }

    // sync settings
    settings.endGroup();
    settings.sync();
    // return status
    return (settings.status() == QSettings::NoError);
}


void AWTelemetryHandler::uploadTelemetry(const QString group,
                                         const QString value)
{
    qCDebug(LOG_AW) << "Upload data with group" << group << "and value"
                    << value;
    if (!m_uploadEnabled) {
        qCInfo(LOG_AW) << "Upload disabled by configuration";
        return;
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager(nullptr);
    connect(manager, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(telemetryReplyRecieved(QNetworkReply *)));

    QUrl url(REMOTE_TELEMETRY_URL);
    url.setPort(REMOTE_TELEMETRY_PORT);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // generate payload
    QVariantMap payload;
    payload[QString("api")] = AWTEAPI;
    payload[QString("client_id")] = m_clientId;
    payload[QString("metadata")] = value;
    payload[QString("type")] = group;
    // convert to QByteArray to send request
    QByteArray data
        = QJsonDocument::fromVariant(payload).toJson(QJsonDocument::Compact);
    qCInfo(LOG_AW) << "Send request with body" << data.data() << "and size"
                   << data.size();

    manager->post(request, data);
}


void AWTelemetryHandler::telemetryReplyRecieved(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qCWarning(LOG_AW) << "An error occurs" << reply->error()
                          << "with message" << reply->errorString();
        return;
    }

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_AW) << "Parse error" << error.errorString();
        return;
    }
    reply->deleteLater();

    // convert to map
    QVariantMap response = jsonDoc.toVariant().toMap();
    QString message = response[QString("message")].toString();
    qCInfo(LOG_AW) << "Server reply on telemetry" << message;

    return emit(replyReceived(message));
}


QString AWTelemetryHandler::getKey(const int count) const
{
    qCDebug(LOG_AW) << "Get key for keys count" << count;

    return QString("%1").arg(count, 3, 10, QChar('0'));
}
