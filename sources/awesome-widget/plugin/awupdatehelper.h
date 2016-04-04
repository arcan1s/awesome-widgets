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


#ifndef AWUPDATEHELPER_H
#define AWUPDATEHELPER_H

#include <QMessageBox>
#include <QObject>
#include <QVersionNumber>


class QNetworkReply;

class AWUpdateHelper : public QObject
{
    Q_OBJECT

public:
    explicit AWUpdateHelper(QObject *parent = nullptr);
    virtual ~AWUpdateHelper();
    void checkUpdates(const bool showAnyway = false);
    bool checkVersion();

private slots:
    void showInfo(const QVersionNumber version);
    void showUpdates(const QVersionNumber version);
    void userReplyOnUpdates(QAbstractButton *button);
    void versionReplyRecieved(QNetworkReply *reply, const bool showAnyway);

private:
    QMessageBox *genMessageBox(const QString title, const QString body,
                               const QMessageBox::StandardButtons buttons);
    QVersionNumber m_foundVersion;
    QString m_genericConfig;
};


#endif /* AWUPDATEHELPER_H */
