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

#ifndef HDDTEMPSOURCE_H
#define HDDTEMPSOURCE_H

#include <QObject>

#include "abstractextsysmonsource.h"


class HDDTemperatureSource : public AbstractExtSysMonSource
{
public:
    explicit HDDTemperatureSource(QObject *parent, const QStringList args);
    virtual ~HDDTemperatureSource();
    QVariant data(QString source);
    QVariantMap initialData(QString source) const;
    void run(){};
    QStringList sources() const;

private:
    // configuration and values
    QString m_cmd;
    QStringList m_devices;
    bool m_smartctl;
};


#endif /* HDDTEMPSOURCE_H */
