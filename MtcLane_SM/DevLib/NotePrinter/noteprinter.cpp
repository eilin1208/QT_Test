#include "noteprinter.h"
#include <QtSerialPort/QSerialPort>
#include "qparport.h"
#include <QDebug>
#include <QFile>
#include <sys/ioctl.h>
#include <fcntl.h>
using namespace TNX;
//using TNX::QSerialPort;
//是否使用并口
bool g_bUsePar = false;
//串口或并口对象
QIODevice* g_pPort = NULL;

//打印失败
bool g_bPrtFail = false;

//是否忙碌
bool g_bBusy = false;

//是否在运行
bool g_bRun = false;
//打印机状态
int g_nStatus = 0;

//
int g_nOldCTS = 0;
int g_nOldDSR = 0;
int g_nOldRLSD = 0;

QFile g_file("Printer.log");
void LogPrinter(const QString& sLog)
{
    if(!g_file.isOpen())
    {
        g_file.open(QIODevice::ReadWrite | QIODevice::Append);

    }
    g_file.write(sLog.toLocal8Bit());
    g_file.flush();
}
void GetStatus();
//是否联机
bool g_bOnLine = false;
QIODevice* getNotePrinterPort(const char *name, const char *settings, bool usePar)
{
    g_bUsePar = usePar;
    QIODevice* tmpPort = NULL;
    //使用并口，返回并口对象
    if(g_bUsePar)
    {
        tmpPort = new  QParPort(QString(name));
    }
    //使用串口，返回串口对象
    else
    {
        LogPrinter("use serial port");
        tmpPort = new QSerialPort(name,settings);
    }
    return tmpPort;
}

void ReleasePort()
{
    if(g_pPort != NULL)
    {
        delete g_pPort;
        g_pPort = NULL;
    }
}

void ChangeFormat(char DT16[], const char* ADateTime14)
{
    if(strlen(ADateTime14) < 12)
    {
        return;
    }
    DT16[0] = ADateTime14[0];
    DT16[1] = ADateTime14[1];
    DT16[2] = ADateTime14[2];
    DT16[3] = ADateTime14[3];
    DT16[4] = '-';
    DT16[5] = ADateTime14[4];
    DT16[6] = ADateTime14[5];
    DT16[7] = '-';
    DT16[8] = ADateTime14[6];
    DT16[9] = ADateTime14[7];
    DT16[10] = ' ';
    DT16[11] = ADateTime14[8];
    DT16[12] = ADateTime14[9];
    DT16[13] = ':';
    DT16[14] = ADateTime14[10];
    DT16[15] = ADateTime14[11];
    DT16[16] = 0;
}

int SendDataToPtr(const char *strSendData, bool bWait)
{
    if(g_pPort == NULL)
    {
        return -1;
    }
    if(!g_pPort->isOpen())
    {
        qDebug() << "send data to ptr: port not open";
        return -1;
    }
    if(g_pPort->write(strSendData) == -1)
    {
        qDebug() << "send data to ptr: write fail";
        return -1;
    }
    if(bWait)
    {
        g_pPort->waitForBytesWritten(5000);
    }
    return 0;
}

void GetPtrInit(char ptrinit[])
{
    int a;
    a = 0;
    // 设置左边距8 原来为11
    ptrinit[a] = 0x1B;//27
    a++;
    ptrinit[a] = 0x6c;//108
    a++;
    ptrinit[a] = 0x08;// 8
    a++;
    // 5X9
    ptrinit[a] = 0x1B;//27
    a++;
    ptrinit[a] = 0x50;//80
    a++;
    ptrinit[a] = 0;
}

void SetClearPtr()
{
    char ptrinit[3] = "";
    ptrinit[0] = 0x18;
    ptrinit[1] = 0;
    SendDataToPtr(ptrinit);
}

/*
功能：设置一种格式的行间距（）x/72
返回：0成功，－1失败
*/
bool SetSpaceNum(char jj)
{
    char ptrinit[5] = "";
    ptrinit[0] = 0x1B;
    ptrinit[1] = 0x41;
    ptrinit[2] = jj;
    ptrinit[3] = 0x1B;
    ptrinit[4] = 0x32;
    ptrinit[5] = 0;
    return SendDataToPtr(ptrinit);
}

