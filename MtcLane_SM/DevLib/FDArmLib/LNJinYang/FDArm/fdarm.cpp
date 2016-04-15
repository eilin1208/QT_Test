#include "fdarm.h"
#include "QtSerialPort/QSerialPort"
#include <QTextCodec>
#include <QFile>
#include <QDateTime>
#include <QDebug>
using namespace TNX;
const int ROWCOUNT = 4;
const int COLCOUNT = 16;
int g_nLaneType = 0;
bool g_bTollByWeight = true;
char sContent[ROWCOUNT][COLCOUNT];
QByteArray g_tmpCmd;
QByteArray g_finalCmd;
quint8 g_nLight = 255;
char g_nCmdType = 0;
QFile g_logFile("./FD.log");
void LogFD(const QString& sLog)
{
    if(!g_logFile.isOpen())
    {
        g_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    QString sFinal = QObject::tr("[%1] %2\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(sLog);
    g_logFile.write(sFinal.toLocal8Bit());
	g_logFile.flush();
}

//UTF8 TO GB2312
QByteArray UTF8toGB2312(const char* str, int length = -1)
{
    return QTextCodec::codecForName("GB2312")->fromUnicode(QTextCodec::codecForName("UTF-8")->toUnicode(str));
}
//GET BCC
char GetBcc(const char* szSrc, int nLength)
{
    char nRlt = szSrc[0];
    for(int i = 1; i < nLength; i++)
    {
        nRlt = nRlt ^ szSrc[i];
    }
    return nRlt;
}

void CreateCmd()
{
    QByteArray cmd;
    cmd.append(0xff);
    cmd.append(0xdc);
    cmd.append(g_tmpCmd);
    quint8 bcc = GetBcc(cmd.data(), cmd.length());
    cmd.append((char)bcc);
    g_finalCmd = cmd;
}

//显示16字符命令
void Create16CharCmd(const char* sText, int colNum)
{
    g_nCmdType = 'D';
    g_tmpCmd.clear();
    g_tmpCmd.append("\x17\x44");
    g_tmpCmd.append(g_nLight);
    g_tmpCmd.append(colNum);
    g_tmpCmd.append(sText, 16);
    CreateCmd();
}

QSerialPort* getSerialPort(char* sConnPara = NULL)
{
    static QSerialPort port(sConnPara);
    return &port;
}

// 设置费显工作模式
int FD_SetMode(int nLaneType, bool bTollByWeight)
{
    g_nLaneType = nLaneType;
    g_bTollByWeight = bTollByWeight;
    return 0;
}

// 关闭设备，释放占用的资源
int FD_CloseDev()
{
    if(getSerialPort()->isOpen())
    {
        getSerialPort()->close();
    }
    LogFD("fare display close...");
    return 0;
}


// 显示车型：nVehClass小于0表示清空车型信息（若超出费额显示器显示范围，应清空，不应显示错误值）
int FD_VehClass(int nVehClass)
{
    char sText[16];
    memset(sText, ' ', 16);
    strncpy(sText, UTF8toGB2312("车型：").data(), 6);
    char sVeh[4];
    if(nVehClass > 10)
    {
        snprintf(sVeh, sizeof(sVeh), "%d%s", nVehClass % 10, UTF8toGB2312("货").data());
    }
    else
    {
        snprintf(sVeh, sizeof(sVeh), "%d%s", nVehClass, UTF8toGB2312("客").data());
    }
    //strncpy(sText + COLCOUNT - 3, UTF8toGB2312(sVeh), 3);
    strncpy(sText + COLCOUNT - 3, sVeh, 3);

    LogFD(QObject::tr("show veh class:%1\n").arg(QString::fromLocal8Bit(sText, 16)));

    //拼接命令
    Create16CharCmd(sText, 1);
    return 0;
}

// 显示车货总重（kg）、限载（kg）、超限率（千分数）
int FD_Weight(int nTotalWeight, int nWeightLimit, int nOverweightRate)
{
    if((g_nLaneType == 2 || g_nLaneType == 4) && g_bTollByWeight)
    {
        char sText[16];
        memset(sText, ' ', 16);
        //总重
        strncpy(sText, UTF8toGB2312("总重：").data(), 6);
        QString sTmp = QString::number(nTotalWeight);
        if(sTmp.length() <= 8)
        {
            sTmp.append("kg");
            qstrncpy(sText + COLCOUNT - sTmp.length(),
                     sTmp.toLocal8Bit().data(),
                     10);
        }
        LogFD(QObject::tr("show Total Weight:%1\n").arg(QString::fromLocal8Bit(sText, 16)));

        Create16CharCmd(sText, 3);
        int nRlt = FD_SynData();
        if(nRlt == -1)
        {
            return -1;
        }
        //限载
        memset(sText, ' ', 16);
        strncpy(sText, UTF8toGB2312("超重：").data(), 6);
        sTmp = QString::number(nWeightLimit);
        if(sTmp.length() <= 8)
        {
            sTmp.append("kg");
            qstrncpy(sText + COLCOUNT - sTmp.length(),
                     sTmp.toLocal8Bit().data(),
                     10);
        }
        LogFD(QObject::tr("show Weight Limit:%1\n").arg(QString::fromLocal8Bit(sText, 16)));

        Create16CharCmd(sText, 4);
        nRlt = FD_SynData();
        if(nRlt == -1)
        {
            return -1;
        }
//        //超限率
//        memset(sText, ' ', 16);
//        strncpy(sText, UTF8toGB2312("超限率："), 6);
//        sTmp = QString::number(nOverweightRate / 10);
//        if(sTmp.length() <= 9)
//        {
//            sTmp.append("%");
//            qstrncpy(sText + COLCOUNT - sTmp.length(),
//                     sTmp.toLocal8Bit().data(),
//                     10);
//        }
//        LogFD(QObject::tr("show Over weight Rate:%1\n").arg(QString::fromLocal8Bit(sText, 16)));

//        Create16CharCmd(sText, 4);
//        nRlt = FD_SynData();
//        if(nRlt == -1)
//        {
//            return -1;
//        }
    }
    return 0;
}

// 显示应收金额：nFare小于0表示清空金额信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
int FD_Fare(int nFare)
{
    if(g_nLaneType == 2 || g_nLaneType == 4)
    {
        QString sFare;
        char sText[16];
        memset(sText, ' ', 16);
        strncpy(sText, UTF8toGB2312("金额：").data(), 6);
        sFare = QString::number( nFare / 100 );
        if(sFare.length() > 8)
        {
            return -1;
        }
        strncpy(sText + COLCOUNT - 2 - sFare.length() ,
                UTF8toGB2312(sFare.toLocal8Bit().data()).data(),
                sFare.length());
        strncpy(sText + COLCOUNT - 2,
                UTF8toGB2312("元").data(),
                2);
        LogFD(QObject::tr("show nFare:%1\n").arg(QString::fromLocal8Bit(sText, 16)));

        //拼接命令
        Create16CharCmd(sText, 2);
    }
    return 0;
}

// 显示扣款前余额：小于0表示清空该段信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
int FD_BalanceBeforeDeduct(int nBalanceBeforeDeduct, bool bIsTallyCard)
{
    if(g_nLaneType == 2 || g_nLaneType == 4)
    {
        QString sFare;
        char sText[16];
        memset(sText, ' ', 16);
        strncpy(sText, UTF8toGB2312("余额：").data(), 6);
        sFare = QString::number( nBalanceBeforeDeduct / 100 );
        if(sFare.length() > 8)
        {
            return -1;
        }
        strncpy(sText + COLCOUNT - 2 - sFare.length() ,
                UTF8toGB2312(sFare.toLocal8Bit().data()).data(),
                sFare.length());
        strncpy(sText + COLCOUNT - 2,
                UTF8toGB2312("元").data(),
                2);
        LogFD(QObject::tr("show nBalanceBeforeDeduct:%1\n").arg(QString::fromLocal8Bit(sText, 16)));

        //拼接命令
        Create16CharCmd(sText, 3);
    }
    return 0;
}


// 显示实收金额和扣款后余额：小于0表示清空该段信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
int FD_BalanceAfterDeduct(int nFarePaid, int nBalanceAfterDeduct, bool bIsTallyCard)
{
    if(g_nLaneType == 2 || g_nLaneType == 4)
    {
        QString sFare;
        char sText[16];
        //消费
        memset(sText, ' ', 16);
        strncpy(sText, UTF8toGB2312("消费：").data(), 6);
        sFare = QString::number( nFarePaid / 100 );
        if(sFare.length() > 8)
        {
            return -1;
        }
        strncpy(sText + COLCOUNT - 2 - sFare.length() ,
                UTF8toGB2312(sFare.toLocal8Bit().data()).data(),
                sFare.length());
        strncpy(sText + COLCOUNT - 2,
                UTF8toGB2312("元").data(),
                2);
        //拼接命令
        LogFD(QObject::tr("show nFarePaid:%1\n").arg(QString::fromLocal8Bit(sText, 16)));

        Create16CharCmd(sText, 3);
        int nRlt = FD_SynData();
        if(nRlt == -1)
        {
            return -1;
        }

        //余额
        memset(sText, ' ', 16);
        strncpy(sText, UTF8toGB2312("余额：").data(), 6);
        sFare = QString::number( nBalanceAfterDeduct / 100 );
        if(sFare.length() > 8)
        {
            return -1;
        }
        strncpy(sText + COLCOUNT - 2 - sFare.length() ,
                UTF8toGB2312(sFare.toLocal8Bit().data()).data(),

                sFare.length());
        strncpy(sText + COLCOUNT - 2,
                UTF8toGB2312("元").data(),
                2);
        LogFD(QObject::tr("show nBalanceAfterDeduct:%1\n").arg(QString::fromLocal8Bit(sText, 16)));

        //拼接命令
        Create16CharCmd(sText, 4);
        nRlt = FD_SynData();
        if(nRlt == -1)
        {
            return -1;
        }

    }
    return 0;
}

//显示入口站
int FD_EntrySta(char *lpszEntrySta)
{
    //判断文字是否超出
    if(qstrlen(lpszEntrySta) > 16)
    {
        return -1;
    }
    char sText[16];
    memset(sText, ' ', 16);
    qstrncpy(sText, UTF8toGB2312(lpszEntrySta).data(), 16);
    LogFD(QObject::tr("show lpszEntrySta:%1\n").arg(QString::fromLocal8Bit(sText, 16)));

    //封装命令
    Create16CharCmd(sText, 2);
    CreateCmd();
    return 0;
}

//清屏
int FD_ClearAll()
{
    g_nCmdType = 'C';
    g_tmpCmd.clear();
    g_tmpCmd.append(0x05);
    g_tmpCmd.append(0x43);
    LogFD("clear screen...");
    //生成命令
    CreateCmd();
    //发送
    return FD_SynData();
}
//显示文字
int FD_Text(char *lpszText, int nColor)
{
    //首先清屏
    FD_ClearAll();
    //判断文字长度是否超出
    if(qstrlen(lpszText) > 64)
    {
        return -1;
    }
    char sText[64];
    memset(sText, ' ', 64);
    strncpy(sText, UTF8toGB2312(lpszText).data(), 64);
    //拼接命令
    g_tmpCmd.clear();
    g_tmpCmd.append("\x47\x44");
    g_tmpCmd.append((char)g_nLight);
    g_tmpCmd.append("\x00");
    g_tmpCmd.append(sText, 64);
    LogFD("show Text...");
    LogFD(g_tmpCmd);
    CreateCmd();
    //发送
    return FD_SynData();
}

//设置亮度
int FD_Brightness(int nBrightness)
{
    g_nLight = nBrightness * 2.55;
    return 0;
}

// 初始化
int FD_OpenDev(char* lpszConnPara)
{
    if(getSerialPort(lpszConnPara)->open())
    {
        LogFD("open fare display success");
        return 0;
    }
    LogFD("open fare display fail");

    return -1;
}

// 黄闪报警灯控制
int FD_AlarmLight(bool bStatus)
{
    return 0;
}

// 报警声音控制
int FD_AlarmSound(bool bStatus)
{
    return 0;
}

// 通行信号灯控制
int FD_PassLight(bool bIsGreen)
{
    return 0;
}

QByteArray GetNormalResponse()
{
    QByteArray normal;
    normal.append("\xff\xdc\x05");
    normal.append(g_nCmdType);
    normal.append(GetBcc(normal.data(), normal.length()));
    return normal;
}

int FD_SynData()
{
    if(!getSerialPort()->isOpen())
    {
        return -1;
    }
    if(g_finalCmd.isEmpty())
    {
        return 0;
    }
    //发送数据
    LogFD(QObject::tr("write data[%1]").arg(g_finalCmd.toHex().data()));
    getSerialPort()->write(g_finalCmd);
    bool bRlt = getSerialPort()->waitForBytesWritten(200);
    if(!bRlt)
    {
        LogFD("write data fail!");
        return -1;
    }
    //读取应答
    bRlt = getSerialPort()->waitForReadyRead(200);
    if(!bRlt)
    {
        LogFD("read data fail!");
        return -1;
    }
    QByteArray response = getSerialPort()->read(getSerialPort()->bytesAvailable());
    LogFD(QObject::tr("read data length[%1], data[%2]")
          .arg(getSerialPort()->bytesAvailable())
          .arg(response.toHex().data()));
    //比对
    if(response.contains(GetNormalResponse()))
    {
        return 0;
    }
    g_finalCmd.clear();
    g_tmpCmd.clear();
    return -1;
}

