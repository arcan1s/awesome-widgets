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

#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>

#include <pdebug/pdebug.h>

#include "abstractextitemaggregator.h"


template <class T>
class ExtItemAggregator : public AbstractExtItemAggregator
{
public:
    explicit ExtItemAggregator(QWidget *parent, const QString type,
                               const bool debugCmd = false)
        : AbstractExtItemAggregator(parent, debugCmd),
          debug(debugCmd),
          m_type(type)
    {
        initItems();
    };

    ~ExtItemAggregator()
    {
        if (debug) qDebug() << PDEBUG;

        m_items.clear();
    }

    void editItems()
    {
        if (debug) qDebug() << PDEBUG;

        repaint();
        int ret = dialog->exec();
        if (debug) qDebug() << PDEBUG << ":" << "Dialog returns" << ret;
    };

    void initItems()
    {
        if (debug) qDebug() << PDEBUG;

        m_items.clear();
        m_items = getItems();
    };

    T *itemByTag(const QString _tag) const
    {
        if (debug) qDebug() << PDEBUG;

        T *found = nullptr;
        foreach(T *item, m_items) {
            if (item->tag() != _tag) continue;
            found = item;
            break;
        }

        return found;
    }

    T *itemByTagNumber(const int _number) const
    {
        if (debug) qDebug() << PDEBUG;

        T *found = nullptr;
        foreach(T *item, m_items) {
            if (item->number() != _number) continue;
            found = item;
            break;
        }

        return found;
    }

    T *itemFromWidget() const
    {
        if (debug) qDebug() << PDEBUG;

        QListWidgetItem *widgetItem = widgetDialog->currentItem();
        if (widgetItem == nullptr) return nullptr;

        T *found = nullptr;
        foreach(T *item, m_items) {
            if (item->fileName() != widgetItem->text()) continue;
            found = item;
            break;
        }

        return found;
    };

    QList<T *> items() const
    {
        if (debug) qDebug() << PDEBUG;

        return m_items;
    };

    int uniqNumber() const
    {
        if (debug) qDebug() << PDEBUG;

        QList<int> tagList;
        foreach(T *item, m_items) tagList.append(item->number());
        int number = 0;
        while (tagList.contains(number)) number++;

        return number;
    };

private:
    bool debug;
    QList<T *> m_items;
    QString m_type;

    // init method
    QList<T *> getItems()
    {
        if (debug) qDebug() << PDEBUG;

        // create directory at $HOME
        QString localDir = QString("%1/awesomewidgets/%2")
                           .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation))
                           .arg(m_type);
        QDir localDirectory;
        if (localDirectory.mkpath(localDir))
            if (debug) qDebug() << PDEBUG << ":" << "Created directory" << localDir;

        QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                           QString("awesomewidgets/%1").arg(m_type),
                           QStandardPaths::LocateDirectory);
        QStringList names;
        QList<T *> items;
        foreach(QString dir, dirs) {
            QStringList files = QDir(dir).entryList(QDir::Files, QDir::Name);
            foreach(QString file, files) {
                if (!file.endsWith(QString(".desktop"))) continue;
                if (names.contains(file)) continue;
                if (debug) qDebug() << PDEBUG << ":" << "Found file" << file << "in" << dir;
                names.append(file);
                items.append(new T(this, file, dirs, debug));
            }
        }

        return items;
    };

    void repaint()
    {
        widgetDialog->clear();
        foreach(T *_item, m_items) {
            QListWidgetItem *item = new QListWidgetItem(_item->fileName(), widgetDialog);
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
        if (debug) qDebug() << PDEBUG;

        T *source = itemFromWidget();
        QString fileName = getName();
        int number = uniqNumber();
        if ((source == nullptr) || (fileName.isEmpty())) return;

        T *newItem = source->copy(fileName, number);
        if (newItem->showConfiguration(configArgs()) == 1) {
            initItems();
            repaint();
        }
    };

    void createItem()
    {
        if (debug) qDebug() << PDEBUG;

        QString fileName = getName();
        int number = uniqNumber();
        QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                           QString("awesomewidgets/%1").arg(m_type),
                           QStandardPaths::LocateDirectory);
        if (fileName.isEmpty()) return;

        T *newItem = new T(this, fileName, dirs, debug);
        newItem->setNumber(number);
        if (newItem->showConfiguration(configArgs()) == 1) {
            initItems();
            repaint();
        }
    };

    void deleteItem()
    {
        if (debug) qDebug() << PDEBUG;

        T *source = itemFromWidget();
        if (source == nullptr) return;

        if (source->tryDelete()) {
            initItems();
            repaint();
        }
    };

    void editItem()
    {
        if (debug) qDebug() << PDEBUG;

        T *source = itemFromWidget();
        if (source == nullptr) return;

        if (source->showConfiguration(configArgs()) == 1) {
            initItems();
            repaint();
        }
    };
};


#endif /* EXTITEMAGGREGATOR_H */
