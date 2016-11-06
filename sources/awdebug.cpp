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


#include <KI18n/KLocalizedString>

#include "awdebug.h"


Q_LOGGING_CATEGORY(LOG_AW, "org.kde.plasma.awesomewidget",
                   QtMsgType::QtWarningMsg)
Q_LOGGING_CATEGORY(LOG_DBUS, "org.kde.plasma.awdbus", QtMsgType::QtWarningMsg)
Q_LOGGING_CATEGORY(LOG_DP, "org.kde.plasma.desktoppanel",
                   QtMsgType::QtWarningMsg)
Q_LOGGING_CATEGORY(LOG_ESM, "org.kde.plasma.extsysmon", QtMsgType::QtWarningMsg)
Q_LOGGING_CATEGORY(LOG_ESS, "org.kde.plasma.extsysmonsources",
                   QtMsgType::QtWarningMsg)
Q_LOGGING_CATEGORY(LOG_LIB, "org.kde.plasma.awesomewidgets",
                   QtMsgType::QtWarningMsg)


QString AWDebug::getAboutText(const QString type)
{
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
        text = i18n("Links:") + QString("<ul>")
               + QString("<li><a href=\"%1\">%2</a></li>")
                     .arg(QString(HOMEPAGE))
                     .arg(i18n("Homepage"))
               + QString("<li><a href=\"%1\">%2</a></li>")
                     .arg(QString(REPOSITORY))
                     .arg(i18n("Repository"))
               + QString("<li><a href=\"%1\">%2</a></li>")
                     .arg(QString(BUGTRACKER))
                     .arg(i18n("Bugtracker"))
               + QString("<li><a href=\"%1\">%2</a></li>")
                     .arg(QString(TRANSLATION))
                     .arg(i18n("Translation issue"))
               + QString("<li><a href=\"%1\">%2</a></li>")
                     .arg(QString(AUR_PACKAGES))
                     .arg(i18n("AUR packages"))
               + QString("<li><a href=\"%1\">%2</a></li>")
                     .arg(QString(OPENSUSE_PACKAGES))
                     .arg(i18n("openSUSE packages"))
               + QString("</ul>");
    } else if (type == QString("copy")) {
        text = QString("<small>&copy; %1 <a href=\"mailto:%2\">%3</a><br>")
                   .arg(QString(DATE))
                   .arg(QString(EMAIL))
                   .arg(QString(AUTHOR))
               + i18n("This software is licensed under %1", QString(LICENSE))
               + QString("</small>");
    } else if (type == QString("translators")) {
        QStringList translatorList = QString(TRANSLATORS).split(QChar(','));
        for (auto &translator : translatorList)
            translator = QString("<li>%1</li>").arg(translator);
        text = i18n("Translators:") + QString("<ul>")
               + translatorList.join(QString("")) + QString("</ul>");
    } else if (type == QString("3rdparty")) {
        QStringList trdPartyList
            = QString(TRDPARTY_LICENSE)
                  .split(QChar(';'), QString::SkipEmptyParts);
        for (int i = 0; i < trdPartyList.count(); i++)
            trdPartyList[i]
                = QString("<li><a href=\"%3\">%1</a> (%2 license)</li>")
                      .arg(trdPartyList.at(i).split(QChar(','))[0])
                      .arg(trdPartyList.at(i).split(QChar(','))[1])
                      .arg(trdPartyList.at(i).split(QChar(','))[2]);
        text = i18n("This software uses:") + QString("<ul>")
               + trdPartyList.join(QString("")) + QString("</ul>");
    } else if (type == QString("thanks")) {
        QStringList thanks = QString(SPECIAL_THANKS)
                                 .split(QChar(';'), QString::SkipEmptyParts);
        for (int i = 0; i < thanks.count(); i++)
            thanks[i] = QString("<li><a href=\"%2\">%1</a></li>")
                            .arg(thanks.at(i).split(QChar(','))[0])
                            .arg(thanks.at(i).split(QChar(','))[1]);
        text = i18n("Special thanks to:") + QString("<ul>")
               + thanks.join(QString("")) + QString("</ul>");
    }

    return text;
}


