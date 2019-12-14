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
    explicit ExtendedSysMon(QObject *_parent, const QVariantList &_args);
    ~ExtendedSysMon() override;

protected:
    QStringList sources() const override;
    bool sourceRequestEvent(const QString &_source) override;
    bool updateSourceEvent(const QString &_source) override;

private:
    // configuration
    ExtSysMonAggregator *m_aggregator = nullptr;
    QHash<QString, QString> m_configuration;
    // methods
    void readConfiguration();
    QHash<QString, QString> updateConfiguration(QHash<QString, QString> _rawConfig) const;
};


#endif /* EXTSYSMON_H */
