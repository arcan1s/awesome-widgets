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

#ifndef AWJSONFORMATTER_H
#define AWJSONFORMATTER_H

#include "awabstractformatter.h"


namespace Ui
{
class AWJsonFormatter;
}

class AWJsonFormatter : public AWAbstractFormatter
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath)

public:
    explicit AWJsonFormatter(QWidget *parent,
                             const QString &filePath = QString());
    virtual ~AWJsonFormatter();
    QString convert(const QVariant &_value) const;
    AWJsonFormatter *copy(const QString &_fileName, const int _number);
    // properties
    QString path() const;
    void setPath(const QString &_path);

public slots:
    void readConfiguration();
    int showConfiguration(const QVariant &args = QVariant());
    void writeConfiguration() const;

private:
    Ui::AWJsonFormatter *ui = nullptr;
    QVariant getFromJson(const QVariant &value, const QVariant &element) const;
    QVariant getFromList(const QVariant &value, const int index) const;
    QVariant getFromMap(const QVariant &value, const QString &key) const;
    void initPath();
    void translate();
    // properties
    QString m_path;
    QVariantList m_splittedPath;
};


#endif /* AWJSONFORMATTER_H */
