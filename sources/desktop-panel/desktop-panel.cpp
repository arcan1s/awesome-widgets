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

#include "desktop-panel.h"

#include <KConfigDialog>
#include <KGlobal>
#include <KStandardDirs>
#include <KWindowSystem>
#include <Plasma/Containment>
#include <Plasma/Corona>
#include <Plasma/Theme>
#include <QDebug>
#include <QFile>
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QProcessEnvironment>
#include <QTextCodec>

#include <fontdialog/fontdialog.h>
#include <pdebug/pdebug.h>


CustomPlasmaLabel::CustomPlasmaLabel(DesktopPanel *wid, const int num, const bool debugCmd)
    : Plasma::Label(wid),
      debug(debugCmd),
      number(num),
      widget(wid)
{
    if (debug) qDebug() << PDEBUG << ":" << "Init label" << number;
}


CustomPlasmaLabel::~CustomPlasmaLabel()
{
}


int CustomPlasmaLabel::getNumber()
{
    if (debug) qDebug() << PDEBUG;

    return number;
}


void CustomPlasmaLabel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Get signal" << event->button();

    if (event->button() == Qt::LeftButton)
        widget->setCurrentDesktop(number);
}



DesktopPanel::DesktopPanel(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    if (debugEnv == QString("yes"))
        debug = true;
    else
        debug = false;

    setHasConfigurationInterface(true);
    // text format init
    currentFormatLine.append(QString(""));
    currentFormatLine.append(QString(""));
    formatLine.append(QString(""));
    formatLine.append(QString(""));
}


DesktopPanel::~DesktopPanel()
{
//     for (int i=0; i<labels.count(); i++)
//         delete labels[i];
//     delete layout;
}


void DesktopPanel::init()
{
    if (debug) qDebug() << PDEBUG;

    extsysmonEngine = dataEngine(QString("ext-sysmon"));

    layout = new QGraphicsLinearLayout();
    layout->setContentsMargins(1, 1, 1, 1);
    setLayout(layout);

    currentDesktop = 1;

    // read variables
    configChanged();
    connect(this, SIGNAL(activate()), this, SLOT(changePanelsState()));
}


QList<Plasma::Containment *> DesktopPanel::getPanels()
{
    if (debug) qDebug() << PDEBUG;

    QList<Plasma::Containment *> panels;
    for (int i=0; i<containment()->corona()->containments().count(); i++)
        if (containment()->corona()->containments()[i]->containmentType() == Plasma::Containment::PanelContainment)
            panels.append(containment()->corona()->containments()[i]);

    return panels;
}


QString DesktopPanel::panelLocationToStr(Plasma::Location loc)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Location" << loc;

    switch(loc) {
    case Plasma::TopEdge:
        return i18n("Top Edge");
    case Plasma::BottomEdge:
        return i18n("Bottom Edge");
    case Plasma::LeftEdge:
        return i18n("Left Edge");
    case Plasma::RightEdge:
        return i18n("Right Edge");
    default:
        return i18n("Unknown Position (%1)", loc);
    }
}


QString DesktopPanel::parsePattern(const QString rawLine, const int num)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Run function with raw line" << rawLine;
    if (debug) qDebug() << PDEBUG << ":" << "Run function with number" << num;

    QString line, mark;
    line = rawLine;
    if (currentDesktop == num + 1)
        mark = configuration[QString("mark")];
    else
        mark = QString("");
    if (line.split(QString("$mark$"))[0] != line) {
        if (debug) qDebug() << PDEBUG << ":" << "Found mark";
        line = line.split(QString("$mark$"))[0] + mark + line.split(QString("$mark$"))[1];
    }
    if (line.split(QString("$name$"))[0] != line) {
        if (debug) qDebug() << PDEBUG << ":" << "Found name";
        line = line.split(QString("$name$"))[0] + desktopNames[num] + line.split(QString("$name$"))[1];
    }
    if (line.split(QString("$number$"))[0] != line) {
        if (debug) qDebug() << PDEBUG << ":" << "Found number";
        line = line.split(QString("$number$"))[0] + QString::number(num + 1) + line.split(QString("$number$"))[1];
    }
    if (line.split(QString("$total$"))[0] != line) {
        if (debug) qDebug() << PDEBUG << ":" << "Found total";
        line = line.split(QString("$total$"))[0] + QString::number(desktopNames.count()) + line.split(QString("$total$"))[1];
    }

    return line;
}


