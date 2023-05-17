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
    ~AWFormatterHelper() override;
    // read-write methods
    void initItems() override;
    // methods
    [[nodiscard]] QString convert(const QVariant &_value, const QString &_name) const;
    [[nodiscard]] QStringList definedFormatters() const;
    [[nodiscard]] QList<AbstractExtItem *> items() const override;
    // configuration related
    void editPairs() override;
    QStringList leftKeys() override;
    QStringList rightKeys() override;

public slots:
    void editItems();

private:
    // methods
    static AWAbstractFormatter::FormatterClass defineFormatterClass(const QString &_stringType);
    void initFormatters();
    [[nodiscard]] static QPair<QString, AWAbstractFormatter::FormatterClass> readMetadata(const QString &_filePath);
    // parent methods
    void doCreateItem() override;
    // properties
    QHash<QString, AWAbstractFormatter *> m_formatters;
    QHash<QString, AWAbstractFormatter *> m_formattersClasses;
};


#endif /* AWFORMATTERHELPER_H */
