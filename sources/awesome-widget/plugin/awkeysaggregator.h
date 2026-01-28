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
#include <QObject>

#include "formatters/awpluginformatsettings.h"
#include "matchers/awpluginmatchersettings.h"


class AWDataEngineMapper;

class AWKeysAggregator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString acOffline READ acOffline WRITE setAcOffline);
    Q_PROPERTY(QString acOnline READ acOnline WRITE setAcOnline);
    Q_PROPERTY(QString customTime READ customTime WRITE setCustomTime);
    Q_PROPERTY(QString customUptime READ customUptime WRITE setCustomUptime);
    Q_PROPERTY(QString tempUnits READ tempUnits WRITE setTempUnits);
    Q_PROPERTY(bool translate READ translate WRITE setTranslate);

public:
    explicit AWKeysAggregator(QObject *_parent = nullptr);
    ~AWKeysAggregator() override = default;
    void initFormatters();
    // get methods
    [[nodiscard]] QString acOffline() const;
    [[nodiscard]] QString acOnline() const;
    [[nodiscard]] QString customTime() const;
    [[nodiscard]] QString customUptime() const;
    [[nodiscard]] QString formatter(const QVariant &_data, const QString &_key, bool replaceSpace) const;
    [[nodiscard]] QStringList keysFromSource(const QString &_source) const;
    [[nodiscard]] QString tempUnits() const;
    [[nodiscard]] bool translate() const;
    // set methods
    void setAcOffline(const QString &_inactive);
    void setAcOnline(const QString &_active);
    void setCustomTime(const QString &_customTime);
    void setCustomUptime(const QString &_customUptime);
    void setDevices(const AWPluginMatcherSettings &_settings);
    void setTempUnits(const QString &_units);
    void setTranslate(bool _translate);

public slots:
    QStringList registerSource(const QString &_source, KSysGuard::Unit _units, const QStringList &_keys,
                               const bool _disconnectUnused);

private:
    AWPluginFormatSettings m_settings;
    AWDataEngineMapper *m_mapper = nullptr;
};
