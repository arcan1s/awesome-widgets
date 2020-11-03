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
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "awdebug.h"


AWBugReporter::AWBugReporter(QObject *_parent)
    : QObject(_parent)
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
    connect(this, SIGNAL(replyReceived(const int, const QString &)), this,
            SLOT(showInformation(int, const QString &)));
}


QString AWBugReporter::generateText(const QString &_description, const QString &_reproduce,
                                    const QString &_expected, const QString &_logs)
{
    // do not log _logs here, it may have quite large size
    qCDebug(LOG_AW) << "Generate text with description" << _description << "steps" << _reproduce
                    << "and expected result" << _expected;

    QString output;
    output += QString("**Description**\n\n%1\n\n").arg(_description);
    output += QString("**Step to _reproduce**\n\n%1\n\n").arg(_reproduce);
    output += QString("**Expected result**\n\n%1\n\n").arg(_expected);
    output += QString("**Version**\n\n%1\n\n").arg(AWDebug::getBuildData().join(QString("\n")));
    // append _logs
    output += QString("**Logs**\n\n%1").arg(_logs);

    return output;
}


void AWBugReporter::sendBugReport(const QString &_title, const QString &_body)
{
    qCDebug(LOG_AW) << "Send bug report with title" << _title << "and body" << _body;

    auto *manager = new QNetworkAccessManager(nullptr);
    connect(manager, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(issueReplyRecieved(QNetworkReply *)));

    QNetworkRequest request = QNetworkRequest(QUrl(BUGTRACKER_API));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // generate payload
    QVariantMap payload;
    payload["title"] = _title;
    payload["body"] = _body;
    payload["labels"] = QStringList() << "from application";
    // convert to QByteArray to send request
    QByteArray data = QJsonDocument::fromVariant(payload).toJson(QJsonDocument::Compact);
    qCInfo(LOG_AW) << "Send request with _body" << data.data() << "and size" << data.size();

    manager->post(request, data);
}


void AWBugReporter::issueReplyRecieved(QNetworkReply *_reply)
{
    if (_reply->error() != QNetworkReply::NoError) {
        qCWarning(LOG_AW) << "An error occurs" << _reply->error() << "with message"
                          << _reply->errorString();
        return emit(replyReceived(0, ""));
    }

    QJsonParseError error{};
    QJsonDocument jsonDoc = QJsonDocument::fromJson(_reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_AW) << "Parse error" << error.errorString();
        return emit(replyReceived(0, ""));
    }
    _reply->deleteLater();

    // convert to map
    QVariantMap response = jsonDoc.toVariant().toMap();
    QString url = response["html_url"].toString();
    int number = response["number"].toInt();

    return emit(replyReceived(number, url));
}


void AWBugReporter::showInformation(const int _number, const QString &_url)
{
    qCDebug(LOG_AW) << "Created issue with number" << _number << "and url" << _url;

    // cache url first
    m_lastBugUrl = _url;

    auto *msgBox = new QMessageBox(nullptr);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setModal(false);
    msgBox->setWindowTitle(i18n("Issue created"));
    msgBox->setText(i18n("Issue %1 has been created", _number));
    msgBox->setStandardButtons(QMessageBox::Open | QMessageBox::Close);
    msgBox->setIcon(QMessageBox::Information);

    msgBox->open(this, SLOT(userReplyOnBugReport(QAbstractButton *)));
}


void AWBugReporter::userReplyOnBugReport(QAbstractButton *_button)
{
    QMessageBox::ButtonRole ret = dynamic_cast<QMessageBox *>(sender())->buttonRole(_button);
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
