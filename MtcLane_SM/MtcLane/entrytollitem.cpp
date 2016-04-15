#include "entrytollitem.h"
#include "shiftsum.h"
EntryTollItem::EntryTollItem(QObject *parent) :
    TollItem(parent)
{
}

void EntryTollItem::FillYwWaste(QDateTime lpTime)
{
    int msgID,msgType;
    msgID = 1;
    msgType = FnYwType;
    ClearSendMsg(FMsgToSend);
    FillMsgToSend(msgID,msgType,FMsgToSend);
    FMsgToSend.VehInfo.VCnt = 1;
    DateTimeToChar14(lpTime,FMsgToSend.LaneInfo.OccurTime);
    FillHedgeMsg(FMsgToSend);
}

void EntryTollItem::SendYewuWasteMsg(int nInductCnt)
{

    TMsgShiftSum MsgShiftSum;
    int msgID,msgType;
    FMsgToSend.VehInfo.InductCnt = nInductCnt;
    msgID = ArCharToInt(FMsgToSend.BasicInfo.msgHead.cMsgID,sizeof(FMsgToSend.BasicInfo.msgHead.cMsgID));
    msgType = ArCharToInt(FMsgToSend.BasicInfo.msgHead.cMsgType,sizeof(FMsgToSend.BasicInfo.msgHead.cMsgType));
    if(m_nViolateHeldBack == LST_TYPE_VIOLATE_LOGOUT || m_nViolateHeldBack == LST_TYPE_VIOLATE)
    {
        FMsgToSend.DealInfo.SpInfo[DS_SECG1] = '1';
    }
    getShiftSum()->GetSmallShiftInfo(MsgShiftSum);
    if(!SaveMsg(SEND_BOTH,msgID,msgType,FMsgToSend,true,&MsgShiftSum,sizeof(MsgShiftSum)))
    {
        LogMsg("lane",tr("保存发卡业务报文失败"));
    }
    FLastMsgToSend = FMsgToSend;
    LogMsg("lane",tr("SendYewuWasteMsg保存发卡业务报文成功"));

}
