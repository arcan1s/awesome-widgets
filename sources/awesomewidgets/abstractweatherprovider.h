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

#ifndef ABSTRACTWEATHERPROVIDER_H
#define ABSTRACTWEATHERPROVIDER_H

#include <QObject>
#include <QUrl>


class AbstractWeatherProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int number READ number)

public:
    explicit AbstractWeatherProvider(QObject *parent, const int number)
        : QObject(parent)
        , m_number(number){};
    virtual ~AbstractWeatherProvider(){};
    virtual void initUrl(const QString city, const QString country,
                         const int ts)
        = 0;
    virtual QVariantHash parse(const QVariantMap &json) const = 0;
    virtual QUrl url() const = 0;
    int number() const { return m_number; };

private:
    int m_number;
};


#endif /* ABSTRACTWEATHERPROVIDER_H */
