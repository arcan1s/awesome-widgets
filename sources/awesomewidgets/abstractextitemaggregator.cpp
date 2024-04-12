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
#include <utility>


AbstractExtItemAggregator::AbstractExtItemAggregator(QObject *_parent, QString _type)
    : QObject(_parent)
    , m_type(std::move(_type))
{
    qCDebug(LOG_LIB) << __PRETTY_FUNCTION__;

    // create directory at $HOME
    auto localDir = QString("%1/awesomewidgets/%2")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation), type());
    if (QDir().mkpath(localDir))
        qCInfo(LOG_LIB) << "Created directory" << localDir;
}


void AbstractExtItemAggregator::copyItem(QListWidget *_widget)
{
    auto source = itemFromWidget(_widget);
    auto fileName = getName();
    auto number = uniqNumber();
    auto dir = QString("%1/awesomewidgets/%2")
                   .arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation), m_type);
    if ((!source) || (fileName.isEmpty())) {
        qCWarning(LOG_LIB) << "Nothing to copy";
        return;
    }
    auto filePath = QString("%1/%2").arg(dir, fileName);

    auto newItem = source->copy(filePath, number);
    if (newItem->showConfiguration(nullptr, configArgs()) == 1) {
        initItems();
        repaintList(_widget);
    }
}


void AbstractExtItemAggregator::deleteItem(QListWidget *_widget)
{
    auto source = itemFromWidget(_widget);
    if (!source) {
        qCWarning(LOG_LIB) << "Nothing to delete";
        return;
    }

    if (source->tryDelete()) {
        initItems();
        repaintList(_widget);
    }
}


void AbstractExtItemAggregator::editItem(QListWidget *_widget)
{
    auto source = itemFromWidget(_widget);
    if (!source) {
        qCWarning(LOG_LIB) << "Nothing to edit";
        return;
    }

    if (source->showConfiguration(nullptr, configArgs()) == 1) {
        initItems();
        repaintList(_widget);
    }
}


int AbstractExtItemAggregator::exec()
{
    auto dialog = new QDialog();
    auto ui = new Ui::AbstractExtItemAggregator();
    ui->setupUi(dialog);

    auto copyButton = ui->buttonBox->addButton(i18n("Copy"), QDialogButtonBox::ActionRole);
    auto createButton = ui->buttonBox->addButton(i18n("Create"), QDialogButtonBox::ActionRole);
    auto deleteButton = ui->buttonBox->addButton(i18n("Remove"), QDialogButtonBox::ActionRole);

    connect(copyButton, &QPushButton::clicked, [this, ui]() { copyItem(ui->listWidget); });
    connect(createButton, &QPushButton::clicked, [this, ui]() { doCreateItem(ui->listWidget); });
    connect(deleteButton, &QPushButton::clicked, [this, ui]() { deleteItem(ui->listWidget); });
    connect(ui->buttonBox, &QDialogButtonBox::accepted, [this, ui]() { editItem(ui->listWidget); });

    connect(ui->buttonBox, &QDialogButtonBox::rejected, [dialog]() { dialog->reject(); });
    connect(ui->listWidget, &QListWidget::itemActivated, [this, ui](QListWidgetItem *) { editItem(ui->listWidget); });

    repaintList(ui->listWidget);
    auto ret = dialog->exec();

    dialog->deleteLater();
    delete ui;

    return ret;
}


QString AbstractExtItemAggregator::getName()
{
    bool ok;
    auto name = QInputDialog::getText(nullptr, i18n("Enter file name"), i18n("File name"), QLineEdit::Normal, "", &ok);
    if ((!ok) || (name.isEmpty()))
        return "";
    if (!name.endsWith(".desktop"))
        name += ".desktop";

    return name;
}


AbstractExtItem *AbstractExtItemAggregator::itemFromWidget(QListWidget *_widget) const
{
    auto widgetItem = _widget->currentItem();
    if (!widgetItem)
        return nullptr;

    AbstractExtItem *found = nullptr;
    for (auto item : items()) {
        auto fileName = item->fileName();
        if (fileName != widgetItem->text())
            continue;
        found = item;
        break;
    }
    if (!found)
        qCWarning(LOG_LIB) << "Could not find item by name" << widgetItem->text();

    return found;
}


void AbstractExtItemAggregator::repaintList(QListWidget *_widget) const
{
    _widget->clear();
    for (auto _item : items()) {
        QString fileName = _item->fileName();
        auto item = new QListWidgetItem(fileName, _widget);
        QStringList tooltip;
        tooltip.append(i18n("Name: %1", _item->name()));
        tooltip.append(i18n("Comment: %1", _item->comment()));
        tooltip.append(i18n("Identity: %1", _item->uniq()));
        item->setToolTip(tooltip.join('\n'));
        _widget->addItem(item);
    }
}


int AbstractExtItemAggregator::uniqNumber() const
{
    QList<int> tagList;
    for (auto item : items())
        tagList.append(item->number());

    auto number = 0;
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
    return QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString("awesomewidgets/%1").arg(type()),
                                     QStandardPaths::LocateDirectory);
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
