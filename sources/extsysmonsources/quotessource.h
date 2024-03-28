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

#include <QObject>

#include "abstractextsysmonsource.h"
#include "extitemaggregator.h"


class ExtQuotes;

class QuotesSource : public AbstractExtSysMonSource
{
    Q_OBJECT

public:
    explicit QuotesSource(QObject *_parent);
    ~QuotesSource() override = default;
    QVariant data(const QString &_source) override;
    [[nodiscard]] QHash<QString, KSysGuard::SensorInfo *> sources() const override;

private:
    // configuration and values
    ExtItemAggregator<ExtQuotes> *m_extQuotes = nullptr;
    QVariantHash m_values;
};
