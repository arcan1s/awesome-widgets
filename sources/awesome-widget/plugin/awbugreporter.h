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

#include <QObject>


class QNetworkAccessManager;
class QNetworkReply;

class __attribute__((visibility("default"))) AWBugReporter : public QObject
{
    Q_OBJECT

public:
    explicit AWBugReporter(QObject *_parent = nullptr);
    ~AWBugReporter() override;
    Q_INVOKABLE void doConnect() const;
    Q_INVOKABLE static QString generateText(const QString &_description, const QString &_reproduce,
                                            const QString &_expected, const QString &_logs);
    Q_INVOKABLE void sendBugReport(const QString &_title, const QString &_body);

signals:
    void replyReceived(int _number, const QString &_url);

private slots:
    void issueReplyReceived(QNetworkReply *_reply);
    void openBugReport();
    void showInformation(int _number, const QString &_url);

private:
    QString m_lastBugUrl;
    QNetworkAccessManager *m_manager = nullptr;
};
