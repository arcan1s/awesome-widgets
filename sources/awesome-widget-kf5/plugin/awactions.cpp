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
#include <QHBoxLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkInterface>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QProcessEnvironment>
#include <QRegExp>
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


bool AWActions::checkKeys(const QMap<QString, QVariant> data)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Data" << data;

    return (data.count() != 0);
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


void AWActions::addDevice(const QString source)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source" << source;

    QRegExp diskRegexp = QRegExp(QString("disk/(?:md|sd|hd)[a-z|0-9]_.*/Rate/(?:rblk)"));
    QRegExp fanRegexp = QRegExp(QString("lmsensors/.*/fan.*"));
    QRegExp mountRegexp = QRegExp(QString("partitions/.*/filllevel"));
    QRegExp tempRegexp = QRegExp(QString("lmsensors/.*temp.*/.*"));

    if (diskRegexp.indexIn(source) > -1) {
        QStringList splitSource = source.split(QChar('/'));
        QString device = splitSource[0] + QString("/") + splitSource[1];
        diskDevices.append(device);
    } else if (fanRegexp.indexIn(source) > -1)
        fanDevices.append(source);
    else if (mountRegexp.indexIn(source) > -1) {
        QString device = source;
        device.remove(QString("partitions")).remove(QString("/filllevel"));
        mountDevices.append(device);
    } else if (tempRegexp.indexIn(source) > -1)
        tempDevices.append(source);
}


QString AWActions::getAboutText(const QString type)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Type" << type;

    QString text;
    if (type == QString("header"))
        text = QString(NAME);
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


QStringList AWActions::getDiskDevices()
{
    if (debug) qDebug() << PDEBUG;

    diskDevices.sort();

    return diskDevices;
}


QStringList AWActions::getFanDevices()
{
    if (debug) qDebug() << PDEBUG;

    fanDevices.sort();

    return fanDevices;
}


QStringList AWActions::getHddDevices()
{
    if (debug) qDebug() << PDEBUG;

    QStringList allDevices = QDir(QString("/dev")).entryList(QDir::System, QDir::Name);
    QStringList devices = allDevices.filter(QRegExp(QString("^[hms]d[a-z]$")));
    for (int i=0; i<devices.count(); i++)
        devices[i] = QString("/dev/") + devices[i];
    devices.sort();

    return devices;
}


QStringList AWActions::getMountDevices()
{
    if (debug) qDebug() << PDEBUG;

    mountDevices.sort();

    return mountDevices;
}


QStringList AWActions::getNetworkDevices()
{
    if (debug) qDebug() << PDEBUG;

    QStringList interfaceList;
    QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
    for (int i=0; i<rawInterfaceList.count(); i++)
        interfaceList.append(rawInterfaceList[i].name());
    interfaceList.sort();

    return interfaceList;
}


QStringList AWActions::getTempDevices()
{
    if (debug) qDebug() << PDEBUG;

    tempDevices.sort();

    return tempDevices;
}


QMap<QString, QVariant> AWActions::getFont(const QMap<QString, QVariant> defaultFont)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QVariant> fontMap;
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


QString AWActions::selectDevices(const QStringList source, const QStringList current)
{
    if (debug) qDebug() << PDEBUG;

    // paint
    QDialog *dialog = new QDialog(0);
    QListWidget *widget = new QListWidget(dialog);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                     Qt::Vertical, dialog);
    QHBoxLayout *layout = new QHBoxLayout(dialog);
    layout->addWidget(widget);
    layout->addWidget(buttons);
    dialog->setLayout(layout);
    connect(buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), dialog, SLOT(reject()));

    // fill
    for (int i=0; i<source.count(); i++) {
        QListWidgetItem *item = new QListWidgetItem(source[i]);
        if (current.contains(source[i]))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        widget->addItem(item);
    }

    // exec
    QStringList selected;
    int ret = dialog->exec();
    if (debug) qDebug() << PDEBUG << ":" << "Dialog returns" << ret;
    if (ret == QDialog::Accepted) {
        for (int i=0; i<widget->count(); i++)
            if (widget->item(i)->checkState() == Qt::Checked)
                selected.append(widget->item(i)->text());
    } else
        selected = current;
    delete dialog;

    return selected.join(QString("@@"));
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
