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

#include "extquotes.h"
#include "ui_extquotes.h"

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QSettings>
#include <QUrlQuery>

#include <qreplytimeout/qreplytimeout.h>

#include "awdebug.h"


ExtQuotes::ExtQuotes(QWidget *_parent, const QString &_filePath)
    : AbstractExtItem(_parent, _filePath)
    , ui(new Ui::ExtQuotes)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        readConfiguration();
    ui->setupUi(this);
    translate();

    m_values[tag("ask")] = 0.0;
    m_values[tag("askchg")] = 0.0;
    m_values[tag("percaskchg")] = 0.0;
    m_values[tag("bid")] = 0.0;
    m_values[tag("bidchg")] = 0.0;
    m_values[tag("percbidchg")] = 0.0;
    m_values[tag("price")] = 0.0;
    m_values[tag("pricechg")] = 0.0;
    m_values[tag("percpricechg")] = 0.0;

    // HACK declare as child of nullptr to avoid crash with plasmawindowed
    // in the destructor
    m_manager = new QNetworkAccessManager(nullptr);
    connect(m_manager, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(quotesReplyReceived(QNetworkReply *)));

    connect(this, SIGNAL(requestDataUpdate()), this, SLOT(sendRequest()));
}


ExtQuotes::~ExtQuotes()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    disconnect(m_manager, SIGNAL(finished(QNetworkReply *)), this,
               SLOT(quotesReplyReceived(QNetworkReply *)));
    disconnect(this, SIGNAL(requestDataUpdate()), this, SLOT(sendRequest()));

    m_manager->deleteLater();
    delete ui;
}


ExtQuotes *ExtQuotes::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    ExtQuotes *item
        = new ExtQuotes(static_cast<QWidget *>(parent()), _fileName);
    copyDefaults(item);
    item->setNumber(_number);
    item->setTicker(ticker());

    return item;
}


QString ExtQuotes::ticker() const
{
    return m_ticker;
}


QString ExtQuotes::uniq() const
{
    return ticker();
}


void ExtQuotes::setTicker(const QString &_ticker)
{
    qCDebug(LOG_LIB) << "Ticker" << _ticker;

    m_ticker = _ticker;
    initUrl();
}


void ExtQuotes::readConfiguration()
{
    AbstractExtItem::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup("Desktop Entry");
    setTicker(settings.value("X-AW-Ticker", ticker()).toString());
    settings.endGroup();

    bumpApi(AW_EXTQUOTES_API);
}


QVariantHash ExtQuotes::run()
{
    if (m_isRunning)
        return m_values;
    startTimer();

    return m_values;
}


int ExtQuotes::showConfiguration(const QVariant &_args)
{
    Q_UNUSED(_args)

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->lineEdit_ticker->setText(ticker());
    ui->checkBox_active->setCheckState(isActive() ? Qt::Checked
                                                  : Qt::Unchecked);
    ui->lineEdit_schedule->setText(cron());
    ui->lineEdit_socket->setText(socket());
    ui->spinBox_interval->setValue(interval());

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setNumber(ui->label_numberValue->text().toInt());
    setApiVersion(AW_EXTQUOTES_API);
    setTicker(ui->lineEdit_ticker->text());
    setActive(ui->checkBox_active->checkState() == Qt::Checked);
    setCron(ui->lineEdit_schedule->text());
    setSocket(ui->lineEdit_socket->text());
    setInterval(ui->spinBox_interval->value());

    writeConfiguration();
    return ret;
}


void ExtQuotes::writeConfiguration() const
{
    AbstractExtItem::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("X-AW-Ticker", ticker());
    settings.endGroup();

    settings.sync();
}

void ExtQuotes::quotesReplyReceived(QNetworkReply *_reply)
{
    if (_reply->error() != QNetworkReply::NoError) {
        qCWarning(LOG_AW) << "An error occurs" << _reply->error()
                          << "with message" << _reply->errorString();
        return;
    }

    m_isRunning = false;
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(_reply->readAll(), &error);
    _reply->deleteLater();
    if (error.error != QJsonParseError::NoError) {
        qCWarning(LOG_LIB) << "Parse error" << error.errorString();
        return;
    }
    QVariantMap jsonQuotes = jsonDoc.toVariant().toMap()["query"].toMap();
    jsonQuotes = jsonQuotes["results"].toMap()["quote"].toMap();
    double value;

    // ask
    value = jsonQuotes["Ask"].toString().toDouble();
    m_values[tag("askchg")] = m_values[tag("ask")].toDouble() == 0.0
                                  ? 0.0
                                  : value - m_values[tag("ask")].toDouble();
    m_values[tag("percaskchg")] = 100.0 * m_values[tag("askchg")].toDouble()
                                  / m_values[tag("ask")].toDouble();
    m_values[tag("ask")] = value;

    // bid
    value = jsonQuotes["Bid"].toString().toDouble();
    m_values[tag("bidchg")] = m_values[tag("bid")].toDouble() == 0.0
                                  ? 0.0
                                  : value - m_values[tag("bid")].toDouble();
    m_values[tag("percbidchg")] = 100.0 * m_values[tag("bidchg")].toDouble()
                                  / m_values[tag("bid")].toDouble();
    m_values[tag("bid")] = value;

    // last trade
    value = jsonQuotes["LastTradePriceOnly"].toString().toDouble();
    m_values[tag("pricechg")] = m_values[tag("price")].toDouble() == 0.0
                                    ? 0.0
                                    : value - m_values[tag("price")].toDouble();
    m_values[tag("percpricechg")] = 100.0 * m_values[tag("pricechg")].toDouble()
                                    / m_values[tag("price")].toDouble();
    m_values[tag("price")] = value;

    emit(dataReceived(m_values));
}


void ExtQuotes::sendRequest()
{
    m_isRunning = true;
    QNetworkReply *reply = m_manager->get(QNetworkRequest(m_url));
    new QReplyTimeout(reply, REQUEST_TIMEOUT);
}


void ExtQuotes::initUrl()
{
    // init query
    m_url = QUrl(YAHOO_QUOTES_URL);
    QUrlQuery params;
    params.addQueryItem("format", "json");
    params.addQueryItem("env", "store://datatables.org/alltableswithkeys");
    params.addQueryItem("q", QString(YAHOO_QUOTES_QUERY).arg(ticker()));
    m_url.setQuery(params);
}


void ExtQuotes::translate()
{
    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_number->setText(i18n("Tag"));
    ui->label->setText(
        i18n("<html><head/><body><p>Use YAHOO! finance ticker to \
get quotes for the instrument. Refer to <a href=\"http://finance.yahoo.com/\">\
<span style=\" text-decoration: underline; color:#0057ae;\">http://finance.yahoo.com/\
</span></a></p></body></html>"));
    ui->label_ticker->setText(i18n("Ticker"));
    ui->checkBox_active->setText(i18n("Active"));
    ui->label_schedule->setText(i18n("Schedule"));
    ui->label_socket->setText(i18n("Socket"));
    ui->label_interval->setText(i18n("Interval"));
}
