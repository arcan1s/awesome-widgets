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


#ifndef AWTELEMETRYHANDLER_H
#define AWTELEMETRYHANDLER_H

#include <QObject>


class QNetworkReply;

class AWTelemetryHandler : public QObject
{
    Q_OBJECT

public:
    const char *REMOTE_TELEMETRY_URL = "https://arcanis.me/telemetry";

    explicit AWTelemetryHandler(QObject *_parent = nullptr, const QString &_clientId = "");
    ~AWTelemetryHandler() override;
    Q_INVOKABLE QStringList get(const QString &_group) const;
    Q_INVOKABLE QString getLast(const QString &_group) const;
    Q_INVOKABLE void init(const int _count, const bool _enableRemote, const QString &_clientId);
    Q_INVOKABLE bool put(const QString &_group, const QString &_value) const;
    Q_INVOKABLE void uploadTelemetry(const QString &_group, const QString &_value);

signals:
    void replyReceived(const QString &_message);

private slots:
    void telemetryReplyRecieved(QNetworkReply *_reply);

private:
    static QString getKey(const int _count);
    QString m_clientId;
    QString m_localFile;
    int m_storeCount = 0;
    bool m_uploadEnabled = false;
};


#endif /* AWTELEMETRYHANDLER_H */
