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

#include "awabstractpairconfig.h"
#include "ui_awabstractpairconfig.h"

#include <KI18n/KLocalizedString>

#include <QPushButton>
#include <utility>

#include "awabstractselector.h"
#include "awdebug.h"


AWAbstractPairConfig::AWAbstractPairConfig(QWidget *_parent, const bool _hasEdit, QStringList _keys)
    : QDialog(_parent)
    , ui(new Ui::AWAbstractPairConfig)
    , m_hasEdit(_hasEdit)
    , m_keys(std::move(_keys))
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AWAbstractPairConfig::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AWAbstractPairConfig::reject);

    // edit feature
    if (m_hasEdit) {
        m_editButton = ui->buttonBox->addButton(i18n("Edit"), QDialogButtonBox::ActionRole);
        connect(m_editButton, &QPushButton::clicked, [this]() { return edit(); });
    }
}


AWAbstractPairConfig::~AWAbstractPairConfig()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    clearSelectors();

    delete ui;
}


void AWAbstractPairConfig::showDialog()
{
    // update dialog
    updateDialog();
    // exec dialog
    return execDialog();
}


void AWAbstractPairConfig::setEditable(const bool _first, const bool _second)
{
    qCDebug(LOG_AW) << "Set editable" << _first << _second;

    m_editable = {_first, _second};
}


void AWAbstractPairConfig::edit()
{
    m_helper->editPairs();
    updateDialog();
}


void AWAbstractPairConfig::updateUi()
{
    auto current = dynamic_cast<AWAbstractSelector *>(sender())->current();
    auto index = m_selectors.indexOf(dynamic_cast<AWAbstractSelector *>(sender()));

    if ((current.first.isEmpty()) && (current.second.isEmpty())) {
        // remove current selector if it is empty and does not last
        if (sender() == m_selectors.last())
            return;
        auto selector = m_selectors.takeAt(index);
        ui->verticalLayout->removeWidget(selector);
        selector->deleteLater();
    } else {
        // add new selector if something changed
        if (sender() != m_selectors.last())
            return;
        auto keys = initKeys();
        addSelector(keys.first, keys.second, QPair<QString, QString>());
    }
}


void AWAbstractPairConfig::addSelector(const QStringList &_keys, const QStringList &_values,
                                       const QPair<QString, QString> &_current)
{
    qCDebug(LOG_AW) << "Add selector with keys" << _keys << "values" << _values << "and current ones" << _current;

    auto selector = new AWAbstractSelector(ui->scrollAreaWidgetContents, m_editable);
    selector->init(_keys, _values, _current);
    ui->verticalLayout->insertWidget(ui->verticalLayout->count() - 1, selector);
    connect(selector, &AWAbstractSelector::selectionChanged, this, &AWAbstractPairConfig::updateUi);
    m_selectors.append(selector);
}


void AWAbstractPairConfig::clearSelectors()
{
    for (auto &selector : m_selectors) {
        disconnect(selector, &AWAbstractSelector::selectionChanged, this, &AWAbstractPairConfig::updateUi);
        ui->verticalLayout->removeWidget(selector);
        selector->deleteLater();
    }
    m_selectors.clear();
}


void AWAbstractPairConfig::execDialog()
{
    int ret = exec();
    QHash<QString, QString> data;
    for (auto &selector : m_selectors) {
        QPair<QString, QString> select = selector->current();
        if (select.first.isEmpty())
            continue;
        data[select.first] = select.second;
    }

    // save configuration if required
    auto status = true;
    switch (ret) {
    case 0:
        break;
    case 1:
    default:
        status &= m_helper->writeItems(data);
        status &= m_helper->removeUnusedKeys(data.keys());
        break;
    }
    qCDebug(LOG_AW) << "Configuration save status" << status;
}


QPair<QStringList, QStringList> AWAbstractPairConfig::initKeys() const
{
    // we are adding empty string at the start
    QStringList left = {""};
    left.append(m_helper->leftKeys().isEmpty() ? m_keys : m_helper->leftKeys());
    left.sort();
    QStringList right = {""};
    right.append(m_helper->rightKeys().isEmpty() ? m_keys : m_helper->rightKeys());
    right.sort();

    return {left, right};
}


void AWAbstractPairConfig::updateDialog()
{
    clearSelectors();
    auto pairs = m_helper->pairs();
    auto keys = initKeys();

    for (auto &key : m_helper->keys())
        addSelector(keys.first, keys.second, QPair<QString, QString>(key, pairs[key]));
    // empty one
    addSelector(keys.first, keys.second, QPair<QString, QString>());
}
