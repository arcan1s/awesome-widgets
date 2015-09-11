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

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHash>
#include <QObject>
#include <QPixmap>
#include <QVariant>


class AWDataAggregator : public QObject
{
    Q_OBJECT

public:
    explicit AWDataAggregator(QObject *parent = nullptr, QVariantMap settings = QVariantMap());
    virtual ~AWDataAggregator();
    QSize getTooltipSize() const;
    QString htmlImage(const QPixmap source);
    QPixmap tooltipImage();

signals:
    void updateData(QHash<QString, QString> values);
    void toolTipPainted(QString image);

private slots:
    void dataUpdate(QHash<QString, QString> values);

private:
    // ui
    QGraphicsScene *toolTipScene = nullptr;
    QGraphicsView *toolTipView = nullptr;
    void checkValue(const QString source, const float value, const float extremum) const;
    void checkValue(const QString source,  const QString current, const QString received) const;
    QString notificationText(const QString source, const float value) const;
    QString notificationText(const QString source, const QString value) const;
    void setData(const QString source, float value, const float extremum = -1.0);
    // different signature for battery device
    void setData(const bool dontInvert, const QString source, float value);
    // variables
    int counts = 0;
    QVariantHash configuration;
    float currentGPULoad = 0.0;
    QString currentNetworkDevice;
    QHash<QString, float> boundaries;
    QHash<QString, QList<float>> data;
    bool enablePopup = false;
    QStringList requiredKeys;
    QSize size;
};


#endif /* AWTOOLTIP_H */
