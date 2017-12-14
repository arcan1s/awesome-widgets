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

#include "abstractextitemaggregator.h"
#include "awabstractformatter.h"
#include "awabstractpairhelper.h"


class AWFormatterHelper : public AbstractExtItemAggregator, public AWAbstractPairHelper
{
    Q_OBJECT

public:
    explicit AWFormatterHelper(QWidget *_parent = nullptr);
    virtual ~AWFormatterHelper();
    // read-write methods
    void initItems();
    // methods
    QString convert(const QVariant &_value, const QString &_name) const;
    QStringList definedFormatters() const;
    QList<AbstractExtItem *> items() const;
    // configuration related
    virtual void editPairs();
    virtual QStringList leftKeys();
    virtual QStringList rightKeys();

public slots:
    void editItems();

private:
    // methods
    AWAbstractFormatter::FormatterClass defineFormatterClass(const QString &_stringType) const;
    void initFormatters();
    QPair<QString, AWAbstractFormatter::FormatterClass>
    readMetadata(const QString &_filePath) const;
    // parent methods
    void doCreateItem();
    // properties
    QHash<QString, AWAbstractFormatter *> m_formatters;
    QHash<QString, AWAbstractFormatter *> m_formattersClasses;
};


#endif /* AWFORMATTERHELPER_H */
