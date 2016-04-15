#include "MsgDefine.h"
#include "LaneType.h"
#include "laneutils_new.h"
void FillMsgHead(int MsgID, int MsgType, int ReceivedID,int laneId, int nOrgCode, TMsgHeadInfo &MsgHead)
{
    IntToArChar(MsgHead.cMsgID,sizeof(MsgHead.cMsgID),MsgID);
    IntToArChar(MsgHead.cMsgType,sizeof(MsgHead.cMsgType),MsgType);
    MsgHead.ZipType = '0';
    IntToArChar(MsgHead.Sender,9, nOrgCode*1000+laneId);
    IntToArChar(MsgHead.Receiver,9,ReceivedID);
    DateTimeToChar14(QDateTime::currentDateTime(), MsgHead.SendTime);
    memset(MsgHead.VerifyCode, 0, sizeof(MsgHead.VerifyCode));
}
