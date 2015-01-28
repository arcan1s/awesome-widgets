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

#include <QMap>
#include <QObject>
#include <QPixmap>
#include <QVariant>


class AWToolTip : public QObject
{
    Q_OBJECT

public:
    AWToolTip(QObject *parent = 0,
              QMap<QString, QVariant> m_settings = QMap<QString, QVariant>());
    ~AWToolTip();

    Q_INVOKABLE QPixmap image();
    void setData(const QString source, const float value,
                 const bool ac = true);

private:
    // variables
    bool debug = false;
    QMap<QString, QVariant> m_settings;
    QMap<QString, QList<float>> data;
};


#endif /* AWTOOLTIP_H */
