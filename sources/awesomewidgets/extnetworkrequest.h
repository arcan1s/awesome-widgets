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

#pragma once

#include <QNetworkReply>

#include "abstractextitem.h"


class ExtNetworkRequest : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(QString stringUrl READ stringUrl WRITE setStringUrl)

public:
    explicit ExtNetworkRequest(QObject *_parent = nullptr, const QString &_filePath = "");
    ~ExtNetworkRequest() override;
    ExtNetworkRequest *copy(const QString &_fileName, int _number) override;
    // get methods
    [[nodiscard]] QString stringUrl() const;
    [[nodiscard]] QString uniq() const override;
    // set methods
    void setStringUrl(const QString &_url);

public slots:
    void readConfiguration() override;
    QVariantHash run() override;
    int showConfiguration(QWidget *_parent, const QVariant &_args) override;
    void writeConfiguration() const override;

private slots:
    void networkReplyReceived(QNetworkReply *_reply);
    void sendRequest();

private:
    QNetworkAccessManager *m_manager = nullptr;
    QUrl m_url;
    bool m_isRunning = false;
    void initUrl();
    void translate(void *_ui) override;
    // properties
    QString m_stringUrl = "https://httpbin.org/get";
    // values
    QVariantHash m_values;
};
