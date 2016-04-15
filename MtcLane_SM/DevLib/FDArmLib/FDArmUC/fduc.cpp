#include "fduc.h"
#include <QDebug>
//#include "MiniLog/minilog.h"
//using namespace mini;
FDUC g_fd;

quint8 GetXOR(const quint8 *pBuf, int nBufLen)
{
    if(!pBuf)
        return 0;
    quint8 tmp=0;
    for(int i=0;i<nBufLen;++i)
    {
        tmp = tmp ^(quint8)pBuf[i];
    }
    return tmp;
}

FDUC::FDUC(QObject *parent) :
    FDInterface(parent), m_serialPort(NULL)
{
    ::setInstance(this);
    //mini::SetLogName("FD.log");
}

int FDUC::openDevImpl(char *lpszConnPara)
{
    int ret = -1;
    m_mutex.lock();
    //LogMsg("begin open uc fare display");
    if(m_serialPort != NULL)
    {
        //LogMsg("serial port is open, close first");
        if(m_serialPort->isOpen())
        {
            m_serialPort->close();
        }
        delete m_serialPort;
    }
    //LogMsg(tr("open serial port [%1]").arg(lpszConnPara));
    m_serialPort = new QSerialPort(lpszConnPara);
    if(!m_serialPort->open())
    {
        m_serialPort->close();
        delete m_serialPort;
        m_serialPort = NULL;
        //LogMsg("open serial port error");
    }
    else
    {
        ret = 0;
        //LogMsg("open serial port success");
    }
    m_mutex.unlock();
    return ret;
}

int FDUC::setModeImpl(int nLaneType, bool bTollByWeight)
{
    m_laneType = nLaneType;
    m_tollByWeight = bTollByWeight;
    //LogMsg(tr("set uc fare display, lanetype[%1], toll by weight[%2]").arg(m_laneType).arg(m_tollByWeight));
    return 0;
}

int FDUC::closeDevImpl()
{
    m_mutex.lock();
    //LogMsg("begin close uc fare display");
    if(m_serialPort != NULL)
    {
        if(m_serialPort->isOpen())
        {
            //LogMsg("close serial port");
            m_serialPort->close();
        }
        delete m_serialPort;
        m_serialPort = NULL;
        //LogMsg("release serial port");
    }
    m_mutex.unlock();
    return 0;
}

int FDUC::vehClassImpl(int nVehClass)
{
    int ret = -1;
    if((nVehClass > 0 && nVehClass < 5) || (nVehClass > 10 && nVehClass < 16))
    {
        //LogMsg("begin encode veh class frame");
        QByteArray content;
        content.append(UTF8toGB2312(QString::fromUtf8("车型")));
        content.append(tr("%1").arg(nVehClass % 10, 10, 10, QLatin1Char(0x20)));
        content.append(UTF8toGB2312(QString::fromUtf8(nVehClass > 10 ? "货" : "客")));
        createLine(content, 0);
        createCommand(content);
        ret = 0;
    }
    else
    {
        //LogMsg(tr("veh class [%1] is invalid").arg(nVehClass));
    }
    return ret;
}

int FDUC::vehWeightImpl(int nTotalWeight, int nWeightLimit, int nOverweightRate)
{
    int ret = -1;
    if(QString::number(nTotalWeight).length() <= 10 &&
            QString::number(nWeightLimit).length() <= 10)
    {
        //LogMsg("begin encode veh weight frame");
        QByteArray content;
        content.append(UTF8toGB2312(QString::fromUtf8("总重")));
        content.append(tr("%1kg").arg(nTotalWeight, 10, 10, QLatin1Char(0x20)));
        createLine(content, 2);
        createCommand(content);
        content.clear();
        content.append(UTF8toGB2312(QString::fromUtf8("超重")));
        content.append(tr("%1kg").arg(nWeightLimit, 10, 10, QLatin1Char(0x20)));
        createLine(content, 3);
        createCommand(content);
        ret = 0;
    }
    else
    {
        //LogMsg(tr("veh weight [%1] & over weight [%2] is invalid").arg(nTotalWeight).arg(nWeightLimit));
    }
    return ret;
}

int FDUC::fareImpl(int nFare)
{
    int ret = -1;
    if(QString::number(nFare / 100).length() <= 10)
    {
        //LogMsg("begin encode fare frame");
        QByteArray content;
        content.append(UTF8toGB2312(QString::fromUtf8("金额")));
        content.append(UTF8toGB2312(tr("%1元").arg(nFare / 100, 10, 10, QLatin1Char(0x20))));
        createLine(content, 1);
        createCommand(content);
        createCleanVoiceCommand();
        createFareCommand(nFare / 100);
        ret = 0;
    }
    else
    {
        //LogMsg(tr("veh fare [%1] is invalid").arg(nFare / 100));
    }
    return ret;
}

int FDUC::balanceBeforeDeductImpl(int nBalanceBeforeDeduct, bool bIsTallyCard)
{
    int ret = -1;
    if(QString::number(nBalanceBeforeDeduct / 100).length() <= 10)
    {
        //LogMsg("begin encode balance before frame");
        QByteArray content;
        if(bIsTallyCard)
        {
            content.append(UTF8toGB2312(QString::fromUtf8("记账卡")));
            for(int i = 0; i < 10; i++)
            {
                content.append(0x20);
            }
        }
        else
        {
            content.append(UTF8toGB2312(QString::fromUtf8("余额")));
            content.append(tr("%1").arg(nBalanceBeforeDeduct / 100, 10, 10, QLatin1Char(0x20)));
            content.append(UTF8toGB2312(QString::fromUtf8("元")));
        }
        createLine(content, 3);
        createCommand(content);
        ret = 0;
    }
    else
    {
        //LogMsg(tr("balance before [%1] is invalid").arg(nBalanceBeforeDeduct));
    }
    return ret;
}

