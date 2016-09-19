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

#include "awformatterconfig.h"
#include "ui_awformatterconfig.h"

#include <KI18n/KLocalizedString>

#include <QPushButton>

#include "awabstractselector.h"
#include "awdebug.h"
#include "awformatterhelper.h"


AWFormatterConfig::AWFormatterConfig(QWidget *parent, const QStringList keys)
    : QDialog(parent)
    , ui(new Ui::AWFormatterConfig)
    , m_keys(keys)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    ui->setupUi(this);
    m_editButton
        = ui->buttonBox->addButton(i18n("Edit"), QDialogButtonBox::ActionRole);
    init();

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_editButton, SIGNAL(clicked(bool)), this, SLOT(editFormatters()));
}


AWFormatterConfig::~AWFormatterConfig()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    clearSelectors();

    delete m_helper;
    delete ui;
}


void AWFormatterConfig::showDialog()
{
    // update dialog
    updateDialog();
    // exec dialog
    return execDialog();
}


void AWFormatterConfig::editFormatters()
{
    m_helper->editItems();
    updateDialog();
}


void AWFormatterConfig::updateUi()
{
    QPair<QString, QString> current
        = static_cast<AWAbstractSelector *>(sender())->current();
    int index
        = m_selectors.indexOf(static_cast<AWAbstractSelector *>(sender()));

    if ((current.first.isEmpty()) && (current.second.isEmpty())) {
        if (sender() == m_selectors.last())
            return;
        AWAbstractSelector *selector = m_selectors.takeAt(index);
        ui->verticalLayout->removeWidget(selector);
        selector->deleteLater();
    } else {
        if (sender() != m_selectors.last())
            return;
        auto keys = initKeys();
        addSelector(keys.first, keys.second, QPair<QString, QString>());
    }
}


void AWFormatterConfig::addSelector(const QStringList &keys,
                                    const QStringList &values,
                                    const QPair<QString, QString> &current)
{
    qCDebug(LOG_AW) << "Add selector with keys" << keys << "values" << values
                    << "and current ones" << current;

    AWAbstractSelector *selector
        = new AWAbstractSelector(ui->scrollAreaWidgetContents);
    selector->init(keys, values, current);
    ui->verticalLayout->insertWidget(ui->verticalLayout->count() - 1, selector);
    connect(selector, SIGNAL(selectionChanged()), this, SLOT(updateUi()));
    m_selectors.append(selector);
}


void AWFormatterConfig::clearSelectors()
{
    for (auto selector : m_selectors) {
        disconnect(selector, SIGNAL(selectionChanged()), this,
                   SLOT(updateUi()));
        ui->verticalLayout->removeWidget(selector);
        selector->deleteLater();
    }
    m_selectors.clear();
}


void AWFormatterConfig::execDialog()
{
    int ret = exec();
    QHash<QString, QString> data;
    for (auto selector : m_selectors) {
        QPair<QString, QString> select = selector->current();
        if (select.first.isEmpty())
            continue;
        data[select.first] = select.second;
    }

    // save configuration if required
    switch (ret) {
    case 0:
        break;
    case 1:
    default:
        m_helper->writeFormatters(data);
        m_helper->removeUnusedFormatters(data.keys());
        break;
    }
}


void AWFormatterConfig::init()
{
    delete m_helper;
    m_helper = new AWFormatterHelper(this);
}


QPair<QStringList, QStringList> AWFormatterConfig::initKeys() const
{
    // we are adding empty string at the start
    QStringList keys = QStringList() << QString("");
    keys.append(m_keys);
    keys.sort();
    QStringList knownFormatters = QStringList() << QString("");
    knownFormatters.append(m_helper->knownFormatters());
    knownFormatters.sort();

    return QPair<QStringList, QStringList>(keys, knownFormatters);
}


void AWFormatterConfig::updateDialog()
{
    clearSelectors();
    QHash<QString, QString> appliedFormatters = m_helper->getFormatters();
    auto keys = initKeys();

    for (auto key : appliedFormatters.keys())
        addSelector(keys.first, keys.second,
                    QPair<QString, QString>(key, appliedFormatters[key]));
    // empty one
    addSelector(keys.first, keys.second, QPair<QString, QString>());
}
