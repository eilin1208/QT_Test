#include "rsuarmcomm.h"
#include <QDebug>

RsuArmComm::RsuArmComm(const QString & portName,QObject *parent) :
    QObject(parent)
{
    pSerialPort = new QSerialPort(portName);
    pSerialPort->setBaudRate(QPortSettings::BAUDR_115200);
    initParm();
}
RsuArmComm::RsuArmComm(QObject *parent):
    QObject(parent)
{
    QString str = QString("COM1");
    pSerialPort = new QSerialPort(str);
    initParm();
    //Open Debug message
    SetDebugflag(true);
}

RsuArmComm::~RsuArmComm()
{
    closeComm();
}

//参数初始化；
void RsuArmComm::initParm()
{
    m_RecivedRawData.clear();
    m_RecivedFrameData.clear();
    reciveDataTimer = new QTimer(this);
    bDebugFlag=false;
    connect(reciveDataTimer,SIGNAL(timeout()),this,SLOT(readCommData()));
}

void RsuArmComm::printCommData()
{
    if(pSerialPort->bytesAvailable()<=0)
    {
        DebugMessage("unavailable");
        return;
    }
    DebugMessage(tr("print recived raw data :%1")
                 .arg(QString(pSerialPort->readAll().toHex().data())));

}

//打开串口
bool RsuArmComm::openComm()
{
    if ( !pSerialPort->open() )
    {
        qFatal("Cannot open serial port %s. Exiting..", qPrintable(pSerialPort->portName()));
        return false;
    }

    if ( !pSerialPort->setCommTimeouts(QSerialPort::CtScheme_NonBlockingRead) )
    {
        qWarning("Cannot set communications timeout values at port %s.", qPrintable(pSerialPort->portName()));
        return false;
    }

    setDataRecivedTimeInterval_ms();

    pSerialPort->flushInBuffer();
    pSerialPort->flushOutBuffer();

    return true;

}

//关闭串口
void RsuArmComm::closeComm()
{
    if(isOpenComm())
    {
        pSerialPort->close();
        reciveDataTimer->stop();
    }
}

bool RsuArmComm::isOpenComm()
{
    pSerialPort->isOpen();
}

//串口参数设置
bool RsuArmComm::setPortSettings(const QPortSettings &portSettings)
{
    pSerialPort->setPortSettings(portSettings);
}
bool RsuArmComm::setPortName(const QString & portName)
{
    bool flag = false;
    pSerialPort->setPortName(portName);
    flag =true;
    return flag;
}

void RsuArmComm::setDataRecivedTimeInterval_ms(int ms)
{
    if(ms >= 0)
    {
        reciveDataTimer->stop();
        reciveDataTimer->start(ms);
    }
    else
    {
        reciveDataTimer->stop();
        reciveDataTimer->start(1);
    }

}

/*串口接收数据*/
void RsuArmComm::readCommData()
{
    //所发送的不是空报文时才进行相应的解析
    if(pSerialPort->bytesAvailable()<=0)
    {
        //qDebug()<<"unavailable ";
        return;
    }
    //进行报文的截取
    if(!readFrameData())
    {
        DebugMessage("readFrameData()  error!");
        return;
    }
}
bool  RsuArmComm::readFrameData()
{
    DebugMessage("接收到报文");
    //先将接收到的报文加入队列中
    m_RecivedRawData.append(pSerialPort->read(pSerialPort->bytesAvailable()));
    DebugMessage(tr("recived raw data :%1")
                 .arg(QString(m_RecivedRawData.toHex())));

    //add by dafang 2012-10-8
    int step = 0 ;
    int startIndex = 0;
    int endIndex = 0;

    //清空接收帧数据的内存
    m_RecivedFrameData.clear();

    //查找帧头
    if(step == 0)
    {
        qDebug()<<"step = 0 ";
        int i = 1;
        for(i=0; i<m_RecivedRawData.size()-2; i++)
        {
            if(((quint8)m_RecivedRawData.at(i) == 0xff)&&((quint8)m_RecivedRawData.at(i+1) == 0xff))
            {
                if((quint8)m_RecivedRawData.at(i+2) != 0xff)
                {
                    //qDebug()<<"step = 0 to 1";
                    step = 1;
                    startIndex = i;
                    break;
                }
            }
        }
        // qDebug()<<"i"<<i;
        if(i == m_RecivedRawData.size()-2)
        {
            if(((quint8)m_RecivedRawData.at(i) != 0xff)||((quint8)m_RecivedRawData.at(i+1) != 0xff))
            {
                m_RecivedRawData.clear();
                return false;
            }
        }
    }
    //查找帧尾
    if(step == 1)
    {
        qDebug()<<"step = 1";

        if(m_RecivedRawData.size()<4)
        {
            m_RecivedRawData.clear();
            step = 0;
            startIndex = 0;
            return false;
        }
        int j = 0;
        for( j=startIndex+2; j<m_RecivedRawData.size(); j++)
        {
            if((quint8)m_RecivedRawData.at(j) == 0xff)
            {
                step = 2;
                endIndex = j;
                break;
            }
        }
        if(j == m_RecivedRawData.size()-1)
        {
            if(((quint8)m_RecivedRawData.at(j)!= 0xff))
            {
                m_RecivedFrameData.clear();
                //endIndex = 0;
                return true;//此处没有接完一帧数据，等待再次接受。
            }
        }
    }
    //复制帧到m_RecivedFrameData；并将这部分从m_RecivedRawData中清除
    if(step == 2)
    {
        qDebug()<<"step = 2 ";
        m_RecivedFrameData.clear();
        m_RecivedFrameData.append(m_RecivedRawData.mid(startIndex, endIndex-startIndex+1));

        //清除 帧"ffff"and tail "ff"
        InterceptFrameData(m_RecivedFrameData);
        //字符转换
        InputTrans(m_RecivedFrameData);
        //清除解析过的部分
        m_RecivedRawData = m_RecivedRawData.mid(endIndex+1);
        if(m_RecivedFrameData.size() < 4)
        {
            m_RecivedRawData.clear();
        }
        else
        {
            DebugMessage(tr("m_RecivedFrameData:")
                         .arg(QString(m_RecivedFrameData.toHex())));
            emit readyReadFrameData(getRecivedFrameData());
        }
    }    
    return true;
}

