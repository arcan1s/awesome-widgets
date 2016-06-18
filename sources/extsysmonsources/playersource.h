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

#include "abstractextsysmonsource.h"


#ifndef DBUS_CALL_TIMEOUT
#define DBUS_CALL_TIMEOUT 3000
#endif /* DBUS_CALL_TIMEOUT */

class QProcess;

class PlayerSource : public AbstractExtSysMonSource
{
public:
    explicit PlayerSource(QObject *parent, const QStringList args);
    virtual ~PlayerSource();
    QVariant data(QString source);
    QString getAutoMpris() const;
    QVariantMap initialData(QString source) const;
    void run();
    QStringList sources() const;
    // additional method to build dynamic tags
    static QString buildString(const QString &current, const QString &value,
                               const int s);
    static QString stripString(const QString &value, const int s);

private slots:
    void updateMpdValue();

private:
    inline QVariantHash defaultInfo() const;
    QVariantHash getPlayerMpdInfo(const QString mpdAddress) const;
    QVariantHash getPlayerMprisInfo(const QString mpris) const;
    // configuration and values
    QString m_mpdAddress;
    QVariantHash m_mpdCached;
    QProcess *m_mpdProcess = nullptr;
    QString m_mpris;
    QMutex m_dbusMutex;
    QString m_player;
    int m_symbols;
    QStringList m_metadata = QStringList() << QString("album")
                                           << QString("artist")
                                           << QString("title");
    QVariantHash m_values;
};


#endif /* PLAYERSOURCE_H */
