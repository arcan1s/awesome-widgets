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


#ifndef AWKEYS_H
#define AWKEYS_H

#include <QMap>
#include <QObject>
#include <QStringList>
#include <QVariant>


class ExtScript;
class GraphicalItem;

class AWKeys : public QObject
{
    Q_OBJECT

public:
    AWKeys(QObject *parent = 0);
    ~AWKeys();

    Q_INVOKABLE void initKeys(const QString pattern);
    Q_INVOKABLE void initValues();
    Q_INVOKABLE bool isDebugEnabled();
    Q_INVOKABLE QString networkDevice(const QString custom = QString(""));
    Q_INVOKABLE int numberCpus();
    Q_INVOKABLE QString parsePattern(const QString pattern, const QMap<QString, QVariant> values);
    Q_INVOKABLE QStringList sourcesForDataEngine(const QString pattern,
                                                 const QString dataEngine = QString("systemmonitor"));
    Q_INVOKABLE float temperature(const float temp, const QString units = QString("Celsius"));
    // keys
    Q_INVOKABLE QMap<QString, QVariant> counts();
    Q_INVOKABLE QStringList dictKeys();
    Q_INVOKABLE QStringList extScriptsInfo();
    Q_INVOKABLE QStringList graphicalItemsInfo();
    Q_INVOKABLE QStringList timeKeys();
    Q_INVOKABLE QStringList findGraphicalItems(const QString pattern);
    Q_INVOKABLE QStringList findKeys(const QString pattern);

private:
    QList<ExtScript *> getExtScripts();
    QList<GraphicalItem *> getGraphicalItems();
    GraphicalItem *getItemByTag(const QString tag);
    // variables
    bool debug = false;
    QList<GraphicalItem *> graphicalItems;
    QList<ExtScript *> extScripts;
    QStringList foundBars, foundKeys, keys;
};


#endif /* AWKEYS_H */
