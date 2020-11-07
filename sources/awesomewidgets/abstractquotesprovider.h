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

#ifndef ABSTRACTQUOTESPROVIDER_H
#define ABSTRACTQUOTESPROVIDER_H

#include <QObject>
#include <QUrl>

#include "abstractextitem.h"


class AbstractQuotesProvider : public QObject
{
    Q_OBJECT

public:
    explicit AbstractQuotesProvider(QObject *_parent)
        : QObject(_parent){};
    ~AbstractQuotesProvider() override = default;
    virtual void initUrl(const QString &_asset) = 0;
    [[nodiscard]] virtual QVariantHash parse(const QByteArray &_source,
                                             const QVariantHash &_oldValues) const = 0;
    [[nodiscard]] QString tag(const QString &_type) const
    {
        return dynamic_cast<AbstractExtItem *>(parent())->tag(_type);
    };
    [[nodiscard]] virtual QUrl url() const = 0;
};


#endif /* ABSTRACTQUOTESPROVIDER_H */
