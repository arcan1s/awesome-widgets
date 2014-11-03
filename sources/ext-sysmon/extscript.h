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

#ifndef EXTSCRIPT_H
#define EXTSCRIPT_H

#include <QObject>
#include <QStringList>


class ExtScript : public QObject
{
public:
    enum Redirect {
        stdout2stderr = -1,
        nothing,
        stderr2stdout
    };
    typedef struct {
        bool active;
        QString name;
        QString output;
        bool refresh;
    } ScriptData;

    ExtScript(const QString scriptName, const QStringList directories, const bool debugCmd = false);
    ~ExtScript();
    // configuration
    void addDirectory(const QString dir);
    QStringList directories();
    int getInterval();
    QString getName();
    QString getPrefix();
    Redirect getRedirect();
    bool hasOutput();
    bool isActive();
    void setActive(const bool state = true);
    void setDirectories(const QStringList directories);
    void setHasOutput(const bool state = true);
    void setInterval(const int inter = 1);
    void setPrefix(const QString pref = QString(""));
    void setRedirect(const Redirect redir);

public slots:
    void readConfiguration();
    ScriptData run(const int time);
    void tryDelete();
    void writeConfiguration();

private:
    // configuration
    void fromExternalConfiguration(const QMap<QString, QString> settings);
    QMap<QString, QString> getConfigurationFromFile(const QString fileName);
    QMap<QString, QString> toExternalConfiguration();
    // properties
    bool active = true;
    QString name;
    QStringList dirs;
    int interval = 1;
    bool output = true;
    QString prefix = QString("");
    Redirect redirect = nothing;
    // other
    bool debug;
};


#endif /* EXTSCRIPT_H */
