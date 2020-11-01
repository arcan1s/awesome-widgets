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


AWActions::AWActions(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_updateHelper = new AWUpdateHelper(this);
}


AWActions::~AWActions()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


void AWActions::checkUpdates(const bool _showAnyway)
{
    qCDebug(LOG_AW) << "Show anyway" << _showAnyway;

    if (!m_updateHelper->checkVersion())
        m_updateHelper->checkUpdates(_showAnyway);
}


QString AWActions::getFileContent(const QString &_path)
{
    qCDebug(LOG_AW) << "Get content from file" << _path;

    QFile inputFile(_path);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(LOG_AW) << "Could not open file as text" << inputFile.fileName();
        return "";
    }

    QString output = inputFile.readAll();
    inputFile.close();
    return output;
}


// HACK: since QML could not use QLoggingCategory I need this hack
bool AWActions::isDebugEnabled()
{
    return LOG_AW().isDebugEnabled();
}


bool AWActions::runCmd(const QString &_cmd, const QStringList &_args)
{
    qCDebug(LOG_AW) << "Cmd" << _cmd << "args" << _args;

    sendNotification(QString("Info"), i18n("Run %1", _cmd));

    return QProcess::startDetached(_cmd, _args);
}


// HACK: this method uses variable from version.h
void AWActions::showReadme()
{
    QDesktopServices::openUrl(QUrl(HOMEPAGE));
}


void AWActions::showLegacyInfo()
{
    auto *msgBox = new QMessageBox(nullptr);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setModal(false);
    msgBox->setWindowTitle(i18n("Not supported"));
    msgBox->setText(i18n("You are using mammoth's Qt version, try to update it first"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Information);

    msgBox->open();
}


// HACK: this method uses variables from version.h
QString AWActions::getAboutText(const QString &_type)
{
    qCDebug(LOG_AW) << "Type" << _type;

    return AWDebug::getAboutText(_type);
}


QVariantMap AWActions::getFont(const QVariantMap &_defaultFont)
{
    qCDebug(LOG_AW) << "Default font is" << _defaultFont;

    QVariantMap fontMap;
    int ret = 0;
    CFont defaultCFont = CFont(_defaultFont["family"].toString(), _defaultFont["size"].toInt(), 400,
                               false, _defaultFont["color"].toString());
    CFont font = CFontDialog::getFont(i18n("Select font"), defaultCFont, false, false, &ret);

    fontMap["applied"] = ret;
    fontMap["color"] = font.color().name();
    fontMap["family"] = font.family();
    fontMap["size"] = font.pointSize();

    return fontMap;
}


// to avoid additional object definition this method is static
void AWActions::sendNotification(const QString &_eventId, const QString &_message)
{
    qCDebug(LOG_AW) << "Event" << _eventId << "with message" << _message;

    KNotification *notification
        = KNotification::event(_eventId, QString("Awesome Widget ::: %1").arg(_eventId), _message);
    notification->setComponentName("plasma-applet-org.kde.plasma.awesome-widget");
}
