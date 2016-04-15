#include "formmainwindow.h"
#include "ui_formmainwindow.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTimer>
#include "formbottombar.h"
#include "formcapture.h"
#include "formvpr.h"
#include "formdevicestate.h"
#include "formmtctitle.h"
#include "formrecordtable.h"
#include "formwtinfo.h"
#include "formpublish.h"
#include "laneinfo.h"
#include "reqparam.h"
#include "reqtime.h"
#include "CardReaderArm/qcardreader.h"
#include "PacketSendModule/monclient.h"
#include "formmessage.h"
#include "formtablewidget.h"
#include "formcardmgr.h"
#include "formlog.h"
const char* SZCLOSEAPP = "";
const char* SZCLOSEPC = "halt";
const char* SZRESTARTAPP = "";
const char* SZRESTARTPC = "reboot";
FormMainWindow* g_MainWindow = NULL;
FormMainWindow::FormMainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMianWindow)
{
    ui->setupUi(this);
    g_MainWindow = this;
    m_leftLayout = NULL;
    m_nCloseTimer = -1;
    buildWindows();
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(ShowFree()));
    timer->start(1000);
}

FormMainWindow::~FormMainWindow()
{
    delete ui;
}

void FormMainWindow::buildWindows()
{
    this->setWindowFlags( Qt::FramelessWindowHint );
    QWidget* pBottomBar = new FormBottomBar(this);
    QWidget* pCapture = new FormCapture(this);
    QWidget* pDeviceState = new FormDeviceState(this);
    QWidget* pMTCTitle = new FormMTCTitle(this);
    QWidget* pVpr = new FormVpr(this);
    if(getLaneInfo()->isMtcLane())
    {
        if(!getLaneInfo()->isEntryLane())
        {
            m_widgetMap[emFormWtInfo] = new FormWtInfo(this);
        }
        m_widgetMap[emFormPublish] = new FormPublish(this);
        m_widgetMap[emFormWarning] = new FormWarning(this);
        m_widgetMap[emFormRecordTable] = new FormRecordTable(this);
    }
    else
    {
        m_widgetMap[emFormLog] = new FormLog(this);
        m_widgetMap[emFormCardMgr] = new FormCardMgr(this);
    }

    m_widgetMap[emFormTitle] = pMTCTitle;
    m_widgetMap[emFormDevice] = pDeviceState;
    m_widgetMap[emFormCapture] = pCapture;
    m_widgetMap[emFormBottomBar] = pBottomBar;
    m_widgetMap[emFormVpr] = pVpr;
    QWidget *leftWidget = new QWidget(this);
    QWidget *rightWidget = new QWidget(this);
    rightWidget->setMaximumWidth(480);
    if(getLaneInfo()->isMtcLane())
    {
        if(getLaneInfo()->isEntryLane())
        {
            buildEntryWindows(leftWidget,rightWidget);
        }
        else
        {
            buildExitWindows(leftWidget,rightWidget);
        }
    }
    else if(getLaneInfo()->isAutoLane())
    {
        buildAutoWindows(leftWidget, rightWidget);
    }
    //整个布局
    QGridLayout *layout=new QGridLayout;
    layout->addWidget(pMTCTitle,0,0,1,2);
    layout->addWidget(leftWidget,1,0,1,1);
    layout->addWidget(rightWidget,1,1,1,1);
    layout->addWidget(pBottomBar,2,0,1,2);
    layout->setMargin(0);
    layout->setSpacing(0);
    this->setLayout(layout);
}

void FormMainWindow::paintEvent(QPaintEvent *event)
{
    QPalette pal;
    QPixmap image(":/images/image/g1.png");
    pal.setBrush(QPalette::Window,QBrush(image));
    setPalette(pal);
}

bool getWidgetByType(FormType type, QWidget ** widget)
{
    if(g_MainWindow == NULL)
    {
        *widget = NULL;
        return false;
    }
    *widget = g_MainWindow->getWidgetByType(type);
    if(*widget == NULL)
    {
        return false;
    }
    return true;
}

QWidget * FormMainWindow::getWidgetByType(FormType type)
{
    return m_widgetMap.value(type, NULL);
}

void FormMainWindow::buildEntryWindows(QWidget* leftWidget, QWidget* rightWidget)
{
    //左半部分
    m_leftLayout = new QVBoxLayout(this);
    QWidget *  contentWidget = new QWidget();
    contentWidget->setObjectName("FormContent");
    QVBoxLayout* tmpLayout = new QVBoxLayout();
    contentWidget->setLayout(m_leftLayout);
    tmpLayout->addWidget(contentWidget);
    tmpLayout->addWidget(m_widgetMap.value(emFormWarning));
    tmpLayout->addWidget(m_widgetMap.value(emFormRecordTable));
    tmpLayout->addWidget(m_widgetMap.value(emFormPublish));
    leftWidget->setLayout(tmpLayout);
    m_leftLayout->setSpacing(0);
    tmpLayout->setSpacing(0);
    tmpLayout->setMargin(0);
    //右半部分
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_widgetMap.value(emFormCapture));
    rightLayout->addWidget(m_widgetMap.value(emFormVpr));
    rightLayout->addWidget(m_widgetMap.value(emFormDevice));
    rightWidget->setLayout(rightLayout);
    rightLayout->setSpacing(0);
    rightLayout->setMargin(0);
}

