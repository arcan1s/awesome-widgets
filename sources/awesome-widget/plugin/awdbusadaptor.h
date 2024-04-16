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

#include <QDBusAbstractAdaptor>

#include "version.h"


class AWKeys;

class AWDBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", AWDBUS_SERVICE_NAME)

public:
    explicit AWDBusAdaptor(AWKeys *_parent = nullptr);
    ~AWDBusAdaptor() override = default;

public slots:
    // get methods
    [[nodiscard]] static QStringList ActiveServices();
    [[nodiscard]] QString Info(const QString &key) const;
    [[nodiscard]] QStringList Keys(const QString &regexp) const;
    [[nodiscard]] QString Value(const QString &key) const;
    [[nodiscard]] qlonglong WhoAmI() const;
    // set methods
    void SetLogLevel(const QString &what, int level);
    void SetLogLevel(const QString &what, const QString &level, bool enabled);

private:
    AWKeys *m_plugin = nullptr;
    QStringList m_logLevels = {"debug", "info", "warning", "critical"};
};
