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

#ifndef AWDATETIMEFORMATTER_H
#define AWDATETIMEFORMATTER_H

#include "awabstractformatter.h"


namespace Ui
{
class AWDateTimeFormatter;
}

class AWDateTimeFormatter : public AWAbstractFormatter
{
    Q_OBJECT
    Q_PROPERTY(QString format READ format WRITE setFormat)

public:
    explicit AWDateTimeFormatter(QWidget *parent, const QString filePath);
    explicit AWDateTimeFormatter(const QString format, QWidget *parent);
    virtual ~AWDateTimeFormatter();
    QString convert(const QVariant &_value) const;
    AWDateTimeFormatter *copy(const QString _fileName, const int _number);
    // properties
    QString format() const;
    void setFormat(const QString _format);

public slots:
    void readConfiguration();
    int showConfiguration(const QVariant args = QVariant());
    void writeConfiguration() const;

private:
    Ui::AWDateTimeFormatter *ui;
    void translate();
    // properties
    QString m_format = QString();
};


#endif /* AWDATETIMEFORMATTER_H */
