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

#include <QSettings>

#include <qreplytimeout/qreplytimeout.h>

#include "awdebug.h"
#include "stooqquotesprovider.h"


ExtQuotes::ExtQuotes(QObject *_parent, const QString &_filePath)
    : AbstractExtItem(_parent, _filePath)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!_filePath.isEmpty())
        ExtQuotes::readConfiguration();

    m_values[tag("price")] = 0.0;
    m_values[tag("pricechg")] = 0.0;
    m_values[tag("percpricechg")] = 0.0;
    m_values[tag("volume")] = 0;
    m_values[tag("volumechg")] = 0;
    m_values[tag("percvolumechg")] = 0.0;

    // HACK declare as child of nullptr to avoid crash with plasmawindowed
    // in the destructor
    m_manager = new QNetworkAccessManager(nullptr);
    connect(m_manager, &QNetworkAccessManager::finished, this, &ExtQuotes::quotesReplyReceived);
    connect(this, &ExtQuotes::requestDataUpdate, this, &ExtQuotes::sendRequest);
}


ExtQuotes::~ExtQuotes()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    m_manager->deleteLater();
}


ExtQuotes *ExtQuotes::copy(const QString &_fileName, const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    auto item = new ExtQuotes(parent(), _fileName);
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
    initProvider();
}


void ExtQuotes::readConfiguration()
{
    AbstractExtItem::readConfiguration();

    QSettings settings(filePath(), QSettings::IniFormat);

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


int ExtQuotes::showConfiguration(QWidget *_parent, const QVariant &_args)
{
    Q_UNUSED(_args)

    auto dialog = new QDialog(_parent);
    auto ui = new Ui::ExtQuotes();
    ui->setupUi(dialog);

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->lineEdit_ticker->setText(ticker());
    ui->checkBox_active->setCheckState(isActive() ? Qt::Checked : Qt::Unchecked);
    ui->lineEdit_schedule->setText(cron());
    ui->lineEdit_socket->setText(socket());
    ui->spinBox_interval->setValue(interval());

    auto ret = dialog->exec();
    if (ret == 1) {
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
    }

    dialog->deleteLater();
    delete ui;

    return ret;
}


void ExtQuotes::writeConfiguration() const
{
    AbstractExtItem::writeConfiguration();

    QSettings settings(writableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup("Desktop Entry");
    settings.setValue("X-AW-Ticker", ticker());
    settings.endGroup();

    settings.sync();
}

void ExtQuotes::quotesReplyReceived(QNetworkReply *_reply)
{
    if (_reply->error() != QNetworkReply::NoError) {
        qCWarning(LOG_AW) << "An error occurs" << _reply->error() << "with message" << _reply->errorString();
        return;
    }

    m_isRunning = false;
    auto text = _reply->readAll();
    _reply->deleteLater();

    auto data = m_providerObject->parse(text);
    for (auto [key, value] : data.asKeyValueRange()) {
        m_values[tag(key)] = value;
    }

    emit(dataReceived(m_values));
}


void ExtQuotes::sendRequest()
{
    m_isRunning = true;
    auto reply = m_manager->get(QNetworkRequest(m_providerObject->url()));
    new QReplyTimeout(reply, REQUEST_TIMEOUT);
}


void ExtQuotes::initProvider()
{
    // in the future release it is possible to change provider here
    m_providerObject = std::make_unique<StooqQuotesProvider>();

    return m_providerObject->initUrl(ticker());
}
