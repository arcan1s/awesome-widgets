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

#ifndef AWLISTFORMATTER_H
#define AWLISTFORMATTER_H

#include "awabstractformatter.h"


namespace Ui
{
class AWListFormatter;
}

class AWListFormatter : public AWAbstractFormatter
{
    Q_OBJECT
    Q_PROPERTY(QString filter READ filter WRITE setFilter)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator)
    Q_PROPERTY(bool sorted READ isSorted WRITE setSorted)

public:
    explicit AWListFormatter(QWidget *_parent = nullptr, const QString &_filePath = "");
    ~AWListFormatter() override;
    QString convert(const QVariant &_value) const override;
    AWListFormatter *copy(const QString &_fileName, const int _number) override;
    // properties
    QString filter() const;
    bool isSorted() const;
    QString separator() const;
    void setFilter(const QString &_filter);
    void setSeparator(const QString &_separator);
    void setSorted(const bool _sorted);

public slots:
    void readConfiguration() override;
    int showConfiguration(const QVariant &_args) override;
    void writeConfiguration() const override;

private:
    Ui::AWListFormatter *ui = nullptr;
    void translate() override;
    // properties
    QString m_filter = "";
    QString m_separator = "";
    bool m_sorted = false;
    QRegExp m_regex;
};


#endif /* AWLISTFORMATTER_H */
