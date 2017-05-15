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


namespace Ui
{
class ExtUpgrade;
}

class ExtUpgrade : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(QString executable READ executable WRITE setExecutable)
    Q_PROPERTY(QString filter READ filter WRITE setFilter)
    Q_PROPERTY(int null READ null WRITE setNull)

public:
    explicit ExtUpgrade(QWidget *_parent = nullptr,
                        const QString &_filePath = "");
    virtual ~ExtUpgrade();
    ExtUpgrade *copy(const QString &_fileName, const int _number);
    // get methods
    QString executable() const;
    QString filter() const;
    int null() const;
    QString uniq() const;
    // set methods
    void setExecutable(const QString &_executable);
    void setFilter(const QString &_filter);
    void setNull(const int _null);

public slots:
    void readConfiguration();
    QVariantHash run();
    int showConfiguration(const QVariant &_args);
    void writeConfiguration() const;

private slots:
    void startProcess();
    void updateValue();

private:
    QProcess *m_process = nullptr;
    Ui::ExtUpgrade *ui = nullptr;
    void translate();
    // properties
    QString m_executable = "/usr/bin/true";
    QString m_filter = "";
    int m_null = 0;
    // internal properties
    QVariantHash m_values;
};


#endif /* EXTUPGRADE_H */
