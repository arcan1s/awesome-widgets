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


#ifndef DPADDS_H
#define DPADDS_H

#include <Plasma/Containment>

#include <QMap>
#include <QObject>
#include <QVariant>


class DPAdds : public QObject
{
    Q_OBJECT

    typedef struct {
        WId id;
        QString name;
        QRect rect;
    } WindowData;

    typedef struct {
        QRect desktop;
        QList<WindowData> desktopsData;
        QList<WindowData> windowsData;
    } DesktopWindowsInfo;

public:
    DPAdds(QObject *parent = nullptr);
    ~DPAdds();

    Q_INVOKABLE bool isDebugEnabled();
    Q_INVOKABLE int currentDesktop();
    Q_INVOKABLE QStringList dictKeys();
    Q_INVOKABLE int numberOfDesktops();
    Q_INVOKABLE QString toolTipImage(const int desktop);
    Q_INVOKABLE QString parsePattern(const QString pattern, const int desktop);
    // values
    Q_INVOKABLE void setMark(const QString newMark);
    Q_INVOKABLE void setPanelsToControl(const QString newPanels);
    Q_INVOKABLE void setToolTipData(const QVariantMap tooltipData);
    Q_INVOKABLE QString valueByKey(const QString key, int desktop = -1);
    // configuration slots
    Q_INVOKABLE QString editPanelsToContol(const QString current);
    Q_INVOKABLE QString getAboutText(const QString type = "header");
    Q_INVOKABLE QVariantMap getFont(const QVariantMap defaultFont);

signals:
    void desktopChanged();
    void windowListChanged();

public slots:
    Q_INVOKABLE void changePanelsState();
    Q_INVOKABLE static void sendNotification(const QString eventId, const QString message);
    Q_INVOKABLE void setCurrentDesktop(const int desktop);

private slots:
    void changeDesktop(const int desktop);
    void changeWindowList(const WId window);

private:
    DesktopWindowsInfo getInfoByDesktop(const int desktop);
    QList<Plasma::Containment *> getPanels();
    QString panelLocationToStr(Plasma::Types::Location location);
    // variables
    bool debug = false;
    int oldState, tooltipWidth = 200;
    QString mark = QString("*");
    QString tooltipColor = QString("#000000");
    QString tooltipType = QString("none");
    QList<int> panelsToControl;
};


#endif /* DPADDS_H */
