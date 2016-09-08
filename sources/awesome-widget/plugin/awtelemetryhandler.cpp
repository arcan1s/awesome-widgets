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
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QStandardPaths>
#include <QUuid>

#include "awdebug.h"


AWTelemetryHandler::AWTelemetryHandler(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_genericConfig = QString("%1/awesomewidgets/general.ini")
                          .arg(QStandardPaths::writableLocation(
                              QStandardPaths::GenericDataLocation));
    m_localFile = QString("%1/awesomewidgets/telemetry.ini")
                      .arg(QStandardPaths::writableLocation(
                          QStandardPaths::GenericDataLocation));

    init();
}


AWTelemetryHandler::~AWTelemetryHandler()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


QStringList AWTelemetryHandler::get(const QString group) const
{
    qCDebug(LOG_AW) << "Get stored data for group" << group;

    QStringList values;

    QSettings settings(m_localFile, QSettings::IniFormat);
    settings.beginGroup(group);
    for (auto key : settings.childKeys())
        values.append(settings.value(key).toString());
    settings.endGroup();

    return values;
}


QString AWTelemetryHandler::getLast(const QString group) const
{
    qCDebug(LOG_AW) << "Get last stored data for group" << group;

    return get(group).last();
}


bool AWTelemetryHandler::put(const QString group, const QString value) const
{
    qCDebug(LOG_AW) << "Store data with group" << group << "and value" << value;

    QSettings settings(m_localFile, QSettings::IniFormat);
    settings.beginGroup(group);
    // values will be stored as num=value inside specified group
    QString key
        = QString("%1").arg(settings.childKeys().count(), 3, 10, QChar('0'));
    settings.setValue(key, value);
    settings.endGroup();

    // sync settings
    settings.sync();
    // check status
    if (settings.status() != QSettings::NoError)
        return false;
    // rotate
    return rotate();
}


bool AWTelemetryHandler::rotate() const
{
    QSettings settings(m_localFile, QSettings::IniFormat);

    for (auto group : settings.childGroups()) {
        QStringList data;
        settings.beginGroup(group);
        // load current data
        for (auto key : settings.childKeys())
            data.append(settings.value(key).toString());
        // remove first item from list
        while (data.count() > m_storeCount)
            data.takeFirst();
        // clear values
        settings.remove(QString(""));
        // save new values
        for (auto value : data) {
            QString key = QString("%1").arg(settings.childKeys().count(), 3, 10,
                                            QChar('0'));
            settings.setValue(key, value);
        }
        settings.endGroup();
    }

    // sync settings
    settings.sync();
    // return status
    return (settings.status() == QSettings::NoError);
}


void AWTelemetryHandler::init()
{
    QSettings settings(m_genericConfig, QSettings::IniFormat);
    settings.beginGroup(QString("Telemetry"));

    // unique client id
    m_clientId
        = settings.value(QString("ClientID"), QUuid::createUuid().toString())
              .toString();
    setConfiguration(QString("ClientID"), m_clientId, false);
    // count items to store
    m_storeCount = settings.value(QString("StoreHistory"), 100).toInt();
    setConfiguration(QString("StoreHistory"), m_storeCount, false);

    settings.endGroup();
}


bool AWTelemetryHandler::setConfiguration(const QString key,
                                          const QVariant value,
                                          const bool override) const
{
    qCDebug(LOG_AW) << "Set configuration key" << key << "to value" << value
                    << "force override enabled" << override;

    QSettings settings(m_genericConfig, QSettings::IniFormat);
    settings.beginGroup(QString("Telemetry"));
    if (settings.childKeys().contains(key) && !override)
        return true;

    settings.setValue(key, value);
    settings.endGroup();
    settings.sync();

    return (settings.status() == QSettings::NoError);
}
