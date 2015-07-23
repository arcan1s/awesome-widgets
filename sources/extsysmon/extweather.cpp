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

#include "extweather.h"
#include "ui_extweather.h"

#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QSettings>

#include <pdebug/pdebug.h>
#include <qreplytimeout/qreplytimeout.h>

#include "version.h"


ExtWeather::ExtWeather(QWidget *parent, const QString weatherName,
                       const QStringList directories, const bool debugCmd)
    : AbstractExtItem(parent, weatherName, directories, debugCmd),
      debug(debugCmd),
      ui(new Ui::ExtWeather)
{
    readConfiguration();
    ui->setupUi(this);

    values[QString("weatherId")] = 0;
    values[QString("weather")] = QString("");
    values[QString("humidity")] = 0;
    values[QString("pressure")] = 0.0;
    values[QString("temperature")] = 0.0;

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(weatherReplyReceived(QNetworkReply *)));
}


ExtWeather::~ExtWeather()
{
    if (debug) qDebug() << PDEBUG;

    disconnect(manager, SIGNAL(finished(QNetworkReply *)),
               this, SLOT(weatherReplyReceived(QNetworkReply *)));

    delete manager;
    delete ui;
}


QString ExtWeather::weatherFromInt(const int _id) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "ID" << _id;
    // refer to http://openweathermap.org/weather-conditions

    QString weather;
    switch (_id) {
    case 800:
        // 01d
        weather = QString("\u2600");
        break;
    case 801:
        // 02d
        weather = QString("\u26C5");
        break;
    case 802:
    case 803:
        // 03d
        weather = QString("\u2601");
        break;
    case 804:
        // 04d
        weather = QString("\u2601");
        break;
    case 300:
    case 301:
    case 302:
    case 310:
    case 311:
    case 312:
    case 313:
    case 314:
    case 321:
    case 520:
    case 521:
    case 522:
    case 531:
        // 09d
        weather = QString("\u2602");
        break;
    case 500:
    case 501:
    case 502:
    case 503:
    case 504:
        // 10d
        weather = QString("\u2614");
        break;
    case 200:
    case 201:
    case 202:
    case 210:
    case 211:
    case 212:
    case 221:
    case 230:
    case 231:
    case 232:
        // 11d
        weather = QString("\u2608");
        break;
    case 511:
    case 600:
    case 601:
    case 602:
    case 611:
    case 612:
    case 615:
    case 616:
    case 620:
    case 621:
    case 622:
        // 13d
        weather = QString("\u2603");
//         weather = QString("\u26C4");
        break;
    case 701:
    case 711:
    case 721:
    case 731:
    case 741:
    case 751:
    case 761:
    case 762:
    case 771:
    case 781:
        // 50d
        weather = QString("\u26C5");
        break;
    default:
        // extreme other conditions
        weather = QString("\u2604");
        break;
    }

    return weather;
}


QString ExtWeather::city() const
{
    if (debug) qDebug() << PDEBUG;

    return m_city;
}


QString ExtWeather::country() const
{
    if (debug) qDebug() << PDEBUG;

    return m_country;
}


int ExtWeather::ts() const
{
    if (debug) qDebug() << PDEBUG;

    return m_ts;
}


void ExtWeather::setCity(const QString _city)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "City" << _city;

    m_city = _city;
}


void ExtWeather::setCountry(const QString _country)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Country" << _country;

    m_country = _country;
}


void ExtWeather::setTs(const int _ts)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Timestamp" << _ts;

    m_ts = _ts;
}


void ExtWeather::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;
    AbstractExtItem::readConfiguration();

    for (int i=directories().count()-1; i>=0; i--) {
        if (!QDir(directories()[i]).entryList(QDir::Files).contains(fileName())) continue;
        QSettings settings(QString("%1/%2").arg(directories()[i]).arg(fileName()), QSettings::IniFormat);

        settings.beginGroup(QString("Desktop Entry"));
        setCity(settings.value(QString("X-AW-City"), m_city).toString());
        setCountry(settings.value(QString("X-AW-Country"), m_country).toString());
        setTs(settings.value(QString("X-AW-TS"), m_ts).toInt());
        settings.endGroup();
    }

    // update for current API
    if ((apiVersion() > 0) && (apiVersion() < AWEWAPI)) {
        setApiVersion(AWEWAPI);
        writeConfiguration();
    }
}


