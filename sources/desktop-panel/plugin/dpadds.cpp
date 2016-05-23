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

#include "dpadds.h"

#include <KI18n/KLocalizedString>
#include <KNotifications/KNotification>
#include <KWindowSystem/KWindowSystem>

#include <QBuffer>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QPixmap>
#include <QScreen>

#include <fontdialog/fontdialog.h>

#include "awdebug.h"


DPAdds::DPAdds(QObject *parent)
    : QObject(parent)
{
    qSetMessagePattern(LOG_FORMAT);
    qCDebug(LOG_DP) << __PRETTY_FUNCTION__;
    for (auto metadata : getBuildData())
        qCDebug(LOG_DP) << metadata;

    connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)), this,
            SIGNAL(desktopChanged()));
    connect(KWindowSystem::self(), SIGNAL(windowAdded(WId)), this,
            SIGNAL(windowListChanged()));
    connect(KWindowSystem::self(), SIGNAL(windowRemoved(WId)), this,
            SIGNAL(windowListChanged()));
}


DPAdds::~DPAdds()
{
    qCDebug(LOG_DP) << __PRETTY_FUNCTION__;
}


// HACK: since QML could not use QLoggingCategory I need this hack
bool DPAdds::isDebugEnabled() const
{
    return LOG_DP().isDebugEnabled();
}


int DPAdds::currentDesktop() const
{
    return KWindowSystem::currentDesktop();
}


QStringList DPAdds::dictKeys() const
{
    QStringList allKeys;
    allKeys.append(QString("mark"));
    allKeys.append(QString("name"));
    allKeys.append(QString("number"));
    allKeys.append(QString("total"));

    return allKeys;
}


int DPAdds::numberOfDesktops() const
{
    return KWindowSystem::numberOfDesktops();
}


QString DPAdds::toolTipImage(const int desktop) const
{
    qCDebug(LOG_DP) << "Desktop" << desktop;
    // drop if no tooltip required
    if (m_tooltipType == QString("none"))
        return QString();

    // prepare
    DesktopWindowsInfo info = getInfoByDesktop(desktop);
    // special tooltip format for names
    if (m_tooltipType == QString("names")) {
        QStringList windowList;
        std::for_each(
            info.windowsData.cbegin(), info.windowsData.cend(),
            [&windowList](WindowData data) { windowList.append(data.name); });
        return QString("<ul><li>%1</li></ul>")
            .arg(windowList.join(QString("</li><li>")));
    }
    // init
    QGraphicsScene *toolTipScene = new QGraphicsScene();
    QGraphicsView *toolTipView = new QGraphicsView(toolTipScene);
    toolTipView->setStyleSheet(QString("background: transparent"));
    toolTipView->setContentsMargins(0, 0, 0, 0);
    toolTipView->setFrameShape(QFrame::NoFrame);
    toolTipView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    toolTipView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // update
    float margin = 5.0 * info.desktop.width() / 400.0;
    toolTipView->resize(info.desktop.width() + 2.0 * margin,
                        info.desktop.height() + 2.0 * margin);
    toolTipScene->clear();
    toolTipScene->setBackgroundBrush(QBrush(Qt::NoBrush));
    // borders
    toolTipScene->addLine(0, 0, 0, info.desktop.height() + 2.0 * margin);
    toolTipScene->addLine(0, info.desktop.height() + 2.0 * margin,
                          info.desktop.width() + 2.0 * margin,
                          info.desktop.height() + 2.0 * margin);
    toolTipScene->addLine(info.desktop.width() + 2.0 * margin,
                          info.desktop.height() + 2.0 * margin,
                          info.desktop.width() + 2.0 * margin, 0);
    toolTipScene->addLine(info.desktop.width() + 2.0 * margin, 0, 0, 0);

    if (m_tooltipType == QString("contours")) {
        QPen pen = QPen();
        pen.setWidthF(2.0 * info.desktop.width() / 400.0);
        pen.setColor(QColor(m_tooltipColor));
        for (auto data : info.windowsData) {
            QRect rect = data.rect;
            toolTipScene->addLine(rect.left() + margin, rect.bottom() + margin,
                                  rect.left() + margin, rect.top() + margin,
                                  pen);
            toolTipScene->addLine(rect.left() + margin, rect.top() + margin,
                                  rect.right() + margin, rect.top() + margin,
                                  pen);
            toolTipScene->addLine(rect.right() + margin, rect.top() + margin,
                                  rect.right() + margin, rect.bottom() + margin,
                                  pen);
            toolTipScene->addLine(rect.right() + margin, rect.bottom() + margin,
                                  rect.left() + margin, rect.bottom() + margin,
                                  pen);
        }
    } else if (m_tooltipType == QString("clean")) {
        QScreen *screen = QGuiApplication::primaryScreen();
        std::for_each(info.desktopsData.cbegin(), info.desktopsData.cend(),
                      [&toolTipScene, &screen](WindowData data) {
                          QPixmap desktop = screen->grabWindow(data.id);
                          toolTipScene->addPixmap(desktop)->setOffset(
                              data.rect.left(), data.rect.top());
                      });
    } else if (m_tooltipType == QString("windows")) {
        QScreen *screen = QGuiApplication::primaryScreen();
        std::for_each(info.desktopsData.cbegin(), info.desktopsData.cend(),
                      [&toolTipScene, &screen](WindowData data) {
                          QPixmap desktop = screen->grabWindow(data.id);
                          toolTipScene->addPixmap(desktop)->setOffset(
                              data.rect.left(), data.rect.top());
                      });
        std::for_each(info.windowsData.cbegin(), info.windowsData.cend(),
                      [&toolTipScene, &screen](WindowData data) {
                          QPixmap window = screen->grabWindow(data.id);
                          toolTipScene->addPixmap(window)->setOffset(
                              data.rect.left(), data.rect.top());
                      });
    }

    QPixmap image = toolTipView->grab().scaledToWidth(m_tooltipWidth);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    image.save(&buffer, "PNG");

    delete toolTipView;
    delete toolTipScene;

    return QString("<img src=\"data:image/png;base64,%1\"/>")
        .arg(QString(byteArray.toBase64()));
}


