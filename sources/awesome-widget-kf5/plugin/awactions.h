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


#ifndef AWACTIONS_H
#define AWACTIONS_H

#include <QMap>
#include <QObject>
#include <QVariant>


class QNetworkReply;

class AWActions : public QObject
{
    Q_OBJECT

public:
    AWActions(QObject *parent = 0);
    ~AWActions();

    Q_INVOKABLE void checkUpdates();
    Q_INVOKABLE void runCmd(const QString cmd = QString("/usr/bin/true"));
    Q_INVOKABLE void sendNotification(const QString eventId, const QString message);
    Q_INVOKABLE void showReadme();
    // dataengine
    Q_INVOKABLE QMap<QString, QVariant> readDataEngineConfiguration();
    Q_INVOKABLE void writeDataEngineConfiguration(const QMap<QString, QVariant> configuration);

private slots:
    void showUpdates(QString version);
    void versionReplyRecieved(QNetworkReply *reply);

private:
    QMap<QString, QVariant> updateDataEngineConfiguration(QMap<QString, QVariant> rawConfig);
    // variables
    bool debug = false;
};


#endif /* AWACTIONS_H */
