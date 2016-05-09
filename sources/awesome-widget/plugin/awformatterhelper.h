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
    enum class FormatterClass { DateTime, Float, Script, NoFormat };

    explicit AWFormatterHelper(QObject *parent = nullptr);
    virtual ~AWFormatterHelper();
    QString convert(const QVariant &value, const QString name) const;
    QStringList definedFormatters() const;
    QStringList knownFormatters() const;

private:
    AWFormatterHelper::FormatterClass
    defineFormatterClass(const QString stringType) const;
    void initFormatters();
    void initKeys();
    void installDirectories();
    QPair<QString, AWFormatterHelper::FormatterClass>
    readMetadata(const QString filePath) const;
    // properties
    QStringList m_directories;
    QString m_formatterConfig;
    QHash<QString, AWAbstractFormatter *> m_formatters;
    QHash<QString, AWAbstractFormatter *> m_formattersClasses;
};


#endif /* AWFORMATTERHELPER_H */
