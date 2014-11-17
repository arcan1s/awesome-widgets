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
        stdout2stderr = 0,
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
    // get methods
    int getInterval();
    QString getName();
    QString getPrefix();
    Redirect getRedirect();
    bool hasOutput();
    bool isActive();
    // set methods
    void setActive(const bool state = true);
    void setHasOutput(const bool state = true);
    void setInterval(const int interval = 1);
    void setPrefix(const QString prefix = QString(""));
    void setRedirect(const QString redirect = QString("nothing"));

public slots:
    void readConfiguration();
    ScriptData run(const int time);
    void showConfiguration();
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
    bool _active = true;
    QString _comment = QString("");
    int _interval = 1;
    bool _output = true;
    QString _prefix = QString("");
    Redirect _redirect = nothing;
};


#endif /* EXTSCRIPT_H */
