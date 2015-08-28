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

#include <qreplytimeout/qreplytimeout.h>

#include "awdebug.h"
#include "version.h"


ExtQuotes::ExtQuotes(QWidget *parent, const QString quotesName,
                     const QStringList directories, const bool debugCmd)
    : AbstractExtItem(parent, quotesName, directories, debugCmd),
      ui(new Ui::ExtQuotes)
{
    // logging
    const_cast<QLoggingCategory &>(LOG_ESM()).setEnabled(QtMsgType::QtDebugMsg, debugCmd);
    qSetMessagePattern(LOG_FORMAT);

    readConfiguration();
    ui->setupUi(this);
    translate();

    values[tag(QString("ask"))] = 0.0;
    values[tag(QString("askchg"))] = 0.0;
    values[tag(QString("percaskchg"))] = 0.0;
    values[tag(QString("bid"))] = 0.0;
    values[tag(QString("bidchg"))] = 0.0;
    values[tag(QString("percbidchg"))] = 0.0;
    values[tag(QString("price"))] = 0.0;
    values[tag(QString("pricechg"))] = 0.0;
    values[tag(QString("percpricechg"))] = 0.0;

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(quotesReplyReceived(QNetworkReply *)));
}


ExtQuotes::~ExtQuotes()
{
    qCDebug(LOG_ESM);

    disconnect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(quotesReplyReceived(QNetworkReply *)));

    delete manager;
    delete ui;
}


ExtQuotes *ExtQuotes::copy(const QString fileName, const int number)
{
    qCDebug(LOG_ESM);

    ExtQuotes *item = new ExtQuotes(static_cast<QWidget *>(parent()), fileName,
                                    directories(), LOG_ESM().isDebugEnabled());
    item->setActive(isActive());
    item->setApiVersion(apiVersion());
    item->setComment(comment());
    item->setInterval(interval());
    item->setName(name());
    item->setNumber(number);
    item->setTicker(ticker());

    return item;
}


QString ExtQuotes::ticker() const
{
    qCDebug(LOG_ESM);

    return m_ticker;
}


QString ExtQuotes::uniq() const
{
    qCDebug(LOG_ESM);

    return m_ticker;
}


void ExtQuotes::setTicker(const QString _ticker)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Ticker" << _ticker;

    m_ticker = _ticker;
}


void ExtQuotes::readConfiguration()
{
    qCDebug(LOG_ESM);
    AbstractExtItem::readConfiguration();

    for (int i=directories().count()-1; i>=0; i--) {
        if (!QDir(directories().at(i)).entryList(QDir::Files).contains(fileName())) continue;
        QSettings settings(QString("%1/%2").arg(directories().at(i)).arg(fileName()), QSettings::IniFormat);

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


QVariantHash ExtQuotes::run()
{
    qCDebug(LOG_ESM);
    if ((!isActive()) || (isRunning)) return values;

    if (times == 1) {
        qCDebug(LOG_ESM) << "Send request";
        isRunning = true;
        QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url())));
        new QReplyTimeout(reply, 1000);
    }

    // update value
    if (times >= interval()) times = 0;
    times++;

    return values;
}


int ExtQuotes::showConfiguration(const QVariant args)
{
    Q_UNUSED(args)
    qCDebug(LOG_ESM);

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


void ExtQuotes::writeConfiguration() const
{
    qCDebug(LOG_ESM);
    AbstractExtItem::writeConfiguration();

    QSettings settings(QString("%1/%2").arg(directories().first()).arg(fileName()), QSettings::IniFormat);
    qCDebug(LOG_ESM) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-Ticker"), m_ticker);
    settings.endGroup();

    settings.sync();
}

void ExtQuotes::quotesReplyReceived(QNetworkReply *reply)
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
    QVariantMap jsonQuotes = jsonDoc.toVariant().toMap()[QString("query")].toMap();
    jsonQuotes = jsonQuotes[QString("results")].toMap()[QString("quote")].toMap();
    float value;

    // ask
    value = jsonQuotes[QString("Ask")].toString().toFloat();
    values[tag(QString("askchg"))] = values[QString("ask")].toFloat() == 0.0 ? 0.0 :
                                value - values[QString("ask")].toFloat();
    values[tag(QString("percaskchg"))] = 100.0 * values[QString("askchg")].toFloat() / values[QString("ask")].toFloat();
    values[tag(QString("ask"))] = value;

    // bid
    value = jsonQuotes[QString("Bid")].toString().toFloat();
    values[tag(QString("bidchg"))] = values[QString("bid")].toFloat() == 0.0 ? 0.0 :
                                value - values[QString("bid")].toFloat();
    values[tag(QString("percbidchg"))] = 100.0 * values[QString("bidchg")].toFloat() / values[QString("bid")].toFloat();
    values[tag(QString("bid"))] = value;

    // last trade
    value = jsonQuotes[QString("LastTradePriceOnly")].toString().toFloat();
    values[tag(QString("pricechg"))] = values[QString("price")].toFloat() == 0.0 ? 0.0 :
                                value - values[QString("price")].toFloat();
    values[tag(QString("percpricechg"))] = 100.0 * values[QString("pricechg")].toFloat() / values[QString("price")].toFloat();
    values[tag(QString("price"))] = value;
}


void ExtQuotes::translate()
{
    qCDebug(LOG_ESM);

    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_number->setText(i18n("Tag"));
    ui->label->setText(i18n("<html><head/><body><p>Use YAHOO! finance ticker to get quotes for the instrument. Refer to <a href=\"http://finance.yahoo.com/\"><span style=\" text-decoration: underline; color:#0057ae;\">http://finance.yahoo.com/</span></a></p></body></html>"));
    ui->label_ticker->setText(i18n("Ticker"));
    ui->checkBox_active->setText(i18n("Active"));
    ui->label_interval->setText(i18n("Interval"));
}


QString ExtQuotes::url() const
{
    qCDebug(LOG_ESM);

    QString apiUrl = QString(YAHOO_URL);
    apiUrl.replace(QString("$TICKER"), m_ticker);
    qCDebug(LOG_ESM) << "API url" << apiUrl;

    return apiUrl;
}
