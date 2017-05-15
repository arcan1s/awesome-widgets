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


#ifndef AWDATAENGINEAGGREGATOR_H
#define AWDATAENGINEAGGREGATOR_H

#include <Plasma/DataEngine>
#include <Plasma/DataEngineConsumer>

#include <QObject>


class AWDataEngineAggregator : public QObject
{
    Q_OBJECT

public:
    explicit AWDataEngineAggregator(QObject *_parent = nullptr);
    virtual ~AWDataEngineAggregator();
    void clear();
    void disconnectSources();
    void initDataEngines(const int _interval);

signals:
    void deviceAdded(const QString &_source);

public slots:
    void dropSource(const QString &_source);
    void reconnectSources(const int _interval);

private:
    void createQueuedConnection();
    Plasma::DataEngineConsumer *m_consumer = nullptr;
    QHash<QString, Plasma::DataEngine *> m_dataEngines;
};


#endif /* AWDATAENGINEAGGREGATOR_H */
