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

#include <ksysguard/formatter/Unit.h>

#include <QHash>

#include <memory>

#include "awpluginmatchersettings.h"


class AWPluginFormaterInterface;

class AWPluginMatcherInterface
{
public:
    virtual ~AWPluginMatcherInterface() = default;
    [[nodiscard]] virtual QHash<QString, AWPluginFormaterInterface *>
    keys(const QString &_source, KSysGuard::Unit _units, const AWPluginMatcherSettings &_settings) const = 0;
    [[nodiscard]] virtual bool matches(const QString &_source) const = 0;
};


template <typename Matcher> class AWPluginMatcher : public AWPluginMatcherInterface
{
public:
    AWPluginMatcher(AWPluginMatcher &) = delete;
    void operator=(const AWPluginMatcher &) = delete;

    [[nodiscard]] static Matcher *instance()
    {
        static auto instance = std::make_unique<Matcher>();
        return instance.get();
    };

    [[nodiscard]] static QString device(const QString &_source) { return _source.split('/')[1]; };

    [[nodiscard]] static qsizetype index(const QString &_source, const QStringList &_devices)
    {
        auto device = AWPluginMatcher::device(_source);
        return _devices.indexOf(device);
    }

protected:
    AWPluginMatcher() = default;
};
