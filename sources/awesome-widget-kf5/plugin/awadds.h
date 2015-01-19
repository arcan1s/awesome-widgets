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


#ifndef AWADDS_H
#define AWADDS_H

#include <QMap>
#include <QObject>
#include <QStringList>
#include <QVariant>


class ExtScript;
class GraphicalItem;
class QNetworkReply;

class AWAdds : public QObject
{
    Q_OBJECT

public:
    AWAdds(QObject *parent = 0);
    ~AWAdds();

    Q_INVOKABLE void checkUpdates();
    Q_INVOKABLE void initValues();
    Q_INVOKABLE bool isDebugEnabled();
    Q_INVOKABLE QString networkDevice(const QString custom = QString(""));
    Q_INVOKABLE int numberCpus();
    Q_INVOKABLE QString parsePattern(const QString pattern, const QMap<QString, QVariant> dict,
                                     const QMap<QString, QVariant> values,
                                     const QStringList foundKeys, const QStringList foundBars);
    Q_INVOKABLE float tempepature(const float temp, const QString units = QString("Celsius"));
    // keys
    Q_INVOKABLE QMap<QString, QVariant> counts();
    Q_INVOKABLE QStringList dictKeys();
//    Q_INVOKABLE QStringList graphicalItemsNames();
    Q_INVOKABLE QStringList timeKeys();
    Q_INVOKABLE QStringList findGraphicalItems(const QString pattern);
    Q_INVOKABLE QStringList findKeys(const QString pattern);
    // actions
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
    QList<ExtScript *> getExtScripts();
    QList<GraphicalItem *> getGraphicalItems();
    GraphicalItem *getItemByTag(const QString tag);
    QMap<QString, QVariant> updateDataEngineConfiguration(QMap<QString, QVariant> rawConfig);
    // variables
    bool debug = false;
    QList<GraphicalItem *> graphicalItems;
    QList<ExtScript *> extScripts;
    QStringList keys;
};


#endif /* AWADDS_H */
