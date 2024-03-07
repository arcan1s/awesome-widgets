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


ProcessesSource::ProcessesSource(QObject *_parent, const QStringList &_args)
    : AbstractExtSysMonSource(_parent, _args)
{
    Q_ASSERT(_args.count() == 0);
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


ProcessesSource::~ProcessesSource()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;
}


QVariant ProcessesSource::data(const QString &_source)
{
    qCDebug(LOG_ESS) << "Source" << _source;

    if (!m_values.contains(_source))
        run();
    QVariant value = m_values.take(_source);
    return value;
}


QVariantMap ProcessesSource::initialData(const QString &_source) const
{
    qCDebug(LOG_ESS) << "Source" << _source;

    QVariantMap data;
    if (_source == "ps/running/count") {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = "Count of running processes";
        data["type"] = "integer";
        data["units"] = "";
    } else if (_source == "ps/running/list") {
        data["min"] = QStringList();
        data["max"] = QStringList();
        data["name"] = "All running processes list";
        data["type"] = "QStringList";
        data["units"] = "";
    } else if (_source == "ps/total/count") {
        data["min"] = 0;
        data["max"] = 0;
        data["name"] = "Total count of processes";
        data["type"] = "integer";
        data["units"] = "";
    }

    return data;
}


void ProcessesSource::run()
{
    QStringList allDirectories = QDir("/proc").entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    QStringList directories = allDirectories.filter(QRegularExpression("(\\d+)"));
    QStringList running;

    for (auto &dir : directories) {
        QFile statusFile(QString("/proc/%1/status").arg(dir));
        if (!statusFile.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        QFile cmdFile(QString("/proc/%1/cmdline").arg(dir));
        if (!cmdFile.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QString output = statusFile.readAll();
        if (output.contains("running"))
            running.append(cmdFile.readAll());
        statusFile.close();
        cmdFile.close();
    }

    m_values["ps/running/count"] = running.count();
    m_values["ps/running/list"] = running;
    m_values["ps/total/count"] = directories.count();
}


QStringList ProcessesSource::sources() const
{
    QStringList sources;
    sources.append("ps/running/count");
    sources.append("ps/running/list");
    sources.append("ps/total/count");

    return sources;
}
