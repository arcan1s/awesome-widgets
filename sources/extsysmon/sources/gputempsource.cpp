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


#include "gputempsource.h"

#include <QTextCodec>

#include <task/taskadds.h>

#include "awdebug.h"


GPUTemperatureSource::GPUTemperatureSource(QObject *parent,
                                           const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 1);
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;

    m_device = args.at(0);
}


GPUTemperatureSource::~GPUTemperatureSource()
{
    qCDebug(LOG_ESM) << __PRETTY_FUNCTION__;
}


QVariant GPUTemperatureSource::data(QString source)
{
    qCDebug(LOG_ESM) << "Source" << source;

    if (source == QString("gpu/temperature")) {
        float value = 0.0;
        if ((m_device != QString("nvidia")) && (m_device != QString("ati")))
            return value;
        // build cmd
        QString cmd = m_device == QString("nvidia")
                          ? QString("nvidia-smi -q -x")
                          : QString("aticonfig --od-gettemperature");
        qCInfo(LOG_ESM) << "cmd" << cmd;
        TaskResult process = runTask(cmd);
        qCInfo(LOG_ESM) << "Cmd returns" << process.exitCode;
        qCInfo(LOG_ESM) << "Error" << process.error;
        // parse
        QString qoutput
            = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
        if (m_device == QString("nvidia")) {
            foreach (QString str,
                     qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
                if (!str.contains(QString("<gpu_temp>")))
                    continue;
                QString temp = str.remove(QString("<gpu_temp>"))
                                   .remove(QString("C</gpu_temp>"));
                value = temp.toFloat();
                break;
            }
        } else if (m_device == QString("ati")) {
            foreach (QString str,
                     qoutput.split(QChar('\n'), QString::SkipEmptyParts)) {
                if (!str.contains(QString("Temperature")))
                    continue;
                QString temp
                    = str.split(QChar(' '), QString::SkipEmptyParts).at(4);
                value = temp.toFloat();
                break;
            }
        }
        // return
        return value;
    }

    return QVariant();
}


QVariantMap GPUTemperatureSource::initialData(QString source) const
{
    qCDebug(LOG_ESM) << "Source" << source;

    QVariantMap data;
    if (source == QString("gpu/temperature")) {
        data[QString("min")] = 0.0;
        data[QString("max")] = 0.0;
        data[QString("name")] = QString("GPU temperature");
        data[QString("type")] = QString("float");
        data[QString("units")] = QString("°C");
    }

    return data;
}


QStringList GPUTemperatureSource::sources() const
{
    QStringList sources;
    sources.append(QString("gpu/temperature"));

    return sources;
}
