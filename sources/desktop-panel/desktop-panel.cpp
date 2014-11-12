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
#include <Plasma/ToolTipManager>
#include <QDebug>
#include <QFile>
#include <QGraphicsGridLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QProcessEnvironment>
#include <QTextCodec>
#include <QTimer>

#include <fontdialog/fontdialog.h>
#include <pdebug/pdebug.h>


CustomPlasmaLabel::CustomPlasmaLabel(DesktopPanel *wid, const int num, const bool debugCmd)
    : QLabel(0),
      debug(debugCmd),
      number(num),
      widget(wid)
{
    if (debug) qDebug() << PDEBUG << ":" << "Init label" << number;
}


CustomPlasmaLabel::~CustomPlasmaLabel()
{
    if (debug) qDebug() << PDEBUG;
}


int CustomPlasmaLabel::getNumber()
{
    if (debug) qDebug() << PDEBUG;

    return number;
}


void CustomPlasmaLabel::enterEvent(QEvent *event)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Event" << event->type();

    if (event->type() == QEvent::Enter)
        widget->paintTooltip(number);

    emit(QLabel::enterEvent(event));
}


void CustomPlasmaLabel::mousePressEvent(QMouseEvent *event)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Get signal" << event->button();

    if (event->button() == Qt::LeftButton)
        widget->setCurrentDesktop(number);
    emit(QLabel::mousePressEvent(event));
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

    setBackgroundHints(DefaultBackground);
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setHasConfigurationInterface(true);
    // text format init
    currentFormatLine.append(QString(""));
    currentFormatLine.append(QString(""));
    formatLine.append(QString(""));
    formatLine.append(QString(""));
}


DesktopPanel::~DesktopPanel()
{
    if (debug) qDebug() << PDEBUG;
}


void DesktopPanel::init()
{
    if (debug) qDebug() << PDEBUG;

    layout = new QGraphicsGridLayout();
    layout->setContentsMargins(1, 1, 1, 1);
    setLayout(layout);

    // tooltip
    toolTip = Plasma::ToolTipContent();
    toolTipScene = new QGraphicsScene();
    toolTipView = new QGraphicsView(toolTipScene);
    toolTipView->setStyleSheet(QString("background: transparent"));
    toolTipView->setContentsMargins(0, 0, 0, 0);
    toolTipView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    toolTipView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    Plasma::ToolTipManager::self()->setContent(this, toolTip);

    // read variables
    configChanged();
    connect(this, SIGNAL(activate()), this, SLOT(changePanelsState()));
    connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)), this, SLOT(updateText(int)));
}


DesktopPanel::DesktopWindowsInfo DesktopPanel::getInfoByDesktop(const int num)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Number" << num;


    DesktopWindowsInfo info;
    info.desktop = KWindowSystem::workArea(num);

    QList<WId> windows = KWindowSystem::windows();
    for (int i=0; i<windows.count(); i++) {
        KWindowInfo winInfo = KWindowSystem::windowInfo(windows[i],
                                                        NET::Property::WMDesktop | NET::Property::WMGeometry |
                                                        NET::Property::WMState | NET::Property::WMWindowType);
        if (winInfo.windowType(NET::WindowTypeMask::NormalMask) != NET::WindowType::Normal) continue;
        if (winInfo.isMinimized()) continue;
        if (!winInfo.isOnDesktop(num)) continue;
        info.windows.append(winInfo.geometry());
    }

    return info;
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
    if (debug) qDebug() << PDEBUG << ":" << "Raw line" << rawLine;
    if (debug) qDebug() << PDEBUG << ":" << "Number" << num;

    QString line, fullMark, mark;
    line = rawLine;
    if (KWindowSystem::currentDesktop() == num + 1)
        mark = configuration[QString("mark")];
    else
        mark = QString("");
    fullMark = QString("%1").arg(mark, configuration[QString("mark")].count(), QLatin1Char(' '));
    if (line.contains(QString("$fullmark")))
        line.replace(QString("$fullmark"), fullMark);
    if (line.contains(QString("$mark")))
        line.replace(QString("$mark"), mark);
    if (line.contains(QString("$name")))
        line.replace(QString("$name"), desktopNames[num]);
    if (line.contains(QString("$number")))
        line.replace(QString("$number"), QString::number(num + 1));
    if (line.contains(QString("$total")))
        line.replace(QString("$total"), QString::number(desktopNames.count()));

    return line;
}


