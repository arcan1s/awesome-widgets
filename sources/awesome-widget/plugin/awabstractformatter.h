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

#ifndef AWABSTRACTFORMATTER_H
#define AWABSTRACTFORMATTER_H

#include <QObject>
#include <QVariant>


class AWAbstractFormatter : public QObject
{
public:
    explicit AWAbstractFormatter(QObject *parent, const QString, const QString)
        : QObject(parent){};
    explicit AWAbstractFormatter(QObject *parent)
        : QObject(parent){};
    virtual ~AWAbstractFormatter(){};
    virtual QString convert(const QVariant &value) const = 0;
};


#endif /* AWABSTRACTFORMATTER_H */
