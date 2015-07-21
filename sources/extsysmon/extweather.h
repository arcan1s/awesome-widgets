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

#include <QDialog>
#include <QMap>
#include <QNetworkReply>

#define OWM_URL "http://api.openweathermap.org/data/2.5/weather?q=$CITY,$COUNTRY&units=metric"
#define OWM_FORECAST_URL "http://api.openweathermap.org/data/2.5/forecast?q=$CITY,$COUNTRY&units=metric"


namespace Ui {
    class ExtWeather;
}

class ExtWeather : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int apiVersion READ apiVersion WRITE setApiVersion)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString comment READ comment WRITE setComment)
    Q_PROPERTY(int interval READ interval WRITE setInterval)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_PROPERTY(int number READ number WRITE setNumber)
    Q_PROPERTY(QString city READ city WRITE setCity)
    Q_PROPERTY(QString country READ country WRITE setCountry)
    Q_PROPERTY(int ts READ ts WRITE setTs)

public:
    explicit ExtWeather(QWidget *parent = nullptr, const QString weatherName = QString(),
                        const QStringList directories = QStringList(), const bool debugCmd = false);
    ~ExtWeather();
    QString weatherFromInt(const int _id);
    // get methods
    int apiVersion();
    QString comment();
    QString fileName();
    int interval();
    bool isActive();
    QString name();
    int number();
    QString tag(const QString _type = QString("temperature"));
    QString city();
    QString country();
    int ts();
    // set methods
    void setApiVersion(const int _apiVersion = 0);
    void setActive(const bool _state = true);
    void setComment(const QString _comment = QString("empty"));
    void setInterval(const int _interval = 0);
    void setName(const QString _name = QString("none"));
    void setNumber(int _number = -1);
    void setCity(const QString _city = QString("New York"));
    void setCountry(const QString _country = QString("us"));
    void setTs(const int _ts = 0);

public slots:
    void readConfiguration();
    QVariantMap run();
    int showConfiguration();
    bool tryDelete();
    void writeConfiguration();

private slots:
    void weatherReplyReceived(QNetworkReply *reply);

private:
    QString m_fileName;
    QStringList m_dirs;
    bool debug;
    QNetworkAccessManager *manager;
    bool isRunning = false;
    Ui::ExtWeather *ui;
    QVariantMap parseSingleJson(const QVariantMap json);
    QString url(const bool isForecast = false);
    // properties
    int m_apiVersion = 0;
    bool m_active = true;
    QString m_comment = QString("empty");
    int m_interval = 3600;
    QString m_name = QString("none");
    int m_number = -1;
    QString m_city = QString("New York");
    QString m_country = QString("us");
    int m_ts = 0;
    // values
    int times = 0;
    QVariantMap values;
};


#endif /* EXTWEATHER_H */
