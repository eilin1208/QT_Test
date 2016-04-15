#include "pnllanemonitor.h"
#include "ui_pnllanemonitor.h"
#include "common/unused.h"
#include "mainwindow.h"
#include "iconmanager.h"

CPnlLaneMonitor::CPnlLaneMonitor(QWidget *parent) :
    MdiChild(parent),
    ui(new Ui::CPnlLaneMonitor)
{
    ui->setupUi(this);
    initActions();
    updateShortcutTips();
}

CPnlLaneMonitor::~CPnlLaneMonitor()
{
    delete ui;
}


void CPnlLaneMonitor::InitUI()
{
    CPnlStation *p1 = new CPnlStation(this);
    p1->InitUI(500603);
    m_pStationMap[500603] = p1;
    ui->verticalLayout_2->insertWidget(0, p1);
    connect(p1,SIGNAL(ChangeStationSize()),this,SLOT(ChangeStationSize()));

    CPnlStation *p2 = new CPnlStation(this);
    p2->InitUI(500602);
    m_pStationMap[500602] = p2;
    ui->verticalLayout_2->insertWidget(1, p2);
    connect(p2,SIGNAL(ChangeStationSize()),this,SLOT(ChangeStationSize()));

    ChangeStationSize();
}

void CPnlLaneMonitor::resizeEvent(QResizeEvent *event)
{
    UNUSED(event);
}

void CPnlLaneMonitor::ChangeStationSize()
{
    int nHeight =0;
    QMap<int, CPnlStation*>::iterator it = m_pStationMap.begin();
    for(; it!=m_pStationMap.end(); it++)
    {
        nHeight += it.value()->GetUIHeight();
    }
    nHeight += 20;

    ui->scrollAreaWidgetContents->resize(ui->scrollAreaWidgetContents->width(), nHeight);
    ui->scrollAreaWidgetContents->setMinimumHeight(nHeight);
    ui->scrollAreaWidgetContents->setMaximumHeight(nHeight);
}


QToolBar* CPnlLaneMonitor::getToolBar(int toolbar) const
{
    UNUSED(toolbar);
    return nullptr;
}

void CPnlLaneMonitor::createActions()
{

}

void CPnlLaneMonitor::setupDefShortcuts()
{

}

QVariant CPnlLaneMonitor::saveSession()
{
    return 0;
}

bool CPnlLaneMonitor::restoreSession(const QVariant& sessionValue)
{
    UNUSED(sessionValue);
    return true;
}

Icon* CPnlLaneMonitor::getIconNameForMdiWindow()
{
    return ICONS.LANEMONITOR;
}

QString CPnlLaneMonitor::getTitleForMdiWindow()
{
//    QStringList existingNames = MainWindow::getInstance()->getMdiArea()->getWindowTitles();
//    QString title = tr("SQL editor %1").arg(sqlEditorNum++);
//    while (existingNames.contains(title))
//        title = tr("SQL editor %1").arg(sqlEditorNum++);

    return tr("Lane Monitor");
}


bool CPnlLaneMonitor::isUncommited() const
{
    return false;
}

QString CPnlLaneMonitor::getQuitUncommitedConfirmMessage() const
{
    return "";
//    return tr("window \"%1\" has uncommited data.").arg(getMdiWindow()->windowTitle());
}
