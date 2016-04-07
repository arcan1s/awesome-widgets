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

#include "awupdatehelper.h"

#include <KI18n/KLocalizedString>

#include <QtConcurrent/QtConcurrent>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>

#include "awdebug.h"
#include "version.h"


AWUpdateHelper::AWUpdateHelper(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_foundVersion = QVersionNumber::fromString(VERSION);
    m_genericConfig = QString("%1/awesomewidgets/general.ini")
                          .arg(QStandardPaths::writableLocation(
                              QStandardPaths::GenericDataLocation));
}


AWUpdateHelper::~AWUpdateHelper()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


void AWUpdateHelper::checkUpdates(const bool showAnyway)
{
    qCDebug(LOG_AW) << "Show anyway" << showAnyway;

    // showAnyway options requires to show message if no updates found on direct
    // request. In case of automatic check no message will be shown
    QNetworkAccessManager *manager = new QNetworkAccessManager(nullptr);
    connect(manager, &QNetworkAccessManager::finished,
            [showAnyway, this](QNetworkReply *reply) {
                return versionReplyRecieved(reply, showAnyway);
            });

    manager->get(QNetworkRequest(QUrl(VERSION_API)));
}


bool AWUpdateHelper::checkVersion()
{
    QSettings settings(m_genericConfig, QSettings::IniFormat);
    QVersionNumber version = QVersionNumber::fromString(
        settings.value(QString("Version"), QString(VERSION)).toString());
    // update version
    settings.setValue(QString("Version"), QString(VERSION));
    settings.sync();

    qCInfo(LOG_AW) << "Found version" << version << "actual one is" << VERSION;
    if (version != QVersionNumber::fromString(VERSION)) {
        genMessageBox(i18n("Changelog of %1", QString(VERSION)),
                      QString(CHANGELOG).replace(QChar('@'), QChar('\n')),
                      QMessageBox::Ok)
            ->open();
        return true;
    } else {
        qCInfo(LOG_AW) << "No need to update version";
    }

    return false;
}


void AWUpdateHelper::showInfo(const QVersionNumber version)
{
    qCDebug(LOG_AW) << "Version" << version;

    QString text
        = i18n("You are using the actual version %1", version.toString());
    if (!QString(COMMIT_SHA).isEmpty())
        text += QString(" (%1)").arg(QString(COMMIT_SHA));
    return genMessageBox(i18n("No new version found"), text, QMessageBox::Ok)
        ->open();
}


void AWUpdateHelper::showUpdates(const QVersionNumber version)
{
    qCDebug(LOG_AW) << "Version" << version;

    QString text;
    text += i18n("Current version : %1", QString(VERSION));
    text += QString(COMMIT_SHA).isEmpty()
                ? QString("\n")
                : QString(" (%1)\n").arg(QString(COMMIT_SHA));
    text += i18n("New version : %1", version.toString()) + QString("\n\n");
    text += i18n("Click \"Ok\" to download");

    genMessageBox(i18n("There are updates"), text,
                  QMessageBox::Ok | QMessageBox::Cancel)
        ->open(this, SLOT(userReplyOnUpdates(QAbstractButton *)));
}


void AWUpdateHelper::userReplyOnUpdates(QAbstractButton *button)
{
    QMessageBox::ButtonRole ret = static_cast<QMessageBox *>(sender())->buttonRole(button);
    qCInfo(LOG_AW) << "User select" << ret;

    switch (ret) {
    case QMessageBox::AcceptRole:
        QDesktopServices::openUrl(QString(RELEASES)
                                  + m_foundVersion.toString());
        break;
    case QMessageBox::RejectRole:
    default:
        break;
    }
}


void AWUpdateHelper::versionReplyRecieved(QNetworkReply *reply,
                                          const bool showAnyway)
{
    qCDebug(LOG_AW) << "Return code" << reply->error() << "with message"
                    << reply->errorString() << "and show anyway" << showAnyway;

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    if ((reply->error() != QNetworkReply::NoError)
        || (error.error != QJsonParseError::NoError)) {
        qCWarning(LOG_AW) << "Parse error" << error.errorString();
        return;
    }
    reply->deleteLater();

    // convert to map
    QVariantMap firstRelease = jsonDoc.toVariant().toList().first().toMap();
    QString version = firstRelease[QString("tag_name")].toString();
    version.remove(QString("V."));
    m_foundVersion = QVersionNumber::fromString(version);
    qCInfo(LOG_AW) << "Update found version to" << m_foundVersion;

    QVersionNumber oldVersion = QVersionNumber::fromString(VERSION);
    if (oldVersion < m_foundVersion)
        return showUpdates(m_foundVersion);
    else if (showAnyway)
        return showInfo(m_foundVersion);
}


// additional method which is used to show message box which does not block UI
QMessageBox *
AWUpdateHelper::genMessageBox(const QString title, const QString body,
                              const QMessageBox::StandardButtons buttons)
{
    qCDebug(LOG_AW) << "Construct message box with title" << title << "and body"
                    << body;

    QMessageBox *msgBox = new QMessageBox(nullptr);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setModal(false);
    msgBox->setWindowTitle(title);
    msgBox->setText(body);
    msgBox->setStandardButtons(buttons);
    msgBox->setIcon(QMessageBox::Information);

    return msgBox;
}