void DesktopPanel::reinit()
{
    if (debug) qDebug() << PDEBUG;

    // clear
    // labels
    for (int i=0; i<proxyWidgets.count(); i++) {
        layout->removeItem(proxyWidgets[i]);
        proxyWidgets[i]->setWidget(0);
        delete labels[i];
        delete proxyWidgets[i];
    }
    labels.clear();
    proxyWidgets.clear();
    desktopNames.clear();

    // add
    int total = KWindowSystem::numberOfDesktops();
    for (int i=1; i<total+1; i++)
        desktopNames.append(KWindowSystem::desktopName(i));
    // layout
    if (configuration[QString("background")].toInt() == 0)
        setBackgroundHints(NoBackground);
    // labels
    for (int i=0; i<desktopNames.count(); i++) {
        proxyWidgets.append(new QGraphicsProxyWidget(this));
        labels.append(new CustomPlasmaLabel(this, i, debug));
        labels[i]->setWordWrap(false);
        labels[i]->setAttribute(Qt::WA_TranslucentBackground, true);
        proxyWidgets[i]->setWidget(labels[i]);
        if (configuration[QString("layout")].toInt() == 0)
            layout->addItem(proxyWidgets[i], 0, i);
        else
            layout->addItem(proxyWidgets[i], i, 0);
    }

    updateText(KWindowSystem::currentDesktop());
    for (int i=0; i<proxyWidgets.count(); i++) {
        labels[i]->adjustSize();
        proxyWidgets[i]->setGeometry(labels[i]->geometry());
    }
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


void DesktopPanel::paintTooltip(const int active)
{
    if (debug) qDebug() << PDEBUG;
    if (active == activeTooltip) return;

    // prepare
    activeTooltip = active;
    DesktopWindowsInfo info = getInfoByDesktop(active + 1);
    float margin = 3.0 * info.desktop.width() / 400.0;
    toolTipView->resize(info.desktop.width() + 2.0 * margin, info.desktop.height() + 2.0 * margin);
    toolTipScene->clear();
    toolTipScene->setBackgroundBrush(QBrush(Qt::NoBrush));
    // borders
    toolTipScene->addLine(0, 0, 0, info.desktop.height() + 2.0 * margin);
    toolTipScene->addLine(0, info.desktop.height() + 2.0 * margin,
                          info.desktop.width() + 2.0 * margin, info.desktop.height() + 2.0 * margin);
    toolTipScene->addLine(info.desktop.width() + 2.0 * margin, info.desktop.height() + 2.0 * margin,
                          info.desktop.width() + 2.0 * margin, 0);
    toolTipScene->addLine(info.desktop.width() + 2.0 * margin, 0, 0, 0);

    QPen pen = QPen();
    pen.setWidthF(2.0 * info.desktop.width() / 400.0);
    pen.setColor(QColor(configuration[QString("tooltipColor")]));
    for (int i=0; i<info.windows.count(); i++) {
        toolTipScene->addLine(info.windows[i].left() + margin, info.windows[i].bottom() + margin,
                              info.windows[i].left() + margin, info.windows[i].top() + margin, pen);
        toolTipScene->addLine(info.windows[i].left() + margin, info.windows[i].top() + margin,
                              info.windows[i].right() + margin, info.windows[i].top() + margin, pen);
        toolTipScene->addLine(info.windows[i].right() + margin, info.windows[i].top() + margin,
                              info.windows[i].right() + margin, info.windows[i].bottom() + margin, pen);
        toolTipScene->addLine(info.windows[i].right() + margin, info.windows[i].bottom() + margin,
                              info.windows[i].left() + margin, info.windows[i].bottom() + margin, pen);
    }

    toolTip.setImage(QPixmap::grabWidget(toolTipView).scaledToWidth(configuration[QString("tooltipWidth")].toInt()));
    Plasma::ToolTipManager::self()->setContent(this, toolTip);
}


void DesktopPanel::setCurrentDesktop(const int number)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Set desktop" << number + 1;

    KWindowSystem::setCurrentDesktop(number + 1);
}


