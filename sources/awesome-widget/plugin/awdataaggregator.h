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
    explicit AWDataAggregator(QObject *_parent = nullptr);
    ~AWDataAggregator() override;
    static QString htmlImage(const QPixmap &_source);
    void setParameters(const QVariantMap &_settings);
    QPixmap tooltipImage();

public slots:
    void dataUpdate(const QVariantHash &_values);

signals:
    void toolTipPainted(const QString &_image) const;

private:
    // ui
    QGraphicsScene *m_toolTipScene = nullptr;
    QGraphicsView *m_toolTipView = nullptr;
    void checkValue(const QString &_source, float _value, float _extremum) const;
    void checkValue(const QString &_source, const QString &_current,
                    const QString &_received) const;
    void initScene();
    static QString notificationText(const QString &_source, float _value);
    static QString notificationText(const QString &_source, const QString &_value);
    // main method
    void setData(const QVariantHash &_values);
    void setData(const QString &_source, float _value, float _extremum = -1.0f);
    // different signature for battery device
    void setData(bool _dontInvert, const QString &_source, float _value);
    // variables
    int m_counts = 0;
    QVariantHash m_configuration;
    QString m_currentNetworkDevice = "lo";
    QHash<QString, float> m_boundaries;
    QHash<QString, QList<float>> m_values;
    bool m_enablePopup = false;
    QStringList requiredKeys;
};


#endif /* AWTOOLTIP_H */
