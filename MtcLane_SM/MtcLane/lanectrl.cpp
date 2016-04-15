#include "lanectrl.h"
#include "devicefactory.h"
#include "laneinfo.h"
#include "CommonModule/laneutils_new.h"
#include "exittollitem.h"
#include "entrytollitem.h"
#include "ParamModule/qparammgr.h"
#include "reqparam.h"
#include "reqtime.h"
#include "formmainwindow.h"

#include "shiftsum.h"
#include <QDebug>
#include "formtablewidget.h"
#include "formbase.h"
#include "PacketSendModule/sqlitedatasaver.h"
#include "formmainwindow.h"
#include "formvpr.h"
#include <QTime>
LaneCtrl::LaneCtrl(QObject *parent) :
    QObject(parent),m_pTollItem(NULL), m_nParamTimerId(0),
    m_nVerifyTimerId(0), m_nUpdateTimerId(0),
    m_nBigShiftTimerId(0), m_nDownloadCnt(0)
{
    connect(getDeviceFactory()->getIOBoard(), SIGNAL(InputChanged(quint32,quint8)),
            this, SLOT(IoInputChanged(quint32,quint8)));
    connect(MtcKeyFilter::instance(), SIGNAL(keyPressed(MtcKeyPropertity)),
            this, SLOT(KeyPressed(MtcKeyPropertity)));
    connect(getDeviceFactory()->getIOBoard(), SIGNAL(OutputChanged(int,int)),
            this ,SLOT(IoOutputChanged(int,int)));
//    getLaneInfo()->setShiftState((CLaneStatus)-1);
}

void LaneCtrl::IoInputChanged(quint32 dev, quint8 nStatus)
{
    switch(dev)
    {
    case DI_LoopBack:
        if(nStatus)
        {
            //播放语音
            QStringList soundFile;
            soundFile << "bye.mp3";
            QProcess::startDetached("/Program_Files/MtcLane/playsound.sh",soundFile);
            CarPassing();
            getShiftSum()->AddLoop(2);
            //当不是车队操作，且通过车辆数为0时，通行灯变红
            if(m_nState != LCTRL_Motor && m_vehQueue.VehCountAllowPass() == 0)
            {
                getDeviceFactory()->getIOBoard()->SetLaneLamp(0);
            }
        }
        else
        {
            CarPassed();
        }
        emit AllowCarPassing(m_vehQueue.VehCountAllowPass());
        break;
    case DI_LoopFront:
        //TODO
        if(nStatus)
        {
            //播放语音
            QStringList soundFile;
            soundFile << "welcome.mp3";
            QProcess::startDetached("/Program_Files/MtcLane/playsound.sh",soundFile);
            if(getLaneInfo()->getShiftState() == lsUnlogin ||getLaneInfo()->getShiftState() == lsNormalWorking )
            {
                if(getDeviceFactory()->getVideoCard()->Prepare())
                {
                    //显示抓拍图片
                    FormVpr* pVpr = NULL;
                    qDebug()<<"1";
                    getWidgetByType(emFormVpr,(QWidget**)&pVpr);
                    qDebug()<<"2";
                    if(pVpr != NULL)
                    {
                        pVpr->setCapImage(getDeviceFactory()->getVideoCard()->getFileName());
                    }
                    qDebug()<<"3";
                    //发送图片信息 modify by xiagnkai
//                    getTollItem()->SendImageMsg(getDeviceFactory()->getVideoCard()->getFileName());
                }
                qDebug()<<"4";
                //如果需要手动抓拍，需在此调用,只在上班时有效
                if(getLaneInfo()->getUseManualCapture() &&
                        getLaneInfo()->getShiftState() == lsNormalWorking)
                {
                    bool ret = getDeviceFactory()->getVpr()->Capture();
                    LogMsg("lane", tr("车牌识别手动抓拍，结果[%1]").arg(ret));
                }
            }
            getShiftSum()->AddLoop(1);

        }
        break;
    case DI_BarrierDown:
        getShiftSum()->AddBar(false);
        break;
    case DI_BarrierUp:
        getShiftSum()->AddBar(true);
        break;
    default:
        break;
    }
    if(getTollItem())
    {
        getTollItem()->SetIOStatus(0, dev, nStatus);
    }
}

void LaneCtrl::IoOutputChanged(int dev, int nStatus)
{
    if(getTollItem())
    {
        getTollItem()->SetIOStatus(1, dev, nStatus);
    }
}

