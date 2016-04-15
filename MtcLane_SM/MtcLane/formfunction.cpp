#include "formfunction.h"
#include "ui_formfunction.h"
#include "formmainwindow.h"
#include "PacketSendModule/sqlitedatasaver.h"
FormFunction::FormFunction(QWidget *parent, QLayout* layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormFunction),
    m_nType(0), m_nStatus(WAIT)
{
    ui->setupUi(this);
    connect(&m_bakUpMgr, SIGNAL(devAdd(int)),this, SLOT(devActive(int)));
    connect(&m_bakUpMgr, SIGNAL(bakUpSuccess()), this, SLOT(resultSuccess()));
    connect(&m_bakUpMgr, SIGNAL(bakUpFail()), this, SLOT(resultFail()));
    m_bakUpMgr.setDataSaver(getDataSaver());
    m_bakUpMgr.startDetect();

}

FormFunction::~FormFunction()
{
    delete ui;
    m_bakUpMgr.stopDetect();

}

void FormFunction::setFuncType(int nType)
{
    m_nType = nType;
}

void FormFunction::devActive(int nStatus)
{
    switch(nStatus)
    {
    case SDOK:
        showNote(WAIT);
        break;
    case SDFULL:
        showNote(FULL);
        break;
    default:
        break;
    }
}

void FormFunction::resultSuccess()
{
    showNote(SUCCESS);
}

void FormFunction::resultFail()
{
    showNote(FAIL);
}

void FormFunction::mtcKeyPressed(MtcKeyPressedEvent *event)
{
    //只有在等待、备份成功和失败的情况下才允许退出
    if(event->isFuncKey() && event->func() == KeyEsc)
    {
        if(isAllowEsc())
        {
            if(m_pEventLoop != 0)
            {
                setResult(0);
                showNote(DETECT);
            }
        }
        return;
    }
    //在等待时，回车开始备份，在结束时，回车结束备份
    if(event->isFuncKey() && event->func() == KeyConfirm)
    {
        if(isWaiting())
        {
            beginBakup();
        }
        else if(isFinish())
        {
            setResult(1);
            showNote(DETECT);
        }
    }
    event->setKeyType(KC_Func);
}

void FormFunction::showEvent(QShowEvent *)
{
    showNote(DETECT);
}

void FormFunction::showNote(FormFunction::FUNCSTATUS status)
{
    m_nStatus = status;
    switch(status)
    {
    case DETECT:
        ui->labContent->setText(tr("请将U盘接入工控机\n按ESC返回..."));
        break;
    case WAIT:
        ui->labContent->setText(tr("已检测到SD卡插入\n剩余空间足够是否备份？"));
        ::showWarning(tr("【确认】键开始备份，【取消】键返回"));
        break;
    case EXEC:
        ui->labContent->setText(tr("正在备份数据\n请稍候..."));
        break;
    case SUCCESS:
        ui->labContent->setText(tr("数据备份成功\n请按【确认】键继续操作"));
        break;
    case FAIL:
        ui->labContent->setText(tr("数据备份失败\n请按【确认】键继续操作"));
        break;
    case FULL:
        ui->labContent->setText(tr("已检测到SD卡插入\n剩余空间不足请更换SD卡"));
        break;
    default:
        break;
    }
}

void FormFunction::beginBakup()
{
    showNote(EXEC);
    //m_bakUpMgr.bakUp(m_nType);
}


FormFunction* getFuncForm(QWidget *widget, QLayout *layout)
{
    static FormFunction* g_pFunc = NULL;
    if(g_pFunc == NULL)
    {
        g_pFunc = new FormFunction(widget, layout);
    }
    return g_pFunc;
}

