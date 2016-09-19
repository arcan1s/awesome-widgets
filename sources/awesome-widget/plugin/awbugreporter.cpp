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

#include <KI18n/KLocalizedString>

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QMessageBox>
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


void AWBugReporter::doConnect()
{
    // additional method for testing needs
    connect(this, SIGNAL(replyReceived(const int, const QString)), this,
            SLOT(showInformation(const int, const QString)));
}


QString AWBugReporter::generateText(const QString description,
                                    const QString reproduce,
                                    const QString expected,
                                    const QString logs) const
{
    // do not log logs here, it may have quite large size
    qCDebug(LOG_AW) << "Generate text with description" << description
                    << "steps" << reproduce << "and expected result"
                    << expected;

    QString output;
    output += QString("**Description**\n\n%1\n\n").arg(description);
    output += QString("**Step to reproduce**\n\n%1\n\n").arg(reproduce);
    output += QString("**Expected result**\n\n%1\n\n").arg(expected);
    output += QString("**Version**\n\n%1\n\n")
                  .arg(getBuildData().join(QString("\n")));
    // append logs
    output += QString("**Logs**\n\n%1").arg(logs);

    return output;
}


void AWBugReporter::sendBugReport(const QString title, const QString body)
{
    qCDebug(LOG_AW) << "Send bug report with title" << title << "and body"
                    << body;

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
        return emit(replyReceived(0, QString()));
    }

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_AW) << "Parse error" << error.errorString();
        return emit(replyReceived(0, QString()));
    }
    reply->deleteLater();

    // convert to map
    QVariantMap response = jsonDoc.toVariant().toMap();
    QString url = response[QString("html_url")].toString();
    int number = response[QString("number")].toInt();

    return emit(replyReceived(number, url));
}


void AWBugReporter::showInformation(const int number, const QString url)
{
    qCDebug(LOG_AW) << "Created issue with number" << number << "and url"
                    << url;

    // cache url first
    m_lastBugUrl = url;

    QMessageBox *msgBox = new QMessageBox(nullptr);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setModal(false);
    msgBox->setWindowTitle(i18n("Issue created"));
    msgBox->setText(i18n("Issue %1 has been created", number));
    msgBox->setStandardButtons(QMessageBox::Open | QMessageBox::Close);
    msgBox->setIcon(QMessageBox::Information);

    msgBox->open(this, SLOT(userReplyOnBugReport(QAbstractButton *)));
}


void AWBugReporter::userReplyOnBugReport(QAbstractButton *button)
{
    QMessageBox::ButtonRole ret
        = static_cast<QMessageBox *>(sender())->buttonRole(button);
    qCInfo(LOG_AW) << "User select" << ret;

    switch (ret) {
    case QMessageBox::AcceptRole:
        QDesktopServices::openUrl(m_lastBugUrl);
        break;
    case QMessageBox::RejectRole:
    default:
        break;
    }
}
