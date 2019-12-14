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

#ifndef EXTSYSMONAGGREGATOR_H
#define EXTSYSMONAGGREGATOR_H

#include <QObject>

#include "abstractextsysmonsource.h"


class ExtSysMonAggregator : public QObject
{
    Q_OBJECT

public:
    explicit ExtSysMonAggregator(QObject *_parent, const QHash<QString, QString> &_config);
    ~ExtSysMonAggregator() override;
    QVariant data(const QString &_source) const;
    bool hasSource(const QString &_source) const;
    QVariantMap initialData(const QString &_source) const;
    QStringList sources() const;

private:
    void init(const QHash<QString, QString> &_config);
    QHash<QString, AbstractExtSysMonSource *> m_map;
};


#endif /* EXTSYSMONAGGREGATOR_H */
