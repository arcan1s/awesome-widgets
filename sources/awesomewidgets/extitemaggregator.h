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
    explicit ExtItemAggregator(QWidget *parent, const QString type)
        : AbstractExtItemAggregator(parent)
        , m_type(type)
    {
        qSetMessagePattern(LOG_FORMAT);
        qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;
        for (auto metadata : getBuildData())
            qCDebug(LOG_LIB) << metadata;

        qCDebug(LOG_LIB) << "Type" << type;

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
        repaint();
        int ret = dialog->exec();
        qCInfo(LOG_LIB) << "Dialog returns" << ret;
    };

    T *itemByTag(const QString _tag) const
    {
        qCDebug(LOG_LIB) << "Tag" << _tag;

        T *found = nullptr;
        for (auto item : m_items) {
            if (item->tag() != _tag)
                continue;
            found = item;
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
        for (auto item : m_items) {
            if (item->number() != _number)
                continue;
            found = item;
            break;
        }
        if (found == nullptr)
            qCWarning(LOG_LIB) << "Could not find item by number" << _number;

        return found;
    };

    T *itemFromWidget() const
    {
        QListWidgetItem *widgetItem = widgetDialog->currentItem();
        if (widgetItem == nullptr)
            return nullptr;

        T *found = nullptr;
        for (auto item : m_items) {
            if (item->fileName() != widgetItem->text())
                continue;
            found = item;
            break;
        }
        if (found == nullptr)
            qCWarning(LOG_LIB) << "Could not find item by name"
                               << widgetItem->text();

        return found;
    };

    QList<T *> items() const { return m_items; };

    int uniqNumber() const
    {
        QList<int> tagList;
        for (auto item : m_items)
            tagList.append(item->number());
        int number = 0;
        while (tagList.contains(number))
            number++;

        return number;
    };

private:
    QList<T *> m_items;
    QList<T *> m_activeItems;
    QString m_type;

    // init method
    QList<T *> getItems()
    {
        // create directory at $HOME
        QString localDir = QString("%1/awesomewidgets/%2")
                               .arg(QStandardPaths::writableLocation(
                                   QStandardPaths::GenericDataLocation))
                               .arg(m_type);
        QDir localDirectory;
        if (localDirectory.mkpath(localDir))
            qCInfo(LOG_LIB) << "Created directory" << localDir;

        QStringList dirs = QStandardPaths::locateAll(
            QStandardPaths::GenericDataLocation,
            QString("awesomewidgets/%1").arg(m_type),
            QStandardPaths::LocateDirectory);
        QStringList names;
        QList<T *> items;
        for (auto dir : dirs) {
            QStringList files = QDir(dir).entryList(QDir::Files, QDir::Name);
            for (auto file : files) {
                if ((!file.endsWith(QString(".desktop")))
                    || (names.contains(file)))
                    continue;
                qCInfo(LOG_LIB) << "Found file" << file << "in" << dir;
                names.append(file);
                items.append(new T(this, file, dirs));
            }
        }

        // sort items
        std::sort(items.begin(), items.end(), [](const T *lhs, const T *rhs) {
            return lhs->number() < rhs->number();
        });
        return items;
    };

    void initItems()
    {
        m_items.clear();
        m_activeItems.clear();

        m_items = getItems();
        for (auto item : m_items) {
            if (!item->isActive())
                continue;
            m_activeItems.append(item);
        }
    };

    void repaint()
    {
        widgetDialog->clear();
        for (auto _item : m_items) {
            QListWidgetItem *item
                = new QListWidgetItem(_item->fileName(), widgetDialog);
            QStringList tooltip;
            tooltip.append(i18n("Name: %1", _item->name()));
            tooltip.append(i18n("Comment: %1", _item->comment()));
            tooltip.append(i18n("Identity: %1", _item->uniq()));
            item->setToolTip(tooltip.join(QChar('\n')));
            widgetDialog->addItem(item);
        }
    };

    // methods
    void copyItem()
    {
        T *source = itemFromWidget();
        QString fileName = getName();
        int number = uniqNumber();
        if ((source == nullptr) || (fileName.isEmpty())) {
            qCWarning(LOG_LIB) << "Nothing to copy";
            return;
        }

        T *newItem = source->copy(fileName, number);
        if (newItem->showConfiguration(configArgs()) == 1) {
            initItems();
            repaint();
        }
    };

    void createItem()
    {
        QString fileName = getName();
        int number = uniqNumber();
        QStringList dirs = QStandardPaths::locateAll(
            QStandardPaths::GenericDataLocation,
            QString("awesomewidgets/%1").arg(m_type),
            QStandardPaths::LocateDirectory);
        if (fileName.isEmpty()) {
            qCWarning(LOG_LIB) << "Nothing to create";
            return;
        };

        T *newItem = new T(this, fileName, dirs);
        newItem->setNumber(number);
        if (newItem->showConfiguration(configArgs()) == 1) {
            initItems();
            repaint();
        }
    };

    void deleteItem()
    {
        T *source = itemFromWidget();
        if (source == nullptr) {
            qCWarning(LOG_LIB) << "Nothing to delete";
            return;
        };

        if (source->tryDelete()) {
            initItems();
            repaint();
        }
    };

    void editItem()
    {
        T *source = itemFromWidget();
        if (source == nullptr) {
            qCWarning(LOG_LIB) << "Nothing to edit";
            return;
        };

        if (source->showConfiguration(configArgs()) == 1) {
            initItems();
            repaint();
        }
    };
};


#endif /* EXTITEMAGGREGATOR_H */
