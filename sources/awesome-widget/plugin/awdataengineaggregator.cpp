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


AWDataEngineAggregator::AWDataEngineAggregator(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
}


AWDataEngineAggregator::~AWDataEngineAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    clear();
}


void AWDataEngineAggregator::clear()
{
    // disconnect sources first
    disconnectSources();
    m_dataEngines.clear();
    delete m_consumer;
}


void AWDataEngineAggregator::disconnectSources()
{
    for (auto dataengine : m_dataEngines.values())
        for (auto source : dataengine->sources())
            dataengine->disconnectSource(source, parent());
}


void AWDataEngineAggregator::initDataEngines(const int interval)
{
    qCDebug(LOG_AW) << "Init dataengines with interval" << interval;

    m_consumer = new Plasma::DataEngineConsumer();
    m_dataEngines[QString("systemmonitor")]
        = m_consumer->dataEngine(QString("systemmonitor"));
    m_dataEngines[QString("extsysmon")]
        = m_consumer->dataEngine(QString("extsysmon"));
    m_dataEngines[QString("time")] = m_consumer->dataEngine(QString("time"));

    // additional method required by systemmonitor structure
    connect(m_dataEngines[QString("systemmonitor")],
            &Plasma::DataEngine::sourceAdded,
            [this, interval](const QString source) {
                emit(deviceAdded(source));
                m_dataEngines[QString("systemmonitor")]->connectSource(
                    source, parent(), interval);
            });

    return reconnectSources(interval);
}


void AWDataEngineAggregator::dropSource(const QString source)
{
    qCDebug(LOG_AW) << "Source" << source;

    // HACK there is no possibility to check to which dataengine source
    // connected we will try to disconnect it from systemmonitor and extsysmon
    m_dataEngines[QString("systemmonitor")]->disconnectSource(source, parent());
    m_dataEngines[QString("extsysmon")]->disconnectSource(source, parent());
    m_dataEngines[QString("time")]->disconnectSource(source, parent());
}


void AWDataEngineAggregator::reconnectSources(const int interval)
{
    qCDebug(LOG_AW) << "Reconnect sources with interval" << interval;

    m_dataEngines[QString("systemmonitor")]->connectAllSources(parent(),
                                                               interval);
    m_dataEngines[QString("extsysmon")]->connectAllSources(parent(), interval);
    m_dataEngines[QString("time")]->connectSource(QString("Local"), parent(),
                                                  1000);
}
