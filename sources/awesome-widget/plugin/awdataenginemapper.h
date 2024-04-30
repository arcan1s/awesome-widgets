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

#include <QMultiHash>
#include <QObject>

#include "formatters/awpluginformatter.h"
#include "matchers/awpluginmatchersettings.h"


class AWFormatterHelper;

class AWDataEngineMapper : public QObject
{
    Q_OBJECT

public:
    explicit AWDataEngineMapper(QObject *_parent = nullptr, AWFormatterHelper *_custom = nullptr);
    ~AWDataEngineMapper() override = default;
    // get methods
    [[nodiscard]] AWPluginFormaterInterface *formatter(const QString &_key) const;
    [[nodiscard]] QStringList keysFromSource(const QString &_source) const;
    // set methods
    QStringList registerSource(const QString &_source, KSysGuard::Unit _units, const QStringList &_keys);
    void setDevices(const AWPluginMatcherSettings &_settings);

private:
    AWFormatterHelper *m_customFormatters = nullptr;
    AWPluginMatcherSettings m_settings;
    // variables
    QHash<QString, QStringList> m_devices;
    QHash<QString, AWPluginFormaterInterface *> m_formatter;
    QMultiHash<QString, QString> m_map;
};
