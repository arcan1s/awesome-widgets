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
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QPixmap>
#include <QProcessEnvironment>
#include <QScreen>

#include <fontdialog/fontdialog.h>
#include <pdebug/pdebug.h>

#include "version.h"


DPAdds::DPAdds(QObject *parent)
    : QObject(parent)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));

    connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)), this, SLOT(changeDesktop(int)));
    connect(KWindowSystem::self(), SIGNAL(windowAdded(WId)), this, SLOT(changeWindowList(WId)));
    connect(KWindowSystem::self(), SIGNAL(windowRemoved(WId)), this, SLOT(changeWindowList(WId)));
}


DPAdds::~DPAdds()
{
    if (debug) qDebug() << PDEBUG;
}


bool DPAdds::isDebugEnabled()
{
    if (debug) qDebug() << PDEBUG;

    return debug;
}


int DPAdds::currentDesktop()
{
    if (debug) qDebug() << PDEBUG;

    return KWindowSystem::currentDesktop();
}


QStringList DPAdds::dictKeys()
{
    if (debug) qDebug() << PDEBUG;

    QStringList allKeys;
    allKeys.append(QString("mark"));
    allKeys.append(QString("name"));
    allKeys.append(QString("number"));
    allKeys.append(QString("total"));

    return allKeys;
}


int DPAdds::numberOfDesktops()
{
    if (debug) qDebug() << PDEBUG;

    return KWindowSystem::numberOfDesktops();
}


QString DPAdds::toolTipImage(const int desktop)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Desktop" << desktop;
    if (tooltipType == QString("none")) return QString();

    // prepare
    DesktopWindowsInfo info = getInfoByDesktop(desktop);
    if (tooltipType == QString("names")) {
        QStringList windowList;
        for (int i=0; i<info.windowsData.count(); i++)
            windowList.append(info.windowsData[i].name);
        return QString("<ul><li>%1</li></ul>").arg(windowList.join(QString("</li><li>")));
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
    toolTipView->resize(info.desktop.width() + 2.0 * margin, info.desktop.height() + 2.0 * margin);
    toolTipScene->clear();
    toolTipScene->setBackgroundBrush(QBrush(Qt::NoBrush));
    // borders
    toolTipScene->addLine(0, 0, 0, info.desktop.height() + 2.0 * margin);
    toolTipScene->addLine(0, info.desktop.height() + 2.0 * margin,
                          info.desktop.width() + 2.0 * margin, info.desktop.height() + 2.0 * margin);
    toolTipScene->addLine(info.desktop.width() + 2.0 * margin, info.desktop.height() + 2.0 * margin,
                          info.desktop.width() + 2.0 * margin, 0);
    toolTipScene->addLine(info.desktop.width() + 2.0 * margin, 0, 0, 0);

    if (tooltipType == QString("contours")) {
        QPen pen = QPen();
        pen.setWidthF(2.0 * info.desktop.width() / 400.0);
        pen.setColor(QColor(tooltipColor));
        for (int i=0; i<info.windowsData.count(); i++) {
            QRect rect = info.windowsData[i].rect;
            toolTipScene->addLine(rect.left() + margin, rect.bottom() + margin,
                                  rect.left() + margin, rect.top() + margin, pen);
            toolTipScene->addLine(rect.left() + margin, rect.top() + margin,
                                  rect.right() + margin, rect.top() + margin, pen);
            toolTipScene->addLine(rect.right() + margin, rect.top() + margin,
                                  rect.right() + margin, rect.bottom() + margin, pen);
            toolTipScene->addLine(rect.right() + margin, rect.bottom() + margin,
                                  rect.left() + margin, rect.bottom() + margin, pen);
        }
    } else if (tooltipType == QString("clean")) {
        QScreen *screen = QGuiApplication::primaryScreen();
        for (int i=0; i<info.desktopsData.count(); i++) {
            QPixmap desktop = screen->grabWindow(info.desktopsData[i].id);
            toolTipScene->addPixmap(desktop)
                        ->setOffset(info.desktopsData[i].rect.left(), info.desktopsData[i].rect.top());
        }
    } else if (tooltipType == QString("windows")) {
        QScreen *screen = QGuiApplication::primaryScreen();
        for (int i=0; i<info.desktopsData.count(); i++) {
            QPixmap desktop = screen->grabWindow(info.desktopsData[i].id);
            toolTipScene->addPixmap(desktop)
                        ->setOffset(info.desktopsData[i].rect.left(), info.desktopsData[i].rect.top());
        }
        for (int i=0; i<info.windowsData.count(); i++) {
            QPixmap window = screen->grabWindow(info.windowsData[i].id);
            toolTipScene->addPixmap(window)
                        ->setOffset(info.windowsData[i].rect.left(), info.windowsData[i].rect.top());
        }
    }

    QPixmap image = toolTipView->grab().scaledToWidth(tooltipWidth);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    image.save(&buffer, "PNG");

    delete toolTipView;
    delete toolTipScene;

    return QString("<img src=\"data:image/png;base64,%1\"/>").arg(QString(byteArray.toBase64()));
}


