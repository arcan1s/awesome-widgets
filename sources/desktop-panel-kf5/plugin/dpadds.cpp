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
#include <Plasma/Corona>

#include <QDebug>
#include <QMessageBox>
#include <QProcessEnvironment>

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


QStringList DPAdds::dictKeys()
{
    if (debug) qDebug() << PDEBUG;

    QStringList allKeys;
    allKeys.append(QString("fullmark"));
    allKeys.append(QString("mark"));
    allKeys.append(QString("name"));
    allKeys.append(QString("number"));
    allKeys.append(QString("total"));

    return allKeys;
}


QString DPAdds::parsePattern(const QString pattern, const int desktop)
{
    if (debug) qDebug() << PDEBUG;

    QString parsed = pattern;
    QStringList keys = dictKeys();
    parsed.replace(QString(" "), QString("&nbsp;"));
    parsed.replace(QString("$$"), QString("$\\$\\"));
    for (int i=0; i<keys.count(); i++)
        parsed.replace(QString("$") + keys[i], valueByKey(keys[i], desktop));
    parsed.replace(QString("$\\$\\"), QString("$$"));

    return parsed;
}


QString DPAdds::valueByKey(const QString key, const int desktop)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Requested key" << key;

    QString currentMark;
    if (KWindowSystem::currentDesktop() == desktop)
        currentMark = mark;
    else
        currentMark = QString("");
    if (key == QString("fullmark"))
        return QString("%1").arg(currentMark, currentMark.count(), QLatin1Char(' '));
    else if (key == QString("mark"))
        return currentMark;
    else if (key == QString("name"))
        return KWindowSystem::desktopName(desktop);
    else if (key == QString("number"))
        return QString::number(desktop);
    else if (key == QString("total"))
        return QString::number(KWindowSystem::numberOfDesktops());
    else
        return QString();
}


QString DPAdds::getAboutText(const QString type)
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


QMap<QString, QVariant> DPAdds::getFont(const QMap<QString, QVariant> defaultFont)
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


void DPAdds::changePanelsState()
{
    if (debug) qDebug() << PDEBUG;
    if (panelsToControl == QString("-1")) return;

    QList<Plasma::Containment *> panels = getPanels();
//     for (int i=0; i<panels.count(); i++) {
//         if (!panelsToControl.split(QChar(',')).contains(QString::number(i))) continue;
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
    panels.clear();
}


void DPAdds::sendNotification(const QString eventId, const QString message)
{
    KNotification *notification = KNotification::event(eventId, QString("Desktop Panel ::: ") + eventId, message);
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


QList<Plasma::Containment *> DPAdds::getPanels()
{
    if (debug) qDebug() << PDEBUG;

    Plasma::Containment *containment = new Plasma::Containment(this, QVariantList());
    QList<Plasma::Containment *> panels;
    for (int i=0; i<containment->corona()->containments().count(); i++)
        if (containment->corona()->containments()[i]->containmentType() ==
            Plasma::Types::ContainmentType::PanelContainment)
            panels.append(containment->corona()->containments()[i]);
    delete containment;

    return panels;
}


QString DPAdds::panelLocationToStr(Plasma::Types::Location location)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Location" << location;

    switch(location) {
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