void FormMainWindow::buildExitWindows(QWidget* leftWidget, QWidget* rightWidget)
{
    //左半部分
    m_leftLayout = new QVBoxLayout(this);
    QWidget * contentWidget = new QWidget();
    contentWidget->setObjectName("FormContent");
    QVBoxLayout * tmpLayout = new QVBoxLayout();
    contentWidget->setLayout(m_leftLayout);
    tmpLayout->addWidget(contentWidget);
    tmpLayout->addWidget(m_widgetMap.value(emFormWarning));
    tmpLayout->addWidget(m_widgetMap.value(emFormWtInfo));
    tmpLayout->addWidget(m_widgetMap.value(emFormRecordTable));
    leftWidget->setLayout(tmpLayout);
    m_leftLayout->setSpacing(0);
    tmpLayout->setSpacing(0);
    tmpLayout->setMargin(0);
    //右半部分
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_widgetMap.value(emFormCapture));
    rightLayout->addWidget(m_widgetMap.value(emFormVpr));
    rightLayout->addWidget(m_widgetMap.value(emFormDevice));
    rightLayout->addWidget(m_widgetMap.value(emFormPublish));
    rightWidget->setLayout(rightLayout);
    rightLayout->setSpacing(0);
    rightLayout->setMargin(0);
}

void FormMainWindow::buildAutoWindows(QWidget *leftWidget, QWidget *rightWidget)
{
    //左半部分
    m_leftLayout = new QVBoxLayout(this);
    QWidget * contentWidget = new QWidget();
    contentWidget->setObjectName("FormContent");
    QVBoxLayout * tmpLayout = new QVBoxLayout();
    contentWidget->setLayout(m_leftLayout);
    tmpLayout->addWidget(contentWidget);
    tmpLayout->addWidget(m_widgetMap.value(emFormCardMgr));
    tmpLayout->addWidget(m_widgetMap.value(emFormLog));
    leftWidget->setLayout(tmpLayout);
    m_leftLayout->setSpacing(0);
    tmpLayout->setSpacing(0);
    tmpLayout->setMargin(0);
    tmpLayout->setStretch(0,4);
    tmpLayout->setStretch(1,20);
    tmpLayout->setStretch(2,1);
    //右半部分
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_widgetMap.value(emFormCapture));
    rightLayout->addWidget(m_widgetMap.value(emFormVpr));
    rightLayout->addWidget(m_widgetMap.value(emFormDevice));
    rightWidget->setLayout(rightLayout);
    rightLayout->setSpacing(0);
    rightLayout->setMargin(0);
}

void FormMainWindow::closeEvent(QCloseEvent *event)
{
    if(m_nCloseTimer == -1)
    {
        if(getLaneInfo()->getShiftState() != lsNormalWorking)
        {
            m_nCloseTimer = startTimer(6000);
            //TODO_liub
            getMsgWidget()->setTitle(tr("系统关闭"));
            getMsgWidget()->setMessage(tr("系统正在释放资源并关闭\n请稍后..."));
        }
        else
        {
            ::showWarning("未下班不允许关闭程序");
        }
        event->ignore();
    }
}

void FormMainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_nCloseTimer)
    {
        //抬杆
        getDeviceFactory()->getIOBoard()->SetBar(1);
        //关闭所有设备
        getDeviceFactory()->ReleaseAllDevice();
        //qDebug() << "shut down get here 4";
        //关闭参数下载线程
        //qDebug() << "shut down get here 1";

        getReqParam()->StopExcute();
        getReqParam()->wait();
        //qDebug() << "shut down get here 2";

        //关闭校时线程
        getReqTime()->Stop();
        //qDebug() << "shut down get here 3";



        //释放监控线程
        getMon()->Stop();
        //qDebug() << "shut down get here 5";

        //关闭数据库
        getDataFileMgr()->Close();
        //qDebug() << "shut down get here";
        QApplication::instance()->quit();
        ////qDebug() <<getLaneInfo()->getCloseType();
        switch(getLaneInfo()->getCloseType())
        {
        case emCloseApp:
            break;
        case emClosePC:
            QProcess::execute(SZCLOSEPC);
            break;
        case emRestartApp:
            QProcess::startDetached(QApplication::applicationFilePath() + " -qws");
            break;
        case emRestartPC:
            QProcess::execute(SZRESTARTPC);
            break;
        default:
            break;
        }
    }
}

void showWarning(const QString &sWarning,bool bOpenTimer, FormWarning::WARNINGTYPE type)
{
    FormWarning* pWarning = NULL;
    bool ok = getWidgetByType(emFormWarning, (QWidget**)&pWarning);
    if(ok)
    {
        pWarning->setWarning(sWarning, bOpenTimer, type);
    }
    else
    {
        FormLog* pLog = NULL;
        ok = getWidgetByType(emFormLog, (QWidget**)&pLog);
        if(ok)
        {
            pLog->showLog(sWarning);
        }
    }
}

QWidget* getMainWindow()
{
    return g_MainWindow;
}
