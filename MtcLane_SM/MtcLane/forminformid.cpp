#include "forminformid.h"
#include "ui_forminformid.h"
#include "formmainwindow.h"
#include "MtcKey/MtcLaneState.h"
#include "datafilemgr.h"
#include "statemachinetransition.h"
#include "lanectrl.h"
#include "exittollitem.h"
ForminformId::ForminformId(QWidget *parent,QLayout *layout) :
    MtcOpWidget(parent,layout),
    ui(new Ui::ForminformId)
{
    ui->setupUi(this);
    this->filterChildrenKeyEvent();
    m_pCurLineEdit = ui->lineEdit;
    QSettings setting("./Lane.ini", QSettings::IniFormat);
    m_ticketLength = setting.value("Other/TicketLength", 12).toInt();
}

ForminformId::~ForminformId()
{
    delete ui;
}

bool ForminformId::IsEnterInvFinished(QString &sError)
{
    //liujian 换上票号 此处输入完成后不能确认此次操作时换上操作还是修改操作
    ((ExitTollItem*)getLaneCtrl()->getTollItem())->SendInvDeal(IOT_InvUp);
    ::showWarning(tr(""), true, FormWarning::WARNING);
    return true;
}

void ForminformId::clearData()
{
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

qint64 ForminformId::getNewInvoiceNo()
{
    return ui->lineEdit_2->text().toLongLong();
}

void ForminformId::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Number);
        handleNumberKey(mtcKeyEvent);
    }
    if(mtcKeyEvent->isFuncKey())
    {
        switch(mtcKeyEvent->func())
        {
        case KeyConfirm:
            handleEnterKey(mtcKeyEvent);
            break;
        case KeyDel:
            handleEraserKey();
            mtcKeyEvent->setKeyType(KC_Func);
            break;
        case KeyEsc:
            ::showWarning(tr(""), true, FormWarning::WARNING);
        default:
            break;
        }
    }
    if(m_pEventLoop != 0)
    {
        mtcKeyEvent->setKeyType(KC_Func);
    }
}

void ForminformId::showEvent(QShowEvent *event)
{

    ui->stackedWidget->setCurrentIndex(0);
    m_pCurLineEdit = ui->lineEdit;
    ::showWarning(tr("请按【数字】键输入票据起号"),true);
    clearData();
}

void ForminformId::handleNumberKey(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(m_pCurLineEdit->text().length() < m_ticketLength)
    {
        m_pCurLineEdit->setText(
                    m_pCurLineEdit->text().append(mtcKeyEvent->getLogicKeyName()));
    }
}


void ForminformId::handleEnterKey(MtcKeyPressedEvent* mtcKeyEvent)
{
    //第一遍输入票号完成
    if(ui->stackedWidget->currentIndex() == 0)
    {
        mtcKeyEvent->setKeyType(KC_Func);
        if(m_pCurLineEdit->text().toLongLong() == 0)
        {
            ::showWarning(tr("票号不能为0"));
            ui->lineEdit->clear();
            return;
        }
        ui->stackedWidget->setCurrentIndex(1);
        m_pCurLineEdit = ui->lineEdit_2;
        ::showWarning(tr("请再次按【数字】键输入票据起号"),true);
    }
    else
    {
        if(ui->lineEdit->text().toLongLong() != ui->lineEdit_2->text().toLongLong())
        {
            ::showWarning(tr("输入的票号不一致，请重新输入！"),true);
            mtcKeyEvent->setKeyType(KC_Func);
            ui->lineEdit->clear();
            ui->lineEdit_2->clear();
            m_pCurLineEdit = ui->lineEdit;
            ui->stackedWidget->setCurrentIndex(0);
        }
        else if(m_pEventLoop != 0)
        {
            getDataFileMgr()->UpInvoice(getNewInvoiceNo(),getNewInvoiceNo()+ 100000, 100000 );
            //显示票号
            getDataFileMgr()->UpdateWidgetData();
            this->clearData();
            ((ExitTollItem*)getLaneCtrl()->getTollItem())->SendInvDeal(IOT_InvUp);
            setResult(1);
        }
    }
}

void ForminformId::handleEraserKey()
{
    m_pCurLineEdit->setText(m_pCurLineEdit->text().left(m_pCurLineEdit->text().length() -1));
}

ForminformId* getInformWidget(QWidget* widget, QLayout* layout)
{
    static ForminformId* g_pInform = NULL;
    if(g_pInform == NULL)
    {
        g_pInform = new ForminformId(widget, layout);
    }
    return g_pInform;
}
void ForminformId::handleEscKey(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(m_pEventLoop != 0)
    {
        mtcKeyEvent->setKeyType(KC_Func);
        setResult(0);
    }
}

void ForminformId::setInfoTitle1()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->label_2->setText(tr("换上票据"));
    ui->stackedWidget->setCurrentIndex(1);
    ui->label_4->setText(tr("换上票据"));
}

void ForminformId::setInfoTitle2()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->label_2->setText(tr("更改票据"));
    ui->stackedWidget->setCurrentIndex(0);
    ui->label_4->setText(tr("更改票据"));
}


