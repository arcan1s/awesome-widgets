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
#include "ui_abstractextitemaggregator.h"

#include <KI18n/KLocalizedString>

#include <QDir>
#include <QInputDialog>
#include <QPushButton>


AbstractExtItemAggregator::AbstractExtItemAggregator(QWidget *_parent, const QString &_type)
    : QDialog(_parent)
    , ui(new Ui::AbstractExtItemAggregator)
    , m_type(_type)
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    // create directory at $HOME
    QString localDir
        = QString("%1/awesomewidgets/%2")
              .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation))
              .arg(type());
    QDir localDirectory;
    if (localDirectory.mkpath(localDir))
        qCInfo(LOG_LIB) << "Created directory" << localDir;

    ui->setupUi(this);
    copyButton = ui->buttonBox->addButton(i18n("Copy"), QDialogButtonBox::ActionRole);
    createButton = ui->buttonBox->addButton(i18n("Create"), QDialogButtonBox::ActionRole);
    deleteButton = ui->buttonBox->addButton(i18n("Remove"), QDialogButtonBox::ActionRole);

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton *)), this,
            SLOT(editItemButtonPressed(QAbstractButton *)));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->listWidget, SIGNAL(itemActivated(QListWidgetItem *)), this,
            SLOT(editItemActivated(QListWidgetItem *)));
}


AbstractExtItemAggregator::~AbstractExtItemAggregator()
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    delete ui;
}


void AbstractExtItemAggregator::copyItem()
{
    AbstractExtItem *source = itemFromWidget();
    QString fileName = getName();
    int number = uniqNumber();
    QString dir = QString("%1/awesomewidgets/%2")
                      .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation))
                      .arg(m_type);
    if ((!source) || (fileName.isEmpty())) {
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
    if (!source) {
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
    if (!source) {
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
    QString name = QInputDialog::getText(this, i18n("Enter file name"), i18n("File name"),
                                         QLineEdit::Normal, "", &ok);
    if ((!ok) || (name.isEmpty()))
        return "";
    if (!name.endsWith(".desktop"))
        name += ".desktop";

    return name;
}


AbstractExtItem *AbstractExtItemAggregator::itemFromWidget()
{
    QListWidgetItem *widgetItem = ui->listWidget->currentItem();
    if (!widgetItem)
        return nullptr;

    AbstractExtItem *found = nullptr;
    for (auto &item : items()) {
        QString fileName = QFileInfo(item->fileName()).fileName();
        if (fileName != widgetItem->text())
            continue;
        found = item;
        break;
    }
    if (!found)
        qCWarning(LOG_LIB) << "Could not find item by name" << widgetItem->text();

    return found;
}


void AbstractExtItemAggregator::repaintList()
{
    ui->listWidget->clear();
    for (auto &_item : items()) {
        QString fileName = QFileInfo(_item->fileName()).fileName();
        QListWidgetItem *item = new QListWidgetItem(fileName, ui->listWidget);
        QStringList tooltip;
        tooltip.append(i18n("Name: %1", _item->name()));
        tooltip.append(i18n("Comment: %1", _item->comment()));
        tooltip.append(i18n("Identity: %1", _item->uniq()));
        item->setToolTip(tooltip.join('\n'));
        ui->listWidget->addItem(item);
    }
}


int AbstractExtItemAggregator::uniqNumber() const
{
    QList<int> tagList;
    for (auto &item : items())
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


QStringList AbstractExtItemAggregator::directories() const
{
    auto dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                          QString("awesomewidgets/%1").arg(type()),
                                          QStandardPaths::LocateDirectory);

    return dirs;
}


QString AbstractExtItemAggregator::type() const
{
    return m_type;
}


void AbstractExtItemAggregator::setConfigArgs(const QVariant &_configArgs)
{
    qCDebug(LOG_LIB) << "Configuration arguments" << _configArgs;

    m_configArgs = _configArgs;
}


void AbstractExtItemAggregator::editItemActivated(QListWidgetItem *)
{
    return editItem();
}


void AbstractExtItemAggregator::editItemButtonPressed(QAbstractButton *_button)
{
    if (static_cast<QPushButton *>(_button) == copyButton)
        return copyItem();
    else if (static_cast<QPushButton *>(_button) == createButton)
        return doCreateItem();
    else if (static_cast<QPushButton *>(_button) == deleteButton)
        return deleteItem();
    else if (ui->buttonBox->buttonRole(_button) == QDialogButtonBox::AcceptRole)
        return editItem();
}
