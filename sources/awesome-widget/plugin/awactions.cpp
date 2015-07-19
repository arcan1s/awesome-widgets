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

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>
#include <QStandardPaths>

#include <fontdialog/fontdialog.h>
#include <pdebug/pdebug.h>

#include "extscript.h"
#include "graphicalitem.h"
#include "version.h"


AWActions::AWActions(QObject *parent)
    : QObject(parent)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));
}


AWActions::~AWActions()
{
    if (debug) qDebug() << PDEBUG;
}


void AWActions::checkUpdates()
{
    if (debug) qDebug() << PDEBUG;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(versionReplyRecieved(QNetworkReply *)));

    manager->get(QNetworkRequest(QUrl(VERSION_API)));
}


void AWActions::runCmd(const QString cmd)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Cmd" << cmd;

    QProcess command;
    sendNotification(QString("Info"), i18n("Run %1", cmd));

    command.startDetached(cmd);
}


void AWActions::sendEmail()
{
    if (debug) qDebug() << PDEBUG;
}


void AWActions::showReadme()
{
    if (debug) qDebug() << PDEBUG;

    QDesktopServices::openUrl(QString(HOMEPAGE));
}


QString AWActions::getAboutText(const QString type)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Type" << type;

    QString text;
    if (type == QString("header"))
        text = QString(NAME);
    else if (type == QString("version"))
        text = i18n("Version %1 (build date %2)", QString(VERSION), QString(BUILD_DATE));
    else if (type == QString("description"))
        text = i18n("A set of minimalistic plasmoid widgets");
    else if (type == QString("links"))
        text = i18n("Links:") + QString("<br>") +
               QString("<a href=\"%1\">%2</a><br>").arg(QString(HOMEPAGE)).arg(i18n("Homepage")) +
               QString("<a href=\"%1\">%2</a><br>").arg(QString(REPOSITORY)).arg(i18n("Repository")) +
               QString("<a href=\"%1\">%2</a><br>").arg(QString(BUGTRACKER)).arg(i18n("Bugtracker")) +
               QString("<a href=\"%1\">%2</a><br>").arg(QString(TRANSLATION)).arg(i18n("Translation issue")) +
               QString("<a href=\"%1\">%2</a><br>").arg(QString(AUR_PACKAGES)).arg(i18n("AUR packages")) +
               QString("<a href=\"%1\">%2</a>").arg(QString(OPENSUSE_PACKAGES)).arg(i18n("openSUSE packages"));
    else if (type == QString("copy"))
        text = QString("<small>&copy; %1 <a href=\"mailto:%2\">%3</a><br>").arg(QString(DATE)).arg(QString(EMAIL)).arg(QString(AUTHOR)) +
               i18n("This software is licensed under %1", QString(LICENSE)) + QString("</small>");
    else if (type == QString("translators"))
        text = i18n("Translators: %1", QString(TRANSLATORS));
    else if (type == QString("3rdparty")) {
        QStringList trdPartyList = QString(TRDPARTY_LICENSE).split(QChar(';'), QString::SkipEmptyParts);
        for (int i=0; i<trdPartyList.count(); i++)
            trdPartyList[i] = QString("<a href=\"%3\">%1</a> (%2 license)")
                    .arg(trdPartyList[i].split(QChar(','))[0])
                    .arg(trdPartyList[i].split(QChar(','))[1])
                    .arg(trdPartyList[i].split(QChar(','))[2]);
        text = i18n("This software uses: %1", trdPartyList.join(QString(", ")));
    }

    return text;
}


QVariantMap AWActions::getFont(const QVariantMap defaultFont)
{
    if (debug) qDebug() << PDEBUG;

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


QVariantMap AWActions::readDataEngineConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QString fileName = QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("plasma-dataengine-extsysmon.conf"));
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
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
    settings.endGroup();

    return configuration;
}


void AWActions::writeDataEngineConfiguration(const QVariantMap configuration)
{
    if (debug) qDebug() << PDEBUG;

    QString fileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QString("/plasma-dataengine-extsysmon.conf");
    QSettings settings(fileName, QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Configuration"));
    settings.setValue(QString("ACPIPATH"), configuration[QString("ACPIPATH")]);
    settings.setValue(QString("GPUDEV"), configuration[QString("GPUDEV")]);
    settings.setValue(QString("HDDDEV"), configuration[QString("HDDDEV")]);
    settings.setValue(QString("HDDTEMPCMD"), configuration[QString("HDDTEMPCMD")]);
    settings.setValue(QString("MPDADDRESS"), configuration[QString("MPDADDRESS")]);
    settings.setValue(QString("MPDPORT"), configuration[QString("MPDPORT")]);
    settings.setValue(QString("MPRIS"), configuration[QString("MPRIS")]);
    settings.setValue(QString("PLAYER"), configuration[QString("PLAYER")]);
    settings.endGroup();

    settings.sync();
}


void AWActions::sendNotification(const QString eventId, const QString message, const bool enablePopup)
{
    if ((eventId == QString("event")) && (!enablePopup)) return;

    KNotification *notification = KNotification::event(eventId, QString("Awesome Widget ::: %1").arg(eventId), message);
    notification->setComponentName(QString("plasma-applet-org.kde.plasma.awesome-widget"));
}


void AWActions::showUpdates(QString version)
{
    if (debug) qDebug() << PDEBUG;

    QString text;
    text += i18n("Current version : %1", QString(VERSION)) + QString("\n");
    text += i18n("New version : %1", version) + QString("\n\n");
    text += i18n("Click \"Ok\" to download");

    int select = QMessageBox::information(nullptr, i18n("There are updates"), text,
                                          QMessageBox::Ok | QMessageBox::Cancel);
    switch (select) {
    case QMessageBox::Ok:
        QDesktopServices::openUrl(QString(RELEASES) + version);
        break;
    }
}


void AWActions::versionReplyRecieved(QNetworkReply *reply)
{
    if (debug) qDebug() << PDEBUG;

    QString answer = reply->readAll();
    if (!answer.contains(QString("tag_name"))) return;
    QString version = QString(VERSION);
    if (debug) qDebug() << PDEBUG << answer;
    for (int i=0; i<answer.split(QString("tag_name")).count(); i++) {
        version = answer.split(QString("tag_name"))[1].split(QChar(','))[0];
        version.remove(QChar('"'));
        version.remove(QChar(':'));
        version.remove(QString("V."));
        break;
    }

    int old_major = QString(VERSION).split(QChar('.'))[0].toInt();
    int old_minor = QString(VERSION).split(QChar('.'))[1].toInt();
    int old_patch = QString(VERSION).split(QChar('.'))[2].toInt();
    int new_major = QString(version).split(QChar('.'))[0].toInt();
    int new_minor = QString(version).split(QChar('.'))[1].toInt();
    int new_patch = QString(version).split(QChar('.'))[2].toInt();
    if ((old_major < new_major) ||
        ((old_major == new_major) && (old_minor < new_minor)) ||
        ((old_major == new_major) && (old_minor == new_minor) && (old_patch < new_patch)))
        showUpdates(version);
}
