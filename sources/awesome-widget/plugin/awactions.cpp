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

    auto output = inputFile.readAll();
    inputFile.close();
    return output;
}


bool AWActions::runCmd(const QString &_cmd, const QStringList &_args)
{
    qCDebug(LOG_AW) << "Cmd" << _cmd << "args" << _args;

    sendNotification("system", i18n("Run %1", _cmd));

    return QProcess::startDetached(_cmd, _args);
}


// HACK: this method uses variable from version.h
void AWActions::showReadme()
{
    QDesktopServices::openUrl(QUrl(HOMEPAGE));
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
    auto ret = 0;
    auto defaultCFont = CFont(_defaultFont["family"].toString(), _defaultFont["size"].toInt(), 400, false,
                              _defaultFont["color"].toString());
    auto font = CFontDialog::getFont(i18n("Select font"), defaultCFont, false, false, &ret);

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

    auto event = KNotification::event(_eventId, QString("Awesome Widget ::: %1").arg(_eventId), _message);
    event->setComponentName("plasma-applet-org.kde.plasma.awesome-widget");
}