/*******************************************************************
名  称：int  InitFormat()
返回值：
        成功，0
        失败，返回负值
功  能：
        初始化打印机格式。
*******************************************************************/
int InitFormat()
{
    char init[20] = "";
    if(g_pPort == NULL)
        return -1;

    if(!g_pPort->isOpen())
    {
        return -1;
    }
    GetPtrInit(init);
    return SendDataToPtr(init);
}

//打印N个空行
int PrintNSpace(int n)
{
    char Lc_Line[3] = "";
    char Content[300] = "";
    Lc_Line[0] = 0x0A;
    Lc_Line[1] = 0x0D;
    Lc_Line[2] = 0;
    if (n <= 0 )
        return 0;
    for (int i = 0; i < n ;i++)
    {
        strcat(Content, Lc_Line);
    }
    return SendDataToPtr(Content);
}

int PrintText(const char * Es_CarType,const char * Es_Fare_chn,
              const char * Es_DateTime,const char * Es_SerialNum)
{
    char Lc_Line[3] = "";
    char Lc_zt[3] = "";
    char Lc_czt[3] = "";
    char Content[300] = "";
    Lc_Line[0] = 0x0A;
    Lc_Line[1] = 0x0D;
    Lc_Line[2] = 0;
    Lc_zt[0] = 0x0E;
    Lc_zt[1] = 0;
    Lc_czt[0] = 0x14;
    Lc_czt[1] = 0;

    // 放大字体
    //strcpy(Content, Lc_zt);

    // 恢复字体
    //strcat(Content, Lc_czt);

    //strcat(Content, Lc_Line);
    // 打印号=收费员工号
    strcat(Content, Es_SerialNum);
    strcat(Content, Lc_Line);
    // 放大字体
    //strcpy(Content, Lc_zt);
    // 车型  和重量
    strcat(Content, Es_CarType);
    strcat(Content, Lc_Line);
    // 大写金额
    strcat(Content, Es_Fare_chn);
    strcat(Content, Lc_Line);
    // 恢复字体
    //strcat(Content, Lc_czt);
    // 时间
    strcat(Content, Es_DateTime);
    //strcat(Content, Lc_Line);

    if ( strlen(Content) <= 0) return -1;
    return SendDataToPtr(Content);
}

/*******************************************************************
名  称：int PrtInit(int port)
参  数：
        port: 端口号
返回值：
        成功，返回所使用的端口号
        失败，返回负值
功  能：
       初始化设备（打印机）
*******************************************************************/
int PrtInit(const char *port, const char *settings, bool bUsePort)
{
    g_pPort = getNotePrinterPort(port, settings, bUsePort);
    if(!g_pPort->open(QIODevice::ReadWrite))
    {
        return -1;
    }
    GetStatus();

    SetClearPtr();
    LogPrinter("print init");
    qDebug() << "lib status ......" << g_nStatus;
    return 1;
}

