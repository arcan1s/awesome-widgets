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

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QSettings>

#include <qreplytimeout/qreplytimeout.h>

#include "awdebug.h"
#include "version.h"


ExtWeather::ExtWeather(QWidget *parent, const QString weatherName,
                       const QStringList directories, const bool debugCmd)
    : AbstractExtItem(parent, weatherName, directories, debugCmd),
      ui(new Ui::ExtWeather)
{
    // logging
    const_cast<QLoggingCategory &>(LOG_ESM()).setEnabled(QtMsgType::QtDebugMsg, debugCmd);
    qSetMessagePattern(LOG_FORMAT);

    readConfiguration();
    ui->setupUi(this);
    translate();

    values[tag(QString("weatherId"))] = 0;
    values[tag(QString("weather"))] = QString("");
    values[tag(QString("humidity"))] = 0;
    values[tag(QString("pressure"))] = 0.0;
    values[tag(QString("temperature"))] = 0.0;

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(weatherReplyReceived(QNetworkReply *)));
}


ExtWeather::~ExtWeather()
{
    qCDebug(LOG_ESM);

    disconnect(manager, SIGNAL(finished(QNetworkReply *)),
               this, SLOT(weatherReplyReceived(QNetworkReply *)));

    delete manager;
    delete ui;
}


ExtWeather *ExtWeather::copy(const QString fileName, const int number)
{
    qCDebug(LOG_ESM);

    ExtWeather *item = new ExtWeather(static_cast<QWidget *>(parent()), fileName,
                                      directories(), LOG_ESM().isDebugEnabled());
    item->setActive(isActive());
    item->setApiVersion(apiVersion());
    item->setCity(city());
    item->setComment(comment());
    item->setCountry(country());
    item->setInterval(interval());
    item->setName(name());
    item->setNumber(number);
    item->setTs(ts());

    return item;
}


QString ExtWeather::weatherFromInt(const int _id) const
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "ID" << _id;
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
    qCDebug(LOG_ESM);

    return m_city;
}


QString ExtWeather::country() const
{
    qCDebug(LOG_ESM);

    return m_country;
}


int ExtWeather::ts() const
{
    qCDebug(LOG_ESM);

    return m_ts;
}


QString ExtWeather::uniq() const
{
    qCDebug(LOG_ESM);

    return QString("%1 (%2) at %3").arg(m_city).arg(m_country).arg(m_ts);
}


void ExtWeather::setCity(const QString _city)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "City" << _city;

    m_city = _city;
}


void ExtWeather::setCountry(const QString _country)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Country" << _country;

    m_country = _country;
}


void ExtWeather::setTs(const int _ts)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Timestamp" << _ts;

    m_ts = _ts;
}


void ExtWeather::readConfiguration()
{
    qCDebug(LOG_ESM);
    AbstractExtItem::readConfiguration();

    for (int i=directories().count()-1; i>=0; i--) {
        if (!QDir(directories().at(i)).entryList(QDir::Files).contains(fileName())) continue;
        QSettings settings(QString("%1/%2").arg(directories().at(i)).arg(fileName()), QSettings::IniFormat);

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


QVariantHash ExtWeather::run()
{
    qCDebug(LOG_ESM);
    if ((!isActive()) || (isRunning)) return values;

    if (times == 1) {
        qCDebug(LOG_ESM) << "Send request";
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
    qCDebug(LOG_ESM);

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
    qCDebug(LOG_ESM);
    AbstractExtItem::writeConfiguration();

    QSettings settings(QString("%1/%2").arg(directories().first()).arg(fileName()), QSettings::IniFormat);
    qCDebug(LOG_ESM) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-City"), m_city);
    settings.setValue(QString("X-AW-Country"), m_country);
    settings.setValue(QString("X-AW-TS"), m_ts);
    settings.endGroup();

    settings.sync();
}


void ExtWeather::weatherReplyReceived(QNetworkReply *reply)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Return code" << reply->error();
    qCDebug(LOG_ESM) << "Reply error message" << reply->errorString();

    isRunning = false;
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    reply->deleteLater();
    if ((reply->error() != QNetworkReply::NoError) ||
        (error.error != QJsonParseError::NoError)) {
        qCWarning(LOG_ESM) << "Parse error" << error.errorString();
        return;
    }

    // convert to map
    QVariantMap json = jsonDoc.toVariant().toMap();
    if (json[QString("cod")].toInt() != 200) {
        qCWarning(LOG_ESM) << "Invalid return code";
        return;
    }

    QVariantHash data;
    if (m_ts == 0)
        data = parseSingleJson(json);
    else {
        QVariantList list = json[QString("list")].toList();
        data = parseSingleJson(list.count() <= m_ts ? list.at(m_ts-1).toMap() : list.last().toMap());
    }
    foreach(QString key, data.keys()) values[tag(key)] = data[key];
}


QVariantHash ExtWeather::parseSingleJson(const QVariantMap json) const
{
    qCDebug(LOG_ESM);

    QVariantHash output;

    // weather status
    QVariantList weather = json[QString("weather")].toList();
    if (!weather.isEmpty()) {
        int _id = weather.first().toMap()[QString("id")].toInt();
        output[QString("weatherId")] = _id;
        output[QString("weather")] = weatherFromInt(_id);
    }

    // main data
    QVariantMap mainWeather = json[QString("main")].toMap();
    if (!weather.isEmpty()) {
        output[QString("humidity")] = mainWeather[QString("humidity")].toFloat();
        output[QString("pressure")] = mainWeather[QString("pressure")].toFloat();
        output[QString("temperature")] = mainWeather[QString("temp")].toFloat();
    }

    return output;
}


void ExtWeather::translate()
{
    qCDebug(LOG_ESM);

    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_number->setText(i18n("Tag"));
    ui->label_city->setText(i18n("City"));
    ui->label_country->setText(i18n("Country"));
    ui->label_timestamp->setText(i18n("Timestamp"));
    ui->checkBox_active->setText(i18n("Active"));
    ui->label_interval->setText(i18n("Interval"));
}


QString ExtWeather::url(const bool isForecast) const
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Is forecast" << isForecast;

    QString apiUrl = isForecast ? QString(OWM_FORECAST_URL) : QString(OWM_URL);
    apiUrl.replace(QString("$CITY"), m_city);
    apiUrl.replace(QString("$COUNTRY"), m_country);
    qCDebug(LOG_ESM) << "API url" << apiUrl;

    return apiUrl;
}
