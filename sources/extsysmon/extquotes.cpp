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
#include <QTime>

#include <pdebug/pdebug.h>

#include "version.h"


ExtQuotes::ExtQuotes(QWidget *parent, const QString quotesName, const QStringList directories, const bool debugCmd)
    : QDialog(parent),
      m_fileName(quotesName),
      m_dirs(directories),
      debug(debugCmd),
      ui(new Ui::ExtQuotes)
{
    m_name = m_fileName;
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
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(quotesReplyReceived(QNetworkReply *)));
}


ExtQuotes::~ExtQuotes()
{
    if (debug) qDebug() << PDEBUG;

    disconnect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(quotesReplyReceived(QNetworkReply *)));

    delete manager;
    delete ui;
}


int ExtQuotes::apiVersion()
{
    if (debug) qDebug() << PDEBUG;

    return m_apiVersion;
}


QString ExtQuotes::comment()
{
    if (debug) qDebug() << PDEBUG;

    return m_comment;
}


QString ExtQuotes::fileName()
{
    if (debug) qDebug() << PDEBUG;

    return m_fileName;
}


int ExtQuotes::interval()
{
    if (debug) qDebug() << PDEBUG;

    return m_interval;
}


bool ExtQuotes::isActive()
{
    if (debug) qDebug() << PDEBUG;

    return m_active;
}


QString ExtQuotes::name()
{
    if (debug) qDebug() << PDEBUG;

    return m_name;
}


int ExtQuotes::number()
{
    if (debug) qDebug() << PDEBUG;

    return m_number;
}


QString ExtQuotes::tag(const QString _type)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Tag type" << _type;

    return QString("%1%2").arg(_type).arg(m_number);
}


QString ExtQuotes::ticker()
{
    if (debug) qDebug() << PDEBUG;

    return m_ticker;
}


void ExtQuotes::setApiVersion(const int _apiVersion)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Version" << _apiVersion;

    m_apiVersion = _apiVersion;
}


void ExtQuotes::setActive(const bool _state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << _state;

    m_active = _state;
}


void ExtQuotes::setComment(const QString _comment)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Comment" << _comment;

    m_comment = _comment;
}


void ExtQuotes::setInterval(const int _interval)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Interval" << _interval;

    m_interval = _interval;
}


void ExtQuotes::setName(const QString _name)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Name" << _name;

    m_name = _name;
}


void ExtQuotes::setNumber(int _number)
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


void ExtQuotes::setTicker(const QString _ticker)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Ticker" << _ticker;

    m_ticker = _ticker;
}


void ExtQuotes::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    for (int i=m_dirs.count()-1; i>=0; i--) {
        if (!QDir(m_dirs[i]).entryList(QDir::Files).contains(m_fileName)) continue;
        QSettings settings(QString("%1/%2").arg(m_dirs[i]).arg(m_fileName), QSettings::IniFormat);

        settings.beginGroup(QString("Desktop Entry"));
        setName(settings.value(QString("Name"), m_name).toString());
        setComment(settings.value(QString("Comment"), m_comment).toString());
        setApiVersion(settings.value(QString("X-AW-ApiVersion"), m_apiVersion).toInt());
        setTicker(settings.value(QString("X-AW-Ticker"), m_ticker).toString());
        setActive(settings.value(QString("X-AW-Active"), QVariant(m_active)).toString() == QString("true"));
        setInterval(settings.value(QString("X-AW-Interval"), m_interval).toInt());
        setNumber(settings.value(QString("X-AW-Number"), m_number).toInt());
        settings.endGroup();
    }

    // update for current API
    if ((m_apiVersion > 0) && (m_apiVersion < AWEQAPI)) {
        setApiVersion(AWEQAPI);
        writeConfiguration();
    }
}


QMap<QString, float> ExtQuotes::run()
{
    if (debug) qDebug() << PDEBUG;
    if ((!m_active) || (isRunning)) return values;

    if (times == 1) {
        if (debug) qDebug() << PDEBUG << ":" << "Send request";
        isRunning = true;
        QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url())));
        new QReplyTimeout(reply, 1000);
    }

    // update value
    if (times >= m_interval) times = 0;
    times++;

    return values;
}


int ExtQuotes::showConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    ui->lineEdit_name->setText(m_name);
    ui->lineEdit_comment->setText(m_comment);
    ui->label_numberValue->setText(QString("%1").arg(m_number));
    ui->lineEdit_ticker->setText(m_ticker);
    ui->checkBox_active->setCheckState(m_active ? Qt::Checked : Qt::Unchecked);
    ui->spinBox_interval->setValue(m_interval);

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


bool ExtQuotes::tryDelete()
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


void ExtQuotes::writeConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QSettings settings(QString("%1/%2").arg(m_dirs[0]).arg(m_fileName), QSettings::IniFormat);
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("Encoding"), QString("UTF-8"));
    settings.setValue(QString("Name"), m_name);
    settings.setValue(QString("Comment"), m_comment);
    settings.setValue(QString("X-AW-Ticker"), m_ticker);
    settings.setValue(QString("X-AW-ApiVersion"), m_apiVersion);
    settings.setValue(QString("X-AW-Active"), QVariant(m_active).toString());
    settings.setValue(QString("X-AW-Interval"), m_interval);
    settings.setValue(QString("X-AW-Number"), m_number);
    settings.endGroup();

    settings.sync();
}

void ExtQuotes::quotesReplyReceived(QNetworkReply *reply)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Return code" << reply->error();
    if (debug) qDebug() << PDEBUG << ":" << "Reply error message" << reply->errorString();

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
    values[QString("askchg")] = values[QString("ask")] == 0 ? 0.0 : value - values[QString("ask")];
    values[QString("percaskchg")] = 100 * values[QString("askchg")] / values[QString("ask")];
    values[QString("ask")] = value;

    // bid
    value = jsonQuotes[QString("Bid")].toString().toFloat();
    values[QString("bidchg")] = values[QString("bid")] == 0 ? 0.0 : value - values[QString("bid")];
    values[QString("percbidchg")] = 100 * values[QString("bidchg")] / values[QString("bid")];
    values[QString("bid")] = value;

    // last trade
    value = jsonQuotes[QString("LastTradePriceOnly")].toString().toFloat();
    values[QString("pricechg")] = values[QString("price")] == 0 ? 0.0 : value - values[QString("price")];
    values[QString("percpricechg")] = 100 * values[QString("pricechg")] / values[QString("price")];
    values[QString("price")] = value;

    isRunning = false;
}


QString ExtQuotes::url()
{
    if (debug) qDebug() << PDEBUG;

    QString apiUrl = QString(YAHOO_URL);
    apiUrl.replace(QString("$TICKER"), m_ticker);
    if (debug) qDebug() << PDEBUG << ":" << "API url" << apiUrl;

    return apiUrl;
}
