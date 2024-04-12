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

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QSettings>
#include <QStandardPaths>

#include <ranges>

#include "abstractextitemaggregator.h"
#include "awdebug.h"


template <class T> class ExtItemAggregator : public AbstractExtItemAggregator
{
public:
    explicit ExtItemAggregator(QObject *_parent, const QString &_type)
        : AbstractExtItemAggregator(_parent, _type)
    {
        qSetMessagePattern(AWDebug::LOG_FORMAT);
        qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
        for (auto &metadata : AWDebug::getBuildData())
            qCDebug(LOG_LIB) << metadata;

        qCDebug(LOG_LIB) << "Type" << _type;

        ExtItemAggregator::initItems();
    };

    ~ExtItemAggregator() override
    {
        qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

        m_items.clear();
    };

    auto activeItems()
    {
        return m_items | std::ranges::views::filter([](auto item) { return item->isActive(); });
    };

    void editItems()
    {
        auto ret = exec();
        qCInfo(LOG_LIB) << "Dialog returns" << ret;
    };

    void initItems() override
    {
        m_items.clear();
        m_items = getItems();
    };

    void initSockets()
    {
        // HACK as soon as per one widget instance we have two objects each of
        // them will try to control socket, whereas actually only one of them
        // should be owner of the socket
        for (auto item : m_items)
            item->initSocket();
    }

    T *itemByTag(const QString &_tag, const QString &_type) const
    {
        qCDebug(LOG_LIB) << "Tag" << _tag << "with used type" << _type;

        auto found = std::find_if(m_items.cbegin(), m_items.cend(),
                                  [&_tag, &_type](auto item) { return item->tag(_type) == _tag; });

        if (found == std::end(m_items)) {
            qCWarning(LOG_LIB) << "Could not find item by tag" << _tag;
            return nullptr;
        }
        return static_cast<T *>(*found);
    };

    T *itemByTagNumber(const int _number) const
    {
        qCDebug(LOG_LIB) << "Number" << _number;

        auto found = std::find_if(m_items.cbegin(), m_items.cend(),
                                  [_number](auto item) { return item->number() == _number; });

        if (found == std::end(m_items)) {
            qCWarning(LOG_LIB) << "Could not find item by number" << _number;
            return nullptr;
        }
        return static_cast<T *>(*found);
    };

    [[nodiscard]] QList<AbstractExtItem *> items() const override { return m_items; };

private:
    QList<AbstractExtItem *> m_items;

    void doCreateItem(QListWidget *_widget) override { return createItem<T>(_widget); }

    QList<AbstractExtItem *> getItems()
    {
        QList<AbstractExtItem *> items;

        for (auto &dir : directories()) {
            auto files = QDir(dir).entryList(QDir::Files, QDir::Name);
            for (auto &file : files) {
                // check filename
                if (!file.endsWith(".desktop"))
                    continue;
                qCInfo(LOG_LIB) << "Found file" << file << "in" << dir;
                auto filePath = QString("%1/%2").arg(dir, file);
                // check if already exists
                if (std::any_of(items.cbegin(), items.cend(), [&file](auto item) { return item->fileName() == file; }))
                    continue;
                items.append(new T(this, filePath));
            }
        }

        // sort items
        std::sort(items.begin(), items.end(), [](auto lhs, auto rhs) { return lhs->number() < rhs->number(); });
        return items;
    };
};
