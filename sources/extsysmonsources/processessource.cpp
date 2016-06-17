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


ProcessesSource::ProcessesSource(QObject *parent, const QStringList args)
    : AbstractExtSysMonSource(parent, args)
{
    Q_ASSERT(args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


ProcessesSource::~ProcessesSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant ProcessesSource::data(QString source)
{
    qCDebug(LOG_ESS) << "Source" << source;

    if (!m_values.contains(source))
        run();
    QVariant value = m_values.take(source);
    return value;
}


QVariantMap ProcessesSource::initialData(QString source) const
{
    qCDebug(LOG_ESS) << "Source" << source;

    QVariantMap data;
    if (source == QString("ps/running/count")) {
        data[QString("min")] = 0;
        data[QString("max")] = 0;
        data[QString("name")] = QString("Count of running processes");
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("");
    } else if (source == QString("ps/running/list")) {
        data[QString("min")] = QStringList();
        data[QString("max")] = QStringList();
        data[QString("name")] = QString("All running processes list");
        data[QString("type")] = QString("QStringList");
        data[QString("units")] = QString("");
    } else if (source == QString("ps/total/count")) {
        data[QString("min")] = 0;
        data[QString("max")] = 0;
        data[QString("name")] = QString("Total count of processes");
        data[QString("type")] = QString("integer");
        data[QString("units")] = QString("");
    }

    return data;
}


void ProcessesSource::run()
{
    QStringList allDirectories
        = QDir(QString("/proc"))
              .entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    QStringList directories = allDirectories.filter(QRegExp(QString("(\\d+)")));
    QStringList running;

    for (auto dir : directories) {
        QFile statusFile(QString("/proc/%1/status").arg(dir));
        if (!statusFile.open(QIODevice::ReadOnly))
            continue;
        QFile cmdFile(QString("/proc/%1/cmdline").arg(dir));
        if (!cmdFile.open(QIODevice::ReadOnly))
            continue;

        QString output = statusFile.readAll();
        if (output.contains(QString("running")))
            running.append(cmdFile.readAll());
    }

    m_values[QString("ps/running/count")] = running.count();
    m_values[QString("ps/running/list")] = running;
    m_values[QString("ps/total/count")] = directories.count();
}


QStringList ProcessesSource::sources() const
{
    QStringList sources;
    sources.append(QString("ps/running/count"));
    sources.append(QString("ps/running/list"));
    sources.append(QString("ps/total/count"));

    return sources;
}