/*******************************************************************
名  称：int _stdcall PrtNote(TNoteContent NoteContent)
参  数：
        NoteContent: 打印内容，定义见StructPacked单元或相关定义单元。
返回值：
        成功，返回打印张数(全部为1)
        失败，返回负值 -1
功  能：
        根据参数，调用其它方法实现打印。
2002-01-07，郭子英编写
*******************************************************************/
int PrtNote(TNoteContent NoteContent)
{
    g_bBusy = true;
    char Ls_CarType[4] = "";   //车型}
    char Ls_Fare_chn[32] = "";  //金额}
    char Ls_DateTime[32] = "";  //日期时间}
    char Ls_SerialNum[16] = "";  //收费员
    char Ls_EnStationName[32] = "";
    char Ls_ExStationName[32] = "";
    char Ls_temp[32] = "";
    char Ls_sweight[10] = "",Ls_limitw[10]="", Ls_Over[10] = "";  // 重量
    char Ls_PtrContent[280] = "";
    char Ls_One[100] = "",Ls_Two[100] = "",Ls_Three[100] = "",Ls_Four[100] = "",Ls_Five[100]="";
    int i,nLen;
    /////////////////////////////
    char Lc_Line[3] = "";
    char Lc_zt[3] = "";
    char Lc_czt[3] = "";
    char Content[600] = "";
    int li_distance = 18;  // 两列字开头距离
    Lc_Line[0] = 0x0A;
    Lc_Line[1] = 0x0D;
    Lc_Line[2] = 0;
    Lc_zt[0] = 0x0E;
    Lc_zt[1] = 0;
    Lc_czt[0] = 0x14;
    Lc_czt[1] = 0;
    if(!g_pPort->isOpen())
    {
        qDebug() << "prt note fail: port not open";
        return false;
    }
    if(!g_bOnLine)
    {
        qDebug() << "port note fail: off line";
    }

    // 车型
    strcpy(Ls_CarType, NoteContent.CarType);
    // 打印小写金额
    if (NoteContent.test == 1)	 // 1测试，0非测试
        sprintf(Ls_Fare_chn, "%.2d测试", NoteContent.Fare_Num/100);
    else
        sprintf(Ls_Fare_chn, "%.2d", NoteContent.Fare_Num/100);
    // 日期
    strcpy(Ls_DateTime, NoteContent.DateTime);
    // 收费员
    strcpy(Ls_SerialNum, NoteContent.CollectorID);
    //入口站
    strcpy(Ls_EnStationName,NoteContent.EnStationName);
    //出口站
    strcpy(Ls_ExStationName,NoteContent.ExStationName);
    // 重量
    strcpy(Ls_sweight, NoteContent.weight);
    //限载
    strcpy(Ls_limitw,NoteContent.limitw);
    // 超限率
    strcpy(Ls_Over, NoteContent.OverP);

    //////// 整理打印内容  ////////
    //第一行              收费员
    strcat(Ls_One, Ls_CarType);
    for (i = 1; i<=6; i++)
    {
        strcat(Ls_One, " ");
    }
    strcat(Ls_One,Ls_EnStationName);
    for (i = 1; i<=6; i++)
    {
        strcat(Ls_One, " ");
    }
    strcat(Ls_One,Ls_ExStationName);

    //第四行
    for (i = 1; i<=4; i++)
    {
        strcat(Ls_Two, " ");
    }
    strcat(Ls_Two,Ls_Fare_chn);
    nLen = li_distance - sizeof(Ls_SerialNum);
    for (i = 1; i<=4; i++)
    {
        strcat(Ls_Two, " ");
    }
    strcat(Ls_Two,Ls_SerialNum);
    //第二行
    if (NoteContent.iFeeType == 1)
    {
        for (i = 1; i<=4; i++)
        {
            strcat(Ls_Three, " ");
        }
        strcat(Ls_Three,Ls_limitw);
        for (i = 1; i<=7; i++)
        {
            strcat(Ls_Three, " ");
        }
        strcat(Ls_Three,Ls_sweight);
        //第三行
        for (i = 1; i<=4; i++)
        {
            strcat(Ls_Four, " ");
        }
        strcat(Ls_Four,Ls_sweight);
        for (i = 1; i<=7; i++)
        {
            strcat(Ls_Four, " ");
        }
        strcat(Ls_Four,Ls_Over);
    }
    //第五行
    strcat(Ls_Five,Ls_DateTime);
    //////// 打印内容  ////////
    if (SetSpaceNum(0x20) !=0)    // 48/72
    {
        //g_Log.ErrorReport("打印过程中，调用PrtNote，SetSpaceNum(0x30)打印开头空行失败，继续执行！");
    }
    if (PrintNSpace(1) !=0)
    {
        //g_Log.ErrorReport("打印过程中，调用PrtNote，PrintNSpace(1)打印开头空行失败，继续执行！");
    }
    if (SetSpaceNum(0x19) !=0)    // 48/72
    {
        //g_Log.ErrorReport("打印过程中，调用PrtNote，SetSpaceNum(0x30)打印开头空行失败，继续执行！");
    }
    if (PrintNSpace(3) !=0)
    {
        //g_Log.ErrorReport("打印过程中，调用PrtNote，PrintNSpace(1)打印开头空行失败，继续执行！");
    }
    //打印第一行
    if (SetSpaceNum(0x19) !=0)    // 48/72
    {
        qDebug() << "prt note: set space num fail, go on";
    }
    strcpy(Content, Ls_One);
    strcat(Content, Lc_Line);
    qDebug() << QString(Content);
    if (SendDataToPtr(Content, true) != 0)
    {
        qDebug() << "prt note: send data to ptr fail, go on";
    }
    //打印第二行
    memset(Content, 0, sizeof(Content));
    strcpy(Content, Ls_Two);
    strcat(Content, Lc_Line);
    qDebug() << QString(Content);
    if (SendDataToPtr(Content, true) != 0)
    {
        qDebug() << "prt note: send data to ptr fail,2 row go on";
    }
    //打印第三行
    memset(Content, 0, sizeof(Content));
    strcpy(Content, Ls_Three);
    strcat(Content, Lc_Line);
    qDebug() << QString(Content);
    if (SendDataToPtr(Content, true) != 0)
    {
        qDebug() << "prt note: send data to ptr fail,3 row go on";
    }
    //打印第四行
    memset(Content, 0, sizeof(Content));
    strcpy(Content, Ls_Four);
    strcat(Content, Lc_Line);
    qDebug() << QString(Content);
    if (SendDataToPtr(Content, true) != 0)
    {
        qDebug() << "prt note: send data to ptr fail,4 row go on";
    }
    //打印第五行
    if (SetSpaceNum(0x21) !=0) // 120/72
    {
        qDebug() << "prt note: send data to ptr fail,weibu go on";
    }
    memset(Content, 0, sizeof(Content));
    strcpy(Content, Ls_Five);
    qDebug() << QString(Content);
    if (SendDataToPtr(Content, TRUE) != 0)
    {
        qDebug() << "prt note: send data to ptr fail,5 row go on";
    }
    //////// 处理票据尾部  ////////
    if (SetSpaceNum(0x28) !=0) // 120/72
    {
        qDebug() << "prt note: send data to ptr fail,weibu row go on";
    }
    if (PrintNSpace(1) !=0)
    {
        qDebug() << "prt note: send data to ptr fail,weibu row go on";

    }
    if (SetSpaceNum(0x29) !=0) // 120/72
    {
        qDebug() << "prt note: send data to ptr fail,weibu row go on";

    }
    if (PrintNSpace(1) !=0)
    {
        qDebug() << "prt note: send data to ptr fail,weibu row go on";

    }
    g_bBusy = false;
    return 1;

}

