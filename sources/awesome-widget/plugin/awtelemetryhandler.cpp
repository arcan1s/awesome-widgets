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

#include "awtelemetryhandler.h"

#include <QJsonDocument>
#include <QSettings>
#include <QStandardPaths>
#include <QUuid>

#include "awdebug.h"


AWTelemetryHandler::AWTelemetryHandler(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_localFile = QString("%1/awesomewidgets/telemetry.ini")
                      .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
}


QStringList AWTelemetryHandler::get(const QString &_group) const
{
    qCDebug(LOG_AW) << "Get stored data for group" << _group;

    QStringList values;

    QSettings settings(m_localFile, QSettings::IniFormat);
    settings.beginGroup(_group);
    for (auto &key : settings.childKeys())
        values.append(settings.value(key).toString());
    settings.endGroup();

    return values;
}


QString AWTelemetryHandler::getLast(const QString &_group) const
{
    qCDebug(LOG_AW) << "Get last stored data for group" << _group;

    return get(_group).last();
}


void AWTelemetryHandler::init(const int _count)
{
    qCDebug(LOG_AW) << "Init telemetry with count" << _count;

    m_storeCount = _count;
}


bool AWTelemetryHandler::put(const QString &_group, const QString &_value) const
{
    qCDebug(LOG_AW) << "Store data with group" << _group << "and value" << _value;

    QSettings settings(m_localFile, QSettings::IniFormat);
    settings.beginGroup(_group);
    // values will be stored as num=value inside specified group
    // load all values to memory
    QStringList saved;
    for (auto &key : settings.childKeys())
        saved.append(settings.value(key).toString());
    // check if this value is already saved
    if (saved.contains(_value)) {
        qCInfo(LOG_AW) << "Configuration" << _value << "for group" << _group << "is already saved";
        return false;
    }
    saved.append(_value);
    // remove old ones
    while (saved.count() > m_storeCount)
        saved.takeFirst();
    // clear group
    settings.remove("");
    // and save now
    for (auto &val : saved) {
        auto key = getKey(settings.childKeys().count());
        settings.setValue(key, val);
    }

    // sync settings
    settings.endGroup();
    settings.sync();
    // return status
    return (settings.status() == QSettings::NoError);
}


QString AWTelemetryHandler::getKey(const int _count)
{
    qCDebug(LOG_AW) << "Get key for keys count" << _count;

    return QString("%1").arg(_count, 3, 10, QChar('0'));
}
