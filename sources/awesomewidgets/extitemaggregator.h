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

#ifndef EXTITEMAGGREGATOR_H
#define EXTITEMAGGREGATOR_H

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QSettings>
#include <QStandardPaths>

#include "abstractextitemaggregator.h"
#include "awdebug.h"


template <class T> class ExtItemAggregator : public AbstractExtItemAggregator
{
public:
    explicit ExtItemAggregator(QWidget *_parent, const QString &_type)
        : AbstractExtItemAggregator(_parent, _type)
    {
        qSetMessagePattern(AWDebug::LOG_FORMAT);
        qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
        for (auto &metadata : AWDebug::getBuildData())
            qCDebug(LOG_LIB) << metadata;

        qCDebug(LOG_LIB) << "Type" << _type;

        initItems();
    };

    virtual ~ExtItemAggregator()
    {
        qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

        m_items.clear();
        m_activeItems.clear();
    };

    QList<T *> activeItems() { return m_activeItems; };

    void editItems()
    {
        repaintList();
        int ret = exec();
        qCInfo(LOG_LIB) << "Dialog returns" << ret;
    };

    void initSockets()
    {
        // HACK as soon as per one widget instance we have two objects each of
        // them will try to control socket, whereas actually only one of them
        // should be owner of the socket
        for (auto &item : m_items)
            item->initSocket();
    }

    T *itemByTag(const QString &_tag, const QString &_type) const
    {
        qCDebug(LOG_LIB) << "Tag" << _tag << "with used type" << _type;

        T *found = nullptr;
        for (auto &item : m_items) {
            if (item->tag(_type) != _tag)
                continue;
            found = static_cast<T *>(item);
            break;
        }
        if (found == nullptr)
            qCWarning(LOG_LIB) << "Could not find item by tag" << _tag;

        return found;
    };

    T *itemByTagNumber(const int _number) const
    {
        qCDebug(LOG_LIB) << "Number" << _number;

        T *found = nullptr;
        for (auto &item : m_items) {
            if (item->number() != _number)
                continue;
            found = static_cast<T *>(item);
            break;
        }
        if (found == nullptr)
            qCWarning(LOG_LIB) << "Could not find item by number" << _number;

        return found;
    };

    QList<AbstractExtItem *> items() const { return m_items; };

private:
    QList<AbstractExtItem *> m_items;
    QList<T *> m_activeItems;

    void doCreateItem() { return createItem<T>(); }

    QList<AbstractExtItem *> getItems()
    {
        // create directory at $HOME
        QString localDir = QString("%1/awesomewidgets/%2")
                               .arg(QStandardPaths::writableLocation(
                                   QStandardPaths::GenericDataLocation))
                               .arg(type());
        QDir localDirectory;
        if (localDirectory.mkpath(localDir))
            qCInfo(LOG_LIB) << "Created directory" << localDir;

        QStringList dirs = QStandardPaths::locateAll(
            QStandardPaths::GenericDataLocation,
            QString("awesomewidgets/%1").arg(type()),
            QStandardPaths::LocateDirectory);
        QStringList names;
        QList<AbstractExtItem *> items;
        for (auto &dir : dirs) {
            QStringList files = QDir(dir).entryList(QDir::Files, QDir::Name);
            for (auto &file : files) {
                if ((!file.endsWith(".desktop")) || (names.contains(file)))
                    continue;
                qCInfo(LOG_LIB) << "Found file" << file << "in" << dir;
                names.append(file);
                QString filePath = QString("%1/%2").arg(dir).arg(file);
                items.append(new T(this, filePath));
            }
        }

        // sort items
        std::sort(items.begin(), items.end(),
                  [](const AbstractExtItem *lhs, const AbstractExtItem *rhs) {
                      return lhs->number() < rhs->number();
                  });
        return items;
    };

    void initItems()
    {
        m_items.clear();
        m_activeItems.clear();

        m_items = getItems();
        for (auto &item : m_items) {
            if (!item->isActive())
                continue;
            m_activeItems.append(static_cast<T *>(item));
        }
    };
};


#endif /* EXTITEMAGGREGATOR_H */
