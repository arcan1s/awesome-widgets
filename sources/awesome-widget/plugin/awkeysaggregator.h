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


#ifndef AWKEYSAGGREGATOR_H
#define AWKEYSAGGREGATOR_H

#include <QHash>
#include <QObject>


class AWFormatterHelper;
class AWDataEngineMapper;

class AWKeysAggregator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString acOffline MEMBER m_acOffline WRITE setAcOffline);
    Q_PROPERTY(QString acOnline MEMBER m_acOnline WRITE setAcOnline);
    Q_PROPERTY(QString customTime MEMBER m_customTime WRITE setCustomTime);
    Q_PROPERTY(QString customUptime MEMBER m_customUptime WRITE setCustomUptime);
    Q_PROPERTY(QString tempUnits MEMBER m_tempUnits WRITE setTempUnits);
    Q_PROPERTY(bool translate MEMBER m_translate WRITE setTranslate);

public:
    enum class FormatterType {
        // general formatters
        Custom,
        NoFormat,
        Float,
        FloatTwoSymbols,
        Integer,
        IntegerFive,
        IntegerThree,
        List,
        // unit specific formatters
        ACFormat,
        MemGBFormat,
        MemMBFormat,
        NetSmartFormat,
        NetSmartUnits,
        Quotes,
        Temperature,
        Time,
        TimeCustom,
        TimeISO,
        TimeLong,
        TimeShort,
        Timestamp,
        Uptime,
        UptimeCustom
    };

    explicit AWKeysAggregator(QObject *_parent = nullptr);
    ~AWKeysAggregator() override;
    void initFormatters();
    // get methods
    QString formatter(const QVariant &_data, const QString &_key) const;
    QStringList keysFromSource(const QString &_source) const;
    // set methods
    void setAcOffline(const QString &_inactive);
    void setAcOnline(const QString &_active);
    void setCustomTime(const QString &_customTime);
    void setCustomUptime(const QString &_customUptime);
    void setDevices(const QHash<QString, QStringList> &_devices);
    void setTempUnits(const QString &_units);
    void setTranslate(const bool _translate);

public slots:
    QStringList registerSource(const QString &_source, const QString &_units,
                               const QStringList &_keys);

private:
    float temperature(const float temp) const;
    AWFormatterHelper *m_customFormatters = nullptr;
    AWDataEngineMapper *m_mapper = nullptr;
    QStringList m_timeKeys;
    // variables
    QString m_acOffline;
    QString m_acOnline;
    QString m_customTime;
    QString m_customUptime;
    QString m_tempUnits;
    bool m_translate = false;
};


#endif /* AWKEYSAGGREGATOR_H */
