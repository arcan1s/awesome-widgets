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

#include <QMutex>
#include <QObject>

#include <ksysguard/systemstats/SensorInfo.h>


class AWDataAggregator;
class AWDataEngineAggregator;
class AWKeyOperations;
class AWKeysAggregator;
class QThreadPool;
class QTimer;

class AWKeys : public QObject
{
    Q_OBJECT

public:
    explicit AWKeys(QObject *_parent = nullptr);
    ~AWKeys() override;
    Q_INVOKABLE void initDataAggregator(const QVariantMap &_tooltipParams);
    Q_INVOKABLE void initKeys(const QString &_currentPattern, int _interval, int _limit, bool _optimize);
    Q_INVOKABLE void setAggregatorProperty(const QString &_key, const QVariant &_value);
    Q_INVOKABLE void setWrapNewLines(bool _wrap);
    // additional method to force load keys from Qml UI. Used in some
    // configuration pages
    Q_INVOKABLE void updateCache();
    // keys
    Q_INVOKABLE [[nodiscard]] QStringList dictKeys(bool _sorted = false, const QString &_regexp = "") const;
    Q_INVOKABLE [[nodiscard]] QVariantList getHddDevices() const;
    // values
    Q_INVOKABLE [[nodiscard]] QString infoByKey(const QString &_key) const;
    Q_INVOKABLE [[nodiscard]] QString valueByKey(const QString &_key) const;
    // configuration
    Q_INVOKABLE void editItem(const QString &_type);

public slots:
    void dataUpdated(const QHash<QString, KSysGuard::SensorInfo> &_sensors, const KSysGuard::SensorDataList &_data);

signals:
    void dropSourceFromDataengine(const QString &_source);
    void needTextToBeUpdated(const QString &_newText) const;
    void needToolTipToBeUpdated(const QString &_newText) const;

private slots:
    void reinitKeys(const QStringList &_currentKeys);
    void updateTextData();

private:
    // methods
    void calculateValues();
    void createDBusInterface();
    [[nodiscard]] QString parsePattern(QString _pattern) const;
    void setDataBySource(const QString &_source, const KSysGuard::SensorInfo &_sensor, const KSysGuard::SensorData &_data);
    // objects
    AWDataAggregator *m_dataAggregator = nullptr;
    AWDataEngineAggregator *m_dataEngineAggregator = nullptr;
    AWKeysAggregator *m_aggregator = nullptr;
    AWKeyOperations *m_keyOperator = nullptr;
    QTimer *m_timer = nullptr;
    // variables
    QVariantMap m_tooltipParams;
    QStringList m_foundBars, m_foundKeys, m_foundLambdas, m_requiredKeys;
    QVariantHash m_values;
    bool m_optimize = false;
    bool m_wrapNewLines = false;
    // multithread features
    QThreadPool *m_threadPool = nullptr;
    QMutex m_mutex;
};


#endif /* AWKEYS_H */
