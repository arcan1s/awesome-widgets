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

#include "extnetworkrequest.h"
#include "ui_extnetworkrequest.h"

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QSettings>
#include <QTextCodec>

#include <qreplytimeout/qreplytimeout.h>

#include "awdebug.h"


ExtNetworkRequest::ExtNetworkRequest(QWidget *parent, const QString filePath)
    : AbstractExtItem(parent, filePath)
    , ui(new Ui::ExtNetworkRequest)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    if (!filePath.isEmpty())
        readConfiguration();
    ui->setupUi(this);
    translate();

    m_values[tag(QString("response"))] = QString();

    // HACK declare as child of nullptr to avoid crash with plasmawindowed
    // in the destructor
    m_manager = new QNetworkAccessManager(nullptr);
    connect(m_manager, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(networkReplyReceived(QNetworkReply *)));
}


ExtNetworkRequest::~ExtNetworkRequest()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    disconnect(m_manager, SIGNAL(finished(QNetworkReply *)), this,
               SLOT(networkReplyReceived(QNetworkReply *)));

    m_manager->deleteLater();
    delete ui;
}


ExtNetworkRequest *ExtNetworkRequest::copy(const QString _fileName,
                                           const int _number)
{
    qCDebug(LOG_LIB) << "File" << _fileName << "with number" << _number;

    ExtNetworkRequest *item
        = new ExtNetworkRequest(static_cast<QWidget *>(parent()), _fileName);
    copyDefaults(item);
    item->setNumber(_number);
    item->setStringUrl(stringUrl());

    return item;
}


QString ExtNetworkRequest::stringUrl() const
{
    return m_stringUrl;
}


QString ExtNetworkRequest::uniq() const
{
    return m_url.toString();
}


void ExtNetworkRequest::setStringUrl(const QString _url)
{
    qCDebug(LOG_LIB) << "Url" << _url;

    m_stringUrl = _url;
    initUrl();
}


void ExtNetworkRequest::readConfiguration()
{
    AbstractExtItem::readConfiguration();

    QSettings settings(fileName(), QSettings::IniFormat);

    settings.beginGroup(QString("Desktop Entry"));
    setStringUrl(settings.value(QString("X-AW-Url"), stringUrl()).toString());
    settings.endGroup();

    bumpApi(AW_EXTNETREQUEST_API);
}


QVariantHash ExtNetworkRequest::run()
{
    if ((!isActive()) || (m_isRunning))
        return m_values;

    if (m_times == 1) {
        qCInfo(LOG_LIB) << "Send request";
        m_isRunning = true;
        QNetworkReply *reply = m_manager->get(QNetworkRequest(m_url));
        new QReplyTimeout(reply, REQUEST_TIMEOUT);
    }

    // update value
    if (m_times >= interval())
        m_times = 0;
    m_times++;

    return m_values;
}


int ExtNetworkRequest::showConfiguration(const QVariant args)
{
    Q_UNUSED(args)

    ui->lineEdit_name->setText(name());
    ui->lineEdit_comment->setText(comment());
    ui->label_numberValue->setText(QString("%1").arg(number()));
    ui->lineEdit_url->setText(stringUrl());
    ui->checkBox_active->setCheckState(isActive() ? Qt::Checked
                                                  : Qt::Unchecked);
    ui->spinBox_interval->setValue(interval());

    int ret = exec();
    if (ret != 1)
        return ret;
    setName(ui->lineEdit_name->text());
    setComment(ui->lineEdit_comment->text());
    setNumber(ui->label_numberValue->text().toInt());
    setApiVersion(AW_EXTNETREQUEST_API);
    setStringUrl(ui->lineEdit_url->text());
    setActive(ui->checkBox_active->checkState() == Qt::Checked);
    setInterval(ui->spinBox_interval->value());

    writeConfiguration();
    return ret;
}


void ExtNetworkRequest::writeConfiguration() const
{
    AbstractExtItem::writeConfiguration();

    QSettings settings(writtableConfig(), QSettings::IniFormat);
    qCInfo(LOG_LIB) << "Configuration file" << settings.fileName();

    settings.beginGroup(QString("Desktop Entry"));
    settings.setValue(QString("X-AW-Url"), stringUrl());
    settings.endGroup();

    settings.sync();
}

void ExtNetworkRequest::networkReplyReceived(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qCWarning(LOG_AW) << "An error occurs" << reply->error()
                          << "with message" << reply->errorString();
        return;
    }

    m_isRunning = false;
    m_values[tag(QString("response"))]
        = QTextCodec::codecForMib(106)->toUnicode(reply->readAll()).trimmed();

    emit(dataReceived(m_values));
}


void ExtNetworkRequest::initUrl()
{
    m_url = QUrl(m_stringUrl);
}


void ExtNetworkRequest::translate()
{
    ui->label_name->setText(i18n("Name"));
    ui->label_comment->setText(i18n("Comment"));
    ui->label_number->setText(i18n("Tag"));
    ui->label_url->setText(i18n("URL"));
    ui->checkBox_active->setText(i18n("Active"));
    ui->label_interval->setText(i18n("Interval"));
}
