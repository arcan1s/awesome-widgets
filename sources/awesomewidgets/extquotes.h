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


class AbstractQuotesProvider;
namespace Ui
{
class ExtQuotes;
}

class ExtQuotes : public AbstractExtItem
{
    Q_OBJECT
    Q_PROPERTY(QString ticker READ ticker WRITE setTicker)

public:
    explicit ExtQuotes(QWidget *_parent = nullptr, const QString &_filePath = "");
    virtual ~ExtQuotes();
    ExtQuotes *copy(const QString &_fileName, const int _number);
    // get methods
    QString ticker() const;
    QString uniq() const;
    // set methods
    void setTicker(const QString &_ticker);

public slots:
    void readConfiguration();
    QVariantHash run();
    int showConfiguration(const QVariant &_args);
    void writeConfiguration() const;

private slots:
    void quotesReplyReceived(QNetworkReply *_reply);
    void sendRequest();

private:
    AbstractQuotesProvider *m_providerObject = nullptr;
    QNetworkAccessManager *m_manager = nullptr;
    bool m_isRunning = false;
    Ui::ExtQuotes *ui = nullptr;
    void initProvider();
    void translate();
    // properties
    QString m_ticker = "EURUSD=X";
    // values
    QVariantHash m_values;
};


#endif /* EXTQUOTES_H */
