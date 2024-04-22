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

#pragma once

#include <QNetworkReply>

#include "abstractextitem.h"
#include "awtranslation.h"


class AbstractWeatherProvider;

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

    explicit ExtWeather(QObject *_parent = nullptr, const QString &_filePath = "");
    ~ExtWeather() override;
    ExtWeather *copy(const QString &_fileName, int _number) override;
    static QString jsonMapFile();
    [[nodiscard]] QString weatherFromInt(int _id) const;
    // get methods
    [[nodiscard]] QString city() const;
    [[nodiscard]] QString country() const;
    [[nodiscard]] bool image() const;
    [[nodiscard]] Provider provider() const;
    [[nodiscard]] QString strProvider() const;
    [[nodiscard]] int ts() const;
    [[nodiscard]] QString uniq() const override;
    // set methods
    void setCity(const QString &_city);
    void setCountry(const QString &_country);
    void setImage(bool _image);
    void setProvider(Provider _provider);
    void setStrProvider(const QString &_provider);
    void setTs(int _ts);

public slots:
    void readConfiguration() override;
    void readJsonMap();
    QVariantHash run() override;
    int showConfiguration(QWidget *_parent, const QVariant &_args) override;
    void writeConfiguration() const override;

private slots:
    void sendRequest();
    void weatherReplyReceived(QNetworkReply *_reply);

private:
    std::unique_ptr<AbstractWeatherProvider> m_providerObject;
    QNetworkAccessManager *m_manager = nullptr;
    bool m_isRunning = false;
    void initProvider();
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
