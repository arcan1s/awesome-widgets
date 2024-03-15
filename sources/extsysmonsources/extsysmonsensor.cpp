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

#include "abstractextsysmonsource.h"
#include "awdebug.h"


ExtSysMonSensor::ExtSysMonSensor(KSysGuard::SensorContainer *_parent, const QString &_id,
                                 AbstractExtSysMonSource *_source)
    : KSysGuard::SensorObject(_id, _parent)
{
    qCDebug(LOG_ESS) << __PRETTY_FUNCTION__;

    m_source = _source;

    for (auto &source : m_source->sources()) {
        auto property = new KSysGuard::SensorProperty(source, this);

        auto info = m_source->initialData(source);
        property->setName(info->name);

        property->setUnit(info->unit);
        property->setVariantType(info->variantType);

        property->setMin(info->min);
        property->setMax(info->max);

        m_properties[source] = property;
    }
}


void ExtSysMonSensor::update()
{
    for (auto &source : m_properties.keys()) {
        auto value = m_source->data(source);
        m_properties[source]->setValue(value);
    }
}
