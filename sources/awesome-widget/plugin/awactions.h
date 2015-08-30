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


#ifndef AWACTIONS_H
#define AWACTIONS_H

#include <QObject>
#include <QVariant>


class QNetworkReply;

class AWActions : public QObject
{
    Q_OBJECT

public:
    explicit AWActions(QObject *parent = nullptr);
    virtual ~AWActions();

    Q_INVOKABLE void checkUpdates();
    Q_INVOKABLE bool dropCache() const;
    Q_INVOKABLE bool isDebugEnabled() const;
    Q_INVOKABLE void runCmd(const QString cmd = QString("/usr/bin/true")) const;
    Q_INVOKABLE void sendEmail() const;
    Q_INVOKABLE void showReadme() const;
    // configuration slots
    Q_INVOKABLE QString getAboutText(const QString type = QString("header")) const;
    Q_INVOKABLE QVariantMap getFont(const QVariantMap defaultFont) const;
    // dataengine
    Q_INVOKABLE QVariantMap readDataEngineConfiguration() const;
    Q_INVOKABLE void writeDataEngineConfiguration(const QVariantMap configuration) const;

public slots:
    Q_INVOKABLE static void sendNotification(const QString eventId, const QString message,
                                             const bool enablePopup = false);

private slots:
    void showUpdates(QString version) const;
    void versionReplyRecieved(QNetworkReply *reply) const;

private:
};


#endif /* AWACTIONS_H */
