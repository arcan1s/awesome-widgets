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

#include <QProcess>

#include "abstractextitem.h"


namespace Ui {
    class ExtUpgrade;
}

class ExtUpgrade : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(QString executable READ executable WRITE setExecutable)
    Q_PROPERTY(int null READ null WRITE setNull)

public:
    explicit ExtUpgrade(QWidget *parent = nullptr, const QString upgradeName = QString(),
                        const QStringList directories = QStringList(),
                        const bool debugCmd = false);
    ~ExtUpgrade();
    ExtUpgrade *copy(const QString fileName, const int number);
    // get methods
    QString executable() const;
    int null() const;
    QString uniq() const;
    // set methods
    void setExecutable(const QString _executable = QString("/usr/bin/true"));
    void setNull(const int _null = 0);

public slots:
    void readConfiguration();
    QVariantMap run();
    int showConfiguration(const QVariant args = QVariant());
    void writeConfiguration() const;

private slots:
    void updateValue();

private:
    bool debug;
    QProcess *process = nullptr;
    Ui::ExtUpgrade *ui;
    // properties
    QString m_executable = QString("/usr/bin/true");
    int m_null = 0;
    // internal properties
    int times = 0;
    QVariantMap value;
};


#endif /* EXTUPGRADE_H */
