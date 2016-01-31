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

#include <QHBoxLayout>
#include <QInputDialog>
#include <QLineEdit>

#include "awdebug.h"


AbstractExtItemAggregator::AbstractExtItemAggregator(QWidget *parent)
    : QWidget(parent)
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


QVariant AbstractExtItemAggregator::configArgs() const
{
    return m_configArgs;
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
        return createItem();
    else if (static_cast<QPushButton *>(button) == deleteButton)
        return deleteItem();
    else if (dialogButtons->buttonRole(button) == QDialogButtonBox::AcceptRole)
        return editItem();
}
