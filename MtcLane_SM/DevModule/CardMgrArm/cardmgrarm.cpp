#include "cardmgrarm.h"
#include <QVector>
#include "../../DevLib/CardMgrLib/datadef.h"

const int CMCHANNELNONE = -1;
const int CMCHANNEL1 = 0;
const int CMCHANNEL2 = 1;
const int CMCHANNEL3 = 2;
const int CMCHANNEL4 = 3;

//初始化函数
typedef bool (*CMInit)(const char* sCom, int nBps, QObject* pFunc);

//出卡
typedef bool (*CMUpCard)(int nMgrIndex);

//坏卡操作
typedef bool (*CMBadCard)(int nMgrIndex);

//设置卡总数
typedef bool (*CMSetCardCount)(int nMgrIndex, int nCardCount);

//清除所有坏卡
typedef void (*CMClearAllBadCard)();

//获得卡盒信息
typedef bool (*CMGetBoxInfo)(int nBoxCode, int& nBoxId, int& nCount);

//获得频道信息
typedef bool (*CMGetChannelInfo)(int nBoxCode, int& nBoxId, int& nCount, bool& bHasCardOn, char& curStatus);

CMInit funcInit = NULL;
CMUpCard funcUpCard = NULL;
CMBadCard funcBadCard = NULL;
CMSetCardCount funcSetCardCount = NULL;
CMClearAllBadCard funcClearAllBadCard = NULL;
CMGetBoxInfo funcGetBoxInfo = NULL;
CMGetChannelInfo funcGetChannelInfo = NULL;

CardMgrArm::CardMgrArm(const QString & sDevName, QObject *parent) :
    DevBaseInterface(sDevName, parent),
    m_upCurChannel(CMCHANNELNONE),
    m_downCurChannel(CMCHANNELNONE),
    m_bBootUp(false)
{
}

bool CardMgrArm::LoadFunction()
{
    //防止重复加载
    if(m_bDriverLoaded)
    {
        return true;
    }

    funcInit = (CMInit)m_qLibrary.resolve("Init");
    funcUpCard = (CMUpCard)m_qLibrary.resolve("UpCard");
    funcBadCard = (CMBadCard)m_qLibrary.resolve("BadCard");
    funcSetCardCount = (CMSetCardCount)m_qLibrary.resolve("SetCardCount");
    funcClearAllBadCard = (CMClearAllBadCard)m_qLibrary.resolve("ClearAllBadCard");
    funcGetBoxInfo = (CMGetBoxInfo)m_qLibrary.resolve("GetBoxInfo");
    funcGetChannelInfo = (CMGetChannelInfo)m_qLibrary.resolve("GetChannelInfo");
    if( NULL == funcInit ||
            NULL == funcBadCard ||
            NULL == funcClearAllBadCard ||
            NULL == funcGetBoxInfo ||
            NULL == funcGetChannelInfo ||
            NULL == funcSetCardCount ||
            NULL == funcUpCard)
    {
        LogMsg("lane", tr("获取卡机动态库[%1]中的一个或多个函数错误！").arg(m_sLibName));
        return false;
    }

    m_bDriverLoaded = true;
    return true;
}


bool CardMgrArm::Init()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }
    if(!funcInit(m_sConStr1.toLocal8Bit().data(), m_sConStr2.toInt(), this))
    {
        LogMsg("lane", tr("卡机设备打开失败,ConnStr1[%1], ConnStr2[%2]").arg(m_sConStr1).arg(m_sConStr2));
        return false;
    }
    LogMsg("lane", tr("卡机设备打开成功"));
    return true;
}

