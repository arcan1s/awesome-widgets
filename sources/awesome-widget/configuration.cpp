/***************************************************************************
 *   This file is part of pytextmonitor                                    *
 *                                                                         *
 *   pytextmonitor is free software: you can redistribute it and/or        *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   pytextmonitor is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with pytextmonitor. If not, see http://www.gnu.org/licenses/    *
 ***************************************************************************/

#include "awesome-widget.h"

#include <KConfigDialog>
#include <KGlobal>
#include <KStandardDirs>
#include <QNetworkInterface>
#include <QTextCodec>

#include <pdebug/pdebug.h>
#include <task/taskadds.h>


QMap<QString, QString> AwesomeWidget::readDataEngineConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> rawConfig;
    rawConfig[QString("CUSTOM")] = QString("wget -qO- http://ifconfig.me/ip");
    rawConfig[QString("DESKTOPCMD")] = QString("qdbus org.kde.kwin /KWin currentDesktop");
    rawConfig[QString("GPUDEV")] = QString("auto");
    rawConfig[QString("HDDDEV")] = QString("all");
    rawConfig[QString("HDDTEMPCMD")] = QString("sudo hddtemp");
    rawConfig[QString("MPDADDRESS")] = QString("localhost");
    rawConfig[QString("MPDPORT")] = QString("6600");
    rawConfig[QString("MPRIS")] = QString("auto");
    rawConfig[QString("PKGCMD")] = QString("pacman -Qu");
    rawConfig[QString("PKGNULL")] = QString("0");
    rawConfig[QString("PLAYER")] = QString("mpris");

    QString fileName = KGlobal::dirs()->findResource("config", "ext-sysmon.conf");
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QFile configFile(fileName);
    if (!configFile.open(QIODevice::ReadOnly))
        return updateDataEngineConfiguration(rawConfig);
    QString fileStr;
    QStringList value;
    while (true) {
        fileStr = QString(configFile.readLine()).trimmed();
        if ((fileStr.isEmpty()) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar('#')) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar(';')) && (!configFile.atEnd())) continue;
        if (fileStr.contains(QChar('='))) {
            value.clear();
            for (int i=1; i<fileStr.split(QChar('=')).count(); i++)
                value.append(fileStr.split(QChar('='))[i]);
            rawConfig[fileStr.split(QChar('='))[0]] = value.join(QChar('='));
        }
        if (configFile.atEnd()) break;
    }
    configFile.close();

    return updateDataEngineConfiguration(rawConfig);
}


void AwesomeWidget::writeDataEngineConfiguration(const QMap<QString, QString> settings)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> config = updateDataEngineConfiguration(settings);
    QString fileName = KGlobal::dirs()->locateLocal("config", "ext-sysmon.conf");
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QFile configFile(fileName);
    if (!configFile.open(QIODevice::WriteOnly)) return;
    for (int i=0; i<config.keys().count(); i++) {
        QByteArray string = (config.keys()[i] + QString("=") + config[config.keys()[i]] + QString("\n")).toUtf8();
        configFile.write(string);
    }
    configFile.close();
}


QMap<QString, QString> AwesomeWidget::updateDataEngineConfiguration(const QMap<QString, QString> rawConfig)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> config;
    QString key, value;
    // remove spaces and copy source map
    for (int i=0; i<rawConfig.keys().count(); i++) {
        key = rawConfig.keys()[i];
        value = rawConfig[key];
        key.remove(QChar(' '));
        if ((key != QString("CUSTOM")) &&
                (key != QString("DESKTOPCMD")) &&
                (key != QString("HDDTEMPCMD")) &&
                (key != QString("PKGCMD")))
            value.remove(QChar(' '));
        config[key] = value;
    }
    // pkgcmd
    for (int i=config[QString("PKGNULL")].split(QString(","), QString::SkipEmptyParts).count();
         i<config[QString("PKGCMD")].split(QString(","), QString::SkipEmptyParts).count()+1;
         i++)
        config[QString("PKGNULL")] += QString(",0");

    for (int i=0; i<config.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << config.keys()[i] + QString("=") + config[config.keys()[i]];

    return config;
}


