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

#include <QDialog>


namespace Ui {
class ExtScript;
}

class ExtScript : public QDialog
{
    Q_OBJECT

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
    explicit ExtScript(QWidget *parent = 0, const QString scriptName = QString(),
                       const QStringList directories = QStringList(), const bool debugCmd = false);
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
    QString name;
    QStringList dirs;
    bool debug;
    Ui::ExtScript *ui;
    // configuration
    void fromExternalConfiguration(const QMap<QString, QString> settings);
    QMap<QString, QString> getConfigurationFromFile(const QString fileName);
    QMap<QString, QString> toExternalConfiguration();
    // properties
    bool active = true;
    int interval = 1;
    bool output = true;
    QString prefix = QString("");
    Redirect redirect = nothing;
};


#endif /* EXTSCRIPT_H */
