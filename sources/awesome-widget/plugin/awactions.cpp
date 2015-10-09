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

#include "awactions.h"

#include <KI18n/KLocalizedString>
#include <KNotifications/KNotification>

#include <QDesktopServices>
#include <QDir>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>
#include <QStandardPaths>

#include <fontdialog/fontdialog.h>

#include "awdebug.h"
#include "version.h"


AWActions::AWActions(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW);
}


AWActions::~AWActions()
{
    qCDebug(LOG_AW);
}


void AWActions::checkUpdates(const bool showAnyway)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Show anyway" << showAnyway;

    // showAnyway options requires to show message if no updates found on direct
    // request. In case of automatic check no message will be shown
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,
            [showAnyway, this](QNetworkReply *reply) {
                return versionReplyRecieved(reply, showAnyway);
            });

    manager->get(QNetworkRequest(QUrl(VERSION_API)));
}


bool AWActions::dropCache() const
{
    qCDebug(LOG_AW);

    QString fileName = QString("%1/awesomewidgets.ndx").
        arg(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));

    return QFile(fileName).remove();
}


// HACK: since QML could not use QLoggingCategory I need this hack
bool AWActions::isDebugEnabled() const
{
    qCDebug(LOG_AW);

    return LOG_AW().isDebugEnabled();
}


bool AWActions::runCmd(const QString cmd) const
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Cmd" << cmd;

    sendNotification(QString("Info"), i18n("Run %1", cmd));

    return QProcess::startDetached(cmd);
}


// HACK: this method uses variable from version.h
void AWActions::showReadme() const
{
    qCDebug(LOG_AW);

    QDesktopServices::openUrl(QString(HOMEPAGE));
}


// HACK: this method uses variables from version.h
QString AWActions::getAboutText(const QString type) const
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Type" << type;

    QString text;
    if (type == QString("header")) {
        text = QString(NAME);
    } else if (type == QString("version")) {
        text = i18n("Version %1 (build date %2)", QString(VERSION), QString(BUILD_DATE));
        if (!QString(COMMIT_SHA).isEmpty()) text += QString(" (%1)").arg(QString(COMMIT_SHA));
    } else if (type == QString("description")) {
        text = i18n("A set of minimalistic plasmoid widgets");
    } else if (type == QString("links")) {
        text = i18n("Links:") + QString("<br>")
            + QString("<a href=\"%1\">%2</a><br>").arg(QString(HOMEPAGE)).arg(i18n("Homepage"))
            + QString("<a href=\"%1\">%2</a><br>").arg(QString(REPOSITORY)).arg(i18n("Repository"))
            + QString("<a href=\"%1\">%2</a><br>").arg(QString(BUGTRACKER)).arg(i18n("Bugtracker"))
            + QString("<a href=\"%1\">%2</a><br>").arg(QString(TRANSLATION)).arg(i18n("Translation issue"))
            + QString("<a href=\"%1\">%2</a><br>").arg(QString(AUR_PACKAGES)).arg(i18n("AUR packages"))
            + QString("<a href=\"%1\">%2</a>").arg(QString(OPENSUSE_PACKAGES)).arg(i18n("openSUSE packages"));
    } else if (type == QString("copy")) {
        text = QString("<small>&copy; %1 <a href=\"mailto:%2\">%3</a><br>").
            arg(QString(DATE)).arg(QString(EMAIL)).arg(QString(AUTHOR))
            + i18n("This software is licensed under %1", QString(LICENSE))
            + QString("</small>");
    } else if (type == QString("translators")) {
        text = i18n("Translators: %1", QString(TRANSLATORS));
    } else if (type == QString("3rdparty")) {
        QStringList trdPartyList = QString(TRDPARTY_LICENSE).split(QChar(';'), QString::SkipEmptyParts);
        for (int i=0; i<trdPartyList.count(); i++)
            trdPartyList[i] = QString("<a href=\"%3\">%1</a> (%2 license)").
                arg(trdPartyList.at(i).split(QChar(','))[0]).
                arg(trdPartyList.at(i).split(QChar(','))[1]).
                arg(trdPartyList.at(i).split(QChar(','))[2]);
        text = i18n("This software uses: %1", trdPartyList.join(QString(", ")));
    }

    return text;
}


QVariantMap AWActions::getFont(const QVariantMap defaultFont) const
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Default font is" << defaultFont;

    QVariantMap fontMap;
    CFont defaultCFont = CFont(defaultFont[QString("family")].toString(),
                               defaultFont[QString("size")].toInt(),
                               400, false, defaultFont[QString("color")].toString());
    CFont font = CFontDialog::getFont(i18n("Select font"), defaultCFont,
                                      false, false);
    fontMap[QString("color")] = font.color().name();
    fontMap[QString("family")] = font.family();
    fontMap[QString("size")] = font.pointSize();

    return fontMap;
}