void AwesomeWidget::createConfigurationInterface(KConfigDialog *parent)
{
    if (debug) qDebug() << PDEBUG;
    QString cmd, qoutput;
    QStringList headerList;
    TaskResult process;

    QWidget *advWidget = new QWidget;
    uiAdvancedConfig.setupUi(advWidget);
    QWidget *appWidget = new QWidget;
    uiAppConfig.setupUi(appWidget);
    QWidget *configWidget = new QWidget;
    uiWidConfig.setupUi(configWidget);
    QWidget *deConfigWidget = new QWidget;
    uiDEConfig.setupUi(deConfigWidget);
    QWidget *tooltipWidget = new QWidget;
    uiTooltipConfig.setupUi(tooltipWidget);

    //widget
    uiWidConfig.textEdit_elements->setPlainText(configuration[QString("text")]);

    // advanced
    if (configuration[QString("background")].toInt() == 0)
        uiAdvancedConfig.checkBox_background->setCheckState(Qt::Unchecked);
    else
        uiAdvancedConfig.checkBox_background->setCheckState(Qt::Checked);
    if (configuration[QString("layout")].toInt() == 0)
        uiAdvancedConfig.checkBox_layout->setCheckState(Qt::Unchecked);
    else
        uiAdvancedConfig.checkBox_layout->setCheckState(Qt::Checked);
    if (configuration[QString("popup")].toInt() == 0)
        uiAdvancedConfig.checkBox_popup->setCheckState(Qt::Unchecked);
    else
        uiAdvancedConfig.checkBox_popup->setCheckState(Qt::Checked);
    if (configuration[QString("leftStretch")].toInt() == 0)
        uiAdvancedConfig.checkBox_leftStretch->setCheckState(Qt::Unchecked);
    else
        uiAdvancedConfig.checkBox_leftStretch->setCheckState(Qt::Checked);
    if (configuration[QString("rightStretch")].toInt() == 0)
        uiAdvancedConfig.checkBox_rightStretch->setCheckState(Qt::Unchecked);
    else
        uiAdvancedConfig.checkBox_rightStretch->setCheckState(Qt::Checked);
    uiAdvancedConfig.lineEdit_timeFormat->setText(configuration[QString("customTime")]);
    uiAdvancedConfig.lineEdit_uptimeFormat->setText(configuration[QString("customUptime")]);
    uiAdvancedConfig.comboBox_tempUnits->setCurrentIndex(
                uiAdvancedConfig.comboBox_tempUnits->findText(configuration[QString("tempUnits")],
                Qt::MatchFixedString));
    cmd = QString("sensors");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    uiAdvancedConfig.listWidget_tempDevice->clear();
    for (int i=0; i<qoutput.split(QString("\n\n")).count(); i++) {
        QString sensor = qoutput.split(QString("\n\n"))[i];
        for (int j=0; j<sensor.split(QChar('\n')).count(); j++) {
            QString device = sensor.split(QChar('\n'))[j];
            if (device.indexOf(QString("°C")) > -1) {
                QListWidgetItem item = QListWidgetItem(QString("lmsensors/") + sensor.split(QChar('\n'))[0] + QString("/") +
                        device.split(QChar(':'))[0].replace(QChar(' '), QChar('_')));
                item.setCheckState(Qt::Unchecked);
                uiAdvancedConfig.listWidget_tempDevice->addItem(&item);
            }
        }
    }
    for (int i=0; i<configuration[QString("tempDevice")].split(QString("@@")).count(); i++) {
        QList<QListWidgetItem *> items = uiAdvancedConfig.listWidget_tempDevice
                ->findItems(configuration[QString("tempDevice")].split(QString("@@"))[i], Qt::MatchFixedString);
        for (int j=0; j<items.count(); j++)
            items[j]->setCheckState(Qt::Checked);
    }
    cmd = QString("mount");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    uiAdvancedConfig.listWidget_mount->clear();
    for (int i=0; i<qoutput.split(QChar('\n')).count(); i++) {
        QString mountPoint = qoutput.split(QChar('\n'))[i].split(QString(" on "))[1].split(QString(" type "))[0];
        QListWidgetItem item = QListWidgetItem(mountPoint);
        item.setCheckState(Qt::Unchecked);
        uiAdvancedConfig.listWidget_mount->addItem(&item);
    }
    for (int i=0; i<configuration[QString("mount")].split(QString("@@")).count(); i++) {
        QList<QListWidgetItem *> items = uiAdvancedConfig.listWidget_mount
                ->findItems(configuration[QString("mount")].split(QString("@@"))[i], Qt::MatchFixedString);
        for (int j=0; j<items.count(); j++)
            items[j]->setCheckState(Qt::Checked);
    }
    uiAdvancedConfig.listWidget_hddSpeedDevice->clear();
    for (int i=0; i<diskDevices.count(); i++) {
        QListWidgetItem item = QListWidgetItem(diskDevices[i]);
        item.setCheckState(Qt::Unchecked);
        uiAdvancedConfig.listWidget_hddSpeedDevice->addItem(&item);
    }
    for (int i=0; i<configuration[QString("disk")].split(QString("@@")).count(); i++) {
        QList<QListWidgetItem *> items = uiAdvancedConfig.listWidget_hddSpeedDevice
                ->findItems(configuration[QString("disk")].split(QString("@@"))[i], Qt::MatchFixedString);
        for (int j=0; j<items.count(); j++)
            items[j]->setCheckState(Qt::Checked);
    }
    if (configuration[QString("useCustomNetdev")].toInt() == 0)
        uiAdvancedConfig.checkBox_netdev->setCheckState(Qt::Unchecked);
    else
        uiAdvancedConfig.checkBox_netdev->setCheckState(Qt::Checked);
    QList<QNetworkInterface> rawInterfaceList = QNetworkInterface::allInterfaces();
    QStringList interfaceList;
    for (int i=0; i<rawInterfaceList.count(); i++)
        interfaceList.append(rawInterfaceList[i].name());
    uiAdvancedConfig.comboBox_netdev->clear();
    uiAdvancedConfig.comboBox_netdev->addItems(interfaceList);
    uiAdvancedConfig.comboBox_netdev->setCurrentIndex(
                uiAdvancedConfig.comboBox_netdev->findText(configuration[QString("customNetdev")],
                Qt::MatchFixedString));
    uiAdvancedConfig.lineEdit_batdev->setText(configuration[QString("batteryDevice")]);
    uiAdvancedConfig.lineEdit_acdev->setText(configuration[QString("acDevice")]);
    uiAdvancedConfig.lineEdit_acOnline->setText(configuration[QString("acOnline")]);
    uiAdvancedConfig.lineEdit_acOffline->setText(configuration[QString("acOffile")]);

    // tooltip
    uiTooltipConfig.spinBox_tooltipNum->setValue(configuration[QString("tooltipNumber")].toInt());
    if (configuration[QString("useTooltipBackground")].toInt() == 0)
        uiAdvancedConfig.checkBox_background->setCheckState(Qt::Unchecked);
    else
        uiAdvancedConfig.checkBox_background->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_background->setColor(QColor(configuration[QString("tooltipBackground")]));
    if (configuration[QString("cpuTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_cpu->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_cpu->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_cpu->setColor(QColor(configuration[QString("cpuColor")]));
    if (configuration[QString("cpuclockTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_cpuclock->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_cpuclock->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_cpuclock->setColor(QColor(configuration[QString("cpuclockColor")]));
    if (configuration[QString("memTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_mem->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_mem->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_mem->setColor(QColor(configuration[QString("memColor")]));
    if (configuration[QString("swapTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_swap->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_swap->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_swap->setColor(QColor(configuration[QString("swapColor")]));
    if (configuration[QString("downTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_down->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_down->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_down->setColor(QColor(configuration[QString("downColor")]));
    if (configuration[QString("upTooltip")].toInt() == 0)
        uiTooltipConfig.checkBox_up->setCheckState(Qt::Unchecked);
    else
        uiTooltipConfig.checkBox_up->setCheckState(Qt::Checked);
    uiTooltipConfig.kcolorcombo_up->setColor(QColor(configuration[QString("upColor")]));

    // appearance
    KConfigGroup cg = config();
    QString fontFamily = cg.readEntry("fontFamily", "Terminus");
    int fontSize = cg.readEntry("fontSize", 10);
    QString fontColor = cg.readEntry("fontColor", "#000000");
    int fontWeight = cg.readEntry("fontWeight", 400);
    QString fontStyle = cg.readEntry("fontStyle", "normal");
    QFont font = QFont(fontFamily, 12, 400, false);
    uiAppConfig.spinBox_interval->setValue(configuration[QString("interval")].toInt());
    uiAppConfig.fontComboBox->setCurrentFont(font);
    uiAppConfig.spinBox_size->setValue(fontSize);
    uiAppConfig.kcolorcombo->setColor(fontColor);
    if (fontStyle == QString("normal"))
        uiAppConfig.comboBox_style->setCurrentIndex(0);
    else if (fontStyle == QString("italic"))
        uiAppConfig.comboBox_style->setCurrentIndex(1);
    uiAppConfig.spinBox_weight->setValue(fontWeight);

    // dataengine
    QMap<QString, QString> deSettings = readDataEngineConfiguration();
    uiDEConfig.tableWidget_customCommand->clear();
    uiDEConfig.tableWidget_customCommand->setRowCount(deSettings[QString("CUSTOM")].split(QString("@@")).count() + 1);
    headerList.clear();
    headerList.append(i18n("Custom command"));
    uiDEConfig.tableWidget_customCommand->setHorizontalHeaderLabels(headerList);
    uiDEConfig.tableWidget_customCommand->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    for (int i=0; i<deSettings[QString("CUSTOM")].split(QString("@@")).count(); i++)
        uiDEConfig.tableWidget_customCommand->setItem(i, 0, new QTableWidgetItem(deSettings[QString("CUSTOM")].split(QString("@@"))[i]));
    uiDEConfig.lineEdit_desktopCmd->setText(deSettings[QString("DESKTOPCMD")]);
    uiDEConfig.comboBox_gpudev->setCurrentIndex(
                uiDEConfig.comboBox_gpudev->findText(deSettings[QString("GPUDEV")], Qt::MatchFixedString));
    cmd = QString("find /dev -name '[hms]d[a-z]'");
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    qoutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    uiDEConfig.comboBox_hdddev->clear();
    uiDEConfig.comboBox_hdddev->addItem(QString("all"));
    uiDEConfig.comboBox_hdddev->addItem(QString("disable"));
    for (int i=0; i<qoutput.split(QChar('\n')).count(); i++)
        uiDEConfig.comboBox_hdddev->addItem(qoutput.split(QChar('\n'))[i]);
    uiDEConfig.comboBox_hdddev->setCurrentIndex(
                uiDEConfig.comboBox_hdddev->findText(deSettings[QString("HDDDEV")], Qt::MatchFixedString));
    uiDEConfig.lineEdit_hddtempCmd->setText(deSettings[QString("HDDTEMPCMD")]);
    uiDEConfig.lineEdit_mpdaddress->setText(deSettings[QString("MPDADDRESS")]);
    uiDEConfig.spinBox_mpdport->setValue(deSettings[QString("MPDPORT")].toInt());
    uiDEConfig.comboBox_mpris->addItem(deSettings[QString("MPRIS")]);
    uiDEConfig.comboBox_mpris->setCurrentIndex(uiDEConfig.comboBox_mpris->count() - 1);
    uiDEConfig.tableWidget_pkgCommand->clear();
    uiDEConfig.tableWidget_pkgCommand->setRowCount(deSettings[QString("PKGCMD")].split(QChar(',')).count() + 1);
    headerList.clear();
    headerList.append(i18n("Package manager"));
    headerList.append(i18n("Null lines"));
    uiDEConfig.tableWidget_pkgCommand->setHorizontalHeaderLabels(headerList);
    uiDEConfig.tableWidget_pkgCommand->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    for (int i=0; i<deSettings[QString("PKGCMD")].split(QChar(',')).count(); i++) {
        uiDEConfig.tableWidget_pkgCommand->setItem(i, 0, new QTableWidgetItem(deSettings[QString("PKGCMD")].split(QChar(','))[i]));
        uiDEConfig.tableWidget_pkgCommand->setItem(i, 1, new QTableWidgetItem(deSettings[QString("PKGNULL")].split(QChar(','))[i]));
    }
    uiDEConfig.comboBox_playerSelect->setCurrentIndex(
                uiDEConfig.comboBox_playerSelect->findText(deSettings[QString("PLAYER")], Qt::MatchFixedString));

    parent->addPage(configWidget, i18n("Widget"), Applet::icon());
    parent->addPage(advWidget, i18n("Advanced"), QString("system-run"));
    parent->addPage(tooltipWidget, i18n("Tooltip"), QString("preferences-desktop-color"));
    parent->addPage(appWidget, i18n("Appearance"), QString("preferences-desktop-theme"));
    parent->addPage(deConfigWidget, i18n("DataEngine"), QString("utilities-system-monitor"));

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
}


void AwesomeWidget::configAccepted()
{
    if (debug) qDebug() << PDEBUG;
    QStringList items;

    disconnectFromEngine();
    KConfigGroup cg = config();

    // widget
    cg.writeEntry("text", uiWidConfig.textEdit_elements->toPlainText());

    // advanced
    cg.writeEntry("background", QString::number(uiAdvancedConfig.checkBox_background->checkState()));
    cg.writeEntry("layout", QString::number(uiAdvancedConfig.checkBox_layout->checkState()));
    cg.writeEntry("popup", QString::number(uiAdvancedConfig.checkBox_popup->checkState()));
    cg.writeEntry("leftStretch", QString::number(uiAdvancedConfig.checkBox_leftStretch->checkState()));
    cg.writeEntry("rightStretch", QString::number(uiAdvancedConfig.checkBox_rightStretch->checkState()));
    cg.writeEntry("customTime", uiAdvancedConfig.lineEdit_timeFormat->text());
    cg.writeEntry("customUptime", uiAdvancedConfig.lineEdit_uptimeFormat->text());
    cg.writeEntry("tempUnits", uiAdvancedConfig.comboBox_tempUnits->currentText());
    items.clear();
    for (int i=0; i<uiAdvancedConfig.listWidget_tempDevice->count(); i++)
        if (uiAdvancedConfig.listWidget_tempDevice->item(i)->checkState() == Qt::Checked)
            items.append(uiAdvancedConfig.listWidget_tempDevice->item(i)->text());
    cg.writeEntry("tempDevice", items.join(QString("@@")));
    items.clear();
    for (int i=0; i<uiAdvancedConfig.listWidget_mount->count(); i++)
        if (uiAdvancedConfig.listWidget_mount->item(i)->checkState() == Qt::Checked)
            items.append(uiAdvancedConfig.listWidget_mount->item(i)->text());
    cg.writeEntry("mount", items.join(QString("@@")));
    items.clear();
    for (int i=0; i<uiAdvancedConfig.listWidget_hddSpeedDevice->count(); i++)
        if (uiAdvancedConfig.listWidget_hddSpeedDevice->item(i)->checkState() == Qt::Checked)
            items.append(uiAdvancedConfig.listWidget_hddSpeedDevice->item(i)->text());
    cg.writeEntry("disk", items.join(QString("@@")));
    cg.writeEntry("useCustomNetdev", QString::number(uiAdvancedConfig.checkBox_netdev->checkState()));
    cg.writeEntry("customNetdev", uiAdvancedConfig.comboBox_netdev->currentText());
    cg.writeEntry("batteryDevice", uiAdvancedConfig.lineEdit_batdev->text());
    cg.writeEntry("acDevice", uiAdvancedConfig.lineEdit_acdev->text());
    cg.writeEntry("acOnline", uiAdvancedConfig.lineEdit_acOnline->text());
    cg.writeEntry("acOffile", uiAdvancedConfig.lineEdit_acOffline->text());

    // tooltip
    cg.writeEntry("tooltipNumber", QString::number(uiTooltipConfig.spinBox_tooltipNum->value()));
    cg.writeEntry("useTooltipBackground", QString::number(uiTooltipConfig.checkBox_background->checkState()));
    cg.writeEntry("tooltipBackground", uiTooltipConfig.kcolorcombo_background->color().name());
    cg.writeEntry("cpuTooltip", QString::number(uiTooltipConfig.checkBox_cpu->checkState()));
    cg.writeEntry("cpuColor", uiTooltipConfig.kcolorcombo_cpu->color().name());
    cg.writeEntry("cpuclockTooltip", QString::number(uiTooltipConfig.checkBox_cpuclock->checkState()));
    cg.writeEntry("cpuclockColor", uiTooltipConfig.kcolorcombo_cpuclock->color().name());
    cg.writeEntry("memTooltip", QString::number(uiTooltipConfig.checkBox_mem->checkState()));
    cg.writeEntry("memColor", uiTooltipConfig.kcolorcombo_mem->color().name());
    cg.writeEntry("swapTooltip", QString::number(uiTooltipConfig.checkBox_swap->checkState()));
    cg.writeEntry("swapColor", uiTooltipConfig.kcolorcombo_swap->color().name());
    cg.writeEntry("downTooltip", QString::number(uiTooltipConfig.checkBox_down->checkState()));
    cg.writeEntry("downColor", uiTooltipConfig.kcolorcombo_down->color().name());
    cg.writeEntry("upTooltip", QString::number(uiTooltipConfig.checkBox_up->checkState()));
    cg.writeEntry("upColor", uiTooltipConfig.kcolorcombo_up->color().name());

    // appearance
    cg.writeEntry("interval", QString::number(uiAppConfig.spinBox_interval->value()));
    cg.writeEntry("fontFamily", uiAppConfig.fontComboBox->currentFont().family());
    cg.writeEntry("fontSize", QString::number(uiAppConfig.spinBox_size->value()));
    cg.writeEntry("fontColor", uiAppConfig.kcolorcombo->color().name());
    cg.writeEntry("fontStyle", uiAppConfig.comboBox_style->currentText());
    cg.writeEntry("fontWeight", QString::number(uiAppConfig.spinBox_weight->value()));

    // dataengine
    QMap<QString, QString> deSettings;
    items.clear();
    for (int i=0; i<uiDEConfig.tableWidget_customCommand->rowCount(); i++)
        if (uiDEConfig.tableWidget_customCommand->item(i, 0) != 0)
            items.append(uiDEConfig.tableWidget_customCommand->item(i, 0)->text());
    deSettings[QString("CUSTOM")] = items.join(QString("@@"));
    deSettings[QString("DESKTOPCMD")] = uiDEConfig.lineEdit_desktopCmd->text();
    deSettings[QString("GPUDEV")] = uiDEConfig.comboBox_gpudev->currentText();
    deSettings[QString("HDDDEV")] = uiDEConfig.comboBox_hdddev->currentText();
    deSettings[QString("HDDTEMPCMD")] = uiDEConfig.lineEdit_hddtempCmd->text();
    deSettings[QString("MPDADDRESS")] = uiDEConfig.lineEdit_mpdaddress->text();
    deSettings[QString("MPDPORT")] = QString::number(uiDEConfig.spinBox_mpdport->value());
    deSettings[QString("MPRIS")] = uiDEConfig.comboBox_mpris->currentText();
    items.clear();
    for (int i=0; i<uiDEConfig.tableWidget_pkgCommand->rowCount(); i++)
        if (uiDEConfig.tableWidget_pkgCommand->item(i, 0) != 0)
            items.append(uiDEConfig.tableWidget_pkgCommand->item(i, 0)->text());
    deSettings[QString("PKGCMD")] = items.join(QChar(','));
    items.clear();
    for (int i=0; i<uiDEConfig.tableWidget_pkgCommand->rowCount(); i++)
        if (uiDEConfig.tableWidget_pkgCommand->item(i, 0) != 0)
            items.append(uiDEConfig.tableWidget_pkgCommand->item(i, 1)->text());
    deSettings[QString("PKGNULL")] = items.join(QChar(','));
    deSettings[QString("PLAYER")] = uiDEConfig.comboBox_playerSelect->currentText();
    writeDataEngineConfiguration(deSettings);
}


void AwesomeWidget::configChanged()
{
    if (debug) qDebug() << PDEBUG;

    KConfigGroup cg = config();

    // widget
    configuration[QString("text")] = cg.readEntry("text", "[cpu: $cpu$%] [mem: $mem$%] [swap: $swap$%] [$netdev$: $down$/$up$KB/s]");

    // advanced
    configuration[QString("background")] = cg.readEntry("background", "2");
    configuration[QString("layout")] = cg.readEntry("layout", "0");
    configuration[QString("popup")] = cg.readEntry("popup", "2");
    configuration[QString("leftStretch")] = cg.readEntry("leftStretch", "2");
    configuration[QString("rightStretch")] = cg.readEntry("rightStretch", "2");
    configuration[QString("customTime")] = cg.readEntry("customTime", "$hh$:$mm$");
    configuration[QString("customUptime")] = cg.readEntry("customUptime", "$dd$,$hh$,$mm$");
    configuration[QString("tempUnits")] = cg.readEntry("tempUnits", "Celsius");
    configuration[QString("tempDevice")] = cg.readEntry("tempUnits", "");
    configuration[QString("mount")] = cg.readEntry("mount", "/");
    configuration[QString("disk")] = cg.readEntry("disk", "disk/sda_(8:0)");
    configuration[QString("useCustomNetdev")] = cg.readEntry("useCustomNetdev", "0");
    configuration[QString("customNetdev")] = cg.readEntry("customNetdev", "lo");
    configuration[QString("batteryDevice")] = cg.readEntry("batteryDevice", "/sys/class/power_supply/BAT0/capacity");
    configuration[QString("acDevice")] = cg.readEntry("acDevice", "/sys/class/power_supply/AC/online");
    configuration[QString("acOnline")] = cg.readEntry("acOnline", "(*)");
    configuration[QString("acOffile")] = cg.readEntry("acOffile", "( )");

    // tooltip
    configuration[QString("tooltipNumber")] = cg.readEntry("tooltipNumber", "100");
    configuration[QString("useTooltipBackground")] = cg.readEntry("useTooltipBackground", "2");
    configuration[QString("tooltipBackground")] = cg.readEntry("tooltipBackground", "#ffffff");
    configuration[QString("cpuTooltip")] = cg.readEntry("cpuTooltip", "2");
    configuration[QString("cpuColor")] = cg.readEntry("cpuColor", "#ff0000");
    configuration[QString("cpuclockTooltip")] = cg.readEntry("cpuclockTooltip", "2");
    configuration[QString("cpuclockColor")] = cg.readEntry("cpuclockColor", "#00ff00");
    configuration[QString("memTooltip")] = cg.readEntry("memTooltip", "2");
    configuration[QString("memColor")] = cg.readEntry("memColor", "#0000ff");
    configuration[QString("swapTooltip")] = cg.readEntry("swapTooltip", "2");
    configuration[QString("swapColor")] = cg.readEntry("swapColor", "#ffff00");
    configuration[QString("downTooltip")] = cg.readEntry("downTooltip", "2");
    configuration[QString("downColor")] = cg.readEntry("downColor", "#00ffff");
    configuration[QString("upTooltip")] = cg.readEntry("upTooltip", "2");
    configuration[QString("upColor")] = cg.readEntry("upColor", "#ff00ff");

    // appearance
    configuration[QString("interval")] = cg.readEntry("interval", "100");
    QString fontFamily = cg.readEntry("fontFamily", "Terminus");
    int fontSize = cg.readEntry("fontSize", 10);
    QString fontColor = cg.readEntry("fontColor", "#000000");
    int fontWeight = cg.readEntry("fontWeight", 400);
    QString fontStyle = cg.readEntry("fontStyle", "normal");
    formatLine[0] = QString("<html><head><meta name=\"qrichtext\" content=\"1\" />\
    <style type=\"text/css\">p, li { white-space: pre-wrap; }</style>\
    </head><body style=\"font-family:'%1'; font-size:%2pt; font-weight:%3; font-style:%4; color:%5;\"><p>")
                .arg(fontFamily)
                .arg(QString::number(fontSize))
                .arg(QString::number(fontWeight))
                .arg(fontStyle)
                .arg(fontColor);
    formatLine[1] = QString("</p></body></html>");

    reinit();
}
