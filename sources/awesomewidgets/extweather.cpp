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
#include <QStandardPaths>

#include <qreplytimeout/qreplytimeout.h>

#include "awdebug.h"
#include "version.h"


ExtWeather::ExtWeather(QWidget *parent, const QString weatherName,
                       const QStringList directories)
    : AbstractExtItem(parent, weatherName, directories)
    , ui(new Ui::ExtWeather)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    readConfiguration();
    readJsonMap();
    ui->setupUi(this);
    translate();

    values[tag(QString("weatherId"))] = 0;
    values[tag(QString("weather"))] = QString("");
    values[tag(QString("humidity"))] = 0;
    values[tag(QString("pressure"))] = 0.0;
    values[tag(QString("temperature"))] = 0.0;

    // HACK declare as child of nullptr to avoid crash with plasmawindowed
    // in the destructor
    manager = new QNetworkAccessManager(nullptr);
    connect(manager, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(weatherReplyReceived(QNetworkReply *)));
}


ExtWeather::~ExtWeather()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    disconnect(manager, SIGNAL(finished(QNetworkReply *)), this,
               SLOT(weatherReplyReceived(QNetworkReply *)));

    manager->deleteLater();
    delete ui;
}


ExtWeather *ExtWeather::copy(const QString _fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName;
    qCDebug(LOG_LIB) << "Number" << _number;

    ExtWeather *item = new ExtWeather(static_cast<QWidget *>(parent()),
                                      _fileName, directories());
    item->setActive(isActive());
    item->setApiVersion(apiVersion());
    item->setCity(city());
    item->setComment(comment());
    item->setCountry(country());
    item->setInterval(interval());
    item->setImage(image());
    item->setName(name());
    item->setNumber(_number);
    item->setTs(ts());

    return item;
}


QString ExtWeather::weatherFromInt(const int _id) const
{
    qCDebug(LOG_LIB) << "Weather ID" << _id;

    QVariantMap map
        = jsonMap[m_image ? QString("image") : QString("text")].toMap();
    return map.value(QString::number(_id), map[QString("default")]).toString();
}


QString ExtWeather::city() const
{
    return m_city;
}


QString ExtWeather::country() const
{
    return m_country;
}


bool ExtWeather::image() const
{
    return m_image;
}


int ExtWeather::ts() const
{
    return m_ts;
}


QString ExtWeather::uniq() const
{
    return QString("%1 (%2) at %3").arg(m_city).arg(m_country).arg(m_ts);
}


void ExtWeather::setCity(const QString _city)
{
    qCDebug(LOG_LIB) << "City" << _city;

    m_city = _city;
}


void ExtWeather::setCountry(const QString _country)
{
    qCDebug(LOG_LIB) << "Country" << _country;

    m_country = _country;
}


void ExtWeather::setImage(const bool _image)
{
    qCDebug(LOG_LIB) << "Use image" << _image;

    m_image = _image;
}


void ExtWeather::setTs(const int _ts)
{
    qCDebug(LOG_LIB) << "Timestamp" << _ts;

    m_ts = _ts;
}


void ExtWeather::readConfiguration()
{
    AbstractExtItem::readConfiguration();

    for (int i = directories().count() - 1; i >= 0; i--) {
        if (!QDir(directories().at(i))
                 .entryList(QDir::Files)
                 .contains(fileName()))
            continue;
        QSettings settings(
            QString("%1/%2").arg(directories().at(i)).arg(fileName()),
            QSettings::IniFormat);

        settings.beginGroup(QString("Desktop Entry"));
        setCity(settings.value(QString("X-AW-City"), m_city).toString());
        setCountry(
            settings.value(QString("X-AW-Country"), m_country).toString());
        setTs(settings.value(QString("X-AW-TS"), m_ts).toInt());
        // api == 2
        setImage(
            settings.value(QString("X-AW-Image"), QVariant(m_image)).toString()
            == QString("true"));
        settings.endGroup();
    }

    // update for current API
    if ((apiVersion() > 0) && (apiVersion() < AWEWAPI)) {
        qCWarning(LOG_LIB) << "Bump API version from" << apiVersion() << "to"
                           << AWEWAPI;
        setApiVersion(AWEWAPI);
        writeConfiguration();
    }
}


void ExtWeather::readJsonMap()
{
    QString fileName = QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QString("awesomewidgets/weather/awesomewidgets-extweather-ids.json"));
    qCInfo(LOG_LIB) << "Map file" << fileName;
    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(LOG_LIB) << "Could not open" << fileName;
        return;
    }
    QString jsonText = jsonFile.readAll();
    jsonFile.close();

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonText.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_LIB) << "Parse error" << error.errorString();
        return;
    }
    jsonMap = jsonDoc.toVariant().toMap();

    qCInfo(LOG_LIB) << "Weather map" << jsonMap;
}


