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

#ifndef EXTSYSMON_H
#define EXTSYSMON_H

#ifndef MSEC_IN_HOUR
#define MSEC_IN_HOUR 60*60*1000
#endif /* MSEC_IN_HOUR */

#include <Plasma/DataEngine>
#include <QProcess>


class ExtScript;

class ExtendedSysMon : public Plasma::DataEngine
{
    Q_OBJECT

public:
    ExtendedSysMon(QObject *parent, const QVariantList &args);
    // update functions
    QMap<QString, QVariant> getBattery(const QString acpiPath);
    QMap<QString, QVariant> getCurrentDesktop();
    float getGpu(const QString device);
    float getGpuTemp(const QString device);
    float getHddTemp(const QString cmd, const QString device);
    QString getNetworkDevice();
    QMap<QString, QVariant> getPlayerInfo(const QString playerName,
                                          const QString mpdAddress = 0,
                                          const QString mpdPort = 0,
                                          QString mpris = 0);
    QMap<QString, QVariant> getPsStats();
    int getUpgradeInfo(const QString cmd);

protected:
    bool sourceRequestEvent(const QString &source);
    bool updateSourceEvent(const QString &source);
    QStringList sources() const;

private:
    // configuration
    QMap<QString, QString> configuration;
    QList<ExtScript *> externalScripts;
    QList<int> times;
    bool debug;
    // FIXME dirty hack to avoid update package information every second
    int pkgTimeUpdate = MSEC_IN_HOUR;
    // reread configuration
    QString getAllHdd();
    QString getAutoGpu();
    QString getAutoMpris();
    void initScripts();
    void readConfiguration();
    QMap<QString, QString> updateConfiguration(QMap<QString, QString> rawConfig);
};


#endif /* EXTSYSMON_H */
