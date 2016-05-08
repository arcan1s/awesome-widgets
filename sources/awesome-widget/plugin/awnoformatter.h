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

#ifndef AWNOFORMATTER_H
#define AWNOFORMATTER_H

#include "awabstractformatter.h"


class AWNoFormatter : public AWAbstractFormatter
{
    Q_OBJECT

public:
    explicit AWNoFormatter(QObject *parent, const QString filename,
                           const QString section);
    explicit AWNoFormatter(QObject *parent);
    virtual ~AWNoFormatter();
    QString convert(const QVariant &value) const;

private:
    void init(const QString filename, const QString section);
    // properties
};


#endif /* AWNOFORMATTER_H */
