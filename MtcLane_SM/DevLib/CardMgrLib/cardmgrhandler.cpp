#include "cardmgrhandler.h"
#include <QFile>
#include <QDateTime>
#include "cardmgrdecoder.h"
#include "cardmgrencoder.h"
#include <QMutexLocker>
#include <QCoreApplication>
void LogMsg(const QString& sLog)
{
    static QFile file("./CardMgr.log");
    if(!file.isOpen())
    {
        if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            return;
        }
    }
    if(sLog.isEmpty())
    {
        file.close();
    }
    else
    {
        file.write(QObject::tr("%1 %2\n")
                   .arg(QDateTime::currentDateTime().toString("yy-MM-dd mm:dd:ss"))
                   .arg(sLog).toLocal8Bit());
    }
}

CardMgrHandler::CardMgrHandler(QObject *parent) :
    QThread(parent),m_port(NULL), m_bTerm(false), m_showObject(NULL),  m_cRsctl('0'), m_sendType(STNONE)
{
    for(int i = 0; i < CHANNELCOUNT; i++)
    {
        m_channels[i].nCardBoxId = 0x31 + i;
    }
}

bool CardMgrHandler::initImpl(const char *sCom, int nBps, QObject *pFunc)
{
    //如果线程在运行，则先停止线程
    if(isRunning())
    {
        m_bTerm = true;
        wait();
    }
    m_bTerm = false;
    QString portSettings = tr("%1,n,8,1").arg(nBps);
    //关闭已经打开的串口
    if(m_port != NULL)
    {
        m_port->close();
        delete m_port;
        m_port = NULL;
    }
    m_port = new QSerialPort(sCom, portSettings);
    //打开串口
    if(!m_port->open())
    {
        LogMsg(tr("卡机打开串口失败，参数[%1,%2]").arg(sCom).arg(portSettings));
        return false;
    }
    else
    {
        LogMsg(tr("卡机打开串口成功"));
    }
    //注册回调函数
    m_showObject = pFunc;
    //开启监听线程
    start();
    return true;
}

bool CardMgrHandler::upCardImpl(int nMgrIndex)
{
    return sendFrame(STCARDOUT, m_channels[nMgrIndex].nCardBoxId, 0);
}

bool CardMgrHandler::badCardImpl(int nMgrIndex)
{
    return sendFrame(STBADCARD, m_channels[nMgrIndex].nCardBoxId, 0);
}

bool CardMgrHandler::setCardCountImpl(int nMgrIndex, int nCount)
{
    return sendFrame(STSETCHANNEL, m_channels[nMgrIndex].nCardBoxId, nCount);
}

void CardMgrHandler::clearAllBadCardImpl()
{
    for(int i = 0; i < CHANNELCOUNT; i++)
    {
        badCardImpl(m_channels[i].nCardBoxId);
    }
}

bool CardMgrHandler::getBoxInfoImpl(int nBoxCode, int &nBoxId, int &nCount)
{
    if(nBoxCode >= 0 && nBoxCode < CHANNELCOUNT)
    {
        nBoxId = m_channels[nBoxCode].nCardBoxId;
        nCount = m_channels[nBoxCode].nCardCount;
        return true;
    }
    return false;
}

bool CardMgrHandler::getChannelInfoImpl(int nBoxCode, int &nBoxId, int &nCount, bool &bHasCardOn, char &curStatus)
{
    if(nBoxCode >= 0 && nBoxCode < CHANNELCOUNT)
    {
        nBoxId = m_channels[nBoxCode].nCardBoxId;
        nCount = m_channels[nBoxCode].nCardCount;
        bHasCardOn = m_channels[nBoxCode].HasCardOn;
        curStatus = m_channels[nBoxCode].Status;
        return true;
    }
    return false;
}

void CardMgrHandler::run()
{
    while(!m_bTerm)
    {
        //读取数据
        if(m_port->bytesAvailable() > 0)
        {
            QMutexLocker locker(&m_mutex);
            m_tmpData.append(m_port->read(m_port->bytesAvailable()));
            dealData();
        }
        else
        {
            msleep(100);
        }
    }
}