void LaneCtrl::KeyPressed(const MtcKeyPropertity &keyPropertity)
{
    qDebug() << keyPropertity.func;
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeySimulate)
    {
        if(getDeviceFactory()->getIOBoard()->IsLockBar())
        {
            ::showWarning(tr("锁杆状态中，【模拟键】不可用"),true);
            return;
        }
        else if(getLaneCtrl()->CanDoNextCar()) //TODO_liubo
        {
            ::showWarning(tr("【模拟键】不可用"),true);
            return;
        }
        else
        {
            //判断是否启用了模拟键
            if(getLaneInfo()->getUseSimulate())
            {
                m_vehQueue.CardOnBackLoop();
                getDeviceFactory()->getIOBoard()->SimBackLoop();
            }
            else
            {
                ::showWarning(tr("模拟键被禁用"));
            }
        }
    }
}

void LaneCtrl::CarPassing()
{
    //拖车处理
    if(m_bWaitingCarPass)
    {
        if(m_nTractIndex > 0)
        {
            m_nTractIndex--;
        }
        if(m_nTractIndex == 0)
        {
            Reset();
        }
        m_vehQueue.CardOnBackLoop();
        return ;
    }
    //正常业务
    if(m_vehQueue.VehCountAllowPass() > 0 && LCTRL_Normal == m_nState)
    {
        m_vehQueue.CardOnBackLoop();
        Reset();
        return;
    }
    //车队
    if(m_nState == LCTRL_Motor)
    {
        //TODO车辆计数加1
        Reset();
        return;
    }
    //TODO:处理闯关

}

bool LaneCtrl::CanDoNextCar()
{
    if(m_nState == LCTRL_Normal)
    {
        return m_vehQueue.VehCountAllowPass() == 0
                || getLaneInfo()->IsLaptop()
                || getDeviceFactory()->getIOBoard()->IsLockBar();
    }
    else if(m_nState == LCTRL_Tract)
    {
        return 2 > m_vehQueue.VehCountAllowPass();
    }
    return false;
}

bool LaneCtrl::GetAllowLogOut()
{
    return ((0 == m_vehQueue.VehCountAllowPass()) || getLaneInfo()->IsLaptop());
}

bool LaneCtrl::GetAllowReverse(int &nCardType, quint32 &dwPhysicalCardId, QString &sCardId)
{
    int nLaCardType = 0;
    if(getLaneInfo()->getLaneType() == LaneType_MtcIn)
    {
        if(GetVehAllowPass() <= 0)
        {
            return false;
        }
        nLaCardType = m_pTollItem->GetMsgToSend().ICCInfo.CardType;
        sCardId = QString::fromLocal8Bit(m_pTollItem->GetMsgToSend().ICCInfo.CardID,
                                         qMin((int)strlen(m_pTollItem->GetMsgToSend().ICCInfo.CardID), 16));
        dwPhysicalCardId = m_pTollItem->GetMsgToSend().ICCInfo.CardSN;
        switch(nLaCardType)
        {
        case CT_StoreCard:
        case CT_TestStoreCard:
        case CT_ServantCard:
        case CT_POStoreCard:
        case CT_BJCard:
            nCardType = CARD_TYPE_STORE_CARD;
            break;
        case CT_TallyCard:
        case CT_TestTallyCard:
        case CT_POTallyCard:
            nCardType = CARD_TYPE_TALLY_CARD;
            break;
        case CT_TollOperCard:
        case CT_MonOperCard:
        case CT_MainOperCard1:
        case CT_MainOperCard2:
        case CT_HighOperCard:
            nCardType = CARD_TYPE_OP_CARD;
            break;
        case CT_PassCard:
        case CT_TestPassCard:
        case CT_YBMPassCard:
            nCardType = CARD_TYPE_PASS_CARD;
            break;
        case CT_PaperCard:
            nCardType = CARD_TYPE_PAPER;
            break;
        }
    }
    return sCardId.length() > 0;
}

void LaneCtrl::DoMotor(bool bStart, int nVct)
{
    if(bStart)
    {
        m_nState = LCTRL_Motor;
    }
    else
    {
        m_nState = LCTRL_Normal;
    }
    m_pTollItem->SetOccurTime(QDateTime::currentDateTime());
    if(bStart)
    {
        getDeviceFactory()->getIOBoard()->SetOneVehPass();
        if(getLaneInfo()->getControlDevType() == 1)
        {
            if(getDeviceFactory()->getFD() != NULL)
            {
                getDeviceFactory()->getFD()->SetPassLight(true);
            }
        }
        m_pTollItem->SendMotorBeginMsg();
    }
    else
    {
        m_vehQueue.SetRefusePass(true);
        m_pTollItem->ClearSavedInfo();
        //liujian
        //m_pTollItem->SendMotorCarPassMsg(nVct);
        m_pTollItem->SendMotorEndMsg(nVct);
    }
}