QStringList AWDebug::getBuildData()
{
    QStringList metadata;
    metadata.append(QString("=== Awesome Widgets configuration details ==="));
    // build information
    metadata.append(QString("Build details:"));
    metadata.append(QString("    VERSION: %1").arg(VERSION));
    metadata.append(QString("    COMMIT_SHA: %1").arg(COMMIT_SHA));
    metadata.append(QString("    BUILD_DATE: %1").arg(BUILD_DATE));
    // configuration
    metadata.append(QString("API details:"));
    metadata.append(QString("    AW_GRAPHITEM_API: %1").arg(AW_GRAPHITEM_API));
    metadata.append(QString("    AW_EXTQUOTES_API: %1").arg(AW_EXTQUOTES_API));
    metadata.append(QString("    AW_EXTSCRIPT_API: %1").arg(AW_EXTSCRIPT_API));
    metadata.append(
        QString("    AW_EXTUPGRADE_API: %1").arg(AW_EXTUPGRADE_API));
    metadata.append(
        QString("    AW_EXTWEATHER_API: %1").arg(AW_EXTWEATHER_API));
    metadata.append(QString("    AW_FORMATTER_API: %1").arg(AW_FORMATTER_API));
    metadata.append(QString("    REQUEST_TIMEOUT: %1").arg(REQUEST_TIMEOUT));
    metadata.append(QString("    TIME_KEYS: %1").arg(TIME_KEYS));
    metadata.append(QString("    STATIC_KEYS: %1").arg(STATIC_KEYS));
    // cmake properties
    metadata.append(QString("cmake properties:"));
    metadata.append(QString("    CMAKE_BUILD_TYPE: %1").arg(CMAKE_BUILD_TYPE));
    metadata.append(
        QString("    CMAKE_CXX_COMPILER: %1").arg(CMAKE_CXX_COMPILER));
    metadata.append(QString("    CMAKE_CXX_FLAGS: %1").arg(CMAKE_CXX_FLAGS));
    metadata.append(
        QString("    CMAKE_CXX_FLAGS_DEBUG: %1").arg(CMAKE_CXX_FLAGS_DEBUG));
    metadata.append(QString("    CMAKE_CXX_FLAGS_RELEASE: %1")
                        .arg(CMAKE_CXX_FLAGS_RELEASE));
    metadata.append(QString("    CMAKE_CXX_FLAGS_OPTIMIZATION: %1")
                        .arg(CMAKE_CXX_FLAGS_OPTIMIZATION));
    metadata.append(
        QString("    CMAKE_DEFINITIONS: %1").arg(CMAKE_DEFINITIONS));
    metadata.append(
        QString("    CMAKE_INSTALL_PREFIX: %1").arg(CMAKE_INSTALL_PREFIX));
    metadata.append(QString("    CMAKE_MODULE_LINKER_FLAGS: %1")
                        .arg(CMAKE_MODULE_LINKER_FLAGS));
    metadata.append(QString("    CMAKE_SHARED_LINKER_FLAGS: %1")
                        .arg(CMAKE_SHARED_LINKER_FLAGS));
    // components
    metadata.append(QString("Components data:"));
    metadata.append(QString("    BUILD_PLASMOIDS: %1").arg(BUILD_PLASMOIDS));
    metadata.append(
        QString("    BUILD_DEB_PACKAGE: %1").arg(BUILD_DEB_PACKAGE));
    metadata.append(
        QString("    BUILD_RPM_PACKAGE: %1").arg(BUILD_RPM_PACKAGE));
    metadata.append(
        QString("    CLANGFORMAT_EXECUTABLE: %1").arg(CLANGFORMAT_EXECUTABLE));
    metadata.append(QString("    COVERITY_COMMENT: %1").arg(COVERITY_COMMENT));
    metadata.append(
        QString("    COVERITY_DIRECTORY: %1").arg(COVERITY_DIRECTORY));
    metadata.append(QString("    COVERITY_EMAIL: %1").arg(COVERITY_EMAIL));
    metadata.append(
        QString("    COVERITY_EXECUTABLE: %1").arg(COVERITY_EXECUTABLE));
    metadata.append(QString("    COVERITY_URL: %1").arg(COVERITY_URL));
    metadata.append(
        QString("    CPPCHECK_EXECUTABLE: %1").arg(CPPCHECK_EXECUTABLE));
    // additional functions
    metadata.append(QString("    PROP_FUTURE: %1").arg(PROP_FUTURE));
    metadata.append(QString("    PROP_LOAD: %1").arg(PROP_LOAD));
    metadata.append(QString("    PROP_TEST: %1").arg(PROP_TEST));

    return metadata;
}
