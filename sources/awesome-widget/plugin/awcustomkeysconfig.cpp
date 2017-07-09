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

#include "awcustomkeysconfig.h"
#include "ui_awcustomkeysconfig.h"

#include <KI18n/KLocalizedString>

#include <QPushButton>

#include "awabstractselector.h"
#include "awdebug.h"
#include "awcustomkeyshelper.h"


AWCustomKeysConfig::AWCustomKeysConfig(QWidget *_parent, const QStringList &_keys)
    : QDialog(_parent)
    , ui(new Ui::AWCustomKeysConfig)
    , m_keys(_keys)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    ui->setupUi(this);
    init();

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


AWCustomKeysConfig::~AWCustomKeysConfig()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    clearSelectors();

    delete m_helper;
    delete ui;
}


void AWCustomKeysConfig::showDialog()
{
    // update dialog
    updateDialog();
    // exec dialog
    return execDialog();
}


void AWCustomKeysConfig::updateUi()
{
    QPair<QString, QString> current
        = static_cast<AWAbstractSelector *>(sender())->current();
    int index
        = m_selectors.indexOf(static_cast<AWAbstractSelector *>(sender()));

    if ((current.first.isEmpty()) && (current.second.isEmpty())) {
        // remove current selector if it is empty and does not last
        if (sender() == m_selectors.last())
            return;
        AWAbstractSelector *selector = m_selectors.takeAt(index);
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


void AWCustomKeysConfig::addSelector(const QStringList &_keys,
                                    const QStringList &_values,
                                    const QPair<QString, QString> &_current)
{
    qCDebug(LOG_AW) << "Add selector with keys" << _keys << "values" << _values
                    << "and current ones" << _current;

    AWAbstractSelector *selector
        = new AWAbstractSelector(ui->scrollAreaWidgetContents, {true, false});
    selector->init(_keys, _values, _current);
    ui->verticalLayout->insertWidget(ui->verticalLayout->count() - 1, selector);
    connect(selector, SIGNAL(selectionChanged()), this, SLOT(updateUi()));
    m_selectors.append(selector);
}


void AWCustomKeysConfig::clearSelectors()
{
    for (auto &selector : m_selectors) {
        disconnect(selector, SIGNAL(selectionChanged()), this,
                   SLOT(updateUi()));
        ui->verticalLayout->removeWidget(selector);
        selector->deleteLater();
    }
    m_selectors.clear();
}


void AWCustomKeysConfig::execDialog()
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
    switch (ret) {
    case 0:
        break;
    case 1:
    default:
        m_helper->writeItems(data);
        m_helper->removeUnusedKeys(data.keys());
        break;
    }
}


void AWCustomKeysConfig::init()
{
    delete m_helper;
    m_helper = new AWCustomKeysHelper(this);
}


QPair<QStringList, QStringList> AWCustomKeysConfig::initKeys() const
{
    // we are adding empty string at the start
    QStringList keys = QStringList() << "";
    keys.append(m_keys);
    keys.sort();
    QStringList userKeys = QStringList() << "";
    userKeys.append(m_helper->keys());
    userKeys.sort();

    return QPair<QStringList, QStringList>(userKeys, keys);
}


void AWCustomKeysConfig::updateDialog()
{
    clearSelectors();
    auto userKeys = m_helper->getUserKeys();
    auto keys = initKeys();

    for (auto &key : userKeys.keys())
        addSelector(keys.first, keys.second,
                    QPair<QString, QString>(key, userKeys[key]));
    // empty one
    addSelector(keys.first, keys.second, QPair<QString, QString>());
}