void LaneCtrl::MotorPass()
{
    SavePic(QDateTime::currentDateTime(), 0);
    SavePic(QDateTime::currentDateTime(), 1);
    m_pTollItem->SetOccurTime(QDateTime::currentDateTime());
    m_pTollItem->SendMotorCarPassMsg(1);
}

void LaneCtrl::DoTract()
{
    if(m_nState == LCTRL_Motor)
    {
        return ;
    }
    //第一次牵引按键
    if(LCTRL_Normal == m_nState)
    {
        if(GetVehAllowPass() == 0)
        {
            m_nTractIndex = 0;
            m_nState = LCTRL_Tract;
            return;
        }
    }
    //再次按牵引，取消牵引处理.如果两辆车已经处理完则按牵引键无效
    else if(m_nState == LCTRL_Tract)
    {
        if(m_nTractIndex < 2)
        {
            m_nState = LCTRL_Normal;
            if(m_nTractIndex == 1)
            {
                //第一辆车处理完成后后取消牵引车,只执行抬杆操作,拖车索引数不变
                m_bWaitingCarPass = true;
                SetAllowPass(1);
                //getLaneInfo()->setAllowPass(1);//TODO_liubo
            }
            return;
        }
        else
        {
            m_nState = LCTRL_Normal;
            m_bWaitingCarPass = true;
            SetAllowPass(m_nTractIndex);
            //getLaneInfo()->setAllowPass(m_nTractIndex);//TODO_liubo
        }
    }
}

void LaneCtrl::CarPassed()
{
    if(m_nState == LCTRL_Motor)
    {
        return;
    }
    getLaneInfo()->subAllowPass();
    if(getDeviceFactory()->getIOBoard()->IsLockBar())
    {
        return;
    }
    if(m_vehQueue.VehCountAllowPass() == 0)
    {
        m_vehQueue.SetRefusePass();
    }
}

int LaneCtrl::GetVehAllowPass()
{
    return m_vehQueue.VehCountAllowPass();
}

bool LaneCtrl::IsAllowTract()
{
    if(getLaneInfo()->getLaneType() != LaneType_MtcOut)
    {
        return false;
    }
    if(LCTRL_Motor == m_nState)
    {
        return false;
    }
    if(m_bReSendCard)
    {
        return false;
    }
    if(GetVehAllowPass() > 0)
    {
        return false;
    }
    return true;
}

bool LaneCtrl::IsAllowReprint()
{
    if(GetVehAllowPass() > 0)
    {
        return true;
    }
    if(LCTRL_Normal != m_nState)
    {
        return true;
    }
    return false;
}

bool LaneCtrl::IsTract()
{
    return m_nState == LCTRL_Tract;
}

void LaneCtrl::ConfirmComplete(int nAllowCount)
{
    if(getDeviceFactory()->getFD() != NULL)
    {
        getDeviceFactory()->getFD()->ClearAll();
    }
    if(m_nState == LCTRL_Tract)
    {
        TractComplete();
        if(getLaneInfo()->getLaneType() == LaneType_MtcOut)
        {
            //TODO:出口计重收费，删除首车信息
            getWtDataMgr()->Delete(1);
        }
        //清空车牌
        getDeviceFactory()->getVpr()->ClearVPRResult();
        return;
    }
    if(!m_bReSendCard && !(m_pTollItem->GetMsgToSend().DealInfo.SpInfo[18] == '1'))
    {
        SetAllowPass(1);
        //getLaneInfo()->setAllowPass(1);//TODO_liubo
        CompleteTransact(1);
        if(getLaneInfo()->getLaneType() == LaneType_MtcOut)
        {
            //TODO:出口计重收费，删除首车信息
            getWtDataMgr()->Delete(1);
        }
        getDeviceFactory()->getVpr()->ClearVPRResult();
    }
    else
    {
        if(getLaneInfo()->getLaneType() == LaneType_MtcOut)
        {
            //TODO:出口计重收费，删除首车信息
            getWtDataMgr()->Delete(1);
        }
    }
    Reset();
}

