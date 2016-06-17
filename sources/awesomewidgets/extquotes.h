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

#ifndef EXTQUOTES_H
#define EXTQUOTES_H

#include <QNetworkReply>

#include "abstractextitem.h"

#define YAHOO_QUOTES_URL "https://query.yahooapis.com/v1/public/yql"
#define YAHOO_QUOTES_QUERY                                                     \
    "select * from yahoo.finance.quotes where symbol='%1'"


namespace Ui
{
class ExtQuotes;
}

class ExtQuotes : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(QString ticker READ ticker WRITE setTicker)

public:
    explicit ExtQuotes(QWidget *parent, const QString filePath = QString());
    virtual ~ExtQuotes();
    ExtQuotes *copy(const QString _fileName, const int _number);
    // get methods
    QString ticker() const;
    QString uniq() const;
    // set methods
    void setTicker(const QString _ticker = QString("EURUSD=X"));

public slots:
    void readConfiguration();
    QVariantHash run();
    int showConfiguration(const QVariant args = QVariant());
    void writeConfiguration() const;

private slots:
    void quotesReplyReceived(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager = nullptr;
    QUrl m_url;
    bool isRunning = false;
    Ui::ExtQuotes *ui = nullptr;
    void initUrl();
    void translate();
    // properties
    QString m_ticker = QString("EURUSD=X");
    // values
    int times = 0;
    QVariantHash values;
};


#endif /* EXTQUOTES_H */
