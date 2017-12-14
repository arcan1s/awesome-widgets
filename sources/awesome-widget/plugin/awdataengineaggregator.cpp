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


AWDataEngineAggregator::AWDataEngineAggregator(QObject *_parent)
    : QObject(_parent)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_consumer = new Plasma::DataEngineConsumer();
    m_dataEngines["systemmonitor"] = m_consumer->dataEngine("systemmonitor");
    m_dataEngines["extsysmon"] = m_consumer->dataEngine("extsysmon");
    m_dataEngines["time"] = m_consumer->dataEngine("time");

    // additional method required by systemmonitor structure
    m_newSourceConnection
        = connect(m_dataEngines["systemmonitor"], &Plasma::DataEngine::sourceAdded,
                  [this](const QString source) {
                      emit(deviceAdded(source));
                      m_dataEngines["systemmonitor"]->connectSource(source, parent(), 1000);
                  });

    // required to define Qt::QueuedConnection for signal-slot connection
    qRegisterMetaType<Plasma::DataEngine::Data>("Plasma::DataEngine::Data");
}


AWDataEngineAggregator::~AWDataEngineAggregator()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    disconnectSources();
}


void AWDataEngineAggregator::disconnectSources()
{
    for (auto dataengine : m_dataEngines.values())
        for (auto &source : dataengine->sources())
            dataengine->disconnectSource(source, parent());
    disconnect(m_newSourceConnection);
}


void AWDataEngineAggregator::reconnectSources(const int _interval)
{
    qCDebug(LOG_AW) << "Reconnect sources with interval" << _interval;

    disconnectSources();

    m_dataEngines["systemmonitor"]->connectAllSources(parent(), _interval);
    m_dataEngines["extsysmon"]->connectAllSources(parent(), _interval);
    m_dataEngines["time"]->connectSource("Local", parent(), 1000);

    m_newSourceConnection
        = connect(m_dataEngines["systemmonitor"], &Plasma::DataEngine::sourceAdded,
                  [this, _interval](const QString source) {
                      emit(deviceAdded(source));
                      m_dataEngines["systemmonitor"]->connectSource(source, parent(), _interval);
                  });

#ifdef BUILD_FUTURE
    createQueuedConnection();
#endif /* BUILD_FUTURE */
}


void AWDataEngineAggregator::dropSource(const QString &_source)
{
    qCDebug(LOG_AW) << "Source" << _source;

    // HACK there is no possibility to check to which dataengine source
    // connected we will try to disconnect it from all engines
    for (auto dataengine : m_dataEngines.values())
        dataengine->disconnectSource(_source, parent());
}


void AWDataEngineAggregator::createQueuedConnection()
{
    // HACK additional method which forces QueuedConnection instead of Auto one
    // for more details refer to plasma-framework source code
    for (auto &dataEngine : m_dataEngines.keys()) {
        // different source set for different engines
        QStringList sources = dataEngine == "time" ? QStringList() << "Local"
                                                   : m_dataEngines[dataEngine]->sources();
        // reconnect sources
        for (auto &source : sources) {
            Plasma::DataContainer *container
                = m_dataEngines[dataEngine]->containerForSource(source);
            // disconnect old connections first
            disconnect(container, SIGNAL(dataUpdated(QString, Plasma::DataEngine::Data)), parent(),
                       SLOT(dataUpdated(QString, Plasma::DataEngine::Data)));
            // and now reconnect with Qt::QueuedConnection type
            connect(container, SIGNAL(dataUpdated(QString, Plasma::DataEngine::Data)), parent(),
                    SLOT(dataUpdated(QString, Plasma::DataEngine::Data)), Qt::QueuedConnection);
        }
    }
}
