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
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

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


void AWActions::sendNotification(const QString eventId, const QString message)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Event" << eventId;
    if (debug) qDebug() << PDEBUG << ":" << "Message" << message;

    KNotification *notification = KNotification::event(eventId, QString("Awesome Widget ::: ") + eventId, message);
    notification->setComponentName(QString("plasma-applet-org.kde.plasma.awesome-widget"));
}


void AWActions::showReadme()
{
    if (debug) qDebug() << PDEBUG;

    QDesktopServices::openUrl(QString(HOMEPAGE));
}


QMap<QString, QVariant> AWActions::readDataEngineConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QString fileName = QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("plasma-dataengine-extsysmon.conf"));
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QSettings settings(fileName, QSettings::IniFormat);
    QMap<QString, QVariant> rawConfig;

    settings.beginGroup(QString("Configuration"));
    rawConfig[QString("ACPIPATH")] = settings.value(QString("ACPIPATH"), QString("/sys/class/power_supply/"));
    rawConfig[QString("GPUDEV")] = settings.value(QString("GPUDEV"), QString("auto"));
    rawConfig[QString("HDDDEV")] = settings.value(QString("HDDDEV"), QString("all"));
    rawConfig[QString("HDDTEMPCMD")] = settings.value(QString("HDDTEMPCMD"), QString("sudo hddtemp"));
    rawConfig[QString("MPDADDRESS")] = settings.value(QString("MPDADDRESS"), QString("localhost"));
    rawConfig[QString("MPDPORT")] = settings.value(QString("MPDPORT"), QString("6600"));
    rawConfig[QString("MPRIS")] = settings.value(QString("MPRIS"), QString("auto"));
    rawConfig[QString("PKGCMD")] = settings.value(QString("PKGCMD"), QString("pacman -Qu"));
    rawConfig[QString("PKGNULL")] = settings.value(QString("PKGNULL"), QString("0"));
    rawConfig[QString("PLAYER")] = settings.value(QString("PLAYER"), QString("mpris"));
    settings.endGroup();

    return updateDataEngineConfiguration(rawConfig);
}


QMap<QString, QVariant> AWActions::updateDataEngineConfiguration(QMap<QString, QVariant> rawConfig)
{
    if (debug) qDebug() << PDEBUG;

    for (int i=rawConfig[QString("PKGNULL")].toString().split(QString(","), QString::SkipEmptyParts).count();
         i<rawConfig[QString("PKGCMD")].toString().split(QString(","), QString::SkipEmptyParts).count();
         i++)
        rawConfig[QString("PKGNULL")].toString() += QString(",0");

    for (int i=0; i<rawConfig.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" <<
            rawConfig.keys()[i] << QString("=") << rawConfig[rawConfig.keys()[i]];
    return rawConfig;
}


void AWActions::writeDataEngineConfiguration(const QMap<QString, QVariant> configuration)
{
    if (debug) qDebug() << PDEBUG;

    QString fileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QString("/plasma-dataengine-extsysmon.conf");
    QSettings settings(fileName, QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Configuration"));
    settings.setValue(QString("GPUDEV"), configuration[QString("GPUDEV")]);
    settings.setValue(QString("HDDDEV"), configuration[QString("HDDDEV")]);
    settings.setValue(QString("HDDTEMPCMD"), configuration[QString("HDDTEMPCMD")]);
    settings.setValue(QString("MPDADDRESS"), configuration[QString("MPDADDRESS")]);
    settings.setValue(QString("MPDPORT"), configuration[QString("MPDPORT")]);
    settings.setValue(QString("MPRIS"), configuration[QString("MPRIS")]);
    settings.setValue(QString("PKGCMD"), configuration[QString("PKGCMD")]);
    settings.setValue(QString("PKGNULL"), configuration[QString("PKGNULL")]);
    settings.setValue(QString("PLAYER"), configuration[QString("PLAYER")]);
    settings.endGroup();

    settings.sync();
}


void AWActions::showUpdates(QString version)
{
    if (debug) qDebug() << PDEBUG;

    QString text;
    text += i18n("Current version : %1", QString(VERSION)) + QString("\n");
    text += i18n("New version : %1", version) + QString("\n\n");
    text += i18n("Click \"Ok\" to download");

    int select = QMessageBox::information(0, i18n("There are updates"), text, QMessageBox::Ok | QMessageBox::Cancel);
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
