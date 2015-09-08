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


#include "hddtempsource.h"

#include <QTextCodec>

#include <task/taskadds.h>

#include "awdebug.h"


HDDTemperatureSource::HDDTemperatureSource(QObject *parent, const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 2);
    qCDebug(LOG_ESM);

    m_devices = args.at(0).split(QChar(','));
    m_cmd = args.at(1);

    m_smartctl = m_cmd.contains(QString("smartctl"));
    qCInfo(LOG_ESM) << "Parse as smartctl" << m_smartctl;
}


HDDTemperatureSource::~HDDTemperatureSource()
{
    qCDebug(LOG_ESM);
}


QVariant HDDTemperatureSource::data(QString source)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    QString device = source.remove(QString("hdd/temperature"));
    float value = 0.0;
    // run cmd
    TaskResult process = runTask(QString("%1 %2").arg(m_cmd).arg(device));
    qCInfo(LOG_ESM) << "Cmd returns" << process.exitCode;
    qCInfo(LOG_ESM) << "Error" << process.error;

    // parse
    QString qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
    if (m_smartctl) {
        foreach(QString str, qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
            if (!str.startsWith(QString("194"))) continue;
            if (str.split(QChar(' '), QString::SkipEmptyParts).count() < 9) break;
            value = str.split(QChar(' '), QString::SkipEmptyParts).at(9).toFloat();
            break;
        }
    } else {
        if (qoutput.split(QChar(':'), QString::SkipEmptyParts).count() >= 3) {
            QString temp = qoutput.split(QChar(':'), QString::SkipEmptyParts).at(2);
            temp.remove(QChar(0260)).remove(QChar('C'));
            value = temp.toFloat();
        }
    }

    return value;
}


QVariantMap HDDTemperatureSource::initialData(QString source) const
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Source" << source;

    QString device = source.remove(QString("hdd/temperature"));
    QVariantMap data;
    data[QString("min")] = 0.0;
    data[QString("max")] = 0.0;
    data[QString("name")] = QString("HDD '%1' temperature").arg(device);
    data[QString("type")] = QString("float");
    data[QString("units")] = QString("°C");

    return data;
}


QStringList HDDTemperatureSource::sources() const
{
    qCDebug(LOG_ESM);

    QStringList sources;
    foreach(QString device, m_devices)
        sources.append(QString("hdd/temperature%1").arg(device));

    return sources;
}
