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

#ifndef EXTSYSMON_H
#define EXTSYSMON_H

#include <Plasma/DataEngine>


class ExtSysMonAggregator;

class ExtendedSysMon : public Plasma::DataEngine
{
    Q_OBJECT

public:
    explicit ExtendedSysMon(QObject *parent, const QVariantList &args);
    virtual ~ExtendedSysMon();

protected:
    QStringList sources() const;
    bool sourceRequestEvent(const QString &source);
    bool updateSourceEvent(const QString &source);

private:
    // configuration
    ExtSysMonAggregator *aggregator = nullptr;
    QHash<QString, QString> configuration;
    // methods
    void readConfiguration();
    QHash<QString, QString>
    updateConfiguration(QHash<QString, QString> rawConfig) const;
};


#endif /* EXTSYSMON_H */