QString DPAdds::parsePattern(const QString pattern, const int desktop) const
{
    qCDebug(LOG_DP) << "Pattern" << pattern << "for desktop" << desktop;

    QString parsed = pattern;
    parsed.replace(QString("$$"), QString("$\\$\\"));
    for (auto key : dictKeys())
        parsed.replace(QString("$%1").arg(key), valueByKey(key, desktop));
    parsed.replace(QString("$\\$\\"), QString("$$"));

    return parsed;
}


void DPAdds::setMark(const QString newMark)
{
    qCDebug(LOG_DP) << "Mark" << newMark;

    m_mark = newMark;
}


void DPAdds::setToolTipData(const QVariantMap tooltipData)
{
    qCDebug(LOG_DP) << "Data" << tooltipData;

    m_tooltipColor = tooltipData[QString("tooltipColor")].toString();
    m_tooltipType = tooltipData[QString("tooltipType")].toString();
    m_tooltipWidth = tooltipData[QString("tooltipWidth")].toInt();
}


QString DPAdds::valueByKey(const QString key, int desktop) const
{
    qCDebug(LOG_DP) << "Requested key" << key << "for desktop" << desktop;
    if (desktop == -1)
        desktop = currentDesktop();

    QString currentMark = currentDesktop() == desktop ? m_mark : QString("");
    if (key == QString("mark"))
        return QString("%1")
            .arg(currentMark, m_mark.count(), QLatin1Char(' '))
            .replace(QString(" "), QString("&nbsp;"));
    else if (key == QString("name"))
        return KWindowSystem::desktopName(desktop).replace(QString(" "),
                                                           QString("&nbsp;"));
    else if (key == QString("number"))
        return QString::number(desktop);
    else if (key == QString("total"))
        return QString::number(numberOfDesktops());
    else
        return QString();
}


// HACK: this method uses variables from version.h
QString DPAdds::getAboutText(const QString type) const
{
    qCDebug(LOG_DP) << "Type" << type;

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
            trdPartyList[i]
                = QString("<a href=\"%3\">%1</a> (%2 license)")
                      .arg(trdPartyList.at(i).split(QChar(',')).at(0))
                      .arg(trdPartyList.at(i).split(QChar(',')).at(1))
                      .arg(trdPartyList.at(i).split(QChar(',')).at(2));
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


QVariantMap DPAdds::getFont(const QVariantMap defaultFont) const
{
    qCDebug(LOG_DP) << "Default font is" << defaultFont;

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
void DPAdds::sendNotification(const QString eventId, const QString message)
{
    qCDebug(LOG_DP) << "Event" << eventId << "with message" << message;

    KNotification *notification = KNotification::event(
        eventId, QString("Desktop Panel ::: %1").arg(eventId), message);
    notification->setComponentName(
        QString("plasma-applet-org.kde.plasma.desktop-panel"));
}


// slot for mouse click
void DPAdds::setCurrentDesktop(const int desktop) const
{
    qCDebug(LOG_DP) << "Desktop" << desktop;

    KWindowSystem::setCurrentDesktop(desktop);
}


DPAdds::DesktopWindowsInfo DPAdds::getInfoByDesktop(const int desktop) const
{
    qCDebug(LOG_DP) << "Desktop" << desktop;


    DesktopWindowsInfo info;
    info.desktop = KWindowSystem::workArea(desktop);

    for (auto id : KWindowSystem::windows()) {
        KWindowInfo winInfo = KWindowInfo(
            id, NET::Property::WMDesktop | NET::Property::WMGeometry
                    | NET::Property::WMState | NET::Property::WMWindowType
                    | NET::Property::WMVisibleName);
        if (!winInfo.isOnDesktop(desktop))
            continue;
        WindowData data;
        data.id = id;
        data.name = winInfo.visibleName();
        data.rect = winInfo.geometry();
        if (winInfo.windowType(NET::WindowTypeMask::NormalMask)
            == NET::WindowType::Normal) {
            if (winInfo.isMinimized())
                continue;
            info.windowsData.append(data);
        } else if (winInfo.windowType(NET::WindowTypeMask::DesktopMask)
                   == NET::WindowType::Desktop) {
            info.desktopsData.append(data);
        }
    }

    return info;
}
