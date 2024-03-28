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

#include "extsysmonsensor.h"

#include <QTimer>

#include "abstractextsysmonsource.h"
#include "awdebug.h"


ExtSysMonSensor::ExtSysMonSensor(KSysGuard::SensorContainer *_parent, const QString &_id, const QString &_name,
                                 AbstractExtSysMonSource *_source)
    : KSysGuard::SensorObject(_id, _name, _parent)
    , m_source(_source)
    , m_timer(new QTimer(this))
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    loadProperties();

    connect(this, &SensorObject::subscribedChanged, this, &ExtSysMonSensor::changeSubscription);
    connect(m_timer, &QTimer::timeout, this, &ExtSysMonSensor::update);
}


ExtSysMonSensor::~ExtSysMonSensor()
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_timer->stop();
    m_timer->deleteLater();
}


void ExtSysMonSensor::changeSubscription(bool _subscribed)
{
    qCDebug(LOG_ESS) << "Subscription changed to" << _subscribed;

    if (_subscribed) {
        m_timer->start(1000);
    } else {
        m_timer->stop();
    }
}


void ExtSysMonSensor::update()
{
    for (auto sensor : sensors()) {
        if (!sensor->isSubscribed())
            continue; // skip properties which are not explicitly subscribed

        auto value = m_source->data(sensor->id());
        sensor->setValue(value);
    }
}


void ExtSysMonSensor::loadProperties()
{
    auto sensors = m_source->sources();
    for (auto sensor = sensors.cbegin(); sensor != sensors.cend(); ++sensor) {
        auto source = sensor.key();
        auto info = sensor.value();
        auto property = new KSysGuard::SensorProperty(source, info->name, this);

        property->setUnit(info->unit);
        property->setVariantType(info->variantType);

        property->setMin(info->min);
        property->setMax(info->max);

        addProperty(property);
    }
}
