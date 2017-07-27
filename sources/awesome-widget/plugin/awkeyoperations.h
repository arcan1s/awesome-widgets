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


#ifndef AWKEYOPERATIONS_H
#define AWKEYOPERATIONS_H

#include <QObject>

#include "extitemaggregator.h"


class AWCustomKeysHelper;
class ExtNetworkRequest;
class ExtQuotes;
class ExtScript;
class ExtUpgrade;
class ExtWeather;
class GraphicalItem;

class AWKeyOperations : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern)

public:
    explicit AWKeyOperations(QObject *_parent = nullptr);
    virtual ~AWKeyOperations();
    QStringList devices(const QString &_type) const;
    QHash<QString, QStringList> devices() const;
    void updateCache();
    // keys
    QStringList dictKeys() const;
    GraphicalItem *giByKey(const QString &_key) const;
    QStringList requiredUserKeys() const;
    QStringList userKeys() const;
    QString userKeySource(const QString &_key) const;
    // values
    QString infoByKey(const QString &_key) const;
    QString pattern() const;
    void setPattern(const QString &_currentPattern);
    // configuration
    void editItem(const QString &_type);

signals:
    void updateKeys(const QStringList &_currentKeys);

public slots:
    void addDevice(const QString &_source);

private:
    // methods
    void addKeyToCache(const QString &_type, const QString &_key = "");
    void reinitKeys();
    // objects
    AWCustomKeysHelper *m_customKeys = nullptr;
    ExtItemAggregator<GraphicalItem> *m_graphicalItems = nullptr;
    ExtItemAggregator<ExtNetworkRequest> *m_extNetRequest = nullptr;
    ExtItemAggregator<ExtQuotes> *m_extQuotes = nullptr;
    ExtItemAggregator<ExtScript> *m_extScripts = nullptr;
    ExtItemAggregator<ExtUpgrade> *m_extUpgrade = nullptr;
    ExtItemAggregator<ExtWeather> *m_extWeather = nullptr;
    // variables
    QHash<QString, QStringList> m_devices;
    QString m_pattern;
};


#endif /* AWKEYOPERATIONS_H */
