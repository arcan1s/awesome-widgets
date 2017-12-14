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

// ui library required by WId definition
#include <QGuiApplication>
#include <QRect>


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
    explicit DPAdds(QObject *_parent = nullptr);
    virtual ~DPAdds();
    Q_INVOKABLE bool isDebugEnabled() const;
    Q_INVOKABLE int currentDesktop() const;
    Q_INVOKABLE QStringList dictKeys(const bool _sorted = true, const QString &_regexp = "") const;
    Q_INVOKABLE int numberOfDesktops() const;
    Q_INVOKABLE QString toolTipImage(const int _desktop) const;
    Q_INVOKABLE QString parsePattern(const QString &_pattern, const int _desktop) const;
    // values
    Q_INVOKABLE void setMark(const QString &_newMark);
    Q_INVOKABLE void setToolTipData(const QVariantMap &_tooltipData);
    Q_INVOKABLE QString infoByKey(const QString &_key) const;
    Q_INVOKABLE QString valueByKey(const QString &_key, int _desktop = -1) const;
    // configuration slots
    Q_INVOKABLE QString getAboutText(const QString &_type) const;
    Q_INVOKABLE QVariantMap getFont(const QVariantMap &_defaultFont) const;

signals:
    void desktopChanged() const;
    void windowListChanged() const;

public slots:
    Q_INVOKABLE static void sendNotification(const QString &_eventId, const QString &_message);
    Q_INVOKABLE void setCurrentDesktop(const int _desktop) const;

private:
    DesktopWindowsInfo getInfoByDesktop(const int _desktop) const;
    // variables
    int m_tooltipWidth = 200;
    QString m_mark = "*";
    QString m_tooltipColor = "#000000";
    QString m_tooltipType = "none";
};


#endif /* DPADDS_H */
