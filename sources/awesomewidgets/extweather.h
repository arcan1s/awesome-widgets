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

#ifndef EXTWEATHER_H
#define EXTWEATHER_H

#include <QNetworkReply>

#include "abstractextitem.h"

#define YAHOO_WEATHER_URL                                                      \
    "https://query.yahooapis.com/v1/public/yql?format=json&q=select * from "   \
    "weather.forecast where u='c' and woeid in (select woeid from "            \
    "geo.places(1) where text='%1, %2')"


namespace Ui
{
class ExtWeather;
}

class ExtWeather : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(QString city READ city WRITE setCity)
    Q_PROPERTY(QString country READ country WRITE setCountry)
    Q_PROPERTY(bool image READ image WRITE setImage)
    Q_PROPERTY(int ts READ ts WRITE setTs)

public:
    explicit ExtWeather(QWidget *parent = nullptr,
                        const QString weatherName = QString(),
                        const QStringList directories = QStringList());
    virtual ~ExtWeather();
    ExtWeather *copy(const QString _fileName, const int _number);
    QString weatherFromInt(const int _id) const;
    // get methods
    QString city() const;
    QString country() const;
    bool image() const;
    int ts() const;
    QString uniq() const;
    // set methods
    void setCity(const QString _city = QString("London"));
    void setCountry(const QString _country = QString("uk"));
    void setImage(const bool _image = false);
    void setTs(const int _ts = 0);

public slots:
    void readConfiguration();
    void readJsonMap();
    QVariantHash run();
    int showConfiguration(const QVariant args = QVariant());
    void writeConfiguration() const;

private slots:
    void weatherReplyReceived(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    bool isRunning = false;
    Ui::ExtWeather *ui;
    void translate();
    QString url() const;
    // properties
    QString m_city = QString("London");
    QString m_country = QString("uk");
    bool m_image = false;
    int m_ts = 0;
    QVariantMap jsonMap = QVariantMap();
    // values
    int times = 0;
    QVariantHash values;
};


#endif /* EXTWEATHER_H */
