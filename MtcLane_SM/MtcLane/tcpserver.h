#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "CommonModule/MsgDefine.h"
#include "CommonModule/LaneType.h"
#pragma pack(push,1)
struct TRecvFileInfo
{
  int FileLen;
  char FileName[128];
  char* FileContent;
};
#pragma pack(pop)

const int MSG_LISTEN_PORT = 7788;

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);
    
signals:

public slots:
    void init(quint32 nPort);
    void DoMsg();
protected:
    void SendBasicResponseMsg(quint8 nRes);
    bool DoRecvParamFile(const TMsgHeadInfo& msgHead);
    bool DoAdjustTime(const TMsgHeadInfo& msgHead);
    bool DoReceiveFile(const TMsgHeadInfo& msgHead);
    bool DoReceiveCommand(const TMsgHeadInfo& msgHead);
    bool DoReceiveMessage(const TMsgHeadInfo& msgHead);
    bool DoUpdateMonIP(const TMsgHeadInfo& msgHead);
private slots:
    //有一个新的连接
    void newConnection();
    //读取
    void readyRead();
    //发送完成
    void byteWritten(qint64 nLen);
    //关闭连接
    void socketClose();
private:
    QTcpServer m_tcpSvr;
    QTcpServer m_msgSvr;
    QTcpSocket* m_tcpSocket;
    char * m_pMsgRev;
    QString m_sPeerIp;
};

QString GetFileNameFromID(const quint32 dwImgId, const QString& sPicPath, const QString& sFileExt);

TcpServer* getLaneMsgServer();

#endif // TCPSERVER_H
