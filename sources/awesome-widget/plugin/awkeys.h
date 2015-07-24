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
#include <QPushButton>
#include <QVariant>


class AWToolTip;
class ExtQuotes;
class ExtScript;
class ExtUpgrade;
class ExtWeather;
class GraphicalItem;

class AWKeys : public QObject
{
    Q_OBJECT

    enum RequestedItem {
        Nothing,
        RequestedGraphicalItem,
        RequestedExtQuotes,
        RequestedExtScript,
        RequestedExtUpgrade,
        RequestedExtWeather
    };

public:
    AWKeys(QObject *parent = nullptr);
    ~AWKeys();

    Q_INVOKABLE void initKeys(const QString currentPattern);
    Q_INVOKABLE void initTooltip(const QVariantMap tooltipParams);
    Q_INVOKABLE void setPopupEnabled(const bool popup = false);
    Q_INVOKABLE void setWrapNewLines(const bool wrap = false);
    Q_INVOKABLE QString parsePattern() const;
    Q_INVOKABLE QSize toolTipSize() const;
    // keys
    Q_INVOKABLE void addDevice(const QString source);
    Q_INVOKABLE QStringList dictKeys(const bool sorted = false) const;
    Q_INVOKABLE QStringList getHddDevices() const;
    Q_INVOKABLE void setDataBySource(const QString sourceName, const QVariantMap data,
                                     const QVariantMap params);
    // values
    Q_INVOKABLE void graphicalValueByKey() const;
    Q_INVOKABLE QString infoByKey(QString key) const;
    Q_INVOKABLE QString valueByKey(QString key) const;
    // configuration
    Q_INVOKABLE void editItem(const QString type);

signals:
    void dropSourceFromDataengine(const QString source);
    void needTextToBeUpdated(const QString newText) const;
    void needToolTipToBeUpdated(const QString newText) const;
    void needToBeUpdated();

private slots:
    void dataUpdate() const;
    void loadKeysFromCache();
    void reinitKeys();
    // editor
    void editItemButtonPressed(QAbstractButton *button);
    void copyBar(const QString original);
    void copyQuotes(const QString original);
    void copyScript(const QString original);
    void copyUpgrade(const QString original);
    void copyWeather(const QString original);

private:
    // methods
    void addKeyToCache(const QString type, const QString key = QString(""));
    bool checkKeys(const QVariantMap data) const;
    QString htmlValue(QString key) const;
    int numberCpus() const;
    float temperature(const float temp, const QString units) const;
    QString toolTipImage() const;
    // find methods
    QStringList findGraphicalItems() const;
    QStringList findKeys() const;
    // get methods
    QList<ExtQuotes *> getExtQuotes();
    QList<ExtScript *> getExtScripts();
    QList<ExtUpgrade *> getExtUpgrade();
    QList<ExtWeather *> getExtWeather();
    QList<GraphicalItem *> getGraphicalItems();
    GraphicalItem *getItemByTag(const QString tag) const;
    QStringList getTimeKeys() const;
    AWToolTip *toolTip = nullptr;
    // graphical elements
    QDialog *dialog = nullptr;
    QListWidget *widgetDialog = nullptr;
    QDialogButtonBox *dialogButtons = nullptr;
    QPushButton *copyButton = nullptr;
    QPushButton *createButton = nullptr;
    QPushButton *deleteButton = nullptr;
    RequestedItem requestedItem = Nothing;
    // variables
    bool debug = false;
    bool enablePopup = false;
    bool wrapNewLines = false;
    QList<GraphicalItem *> graphicalItems;
    QList<ExtQuotes *> extQuotes;
    QList<ExtScript *> extScripts;
    QList<ExtUpgrade *> extUpgrade;
    QList<ExtWeather *> extWeather;
    QString pattern;
    QStringList foundBars, foundKeys, keys;
    QMap<QString, QString> values;
    QStringList diskDevices, hddDevices, mountDevices, networkDevices, tempDevices;
};


#endif /* AWKEYS_H */
