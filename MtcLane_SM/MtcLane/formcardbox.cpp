#include "formcardbox.h"
#include "ui_formcardbox.h"
#include "formmainwindow.h"
#include "devicefactory.h"
#include "lanectrl.h"
#include "datafilemgr.h"
#include "formmessage.h"
#include "formauth.h"
#include "laneinfo.h"
#include "formmtctitle.h"

FormCardBox::FormCardBox(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormCardBox)
{
    ui->setupUi(this);
}

FormCardBox::~FormCardBox()
{
    delete ui;
}

bool FormCardBox::CardBoxRegister(bool bCardBoxUp)
{
    m_bCardBoxUp = bCardBoxUp;
    if(doModalShow() == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void FormCardBox::DetectCard(quint32 nDetectId, int nCardType)
{
    TBoxCardBasicInfo BasicInfo;
    TBoxCardAppInfo AppInfo;
    QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    //停止卡读写器检测
    pReader->StopCardDetection();
    bool bContinueDetection = false;
    //判断卡类型
    if(TYPE_M1_S50_BLOCK == nCardType || TYPE_M1_S50_MAD == nCardType ||
            TYPE_M1_S70_BLOCK == nCardType || TYPE_M1_S70_MAD == nCardType)
    {
        LogMsg("lane", tr("开始读卡盒卡"));
        if(!pReader->ReadBoxCard(BasicInfo, AppInfo))
        {
            if(m_bCardBoxUp)
            {
                ::showWarning(tr("读卡盒卡失败!"), false, FormWarning::WARNING);
            }
            else
            {
                ::showWarning(tr("读卡盒卡失败，按【取消】键强制换下卡盒!"), false, FormWarning::WARNING);
            }
            pReader->StartCardDetection();
            return;
        }
    }
    else
    {
        ::showWarning(tr("非卡盒卡！"));
        pReader->StartCardDetection();
        return;
    }
    //判断卡盒卡合法性
    if(BasicInfo.M1BasicInfo.wNetworkId != ORG_NETWORKID)
    {
        ::showWarning(tr("非本网络卡！"));
        pReader->StartCardDetection();
        return;
    }
    if(BasicInfo.M1BasicInfo.bType != CARD_TYPE_BOX_CARD)
    {
        ::showWarning(tr("非卡盒卡！"));
        pReader->StartCardDetection();
        return;
    }

    if(!m_bCardBoxUp)
    {
        if(BasicInfo.M1BasicInfo.dwCardNum != getDataFileMgr()->getCardBoxInfo().FlagCardID||
                AppInfo.dwBoxID !=  getDataFileMgr()->getCardBoxInfo().FlagCardID)
        {
            ::showWarning(tr("请刷卡盒卡:%1").arg(getDataFileMgr()->getCardBoxInfo().FlagCardID));
            pReader->StartCardDetection();
            return;
        }
        AppInfo.dwBoxID = getDataFileMgr()->getCardBoxInfo().CardBoxID;
        AppInfo.dwCount = getDataFileMgr()->getCardBoxInfo().nCardCount;
        if(!pReader->WriteBoxCard(BasicInfo.M1BasicInfo.dwCardId, AppInfo))
        {
            ::showWarning(tr("下班回写卡盒卡失败，请重新刷卡！"));
            pReader->StartCardDetection();
            return;
        }
        LogMsg("lane", tr("下班回写卡盒卡成功，卡盒编号：%1，卡数：%2")
               .arg(AppInfo.dwBoxID).arg(AppInfo.dwCount));
    }
    //换卡盒处理
    if(HandBoxCard(BasicInfo, AppInfo))
    {
        bContinueDetection = false;
        //m_nResult = 1;
        FormMTCTitle *pTitle = NULL;
        getWidgetByType(emFormTitle, (QWidget**)&pTitle);
        pTitle->setCardBoxNum(QString::number(AppInfo.dwBoxID));
        pTitle->setCardNum(AppInfo.dwCount);
        setResult(1);

        pReader->StopCardDetection();
    }
    else
    {
        ::showWarning(tr("非卡盒卡！"));
        pReader->StartCardDetection();
    }
    if(bContinueDetection)
    {
        pReader->StartCardDetection();
    }
}

void FormCardBox::showEvent(QShowEvent *event)
{
    //打开读写器开始检测
    QCardReader* pReader = (QCardReader*)getDeviceFactory()->getCardReader(DEV_CardReader1);
    connect(pReader->getDetectThread(), SIGNAL(hasOneCard(quint32,int)),
            this, SLOT(DetectCard(quint32,int)));
    pReader->StartCardDetection();
    if(!m_bCardBoxUp)
    {
        ui->label->setText(tr("卡盒换下, 请刷卡盒卡..."));
    }
    else
    {
        ui->label->setText(tr("卡盒换上, 请刷卡盒卡..."));
    }
    ::showWarning(tr("请在卡读写器上刷卡盒卡"), false );
    //发送卡箱信息报文 modify by xiangkai
    getLaneCtrl()->getTollItem()->SendLaneEvent(LET_CardBox);
}

void FormCardBox::hideEvent(QHideEvent *event)
{
    ::showWarning("", false);
    QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    disconnect(pReader->getDetectThread(), SIGNAL(hasOneCard(quint32,int)),
               this, SLOT(DetectCard(quint32,int)));
    ::showWarning(tr(""), true, FormWarning::WARNING);
}

void FormCardBox::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyEsc)
    {
        if(m_bCardBoxUp)
        {
            m_nResult = 0;
            if(getDataFileMgr()->getCardBoxInfo().CardBoxID > 0)
            {
                LoadCardBox(0,0,0);
            }
            LogMsg("lane", tr("收费员取消卡盒换上操作！"));
        }
        else
        {
            getMsgWidget()->setTitle(tr("强制换下卡盒"));
            getMsgWidget()->setMessage("确定要强制换下卡盒吗？");
            this->hide();
            int nRlt = getMsgWidget()->doModalShow();

            if(!nRlt)
            {
                this->show();
                this->setFocus();
                return;
            }
            else
            {
                //监控授权
                getAuth()->setMonType(mqCompelDownBox);
                this->hide();
                if(!getAuth()->doModalShow())
                {
                    this->show();
                    this->setFocus();
                    return;
                }
                this->show();
                this->setFocus();
                LogMsg("lane", tr("收费员(%1)强制换下卡盒").arg(getLaneInfo()->getOper().sOperName));
                //发送强制换下报文
                UnLoadCardBox(CARDBOXOP_FORCEDOWN);
            }
        }
        //m_nResult = 1;
        setResult(1);
        mtcKeyEvent->setKeyType(KC_Func);
        getDeviceFactory()->getCardReader(DEV_CardReader1)->StopCardDetection();
    }
    mtcKeyEvent->setKeyType(KC_Func);

}

