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


#include "batterysource.h"

#include <QDir>

#include "awdebug.h"


BatterySource::BatterySource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 1);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_acpiPath = _args.at(0);
    m_sources = getSources();
    m_timestamp = QDateTime::currentDateTimeUtc();
}


BatterySource::~BatterySource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QStringList BatterySource::getSources()
{
    QStringList sources;
    sources.append("battery/ac");
    sources.append("battery/bat");
    sources.append("battery/batleft");
    sources.append("battery/batnow");
    sources.append("battery/batrate");
    sources.append("battery/battotal");
    m_batteriesCount
        = QDir(m_acpiPath)
              .entryList(QStringList({"BAT*"}),
                         QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name)
              .count();
    qCInfo(LOG_ESS) << "Init batteries count as" << m_batteriesCount;
    for (int i = 0; i < m_batteriesCount; i++) {
        sources.append(QString("battery/bat%1").arg(i));
        sources.append(QString("battery/batleft%1").arg(i));
        sources.append(QString("battery/batnow%1").arg(i));
        sources.append(QString("battery/batrate%1").arg(i));
        sources.append(QString("battery/battotal%1").arg(i));
    }

    qCInfo(LOG_ESS) << "Sources list" << sources;
    return sources;
}


QVariant BatterySource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (!m_values.contains(_source))
        run();
    QVariant value = m_values.take(_source);
    return value;
}


QVariantMap BatterySource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    QVariantMap data;
    if (_source == "battery/ac") {
        data["min"] = false;
        data["max"] = true;
        data["name"] = "Is AC online or not";
        data["type"] = "bool";
        data["units"] = "";
    } else if (_source == "battery/bat") {
        data["min"] = 0;
        data["max"] = 100;
        data["name"] = "Average battery usage";
        data["type"] = "integer";
        data["units"] = "%";
    } else if (_source == "battery/batleft") {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = "Battery discharge time";
        data["type"] = "integer";
        data["units"] = "s";
    } else if (_source == "battery/batnow") {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = "Current battery capacity";
        data["type"] = "integer";
        data["units"] = "";
    } else if (_source == "battery/batrate") {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = "Average battery discharge rate";
        data["type"] = "float";
        data["units"] = "1/s";
    } else if (_source == "battery/battotal") {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = "Full battery capacity";
        data["type"] = "integer";
        data["units"] = "";
    } else if (_source == "battery/batleft") {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = QString("Battery %1 discharge time").arg(index(_source));
        data["type"] = "integer";
        data["units"] = "s";
    } else if (_source.startsWith("battery/batnow")) {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = QString("Battery %1 capacity").arg(index(_source));
        data["type"] = "integer";
        data["units"] = "";
    } else if (_source.startsWith("battery/battotal")) {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = QString("Battery %1 full capacity").arg(index(_source));
        data["type"] = "integer";
        data["units"] = "";
    } else if (_source.startsWith("battery/batrate")) {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = QString("Battery %1 discharge rate").arg(index(_source));
        data["type"] = "float";
        data["units"] = "1/s";
    } else if (_source.startsWith("battery/bat")) {
        data["min"] = 0;
        data["max"] = 100;
        data["name"] = QString("Battery %1 usage").arg(index(_source));
        data["type"] = "integer";
        data["units"] = "%";
    }

    return data;
}


void BatterySource::run()
{
    // swap data
    m_previousValues.swap(m_values);

    // adaptor
    QFile acFile(QString("%1/AC/online").arg(m_acpiPath));
    if (acFile.open(QIODevice::ReadOnly | QIODevice::Text))
        m_values["battery/ac"]
            = (QString(acFile.readLine()).trimmed().toInt() == 1);
    acFile.close();

    // batteries
    float currentLevel = 0.0, fullLevel = 0.0;
    for (int i = 0; i < m_batteriesCount; i++) {
        // current level
        QFile currentLevelFile(
            QString("%1/BAT%2/energy_now").arg(m_acpiPath).arg(i));
        if (currentLevelFile.open(QIODevice::ReadOnly | QIODevice::Text))
            m_values[QString("battery/batnow%1").arg(i)]
                = QString(currentLevelFile.readLine()).toInt();
        currentLevelFile.close();
        // total
        QFile fullLevelFile(
            QString("%1/BAT%2/energy_full").arg(m_acpiPath).arg(i));
        if (fullLevelFile.open(QIODevice::ReadOnly | QIODevice::Text))
            m_values[QString("battery/battotal%1").arg(i)]
                = QString(fullLevelFile.readLine()).toInt();
        fullLevelFile.close();

        m_values[QString("battery/bat%1").arg(i)] = static_cast<int>(
            100 * m_values[QString("battery/batnow%1").arg(i)].toFloat()
            / m_values[QString("battery/battotal%1").arg(i)].toFloat());
        // accumulate
        currentLevel += m_values[QString("battery/batnow%1").arg(i)].toFloat();
        fullLevel += m_values[QString("battery/battotal%1").arg(i)].toFloat();
    }

    // total
    m_values["battery/batnow"] = static_cast<int>(currentLevel);
    m_values["battery/battotal"] = static_cast<int>(fullLevel);
    m_values["battery/bat"] = static_cast<int>(100 * currentLevel / fullLevel);

    calculateRates();
}


QStringList BatterySource::sources() const
{
    return m_sources;
}


void BatterySource::calculateRates()
{
    // we are using moving average to get good numbers
    // check time interval
    auto now = QDateTime::currentDateTimeUtc();
    auto interval
        = 1000 * (now.toMSecsSinceEpoch() - m_timestamp.toMSecsSinceEpoch());
    m_timestamp.swap(now);

    // check time
    if (m_previousValues.empty()) {
        qCInfo(LOG_ESS) << "No historical data found for charges, exit";
        return;
    }

    // per battery
    for (int i = 0; i < m_batteriesCount; i++) {
        auto rate
            = (m_values[QString("battery/batnow%1").arg(i)].toInt()
               - m_previousValues[QString("battery/batnow%1").arg(i)].toInt())
              / interval;
        m_values[QString("battery/batrate%1").arg(i)]
            = (m_previousValues[QString("battery/batrate%1").arg(i)].toInt()
               + rate)
              / 2;
        m_values[QString("battery/batleft%1").arg(i)]
            = m_values[QString("battery/batnow%1").arg(i)].toInt()
              / m_values[QString("battery/batrate%1").arg(i)].toInt();
    }

    // total
    auto totalRate = (m_values["battery/batnow"].toInt()
                      - m_previousValues["battery/batnow"].toInt())
                     / interval;
    m_values["battery/batrate"]
        = (totalRate + m_previousValues["battery/batrate"].toInt()) / 2;
    m_values["battery/batleft"] = m_values["battery/batnow"].toInt()
                                  / m_values["battery/batrate"].toInt();
}