QVariantHash ExtWeather::run()
{
    if ((!isActive()) || (isRunning))
        return values;

    if (times == 1) {
        qCInfo(LOG_LIB) << "Send request";
        isRunning = true;
        QNetworkReply *reply
            = manager->get(QNetworkRequest(QUrl(url(m_ts != 0))));
        new QReplyTimeout(reply, 1000);
    }

    // update value
    if (times >= interval())
        times = 0;
    times++;

    return values;
}


int ExtWeather::showConfiguration(const QVariant args)
{
    Q_UNUSED(args)

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->lineEdit_city->setText(m_city);
    ui->lineEdit_country->setText(m_country);
    ui->spinBox_timestamp->setValue(m_ts);
    ui->checkBox_image->setCheckState(m_image ? Qt::Checked : Qt::Unchecked);
    ui->checkBox_active->setCheckState(isActive() ? Qt::Checked
                                                  : Qt::Unchecked);
    ui->spinBox_interval->setValue(interval());

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setNumber(ui->label_numberValue->text().toInt());
    setApiVersion(AWEWAPI);
    setCity(ui->lineEdit_city->text());
    setCountry(ui->lineEdit_country->text());
    setTs(ui->spinBox_timestamp->value());
    setImage(ui->checkBox_image->checkState() == Qt::Checked);
    setActive(ui->checkBox_active->checkState() == Qt::Checked);
    setInterval(ui->spinBox_interval->value());

    writeConfiguration();
    return ret;
}


void ExtWeather::writeConfiguration() const
{
    AbstractExtItem::writeConfiguration();

    QSettings settings(
        QString("%1/%2").arg(directories().first()).arg(fileName()),
        QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-City"), m_city);
    settings.setValue(QString("X-AW-Country"), m_country);
    settings.setValue(QString("X-AW-Image"), m_image);
    settings.setValue(QString("X-AW-TS"), m_ts);
    settings.endGroup();

    settings.sync();
}


void ExtWeather::weatherReplyReceived(QNetworkReply *reply)
{
    qCDebug(LOG_LIB) << "Return code" << reply->error();
    qCDebug(LOG_LIB) << "Reply error message" << reply->errorString();

    isRunning = false;
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    reply->deleteLater();
    if ((reply->error() != QNetworkReply::NoError)
        || (error.error != QJsonParseError::NoError)) {
        qCWarning(LOG_LIB) << "Parse error" << error.errorString();
        return;
    }

    // convert to map
    QVariantMap json = jsonDoc.toVariant().toMap();
    if (json[QString("cod")].toInt() != 200) {
        qCWarning(LOG_LIB) << "Invalid OpenWeatherMap return code"
                           << json[QString("cod")].toInt();
        return;
    }

    QVariantHash data;
    if (m_ts == 0) {
        data = parseSingleJson(json);
    } else {
        QVariantList list = json[QString("list")].toList();
        data = parseSingleJson(list.count() <= m_ts ? list.at(m_ts - 1).toMap()
                                                    : list.last().toMap());
    }
    foreach (QString key, data.keys())
        values[tag(key)] = data[key];
}


QVariantHash ExtWeather::parseSingleJson(const QVariantMap json) const
{
    qCDebug(LOG_LIB) << "Single json data" << json;

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
        output[QString("humidity")]
            = mainWeather[QString("humidity")].toFloat();
        output[QString("pressure")]
            = mainWeather[QString("pressure")].toFloat();
        output[QString("temperature")] = mainWeather[QString("temp")].toFloat();
    }

    return output;
}


void ExtWeather::translate()
{
    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_number->setText(i18n("Tag"));
    ui->label_city->setText(i18n("City"));
    ui->label_country->setText(i18n("Country"));
    ui->label_timestamp->setText(i18n("Timestamp"));
    ui->checkBox_image->setText(i18n("Use images"));
    ui->checkBox_active->setText(i18n("Active"));
    ui->label_interval->setText(i18n("Interval"));
}


QString ExtWeather::url(const bool isForecast) const
{
    qCDebug(LOG_LIB) << "Is forecast" << isForecast;

    QString apiUrl = isForecast ? QString(OWM_FORECAST_URL) : QString(OWM_URL);
    apiUrl.replace(QString("$CITY"), m_city);
    apiUrl.replace(QString("$COUNTRY"), m_country);
    qCInfo(LOG_LIB) << "API url" << apiUrl;

    return apiUrl;
}
