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

    setInterval(interval);
    initDataEngines();
    connectVisualization();
}


AWDataEngineAggregator::~AWDataEngineAggregator()
{
    qCDebug(LOG_AW);

    m_dataEngines.clear();
}


void AWDataEngineAggregator::setInterval(const int _interval)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Interval" << _interval;

    m_interval = _interval;
}


void AWDataEngineAggregator::connectVisualization()
{
    qCDebug(LOG_AW);

    connect(this, SIGNAL(updateData(QString, QVariantHash)),
            parent(), SLOT(dataUpdated(QString, QVariantHash)));

    return static_cast<AWKeys *>(parent())->unlock();
}


void AWDataEngineAggregator::disconnectVisualization()
{
    qCDebug(LOG_AW);

    disconnect(this, SIGNAL(updateData(QString, QVariantHash)),
               parent(), SLOT(dataUpdated(QString, QVariantHash)));

    return QTimer::singleShot(5 * m_interval, this, SLOT(connectVisualization()));
}


void AWDataEngineAggregator::dropSource(const QString source)
{
    qCDebug(LOG_AW);
    qCDebug(LOG_AW) << "Source" << source;

    // this method is only used for systemmonitor dataengine
    m_dataEngines[QString("systemmonitor")]->disconnectSource(source, this);
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

    return emit(updateData(QString(sourceName), qvariant_cast<QVariantHash>(data)));
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
