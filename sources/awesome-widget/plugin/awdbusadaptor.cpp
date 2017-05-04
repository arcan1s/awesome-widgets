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

#include "awdbusadaptor.h"

#include "awdebug.h"
#include "awkeys.h"


AWDBusAdaptor::AWDBusAdaptor(AWKeys *parent)
    : QDBusAbstractAdaptor(parent)
    , m_plugin(parent)
{
    qCDebug(LOG_DBUS) << __PRETTY_FUNCTION__;
}


AWDBusAdaptor::~AWDBusAdaptor()
{
    qCDebug(LOG_DBUS) << __PRETTY_FUNCTION__;
}


QString AWDBusAdaptor::Info(const QString &key) const
{
    return m_plugin->infoByKey(key);
}


QStringList AWDBusAdaptor::Keys(const QString &regexp) const
{
    return m_plugin->dictKeys(true, regexp);
}


QString AWDBusAdaptor::Value(const QString &key) const
{
    return m_plugin->valueByKey(key);
}


qlonglong AWDBusAdaptor::WhoAmI() const
{
    return reinterpret_cast<qlonglong>(m_plugin);
}


void AWDBusAdaptor::SetLogLevel(const QString &what, const int level)
{
    qCDebug(LOG_DBUS) << "Set log level" << level << "for" << what;

    if (level >= m_logLevels.count()) {
        qCDebug(LOG_DBUS) << "Invalid logging level" << level
                          << "should be less than" << m_logLevels.count();
        return;
    }

    for (auto &lev : m_logLevels)
        SetLogLevel(what, lev, m_logLevels.indexOf(lev) >= level);
}


void AWDBusAdaptor::SetLogLevel(const QString &what, const QString &level,
                                const bool enabled)
{
    qCDebug(LOG_DBUS) << "Set log level" << level << "enabled" << enabled
                      << "for" << what;

    if (!m_logLevels.contains(level)) {
        qCDebug(LOG_DBUS) << "Invalid logging level" << level << "should be in"
                          << m_logLevels;
        return;
    }

    QString state = enabled ? QString("true") : QString("false");
    QLoggingCategory::setFilterRules(
        QString("%1.%2=%3").arg(what).arg(level).arg(state));
}
