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

#include <Plasma/DataEngine>


class ExtQuotes;
class ExtScript;
class ExtUpgrade;

class ExtendedSysMon : public Plasma::DataEngine
{
    Q_OBJECT

public:
    explicit ExtendedSysMon(QObject *parent, const QVariantList &args);
    ~ExtendedSysMon();
    // update functions
    QVariantMap getBattery(const QString acpiPath);
    QVariantMap getCurrentDesktop();
    float getGpu(const QString device);
    float getGpuTemp(const QString device);
    float getHddTemp(const QString cmd, const QString device);
    QString getNetworkDevice();
    QVariantMap getPlayerInfo(const QString playerName,
                              const QString mpdAddress = QString(),
                              const QString mpdPort = QString(),
                              const QString mpris = QString());
    QVariantMap getPlayerMpdInfo(const QString mpdAddress = QString(),
                                 const QString mpdPort = QString());
    QVariantMap getPlayerMprisInfo(const QString mpris = QString());
    QVariantMap getPsStats();

protected:
    bool sourceRequestEvent(const QString &source);
    bool updateSourceEvent(const QString &source);
    QStringList sources() const;

private:
    // configuration
    QMap<QString, QString> configuration;
    QList<ExtQuotes *> externalQuotes;
    QList<ExtScript *> externalScripts;
    QList<ExtUpgrade *> externalUpgrade;
    bool debug;
    // reread configuration
    QStringList allHddDevices;
    QString getAllHdd();
    QString getAutoGpu();
    QString getAutoMpris();
    void initQuotes();
    void initScripts();
    void initUpgrade();
    void readConfiguration();
    QMap<QString, QString> updateConfiguration(QMap<QString, QString> rawConfig);
};


#endif /* EXTSYSMON_H */
