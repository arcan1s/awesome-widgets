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

#ifndef PLAYERSOURCE_H
#define PLAYERSOURCE_H

#include <QMutex>
#include <QObject>
#include <QTcpSocket>

#include "abstractextsysmonsource.h"


class QProcess;

class PlayerSource : public AbstractExtSysMonSource
{
    Q_OBJECT

public:
    const char *MPD_STATUS_REQUEST = "currentsong\nstatus\n";

    explicit PlayerSource(QObject *parent, const QStringList &args);
    virtual ~PlayerSource();
    QVariant data(const QString &source);
    QString getAutoMpris() const;
    QVariantMap initialData(const QString &source) const;
    void run();
    QStringList sources() const;
    // additional method to build dynamic tags
    static QString buildString(const QString &current, const QString &value,
                               const int s);
    static QString stripString(const QString &value, const int s);
    // additional test method
    bool isMpdSocketConnected() const;

private slots:
    void mpdSocketConnected();
    void mpdSocketReadyRead();
    void mpdSocketWritten(const qint64 bytes);

private:
    inline QVariantHash defaultInfo() const;
    QVariantHash getPlayerMpdInfo();
    QVariantHash getPlayerMprisInfo(const QString &mpris) const;
    QTcpSocket m_mpdSocket;
    // configuration and values
    QString m_mpdAddress;
    int m_mpdPort;
    QVariantHash m_mpdCached;
    QString m_mpris;
    QMutex m_dbusMutex;
    QString m_player;
    int m_symbols;
    QStringList m_metadata = QStringList()
                             << QString("album") << QString("artist")
                             << QString("title");
    QVariantHash m_values;
};


#endif /* PLAYERSOURCE_H */
