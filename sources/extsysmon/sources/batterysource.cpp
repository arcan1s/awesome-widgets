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


BatterySource::BatterySource(QObject *parent, const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 1);
    qCDebug(LOG_ESM);

    m_acpiPath = args.at(0);
    m_sources = getSources();
}


BatterySource::~BatterySource()
{
    qCDebug(LOG_ESM);
}


QVariant BatterySource::data(QString source)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    if (source == QString("battery/ac")) run();
    return values[source];
}


QVariantMap BatterySource::initialData(QString source) const
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    QVariantMap data;
    if (source == QString("battery/ac")) {
        data[QString("min")] = false;
        data[QString("max")] = true;
        data[QString("name")] = QString("Is AC online or not");
        data[QString("type")] = QString("bool");
        data[QString("units")] = QString("");
    } else if (source == QString("battery/bat")) {
        data[QString("min")] = 0;
        data[QString("max")] = 100;
        data[QString("name")] = QString("Average battery usage");
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("%");
    } else {
        data[QString("min")] = 0;
        data[QString("max")] = 100;
        data[QString("name")] = QString("Battery %1 usage").arg(source.remove(QString("battery/bat")));
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("%");
    }

    return data;
}


void BatterySource::run()
{
    qCDebug(LOG_ESM);

    // adaptor
    QFile acFile(QString("%1/AC/online").arg(m_acpiPath));
    if (acFile.open(QIODevice::ReadOnly))
        values[QString("battery/ac")] = (QString(acFile.readLine()).trimmed().toInt() == 1);
    acFile.close();

    // batterites
    float currentLevel = 0.0;
    float fullLevel = 0.0;
    for (int i=0; i<m_batteriesCount; i++) {
        QFile currentLevelFile(QString("%1/BAT%2/energy_now").arg(m_acpiPath).arg(i));
        QFile fullLevelFile(QString("%1/BAT%2/energy_full").arg(m_acpiPath).arg(i));
        if ((currentLevelFile.open(QIODevice::ReadOnly)) &&
            (fullLevelFile.open(QIODevice::ReadOnly))) {
            float batCurrent = QString(currentLevelFile.readLine()).trimmed().toFloat();
            float batFull = QString(fullLevelFile.readLine()).trimmed().toFloat();
            values[QString("battery/bat%1").arg(i)] = static_cast<int>(100 * batCurrent / batFull);
            currentLevel += batCurrent;
            fullLevel += batFull;
        }
        currentLevelFile.close();
        fullLevelFile.close();
    }
    values[QString("battery/bat")] = static_cast<int>(100 * currentLevel / fullLevel);
}


QStringList BatterySource::sources() const
{
    qCDebug(LOG_ESM);

    return m_sources;
}


QStringList BatterySource::getSources()
{
    qCDebug(LOG_ESM);

    QStringList sources;
    sources.append(QString("battery/ac"));
    sources.append(QString("battery/bat"));
    m_batteriesCount = QDir(m_acpiPath).entryList(QStringList() << QString("BAT*"),
                                                  QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name).count();
    qCInfo(LOG_ESM) << "Init batteries count as" << m_batteriesCount;
    for (int i=0; i<m_batteriesCount; i++)
        sources.append(QString("battery/bat%1").arg(i));

    qCInfo(LOG_ESM) << "Sources list" << sources;

    return sources;
}