int FDUC::balanceAfterDeductImpl(int nFarePaid, int nBalanceAfterDeduct, bool bIsTallyCard)
{
    //LogMsg("begin encode balance after deduct");
    return fareImpl(nFarePaid) && balanceBeforeDeductImpl(nBalanceAfterDeduct, bIsTallyCard);
}

int FDUC::entryStaImpl(const char *lpszEntrySta)
{
    QString entry = lpszEntrySta;
    int ret = -1;
    if(entry.length() <= 6)
    {
        //LogMsg("begin encode entry frame");
        QByteArray content;
        content.append(UTF8toGB2312(QString::fromUtf8("入口")));
        content.append(UTF8toGB2312(entry));
        for(int i = 0; i < 16 - content.size(); i++)
        {
            content.append(0x20);
        }
        createLine(content, 2);
        createCommand(content);
        ret = 0;
    }
    else
    {
        //LogMsg("entry name's length is too long, the max length is 12 chars");
    }
    return ret;
}

int FDUC::textImpl(char *lpszText, int nColor)
{
    int ret = -1;
    QString text = lpszText;
    if(text.length() <= 32)
    {
        //LogMsg("begin encode text frame");
        QByteArray content;
        for(int i = 0; i < 4; i++ )
        {
            content.append(UTF8toGB2312(text.mid(i * 8, 8)));
            createLine(content, i);
            createCommand(content);
            content.clear();
        }
        ret = 0;
    }
    else
    {
        //LogMsg("text's length is too long, the max length is 64 chars");
    }
    return ret;
}

int FDUC::synDataImpl()
{
    if(!isRunning())
    {
        start();
        //LogMsg("start thread run");
    }
    else
    {
        //LogMsg("the last run is running, no need to restart");
    }
    return 0;
}

int FDUC::clearAllImpl()
{
    QByteArray content;
    content.append("\x07\x94");
    content.append(char(0));
    content.append(char(0));
    content.append(char(0));
    createCommand(content);
    //LogMsg("clear all data");
    if(!isRunning())
    {
        start();
    }
    return 0;
}

int FDUC::brightnessImpl(int nBrightness)
{
    if(nBrightness <= 8 && nBrightness >= 1)
    {
        nBrightness = 0xf;
        //LogMsg("use auto adapt light");
    }
    //LogMsg("set uc fare display light");
    QByteArray content;
    content.append("\x08\x96");
    content.append(char(0));
    content.append(char(0));
    content.append(char(0));
    content.append((char) nBrightness);
    createCommand(content);
    return 0;
}

int FDUC::alarmLightImpl(bool bStatus)
{
    //LogMsg("set uc fare display warning");
    QByteArray content;
    content.append("\x07\x98");
    content.append((char)bStatus);
    content.append(char(0));
    content.append(char(0));
    createCommand(content);
    return 0;
}

int FDUC::alarmSoundImpl(bool bStatus)
{
    return alarmLightImpl(bStatus);
}

int FDUC::passLightImpl(bool bIsGreen)
{
    //LogMsg("set uc fare display pass light");
    QByteArray content;
    content.append("\x07\x99");
    content.append((char)bIsGreen);
    content.append(char(0));
    content.append(char(0));
    createCommand(content);
    if(bIsGreen)
    {
        createCommand(createPassCommand());
    }
    return 0;
}

void FDUC::run()
{
    m_mutex.lock();
    while(m_data.length() != 0)
    {
        QByteArray data = m_data.dequeue();
        qDebug() << "!!!!!!!!!!!!!!byte writen is" << m_serialPort->write(data);
        qDebug() << "send data is" << data.toHex().data();

        msleep(20);
    }
    m_mutex.unlock();
}


void FDUC::createLine(QByteArray &text, char row)
{
    text.prepend(char(3));
    text.prepend(char(0));
    text.prepend(char(0));
    text.prepend(0x80 + row);
    text.prepend(0x27);
    for(int i = text.length() - 5; i < 32; i++)
    {
        text.append(0x20);
    }
}

void FDUC::createCommand(const QByteArray &content)
{
    QByteArray data;
    data.append(0xAA);
    data.append(content);
    data.append(0xBB);
    data.append(0x01);
    m_mutex.lock();
    m_data.append(data);
    m_mutex.unlock();

    qDebug() << "create command, data is" << data.toHex().data();
    if(!isRunning())
    {
        start();
    }
    //LogMsg(tr("create command , data is [%1]").arg(data.toHex().data()));
}

void FDUC::createFareCommand(int fare)
{
    QByteArray content;
    content.append("\xFF\x0A");
    content.append(tr("%1").arg(fare, 5, 10, QLatin1Char('0')));
    content.append(tr("%1").arg(0, 15, 10, QLatin1Char('0')));
    content.append(0x32);
    content.append(GetXOR((const quint8*)content.data() + 2, 21));
    content.append(0x0D);
    qDebug() << "create voice data" << content.toHex().data();
    m_data.append(content);
    if(!isRunning())
    {
        start();
    }
}

void FDUC::createCleanVoiceCommand()
{
    m_data.append("\x0a\x46\0d");
}

QByteArray FDUC::createPassCommand()
{
    QByteArray content;
    content.append("\x0A\x4B\x01");
    content.append(char(0));
    content.append(char(0));
    content.append(char(0));
    content.append(char(0));
    content.append("\x33");
    return content;
}