QVariantMap ExtWeather::run()
{
    if (debug) qDebug() << PDEBUG;
    if ((!isActive()) || (isRunning)) return values;

    if (times == 1) {
        if (debug) qDebug() << PDEBUG << ":" << "Send request";
        isRunning = true;
        QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url(m_ts != 0))));
        new QReplyTimeout(reply, 1000);
    }

    // update value
    if (times >= interval()) times = 0;
    times++;

    return values;
}


int ExtWeather::showConfiguration(const QVariant args)
{
    Q_UNUSED(args)
    if (debug) qDebug() << PDEBUG;

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->lineEdit_city->setText(m_city);
    ui->lineEdit_country->setText(m_country);
    ui->spinBox_timestamp->setValue(m_ts);
    ui->checkBox_active->setCheckState(isActive() ? Qt::Checked : Qt::Unchecked);
    ui->spinBox_interval->setValue(interval());

    int ret = exec();
    if (ret != 1) return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setNumber(ui->label_numberValue->text().toInt());
    setApiVersion(AWEWAPI);
    setCity(ui->lineEdit_city->text());
    setCountry(ui->lineEdit_country->text());
    setTs(ui->spinBox_timestamp->value());
    setActive(ui->checkBox_active->checkState() == Qt::Checked);
    setInterval(ui->spinBox_interval->value());

    writeConfiguration();
    return ret;
}


void ExtWeather::writeConfiguration() const
{
    if (debug) qDebug() << PDEBUG;
    AbstractExtItem::writeConfiguration();

    QSettings settings(QString("%1/%2").arg(directories()[0]).arg(fileName()), QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-City"), m_city);
    settings.setValue(QString("X-AW-Country"), m_country);
    settings.setValue(QString("X-AW-TS"), m_ts);
    settings.endGroup();

    settings.sync();
}


void ExtWeather::weatherReplyReceived(QNetworkReply *reply)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Return code" << reply->error();
    if (debug) qDebug() << PDEBUG << ":" << "Reply error message" << reply->errorString();

    isRunning = false;
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    reply->deleteLater();
    if (debug) qDebug() << PDEBUG << ":" << "Json parse error" << error.errorString();
    if ((reply->error() != QNetworkReply::NoError) ||
        (error.error != QJsonParseError::NoError)) {
        return;
    }

    // convert to map
    QVariantMap json = jsonDoc.toVariant().toMap();
    if (json[QString("cod")].toInt() != 200) {
        if (debug) qDebug() << PDEBUG << ":" << "Invalid return code";
        return;
    }

    QVariantMap data;
    if (m_ts == 0)
        data = parseSingleJson(json);
    else {
        QVariantList list = json[QString("list")].toList();
        data = parseSingleJson(list.count() <= m_ts ? list[m_ts-1].toMap() : list.last().toMap());
    }
    for (int i=0; i<data.keys().count(); i++)
        values[data.keys()[i]] = data[data.keys()[i]];
}


QVariantMap ExtWeather::parseSingleJson(const QVariantMap json) const
{
    if (debug) qDebug() << PDEBUG;

    QVariantMap output;

    // weather status
    QVariantList weather = json[QString("weather")].toList();
    if (weather.count() > 0) {
        int _id = weather[0].toMap()[QString("id")].toInt();
        output[QString("weatherId")] = _id;
        output[QString("weather")] = weatherFromInt(_id);
    }

    // main data
    QVariantMap mainWeather = json[QString("main")].toMap();
    if (weather.count() > 0) {
        output[QString("humidity")] = mainWeather[QString("humidity")].toFloat();
        output[QString("pressure")] = mainWeather[QString("pressure")].toFloat();
        output[QString("temperature")] = mainWeather[QString("temp")].toFloat();
    }

    return output;
}


QString ExtWeather::url(const bool isForecast) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << "Is forecast" << isForecast;

    QString apiUrl = isForecast ? QString(OWM_FORECAST_URL) : QString(OWM_URL);
    apiUrl.replace(QString("$CITY"), m_city);
    apiUrl.replace(QString("$COUNTRY"), m_country);
    if (debug) qDebug() << PDEBUG << ":" << "API url" << apiUrl;

    return apiUrl;
}
