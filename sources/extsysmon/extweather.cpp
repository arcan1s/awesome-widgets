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
#include <QTime>

#include <pdebug/pdebug.h>
#include <qreplytimeout/qreplytimeout.h>

#include "version.h"


ExtWeather::ExtWeather(QWidget *parent, const QString weatherName,
                       const QStringList directories, const bool debugCmd)
    : QDialog(parent),
      m_fileName(weatherName),
      m_dirs(directories),
      debug(debugCmd),
      ui(new Ui::ExtWeather)
{
    m_name = m_fileName;
    readConfiguration();
    ui->setupUi(this);

    values[QString("weatherId")] = 0;
    values[QString("weather")] = QString("");
    values[QString("humidity")] = 0;
    values[QString("pressure")] = 0.0;
    values[QString("temperature")] = 0.0;

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(weatherReplyReceived(QNetworkReply*)));
}


ExtWeather::~ExtWeather()
{
    if (debug) qDebug() << PDEBUG;

    disconnect(manager, SIGNAL(finished(QNetworkReply *)),
               this, SLOT(weatherReplyReceived(QNetworkReply *)));

    delete manager;
    delete ui;
}


QString ExtWeather::weatherFromInt(const int _id)
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


int ExtWeather::apiVersion()
{
    if (debug) qDebug() << PDEBUG;

    return m_apiVersion;
}


QString ExtWeather::comment()
{
    if (debug) qDebug() << PDEBUG;

    return m_comment;
}


QString ExtWeather::fileName()
{
    if (debug) qDebug() << PDEBUG;

    return m_fileName;
}


int ExtWeather::interval()
{
    if (debug) qDebug() << PDEBUG;

    return m_interval;
}


bool ExtWeather::isActive()
{
    if (debug) qDebug() << PDEBUG;

    return m_active;
}


QString ExtWeather::name()
{
    if (debug) qDebug() << PDEBUG;

    return m_name;
}


int ExtWeather::number()
{
    if (debug) qDebug() << PDEBUG;

    return m_number;
}


QString ExtWeather::tag(const QString _type)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Tag type" << _type;

    return QString("%1%2").arg(_type).arg(m_number);
}


QString ExtWeather::city()
{
    if (debug) qDebug() << PDEBUG;

    return m_city;
}


QString ExtWeather::country()
{
    if (debug) qDebug() << PDEBUG;

    return m_country;
}


int ExtWeather::ts()
{
    if (debug) qDebug() << PDEBUG;

    return m_ts;
}


void ExtWeather::setApiVersion(const int _apiVersion)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Version" << _apiVersion;

    m_apiVersion = _apiVersion;
}


void ExtWeather::setActive(const bool _state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << _state;

    m_active = _state;
}


void ExtWeather::setComment(const QString _comment)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Comment" << _comment;

    m_comment = _comment;
}


void ExtWeather::setInterval(const int _interval)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Interval" << _interval;

    m_interval = _interval;
}


void ExtWeather::setName(const QString _name)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Name" << _name;

    m_name = _name;
}


void ExtWeather::setNumber(int _number)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Number" << _number;
    if (_number == -1) {
        if (debug) qDebug() << PDEBUG << ":" << "Number is empty, generate new one";
        qsrand(QTime::currentTime().msec());
        _number = qrand() % 1000;
        if (debug) qDebug() << PDEBUG << ":" << "Generated number is" << _number;
    }

    m_number = _number;
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

    for (int i=m_dirs.count()-1; i>=0; i--) {
        if (!QDir(m_dirs[i]).entryList(QDir::Files).contains(m_fileName)) continue;
        QSettings settings(QString("%1/%2").arg(m_dirs[i]).arg(m_fileName), QSettings::IniFormat);

        settings.beginGroup(QString("Desktop Entry"));
        setName(settings.value(QString("Name"), m_name).toString());
        setComment(settings.value(QString("Comment"), m_comment).toString());
        setApiVersion(settings.value(QString("X-AW-ApiVersion"), m_apiVersion).toInt());
        setCity(settings.value(QString("X-AW-City"), m_city).toString());
        setCountry(settings.value(QString("X-AW-Country"), m_country).toString());
        setTs(settings.value(QString("X-AW-TS"), m_ts).toInt());
        setActive(settings.value(QString("X-AW-Active"), QVariant(m_active)).toString() == QString("true"));
        setInterval(settings.value(QString("X-AW-Interval"), m_interval).toInt());
        setNumber(settings.value(QString("X-AW-Number"), m_number).toInt());
        settings.endGroup();
    }

    // update for current API
    if ((m_apiVersion > 0) && (m_apiVersion < AWEWAPI)) {
        setApiVersion(AWEWAPI);
        writeConfiguration();
    }
}


QVariantMap ExtWeather::run()
{
    if (debug) qDebug() << PDEBUG;
    if ((!m_active) || (isRunning)) return values;

    if (times == 1) {
        if (debug) qDebug() << PDEBUG << ":" << "Send request";
        isRunning = true;
        QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url(m_ts != 0))));
        new QReplyTimeout(reply, 1000);
    }

    // update value
    if (times >= m_interval) times = 0;
    times++;

    return values;
}


int ExtWeather::showConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    ui->lineEdit_name->setText(m_name);
    ui->lineEdit_comment->setText(m_comment);
    ui->label_numberValue->setText(QString("%1").arg(m_number));
    ui->lineEdit_city->setText(m_city);
    ui->lineEdit_country->setText(m_country);
    ui->spinBox_timestamp->setValue(m_ts);
    ui->checkBox_active->setCheckState(m_active ? Qt::Checked : Qt::Unchecked);
    ui->spinBox_interval->setValue(m_interval);

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


bool ExtWeather::tryDelete()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=0; i<m_dirs.count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << "Remove file" << QString("%1/%2").arg(m_dirs[i]).arg(m_fileName) <<
                               QFile::remove(QString("%1/%2").arg(m_dirs[i]).arg(m_fileName));

    // check if exists
    for (int i=0; i<m_dirs.count(); i++)
        if (QFile::exists(QString("%1/%2").arg(m_dirs[i]).arg(m_fileName))) return false;
    return true;
}


void ExtWeather::writeConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QSettings settings(QString("%1/%2").arg(m_dirs[0]).arg(m_fileName), QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("Encoding"), QString("UTF-8"));
    settings.setValue(QString("Name"), m_name);
    settings.setValue(QString("Comment"), m_comment);
    settings.setValue(QString("X-AW-City"), m_city);
    settings.setValue(QString("X-AW-Country"), m_country);
    settings.setValue(QString("X-AW-TS"), m_ts);
    settings.setValue(QString("X-AW-ApiVersion"), m_apiVersion);
    settings.setValue(QString("X-AW-Active"), QVariant(m_active).toString());
    settings.setValue(QString("X-AW-Interval"), m_interval);
    settings.setValue(QString("X-AW-Number"), m_number);
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


QVariantMap ExtWeather::parseSingleJson(const QVariantMap json)
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


QString ExtWeather::url(const bool isForecast)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << "Is forecast" << isForecast;

    QString apiUrl = isForecast ? QString(OWM_FORECAST_URL) : QString(OWM_URL);
    apiUrl.replace(QString("$CITY"), m_city);
    apiUrl.replace(QString("$COUNTRY"), m_country);
    if (debug) qDebug() << PDEBUG << ":" << "API url" << apiUrl;

    return apiUrl;
}
