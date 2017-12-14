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
#include <QScreen>

#include <fontdialog/fontdialog.h>

#include "awdebug.h"


DPAdds::DPAdds(QObject *_parent)
    : QObject(_parent)
{
    qSetMessagePattern(AWDebug::LOG_FORMAT);
    qCDebug(LOG_DP) << __PRETTY_FUNCTION__;
    for (auto &metadata : AWDebug::getBuildData())
        qCDebug(LOG_DP) << metadata;

    connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)), this,
            SIGNAL(desktopChanged()));
    connect(KWindowSystem::self(), SIGNAL(windowAdded(WId)), this, SIGNAL(windowListChanged()));
    connect(KWindowSystem::self(), SIGNAL(windowRemoved(WId)), this, SIGNAL(windowListChanged()));
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


QStringList DPAdds::dictKeys(const bool _sorted, const QString &_regexp) const
{
    qCDebug(LOG_DP) << "Should be sorted" << _sorted << "and filter applied" << _regexp;

    QStringList allKeys;
    allKeys.append("mark");
    allKeys.append("name");
    allKeys.append("number");
    allKeys.append("total");

    if (_sorted)
        allKeys.sort();

    return allKeys.filter(QRegExp(_regexp));
}


int DPAdds::numberOfDesktops() const
{
    return KWindowSystem::numberOfDesktops();
}


QString DPAdds::toolTipImage(const int _desktop) const
{
    qCDebug(LOG_DP) << "Desktop" << _desktop;
    // drop if no tooltip required
    if (m_tooltipType == "none")
        return "";

    // prepare
    DesktopWindowsInfo info = getInfoByDesktop(_desktop);
    // special tooltip format for names
    if (m_tooltipType == "names") {
        QStringList windowList;
        std::for_each(info.windowsData.cbegin(), info.windowsData.cend(),
                      [&windowList](WindowData data) { windowList.append(data.name); });
        return QString("<ul><li>%1</li></ul>").arg(windowList.join("</li><li>"));
    }

    // init
    QGraphicsScene *toolTipScene = new QGraphicsScene();
    QGraphicsView *toolTipView = new QGraphicsView(toolTipScene);
    toolTipView->setStyleSheet("background: transparent");
    toolTipView->setContentsMargins(0, 0, 0, 0);
    toolTipView->setFrameShape(QFrame::NoFrame);
    toolTipView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    toolTipView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // update
    float margin = 5.0f * info.desktop.width() / 400.0f;
    toolTipView->resize(info.desktop.width() + 2.0f * margin,
                        info.desktop.height() + 2.0f * margin);
    toolTipScene->clear();
    toolTipScene->setBackgroundBrush(QBrush(Qt::NoBrush));
    // borders
    toolTipScene->addLine(0, 0, 0, info.desktop.height() + 2.0 * margin);
    toolTipScene->addLine(0, info.desktop.height() + 2.0 * margin,
                          info.desktop.width() + 2.0 * margin,
                          info.desktop.height() + 2.0 * margin);
    toolTipScene->addLine(info.desktop.width() + 2.0 * margin, info.desktop.height() + 2.0 * margin,
                          info.desktop.width() + 2.0 * margin, 0);
    toolTipScene->addLine(info.desktop.width() + 2.0 * margin, 0, 0, 0);

    if (m_tooltipType == "contours") {
        QPen pen = QPen();
        pen.setWidthF(2.0 * info.desktop.width() / 400.0);
        pen.setColor(QColor(m_tooltipColor));
        for (auto &data : info.windowsData) {
            QRect rect = data.rect;
            toolTipScene->addLine(rect.left() + margin, rect.bottom() + margin,
                                  rect.left() + margin, rect.top() + margin, pen);
            toolTipScene->addLine(rect.left() + margin, rect.top() + margin, rect.right() + margin,
                                  rect.top() + margin, pen);
            toolTipScene->addLine(rect.right() + margin, rect.top() + margin, rect.right() + margin,
                                  rect.bottom() + margin, pen);
            toolTipScene->addLine(rect.right() + margin, rect.bottom() + margin,
                                  rect.left() + margin, rect.bottom() + margin, pen);
        }
    } else if (m_tooltipType == "clean") {
        QScreen *screen = QGuiApplication::primaryScreen();
        std::for_each(info.desktopsData.cbegin(), info.desktopsData.cend(),
                      [&toolTipScene, &screen](const WindowData &data) {
                          QPixmap desktop = screen->grabWindow(data.id);
                          toolTipScene->addPixmap(desktop)->setOffset(data.rect.left(),
                                                                      data.rect.top());
                      });
    } else if (m_tooltipType == "windows") {
        QScreen *screen = QGuiApplication::primaryScreen();
        std::for_each(info.desktopsData.cbegin(), info.desktopsData.cend(),
                      [&toolTipScene, &screen](const WindowData &data) {
                          QPixmap desktop = screen->grabWindow(data.id);
                          toolTipScene->addPixmap(desktop)->setOffset(data.rect.left(),
                                                                      data.rect.top());
                      });
        std::for_each(info.windowsData.cbegin(), info.windowsData.cend(),
                      [&toolTipScene, &screen](const WindowData &data) {
                          QPixmap window = screen->grabWindow(data.id);
                          toolTipScene->addPixmap(window)->setOffset(data.rect.left(),
                                                                     data.rect.top());
                      });
    }

    QPixmap image = toolTipView->grab().scaledToWidth(m_tooltipWidth);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    image.save(&buffer, "PNG");

    delete toolTipView;
    delete toolTipScene;

    return QString("<img src=\"data:image/png;base64,%1\"/>").arg(QString(byteArray.toBase64()));
}


