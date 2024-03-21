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

#include <ksysguard/formatter/Unit.h>
#include <ksysguard/systemstats/SensorInfo.h>

#include <QDir>

#include <cmath>

#include "awdebug.h"


BatterySource::BatterySource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 1);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_acpiPath = _args.at(0);
    m_sources = getSources();
}


BatterySource::~BatterySource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QStringList BatterySource::getSources()
{
    QStringList sources;
    sources.append("ac");
    sources.append("bat");
    sources.append("batleft");
    sources.append("batnow");
    sources.append("batrate");
    sources.append("battotal");

    auto directory = QDir(m_acpiPath);

    if (directory.exists()) {
        m_batteriesCount
            = directory.entryList(QStringList({"BAT*"}), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name).count();
        qCInfo(LOG_ESS) << "Init batteries count as" << m_batteriesCount;

        for (int i = 0; i < m_batteriesCount; i++) {
            sources.append(QString("bat%1").arg(i));
            sources.append(QString("batleft%1").arg(i));
            sources.append(QString("batnow%1").arg(i));
            sources.append(QString("batrate%1").arg(i));
            sources.append(QString("battotal%1").arg(i));
        }
    }

    qCInfo(LOG_ESS) << "Sources list" << sources;
    return sources;
}


QVariant BatterySource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (!m_values.contains(_source))
        run();
    return m_values.take(_source);
}


KSysGuard::SensorInfo *BatterySource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    auto data = new KSysGuard::SensorInfo();
    if (_source == "ac") {
        data->name = "Is AC online or not";
        data->variantType = QVariant::Bool;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "bat") {
        data->min = 0;
        data->max = 100;
        data->name = "Average battery usage";
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitPercent;
    } else if (_source == "batleft") {
        data->min = 0;
        data->max = 0;
        data->name = "Battery discharge time";
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitSecond;
    } else if (_source == "batnow") {
        data->min = 0;
        data->max = 0;
        data->name = "Current battery capacity";
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitNone;
    } else if (_source == "batrate") {
        data->min = 0;
        data->max = 0;
        data->name = "Average battery discharge rate";
        data->variantType = QVariant::Double;
        data->unit = KSysGuard::UnitRate;
    } else if (_source == "battotal") {
        data->min = 0;
        data->max = 0;
        data->name = "Full battery capacity";
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitNone;
    } else if (_source.startsWith("batleft")) {
        data->min = 0;
        data->max = 0;
        data->name = QString("Battery %1 discharge time").arg(index(_source));
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitSecond;
    } else if (_source.startsWith("batnow")) {
        data->min = 0;
        data->max = 0;
        data->name = QString("Battery %1 capacity").arg(index(_source));
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitNone;
    } else if (_source.startsWith("battotal")) {
        data->min = 0;
        data->max = 0;
        data->name = QString("Battery %1 full capacity").arg(index(_source));
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitNone;
    } else if (_source.startsWith("batrate")) {
        data->min = 0;
        data->max = 0;
        data->name = QString("Battery %1 discharge rate").arg(index(_source));
        data->variantType = QVariant::Double;
        data->unit = KSysGuard::UnitRate;
    } else if (_source.startsWith("bat")) {
        data->min = 0;
        data->max = 100;
        data->name = QString("Battery %1 usage").arg(index(_source));
        data->variantType = QVariant::Int;
        data->unit = KSysGuard::UnitPercent;
    }

    return data;
}


void BatterySource::run()
{
    // adaptor
    QFile acFile(QString("%1/AC/online").arg(m_acpiPath));
    if (acFile.open(QIODevice::ReadOnly | QIODevice::Text))
        m_values["ac"] = (QString(acFile.readLine()).trimmed().toInt() == 1);
    acFile.close();

    // batteries
    float currentLevel = 0.0, fullLevel = 0.0;
    for (int i = 0; i < m_batteriesCount; i++) {
        // current level
        QFile currentLevelFile(QString("%1/BAT%2/energy_now").arg(m_acpiPath).arg(i));
        if (currentLevelFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            auto value = QString(currentLevelFile.readLine()).toInt();
            m_trend[i + 1].append(value);
            m_values[QString("batnow%1").arg(i)] = value;
        }
        currentLevelFile.close();
        // total
        QFile fullLevelFile(QString("%1/BAT%2/energy_full").arg(m_acpiPath).arg(i));
        if (fullLevelFile.open(QIODevice::ReadOnly | QIODevice::Text))
            m_values[QString("battotal%1").arg(i)] = QString(fullLevelFile.readLine()).toInt();
        fullLevelFile.close();

        m_values[QString("bat%1").arg(i)] = static_cast<int>(100 * m_values[QString("batnow%1").arg(i)].toFloat()
                                                             / m_values[QString("battotal%1").arg(i)].toFloat());
        // accumulate
        currentLevel += m_values[QString("batnow%1").arg(i)].toFloat();
        fullLevel += m_values[QString("battotal%1").arg(i)].toFloat();
    }

    // total
    m_trend[0].append(static_cast<int>(currentLevel));
    m_values["batnow"] = static_cast<int>(currentLevel);
    m_values["battotal"] = static_cast<int>(fullLevel);
    m_values["bat"] = static_cast<int>(100 * currentLevel / fullLevel);

    calculateRates();
}


QStringList BatterySource::sources() const
{
    return m_sources;
}


double BatterySource::approximate(const QList<int> &_trend)
{
    qCDebug(LOG_ESS) << "Approximate by MSD" << _trend;

    auto count = _trend.count();

    auto sumxy = 0;
    for (int i = 0; i < count; i++)
        sumxy += _trend.at(i) * (i + 1);

    auto sumx = count * (count + 1) / 2;
    auto sumy = std::accumulate(_trend.cbegin(), _trend.cend(), 0);
    auto sumx2 = count * (count + 1) * (2 * count + 1) / 6;

    return (count * sumxy - sumx * sumy) / (count * sumx2 - std::pow(sumx, 2));
}


void BatterySource::calculateRates()
{
    // check if it is first run
    if (!m_timestamp.isValid()) {
        m_timestamp = QDateTime::currentDateTimeUtc();
        return;
    }

    // check time interval
    auto now = QDateTime::currentDateTimeUtc();
    auto interval = m_timestamp.secsTo(now);
    qCDebug(LOG_AW) << interval;
    m_timestamp.swap(now);

    for (int i = 0; i < m_batteriesCount; i++) {
        auto approx = approximate(m_trend[i + 1]);
        m_values[QString("batrate%1").arg(i)] = approx / interval;
        m_values[QString("batleft%1").arg(i)] = interval * m_values[QString("batnow%1").arg(i)].toFloat() / approx;
    }

    // total
    auto approx = approximate(m_trend[0]);
    m_values["batrate"] = approx / interval;
    m_values["batleft"] = interval * m_values["batnow"].toFloat() / approx;

    // old data cleanup
    for (auto &trend : m_trend.keys()) {
        if (m_trend[trend].count() <= TREND_LIMIT)
            continue;
        m_trend[trend].removeFirst();
    }
}
