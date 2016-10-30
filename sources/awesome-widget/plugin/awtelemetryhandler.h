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
#include <QtCore/QVariant>


class QAbstractButton;
class QNetworkReply;

class AWTelemetryHandler : public QObject
{
    Q_OBJECT

public:
    const char *REMOTE_TELEMETRY_URL = "http://arcanis.me/telemetry";
    const int REMOTE_TELEMETRY_PORT = 8080;

    explicit AWTelemetryHandler(QObject *parent = nullptr,
                                const QString clientId = QString());
    virtual ~AWTelemetryHandler();
    Q_INVOKABLE QStringList get(const QString group) const;
    Q_INVOKABLE QString getLast(const QString group) const;
    Q_INVOKABLE void init(const int count, const bool enableRemote, const QString clientId);
    Q_INVOKABLE bool put(const QString group, const QString value) const;
    Q_INVOKABLE void uploadTelemetry(const QString group, const QString value);

signals:
    void replyReceived(QString message);

private slots:
    void telemetryReplyRecieved(QNetworkReply *reply);

private:
    QString getKey(const int count) const;
    QString m_clientId;
    QString m_localFile;
    int m_storeCount = 0;
    bool m_uploadEnabled = false;
};


#endif /* AWTELEMETRYHANDLER_H */
