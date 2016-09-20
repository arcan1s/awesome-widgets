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


#ifndef AWCUSTOMKEYS_H
#define AWCUSTOMKEYS_H

#include <QDialog>


class AWAbstractSelector;
namespace Ui
{
class AWCustomKeys;
}

class AWCustomKeys : public QDialog
{
    Q_OBJECT

public:
    explicit AWCustomKeys(QWidget *parent = nullptr,
                          const QStringList sources = QStringList());
    virtual ~AWCustomKeys();
    Q_INVOKABLE QStringList customKeys() const;
    Q_INVOKABLE void showDialog();
    QVariant valueForKey(const QString &key, const QVariantHash &data);
    Q_INVOKABLE QStringList usedSources() const;

private slots:
    void updateUi();

private:
    Ui::AWCustomKeys *ui = nullptr;
    QList<AWAbstractSelector *> m_selectors;
    // properties
    QString m_filePath;
    QHash<QString, QString> m_keys;
    QStringList m_sources;
    // methods
    void addSelector(const QPair<QString, QString> &current);
    void clearSelectors();
    void execDialog();
    void initKeys();
    void updateDialog();
    bool writeKeys() const;
};


#endif /* AWCUSTOMKEYS_H */
