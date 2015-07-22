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

#include <QMap>
#include <QProcess>

#include "abstractextitem.h"


namespace Ui {
    class ExtScript;
}

class ExtScript : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(QString executable READ executable WRITE setExecutable)
    Q_PROPERTY(QStringList filters READ filters WRITE setFilters)
    Q_PROPERTY(bool output READ hasOutput WRITE setHasOutput)
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)
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
    QString executable();
    QStringList filters();
    bool hasOutput();
    QString prefix();
    Redirect redirect();
    // derivatives
    QString strRedirect();
    // set methods
    void setExecutable(const QString _executable = QString("/usr/bin/true"));
    void setFilters(const QStringList _filters = QStringList());
    void setHasOutput(const bool _state = true);
    void setPrefix(const QString _prefix = QString(""));
    void setRedirect(const Redirect _redirect = nothing);
    void setStrRedirect(const QString _redirect = QString("nothing"));
    // filters
    QString applyFilters(QString _value);
    void updateFilter(const QString _filter, const bool _add = true);

public slots:
    void readConfiguration();
    void readJsonFilters();
    QVariantMap run();
    int showConfiguration();
    void writeConfiguration();

private slots:
    void updateValue();

private:
    bool debug;
    QProcess *process = nullptr;
    Ui::ExtScript *ui;
    // properties
    QString m_executable = QString("/usr/bin/true");
    QStringList m_filters = QStringList();
    bool m_output = true;
    QString m_prefix = QString("");
    Redirect m_redirect = nothing;
    // internal properties
    Q_PID childProcess = 0;
    QVariantMap jsonFilters = QVariantMap();
    int times = 0;
    QVariantMap value;
};


#endif /* EXTSCRIPT_H */
