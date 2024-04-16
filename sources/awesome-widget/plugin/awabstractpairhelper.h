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

#pragma once

#include <QHash>


class AWAbstractPairHelper
{
public:
    explicit AWAbstractPairHelper(QString _filePath = "", QString _section = "");
    virtual ~AWAbstractPairHelper() = default;
    [[nodiscard]] QStringList keys() const;
    [[nodiscard]] QHash<QString, QString> pairs() const;
    [[nodiscard]] QStringList values() const;
    [[nodiscard]] QSet<QString> valuesSet() const;
    // read-write methods
    virtual void initItems();
    [[nodiscard]] virtual bool writeItems(const QHash<QString, QString> &_configuration) const;
    [[nodiscard]] virtual bool removeUnusedKeys(const QStringList &_keys) const;
    // configuration related
    virtual void editPairs() = 0;
    virtual QStringList leftKeys() = 0;
    virtual QStringList rightKeys() = 0;

private:
    // properties
    QHash<QString, QString> m_pairs;
    QString m_filePath;
    QString m_section;
};
