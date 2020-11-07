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

#include <QDesktopServices>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>

#include "awdebug.h"


AWUpdateHelper::AWUpdateHelper(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_foundVersion = QVersionNumber::fromString(VERSION);
    m_genericConfig
        = QString("%1/awesomewidgets/general.ini")
              .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
}


AWUpdateHelper::~AWUpdateHelper()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


void AWUpdateHelper::checkUpdates(const bool _showAnyway)
{
    qCDebug(LOG_AW) << "Show anyway" << _showAnyway;

    // showAnyway options requires to show message if no updates found on direct
    // request. In case of automatic check no message will be shown
    auto *manager = new QNetworkAccessManager(nullptr);
    connect(manager, &QNetworkAccessManager::finished, [_showAnyway, this](QNetworkReply *reply) {
        return versionReplyRecieved(reply, _showAnyway);
    });

    manager->get(QNetworkRequest(QUrl(VERSION_API)));
}


bool AWUpdateHelper::checkVersion()
{
    QSettings settings(m_genericConfig, QSettings::IniFormat);
    QVersionNumber version
        = QVersionNumber::fromString(settings.value("Version", QString(VERSION)).toString());
    // update version
    settings.setValue("Version", QString(VERSION));
    settings.sync();
    qCInfo(LOG_AW) << "Found version" << version << "actual one is" << m_foundVersion;

    if ((version != m_foundVersion) && (!QString(CHANGELOG).isEmpty())) {
        genMessageBox(i18nc("Changelog of %1", VERSION), QString(CHANGELOG).replace('@', '\n'),
                      QMessageBox::Ok)
            ->open();
        return true;
    } else if (version != m_foundVersion) {
        qCWarning(LOG_AW) << "No changelog information provided";
        return true;
    } else {
        qCInfo(LOG_AW) << "No need to update version";
    }

    return false;
}


void AWUpdateHelper::showInfo(const QVersionNumber &_version)
{
    qCDebug(LOG_AW) << "Version" << _version;

    QString text = i18n("You are using the actual version %1", _version.toString());
    if (!QString(COMMIT_SHA).isEmpty())
        text += QString(" (%1)").arg(QString(COMMIT_SHA));
    return genMessageBox(i18n("No new version found"), text, QMessageBox::Ok)->open();
}


void AWUpdateHelper::showUpdates(const QVersionNumber &_version)
{
    qCDebug(LOG_AW) << "Version" << _version;

    QString text;
    text += i18nc("Current version : %1", VERSION);
    text += QString(COMMIT_SHA).isEmpty() ? "\n" : QString(" (%1)\n").arg(QString(COMMIT_SHA));
    text += i18n("New version : %1", _version.toString()) + "\n\n";
    text += i18n("Click \"Ok\" to download");

    genMessageBox(i18n("There are updates"), text, QMessageBox::Ok | QMessageBox::Cancel)
        ->open(this, SLOT(userReplyOnUpdates(QAbstractButton *)));
}


void AWUpdateHelper::userReplyOnUpdates(QAbstractButton *_button)
{
    QMessageBox::ButtonRole ret = dynamic_cast<QMessageBox *>(sender())->buttonRole(_button);
    qCInfo(LOG_AW) << "User select" << ret;

    switch (ret) {
    case QMessageBox::AcceptRole:
        QDesktopServices::openUrl(QString(RELEASES) + m_foundVersion.toString());
        break;
    case QMessageBox::RejectRole:
    default:
        break;
    }
}


void AWUpdateHelper::versionReplyRecieved(QNetworkReply *_reply, const bool _showAnyway)
{
    qCDebug(LOG_AW) << "Show message anyway" << _showAnyway;
    if (_reply->error() != QNetworkReply::NoError) {
        qCWarning(LOG_AW) << "An error occurs" << _reply->error() << "with message"
                          << _reply->errorString();
        return;
    }

    QJsonParseError error = QJsonParseError();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(_reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_AW) << "Parse error" << error.errorString();
        return;
    }
    _reply->deleteLater();

    // convert to map
    QVariantMap firstRelease = jsonDoc.toVariant().toList().first().toMap();
    QString version = firstRelease["tag_name"].toString();
    version.remove("V.");
    m_foundVersion = QVersionNumber::fromString(version);
    qCInfo(LOG_AW) << "Update found version to" << m_foundVersion;

    QVersionNumber oldVersion = QVersionNumber::fromString(VERSION);
    if (oldVersion < m_foundVersion)
        return showUpdates(m_foundVersion);
    else if (_showAnyway)
        return showInfo(m_foundVersion);
}


// additional method which is used to show message box which does not block UI
QMessageBox *AWUpdateHelper::genMessageBox(const QString &_title, const QString &_body,
                                           const QMessageBox::StandardButtons _buttons)
{
    qCDebug(LOG_AW) << "Construct message box with title" << _title << "and body" << _body;

    auto *msgBox = new QMessageBox(nullptr);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setModal(false);
    msgBox->setWindowTitle(_title);
    msgBox->setText(_body);
    msgBox->setStandardButtons(_buttons);
    msgBox->setIcon(QMessageBox::Information);

    return msgBox;
}
