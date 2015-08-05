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

#include <QMap>
#include <QNetworkReply>

#include "abstractextitem.h"

#define YAHOO_URL "https://query.yahooapis.com/v1/public/yql?q=select * from yahoo.finance.quotes where symbol=\"$TICKER\"&env=store://datatables.org/alltableswithkeys&format=json"


namespace Ui {
    class ExtQuotes;
}

class ExtQuotes : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(QString ticker READ ticker WRITE setTicker)

public:
    explicit ExtQuotes(QWidget *parent = nullptr, const QString quotesName = QString(),
                       const QStringList directories = QStringList(),
                       const bool debugCmd = false);
    ~ExtQuotes();
    ExtQuotes *copy(const QString fileName, const int number);
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
    bool debug;
    QNetworkAccessManager *manager;
    bool isRunning = false;
    Ui::ExtQuotes *ui;
    void translate();
    QString url() const;
    // properties
    QString m_ticker = QString("EURUSD=X");
    // values
    int times = 0;
    QVariantHash values;
};


#endif /* EXTQUOTES_H */
