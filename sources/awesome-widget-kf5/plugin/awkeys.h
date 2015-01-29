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
#include <QPixmap>
#include <QVariant>


class AWToolTip;
class ExtScript;
class ExtUpgrade;
class GraphicalItem;

class AWKeys : public QObject
{
    Q_OBJECT

public:
    AWKeys(QObject *parent = 0);
    ~AWKeys();

    Q_INVOKABLE void initKeys(const QString pattern,
                              const QMap<QString, QVariant> params,
                              const QMap<QString, QVariant> tooltipParams);
    Q_INVOKABLE bool isDebugEnabled();
    Q_INVOKABLE bool isReady();
    Q_INVOKABLE QString parsePattern(const QString pattern);
    Q_INVOKABLE QPixmap toolTipImage();
    // keys
    Q_INVOKABLE QStringList dictKeys();
    Q_INVOKABLE QStringList extScriptsInfo();
    Q_INVOKABLE QStringList extUpgradeInfo();
    Q_INVOKABLE QStringList graphicalItemsInfo();
    Q_INVOKABLE void setDataBySource(const QString sourceName,
                                     const QMap<QString, QVariant> data,
                                     const QMap<QString, QVariant> params);
    // values
    Q_INVOKABLE QString valueByKey(QString key);

private:
    // methods
    QString networkDevice(const QString custom = QString(""));
    int numberCpus();
    float temperature(const float temp, const QString units = QString("Celsius"));
    // find methods
    QStringList findGraphicalItems(const QString pattern);
    QStringList findKeys(const QString pattern);
    // get methods
    QMap<QString, QVariant> getCounts(const QMap<QString, QVariant> params);
    QList<ExtScript *> getExtScripts();
    QList<ExtUpgrade *> getExtUpgrade();
    QList<GraphicalItem *> getGraphicalItems();
    GraphicalItem *getItemByTag(const QString tag);
    QStringList getTimeKeys();
    AWToolTip *toolTip = nullptr;
    // variables
    bool debug = false;
    bool ready = false;
    QList<GraphicalItem *> graphicalItems;
    QList<ExtScript *> extScripts;
    QList<ExtUpgrade *> extUpgrade;
    QStringList foundBars, foundKeys, keys;
    QMap<QString, QVariant> counts;
    QMap<QString, QString> values;
};


#endif /* AWKEYS_H */
