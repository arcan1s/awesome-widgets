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

#include <Plasma/DataContainer>

#include "awdebug.h"
#include "awkeys.h"


AWDataEngineAggregator::AWDataEngineAggregator(QObject *parent)
    : QObject(parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    // required to define Qt::QueuedConnection for signal-slot connection
    qRegisterMetaType<Plasma::DataEngine::Data>("Plasma::DataEngine::Data");
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

#ifdef BUILD_FUTURE
    createQueuedConnection();
#endif /* BUILD_FUTURE */
}


void AWDataEngineAggregator::createQueuedConnection()
{
    // HACK additional method which forces QueuedConnection instead of Auto one
    // for more details refer to plasma-framework source code
    for (auto dataEngine : m_dataEngines.keys()) {
        // different source set for different engines
        QStringList sources;
        if (dataEngine == QString("time"))
            sources.append(QString("Local"));
        else
            sources = m_dataEngines[dataEngine]->sources();
        // reconnect sources
        for (auto source : sources) {
            Plasma::DataContainer *container
                = m_dataEngines[dataEngine]->containerForSource(source);
            // disconnect old connections first
            disconnect(container,
                       SIGNAL(dataUpdated(QString, Plasma::DataEngine::Data)),
                       parent(),
                       SLOT(dataUpdated(QString, Plasma::DataEngine::Data)));
            // and now reconnect with Qt::QueuedConnection type
            connect(container,
                    SIGNAL(dataUpdated(QString, Plasma::DataEngine::Data)),
                    parent(),
                    SLOT(dataUpdated(QString, Plasma::DataEngine::Data)),
                    Qt::QueuedConnection);
        }
    }
}