bool FormCardBox::LoadCardBox(quint32 nCardBoxNo, quint32 nFlagCardNo, quint16 nCount)
{
    getDataFileMgr()->LoadCardBox(nCardBoxNo, nFlagCardNo, nCount);
    getLaneCtrl()->getTollItem()->SetOccurTime(QDateTime::currentDateTime());
    getLaneCtrl()->getTollItem()->SendCardBox(1);
    return true;
}

bool FormCardBox::UnLoadCardBox(int nOperType)
{
    getDataFileMgr()->UnLoadCardBox(nOperType);
    getLaneCtrl()->getTollItem()->SetOccurTime(QDateTime::currentDateTime());
    getLaneCtrl()->getTollItem()->SendCardBox(nOperType);
    return true;
}

bool FormCardBox::CardBoxUp(TBoxCardBasicInfo& basicInfo, TBoxCardAppInfo& appInfo)
{
    //卡盒号
    quint32 dwBoxID = appInfo.dwBoxID;
    //卡盒卡号
    quint32 dwCardNum = basicInfo.M1BasicInfo.dwCardNum;
    //卡盒卡数
    quint32 wCardCount = appInfo.dwCount;
    //判断上次是否异常退出，程序异常退出时，要提示收费员恢复上次数据
    if(getDataFileMgr()->getCardBoxInfo().FlagCardID == basicInfo.M1BasicInfo.dwCardNum &&
            getDataFileMgr()->getCardBoxInfo().FlagCardID == appInfo.dwBoxID)
    {
        //要根据入出口类型不同对卡盒数据的恢复判断也不同
        if(getLaneInfo()->getLaneType() == LaneType_MtcIn)
        {
            if(getDataFileMgr()->getCardBoxInfo().nCardCount < appInfo.dwCount)
            {
                //提示收费员恢复数据
                getMsgWidget()->setTitle(tr("卡盒恢复"));
                getMsgWidget()->setMessage(tr("上次程序异常退出\n是否恢复卡盒数据。"));
                this->hide();
                int nRlt = getMsgWidget()->doModalShow();
                this->show();
                this->setFocus();
                if(nRlt)
                {
                    LogMsg("lane", tr("上次程序异常退出，收费员选择恢复。"));
                    LogMsg("lane", tr("卡盒编号：%1，卡数：%2")
                           .arg(getDataFileMgr()->getCardBoxInfo().CardBoxID)
                           .arg(getDataFileMgr()->getCardBoxInfo().nCardCount));
                    wCardCount = getDataFileMgr()->getCardBoxInfo().nCardCount;
                }
            }
        }
        else if(getLaneInfo()->getLaneType() == LaneType_MtcOut)
        {
            //如果是出口，本地文件中保存卡数比标签卡中保存的卡数多时，需要恢复数据
            if(getDataFileMgr()->getCardBoxInfo().nCardCount > appInfo.dwCount)
            {
                //提示收费员恢复数据
                getMsgWidget()->setTitle(tr("卡盒恢复"));
                getMsgWidget()->setMessage(tr("上次程序异常退出\n是否恢复卡盒数据。"));
                this->hide();
                int nRlt = getMsgWidget()->doModalShow();
                this->show();
                this->setFocus();
                if(nRlt)
                {
                    LogMsg("lane", tr("上次程序异常退出，收费员选择恢复。"));
                    LogMsg("lane", tr("卡盒编号：%1，卡数：%2")
                           .arg(getDataFileMgr()->getCardBoxInfo().CardBoxID)
                           .arg(getDataFileMgr()->getCardBoxInfo().nCardCount));
                    wCardCount = getDataFileMgr()->getCardBoxInfo().nCardCount;
                }
            }

        }
    }
    if(!LoadCardBox(dwBoxID, dwCardNum, wCardCount))
    {
        return false;
    }

    //记录日志
    LogMsg("lane", tr("换上卡盒成功，卡号：%1，卡盒编号：%2，卡数：%3").arg(dwBoxID).arg(dwCardNum).arg(wCardCount));

    return true;
}

bool FormCardBox::CardBoxDown(TBoxCardBasicInfo& basicInfo, TBoxCardAppInfo& appInfo)
{
    //保存数据，发送报文
    if (!UnLoadCardBox(CARDBOXOP_DOWN))
    {
        return false;
    }
    //TODO liubo
    appInfo.clear();
    return true;
}

bool FormCardBox::HandBoxCard(TBoxCardBasicInfo &basicInfo, TBoxCardAppInfo &appInfo)
{
    if(m_bCardBoxUp)
    {
        return CardBoxUp(basicInfo, appInfo);
    }
    else
    {
        return CardBoxDown(basicInfo, appInfo);
    }
}

FormCardBox* getCardBox(QWidget *widget, QLayout *layout)
{
    static FormCardBox* p = NULL;
    if(p == NULL)
    {
        p = new FormCardBox(widget, layout);
    }
    return p;
}