void LaneCtrl::CompleteTransact(int nInductCount)
{
    m_pTollItem->SendYewuWasteMsg(nInductCount);
    Reset();
}

bool LaneCtrl::Complete(const QDateTime &OccurTime, bool bFinished)
{
    int nAllowCount = 0;
    SavePic(QDateTime::currentDateTime(), 0);
    SavePic(QDateTime::currentDateTime(), 1);
    if(m_bReSendCard)
    {
        nAllowCount = 0;
    }
    else
    {
        nAllowCount = 1;
    }
    m_pTollItem->Complete(OccurTime, nAllowCount, bFinished);
    return true;

}

bool LaneCtrl::TractComplete()
{
    m_nTractIndex++;
    m_pTollItem->SendYewuWasteMsg(1);
    return true;
}
void LaneCtrl::setRepairCard(bool ret)
{
    m_bReSendCard = ret;
}
void LaneCtrl::CheckLoadNewParam()
{
    getParamMgr()->LoadOutUseTimeParam();
    //TODO:显示费率版本信息
}

void LaneCtrl::CheckDownLoadParam()
{
    //QDebug() << tr("开始下载新版本参数，下载开始");
    killTimer(m_nUpdateTimerId);
    m_nDownloadCnt++;
    int nRet = m_nDownloadCnt % 5;
    switch(nRet)
    {
    case 1:
        getReqParam()->AskFile(cfCardBList, true);
        getReqParam()->AskFile(cfEnd, false);
        break;
    case 2:
        getReqParam()->AskFile(cfVBList, true);
        getReqParam()->AskFile(cfEnd, false);
        break;
    case 3:
        getReqParam()->AskFile(cfVGList, true);
        getReqParam()->AskFile(cfEnd, false);
        break;
    default:
        break;
    }

    if(m_nDownloadCnt % 30 == 0)
    {
        getReqParam()->AskFile(cfCardRange, true);
        getReqParam()->AskFile(cfEnd, false);
    }

    if(m_nDownloadCnt > 3000000)
    {
        m_nDownloadCnt = 0;
    }

    m_nUpdateTimerId = startTimer(120000);
}

void LaneCtrl::CheckVerifyTime()
{
    killTimer(m_nVerifyTimerId);
    ////qDebug() << tr("开始进行与服务端校时");
    getReqTime()->ReqNewTime(getLaneInfo()->getDataSrvIp());
    StartVerifyTimer();
}

void LaneCtrl::CheckUpdateShift()
{
    killTimer(m_nBigShiftTimerId);

    getLaneInfo()->UpdateShiftDateWhenUnlogin();

    StartBigShiftTimer();
}
//void LaneCtrl::CheckDelPic(int saveDays)
//{
//    //TODO_Time
//    QTime time;
//    time.start();

//    LogMsg("lane",tr("CheckDelPic:开始"));
//    QDir dir;
//    QString dirName = QDateTime::currentDateTime().addDays(-saveDays).toString("MMdd");
//    QString tmpDirName;
//    QStringList fileList;
//    QStringList picFileList;
//    QFile file;
//    QDir tmpDir;
//    dir.setPath(getLaneInfo()->getPicPath());
//    QList<QFileInfo> *fileInfo = new QList<QFileInfo>(dir.entryInfoList(fileList));
//    for(int i = 0;i<=fileInfo->count()-1;i++)
//    {
//        if(!fileInfo->at(i).isHidden() &&fileInfo->at(i).isDir()&& fileInfo->at(i).fileName().toInt() >0&& fileInfo->at(i).fileName().toInt() <= dirName.toInt())
//        {
//            tmpDirName = getLaneInfo()->getPicPath()+fileInfo->at(i).fileName();
//            tmpDir.setPath(tmpDirName);
//            QList<QFileInfo> *picFileInfo = new QList<QFileInfo>(tmpDir.entryInfoList(picFileList));
//            for(int i = 0;i<= picFileInfo->count() -1;i++)
//            {
//                if(picFileInfo->at(i).isFile())
//                {
//                    file.setFileName(tmpDirName+"/"+picFileInfo->at(i).fileName());
//                    if(file.isOpen())
//                        file.close();
//                    file.remove();
//                }
//            }
//            dir.rmdir(tmpDir.absolutePath());
//        }
//    }
//    LogMsg("lane",tr("CheckDelPic:结束"));
//    int nTime = time.elapsed();
//    float f = nTime/1000.0;
//    QString tr_nTime = QString("%1").arg(f);
//}
void LaneCtrl::DoLockBar()
{
    if(m_nState == LCTRL_Motor)
    {
        return;
    }
    if(getDeviceFactory()->getIOBoard()->IsLockBar())
    {
        getDeviceFactory()->getIOBoard()->UnLockBar();
        m_vehQueue.SetRefusePass(true);
        emit AllowCarPassing(0);
        //TODO:发送车道事件报文 锁杆解除
        DoLaneEvent(LET_LockBar);
    }
    else
    {
        getDeviceFactory()->getIOBoard()->SetOneVehPass();
        if(getLaneInfo()->getControlDevType() == 1)
        {
            if(getDeviceFactory()->getFD() != NULL)
            {
                getDeviceFactory()->getFD()->SetPassLight(true);
            }
        }
        getDeviceFactory()->getIOBoard()->LockBar();
        //TODO:发送车道事件报文 锁杆开始
        DoLaneEvent(LET_RemoveBar);
    }
}

