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

#include "awdebug.h"

#include "abstractextitemaggregator.h"


template <class T>
class ExtItemAggregator : public AbstractExtItemAggregator
{
public:
    explicit ExtItemAggregator(QWidget *parent, const QString type,
                               const bool debugCmd = false)
        : AbstractExtItemAggregator(parent, debugCmd),
          m_type(type)
    {
        // logging
        const_cast<QLoggingCategory &>(LOG_ESM()).setEnabled(QtMsgType::QtDebugMsg, debugCmd);
        qSetMessagePattern(LOG_FORMAT);

        initItems();
    };

    virtual ~ExtItemAggregator()
    {
        qCDebug(LOG_ESM);

        m_items.clear();
    }

    void editItems()
    {
        qCDebug(LOG_ESM);

        repaint();
        int ret = dialog->exec();
        qCDebug(LOG_ESM) << "Dialog returns" << ret;
    };

    void initItems()
    {
        qCDebug(LOG_ESM);

        m_items.clear();
        m_items = getItems();
    };

    T *itemByTag(const QString _tag) const
    {
        qCDebug(LOG_ESM);

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
        qCDebug(LOG_ESM);

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
        qCDebug(LOG_ESM);

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
        qCDebug(LOG_ESM);

        return m_items;
    };

    int uniqNumber() const
    {
        qCDebug(LOG_ESM);

        QList<int> tagList;
        foreach(T *item, m_items) tagList.append(item->number());
        int number = 0;
        while (tagList.contains(number)) number++;

        return number;
    };

private:
    QList<T *> m_items;
    QString m_type;

    // init method
    QList<T *> getItems()
    {
        qCDebug(LOG_ESM);

        // create directory at $HOME
        QString localDir = QString("%1/awesomewidgets/%2")
                           .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation))
                           .arg(m_type);
        QDir localDirectory;
        if (localDirectory.mkpath(localDir))
            qCDebug(LOG_ESM) << "Created directory" << localDir;

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
                qCDebug(LOG_ESM) << "Found file" << file << "in" << dir;
                names.append(file);
                items.append(new T(this, file, dirs, LOG_ESM().isDebugEnabled()));
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
        qCDebug(LOG_ESM);

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
        qCDebug(LOG_ESM);

        QString fileName = getName();
        int number = uniqNumber();
        QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                           QString("awesomewidgets/%1").arg(m_type),
                           QStandardPaths::LocateDirectory);
        if (fileName.isEmpty()) return;

        T *newItem = new T(this, fileName, dirs, LOG_ESM().isDebugEnabled());
        newItem->setNumber(number);
        if (newItem->showConfiguration(configArgs()) == 1) {
            initItems();
            repaint();
        }
    };

    void deleteItem()
    {
        qCDebug(LOG_ESM);

        T *source = itemFromWidget();
        if (source == nullptr) return;

        if (source->tryDelete()) {
            initItems();
            repaint();
        }
    };

    void editItem()
    {
        qCDebug(LOG_ESM);

        T *source = itemFromWidget();
        if (source == nullptr) return;

        if (source->showConfiguration(configArgs()) == 1) {
            initItems();
            repaint();
        }
    };
};


#endif /* EXTITEMAGGREGATOR_H */
