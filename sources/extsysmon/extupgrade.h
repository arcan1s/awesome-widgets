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

#ifndef EXTUPGRADE_H
#define EXTUPGRADE_H

#include <QDialog>


namespace Ui {
class ExtUpgrade;
}

class ExtUpgrade : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int apiVersion READ apiVersion WRITE setApiVersion)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString comment READ comment WRITE setComment)
    Q_PROPERTY(QString executable READ executable WRITE setExecutable)
    Q_PROPERTY(int null READ null WRITE setNull)
    Q_PROPERTY(bool active READ isActive WRITE setActive)

public:
    explicit ExtUpgrade(QWidget *parent = 0, const QString upgradeName = QString(),
                        const QStringList directories = QStringList(), const bool debugCmd = false);
    ~ExtUpgrade();
    // get methods
    int apiVersion();
    QString comment();
    QString executable();
    QString fileName();
    QString name();
    int null();
    bool isActive();
    // set methods
    void setApiVersion(const int _apiVersion = 0);
    void setActive(const bool _state = true);
    void setComment(const QString _comment = QString("empty"));
    void setExecutable(const QString _executable = QString("/usr/bin/true"));
    void setName(const QString _name = QString("none"));
    void setNull(const int _null = 0);

public slots:
    void readConfiguration();
    int run();
    int showConfiguration();
    int tryDelete();
    void writeConfiguration();

private:
    QString m_fileName;
    QStringList m_dirs;
    bool debug;
    Ui::ExtUpgrade *ui;
    // properties
    int m_apiVersion = 0;
    bool m_active = true;
    QString m_comment = QString("empty");
    QString m_executable = QString("/usr/bin/true");
    QString m_name = QString("none");
    int m_null = 0;
};


#endif /* EXTUPGRADE_H */
