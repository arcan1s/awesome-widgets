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

#include <ksysguard/systemstats/SensorInfo.h>

#include <QObject>
#include <QRegularExpression>
#include <QVariant>

class AbstractExtSysMonSource : public QObject
{
    Q_OBJECT

public:
    inline static QRegularExpression NUMBER_REGEX = QRegularExpression("\\d+$");

    explicit AbstractExtSysMonSource(QObject *_parent);
    ~AbstractExtSysMonSource() override = default;
    virtual QVariant data(const QString &_source) = 0;
    [[nodiscard]] virtual QHash<QString, KSysGuard::SensorInfo *> sources() const = 0;
    static int index(const QString &_source);
    // safe value extractor
    template <class T> static QVariantHash dataByItem(T *_extension, const QString &_source)
    {
        auto idx = index(_source);
        if (idx == -1)
            return {};

        auto item = _extension->itemByTagNumber(idx);
        return item ? item->run() : QVariantHash();
    }
    static KSysGuard::SensorInfo *makeSensorInfo(const QString &_name, QMetaType::Type _type,
                                                 KSysGuard::Unit _unit = KSysGuard::UnitNone, double _min = 0,
                                                 double _max = 0);

signals:
    void dataReceived(const QVariantHash &);
};
