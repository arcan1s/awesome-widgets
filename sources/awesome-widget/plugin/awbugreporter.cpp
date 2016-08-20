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

#include "awbugreporter.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "awdebug.h"


AWBugReporter::AWBugReporter(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


AWBugReporter::~AWBugReporter()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


void AWBugReporter::sendBugReport(const QString title, const QString body)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(nullptr);
    connect(manager, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(issueReplyRecieved(QNetworkReply *)));

    QNetworkRequest request(QUrl(BUGTRACKER_API));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // generate payload
    QVariantMap payload;
    payload[QString("title")] = title;
    payload[QString("body")] = body;
    payload[QString("labels")] = QStringList() << QString("from application");
    // convert to QByteArray to send request
    QByteArray data
        = QJsonDocument::fromVariant(payload).toJson(QJsonDocument::Compact);
    qCInfo(LOG_AW) << "Send request with body" << data.data() << "and size"
                   << data.size();

    manager->post(request, data);
}


void AWBugReporter::issueReplyRecieved(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qCWarning(LOG_AW) << "An error occurs" << reply->error()
                          << "with message" << reply->errorString();
        return emit(replyReceived(false, QString()));
    }

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_AW) << "Parse error" << error.errorString();
        return emit(replyReceived(false, QString()));
    }
    reply->deleteLater();

    // convert to map
    QVariantMap response = jsonDoc.toVariant().toMap();
    QString url = response[QString("html_url")].toString();

    return emit(replyReceived(true, url));
}
