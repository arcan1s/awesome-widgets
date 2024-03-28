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

    explicit AbstractExtSysMonSource(QObject *_parent)
        : QObject(_parent){};
    ~AbstractExtSysMonSource() override = default;
    virtual QVariant data(const QString &_source) = 0;
    [[nodiscard]] virtual QHash<QString, KSysGuard::SensorInfo *> sources() const = 0;

    // used by extensions
    // This method returns -1 in case of invalid source name (like if there is no number)
    static int index(const QString &_source)
    {
        auto match = NUMBER_REGEX.match(_source);
        return match.hasMatch() ? match.captured().toInt() : -1;
    }

    // safe value extractor
    template <class T> static QVariantHash dataByItem(T *_extension, const QString &_source)
    {
        auto idx = index(_source);
        if (idx == -1)
            return {};

        auto item = _extension->itemByTagNumber(idx);
        return item ? item->run() : QVariantHash();
    }

    static KSysGuard::SensorInfo *makeSensorInfo(const QString &_name, const QVariant::Type type,
                                                 const KSysGuard::Unit unit = KSysGuard::UnitNone, const double min = 0,
                                                 const double max = 0)
    {
        auto info = new KSysGuard::SensorInfo();
        info->name = _name;
        info->variantType = type;

        info->unit = unit;

        info->min = min;
        info->max = max;

        return info;
    }

signals:
    void dataReceived(const QVariantHash &);
};
