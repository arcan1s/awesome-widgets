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
#include <QDomDocument>
#include <QDomElement>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
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
    values[QString("bid")] = 0.0;
    values[QString("price")] = 0.0;
}


ExtQuotes::~ExtQuotes()
{
    if (debug) qDebug() << PDEBUG;

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


void ExtQuotes::setActive(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    m_active = state;
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
    if (!m_active) return values;

    if (times == 1) {
        if (debug) qDebug() << PDEBUG << ":" << "Send request";
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(quotesReplyReceived(QNetworkReply *)));

        manager->get(QNetworkRequest(QUrl(url())));
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
    if (m_active)
        ui->checkBox_active->setCheckState(Qt::Checked);
    else
        ui->checkBox_active->setCheckState(Qt::Unchecked);
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
    if (debug) qDebug() << PDEBUG << reply->url();

    QDomDocument doc;
    if (!doc.setContent(reply)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not parse answer to XML";
        return;
    }
    QDomNodeList fields;

    // ask
    fields = doc.elementsByTagName(QString("Ask"));
    for (int i=0; i<fields.size(); i++) {
        values[QString("ask")] = fields.item(i).toElement().text().toFloat();
        if (debug) qDebug() << PDEBUG << "Found ask" << values[QString("ask")];
    }

    // bid
    fields = doc.elementsByTagName(QString("Bid"));
    for (int i=0; i<fields.size(); i++) {
        values[QString("bid")] = fields.item(i).toElement().text().toFloat();
        if (debug) qDebug() << PDEBUG << "Found bid" << values[QString("bid")];
    }

    // last trade
    fields = doc.elementsByTagName(QString("LastTradePriceOnly"));
    for (int i=0; i<fields.size(); i++) {
        values[QString("price")] = fields.item(i).toElement().text().toFloat();
        if (debug) qDebug() << PDEBUG << "Found last trade" << values[QString("price")];
    }

    reply->deleteLater();
}


QString ExtQuotes::url()
{
    if (debug) qDebug() << PDEBUG;

    QString apiUrl = QString(YAHOO_URL);
    apiUrl.replace(QString("$TICKER"), m_ticker);
    if (debug) qDebug() << PDEBUG << ":" << "API url" << apiUrl;

    return apiUrl;
}
