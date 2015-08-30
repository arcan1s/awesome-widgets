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
    explicit DPAdds(QObject *parent = nullptr);
    virtual ~DPAdds();

    Q_INVOKABLE bool isDebugEnabled() const;
    Q_INVOKABLE int currentDesktop() const;
    Q_INVOKABLE QStringList dictKeys() const;
    Q_INVOKABLE int numberOfDesktops() const;
    Q_INVOKABLE QString toolTipImage(const int desktop) const;
    Q_INVOKABLE QString parsePattern(const QString pattern, const int desktop) const;
    // values
    Q_INVOKABLE void setMark(const QString newMark);
    Q_INVOKABLE void setPanelsToControl(const QString newPanels);
    Q_INVOKABLE void setToolTipData(const QVariantMap tooltipData);
    Q_INVOKABLE QString valueByKey(const QString key, int desktop = -1) const;
    // configuration slots
    Q_INVOKABLE QString editPanelsToContol(const QString current);
    Q_INVOKABLE QString getAboutText(const QString type = "header") const;
    Q_INVOKABLE QVariantMap getFont(const QVariantMap defaultFont) const;

signals:
    void desktopChanged() const;
    void windowListChanged() const;

public slots:
    Q_INVOKABLE void changePanelsState() const;
    Q_INVOKABLE static void sendNotification(const QString eventId, const QString message);
    Q_INVOKABLE void setCurrentDesktop(const int desktop) const;

private:
    DesktopWindowsInfo getInfoByDesktop(const int desktop) const;
    QList<Plasma::Containment *> getPanels() const;
    QString panelLocationToStr(Plasma::Types::Location location) const;
    // variables
    int oldState, tooltipWidth = 200;
    QString mark = QString("*");
    QString tooltipColor = QString("#000000");
    QString tooltipType = QString("none");
    QList<int> panelsToControl;
};


#endif /* DPADDS_H */
