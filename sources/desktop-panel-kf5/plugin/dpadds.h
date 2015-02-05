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

public:
    DPAdds(QObject *parent = 0);
    ~DPAdds();

    Q_INVOKABLE bool isDebugEnabled();
    Q_INVOKABLE QStringList dictKeys();
    Q_INVOKABLE QString parsePattern(const QString pattern, const int desktop);
    // values
    Q_INVOKABLE QString valueByKey(const QString key, const int desktop);
    // configuration slots
    Q_INVOKABLE QString getAboutText(const QString type = "header");
    Q_INVOKABLE QMap<QString, QVariant> getFont(const QMap<QString, QVariant> defaultFont);

signals:
    void desktopChanged();

public slots:
    Q_INVOKABLE void changePanelsState();
    Q_INVOKABLE static void sendNotification(const QString eventId, const QString message);
    Q_INVOKABLE void setCurrentDesktop(const int desktop);

private slots:
    void changeDesktop(const int desktop);

private:
    QList<Plasma::Containment *> getPanels();
    QString panelLocationToStr(Plasma::Types::Location location);
    // variables
    bool debug = false;
    int oldState;
    QString mark, panelsToControl;
};


#endif /* DPADDS_H */
