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
#include <QObject>


class QTimer;

class AWDataEngineAggregator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int interval MEMBER m_interval WRITE setInterval);

public:
    explicit AWDataEngineAggregator(QObject *parent = nullptr, const int interval = 1000);
    virtual ~AWDataEngineAggregator();
    // properties
    void setInterval(const int _interval);

signals:
    void startTimer();
    void updateData(const QString sourceName, const QVariantMap sdata);

public slots:
    // additional methods to control this and visualization
    void connectVisualization();
    void disconnectVisualization();
    void dropSource(const QString source);
    void reconnectSources();
    // data engines operations
    void dataUpdated(const QString sourceName, const Plasma::DataEngine::Data data);
    // dummy method required by DataEngine connections
    void modelChanged(QString, QAbstractItemModel *) {};

private:
    void initDataEngines();
    QHash<QString, Plasma::DataEngine *> m_dataEngines;
    int m_interval;
    QTimer *m_timer = nullptr;
};


#endif /* AWDATAENGINEAGGREGATOR_H */