void DesktopPanel::reinit()
{
    if (debug) qDebug() << PDEBUG;
    if (desktopNames.isEmpty()) return;

    // clear
    // labels
    for (int i=0; i<labels.count(); i++) {
        layout->removeItem(labels[i]);
        delete labels[i];
    }
    labels.clear();
    // layout
    layout = new QGraphicsLinearLayout();
    layout->setContentsMargins(1, 1, 1, 1);
    setLayout(layout);

    // add
    // layout
    if (configuration[QString("background")].toInt() == 0)
        setBackgroundHints(NoBackground);
    else
        setBackgroundHints(DefaultBackground);
    if (configuration[QString("layout")].toInt() == 0)
        layout->setOrientation(Qt::Horizontal);
    else
        layout->setOrientation(Qt::Vertical);
    // left stretch
    if (configuration[QString("leftStretch")].toInt() == 2)
        layout->addStretch(1);
    // labels
    for (int i=0; i<desktopNames.count(); i++) {
        labels.append(new CustomPlasmaLabel(this, i));
        layout->addItem(labels[i]);
    }
    // right stretch
    if (configuration[QString("rightStretch")].toInt() == 2)
        layout->addStretch(1);

    updateText();
    resize(10, 10);
}


void DesktopPanel::changePanelsState()
{
    if (debug) qDebug() << PDEBUG;

    QList<Plasma::Containment *> panels = getPanels();
    for (int i=0; i<panels.count(); i++) {
        if ((!configuration[QString("panels")].split(QChar(','))
                .contains(QString::number(i))) &&
                (configuration[QString("panels")] != QString("-1")))
            continue;
        bool wasVisible = panels[i]->view()->isVisible();
        int winId = panels[i]->view()->winId();
        if (wasVisible) {
            if (debug) qDebug() << PDEBUG << ":" << "Hide panel";
            KWindowInfo oldInfo = KWindowSystem::windowInfo(winId, NET::WMState);
            oldState = oldInfo.state();
            panels[i]->view()->setVisible(false);
        } else {
            if (debug) qDebug() << PDEBUG << ":" << "Show panel";
            panels[i]->view()->setVisible(true);
            KWindowSystem::clearState(winId, NET::KeepAbove);
            KWindowSystem::setState(winId, oldState | NET::StaysOnTop);
            KWindowSystem::setOnAllDesktops(winId, true);
        }
    }
}


void DesktopPanel::setCurrentDesktop(const int number)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Set desktop" << number + 1;

    QString cmd = parsePattern(configuration[QString("desktopcmd")], number);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd " << cmd;

    QProcess command;
    command.startDetached(cmd);
}


void DesktopPanel::updateText()
{
    if (debug) qDebug() << PDEBUG;

    if (labels.isEmpty()) return;
    QString line, text;
    for (int i=0; i<labels.count(); i++) {
        if (debug) qDebug() << PDEBUG << ":" << "Label" << i;
        line = parsePattern(configuration[QString("pattern")], i);
        if (currentDesktop == i + 1)
            text = currentFormatLine[0] + line + currentFormatLine[1];
        else
            text = formatLine[0] + line + formatLine[1];
        labels[i]->setText(text);
    }
}


// data engine interaction
void DesktopPanel::dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Run function with source name" << sourceName;

    if (data.keys().count() == 0)
        return;
    if (sourceName == QString("desktop")) {
        currentDesktop = data[QString("currentNumber")].toInt();
        if (desktopNames.isEmpty()) {
            desktopNames = data[QString("list")].toString().split(QString(";;"));
            reinit();
        }
        updateText();
    }
}


