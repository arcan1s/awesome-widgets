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


AbstractExtItemAggregator::AbstractExtItemAggregator(QWidget *parent, const bool debugCmd)
    : QWidget(parent)
{
    // logging
    const_cast<QLoggingCategory &>(LOG_ESM()).setEnabled(QtMsgType::QtDebugMsg, debugCmd);
    qSetMessagePattern(LOG_FORMAT);

    dialog = new QDialog(this);
    widgetDialog = new QListWidget(dialog);
    dialogButtons = new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Close,
                                         Qt::Vertical, dialog);
    copyButton = dialogButtons->addButton(i18n("Copy"), QDialogButtonBox::ActionRole);
    createButton = dialogButtons->addButton(i18n("Create"), QDialogButtonBox::ActionRole);
    deleteButton = dialogButtons->addButton(i18n("Remove"), QDialogButtonBox::ActionRole);
    QHBoxLayout *layout = new QHBoxLayout(dialog);
    layout->addWidget(widgetDialog);
    layout->addWidget(dialogButtons);
    dialog->setLayout(layout);

    connect(dialogButtons, SIGNAL(clicked(QAbstractButton *)),
            this, SLOT(editItemButtonPressed(QAbstractButton *)));
    connect(dialogButtons, SIGNAL(rejected()), dialog, SLOT(reject()));
    connect(widgetDialog, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(editItemActivated(QListWidgetItem *)));
}


AbstractExtItemAggregator::~AbstractExtItemAggregator()
{
    qCDebug(LOG_ESM);

    delete dialog;
}


QString AbstractExtItemAggregator::getName()
{
    qCDebug(LOG_ESM);

    bool ok;
    QString name = QInputDialog::getText(this, i18n("Enter file name"),
                                         i18n("File name"), QLineEdit::Normal,
                                         QString(""), &ok);
    if ((!ok) || (name.isEmpty())) return QString("");
    if (!name.endsWith(QString(".desktop"))) name += QString(".desktop");

    return name;
}


QVariant AbstractExtItemAggregator::configArgs() const
{
    qCDebug(LOG_ESM);

    return m_configArgs;
}


void AbstractExtItemAggregator::setConfigArgs(const QVariant _configArgs)
{
    qCDebug(LOG_ESM);
    qCDebug(LOG_ESM) << "Configuration arguments" << _configArgs;

    m_configArgs = _configArgs;
}


void AbstractExtItemAggregator::editItemActivated(QListWidgetItem *item)
{
    Q_UNUSED(item)
    qCDebug(LOG_ESM);

    return editItem();
}


void AbstractExtItemAggregator::editItemButtonPressed(QAbstractButton *button)
{
    qCDebug(LOG_ESM);

    if (static_cast<QPushButton *>(button) == copyButton)
        return copyItem();
    else if (static_cast<QPushButton *>(button) == createButton)
        return createItem();
    else if (static_cast<QPushButton *>(button) == deleteButton)
        return deleteItem();
    else if (dialogButtons->buttonRole(button) == QDialogButtonBox::AcceptRole)
        return editItem();
}