QVariantMap AWActions::readDataEngineConfiguration() const
{
    qCDebug(LOG_AW);

    QString fileName = QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                              QString("plasma-dataengine-extsysmon.conf"));
    qCInfo(LOG_AW) << "Configuration file" << fileName;
    QSettings settings(fileName, QSettings::IniFormat);
    QVariantMap configuration;

    settings.beginGroup(QString("Configuration"));
    configuration[QString("ACPIPATH")] = settings.value(QString("ACPIPATH"), QString("/sys/class/power_supply/"));
    configuration[QString("GPUDEV")] = settings.value(QString("GPUDEV"), QString("auto"));
    configuration[QString("HDDDEV")] = settings.value(QString("HDDDEV"), QString("all"));
    configuration[QString("HDDTEMPCMD")] = settings.value(QString("HDDTEMPCMD"), QString("sudo smartctl -a"));
    configuration[QString("MPDADDRESS")] = settings.value(QString("MPDADDRESS"), QString("localhost"));
    configuration[QString("MPDPORT")] = settings.value(QString("MPDPORT"), QString("6600"));
    configuration[QString("MPRIS")] = settings.value(QString("MPRIS"), QString("auto"));
    configuration[QString("PLAYER")] = settings.value(QString("PLAYER"), QString("mpris"));
    configuration[QString("PLAYERSYMBOLS")] = settings.value(QString("PLAYERSYMBOLS"), QString("10"));
    settings.endGroup();

    qCInfo(LOG_AW) << "Configuration" << configuration;

    return configuration;
}


void AWActions::writeDataEngineConfiguration(const QVariantMap configuration) const
{
    qCDebug(LOG_AW);

    QString fileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
        + QString("/plasma-dataengine-extsysmon.conf");
    QSettings settings(fileName, QSettings::IniFormat);
    qCInfo(LOG_AW) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Configuration"));
    settings.setValue(QString("ACPIPATH"), configuration[QString("ACPIPATH")]);
    settings.setValue(QString("GPUDEV"), configuration[QString("GPUDEV")]);
    settings.setValue(QString("HDDDEV"), configuration[QString("HDDDEV")]);
    settings.setValue(QString("HDDTEMPCMD"), configuration[QString("HDDTEMPCMD")]);
    settings.setValue(QString("MPDADDRESS"), configuration[QString("MPDADDRESS")]);
    settings.setValue(QString("MPDPORT"), configuration[QString("MPDPORT")]);
    settings.setValue(QString("MPRIS"), configuration[QString("MPRIS")]);
    settings.setValue(QString("PLAYER"), configuration[QString("PLAYER")]);
    settings.setValue(QString("PLAYERSYMBOLS"), configuration[QString("PLAYERSYMBOLS")]);
    settings.endGroup();

    settings.sync();
}


// to avoid additional object definition this method is static
void AWActions::sendNotification(const QString eventId, const QString message)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Event" << eventId;
    qCDebug(LOG_AW) << "Message" << message;

    KNotification *notification = KNotification::event(eventId,
                                                       QString("Awesome Widget ::: %1").arg(eventId),
                                                       message);
    notification->setComponentName(QString("plasma-applet-org.kde.plasma.awesome-widget"));
}


void AWActions::showInfo(const QString version) const
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Version" << version;

    QString text = i18n("You are using the actual version %1", version);
    if (!QString(COMMIT_SHA).isEmpty())
        text += QString(" (%1)").arg(QString(COMMIT_SHA));
    QMessageBox::information(nullptr, i18n("No new version found"), text);
}



void AWActions::showUpdates(const QString version) const
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Version" << version;

    QString text;
    text += i18n("Current version : %1", QString(VERSION));
    text += QString(COMMIT_SHA).isEmpty() ? QString("\n") : QString(" (%1)\n").arg(QString(COMMIT_SHA));
    text += i18n("New version : %1", version) + QString("\n\n");
    text += i18n("Click \"Ok\" to download");

    int select = QMessageBox::information(nullptr, i18n("There are updates"), text,
                                          QMessageBox::Ok | QMessageBox::Cancel);
    switch (select) {
    case QMessageBox::Ok:
        QDesktopServices::openUrl(QString(RELEASES) + version);
        break;
    case QMessageBox::Cancel:
    default:
        break;
    }
}


void AWActions::versionReplyRecieved(QNetworkReply *reply, const bool showAnyway) const
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Return code" << reply->error();
    qCDebug(LOG_AW) << "Reply error message" << reply->errorString();
    qCDebug(LOG_AW) << "Show anyway" << showAnyway;

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    reply->deleteLater();
    if ((reply->error() != QNetworkReply::NoError)
        || (error.error != QJsonParseError::NoError)) {
        qCWarning(LOG_AW) << "Parse error" << error.errorString();
        return;
    }

    // convert to map
    QVariantMap firstRelease = jsonDoc.toVariant().toList().first().toMap();
    QString version = firstRelease[QString("tag_name")].toString();
    version.remove(QString("V."));
    qCInfo(LOG_AW) << "Found version" << version;

    // FIXME: possible there is a better way to check versions
    int old_major = QString(VERSION).split(QChar('.')).at(0).toInt();
    int old_minor = QString(VERSION).split(QChar('.')).at(1).toInt();
    int old_patch = QString(VERSION).split(QChar('.')).at(2).toInt();
    int new_major = QString(version).split(QChar('.')).at(0).toInt();
    int new_minor = QString(version).split(QChar('.')).at(1).toInt();
    int new_patch = QString(version).split(QChar('.')).at(2).toInt();
    if ((old_major < new_major)
        || ((old_major == new_major) && (old_minor < new_minor))
        || ((old_major == new_major) && (old_minor == new_minor) && (old_patch < new_patch)))
        return showUpdates(version);
    else if (showAnyway)
        return showInfo(version);
}