void LaneCtrl::DoReverse()
{
    m_vehQueue.SetRefusePass(true);
    m_pTollItem->SetOccurTime(QDateTime::currentDateTime());
    m_pTollItem->SendHedgeMsg();
    m_pTollItem->ClearSavedInfo();
    //增加卡盒卡数
    getDataFileMgr()->getCardBoxInfo().nCardCount++;
    emit AllowCarPassing(0);//TODO_liubo
}

bool LaneCtrl::DoReprintInvoice()
{
    bool ret = getDeviceFactory()->getPrinter()->RePrint();
    if(ret)
    {
        LogMsg("lane", tr("重新打印票据成功"));
    }
    else
    {
        LogMsg("lane", tr("重新打印票据失败"));
    }
    ExitTollItem *pTmp = static_cast<ExitTollItem*>(m_pTollItem);

    pTmp->SetDeleteInvoiceType(DIT_REPRINT);
    pTmp->ReprintInvoiceMsg();
    return ret;
}

void LaneCtrl::DoViolate(int nViolateType)
{
    if(nViolateType == -1)
    {
        return;
    }
    m_pTollItem->SetViolateHeldBack(nViolateType);
    if(20 == nViolateType || 21 == nViolateType)
    {
        Reset();
        if(getLaneInfo()->getLaneType() == LaneType_MtcOut)
        {
            //计重删除1
            getWtDataMgr()->Delete(1);
        }
        getDeviceFactory()->getVpr()->ClearVPRResult();
    }
    else
    {
        if(nViolateType == 31)
        {
            //误报警处理
            getLaneCtrl()->DoLaneEvent(LET_ViolateFalseAlarm);
        }
        else if(nViolateType == 32)
        {
            //闯关被拦截
            getLaneCtrl()->DoLaneEvent(LET_ViolateHeldBack);
        }
    }
}

TollItem *LaneCtrl::getTollItem()
{
    return m_pTollItem;
}

void LaneCtrl::Init()
{
    if(getLaneInfo()->getLaneType() % 2 == 0)
    {
        m_pTollItem = new ExitTollItem();
    }
    else
    {
        m_pTollItem = new EntryTollItem();
    }
    qDebug() << "lane ctrl init..";
}

void LaneCtrl::DoLaneEvent(TLaneEventType nLaneEvent)
{
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    switch(nLaneEvent)
    {
    case LET_None:
    case LET_UsesFrontLoop:
    case LET_NotUsesFrontLoop:
        getLaneInfo()->setUseFrontLoop(!getLaneInfo()->getUseFrontLoop());
        break;
    case LET_UsesSimulate:

    case LET_NotUsesSimulate:
        getLaneInfo()->setUseSimulate(!getLaneInfo()->getUseSimulate());
        break;
    case LET_UsesVLP:
    case LET_NotUsesVLP:
        getLaneInfo()->setUsePlate(!getLaneInfo()->getUsePlate());
        break;
    case LET_UsesWeight:
    case LET_NotUsesWeight:
        getLaneInfo()->setUseWeight(!getLaneInfo()->getUseWeight());
        //设置费显工作模式
        if(getDeviceFactory()->getFD() != NULL)
        {
            getDeviceFactory()->getFD()->SetMode(getLaneInfo()->getLaneType(), getLaneInfo()->getUseWeight());
        }
    case LET_UFree:
        pExit->setUFree(true);
        break;
    default:
        break;
    }
    pExit->SetOccurTime(QDateTime::currentDateTime());
    pExit->SendLaneEvent(nLaneEvent);
}