//  configuration interface
void DesktopPanel::createConfigurationInterface(KConfigDialog *parent)
{
    if (debug) qDebug() << PDEBUG;

    QWidget *appWidget = new QWidget;
    uiAppConfig.setupUi(appWidget);
    QWidget *configWidget = new QWidget;
    uiWidConfig.setupUi(configWidget);
    QWidget *toggleWidget = new QWidget;
    uiToggleConfig.setupUi(toggleWidget);

    if (configuration[QString("background")].toInt() == 0)
        uiWidConfig.checkBox_background->setCheckState(Qt::Unchecked);
    else
        uiWidConfig.checkBox_background->setCheckState(Qt::Checked);
    if (configuration[QString("layout")].toInt() == 0)
        uiWidConfig.checkBox_layout->setCheckState(Qt::Unchecked);
    else
        uiWidConfig.checkBox_layout->setCheckState(Qt::Checked);
    if (configuration[QString("leftStretch")].toInt() == 0)
        uiWidConfig.checkBox_leftStretch->setCheckState(Qt::Unchecked);
    else
        uiWidConfig.checkBox_leftStretch->setCheckState(Qt::Checked);
    if (configuration[QString("rightStretch")].toInt() == 0)
        uiWidConfig.checkBox_rightStretch->setCheckState(Qt::Unchecked);
    else
        uiWidConfig.checkBox_rightStretch->setCheckState(Qt::Checked);
    uiWidConfig.spinBox_interval->setValue(configuration[QString("interval")].toInt());
    uiWidConfig.comboBox_mark->setItemText(uiWidConfig.comboBox_mark->count()-1, configuration[QString("mark")]);
    uiWidConfig.comboBox_mark->setCurrentIndex(uiWidConfig.comboBox_mark->count()-1);
    uiWidConfig.lineEdit_pattern->setText(configuration[QString("pattern")]);
    uiWidConfig.lineEdit_desktopcmd->setText(configuration[QString("desktopcmd")]);

    KConfigGroup cg = config();
    CFont font(cg.readEntry("currentFontFamily", "Terminus"));
    font.setPointSize(cg.readEntry("currentFontSize", 10));
    font.setCurrentColor(QColor(cg.readEntry("currentFontColor", "#ff0000")));
    font.setHtmlWeight(cg.readEntry("currentFontWeight", 400));
    font.setItalic(cg.readEntry("currentFontStyle", "normal") == QString("italic"));
    uiAppConfig.fontComboBox_fontActive->setCurrentFont(font.toQFont());
    uiAppConfig.spinBox_fontSizeActive->setValue(font.pointSize());
    uiAppConfig.kcolorcombo_fontColorActive->setColor(font.color());
    if (font.italic())
        uiAppConfig.comboBox_fontStyleActive->setCurrentIndex(1);
    else
        uiAppConfig.comboBox_fontStyleActive->setCurrentIndex(0);
    uiAppConfig.spinBox_fontWeightActive->setValue(font.htmlWeight());

    font = CFont(cg.readEntry("fontFamily", "Terminus"));
    font.setPointSize(cg.readEntry("fontSize", 10));
    font.setCurrentColor(QColor(cg.readEntry("fontColor", "#000000")));
    font.setHtmlWeight(cg.readEntry("fontWeight", 400));
    font.setItalic(cg.readEntry("fontStyle", "normal") == QString("italic"));
    uiAppConfig.fontComboBox_fontInactive->setCurrentFont(font.toQFont());
    uiAppConfig.spinBox_fontSizeInactive->setValue(font.pointSize());
    uiAppConfig.kcolorcombo_fontColorInactive->setColor(font.color());
    if (font.italic())
        uiAppConfig.comboBox_fontStyleInactive->setCurrentIndex(1);
    else
        uiAppConfig.comboBox_fontStyleInactive->setCurrentIndex(0);
    uiAppConfig.spinBox_fontWeightInactive->setValue(font.htmlWeight());

    uiToggleConfig.listWidget_list->clear();
    QList<Plasma::Containment *> panels = getPanels();
    for (int i=0; i<panels.count(); i++) {
        QListWidgetItem *item = new QListWidgetItem(panelLocationToStr(panels[i]->location()), uiToggleConfig.listWidget_list);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        if ((configuration[QString("panels")].split(QChar(','))
                .contains(QString::number(i))) ||
                (configuration[QString("panels")] == QString("-1")))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
    }

    parent->addPage(configWidget, i18n("Widget"), Applet::icon());
    parent->addPage(appWidget, i18n("Appearance"), QString("preferences-desktop-theme"));
    parent->addPage(toggleWidget, i18n("Toggle panels"), QString("plasma"));

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
}


