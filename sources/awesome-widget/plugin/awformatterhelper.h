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


#ifndef AWFORMATTERHELPER_H
#define AWFORMATTERHELPER_H

#include <QObject>

#include "awabstractformatter.h"


class AWFormatterHelper : public QObject
{
    Q_OBJECT

public:
    enum class FormatterClass { Float, DateTime, NoFormat };

    explicit AWFormatterHelper(QObject *parent = nullptr);
    virtual ~AWFormatterHelper();
    QString convert(const QVariant &value, const QString name) const;
    QStringList definedFormatters() const;

private:
    AWFormatterHelper::FormatterClass
    defineFormatterClass(const QString name) const;
    void init();
    // properties
    QString m_genericConfig;
    QHash<QString, AWAbstractFormatter *> m_formatters;
};


#endif /* AWFORMATTERHELPER_H */
