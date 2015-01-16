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


#ifndef AWADDS_H
#define AWADDS_H

#include <QMap>
#include <QObject>
#include <QStringList>
#include <QVariant>


class AWAdds : public QObject
{
    Q_OBJECT

public:
    AWAdds(QObject *parent = 0);
    ~AWAdds();

    Q_INVOKABLE QString networkDevice(const QString custom = QString(""));
    Q_INVOKABLE int numberCpus();
    Q_INVOKABLE float tempepature(const float temp, const QString units = QString("Celsius"));
    Q_INVOKABLE QStringList timeKeys();
    // dataengine
    Q_INVOKABLE QMap<QString, QVariant> readDataEngineConfiguration();
    Q_INVOKABLE void writeDataEngineConfiguration(const QMap<QString, QVariant> configuration);

private:
    QMap<QString, QVariant> updateDataEngineConfiguration(QMap<QString, QVariant> rawConfig);
    bool debug = false;
};


#endif /* AWADDS_H */
