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
#include <KNotifications/KNotification>

#include <QDesktopServices>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QStandardPaths>

#include "awdebug.h"


AWUpdateHelper::AWUpdateHelper(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_foundVersion = QVersionNumber::fromString(VERSION);
    m_genericConfig = QString("%1/awesomewidgets/general.ini")
                          .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
}


void AWUpdateHelper::checkUpdates(const bool _showAnyway)
{
    qCDebug(LOG_AW) << "Show anyway" << _showAnyway;

    // showAnyway options required to show message if no updates found on direct
    // request. In case of automatic check no message will be shown
    auto manager = new QNetworkAccessManager(nullptr);
    connect(manager, &QNetworkAccessManager::finished,
            [_showAnyway, this](QNetworkReply *reply) { return versionReplyReceived(reply, _showAnyway); });

    manager->get(QNetworkRequest(QUrl(VERSION_API)));
}


bool AWUpdateHelper::checkVersion()
{
    QSettings settings(m_genericConfig, QSettings::IniFormat);
    auto version = QVersionNumber::fromString(settings.value("Version", QString(VERSION)).toString());
    // update version
    settings.setValue("Version", QString(VERSION));
    settings.sync();
    qCInfo(LOG_AW) << "Found version" << version << "actual one is" << m_foundVersion;

    if ((version != m_foundVersion) && (!QString(CHANGELOG).isEmpty())) {
        sendNotification(i18n("Changelog of %1", VERSION), QString(CHANGELOG).replace('@', '\n'));
        return true;
    } else if (version != m_foundVersion) {
        qCWarning(LOG_AW) << "No changelog information provided";
        return true;
    } else {
        qCInfo(LOG_AW) << "No need to update version";
    }

    return false;
}


void AWUpdateHelper::openReleasesPage()
{
    QDesktopServices::openUrl(QString(RELEASES) + m_foundVersion.toString());
}


void AWUpdateHelper::showInfo(const QVersionNumber &_version)
{
    qCDebug(LOG_AW) << "Version" << _version;

    auto text = i18n("You are using the actual version %1", _version.toString());
    if (!QString(COMMIT_SHA).isEmpty())
        text += QString(" (%1)").arg(QString(COMMIT_SHA));
    sendNotification(i18n("No new version found"), text);
}


void AWUpdateHelper::showUpdates(const QVersionNumber &_version)
{
    qCDebug(LOG_AW) << "Version" << _version;

    QString text;
    text += i18n("Current version : %1", VERSION);
    text += QString(COMMIT_SHA).isEmpty() ? "\n" : QString(" (%1)\n").arg(QString(COMMIT_SHA));
    text += i18n("New version : %1", _version.toString()) + "\n\n";
    text += i18n("Click \"Ok\" to download");

    auto event = sendNotification(i18n("There are updates"), text);
    auto action = event->addAction(i18n("Details"));
    connect(action, &KNotificationAction::activated, this, &AWUpdateHelper::openReleasesPage);
}


void AWUpdateHelper::versionReplyReceived(QNetworkReply *_reply, const bool _showAnyway)
{
    qCDebug(LOG_AW) << "Show message anyway" << _showAnyway;
    if (_reply->error() != QNetworkReply::NoError) {
        qCWarning(LOG_AW) << "An error occurs" << _reply->error() << "with message" << _reply->errorString();
        return;
    }

    auto error = QJsonParseError();
    auto jsonDoc = QJsonDocument::fromJson(_reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_AW) << "Parse error" << error.errorString();
        return;
    }
    _reply->deleteLater();
    _reply->manager()->deleteLater(); // remember to delete manager too

    // convert to map
    auto firstRelease = jsonDoc.toVariant().toList().first().toMap();
    auto version = firstRelease["tag_name"].toString();
    version.remove("V.");
    m_foundVersion = QVersionNumber::fromString(version);
    qCInfo(LOG_AW) << "Update found version to" << m_foundVersion;

    auto oldVersion = QVersionNumber::fromString(VERSION);
    if (oldVersion < m_foundVersion)
        return showUpdates(m_foundVersion);
    else if (_showAnyway)
        return showInfo(m_foundVersion);
}


// additional method which is used to show message box which does not block UI
KNotification *AWUpdateHelper::sendNotification(const QString &_title, const QString &_body)
{
    qCDebug(LOG_AW) << "Construct message box with title" << _title << "and body" << _body;

    auto event = KNotification::event("system", _title, _body);
    event->setComponentName("plasma-applet-org.kde.plasma.awesome-widget");

    return event;
}
