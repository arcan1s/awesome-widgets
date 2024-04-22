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

#include <QLocale>
#include <QVariant>

#include <memory>

#include "awpluginformatsettings.h"


class AWPluginFormaterInterface {
public:
    virtual ~AWPluginFormaterInterface() = default;
    virtual QString format(const QVariant &_value, const QString &_key, const AWPluginFormatSettings &_settings) const = 0;
    virtual void load() {};
};


template<typename Formatter> class AWPluginFormatter : public AWPluginFormaterInterface {

public:
    static constexpr double KBinBytes = 1024.0;
    static constexpr double MBinBytes = 1024.0 * KBinBytes;
    static constexpr double GBinBytes = 1024.0 * MBinBytes;

    AWPluginFormatter(AWPluginFormatter &) = delete;
    void operator=(const AWPluginFormatter &) = delete;

    static Formatter *instance()
    {
        static auto instance = loadInstance();
        return instance.get();
    };
    static QLocale locale(const AWPluginFormatSettings &_settings)
    {
        return _settings.translate ? QLocale::system() : QLocale::c();
    };

protected:
    AWPluginFormatter() = default;
    static std::unique_ptr<Formatter> loadInstance()
    {
        auto instance = std::make_unique<Formatter>();
        instance->load();
        return instance;
    };
};