int PrtStop()
{
    g_bRun = false;
    return 0;
}

int PrtClose()
{
    PrtStop();
    if(g_pPort == NULL)
    {
        return false;
    }
    if(g_pPort->isOpen())
    {
        g_pPort->close();
        ReleasePort();
    }
    return 0;
}

void GetStatus()
{
    if(g_bBusy)
    {
        return;
    }
    if(g_bUsePar)
    {
        QParPort * parPort = (QParPort*)g_pPort;
        if(parPort->isOnline())
        {
            g_nStatus = 0;
            SetClearPtr();
            InitFormat();
            g_bOnLine = true;
        }
        else
        {
            g_nStatus = -1;
        }
        return;
    }
    else
    {
        QSerialPort * serPort = (QSerialPort*)g_pPort;
        serPort->write("\x05");
        bool bRet = serPort->waitForReadyRead(250);
        if(bRet)
        {
            QByteArray array = serPort->read(serPort->bytesAvailable());
            qDebug() << "array " << (int)array.at(0);
            if(array.at(0) == 0x20)
            {
                g_nStatus = 0;
            }
            else
            {
                g_nStatus = -1;
            }

        }
        else
        {
            g_nStatus = -1;
        }
        SetClearPtr();
        InitFormat();
    }
}

int PrtStart()
{
    if(g_bRun)
    {
        return 0;
    }

    if(!g_pPort->isOpen())
    {
        return -1;
    }

    g_nOldCTS = -1;
    g_nOldDSR = -1;
    g_nOldRLSD = -1;

    GetStatus();

    g_bRun = true;
    SetClearPtr();
    InitFormat();
    return 0;
}

int PrtGetStatus()
{
    GetStatus();
    qDebug() << "get status " << g_nStatus;
    return g_nStatus;
}
