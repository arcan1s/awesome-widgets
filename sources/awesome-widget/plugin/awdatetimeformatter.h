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


class AWDateTimeFormatter : public AWAbstractFormatter
{
    Q_OBJECT
    Q_PROPERTY(QString format READ format WRITE setFormat)

public:
    explicit AWDateTimeFormatter(QObject *parent, const QString filename,
                                 const QString section);
    explicit AWDateTimeFormatter(QObject *parent, const QString format);
    virtual ~AWDateTimeFormatter();
    QString convert(const QVariant &value) const;
    // properties
    QString format() const;
    void setFormat(const QString _format);

private:
    void init(const QString filename, const QString section);
    // properties
    QString m_format;
};


#endif /* AWDATETIMEFORMATTER_H */
