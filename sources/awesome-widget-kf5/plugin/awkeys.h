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

#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QMap>
#include <QObject>
#include <QStringList>
#include <QPixmap>
#include <QPushButton>
#include <QVariant>


class AWToolTip;
class ExtScript;
class ExtUpgrade;
class GraphicalItem;

class AWKeys : public QObject
{
    Q_OBJECT

    enum RequestedItem {
        Nothing,
        RequestedGraphicalItem,
        RequestedExtScript,
        RequestedExtUpgrade
    };

public:
    AWKeys(QObject *parent = 0);
    ~AWKeys();

    Q_INVOKABLE bool checkKeys(const QMap<QString, QVariant> data);
    Q_INVOKABLE void initKeys(const QString pattern,
                              const QMap<QString, QVariant> tooltipParams);
    Q_INVOKABLE bool isDebugEnabled();
    Q_INVOKABLE QString parsePattern(const QString pattern);
    Q_INVOKABLE QPixmap toolTipImage();
    // keys
    Q_INVOKABLE bool addDevice(const QString source);
    Q_INVOKABLE QStringList dictKeys();
    Q_INVOKABLE QStringList getDiskDevices();
    Q_INVOKABLE QStringList getFanDevices();
    Q_INVOKABLE QStringList getHddDevices(const bool needAbstract = false);
    Q_INVOKABLE QStringList getMountDevices();
    Q_INVOKABLE QStringList getNetworkDevices();
    Q_INVOKABLE QStringList getTempDevices();
    Q_INVOKABLE bool setDataBySource(const QString sourceName,
                                     const QMap<QString, QVariant> data,
                                     const QMap<QString, QVariant> params);
    // values
    Q_INVOKABLE QString graphicalValueByKey();
    Q_INVOKABLE QString valueByKey(QString key);
    // configuration
    Q_INVOKABLE void editItem(const QString type);

private slots:
    void editItemButtonPressed(QAbstractButton *button);
    void copyBar(const QString original);
    void copyScript(const QString original);
    void copyUpgrade(const QString original);

private:
    // methods
    QString networkDevice();
    int numberCpus();
    float temperature(const float temp, const QString units = QString("Celsius"));
    // find methods
    QStringList findGraphicalItems(const QString pattern);
    QStringList findKeys(const QString pattern);
    // get methods
    QList<ExtScript *> getExtScripts();
    QList<ExtUpgrade *> getExtUpgrade();
    QList<GraphicalItem *> getGraphicalItems();
    GraphicalItem *getItemByTag(const QString tag);
    QStringList getTimeKeys();
    AWToolTip *toolTip = nullptr;
    // graphical elements
    QDialog *dialog = nullptr;
    QListWidget *widgetDialog = nullptr;
    QDialogButtonBox *dialogButtons = nullptr;
    QPushButton *copyButton = nullptr;
    QPushButton *createButton = nullptr;
    RequestedItem requestedItem = Nothing;
    // variables
    bool debug = false;
    bool ready = false;
    QList<GraphicalItem *> graphicalItems;
    QList<ExtScript *> extScripts;
    QList<ExtUpgrade *> extUpgrade;
    QStringList foundBars, foundKeys, keys;
    QMap<QString, QString> values;
    QStringList diskDevices, fanDevices, mountDevices, tempDevices;
};


#endif /* AWKEYS_H */
