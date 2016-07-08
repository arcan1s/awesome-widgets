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
#include "owmweatherprovider.h"
#include "yahooweatherprovider.h"


ExtWeather::ExtWeather(QWidget *parent, const QString filePath)
    : AbstractExtItem(parent, filePath)
    , ui(new Ui::ExtWeather)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!filePath.isEmpty())
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
    m_manager = new QNetworkAccessManager(nullptr);
    connect(m_manager, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(weatherReplyReceived(QNetworkReply *)));
}


ExtWeather::~ExtWeather()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    disconnect(m_manager, SIGNAL(finished(QNetworkReply *)), this,
               SLOT(weatherReplyReceived(QNetworkReply *)));

    m_manager->deleteLater();
    delete m_providerObject;
    delete ui;
}


ExtWeather *ExtWeather::copy(const QString _fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "number" << _number;

    ExtWeather *item
        = new ExtWeather(static_cast<QWidget *>(parent()), _fileName);
    copyDefaults(item);
    item->setCity(city());
    item->setCountry(country());
    item->setImage(image());
    item->setNumber(_number);
    item->setProvider(provider());
    item->setTs(ts());

    return item;
}


QString ExtWeather::weatherFromInt(const int _id) const
{
    qCDebug(LOG_LIB) << "Weather ID" << _id;

    QVariantMap map
        = m_jsonMap[m_image ? QString("image") : QString("text")].toMap();
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


ExtWeather::Provider ExtWeather::provider() const
{
    return m_provider;
}


QString ExtWeather::strProvider() const
{
    QString value;
    switch (m_provider) {
    case Provider::OWM:
        value = QString("OWM");
        break;
    case Provider::Yahoo:
        value = QString("Yahoo");
        break;
    }

    return value;
}


int ExtWeather::ts() const
{
    return m_ts;
}


QString ExtWeather::uniq() const
{
    return QString("%1 (%2) at %3").arg(city()).arg(country()).arg(ts());
}


void ExtWeather::setCity(const QString _city)
{
    qCDebug(LOG_LIB) << "City" << _city;

    m_city = _city;
    initProvider();
}


void ExtWeather::setCountry(const QString _country)
{
    qCDebug(LOG_LIB) << "Country" << _country;

    m_country = _country;
    initProvider();
}


void ExtWeather::setImage(const bool _image)
{
    qCDebug(LOG_LIB) << "Use image" << _image;

    m_image = _image;
}


void ExtWeather::setProvider(const Provider _provider)
{
    qCDebug(LOG_LIB) << "Provider" << static_cast<int>(_provider);

    m_provider = _provider;
    initProvider();
}


void ExtWeather::setStrProvider(const QString _provider)
{
    qCDebug(LOG_LIB) << "Provider" << _provider;

    if (_provider == QString("Yahoo"))
        setProvider(Provider::Yahoo);
    else
        setProvider(Provider::OWM);
}


void ExtWeather::setTs(const int _ts)
{
    qCDebug(LOG_LIB) << "Timestamp" << _ts;

    m_ts = _ts;
    initProvider();
}


void ExtWeather::readConfiguration()
{
    AbstractExtItem::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup(QString("Desktop Entry"));
    setCity(settings.value(QString("X-AW-City"), city()).toString());
    setCountry(settings.value(QString("X-AW-Country"), country()).toString());
    setTs(settings.value(QString("X-AW-TS"), ts()).toInt());
    // api == 2
    setImage(settings.value(QString("X-AW-Image"), QVariant(image())).toString()
             == QString("true"));
    // api == 3
    setStrProvider(
        settings.value(QString("X-AW-Provider"), strProvider()).toString());
    settings.endGroup();

    bumpApi(AWEWAPI);
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
    m_jsonMap = jsonDoc.toVariant().toMap();

    qCInfo(LOG_LIB) << "Weather map" << m_jsonMap;
}


QVariantHash ExtWeather::run()
{
    if ((!isActive()) || (isRunning))
        return values;

    if (times == 1) {
        qCInfo(LOG_LIB) << "Send request";
        isRunning = true;
        QNetworkReply *reply
            = m_manager->get(QNetworkRequest(m_providerObject->url()));
        new QReplyTimeout(reply, REQUEST_TIMEOUT);
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
    ui->comboBox_provider->setCurrentIndex(static_cast<int>(provider()));
    ui->lineEdit_city->setText(city());
    ui->lineEdit_country->setText(country());
    ui->spinBox_timestamp->setValue(ts());
    ui->checkBox_image->setCheckState(image() ? Qt::Checked : Qt::Unchecked);
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
    setProvider(static_cast<Provider>(ui->comboBox_provider->currentIndex()));
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

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-City"), city());
    settings.setValue(QString("X-AW-Country"), country());
    settings.setValue(QString("X-AW-Image"), image());
    settings.setValue(QString("X-AW-Provider"), strProvider());
    settings.setValue(QString("X-AW-TS"), ts());
    settings.endGroup();

    settings.sync();
}


void ExtWeather::weatherReplyReceived(QNetworkReply *reply)
{
    qCDebug(LOG_LIB) << "Return code" << reply->error() << "with message"
                     << reply->errorString();

    isRunning = false;
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    reply->deleteLater();
    if ((reply->error() != QNetworkReply::NoError)
        || (error.error != QJsonParseError::NoError)) {
        qCWarning(LOG_LIB) << "Parse error" << error.errorString();
        return;
    }

    QVariantHash data = m_providerObject->parse(jsonDoc.toVariant().toMap());
    if (data.isEmpty())
        return;
    values = data;
    values[tag(QString("weather"))]
        = weatherFromInt(values[tag(QString("weatherId"))].toInt());

    emit(dataReceived(values));
}


void ExtWeather::initProvider()
{
    delete m_providerObject;

    switch (m_provider) {
    case Provider::OWM:
        m_providerObject = new OWMWeatherProvider(this, number());
        break;
    case Provider::Yahoo:
        m_providerObject = new YahooWeatherProvider(this, number());
        break;
    }

    return m_providerObject->initUrl(city(), country(), ts());
}


void ExtWeather::translate()
{
    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_number->setText(i18n("Tag"));
    ui->label_provider->setText(i18n("Provider"));
    ui->label_city->setText(i18n("City"));
    ui->label_country->setText(i18n("Country"));
    ui->label_timestamp->setText(i18n("Timestamp"));
    ui->checkBox_image->setText(i18n("Use images"));
    ui->checkBox_active->setText(i18n("Active"));
    ui->label_interval->setText(i18n("Interval"));
}