QString DPAdds::parsePattern(const QString &_pattern, const int _desktop) const
{
    qCDebug(LOG_DP) << "Pattern" << _pattern << "for desktop" << _desktop;

    QString parsed = _pattern;
    parsed.replace("$$", "$\\$\\");
    for (auto &key : dictKeys())
        parsed.replace(QString("$%1").arg(key), valueByKey(key, _desktop));
    parsed.replace("$\\$\\", "$$");

    return parsed;
}


void DPAdds::setMark(const QString &_newMark)
{
    qCDebug(LOG_DP) << "Mark" << _newMark;

    m_mark = _newMark;
}


void DPAdds::setToolTipData(const QVariantMap &_tooltipData)
{
    qCDebug(LOG_DP) << "Data" << _tooltipData;

    m_tooltipColor = _tooltipData["tooltipColor"].toString();
    m_tooltipType = _tooltipData["tooltipType"].toString();
    m_tooltipWidth = _tooltipData["tooltipWidth"].toInt();
}


QString DPAdds::infoByKey(const QString &_key) const
{
    qCDebug(LOG_AW) << "Requested info for key" << _key;

    return "(none)";
}


QString DPAdds::valueByKey(const QString &_key, int _desktop) const
{
    qCDebug(LOG_DP) << "Requested key" << _key << "for desktop" << _desktop;
    if (_desktop == -1)
        _desktop = currentDesktop();

    QString currentMark = currentDesktop() == _desktop ? m_mark : "";
    if (_key == "mark")
        return QString("%1")
            .arg(currentMark, m_mark.count(), QLatin1Char(' '))
            .replace(" ", "&nbsp;");
    else if (_key == "name")
        return KWindowSystem::desktopName(_desktop).replace(" ", "&nbsp;");
    else if (_key == "number")
        return QString::number(_desktop);
    else if (_key == "total")
        return QString::number(numberOfDesktops());
    else
        return "";
}


// HACK: this method uses variables from version.h
QString DPAdds::getAboutText(const QString &_type) const
{
    qCDebug(LOG_DP) << "Type" << _type;

    return AWDebug::getAboutText(_type);
}


QVariantMap DPAdds::getFont(const QVariantMap &_defaultFont) const
{
    qCDebug(LOG_DP) << "Default font is" << _defaultFont;

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
void DPAdds::sendNotification(const QString &_eventId, const QString &_message)
{
    qCDebug(LOG_DP) << "Event" << _eventId << "with message" << _message;

    KNotification *notification
        = KNotification::event(_eventId, QString("Desktop Panel ::: %1").arg(_eventId), _message);
    notification->setComponentName("plasma-applet-org.kde.plasma.desktop-panel");
}


// slot for mouse click
void DPAdds::setCurrentDesktop(const int _desktop) const
{
    qCDebug(LOG_DP) << "Desktop" << _desktop;

    KWindowSystem::setCurrentDesktop(_desktop);
}


DPAdds::DesktopWindowsInfo DPAdds::getInfoByDesktop(const int _desktop) const
{
    qCDebug(LOG_DP) << "Desktop" << _desktop;


    DesktopWindowsInfo info;
    info.desktop = KWindowSystem::workArea(_desktop);

    for (auto &id : KWindowSystem::windows()) {
        KWindowInfo winInfo = KWindowInfo(
            id, NET::Property::WMDesktop | NET::Property::WMGeometry | NET::Property::WMState
                    | NET::Property::WMWindowType | NET::Property::WMVisibleName);
        if (!winInfo.isOnDesktop(_desktop))
            continue;
        WindowData data;
        data.id = id;
        data.name = winInfo.visibleName();
        data.rect = winInfo.geometry();
        if (winInfo.windowType(NET::WindowTypeMask::NormalMask) == NET::WindowType::Normal) {
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