void CardMgrHandler::dealData()
{
    //是否将所有临时数据解析完成
    bool bFinish = false;
    //判断接收的数据是否合法
    QByteArray data;
    CMFRAMETYPE dataType = CMNONE;
    quint8 rsctl = 0xff;
    int nChannelIndex = 0;
    LogMsg(tr("before deal data is[%1]").arg(m_tmpData.data()));
    while(!bFinish)
    {
        //1.不存在出现>但是没有<的情况
        //2.如果出现多个>,说明缓冲区已经有多条数据
        //3.如果没有>了，说明缓冲区已经没有完整数据了，需要跳出循环
        if(!m_tmpData.contains('>'))
        {
            bFinish = true;
        }
        else
        {
            //查找第一个<和第一个>
            data = m_tmpData.mid(m_tmpData.indexOf('<') + 1, m_tmpData.indexOf('>') - m_tmpData.indexOf('<'));
            //解析
            bool rlt = CardMgrDecoder::Decode(data, CHANNELCOUNT, rsctl ,dataType, nChannelIndex, m_channels);
            //处理解析后的数据
            if(rlt)
            {
                response(rsctl, dataType);
                LogMsg(tr("notify deal data is [%1][%2]").arg(dataType).arg(nChannelIndex));
                //通知上层
                notify(dataType, nChannelIndex);
            }
            //从缓冲区中将数据移除
            m_tmpData.remove(0, m_tmpData.indexOf('>') + 1);
        }
        LogMsg(tr("after once deal data is [%1]").arg(m_tmpData.data()));
    }
}

void CardMgrHandler::notify(CMFRAMETYPE dataType, int nChannelIndex)
{
    //当收到正负应答信息时，为上次发送到卡机的回复，将上次发送的类型和正负应答信息一起返回
    if(dataType == CMTRUE || dataType == CMFALSE)
    {
        QCoreApplication::postEvent(m_showObject, new CardMgrEvent(dataType, -1, m_sendType));
    }
    else if(dataType == CMSTATUS)
    {
        QCoreApplication::postEvent(m_showObject, new CardMgrEvent(CMSTATUS, 0, STNONE ));
    }
    else
    //收到卡机发送信息,通知上层
    {
        QCoreApplication::postEvent(m_showObject, new CardMgrEvent(dataType, nChannelIndex, STNONE ));
    }

}

void CardMgrHandler::response(int rsctl, CMFRAMETYPE dataType)
{
    bool rlt = false;
    QByteArray data;
    switch(dataType)
    {
    case CMBOOTUP:
        m_sendType = STINIT;
        rlt = CardMgrEncoder::Encode(rsctl, STINIT, 0, 0, data);
        if(rlt)
        {
            m_port->write(data);
        }
        break;
    case CMCARDOUT:
        m_sendType = STTRUE;
        rlt = CardMgrEncoder::Encode(rsctl, STTRUE, 0, 0, data);
        if(rlt)
        {
            m_port->write(data);
        }
        break;
    case CMPUSH:
        m_sendType = STTRUE;
        rlt = CardMgrEncoder::Encode(rsctl, STTRUE, 0, 0, data);
        if(rlt)
        {
            m_port->write(data);
        }
        break;
    case CMCARDAWAY:
        m_sendType = STTRUE;
        rlt = CardMgrEncoder::Encode(rsctl, STTRUE, 0, 0, data);
        if(rlt)
        {
            m_port->write(data);
        }
        break;
    case CMCHANNEL:
        m_sendType = STTRUE;
        rlt = CardMgrEncoder::Encode(rsctl, STTRUE, 0, 0, data);
        if(rlt)
        {
            m_port->write(data);
        }
        break;
    default:
        break;
    }
}

void CardMgrHandler::updateRsctl()
{
    if(m_cRsctl < '9')
    {
        m_cRsctl++;
    }
    else
    {
        m_cRsctl = '0';
    }
}

bool CardMgrHandler::sendFrame(SENDTYPE sendType, int nMgrIndex, int nCount)
{
    QMutexLocker locker(&m_mutex);
    //封装信息帧
    QByteArray data;
    bool rlt = CardMgrEncoder::Encode(m_cRsctl, sendType, nMgrIndex, nCount, data);
    //发送
    if(rlt)
    {
        rlt = m_port->write(data) == data.length();
    }
    //设置等待回应
    m_sendType = sendType;
    //更新rsctl
    updateRsctl();
    return rlt;
}
