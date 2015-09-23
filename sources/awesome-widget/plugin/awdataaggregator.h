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


#ifndef AWTOOLTIP_H
#define AWTOOLTIP_H

#include <QObject>
#include <QVariant>


class QGraphicsScene;
class QGraphicsView;
class QPixmap;
class QThreadPool;

class AWDataAggregator : public QObject
{
    Q_OBJECT

public:
    explicit AWDataAggregator(QObject *parent = nullptr, QThreadPool *pThreadPool = nullptr);
    virtual ~AWDataAggregator();
    QList<float> getData(const QString key) const;
    QString htmlImage(const QPixmap source) const;
    void setParameters(QVariantMap settings);
    QPixmap tooltipImage();

signals:
    void updateData(const QHash<QString, QString> values);
    void toolTipPainted(const QString image) const;

public slots:
    void dataUpdate(const QHash<QString, QString> values);

private:
    // ui
    QGraphicsScene *toolTipScene = nullptr;
    QGraphicsView *toolTipView = nullptr;
    void checkValue(const QString source, const float value, const float extremum) const;
    void checkValue(const QString source, const QString current, const QString received) const;
    void initScene();
    QString notificationText(const QString source, const float value) const;
    QString notificationText(const QString source, const QString value) const;
    // main method
    void setData(const QHash<QString, QString> values);
    void setData(const QString source, float value, const float extremum = -1.0);
    // different signature for battery device
    void setData(const bool dontInvert, const QString source, float value);
    // variables
    int counts = 0;
    QVariantHash configuration;
    float currentGPULoad = 0.0;
    QString currentNetworkDevice = QString("lo");
    QHash<QString, float> boundaries;
    QHash<QString, QList<float>> data;
    bool m_enablePopup = false;
    QStringList requiredKeys;
    QThreadPool *threadPool = nullptr;
};


#endif /* AWTOOLTIP_H */
