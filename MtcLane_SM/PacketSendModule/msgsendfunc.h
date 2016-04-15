#ifndef MSGSENDFUNC_H
#define MSGSENDFUNC_H

#include <QObject>
#include "CommonModule/MsgDefine.h"
#include <QBitArray>
#include "CommonModule/ilogmsg.h"
#include "CommonModule/laneutils_new.h"
#include <qfile.h>
class MsgSendFunc : public QObject
{
    Q_OBJECT
public:
    explicit MsgSendFunc(QObject *parent = 0);
    //计算报文长度
    quint32 GetTotalSize(TMsgToSend &Msg);
    //计算位元中指定位置的是否为1
    int Testbyte(const char *weiyuanbiao,int position);
    //打包报文
    bool PackMsg(const TMsgToSend *Msg,char* MsgBuff,int &nBufflen);
    //返回报文位元
    void GetWeiYuan(int nMsgID,char* weiyuanbiao);
    //打包参数请求报文
    bool PackParamReqMsg(int nMsgID,const TMsgReqParam msg,QByteArray pBuffer);
    //计算速度，传入 行驶里程（米）、行驶时间（秒）
    quint32 CalcVCSpeed(quint32 nDiseMeter,quint32 nDriveTime);
signals:

public slots:

};
MsgSendFunc *getMsgSendFun();
#endif // MSGSENDFUNC_H