void DesktopPanel::configAccepted()
{
    if (debug) qDebug() << PDEBUG;

    extsysmonEngine->disconnectSource(QString("desktop"), this);
    KConfigGroup cg = config();

    cg.writeEntry("background", QString::number(uiWidConfig.checkBox_background->checkState()));
    cg.writeEntry("layout", QString::number(uiWidConfig.checkBox_layout->checkState()));
    cg.writeEntry("leftStretch", QString::number(uiWidConfig.checkBox_leftStretch->checkState()));
    cg.writeEntry("rightStretch", QString::number(uiWidConfig.checkBox_rightStretch->checkState()));
    cg.writeEntry("interval", QString::number(uiWidConfig.spinBox_interval->value()));
    cg.writeEntry("mark", uiWidConfig.comboBox_mark->currentText());
    cg.writeEntry("pattern", uiWidConfig.lineEdit_pattern->text());
    cg.writeEntry("desktopcmd", uiWidConfig.lineEdit_desktopcmd->text());

    cg.writeEntry("currentFontFamily", uiAppConfig.fontComboBox_fontActive->currentFont().family());
    cg.writeEntry("currentFontSize", uiAppConfig.spinBox_fontSizeActive->value());
    cg.writeEntry("currentFontColor", uiAppConfig.kcolorcombo_fontColorActive->color().name());
    cg.writeEntry("currentFontWeight", uiAppConfig.spinBox_fontWeightActive->value());
    cg.writeEntry("currentFontStyle", uiAppConfig.comboBox_fontStyleActive->currentText());

    cg.writeEntry("fontFamily", uiAppConfig.fontComboBox_fontInactive->currentFont().family());
    cg.writeEntry("fontSize", uiAppConfig.spinBox_fontSizeInactive->value());
    cg.writeEntry("fontColor", uiAppConfig.kcolorcombo_fontColorInactive->color().name());
    cg.writeEntry("fontWeight", uiAppConfig.spinBox_fontWeightInactive->value());
    cg.writeEntry("fontStyle", uiAppConfig.comboBox_fontStyleInactive->currentText());

    QStringList indexes;
    for (int i=0; i<uiToggleConfig.listWidget_list->count(); i++)
        if (uiToggleConfig.listWidget_list->item(i)->checkState() == Qt::Checked)
            indexes.append(QString::number(i));
    if (indexes.count() == uiToggleConfig.listWidget_list->count())
        cg.writeEntry("panels", QString("-1"));
    else
        cg.writeEntry("panels", indexes.join(QChar(',')));
}


void DesktopPanel::configChanged()
{
    if (debug) qDebug() << PDEBUG;

    KConfigGroup cg = config();

    configuration[QString("background")] = cg.readEntry("background", "2");
    configuration[QString("desktopcmd")] = cg.readEntry("desktopcmd", "qdbus org.kde.kwin /KWin setCurrentDesktop $number$");
    configuration[QString("interval")] = cg.readEntry("interval", "1000");
    configuration[QString("layout")] = cg.readEntry("layout", "0");
    configuration[QString("leftStretch")] = cg.readEntry("leftStretch", "2");
    configuration[QString("mark")] = cg.readEntry("mark", "¤");
    configuration[QString("panels")] = cg.readEntry("panels", "-1");
    configuration[QString("pattern")] = cg.readEntry("pattern", "[$mark$$number$/$total$: $name$]");
    configuration[QString("rightStretch")] = cg.readEntry("rightStretch", "2");

    extsysmonEngine->connectSource(QString("desktop"), this, configuration[QString("interval")].toInt());

    CFont font = CFont(cg.readEntry("currentFontFamily", "Terminus"));
    font.setPointSize(cg.readEntry("currentFontSize", 10));
    font.setCurrentColor(QColor(cg.readEntry("currentFontColor", "#ff0000")));
    font.setHtmlWeight(cg.readEntry("currentFontWeight", 400));
    font.setItalic(cg.readEntry("currentFontStyle", "normal") == QString("italic"));
    QString fontStyle;
    if (font.italic())
        fontStyle = QString("italic");
    else
        fontStyle = QString("normal");
    currentFormatLine[0] = QString("<html><head><meta name=\"qrichtext\" content=\"1\" />\
    <style type=\"text/css\">p, li { white-space: pre-wrap; }</style>\
    </head><body style=\"font-family:'%1'; font-size:%2pt; font-weight:%3; font-style:%4; color:%5;\">")
            .arg(font.family())
            .arg(font.pointSize())
            .arg(font.htmlWeight())
            .arg(fontStyle)
            .arg(font.color().name());
    currentFormatLine[1] = QString("</body></html>");

    font = CFont(cg.readEntry("fontFamily", "Terminus"));
    font.setPointSize(cg.readEntry("fontSize", 10));
    font.setCurrentColor(QColor(cg.readEntry("fontColor", "#000000")));
    font.setHtmlWeight(cg.readEntry("fontWeight", 400));
    font.setItalic(cg.readEntry("fontStyle", "normal") == QString("italic"));
    if (font.italic())
        fontStyle = QString("italic");
    else
        fontStyle = QString("normal");
    formatLine[0] = QString("<html><head><meta name=\"qrichtext\" content=\"1\" />\
    <style type=\"text/css\">p, li { white-space: pre-wrap; }</style>\
    </head><body style=\"font-family:'%1'; font-size:%2pt; font-weight:%3; font-style:%4; color:%5;\">")
            .arg(font.family())
            .arg(font.pointSize())
            .arg(font.htmlWeight())
            .arg(fontStyle)
            .arg(font.color().name());
    formatLine[1] = QString("</body></html>");

    reinit();
}


#include "desktop-panel.moc"
