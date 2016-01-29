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

#include "awdataengineaggregator.h"

#include "awdebug.h"
#include "awkeys.h"


AWDataEngineAggregator::AWDataEngineAggregator(QObject *parent,
                                               const int interval)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    setInterval(interval);
    initDataEngines();
}


AWDataEngineAggregator::~AWDataEngineAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    // disconnect sources first
    disconnectSources();
    m_dataEngines.clear();
    delete m_consumer;
}


void AWDataEngineAggregator::disconnectSources()
{
    for (auto dataengine : m_dataEngines.keys())
        for (auto source : m_dataEngines[dataengine]->sources())
            m_dataEngines[dataengine]->disconnectSource(source, parent());
}


void AWDataEngineAggregator::setInterval(const int _interval)
{
    qCDebug(LOG_AW) << "Interval" << _interval;

    m_interval = _interval;
}


void AWDataEngineAggregator::dropSource(const QString source)
{
    qCDebug(LOG_AW) << "Source" << source;

    // FIXME there is no possibility to check to which dataengine source
    // connected we will try to disconnect it from systemmonitor and extsysmon
    m_dataEngines[QString("systemmonitor")]->disconnectSource(source, parent());
    m_dataEngines[QString("extsysmon")]->disconnectSource(source, parent());
}


void AWDataEngineAggregator::reconnectSources()
{
    m_dataEngines[QString("systemmonitor")]->connectAllSources(parent(),
                                                               m_interval);
    m_dataEngines[QString("extsysmon")]->connectAllSources(parent(),
                                                           m_interval);
    m_dataEngines[QString("time")]->connectSource(QString("Local"), parent(),
                                                  1000);
}


void AWDataEngineAggregator::initDataEngines()
{
    m_consumer = new Plasma::DataEngineConsumer();
    m_dataEngines[QString("systemmonitor")]
        = m_consumer->dataEngine(QString("systemmonitor"));
    m_dataEngines[QString("extsysmon")]
        = m_consumer->dataEngine(QString("extsysmon"));
    m_dataEngines[QString("time")] = m_consumer->dataEngine(QString("time"));

    // additional method required by systemmonitor structure
    connect(m_dataEngines[QString("systemmonitor")],
            &Plasma::DataEngine::sourceAdded, [this](const QString source) {
                static_cast<AWKeys *>(parent())->addDevice(source);
                m_dataEngines[QString("systemmonitor")]->connectSource(
                    source, parent(), m_interval);
            });
}
