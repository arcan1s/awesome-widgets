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
#include <taskmanager/virtualdesktopinfo.h>
#include <taskmanager/waylandtasksmodel.h>
#include <taskmanager/windowtasksmodel.h>
#include <taskmanager/xwindowtasksmodel.h>

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

    m_vdi = new TaskManager::VirtualDesktopInfo(this);
    m_taskModel = new TaskManager::WindowTasksModel(this);

    connect(m_vdi, &TaskManager::VirtualDesktopInfo::currentDesktopChanged, this, &DPAdds::desktopChanged);
}


DPAdds::~DPAdds()
{
    qCDebug(LOG_DP) << __PRETTY_FUNCTION__;

    m_vdi->deleteLater();
    m_taskModel->deleteLater();
}


int DPAdds::currentDesktop() const
{
    auto current = m_vdi->currentDesktop();
    auto decrement = KWindowSystem::isPlatformX11() ? 1 : 0;
    return m_vdi->position(current) - decrement;
}


QStringList DPAdds::dictKeys(const bool _sorted, const QString &_regexp)
{
    qCDebug(LOG_DP) << "Should be sorted" << _sorted << "and filter applied" << _regexp;

    QStringList allKeys;
    allKeys.append("mark");
    allKeys.append("name");
    allKeys.append("number");
    allKeys.append("total");

    if (_sorted)
        allKeys.sort();

    return allKeys.filter(QRegularExpression(_regexp));
}


int DPAdds::numberOfDesktops() const
{
    return m_vdi->numberOfDesktops();
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
                      [&windowList](const WindowData &data) { windowList.append(data.name); });
        return QString("<ul><li>%1</li></ul>").arg(windowList.join("</li><li>"));
    }

    // init
    auto *toolTipScene = new QGraphicsScene();
    auto *toolTipView = new QGraphicsView(toolTipScene);
    toolTipView->setStyleSheet("background: transparent");
    toolTipView->setContentsMargins(0, 0, 0, 0);
    toolTipView->setFrameShape(QFrame::NoFrame);
    toolTipView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    toolTipView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto screens = QGuiApplication::screens();
    auto desktop
        = std::accumulate(screens.cbegin(), screens.cend(), QRect(0, 0, 0, 0), [](QRect source, const QScreen *screen) {
              return source.united(screen->availableGeometry());
          });

    // update
    auto width = static_cast<float>(desktop.width());
    auto height = static_cast<float>(desktop.height());
    float margin = 5.0f * width / 400.0f;
    toolTipView->resize(static_cast<int>(width + 2.0f * margin), static_cast<int>(height + 2.0f * margin));
    toolTipScene->clear();
    toolTipScene->setBackgroundBrush(QBrush(Qt::NoBrush));
    // borders
    toolTipScene->addLine(0, 0, 0, height + 2.0 * margin);
    toolTipScene->addLine(0, height + 2.0 * margin, width + 2.0 * margin, height + 2.0 * margin);
    toolTipScene->addLine(width + 2.0 * margin, height + 2.0 * margin, width + 2.0 * margin, 0);
    toolTipScene->addLine(width + 2.0 * margin, 0, 0, 0);

    // with wayland countours only are supported
    QPen pen = QPen();
    pen.setWidthF(2.0 * width / 400.0);
    pen.setColor(QColor(m_tooltipColor));
    for (auto &data : info.windowsData) {
        QRect rect = data.rect;
        auto left = static_cast<float>(rect.left());
        auto right = static_cast<float>(rect.right());
        auto top = static_cast<float>(rect.top());
        auto bottom = static_cast<float>(rect.bottom());
        toolTipScene->addLine(left + margin, bottom + margin, left + margin, top + margin, pen);
        toolTipScene->addLine(left + margin, top + margin, right + margin, top + margin, pen);
        toolTipScene->addLine(right + margin, top + margin, right + margin, bottom + margin, pen);
        toolTipScene->addLine(right + margin, bottom + margin, left + margin, bottom + margin, pen);
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


QString DPAdds::infoByKey(const QString &_key)
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
        return QString("%1").arg(currentMark, m_mark.size(), QLatin1Char(' ')).replace(" ", "&nbsp;");
    else if (_key == "name") {
        auto name = m_vdi->desktopNames().at(_desktop);
        return name.replace(" ", "&nbsp;");
    } else if (_key == "number")
        return QString::number(_desktop + 1);
    else if (_key == "total")
        return QString::number(numberOfDesktops());
    else
        return "";
}


// HACK: this method uses variables from version.h
QString DPAdds::getAboutText(const QString &_type)
{
    qCDebug(LOG_DP) << "Type" << _type;

    return AWDebug::getAboutText(_type);
}


QVariantMap DPAdds::getFont(const QVariantMap &_defaultFont)
{
    qCDebug(LOG_DP) << "Default font is" << _defaultFont;

    QVariantMap fontMap;
    int ret = 0;
    CFont defaultCFont = CFont(_defaultFont["family"].toString(), _defaultFont["size"].toInt(), 400, false,
                               _defaultFont["color"].toString());
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
void DPAdds::setCurrentDesktop(const int _desktop)
{
    qCDebug(LOG_DP) << "Desktop" << _desktop;

    m_vdi->requestActivate(m_vdi->desktopIds().at(_desktop));
}


DPAdds::DesktopWindowsInfo DPAdds::getInfoByDesktop(const int _desktop) const
{
    qCDebug(LOG_DP) << "Desktop" << _desktop;

    auto desktop = m_vdi->desktopIds().at(_desktop);

    DesktopWindowsInfo info;
    for (auto i = 0; i < m_taskModel->rowCount(); i++) {
        auto model = m_taskModel->index(i, 0);
        WindowData data;

        data.name = model.data(TaskManager::AbstractTasksModel::AppName).toString();
        data.rect = model.data(TaskManager::AbstractTasksModel::Geometry).toRect();

        auto desktops = model.data(TaskManager::AbstractTasksModel::VirtualDesktops).toList();
        if (desktops.isEmpty()) {
            // don't think it is possible to put desktop to desktop
            info.desktopsData.append(data);
        } else {
            auto isHidden = model.data(TaskManager::AbstractTasksModel::IsHidden).toBool();
            auto isMinimized = model.data(TaskManager::AbstractTasksModel::IsMinimized).toBool();
            if (isHidden || isMinimized || !desktops.contains(desktop))
                continue;
            info.windowsData.append(data);
        }
    }

    return info;
}
