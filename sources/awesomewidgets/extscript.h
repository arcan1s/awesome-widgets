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

#include <QProcess>

#include "abstractextitem.h"


namespace Ui
{
class ExtScript;
}

class ExtScript : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(QString executable READ executable WRITE setExecutable)
    Q_PROPERTY(QStringList filters READ filters WRITE setFilters)
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)
    Q_PROPERTY(Redirect redirect READ redirect WRITE setRedirect)

public:
    enum class Redirect { stdout2stderr = 0, nothing = 1, stderr2stdout = 2, swap = 3 };

    explicit ExtScript(QWidget *parent, const QString filePath = QString());
    virtual ~ExtScript();
    ExtScript *copy(const QString _fileName, const int _number);
    // get methods
    QString executable() const;
    QStringList filters() const;
    QString prefix() const;
    Redirect redirect() const;
    QString uniq() const;
    // derivatives
    QString strRedirect() const;
    // set methods
    void setExecutable(const QString _executable = QString("/usr/bin/true"));
    void setFilters(const QStringList _filters = QStringList());
    void setPrefix(const QString _prefix = QString(""));
    void setRedirect(const Redirect _redirect = Redirect::nothing);
    void setStrRedirect(const QString _redirect = QString("nothing"));
    // filters
    QString applyFilters(QString _value) const;
    void updateFilter(const QString _filter, const bool _add = true);

public slots:
    void readConfiguration();
    void readJsonFilters();
    QVariantHash run();
    int showConfiguration(const QVariant args = QVariant());
    void writeConfiguration() const;

private slots:
    void updateValue();

private:
    QProcess *process = nullptr;
    Ui::ExtScript *ui = nullptr;
    void translate();
    // properties
    QString m_executable = QString("/usr/bin/true");
    QStringList m_filters = QStringList();
    QString m_prefix = QString("");
    Redirect m_redirect = Redirect::nothing;
    // internal properties
    QVariantMap jsonFilters = QVariantMap();
    int times = 0;
    QVariantHash value;
};


#endif /* EXTSCRIPT_H */