QByteArray RsuArmComm::getRecivedFrameData()
{
    return m_RecivedFrameData;
}

qint64 RsuArmComm::sendFrameData(QByteArray &data)
{

    PackageFrameData(data);
    emit sendingFrameData(data);
    return pSerialPort->write(data);
}
qint64 RsuArmComm::sendFrameData(char *data,int size)
{    
    QByteArray temp=QByteArray::fromRawData(data,size);
    PackageFrameData(temp);
//    qDebug()<<"send frame data:"<<temp.toHex();
    emit sendingFrameData(temp);
    return pSerialPort->write(temp.data(),temp.length());
}

void RsuArmComm::DebugMessage(QString message)
{
    if(bDebugFlag)
        qDebug()<<"RsuArmComm debug message:"<<message;
}

void RsuArmComm::SetDebugflag(bool flag)
{
    bDebugFlag=flag;
}

RsuArmComm * GetRsuArmCommInstance()
{
    static RsuArmComm rsuArmComm;
    return &rsuArmComm;
}

void RsuArmComm::InputTrans(QByteArray &message)
{
    //接收数据时，如果出现"FE 01"这样连续两个字节时将之合为一个字节FFH；
    //如果出现"FE 00"这样连续两个字节时将之合为一个字节FEH
    for(int i=0;i<message.length();i++)
    {
        if(message.at(i)==(char)0xfe)
        {
            switch (message.at(i+1))
            {
            case (char)0x01:
            {
                QByteArray temp;
                temp.append(0xff);
                message.replace(i,2,temp);
                break;
            }
            case (char)0x00:
            {
                QByteArray temp;
                temp.append(0xfe);
                message.replace(i,2,temp);
                break;
            }
            default:
            {
                break;
            }
            }
        }
    }
}

void RsuArmComm::OutputTrans(QByteArray &message)
{
    //发送数据时，如果出现FFH字节时，将之分解为FEH和01H这两个字节来发送；
    //如果出现FEH字节时，将之分解为FEH和00H这两个字节来发送。
    for(int i=0;i<message.length();i++)
    {
        if(message.at(i)==(char)0xff)
        {
            QByteArray temp;
            temp.append((char)0xfe);
            temp.append((char)0x01);
            message.replace(i,1,temp);
            continue;
        }
        if(message.at(i)==(char)0xfe)
        {
            QByteArray temp;
            temp.append((char)0xfe);
            temp.append((char)0x00);
            message.replace(i,1,temp);
            continue;
        }
    }
}

void RsuArmComm::InterceptFrameData(QByteArray &message)
{
    message=message.left(message.length()-1);
    message=message.right(message.length()-2);
}

void RsuArmComm::PackageFrameData(QByteArray &message)
{
    //进行数据的异或校验位的添加
    message+=(GetXOR(message.data(),message.length()));
    //进行字符转义
    OutputTrans(message);
    //进行数据的开始与结束位的添加
    QByteArray temp;
    temp.push_back(0xff);
    temp.push_back(0xff);
    message.prepend(temp);
    message.push_back(0xff);
    DebugMessage(tr("要发送的数据：%1").arg(message.toHex().data()));
}
bool RsuArmComm::FrameBCCPass(QByteArray data)
{
    int size=data.size();
    if(size<2)
    {
        DebugMessage(tr("接收到错误的串口数据：%1").arg(QString(data)));
        return false;
    }
    QByteArray temp=data.left(data.size()-1);
    quint8 tmp1 = GetXOR(temp.data(),temp.size());
    quint8 tmp2 = (quint8)data.at(data.size()-1);
    return (tmp1 == tmp2);
}

quint8 RsuArmComm::GetXOR(const QString &str)
{
    if(str.isEmpty())
        return 0;

    quint8 tmp=0x00;
    for(int it=0;it!=str.length();++it)
    {
        tmp = tmp^(quint8 )str.toLocal8Bit().at(it);
    }
    return tmp;
}
quint8 RsuArmComm::GetXOR(const char *Buff,int nLen)
{
    if(!Buff)
        return 0;
    quint8 tmp=0;
    for(int i=0;i<nLen;++i)
    {
        tmp = tmp ^(quint8)Buff[i];
    }
    return tmp;
}
