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

#include <Plasma/DataEngineConsumer>

#include <QTimer>

#include "awdebug.h"
#include "awkeys.h"


AWDataEngineAggregator::AWDataEngineAggregator(QObject *parent, const int interval)
    : QObject(parent)
{
    qCDebug(LOG_AW);

    // timer events
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);

    setInterval(interval);
    initDataEngines();
    connectVisualization();

    connect(this, SIGNAL(startTimer()), m_timer, SLOT(start()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(connectVisualization()));
}


AWDataEngineAggregator::~AWDataEngineAggregator()
{
    qCDebug(LOG_AW);

    m_dataEngines.clear();
    if (m_timer != nullptr) delete m_timer;
}


void AWDataEngineAggregator::setInterval(const int _interval)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Interval" << _interval;

    m_interval = _interval;
    m_timer->setInterval(5 * _interval);
}


void AWDataEngineAggregator::connectVisualization()
{
    qCDebug(LOG_AW);

//     reconnectSources();
    connect(this, SIGNAL(updateData(QString, QVariant, QString )),
            parent(), SLOT(dataUpdated(QString, QVariant, QString)));

    return static_cast<AWKeys *>(parent())->unlock();
}


void AWDataEngineAggregator::disconnectVisualization()
{
    qCDebug(LOG_AW);

    disconnect(this, SIGNAL(updateData(QString, QVariant, QString)),
               parent(), SLOT(dataUpdated(QString, QVariant, QString)));
//     m_dataEngines.clear();

    // HACK run timer in the main thread since a timer could not be started from
    // the different thread
    return emit(startTimer());
}


void AWDataEngineAggregator::dropSource(const QString source)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Source" << source;

    // FIXME there is no possiblibity to check to which dataengine source connected
    // we will try to disconnect it from systemmonitor and extsysmon
    m_dataEngines[QString("systemmonitor")]->disconnectSource(source, this);
    m_dataEngines[QString("extsysmon")]->disconnectSource(source, this);
}


void AWDataEngineAggregator::reconnectSources()
{
    qCDebug(LOG_AW);

    m_dataEngines[QString("systemmonitor")]->connectAllSources(this, m_interval);
    m_dataEngines[QString("extsysmon")]->connectAllSources(this, m_interval);
    m_dataEngines[QString("time")]->connectSource(QString("Local"), this, 1000);
}


void AWDataEngineAggregator::dataUpdated(const QString sourceName, const Plasma::DataEngine::Data data)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Source" << sourceName;
    qCDebug(LOG_AW) << "Data" << data;

    // HACK "deep copy" of data to avoid plasma crash on Data object destruction
    QString units = data[QString("units")].toString();
    // HACK workaround for time values which are stored in the different path
    QVariant value = sourceName == QString("Local") ? data[QString("DateTime")] : data[QString("value")];

    emit(updateData(sourceName, value, units));
}


void AWDataEngineAggregator::initDataEngines()
{
    qCDebug(LOG_AW);

    Plasma::DataEngineConsumer *deConsumer = new Plasma::DataEngineConsumer();
    m_dataEngines[QString("systemmonitor")] = deConsumer->dataEngine(QString("systemmonitor"));
    m_dataEngines[QString("extsysmon")] = deConsumer->dataEngine(QString("extsysmon"));
    m_dataEngines[QString("time")] = deConsumer->dataEngine(QString("time"));

    // additional method required by systemmonitor structure
    connect(m_dataEngines[QString("systemmonitor")], &Plasma::DataEngine::sourceAdded,
            [this](const QString source) {
                static_cast<AWKeys *>(parent())->addDevice(source);
                m_dataEngines[QString("systemmonitor")]->connectSource(source, this, m_interval);
            });
}
