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

#include <QHash>
#include <QObject>
#include <QStringList>
#include <QVariant>

#include "extitemaggregator.h"
#include "version.h"


class AWToolTip;
class ExtQuotes;
class ExtScript;
class ExtUpgrade;
class ExtWeather;
class GraphicalItem;

class AWKeys : public QObject
{
    Q_OBJECT

public:
    AWKeys(QObject *parent = nullptr);
    ~AWKeys();

    Q_INVOKABLE void initKeys(const QString currentPattern);
    Q_INVOKABLE void initTooltip(const QVariantMap tooltipParams);
    Q_INVOKABLE void setPopupEnabled(const bool popup = false);
    Q_INVOKABLE void setTranslateStrings(const bool translate = false);
    Q_INVOKABLE void setWrapNewLines(const bool wrap = false);
    Q_INVOKABLE QSize toolTipSize() const;
    // keys
    Q_INVOKABLE void addDevice(const QString source);
    Q_INVOKABLE QStringList dictKeys(const bool sorted = false,
                                     const QString regexp = QString()) const;
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

private:
    // methods
    void addKeyToCache(const QString type, const QString key = QString(""));
    QString parsePattern() const;
    float temperature(const float temp, const QString units) const;
    // find methods
    QStringList findGraphicalItems() const;
    QStringList findKeys() const;
    QStringList findLambdas() const;
    // get methods
    AWToolTip *toolTip = nullptr;
    bool debug = false;
    bool enablePopup = false;
    bool translateStrings = false;
    bool wrapNewLines = false;
    ExtItemAggregator<GraphicalItem> *graphicalItems;
    ExtItemAggregator<ExtQuotes> *extQuotes;
    ExtItemAggregator<ExtScript> *extScripts;
    ExtItemAggregator<ExtUpgrade> *extUpgrade;
    ExtItemAggregator<ExtWeather> *extWeather;
    QString pattern;
    QStringList foundBars, foundKeys, foundLambdas;
    QStringList timeKeys = QString(TIME_KEYS).split(QChar(','));
    QHash<QString, QString> values;
    QStringList diskDevices, hddDevices, mountDevices, networkDevices, tempDevices;
};


#endif /* AWKEYS_H */
