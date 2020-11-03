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
    Q_PROPERTY(Redirect redirect READ redirect WRITE setRedirect)

public:
    enum class Redirect { stdout2stderr = 0, nothing = 1, stderr2stdout = 2, swap = 3 };

    explicit ExtScript(QWidget *_parent = nullptr, const QString &_filePath = "");
    ~ExtScript() override;
    ExtScript *copy(const QString &_fileName, int _number) override;
    static QString jsonFiltersFile();
    // get methods
    [[nodiscard]] QString executable() const;
    [[nodiscard]] QStringList filters() const;
    [[nodiscard]] Redirect redirect() const;
    [[nodiscard]] QString uniq() const override;
    // derivatives
    [[nodiscard]] QString strRedirect() const;
    // set methods
    void setExecutable(const QString &_executable);
    void setFilters(const QStringList &_filters);
    void setRedirect(Redirect _redirect);
    void setStrRedirect(const QString &_redirect);
    // filters
    [[nodiscard]] QString applyFilters(QString _value) const;
    void updateFilter(const QString &_filter, bool _add);

public slots:
    void readConfiguration() override;
    void readJsonFilters();
    QVariantHash run() override;
    int showConfiguration(const QVariant &_args) override;
    void writeConfiguration() const override;

private slots:
    void startProcess();
    void updateValue();

private:
    QProcess *m_process = nullptr;
    Ui::ExtScript *ui = nullptr;
    void translate() override;
    // properties
    QString m_executable = "/usr/bin/true";
    QStringList m_filters = QStringList();
    QString m_prefix = "";
    Redirect m_redirect = Redirect::nothing;
    // internal properties
    QVariantMap m_jsonFilters;
    QVariantHash m_values;
};


#endif /* EXTSCRIPT_H */