void DesktopPanel::updateText(const int active)
{
    if (debug) qDebug() << PDEBUG;

    QString line, text;
    for (int i=0; i<labels.count(); i++) {
        if (debug) qDebug() << PDEBUG << ":" << "Label" << i;
        line = parsePattern(configuration[QString("pattern")], i);
        if (active == i + 1)
            text = currentFormatLine[0] + line + currentFormatLine[1];
        else
            text = formatLine[0] + line + formatLine[1];
        labels[i]->setText(text);
    }
    int height = 0;
    int width = 0;
    for (int i=0; i<proxyWidgets.count(); i++) {
        labels[i]->adjustSize();
        proxyWidgets[i]->setGeometry(labels[i]->geometry());
        if (configuration[QString("layout")].toInt() == 0) {
            width += proxyWidgets[i]->geometry().width();
            if (height < proxyWidgets[i]->geometry().height())
                height = proxyWidgets[i]->geometry().height();
        } else {
            height += proxyWidgets[i]->geometry().height();
            if (width < proxyWidgets[i]->geometry().width())
                width = proxyWidgets[i]->geometry().width();
        }
    }
    layout->setMinimumSize(width, height);
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

    uiWidConfig.textEdit_elements->setPlainText(configuration[QString("pattern")]);
    if (configuration[QString("tooltip")].toInt() == 0)
        uiWidConfig.checkBox_tooltip->setCheckState(Qt::Unchecked);
    else
        uiWidConfig.checkBox_tooltip->setCheckState(Qt::Checked);
    uiWidConfig.spinBox_tooltip->setValue(configuration[QString("tooltipWidth")].toInt());
    uiWidConfig.kcolorcombo_tooltipColor->setColor(QColor(configuration[QString("tooltipColor")]));
    if (configuration[QString("background")].toInt() == 0)
        uiWidConfig.checkBox_background->setCheckState(Qt::Unchecked);
    else
        uiWidConfig.checkBox_background->setCheckState(Qt::Checked);
    if (configuration[QString("layout")].toInt() == 0)
        uiWidConfig.checkBox_layout->setCheckState(Qt::Unchecked);
    else
        uiWidConfig.checkBox_layout->setCheckState(Qt::Checked);
    uiWidConfig.comboBox_mark->setItemText(uiWidConfig.comboBox_mark->count()-1, configuration[QString("mark")]);
    uiWidConfig.comboBox_mark->setCurrentIndex(uiWidConfig.comboBox_mark->count()-1);

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

    connect(uiWidConfig.pushButton_tags, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_br, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_font, SIGNAL(clicked(bool)), this, SLOT(setFontFormating()));
    connect(uiWidConfig.pushButton_bold, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_italic, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_underline, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_strike, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_left, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_center, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_right, SIGNAL(clicked(bool)), this, SLOT(setFormating()));
    connect(uiWidConfig.pushButton_fill, SIGNAL(clicked(bool)), this, SLOT(setFormating()));

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
}


