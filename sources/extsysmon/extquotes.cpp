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


ExtQuotes::ExtQuotes(QWidget *parent, const QString quotesName,
                     const QStringList directories, const bool debugCmd)
    : AbstractExtItem(parent, quotesName, directories, debugCmd),
      debug(debugCmd),
      ui(new Ui::ExtQuotes)
{
    readConfiguration();
    ui->setupUi(this);

    values[QString("ask")] = 0.0;
    values[QString("askchg")] = 0.0;
    values[QString("percaskchg")] = 0.0;
    values[QString("bid")] = 0.0;
    values[QString("bidchg")] = 0.0;
    values[QString("percbidchg")] = 0.0;
    values[QString("price")] = 0.0;
    values[QString("pricechg")] = 0.0;
    values[QString("percpricechg")] = 0.0;

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(quotesReplyReceived(QNetworkReply *)));
}


ExtQuotes::~ExtQuotes()
{
    if (debug) qDebug() << PDEBUG;

    disconnect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(quotesReplyReceived(QNetworkReply *)));

    delete manager;
    delete ui;
}


QString ExtQuotes::ticker()
{
    if (debug) qDebug() << PDEBUG;

    return m_ticker;
}


void ExtQuotes::setTicker(const QString _ticker)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Ticker" << _ticker;

    m_ticker = _ticker;
}


void ExtQuotes::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=directories().count()-1; i>=0; i--) {
        if (!QDir(directories()[i]).entryList(QDir::Files).contains(fileName())) continue;
        QSettings settings(QString("%1/%2").arg(directories()[i]).arg(fileName()), QSettings::IniFormat);

        settings.beginGroup(QString("Desktop Entry"));
        setTicker(settings.value(QString("X-AW-Ticker"), m_ticker).toString());
        settings.endGroup();
    }

    // update for current API
    if ((apiVersion() > 0) && (apiVersion() < AWEQAPI)) {
        setApiVersion(AWEQAPI);
        writeConfiguration();
    }
}


QVariantMap ExtQuotes::run()
{
    if (debug) qDebug() << PDEBUG;
    if ((!isActive()) || (isRunning)) return values;

    if (times == 1) {
        if (debug) qDebug() << PDEBUG << ":" << "Send request";
        isRunning = true;
        QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url())));
        new QReplyTimeout(reply, 1000);
    }

    // update value
    if (times >= interval()) times = 0;
    times++;

    return values;
}


int ExtQuotes::showConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->lineEdit_ticker->setText(m_ticker);
    ui->checkBox_active->setCheckState(isActive() ? Qt::Checked : Qt::Unchecked);
    ui->spinBox_interval->setValue(interval());

    int ret = exec();
    if (ret != 1) return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setNumber(ui->label_numberValue->text().toInt());
    setApiVersion(AWEQAPI);
    setTicker(ui->lineEdit_ticker->text());
    setActive(ui->checkBox_active->checkState() == Qt::Checked);
    setInterval(ui->spinBox_interval->value());

    writeConfiguration();
    return ret;
}


void ExtQuotes::writeConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QSettings settings(QString("%1/%2").arg(directories()[0]).arg(fileName()), QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-Ticker"), m_ticker);
    settings.endGroup();

    settings.sync();
}

void ExtQuotes::quotesReplyReceived(QNetworkReply *reply)
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
    QVariantMap jsonQuotes = jsonDoc.toVariant().toMap()[QString("query")].toMap();
    jsonQuotes = jsonQuotes[QString("results")].toMap()[QString("quote")].toMap();
    float value;

    // ask
    value = jsonQuotes[QString("Ask")].toString().toFloat();
    values[QString("askchg")] = values[QString("ask")].toFloat() == 0.0 ? 0.0 :
                                value - values[QString("ask")].toFloat();
    values[QString("percaskchg")] = 100.0 * values[QString("askchg")].toFloat() / values[QString("ask")].toFloat();
    values[QString("ask")] = value;

    // bid
    value = jsonQuotes[QString("Bid")].toString().toFloat();
    values[QString("bidchg")] = values[QString("bid")].toFloat() == 0.0 ? 0.0 :
                                value - values[QString("bid")].toFloat();
    values[QString("percbidchg")] = 100.0 * values[QString("bidchg")].toFloat() / values[QString("bid")].toFloat();
    values[QString("bid")] = value;

    // last trade
    value = jsonQuotes[QString("LastTradePriceOnly")].toString().toFloat();
    values[QString("pricechg")] = values[QString("price")].toFloat() == 0.0 ? 0.0 :
                                  value - values[QString("price")].toFloat();
    values[QString("percpricechg")] = 100.0 * values[QString("pricechg")].toFloat() / values[QString("price")].toFloat();
    values[QString("price")] = value;
}


QString ExtQuotes::url()
{
    if (debug) qDebug() << PDEBUG;

    QString apiUrl = QString(YAHOO_URL);
    apiUrl.replace(QString("$TICKER"), m_ticker);
    if (debug) qDebug() << PDEBUG << ":" << "API url" << apiUrl;

    return apiUrl;
}
