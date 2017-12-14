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


class AbstractWeatherProvider;
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
    Q_PROPERTY(Provider povider READ provider WRITE setProvider)
    Q_PROPERTY(QString strPovider READ strProvider WRITE setStrProvider)
    Q_PROPERTY(int ts READ ts WRITE setTs)

public:
    enum class Provider { OWM = 0, Yahoo = 1 };

    explicit ExtWeather(QWidget *_parent = nullptr, const QString &_filePath = "");
    virtual ~ExtWeather();
    ExtWeather *copy(const QString &_fileName, const int _number);
    QString jsonMapFile() const;
    QString weatherFromInt(const int _id) const;
    // get methods
    QString city() const;
    QString country() const;
    bool image() const;
    Provider provider() const;
    QString strProvider() const;
    int ts() const;
    QString uniq() const;
    // set methods
    void setCity(const QString &_city);
    void setCountry(const QString &_country);
    void setImage(const bool _image);
    void setProvider(const Provider _provider);
    void setStrProvider(const QString &_provider);
    void setTs(const int _ts);

public slots:
    void readConfiguration();
    void readJsonMap();
    QVariantHash run();
    int showConfiguration(const QVariant &_args);
    void writeConfiguration() const;

private slots:
    void sendRequest();
    void weatherReplyReceived(QNetworkReply *_reply);

private:
    AbstractWeatherProvider *m_providerObject = nullptr;
    QNetworkAccessManager *m_manager = nullptr;
    bool m_isRunning = false;
    Ui::ExtWeather *ui = nullptr;
    void initProvider();
    void translate();
    // properties
    QString m_city = "London";
    QString m_country = "uk";
    bool m_image = false;
    Provider m_provider = Provider::OWM;
    int m_ts = 0;
    QVariantMap m_jsonMap;
    // values
    QVariantHash m_values;
};


#endif /* EXTWEATHER_H */