void LaneCtrl::StartNewParamTimer()
{
    m_nParamTimerId = startTimer(30000);
}

void LaneCtrl::StartDownLoadTimer()
{
    m_nUpdateTimerId = startTimer(120000);
}

void LaneCtrl::StartVerifyTimer()
{
    m_nVerifyTimerId = startTimer(1800000);
}

void LaneCtrl::StartBigShiftTimer()
{
    m_nBigShiftTimerId = startTimer(60000);
}
void LaneCtrl::StartDelDBDataTimer()
{
    //三小时执行一次数据清除
    m_nDelDBDataTimerId = startTimer(10800*1000);
}
void LaneCtrl::StartDelPicDataTimer()
{
    //m_nDelPicDataTimerId = startTimer(10800*1000);
    //m_nDelPicDataTimerId = startTimer(10000);
}

void LaneCtrl::startUpdateTimer()
{
    m_updateTimerId = startTimer(60000);
}
void LaneCtrl::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_nParamTimerId)
    {
        CheckLoadNewParam();
    }
    else if(event->timerId() == m_nVerifyTimerId)
    {
        CheckVerifyTime();
    }
    else if(event->timerId() == m_nUpdateTimerId)
    {
        CheckDownLoadParam();
    }
    else if(event->timerId() == m_nBigShiftTimerId)
    {
        CheckUpdateShift();
    }
    //liujian
    else if(event->timerId() == m_nDelDBDataTimerId)
    {
        LogMsg("lane",tr("定时器启动删除已上传数据"));
        getDataSaver()->DoDelHistoryData(getLaneInfo()->getSqlSaveDay());
    }
    else if(event->timerId() == m_updateTimerId)
    {
        autoClose();
    }
}

void LaneCtrl::autoClose()
{
    if(getLaneInfo()->getShiftState() != lsNormalWorking)
    {
        //判断目录下是否有新文件
        if(QFile::exists("./update/MtcLane"))
        {
            if(m_updateTimerId != 0)
            {
                killTimer(m_updateTimerId);
            }
            getLaneInfo()->setCloseType(emRestartPC);

            getTollItem()->SendAppMsg(false);
            if(getMainWindow() != NULL)
            {
                getMainWindow()->close();
            }
            LogMsg("lane", tr("检查到新版软件存在，自动关闭软件进行软件升级"));
        }
    }
}

void LaneCtrl::Reset()
{
    if(m_nState != LCTRL_Motor && m_nTractIndex == 2)
    {
        m_nState = LCTRL_Normal;
        m_nTractIndex = 0;
    }
    m_bWaitingCarPass = false;
    m_bReSendCard = false;
}

bool LaneCtrl::SavePic(const QDateTime &capTime, int nPicType)
{
    QString sFileName;
    QString sRelativePath = m_pTollItem->FormatPicFileName(capTime, nPicType, sFileName);
    QString sAbsolutePath = getLaneInfo()->getPicPath() + sRelativePath;
    if(!ForceDirectories(sAbsolutePath.toLocal8Bit().data()))
    {
        LogMsg("lane", tr("图像保存目录[%1]，创建失败！").arg(sAbsolutePath));
        return false;
    }
    QString sFullName = sAbsolutePath + "/" + sFileName;
    if(nPicType == 0)
    {
        bool bRet = getDeviceFactory()->getVideoCard()->SaveAsJpgFile(sFullName);
        if(!bRet)
        {
            return false;
        }
    }
    else
    {
        if(getDeviceFactory()->getVpr()->GetCurrentResult() != NULL)
        {
            int nIndex = getDeviceFactory()->getVpr()->GetCurrentResult()->nIndex;
            bool bRet = getDeviceFactory()->getVpr()->GetBigImage(nIndex, sFullName);
            if(!bRet)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    m_pTollItem->SetPicPath(sFullName, capTime, nPicType);
    LogMsg("lane",tr("[SavePic]图像保存：[%1]").arg(sFullName));
    return true;
}

void LaneCtrl::SetAllowPass(int nCount)
{
    m_vehQueue.SetAllowPass(nCount);
    emit AllowCarPassing(m_vehQueue.VehCountAllowPass());
}

LaneCtrl* getLaneCtrl()
{
    static LaneCtrl laneCtrl;
    return &laneCtrl;
}

bool LaneCtrl::getRepairCard()
{
    return m_bReSendCard;
}