void CardMgrArm::handleCardMgrInfo(CardMgrEvent *event)
{
    //解析index内容
    QVector<int> channels;
    for(int i = 0; i < CHANNELCOUNT; i++)
    {
        if(event->getIndex() & (0x1 << i))
        {
            channels.push_back(i);
        }
    }
    //发送信息
    switch(event->getMgrEventType())
    {
    case CMBOOTUP:
        ////qDebug() << "接收到上电信息";
        emit StatusChanged(DEV_STATUS_Offline);
        break;
    case CMCARDAWAY:
        ////qDebug() << "接收到卡片取走信息";
        if(channels.count() > 0)
        {
            emit cardAway(channels.first());
        }
        break;
    case CMCARDOUT:
        //qDebug() << "接收到出卡信息" << channels.count();
        if(channels.count() > 0)
        {
            emit cardOut(channels.first());
        }
        break;
    case CMCHANNEL:
        //qDebug() << "接收到频道信息";
        emit freshInfo();
        break;
    case CMPUSH:
        //qDebug() << "接收到按键取卡信息";
        if(channels.count() > 0)
        {
            if(isUpPosition(channels.first()))
            {
                m_upCurChannel = channels.first();
                emit buttonPush(POSITIONUP);
            }
            else if(isDownPosition(channels.first()))
            {
                m_downCurChannel = channels.first();
                emit buttonPush(POSITIONDOWN);
            }
        }
        break;
    case CMSTATUS:
        //qDebug() << "接收到状态信息";
        foreach(int n, channels)
        {
            if(isUpPosition(n))
            {
                m_upCurChannel = n;
            }
            if(isDownPosition(n))
            {
                m_downCurChannel = n;
            }
        }
        emit freshInfo();
        break;
    default:
        break;
    }
}


bool CardMgrArm::Close()
{
    return true;
}


bool CardMgrArm::ReleaseFunction()
{
    m_bDriverLoaded = false;
    funcBadCard = NULL;
    funcClearAllBadCard = NULL;
    funcGetBoxInfo = NULL;
    funcGetChannelInfo = NULL;
    funcInit = NULL;
    funcUpCard = NULL;
    funcSetCardCount = NULL;
    return false;
}


void CardMgrArm::customEvent(QEvent *event)
{
    CardMgrEvent* pEvent = static_cast<CardMgrEvent*>(event);
    if(pEvent == NULL)
    {
        return;
    }
    //如果接到正应答信息，则根据发送的类型发送信号,负应答信息暂不处理
    if(pEvent->getMgrEventType() == CMTRUE)
    {
        switch(pEvent->getSendType())
        {
        //初始化成功
        case STINIT:
            m_nStatus = DEV_STATUS_OK;
            m_bBootUp = true;
            emit StatusChanged(DEV_STATUS_OK);
            break;
            //设置频道信息成功
        case STSETCHANNEL:
            break;
        }
    }

    //如果收到卡机主动上送的消息，则分别进行处理
    if(pEvent->getMgrEventType() > CMFALSE)
    {
        handleCardMgrInfo(pEvent);
    }
}


void CardMgrArm::SetCardCount(int nMgrIndex, int nCardCount)
{
    if(funcSetCardCount != NULL)
    {
        funcSetCardCount(nMgrIndex, nCardCount);
    }
}

void CardMgrArm::GetChannelInfo(int nBoxCode, int &boxIndex, int &nCount, bool &bHasCardOn, char &curStatus)
{
    if(funcGetChannelInfo != NULL)
    {
        funcGetChannelInfo(nBoxCode, boxIndex, nCount, bHasCardOn, curStatus);
    }
}

void CardMgrArm::GetBoxInfo(int nBoxCode, int &nCount)
{
    if(funcGetBoxInfo != NULL)
    {
        int nBoxId = 0;
        funcGetBoxInfo(nBoxCode,nBoxId, nCount);
    }
}


void CardMgrArm::UpCard(int nMgrIndex)
{
    if(funcUpCard != NULL)
    {
        funcUpCard(nMgrIndex);
    }
}

void CardMgrArm::BadCard(int nMgrIndex)
{
    if(funcBadCard != NULL)
    {
        funcBadCard(nMgrIndex);
    }
}

void CardMgrArm::ClearAllBadCard()
{
    if(funcClearAllBadCard != NULL)
    {
        funcClearAllBadCard();
    }
}