QString DPAdds::parsePattern(const QString pattern, const int desktop)
{
    if (debug) qDebug() << PDEBUG;

    QString parsed = pattern;
    QStringList keys = dictKeys();
    parsed.replace(QString("$$"), QString("$\\$\\"));
    for (int i=0; i<keys.count(); i++)
        parsed.replace(QString("$%1").arg(keys[i]), valueByKey(keys[i], desktop));
    parsed.replace(QString("$\\$\\"), QString("$$"));

    return parsed;
}


void DPAdds::setMark(const QString newMark)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Mark" << newMark;

    mark = newMark;
}


void DPAdds::setPanelsToControl(const QString newPanels)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Panels" << newPanels;

    panelsToControl.clear();
    if (newPanels == QString("-1")) {
        int count = getPanels().count();
        for (int i=0; i<count; i++)
          panelsToControl.append(i);
    } else
        for (int i=0; i<newPanels.split(QChar(',')).count(); i++)
            panelsToControl.append(newPanels.split(QChar(','))[i].toInt());
}


void DPAdds::setToolTipData(const QVariantMap tooltipData)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Data" << tooltipData;

    tooltipColor = tooltipData[QString("tooltipColor")].toString();
    tooltipType = tooltipData[QString("tooltipType")].toString();
    tooltipWidth = tooltipData[QString("tooltipWidth")].toInt();
}


QString DPAdds::valueByKey(const QString key, int desktop)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Requested key" << key;
    if (desktop == -1) desktop = currentDesktop();

    QString currentMark = currentDesktop() == desktop ? mark : QString("");
    if (key == QString("mark"))
        return QString("%1").arg(currentMark, mark.count(), QLatin1Char(' '))
                            .replace(QString(" "), QString("&nbsp;"));
    else if (key == QString("name"))
        return KWindowSystem::desktopName(desktop).replace(QString(" "), QString("&nbsp;"));
    else if (key == QString("number"))
        return QString::number(desktop);
    else if (key == QString("total"))
        return QString::number(numberOfDesktops());
    else
        return QString();
}


QString DPAdds::editPanelsToContol(const QString current)
{
    if (debug) qDebug() << PDEBUG;

    // paint
    QDialog *dialog = new QDialog(nullptr);
    QListWidget *widget = new QListWidget(dialog);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Close,
                                                     Qt::Vertical, dialog);
    QHBoxLayout *layout = new QHBoxLayout(dialog);
    layout->addWidget(widget);
    layout->addWidget(buttons);
    dialog->setLayout(layout);
    connect(buttons, SIGNAL(rejected()), dialog, SLOT(reject()));
    connect(buttons, SIGNAL(accepted()), dialog, SLOT(accept()));

    // fill
    QList<Plasma::Containment *> panels = getPanels();
    for (int i=0; i<panels.count(); i++) {
        QListWidgetItem *item = new QListWidgetItem(panelLocationToStr(panels[i]->location()), widget);
        if ((current.split(QChar(',')).contains(QString::number(i))) ||
            (current == QString("-1")))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
    }

    // exec
    QString value;
    QStringList indexes;
    int ret = dialog->exec();
    switch (ret) {
    case QDialog::Accepted:
        for (int i=0; i<widget->count(); i++)
            if (widget->item(i)->checkState() == Qt::Checked)
                indexes.append(QString::number(i));
        if (indexes.count() == widget->count())
            value = QString("-1");
        else
            value = indexes.join(QChar(','));
        break;
    default:
        value = current;
        break;
    }

    return value;
}


