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
#include <QProcess>


namespace Ui {
    class ExtScript;
}

class ExtScript : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int apiVersion READ apiVersion WRITE setApiVersion)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString comment READ comment WRITE setComment)
    Q_PROPERTY(QString executable READ executable WRITE setExecutable)
    Q_PROPERTY(int number READ number WRITE setNumber)
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_PROPERTY(bool output READ hasOutput WRITE setHasOutput)
    Q_PROPERTY(int interval READ interval WRITE setInterval)
    Q_PROPERTY(Redirect redirect READ redirect WRITE setRedirect)

public:
    enum Redirect {
        stdout2stderr = 0,
        nothing,
        stderr2stdout
    };

    explicit ExtScript(QWidget *parent = nullptr, const QString scriptName = QString(),
                       const QStringList directories = QStringList(), const bool debugCmd = false);
    ~ExtScript();
    // get methods
    int apiVersion();
    QString comment();
    QString executable();
    QString fileName();
    int interval();
    QString name();
    int number();
    QString prefix();
    Redirect redirect();
    QString strRedirect();
    QString tag();
    bool hasOutput();
    bool isActive();
    // set methods
    void setApiVersion(const int _apiVersion = 0);
    void setActive(const bool _state = true);
    void setComment(const QString _comment = QString("empty"));
    void setExecutable(const QString _executable = QString("/usr/bin/true"));
    void setHasOutput(const bool _state = true);
    void setInterval(const int _interval = 1);
    void setName(const QString _name = QString("none"));
    void setNumber(int _number = -1);
    void setPrefix(const QString _prefix = QString(""));
    void setRedirect(const Redirect _redirect = nothing);
    void setStrRedirect(const QString _redirect = QString("nothing"));

public slots:
    void readConfiguration();
    QString run();
    int showConfiguration();
    bool tryDelete();
    void writeConfiguration();

private slots:
    void updateValue();

private:
    QString m_fileName;
    QStringList m_dirs;
    bool debug;
    QProcess *process = nullptr;
    Ui::ExtScript *ui;
    // properties
    int m_apiVersion = 0;
    bool m_active = true;
    QString m_comment = QString("empty");
    QString m_executable = QString("/usr/bin/true");
    int m_interval = 1;
    QString m_name = QString("none");
    int m_number = -1;
    bool m_output = true;
    QString m_prefix = QString("");
    Redirect m_redirect = nothing;
    Q_PID childProcess = 0;
    int times = 0;
    QString value = QString();
};


#endif /* EXTSCRIPT_H */
