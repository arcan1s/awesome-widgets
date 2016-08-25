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


QString AWActions::getFileContent(const QString path) const
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


bool AWActions::runCmd(const QString cmd) const
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


// HACK: this method uses variables from version.h
QString AWActions::getAboutText(const QString type) const
{
    qCDebug(LOG_AW) << "Type" << type;

    QString text;
    if (type == QString("header")) {
        text = QString(NAME);
    } else if (type == QString("version")) {
        text = i18n("Version %1 (build date %2)", QString(VERSION),
                    QString(BUILD_DATE));
        if (!QString(COMMIT_SHA).isEmpty())
            text += QString(" (%1)").arg(QString(COMMIT_SHA));
    } else if (type == QString("description")) {
        text = i18n("A set of minimalistic plasmoid widgets");
    } else if (type == QString("links")) {
        text = i18n("Links:") + QString("<br>")
               + QString("<a href=\"%1\">%2</a><br>")
                     .arg(QString(HOMEPAGE))
                     .arg(i18n("Homepage"))
               + QString("<a href=\"%1\">%2</a><br>")
                     .arg(QString(REPOSITORY))
                     .arg(i18n("Repository"))
               + QString("<a href=\"%1\">%2</a><br>")
                     .arg(QString(BUGTRACKER))
                     .arg(i18n("Bugtracker"))
               + QString("<a href=\"%1\">%2</a><br>")
                     .arg(QString(TRANSLATION))
                     .arg(i18n("Translation issue"))
               + QString("<a href=\"%1\">%2</a><br>")
                     .arg(QString(AUR_PACKAGES))
                     .arg(i18n("AUR packages"))
               + QString("<a href=\"%1\">%2</a>")
                     .arg(QString(OPENSUSE_PACKAGES))
                     .arg(i18n("openSUSE packages"));
    } else if (type == QString("copy")) {
        text = QString("<small>&copy; %1 <a href=\"mailto:%2\">%3</a><br>")
                   .arg(QString(DATE))
                   .arg(QString(EMAIL))
                   .arg(QString(AUTHOR))
               + i18n("This software is licensed under %1", QString(LICENSE))
               + QString("</small>");
    } else if (type == QString("translators")) {
        text = i18n("Translators: %1", QString(TRANSLATORS));
    } else if (type == QString("3rdparty")) {
        QStringList trdPartyList
            = QString(TRDPARTY_LICENSE)
                  .split(QChar(';'), QString::SkipEmptyParts);
        for (int i = 0; i < trdPartyList.count(); i++)
            trdPartyList[i] = QString("<a href=\"%3\">%1</a> (%2 license)")
                                  .arg(trdPartyList.at(i).split(QChar(','))[0])
                                  .arg(trdPartyList.at(i).split(QChar(','))[1])
                                  .arg(trdPartyList.at(i).split(QChar(','))[2]);
        text = i18n("This software uses: %1", trdPartyList.join(QString(", ")));
    } else if (type == QString("thanks")) {
        QStringList thanks = QString(SPECIAL_THANKS)
                                 .split(QChar(';'), QString::SkipEmptyParts);
        for (int i = 0; i < thanks.count(); i++)
            thanks[i] = QString("<a href=\"%2\">%1</a>")
                            .arg(thanks.at(i).split(QChar(','))[0])
                            .arg(thanks.at(i).split(QChar(','))[1]);
        text = i18n("Special thanks to %1", thanks.join(QString(", ")));
    }

    return text;
}


QVariantMap AWActions::getFont(const QVariantMap defaultFont) const
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
void AWActions::sendNotification(const QString eventId, const QString message)
{
    qCDebug(LOG_AW) << "Event" << eventId << "with message" << message;

    KNotification *notification = KNotification::event(
        eventId, QString("Awesome Widget ::: %1").arg(eventId), message);
    notification->setComponentName(
        QString("plasma-applet-org.kde.plasma.awesome-widget"));
}
