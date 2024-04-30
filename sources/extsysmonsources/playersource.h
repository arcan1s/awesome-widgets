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

    explicit PlayerSource(QObject *_parent, QString _player, QString _mpdAddress, int _mpdPort, QString _mpris,
                          int _symbols);
    ~PlayerSource() override;
    [[nodiscard]] QVariant data(const QString &_source) override;
    [[nodiscard]] static QString getAutoMpris();
    void run();
    [[nodiscard]] QHash<QString, KSysGuard::SensorInfo *> sources() const override;
    // additional method to build dynamic tags
    [[nodiscard]] static QString buildString(const QString &_current, const QString &_value, int _s);
    [[nodiscard]] static QString stripString(const QString &_value, int _s);
    // additional test method
    [[nodiscard]] bool isMpdSocketConnected() const;

private slots:
    void mpdSocketConnected();
    void mpdSocketReadyRead();
    void mpdSocketWritten(qint64 _bytes);

private:
    [[nodiscard]] static inline QVariantHash defaultInfo();
    [[nodiscard]] QVariantHash getPlayerMpdInfo();
    [[nodiscard]] static QVariantHash getPlayerMprisInfo(const QString &_mpris);
    QTcpSocket m_mpdSocket;
    // configuration and values
    QString m_mpdAddress;
    int m_mpdPort;
    QVariantHash m_mpdCached;
    QString m_mpris;
    QMutex m_dbusMutex;
    QString m_player;
    int m_symbols;
    QStringList m_metadata = {"album", "artist", "title"};
    QVariantHash m_values;
};
