/***************************************************************************
 *   This file is part of pytextmonitor                                    *
 *                                                                         *
 *   pytextmonitor is free software: you can redistribute it and/or        *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   pytextmonitor is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with pytextmonitor. If not, see http://www.gnu.org/licenses/    *
 ***************************************************************************/

#ifndef EXTSYSMON_H
#define EXTSYSMON_H

#include <Plasma/DataEngine>
#include <QProcess>

class ExtendedSysMon : public Plasma::DataEngine
{
    Q_OBJECT

public:
    ExtendedSysMon(QObject *parent, const QVariantList &args);
    // update functions
    void getCustomCmd(const QString cmd,
                      const int number = 0);
    void getGpu(const QString device);
    void getGpuTemp(const QString device);
    void getHddTemp(const QString cmd, const QString device,
                    const int number = 0);
    void getPlayerInfo(const QString playerName,
                       const QString mpdAddress = 0,
                       const QString mpdPort = 0);
    void getPsStats();
    void getUpgradeInfo(const QString pkgCommand,
                        const int number = 0);

protected:
    bool sourceRequestEvent(const QString &name);
    bool updateSourceEvent(const QString &source);
    QStringList sources() const;

private slots:
    void setCustomCmd(int exitCode, QProcess::ExitStatus exitStatus);
    void setGpu(int exitCode, QProcess::ExitStatus exitStatus);
    void setGpuTemp(int exitCode, QProcess::ExitStatus exitStatus);
    void setHddTemp(int exitCode, QProcess::ExitStatus exitStatus);
    void setUpgradeInfo(int exitCode, QProcess::ExitStatus exitStatus);
    void setPlayer();
    void setPs();

private:
    // processes
    QMap<QString, QList<QProcess*> > processes;
    // configuration
    QMap<QString, QString> configuration;
    bool debug;
    // reread configuration
    QString getAllHdd();
    QString getAutoGpu();
    void readConfiguration();
    void setProcesses();
    QMap<QString, QString> updateConfiguration(const QMap<QString, QString> rawConfig);
};

#endif /* EXTSYSMON_H */
