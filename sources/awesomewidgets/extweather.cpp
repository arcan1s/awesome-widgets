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

#include <QDir>
#include <QJsonDocument>
#include <QSettings>
#include <QStandardPaths>

#include <qreplytimeout/qreplytimeout.h>

#include "awdebug.h"
#include "owmweatherprovider.h"


ExtWeather::ExtWeather(QObject *_parent, const QString &_filePath)
    : AbstractExtItem(_parent, _filePath)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        ExtWeather::readConfiguration();
    readJsonMap();

    m_values[tag("weatherId")] = 0;
    m_values[tag("weather")] = "";
    m_values[tag("humidity")] = 0;
    m_values[tag("pressure")] = 0.0;
    m_values[tag("temperature")] = 0.0;

    // HACK declare as child of nullptr to avoid crash with plasmawindowed
    // in the destructor
    m_manager = new QNetworkAccessManager(nullptr);
    connect(m_manager, &QNetworkAccessManager::finished, this, &ExtWeather::weatherReplyReceived);
    connect(this, &ExtWeather::requestDataUpdate, this, &ExtWeather::sendRequest);
}


ExtWeather::~ExtWeather()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    m_manager->deleteLater();
}


ExtWeather *ExtWeather::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "number" << _number;

    auto item = new ExtWeather(parent(), _fileName);
    copyDefaults(item);

    item->setCity(city());
    item->setCountry(country());
    item->setImage(image());
    item->setNumber(_number);
    item->setProvider(provider());
    item->setTs(ts());

    return item;
}


QString ExtWeather::jsonMapFile()
{
    QString fileName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                              "awesomewidgets/weather/awesomewidgets-extweather-ids.json");
    qCInfo(LOG_LIB) << "Map file" << fileName;

    return fileName;
}


QString ExtWeather::weatherFromInt(const int _id) const
{
    qCDebug(LOG_LIB) << "Weather ID" << _id;

    auto map = m_jsonMap[m_image ? "image" : "text"].toMap();
    return map.value(QString::number(_id), map["default"]).toString();
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
        value = "OWM";
        break;
    case Provider::Yahoo:
        value = "Yahoo";
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
    return QString("%1 (%2) at %3").arg(city(), country()).arg(ts());
}


void ExtWeather::setCity(const QString &_city)
{
    qCDebug(LOG_LIB) << "City" << _city;

    m_city = _city;
    initProvider();
}


void ExtWeather::setCountry(const QString &_country)
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


void ExtWeather::setStrProvider(const QString &_provider)
{
    qCDebug(LOG_LIB) << "Provider" << _provider;

    if (_provider == "Yahoo")
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

    QSettings settings(filePath(), QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setCity(settings.value("X-AW-City", city()).toString());
    setCountry(settings.value("X-AW-Country", country()).toString());
    setTs(settings.value("X-AW-TS", ts()).toInt());
    // api == 2
    setImage(settings.value("X-AW-Image", image()).toBool());
    // api == 3
    setStrProvider(settings.value("X-AW-Provider", strProvider()).toString());
    settings.endGroup();

    bumpApi(AW_EXTWEATHER_API);
}


void ExtWeather::readJsonMap()
{
    auto fileName = jsonMapFile();
    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(LOG_LIB) << "Could not open" << fileName;
        return;
    }
    auto jsonText = jsonFile.readAll();
    jsonFile.close();

    QJsonParseError error{};
    auto jsonDoc = QJsonDocument::fromJson(jsonText, &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_LIB) << "Parse error" << error.errorString();
        return;
    }
    m_jsonMap = jsonDoc.toVariant().toMap();

    qCInfo(LOG_LIB) << "Weather map" << m_jsonMap;
}


QVariantHash ExtWeather::run()
{
    if (m_isRunning)
        return m_values;
    startTimer();

    return m_values;
}


int ExtWeather::showConfiguration(QWidget *_parent, const QVariant &_args)
{
    Q_UNUSED(_args)

    auto dialog = new QDialog(_parent);
    auto ui = new Ui::ExtWeather();
    ui->setupUi(dialog);

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->comboBox_provider->setCurrentIndex(static_cast<int>(provider()));
    ui->lineEdit_city->setText(city());
    ui->lineEdit_country->setText(country());
    ui->spinBox_timestamp->setValue(ts());
    ui->checkBox_image->setCheckState(image() ? Qt::Checked : Qt::Unchecked);
    ui->checkBox_active->setCheckState(isActive() ? Qt::Checked : Qt::Unchecked);
    ui->lineEdit_schedule->setText(cron());
    ui->lineEdit_socket->setText(socket());
    ui->spinBox_interval->setValue(interval());

    int ret = dialog->exec();
    if (ret == 1) {
        setName(ui->lineEdit_name->text());
        setComment(ui->lineEdit_comment->text());
        setNumber(ui->label_numberValue->text().toInt());
        setApiVersion(AW_EXTWEATHER_API);
        setCity(ui->lineEdit_city->text());
        setCountry(ui->lineEdit_country->text());
        setProvider(static_cast<Provider>(ui->comboBox_provider->currentIndex()));
        setTs(ui->spinBox_timestamp->value());
        setImage(ui->checkBox_image->checkState() == Qt::Checked);
        setActive(ui->checkBox_active->checkState() == Qt::Checked);
        setCron(ui->lineEdit_schedule->text());
        setSocket(ui->lineEdit_socket->text());
        setInterval(ui->spinBox_interval->value());

        writeConfiguration();
    }

    dialog->deleteLater();
    delete ui;

    return ret;
}


void ExtWeather::writeConfiguration() const
{
    AbstractExtItem::writeConfiguration();

    QSettings settings(writableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("X-AW-City", city());
    settings.setValue("X-AW-Country", country());
    settings.setValue("X-AW-Image", image());
    settings.setValue("X-AW-Provider", strProvider());
    settings.setValue("X-AW-TS", ts());
    settings.endGroup();

    settings.sync();
}


void ExtWeather::sendRequest()
{
    m_isRunning = true;
    auto reply = m_manager->get(QNetworkRequest(m_providerObject->url()));
    new QReplyTimeout(reply, REQUEST_TIMEOUT);
}


void ExtWeather::weatherReplyReceived(QNetworkReply *_reply)
{
    if (_reply->error() != QNetworkReply::NoError) {
        qCWarning(LOG_AW) << "An error occurs" << _reply->error() << "with message" << _reply->errorString();
        return;
    }

    m_isRunning = false;
    QJsonParseError error{};
    auto jsonDoc = QJsonDocument::fromJson(_reply->readAll(), &error);
    _reply->deleteLater();
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_LIB) << "Parse error" << error.errorString();
        return;
    }

    auto data = m_providerObject->parse(jsonDoc.toVariant().toMap());
    for (auto [key, value] : data.asKeyValueRange())
        m_values[tag(key)] = value;
    m_values[tag("weather")] = weatherFromInt(m_values[tag("weatherId")].toInt());

    emit(dataReceived(m_values));
}


void ExtWeather::initProvider()
{
    // in the future release it is possible to change provider here
    m_providerObject = std::make_unique<OWMWeatherProvider>();

    return m_providerObject->initUrl(city(), country(), ts());
}
