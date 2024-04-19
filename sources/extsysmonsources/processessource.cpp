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

#include "processessource.h"

#include <QDir>

#include "awdebug.h"


ProcessesSource::ProcessesSource(QObject *_parent)
    : AbstractExtSysMonSource(_parent)
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant ProcessesSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (!m_values.contains(_source))
        run(); // syncronous update of all values

    return m_values.take(_source);
}


void ProcessesSource::run()
{
    auto allDirectories = QDir("/proc").entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    auto directories = allDirectories.filter(QRegularExpression("(\\d+)"));

    QStringList running;
    for (auto &dir : directories) {
        QFile statusFile(QString("/proc/%1/status").arg(dir));
        if (!statusFile.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        QFile cmdFile(QString("/proc/%1/cmdline").arg(dir));
        if (!cmdFile.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        auto output = statusFile.readAll();
        if (output.contains("running"))
            running.append(cmdFile.readAll());
        statusFile.close();
        cmdFile.close();
    }

    m_values["running"] = running.count();
    m_values["list"] = running;
    m_values["count"] = directories.count();
}


QHash<QString, KSysGuard::SensorInfo *> ProcessesSource::sources() const
{
    auto result = QHash<QString, KSysGuard::SensorInfo *>();

    result.insert("running", makeSensorInfo("Count of running processes", QMetaType::Int));
    result.insert("list", makeSensorInfo("All running processes list", QMetaType::QStringList));
    result.insert("count", makeSensorInfo("Total count of processes", QMetaType::Int));

    return result;
}
