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
#include <QFile>
#include <QProcess>
#include <QUrl>

#include <fontdialog/fontdialog.h>

#include "awdebug.h"
#include "awupdatehelper.h"


AWActions::AWActions(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_updateHelper = new AWUpdateHelper(this);
}


AWActions::~AWActions()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    delete m_updateHelper;
}


void AWActions::checkUpdates(const bool showAnyway)
{
    qCDebug(LOG_AW) << "Show anyway" << showAnyway;

    if (!m_updateHelper->checkVersion())
        m_updateHelper->checkUpdates(showAnyway);
}


QString AWActions::getFileContent(const QString &path) const
{
    qCDebug(LOG_AW) << "Get content from file" << path;

    QFile inputFile(path);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(LOG_AW) << "Could not open file as text"
                          << inputFile.fileName();
        return QString();
    }

    QString output = inputFile.readAll();
    inputFile.close();
    return output;
}


// HACK: since QML could not use QLoggingCategory I need this hack
bool AWActions::isDebugEnabled() const
{
    return LOG_AW().isDebugEnabled();
}


bool AWActions::runCmd(const QString &cmd) const
{
    qCDebug(LOG_AW) << "Cmd" << cmd;

    sendNotification(QString("Info"), i18n("Run %1", cmd));

    return QProcess::startDetached(cmd);
}


// HACK: this method uses variable from version.h
void AWActions::showReadme() const
{
    QDesktopServices::openUrl(QUrl(HOMEPAGE));
}


void AWActions::showLegacyInfo() const
{
    QMessageBox *msgBox = new QMessageBox(nullptr);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setModal(false);
    msgBox->setWindowTitle(i18n("Not supported"));
    msgBox->setText(
        i18n("You are using mammoth's Qt version, try to update it first!"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Information);

    msgBox->open();
}


// HACK: this method uses variables from version.h
QString AWActions::getAboutText(const QString &type) const
{
    qCDebug(LOG_AW) << "Type" << type;

    return AWDebug::getAboutText(type);
}


QVariantMap AWActions::getFont(const QVariantMap &defaultFont) const
{
    qCDebug(LOG_AW) << "Default font is" << defaultFont;

    QVariantMap fontMap;
    int ret = 0;
    CFont defaultCFont = CFont(defaultFont[QString("family")].toString(),
                               defaultFont[QString("size")].toInt(), 400, false,
                               defaultFont[QString("color")].toString());
    CFont font = CFontDialog::getFont(i18n("Select font"), defaultCFont, false,
                                      false, &ret);

    fontMap[QString("applied")] = ret;
    fontMap[QString("color")] = font.color().name();
    fontMap[QString("family")] = font.family();
    fontMap[QString("size")] = font.pointSize();

    return fontMap;
}


// to avoid additional object definition this method is static
void AWActions::sendNotification(const QString &eventId, const QString &message)
{
    qCDebug(LOG_AW) << "Event" << eventId << "with message" << message;

    KNotification *notification = KNotification::event(
        eventId, QString("Awesome Widget ::: %1").arg(eventId), message);
    notification->setComponentName(
        QString("plasma-applet-org.kde.plasma.awesome-widget"));
}
