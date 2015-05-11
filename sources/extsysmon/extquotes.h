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

#include <QDialog>
#include <QMap>

#define YAHOO_URL "https://query.yahooapis.com/v1/public/yql?q=select * from yahoo.finance.quotes where symbol in (\"$TICKER\")&env=store://datatables.org/alltableswithkeys"


class QNetworkReply;

namespace Ui {
    class ExtQuotes;
}

class ExtQuotes : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int apiVersion READ apiVersion WRITE setApiVersion)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString comment READ comment WRITE setComment)
    Q_PROPERTY(int interval READ interval WRITE setInterval)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_PROPERTY(int number READ number WRITE setNumber)
    Q_PROPERTY(QString ticker READ ticker WRITE setTicker)

public:
    explicit ExtQuotes(QWidget *parent = nullptr, const QString quotesName = QString(),
                        const QStringList directories = QStringList(), const bool debugCmd = false);
    ~ExtQuotes();
    // get methods
    int apiVersion();
    QString comment();
    QString fileName();
    int interval();
    bool isActive();
    QString name();
    int number();
    QString tag(const QString _type = QString("price"));
    QString ticker();
    // set methods
    void setApiVersion(const int _apiVersion = 0);
    void setActive(const bool _state = true);
    void setComment(const QString _comment = QString("empty"));
    void setInterval(const int _interval = 0);
    void setName(const QString _name = QString("none"));
    void setNumber(int _number = -1);
    void setTicker(const QString _ticker = QString("EURUSD=X"));

public slots:
    void readConfiguration();
    QMap<QString, float> run();
    int showConfiguration();
    bool tryDelete();
    void writeConfiguration();

private slots:
    void quotesReplyReceived(QNetworkReply *reply);

private:
    QString m_fileName;
    QStringList m_dirs;
    bool debug;
    Ui::ExtQuotes *ui;
    QString url();
    // properties
    int m_apiVersion = 0;
    bool m_active = true;
    QString m_comment = QString("empty");
    int m_interval = 60;
    QString m_name = QString("none");
    int m_number = -1;
    QString m_ticker = QString("EURUSD=X");
    // values
    int times = 0;
    QMap<QString, float> values;
};


#endif /* EXTQUOTES_H */
