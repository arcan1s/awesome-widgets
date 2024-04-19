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
#include <QVersionNumber>


class KNotification;
class QNetworkReply;

class AWUpdateHelper : public QObject
{
    Q_OBJECT

public:
    explicit AWUpdateHelper(QObject *_parent = nullptr);
    ~AWUpdateHelper() override = default;
    void checkUpdates(bool _showAnyway = false);
    bool checkVersion();

private slots:
    void openReleasesPage();
    static void showInfo(const QVersionNumber &_version);
    void showUpdates(const QVersionNumber &_version) const;
    void versionReplyReceived(QNetworkReply *_reply, bool _showAnyway);

private:
    static KNotification *sendNotification(const QString &_title, const QString &_body);
    QVersionNumber m_foundVersion;
    QString m_genericConfig;
};