QString DPAdds::getAboutText(const QString type)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Type" << type;

    QString text;
    if (type == QString("header"))
        text = QString(NAME);
    else if (type == QString("version"))
        text = i18n("Version %1 (build date %2)", QString(VERSION), QString(BUILD_DATE));
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


QVariantMap DPAdds::getFont(const QVariantMap defaultFont)
{
    if (debug) qDebug() << PDEBUG;

    QVariantMap fontMap;
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


void DPAdds::changePanelsState()
{
    if (debug) qDebug() << PDEBUG;
    if (panelsToControl.isEmpty()) return;

//     QList<Plasma::Containment *> panels = getPanels();
//     for (int i=0; i<panels.count(); i++) {
//         if (!panelsToControl.contains(i)) continue;
//         bool wasVisible = panels[i]->view()->isVisible();
//         int winId = panels[i]->view()->winId();
//         if (wasVisible) {
//             if (debug) qDebug() << PDEBUG << ":" << "Hide panel";
//             KWindowInfo oldInfo = KWindowSystem::windowInfo(winId, NET::WMState);
//             oldState = oldInfo.state();
//             panels[i]->view()->setVisible(false);
//         } else {
//             if (debug) qDebug() << PDEBUG << ":" << "Show panel";
//             panels[i]->view()->setVisible(true);
//             KWindowSystem::clearState(winId, NET::KeepAbove);
//             KWindowSystem::setState(winId, oldState | NET::StaysOnTop);
//             KWindowSystem::setOnAllDesktops(winId, true);
//         }
//     }
//     panels.clear();
}


void DPAdds::sendNotification(const QString eventId, const QString message)
{
    KNotification *notification = KNotification::event(eventId, QString("Desktop Panel ::: %1").arg(eventId), message);
    notification->setComponentName(QString("plasma-applet-org.kde.plasma.desktop-panel"));
}


void DPAdds::setCurrentDesktop(const int desktop)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Desktop" << desktop;

    KWindowSystem::setCurrentDesktop(desktop);
}


void DPAdds::changeDesktop(const int desktop)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Desktop" << desktop;

    emit(desktopChanged());
}


void DPAdds::changeWindowList(const WId window)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Window" << window;

    emit(windowListChanged());
}


DPAdds::DesktopWindowsInfo DPAdds::getInfoByDesktop(const int desktop)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Desktop" << desktop;


    DesktopWindowsInfo info;
    info.desktop = KWindowSystem::workArea(desktop);

    QList<WId> windows = KWindowSystem::windows();
    for (int i=0; i<windows.count(); i++) {
        KWindowInfo winInfo = KWindowInfo(windows[i],
                                          NET::Property::WMDesktop | NET::Property::WMGeometry |
                                          NET::Property::WMState | NET::Property::WMWindowType |
                                          NET::Property::WMVisibleName);
        if (!winInfo.isOnDesktop(desktop)) continue;
        WindowData data;
        data.id = windows[i];
        data.name = winInfo.visibleName();
        data.rect = winInfo.geometry();
        if (winInfo.windowType(NET::WindowTypeMask::NormalMask) == NET::WindowType::Normal) {
            if (winInfo.isMinimized()) continue;
            info.windowsData.append(data);
        } else if (winInfo.windowType(NET::WindowTypeMask::DesktopMask) == NET::WindowType::Desktop) {
            info.desktopsData.append(data);
        }
    }

    return info;
}


QList<Plasma::Containment *> DPAdds::getPanels()
{
    if (debug) qDebug() << PDEBUG;

//     Plasma::Corona *corona = new Plasma::Corona(this);
    QList<Plasma::Containment *> panels;
//     for (int i=0; i<corona->containments().count(); i++)
//         if (corona->containments()[i]->containmentType() == Plasma::Types::ContainmentType::PanelContainment)
//             panels.append(corona->containments()[i]);
//     delete corona;

    return panels;
}


QString DPAdds::panelLocationToStr(Plasma::Types::Location location)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Location" << location;

    switch (location) {
    case Plasma::Types::Location::TopEdge:
        return i18n("Top Edge");
    case Plasma::Types::Location::BottomEdge:
        return i18n("Bottom Edge");
    case Plasma::Types::Location::LeftEdge:
        return i18n("Left Edge");
    case Plasma::Types::Location::RightEdge:
        return i18n("Right Edge");
    default:
        return i18n("Unknown location (%1)", location);
    }
}
