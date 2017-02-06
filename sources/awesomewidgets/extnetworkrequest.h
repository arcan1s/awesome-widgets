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

#ifndef EXTNETWORKREQUEST_H
#define EXTNETWORKREQUEST_H

#include <QNetworkReply>

#include "abstractextitem.h"


namespace Ui
{
class ExtNetworkRequest;
}

class ExtNetworkRequest : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(QString stringUrl READ stringUrl WRITE setStringUrl)

public:
    explicit ExtNetworkRequest(QWidget *parent,
                               const QString filePath = QString());
    virtual ~ExtNetworkRequest();
    ExtNetworkRequest *copy(const QString _fileName, const int _number);
    // get methods
    QString stringUrl() const;
    QString uniq() const;
    // set methods
    void setStringUrl(const QString _url = QString("https://httpbin.org/get"));

public slots:
    void readConfiguration();
    QVariantHash run();
    int showConfiguration(const QVariant args = QVariant());
    void writeConfiguration() const;

private slots:
    void networkReplyReceived(QNetworkReply *reply);
    void sendRequest();

private:
    QNetworkAccessManager *m_manager = nullptr;
    QUrl m_url;
    bool m_isRunning = false;
    Ui::ExtNetworkRequest *ui = nullptr;
    bool canRun() const;
    void initUrl();
    void translate();
    // properties
    QString m_stringUrl = QString("https://httpbin.org/get");
    // values
    int m_times = 0;
    QVariantHash m_values;
};


#endif /* EXTNETWORKREQUEST_H */