void DesktopPanel::configAccepted()
{
    if (debug) qDebug() << PDEBUG;

    KConfigGroup cg = config();

    cg.writeEntry("pattern", uiWidConfig.textEdit_elements->toPlainText());
    cg.writeEntry("tooltip", QString::number(uiWidConfig.checkBox_tooltip->checkState()));
    cg.writeEntry("tooltipWidth", QString::number(uiWidConfig.spinBox_tooltip->value()));
    cg.writeEntry("tooltipColor", uiWidConfig.kcolorcombo_tooltipColor->color().name());
    cg.writeEntry("background", QString::number(uiWidConfig.checkBox_background->checkState()));
    cg.writeEntry("layout", QString::number(uiWidConfig.checkBox_layout->checkState()));
    cg.writeEntry("mark", uiWidConfig.comboBox_mark->currentText());

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

    configuration[QString("pattern")] = cg.readEntry("pattern", "[$fullmark$number/$total: $name]");
    configuration[QString("tooltip")] = cg.readEntry("tooltip", "2");
    configuration[QString("tooltipWidth")] = cg.readEntry("tooltipWidth", "200");
    configuration[QString("tooltipColor")] = cg.readEntry("tooltipColor", "#ffffff");
    configuration[QString("background")] = cg.readEntry("background", "2");
    configuration[QString("layout")] = cg.readEntry("layout", "0");
    configuration[QString("mark")] = cg.readEntry("mark", "¤");
    configuration[QString("panels")] = cg.readEntry("panels", "-1");

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
    currentFormatLine[0] = QString("<html><head><style type=\"text/css\">p, li { white-space: pre-wrap; }</style>\
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
    formatLine[0] = QString("<html><head><style type=\"text/css\">p, li { white-space: pre-wrap; }</style>\
                            </head><body style=\"font-family:'%1'; font-size:%2pt; font-weight:%3; font-style:%4; color:%5;\">")
            .arg(font.family())
            .arg(font.pointSize())
            .arg(font.htmlWeight())
            .arg(fontStyle)
            .arg(font.color().name());
    formatLine[1] = QString("</body></html>");

    reinit();
}


void DesktopPanel::setFontFormating()
{
    if (debug) qDebug() << PDEBUG;

    CFont defaultFont = CFont(uiAppConfig.fontComboBox_fontInactive->currentFont().family(),
                              uiAppConfig.spinBox_fontSizeInactive->value(),
                              400, false, uiAppConfig.kcolorcombo_fontColorInactive->color());
    CFont font = CFontDialog::getFont(i18n("Select font"), defaultFont,
                                      false, false);
    QString selectedText = uiWidConfig.textEdit_elements->textCursor().selectedText();
    uiWidConfig.textEdit_elements->insertPlainText(QString("<span style=\"color:%1; font-family:'%2'; font-size:%3pt;\">")
                                                   .arg(font.color().name()).arg(font.family()).arg(font.pointSize()) +
                                                   selectedText + QString("</span>"));
}


void DesktopPanel::setFormating()
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Sender" << sender();

    QString selectedText = uiWidConfig.textEdit_elements->textCursor().selectedText();
    if (sender() == uiWidConfig.pushButton_tags)
        uiWidConfig.textEdit_elements->insertPlainText(QString("$") + uiWidConfig.comboBox_tags->currentText());
    else if (sender() == uiWidConfig.pushButton_br)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<br>\n"));
    else if (sender() == uiWidConfig.pushButton_bold)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<b>") + selectedText + QString("</b>"));
    else if (sender() == uiWidConfig.pushButton_italic)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<i>") + selectedText + QString("</i>"));
    else if (sender() == uiWidConfig.pushButton_underline)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<u>") + selectedText + QString("</u>"));
    else if (sender() == uiWidConfig.pushButton_strike)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<s>") + selectedText + QString("</s>"));
    else if (sender() == uiWidConfig.pushButton_left)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<p align=\"left\">") + selectedText + QString("</p>"));
    else if (sender() == uiWidConfig.pushButton_center)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<p align=\"center\">") + selectedText + QString("</p>"));
    else if (sender() == uiWidConfig.pushButton_right)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<p align=\"right\">") + selectedText + QString("</p>"));
    else if (sender() == uiWidConfig.pushButton_fill)
        uiWidConfig.textEdit_elements->insertPlainText(QString("<p align=\"justify\">") + selectedText + QString("</p>"));
}


#include "desktop-panel.moc"
