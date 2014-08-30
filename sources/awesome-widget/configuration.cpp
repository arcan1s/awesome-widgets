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
    setNetworkDevice();
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
    setTooltipBackground();
    uiTooltipConfig.kcolorcombo_cpu->setColor(QColor(configuration[QString("cpuColor")]));
    uiTooltipConfig.kcolorcombo_cpuclock->setColor(QColor(configuration[QString("cpuclockColor")]));
    uiTooltipConfig.kcolorcombo_mem->setColor(QColor(configuration[QString("memColor")]));
    uiTooltipConfig.kcolorcombo_swap->setColor(QColor(configuration[QString("swapColor")]));
    uiTooltipConfig.kcolorcombo_down->setColor(QColor(configuration[QString("downColor")]));
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
    uiAppConfig.comboBox_style->setCurrentIndex(
                uiAppConfig.comboBox_style->findText(fontStyle, Qt::MatchFixedString));
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
}


void AwesomeWidget::configChanged()
{
    if (debug) qDebug() << PDEBUG;

    KConfigGroup cg = config();

    reinit();
}


void AwesomeWidget::setNetworkDevice()
{
    if (debug) qDebug() << PDEBUG;

    if (uiAdvancedConfig.checkBox_netdev->checkState() == 0)
        uiAdvancedConfig.comboBox_netdev->setDisabled(true);
    else if (uiAdvancedConfig.checkBox_netdev->checkState() == 2)
        uiAdvancedConfig.checkBox_netdev->setEnabled(true);
}


void AwesomeWidget::setTooltipBackground()
{
    if (debug) qDebug() << PDEBUG;

    if (uiTooltipConfig.checkBox_background->checkState() == 0)
        uiTooltipConfig.kcolorcombo_cpu->setDisabled(true);
    else if (uiTooltipConfig.checkBox_background->checkState() == 2)
        uiTooltipConfig.kcolorcombo_cpu->setEnabled(true);
}
