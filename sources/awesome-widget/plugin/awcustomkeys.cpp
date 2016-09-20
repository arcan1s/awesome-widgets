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

#include "awcustomkeys.h"
#include "ui_awcustomkeys.h"

#include <QSet>
#include <QSettings>
#include <QStandardPaths>

#include "awabstractselector.h"
#include "awdebug.h"


AWCustomKeys::AWCustomKeys(QWidget *parent, const QStringList sources)
    : QDialog(parent)
    , ui(new Ui::AWCustomKeys)
    , m_sources(sources)
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    m_filePath = QString("awesomewidgets/custom.ini");
    ui->setupUi(this);
    initKeys();

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


AWCustomKeys::~AWCustomKeys()
{
    qCDebug(LOG_AW) << __PRETTY_FUNCTION__;

    clearSelectors();
    delete ui;
}


QStringList AWCustomKeys::customKeys() const
{
    return m_keys.keys();
}


void AWCustomKeys::showDialog()
{
    // update dialog
    updateDialog();
    // exec dialog
    return execDialog();
}


QVariant AWCustomKeys::valueForKey(const QString &key, const QVariantHash &data)
{
    qCDebug(LOG_AW) << "Find value for key" << key << "in data";

    return data[m_keys[key]];
}


QStringList AWCustomKeys::usedSources() const
{
    return QSet<QString>::fromList(m_keys.values()).toList();
}


void AWCustomKeys::updateUi()
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
        addSelector(QPair<QString, QString>());
    }
}


void AWCustomKeys::addSelector(const QPair<QString, QString> &current)
{
    qCDebug(LOG_AW) << "Add selector with current values" << current;

    AWAbstractSelector *selector
        = new AWAbstractSelector(ui->scrollAreaWidgetContents, {true, false});
    selector->init(QStringList(), m_sources, current);
    ui->verticalLayout->insertWidget(ui->verticalLayout->count() - 1, selector);
    connect(selector, SIGNAL(selectionChanged()), this, SLOT(updateUi()));
    m_selectors.append(selector);
}


void AWCustomKeys::clearSelectors()
{
    for (auto selector : m_selectors) {
        disconnect(selector, SIGNAL(selectionChanged()), this,
                   SLOT(updateUi()));
        ui->verticalLayout->removeWidget(selector);
        selector->deleteLater();
    }
    m_selectors.clear();
}


void AWCustomKeys::execDialog()
{
    int ret = exec();
    m_keys.clear();
    for (auto selector : m_selectors) {
        QPair<QString, QString> select = selector->current();
        if (select.first.isEmpty())
            continue;
        m_keys[select.first] = select.second;
    }

    // save configuration if required
    switch (ret) {
    case 0:
        break;
    case 1:
    default:
        writeKeys();
        break;
    }
}


void AWCustomKeys::initKeys()
{
    m_keys.clear();

    QStringList configs = QStandardPaths::locateAll(
        QStandardPaths::GenericDataLocation, m_filePath);

    for (auto fileName : configs) {
        QSettings settings(fileName, QSettings::IniFormat);
        qCInfo(LOG_AW) << "Configuration file" << settings.fileName();

        settings.beginGroup(QString("Custom"));
        QStringList keys = settings.childKeys();
        for (auto key : keys) {
            QString source = settings.value(key).toString();
            qCInfo(LOG_AW) << "Found custom key" << key << "for source"
                           << source << "in" << settings.fileName();
            if (source.isEmpty()) {
                qCInfo(LOG_AW) << "Skip empty source for" << key;
                continue;
            }
            m_keys[key] = source;
        }
        settings.endGroup();
    }
}


void AWCustomKeys::updateDialog()
{
    clearSelectors();

    for (auto key : m_keys.keys())
        addSelector(QPair<QString, QString>(key, m_keys[key]));
    // empty one
    addSelector(QPair<QString, QString>());
}


bool AWCustomKeys::writeKeys() const
{
    QString fileName = QString("%1/%2")
                           .arg(QStandardPaths::writableLocation(
                               QStandardPaths::GenericDataLocation))
                           .arg(m_filePath);
    QSettings settings(fileName, QSettings::IniFormat);
    qCInfo(LOG_AW) << "Configuration file" << fileName;

    settings.beginGroup(QString("Custom"));
    for (auto key : m_keys.keys())
        settings.setValue(key, m_keys[key]);
    settings.endGroup();

    settings.sync();

    return (settings.status() == QSettings::NoError);
}
