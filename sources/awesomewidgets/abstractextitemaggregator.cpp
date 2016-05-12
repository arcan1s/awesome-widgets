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

#include "abstractextitemaggregator.h"

#include <KI18n/KLocalizedString>

#include <QFileInfo>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLineEdit>


AbstractExtItemAggregator::AbstractExtItemAggregator(QWidget *parent,
                                                     const QString type)
    : QWidget(parent)
    , m_type(type)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    dialog = new QDialog(this);
    widgetDialog = new QListWidget(dialog);
    dialogButtons = new QDialogButtonBox(
        QDialogButtonBox::Open | QDialogButtonBox::Close, Qt::Vertical, dialog);
    copyButton
        = dialogButtons->addButton(i18n("Copy"), QDialogButtonBox::ActionRole);
    createButton = dialogButtons->addButton(i18n("Create"),
                                            QDialogButtonBox::ActionRole);
    deleteButton = dialogButtons->addButton(i18n("Remove"),
                                            QDialogButtonBox::ActionRole);
    QHBoxLayout *layout = new QHBoxLayout(dialog);
    layout->addWidget(widgetDialog);
    layout->addWidget(dialogButtons);
    dialog->setLayout(layout);

    connect(dialogButtons, SIGNAL(clicked(QAbstractButton *)), this,
            SLOT(editItemButtonPressed(QAbstractButton *)));
    connect(dialogButtons, SIGNAL(rejected()), dialog, SLOT(reject()));
    connect(widgetDialog, SIGNAL(itemActivated(QListWidgetItem *)), this,
            SLOT(editItemActivated(QListWidgetItem *)));
}


AbstractExtItemAggregator::~AbstractExtItemAggregator()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    delete dialog;
}


void AbstractExtItemAggregator::copyItem()
{
    AbstractExtItem *source = itemFromWidget();
    QString fileName = getName();
    int number = uniqNumber();
    QString dir = QString("%1/awesomewidgets/%2")
                      .arg(QStandardPaths::writableLocation(
                          QStandardPaths::GenericDataLocation))
                      .arg(m_type);
    if ((source == nullptr) || (fileName.isEmpty())) {
        qCWarning(LOG_LIB) << "Nothing to copy";
        return;
    }
    QString filePath = QString("%1/%2").arg(dir).arg(fileName);

    AbstractExtItem *newItem = source->copy(filePath, number);
    if (newItem->showConfiguration(configArgs()) == 1) {
        initItems();
        repaintList();
    }
}


void AbstractExtItemAggregator::deleteItem()
{
    AbstractExtItem *source = itemFromWidget();
    if (source == nullptr) {
        qCWarning(LOG_LIB) << "Nothing to delete";
        return;
    };

    if (source->tryDelete()) {
        initItems();
        repaintList();
    }
}


void AbstractExtItemAggregator::editItem()
{
    AbstractExtItem *source = itemFromWidget();
    if (source == nullptr) {
        qCWarning(LOG_LIB) << "Nothing to edit";
        return;
    };

    if (source->showConfiguration(configArgs()) == 1) {
        initItems();
        repaintList();
    }
}


QString AbstractExtItemAggregator::getName()
{
    bool ok;
    QString name = QInputDialog::getText(this, i18n("Enter file name"),
                                         i18n("File name"), QLineEdit::Normal,
                                         QString(""), &ok);
    if ((!ok) || (name.isEmpty()))
        return QString("");
    if (!name.endsWith(QString(".desktop")))
        name += QString(".desktop");

    return name;
}


AbstractExtItem *AbstractExtItemAggregator::itemFromWidget()
{
    QListWidgetItem *widgetItem = widgetDialog->currentItem();
    if (widgetItem == nullptr)
        return nullptr;

    AbstractExtItem *found = nullptr;
    for (auto item : items()) {
        QString fileName = QFileInfo(item->fileName()).fileName();
        if (fileName != widgetItem->text())
            continue;
        found = item;
        break;
    }
    if (found == nullptr)
        qCWarning(LOG_LIB) << "Could not find item by name"
                           << widgetItem->text();

    return found;
}


void AbstractExtItemAggregator::repaintList()
{
    widgetDialog->clear();
    for (auto _item : items()) {
        QString fileName = QFileInfo(_item->fileName()).fileName();
        QListWidgetItem *item = new QListWidgetItem(fileName, widgetDialog);
        QStringList tooltip;
        tooltip.append(i18n("Name: %1", _item->name()));
        tooltip.append(i18n("Comment: %1", _item->comment()));
        tooltip.append(i18n("Identity: %1", _item->uniq()));
        item->setToolTip(tooltip.join(QChar('\n')));
        widgetDialog->addItem(item);
    }
}


int AbstractExtItemAggregator::uniqNumber() const
{
    QList<int> tagList;
    for (auto item : items())
        tagList.append(item->number());
    int number = 0;
    while (tagList.contains(number))
        number++;

    return number;
}


QVariant AbstractExtItemAggregator::configArgs() const
{
    return m_configArgs;
}


QString AbstractExtItemAggregator::type() const
{
    return m_type;
}


void AbstractExtItemAggregator::setConfigArgs(const QVariant _configArgs)
{
    qCDebug(LOG_LIB) << "Configuration arguments" << _configArgs;

    m_configArgs = _configArgs;
}


void AbstractExtItemAggregator::editItemActivated(QListWidgetItem *)
{
    return editItem();
}


void AbstractExtItemAggregator::editItemButtonPressed(QAbstractButton *button)
{
    if (static_cast<QPushButton *>(button) == copyButton)
        return copyItem();
    else if (static_cast<QPushButton *>(button) == createButton)
        return doCreateItem();
    else if (static_cast<QPushButton *>(button) == deleteButton)
        return deleteItem();
    else if (dialogButtons->buttonRole(button) == QDialogButtonBox::AcceptRole)
        return editItem();
}
