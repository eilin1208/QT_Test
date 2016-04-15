#include "noteprinter.h"
using namespace TNX;
#include<QDebug>

// 打印失败(Failure) 0失败或故障(false)  1成功(true)  -1没有变化
int LI_SignalFailure = -1;
// 是否发送过打印失败，true是   false否
//BOOL m_ISendFailure = FALSE;
// 发送消息的句柄
//HANDLE m_ParentHandle;
//UINT m_ReturnMsg;
// 控制线程的停止  true 已经运行
BOOL m_Run = FALSE;
//int m_IDEvent;
int m_OldCTS, m_OldDSR, m_OldRLSD;
// 标记当前是否联机
BOOL m_OnLine = FALSE;
// 微调打印机纸张位置
int IIntChange = 1;
// 微调打印机，打印1个60，打印多少个61
//int ILongRow;

//int flag = 0;
int m_Status = -1;

BOOL bBusy = FALSE;
/*******************************************************************
Function CString ChangeFormat(CString ADateTime12)
参  数：
        ADateTime12  字符串，日期时间yyyymmddhhmmss 或yyyymmddhhmm
返回值：
        日期时间 yyyy-mm-dd hh:mm
功  能：
        转换字符格式。
*******************************************************************/
void ChangeFormat(char DT16[],const char * ADateTime14)
{
    if(strlen(ADateTime14)<12)
        return;

    DT16[0] = ADateTime14[0]; DT16[1] = ADateTime14[1];
    DT16[2] = ADateTime14[2]; DT16[3] = ADateTime14[3];
    DT16[4] = '-'; DT16[5] = ADateTime14[4];
    DT16[6] = ADateTime14[5]; DT16[7] = '-';
    DT16[8] = ADateTime14[6]; DT16[9] = ADateTime14[7];
    DT16[10] = ' '; DT16[11] = ADateTime14[8];
    DT16[12] = ADateTime14[9]; DT16[13] = ':';
    DT16[14] = ADateTime14[10]; DT16[15] = ADateTime14[11];
    DT16[16] = 0;
    return;

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
    return;
}
//对象serial
QSerialPort* getSerialPort()
{
    static QSerialPort serial("COM1","9600,8,n,1");
    return &serial;
}
int SendDataToPtr(const char* strSendData, BOOL bWait )
{
    Sleep(5);
    if(getSerialPort()->write(strSendData)!=0)
    {
        LI_SignalFailure=0;
        return -1;
    }
    else
    {
        LI_SignalFailure=1;
        return 0;
    }
}
void SetClearPtr()
{
    char ptrinit[3] = "";
    ptrinit[0] = 0x18;
    ptrinit[1] = 0;
    SendDataToPtr(ptrinit);
}
int SetSpaceNum(char jj)
{
    char ptrinit[5] = "";
    ptrinit[0] = 0x1B;
    ptrinit[1] = 0x41;
    ptrinit[2] = jj;//0x12;
    ptrinit[3] = 0x1B;
    ptrinit[4] = 0x32;
    ptrinit[5] = 0;

    return SendDataToPtr(ptrinit);
}
//打印N个空行
int PrintNSpace(int n)
{
    char Lc_Line[3] = "";
    char Content[300] = "";
    int i;


    Lc_Line[0] = 0x0A;
    Lc_Line[1] = 0x0D;
    Lc_Line[2] = 0;

    if (n <= 0 ) return 0;
    for (i = 1; i <= n ;i++)
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
名  称：int  InitFormat()
返回值：
        成功，0
        失败，返回负值
功  能：
        初始化打印机格式。
*******************************************************************/
int InitFormat()
{
    char Ls_Init[20] = "";
    if(getSerialPort()->isOpen()==false)
    {
        return -1;
    }
    GetPtrInit(Ls_Init);
    return SendDataToPtr(Ls_Init);
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
int PrtInit(int port)
{
//    char szPort[15] = "";
//    char szProfile[128] = "";
    if(port>0)
    {

        // 取消当前缓冲区的打印
        SetClearPtr();
        //初始化打印机
        if (InitFormat() == -1)
        {
            return -1;
        }
        IIntChange = 1;
        return port;
        if(getSerialPort()->open()==0)
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }

}
/*******************************************************************
名  称：int PrtNote(TNoteContent NoteContent)
参  数：
        NoteContent: 打印内容，定义见StructPacked单元或相关定义单元。
返回值：
        成功，返回打印张数(全部为1)
        失败，返回负值 -1
功  能：
        根据参数，调用其它方法实现打印。
*******************************************************************/
int PrtNote(TNoteContent NoteContent)
{
    char Ls_CarType[4] = "";   //车型}
    char Ls_Fare_chn[32] = "";  //金额}
    char Ls_DateTime[32] = "";  //日期时间}
    char Ls_SerialNum[16] = "";  //收费员
    char Ls_EnStationName[32] = "";
    char Ls_ExStationName[32] = "";
    //char Ls_temp[32] = "";
    char Ls_sweight[10] = "",Ls_limitw[10]="", Ls_Over[10] = "";  // 重量
//    char Ls_PtrContent[280] = "";
    char Ls_One[100] = "",Ls_Two[100] = "",Ls_Three[100] = "",Ls_Four[100] = "",Ls_Five[100]="";
    int i,nLen;
    /////////////////////////////
    char Lc_Line[3] = "";
    char Lc_zt[3] = "";
    char Lc_czt[3] = "";
    char Content[600] = "";
    int li_distance = 18;  // 两列字开头距离

    bBusy = true;
    Lc_Line[0] = 0x0A;
    Lc_Line[1] = 0x0D;
    Lc_Line[2] = 0;
    Lc_zt[0] = 0x0E;
    Lc_zt[1] = 0;
    Lc_czt[0] = 0x14;
    Lc_czt[1] = 0;
    /////////////////////////////
    if(getSerialPort()->isOpen()==false)
    {
        return -1;
    }
    if (!m_OnLine)
    {
        return -1;
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
    //第一行     收费员
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
    //第二行
    if (NoteContent.iFeeType == 1)
    {
        for (i = 1; i<=4; i++)
        {
            strcat(Ls_Two, " ");
        }
        strcat(Ls_Two,Ls_limitw);
        for (i = 1; i<=7; i++)
        {
            strcat(Ls_Two, " ");
        }
        strcat(Ls_Two,Ls_sweight);
        //第三行
        for (i = 1; i<=4; i++)
        {
            strcat(Ls_Three, " ");
        }
        strcat(Ls_Three,Ls_sweight);
        for (i = 1; i<=7; i++)
        {
            strcat(Ls_Three, " ");
        }
        strcat(Ls_Three,Ls_Over);
    }
    //第四行
    for (i = 1; i<=4; i++)
    {
        strcat(Ls_Four, " ");
    }
    strcat(Ls_Four,Ls_Fare_chn);
    nLen = li_distance - sizeof(Ls_SerialNum);
    for (i = 1; i<=4; i++)
    {
        strcat(Ls_Four, " ");
    }
    strcat(Ls_Four,Ls_SerialNum);
    //第五行
    strcat(Ls_Five,Ls_DateTime);


    //////// 打印内容  ////////
    if (SetSpaceNum(0x20) !=0)    // 48/72
    {
        qDebug()<<"打印过程中，调用PrtNote，SetSpaceNum(0x30)打印开头空行失败，继续执行！";
    }
    if (PrintNSpace(1) !=0)
    {
        qDebug()<<"打印过程中，调用PrtNote，PrintNSpace(1)打印开头空行失败，继续执行！";
    }
    if (SetSpaceNum(0x19) !=0)    // 48/72
    {
        qDebug()<<"打印过程中，调用PrtNote，SetSpaceNum(0x30)打印开头空行失败，继续执行！";
    }
    if (PrintNSpace(3) !=0)
    {
        qDebug()<<"打印过程中，调用PrtNote，PrintNSpace(1)打印开头空行失败，继续执行！";
    }
    //打印第一行
    if (SetSpaceNum(0x19) !=0)    // 48/72
    {
        qDebug()<<"打印过程中，调用PrtNote，SetSpaceNum(0x30)打印开头空行失败，继续执行！";
    }
    strcpy(Content, Ls_One);
    strcat(Content, Lc_Line);
    if (SendDataToPtr(Content, TRUE) != 0)
    {
        qDebug()<<"打印过程中，调用PrtNote，第一行内容SendDataToPtr失败，继续执行！";
    }
    //打印第二行
    memset(Content, 0, sizeof(Content));
    strcpy(Content, Ls_Two);
    strcat(Content, Lc_Line);

    if (SendDataToPtr(Content, TRUE) != 0)
    {
        qDebug()<<"打印过程中，调用PrtNote，第二行内容SendDataToPtr失败，继续执行！";
    }
    //打印第三行
    memset(Content, 0, sizeof(Content));
    strcpy(Content, Ls_Three);
    strcat(Content, Lc_Line);
    if (SendDataToPtr(Content, TRUE) != 0)
    {
        qDebug()<<"打印过程中，调用PrtNote，第三行内容SendDataToPtr失败，继续执行！";
    }
    //打印第四行
    memset(Content, 0, sizeof(Content));
    strcpy(Content, Ls_Four);
    strcat(Content, Lc_Line);
    if (SendDataToPtr(Content, TRUE) != 0)
    {
        qDebug()<<"打印过程中，调用PrtNote，第四行内容SendDataToPtr失败，继续执行！";
    }
    //打印第五行
    if (SetSpaceNum(0x21) !=0) // 120/72
    {
        qDebug()<<"打印过程中，调用PrtNote，SetSpaceNum(0x78)打印尾部空行失败，继续执行！";
    }
    memset(Content, 0, sizeof(Content));
    strcpy(Content, Ls_Five);
    if (SendDataToPtr(Content, TRUE) != 0)
    {
        qDebug()<<"打印过程中，调用PrtNote，第五行内容SendDataToPtr失败，继续执行！";
    }
    //////// 处理票据尾部  ////////
    if (SetSpaceNum(0x28) !=0) // 120/72
    {
        qDebug()<<"打印过程中，调用PrtNote，SetSpaceNum(0x78)打印尾部空行失败，继续执行！";
    }
    if (PrintNSpace(1) !=0)
    {
        qDebug()<<"打印过程中，调用PrtNote，PrintNSpace(4)打印尾部空行失败，继续执行！";
    }
    if (SetSpaceNum(0x29) !=0) // 120/72
    {
        qDebug()<<"打印过程中，调用PrtNote，SetSpaceNum(0x78)打印尾部空行失败，继续执行！";
    }
    if (PrintNSpace(1) !=0)
    {
        qDebug()<<"打印过程中，调用PrtNote，PrintNSpace(4)打印尾部空行失败，继续执行！";
    }
    bBusy = false;
    return 1;  //打印张数


}
/*******************************************************************
函数声明：int PrtStop()

参数说明：无

返回值：成功，返回0

功  能： 停止发送消息，释放线程资源
*******************************************************************/
int PrtStop()
{
    m_Run = false;
    return 0;
}
/********************************************************************
名  称：int  PrtClose()
参  数：
        无
返回值：
        成功，返回0
        失败，返回负值
功  能：
        释放所有的资源，包括所用到的线程，不再发送消息。
********************************************************************/
int PrtClose()
{
    //停止消息处理
    if (PrtStop() !=0)
        PrtStop();

    if(getSerialPort()->isOpen())
    {
        getSerialPort()->close();
        return 0;
    }
    else
    {
        return -1;
    }
}
/*******************************************************************
函数声明：
        void CALLBACK EXPORT TimerProc
功  能：
        根据间隔时间获得设备状态，如果状态变化，
                发送消息到(HWND)m_ParentHandle窗体。
*******************************************************************/
void GetStatus()
{
    // (CTS, DSR, RLSD) 0失败或故障(false)  1成功(true)  -1没有变化
    int Li_SignalCTS, Li_SignalDSR, Li_SignalRLSD;
    // 发送消息, 状态正常和非正常标记
//    int NormalGreen = 0;
//    int AbnormalRed = 1;
//    LPCSTR PPrt_Status[6] = {"联机", "脱机", "缺纸或故障","关机", "打印失败","故障"};

    if (bBusy){
        return;
    }
    // 获得CTS, DSR, RLSD
    //if (m_Serial.GetCTS())
    if(getSerialPort()->cts())
        Li_SignalCTS = 1;
    else
        Li_SignalCTS = 0;

    // if (m_Serial.GetDSR())
    if(getSerialPort()->dsr())
        Li_SignalDSR = 1;
    else
        Li_SignalDSR = 0;
    //
    //if (m_Serial.GetRLSD())
    if(getSerialPort()->dcd())
        Li_SignalRLSD = 1;
    else
        Li_SignalRLSD = 0;

    // 如果状态和上一次一样，不进行消息的发送
    if (m_OldCTS == Li_SignalCTS && m_OldDSR == Li_SignalDSR &&
            m_OldRLSD == Li_SignalRLSD )
    {
        return;
    }

    //g_Log.ErrorReport("CTS=%d  DSR=%d  RLSD=%d", Li_SignalCTS, Li_SignalDSR, Li_SignalRLSD);

    m_OldCTS = Li_SignalCTS;
    m_OldDSR = Li_SignalDSR;
    m_OldRLSD = Li_SignalRLSD;
    m_OnLine = FALSE;

    // 联机
    if ( Li_SignalCTS == 1 && Li_SignalDSR == 1 )//&& Li_SignalRLSD == 1 )
    {
        //PostMessage((HWND)m_ParentHandle, m_ReturnMsg, int(PPrt_Status[0]), NormalGreen);
        m_Status = 0;
        IIntChange = 1;
        SetClearPtr();
        InitFormat();
        m_OnLine = TRUE;
    }
    // 脱机
    if ( Li_SignalCTS == 0 && Li_SignalDSR == 1) //&& Li_SignalRLSD == 1 )
    {
        // PostMessage((HWND)m_ParentHandle, m_ReturnMsg, int(PPrt_Status[1]), AbnormalRed);
        m_Status = -1;
        SetClearPtr();
        InitFormat();
    }
    // 缺纸或故障
    if ( Li_SignalCTS == 0 && Li_SignalDSR == 1 && Li_SignalRLSD == 0 )
    {
        //PostMessage((HWND)m_ParentHandle, m_ReturnMsg, int(PPrt_Status[2]), AbnormalRed);
        m_Status = -1;
        SetClearPtr();
        InitFormat();
    }
    // 电源关闭
    if ( Li_SignalCTS == 0 && Li_SignalDSR == 0 )//&& Li_SignalRLSD == 0 )
    {
        //PostMessage((HWND)m_ParentHandle, m_ReturnMsg, int(PPrt_Status[3]), AbnormalRed);
        m_Status = -3;
    }
}
/*******************************************************************
名  称：int  PrtStart(void)
参  数：
返回值：
        成功，返回0
        失败，返回-1
功  能：
        启动设备状态检测，开始检测设备状态变化。
*******************************************************************/
int PrtStart(void)
{
    if(m_Run)
        return 0;
    if(getSerialPort()->isOpen()==false)
        return  -1;
    m_OldCTS = -1;
    m_OldDSR = -1;
    m_OldRLSD = -1;
    qDebug()<<"begin GetStatus";
    GetStatus();
    qDebug()<<"end GetStatus";
    m_Run = true;
    qDebug()<<"end PrtStart";
    SetClearPtr();
    InitFormat();
    return 0;
}
/*******************************************************************
名  称：int  PrtGetStatus()
参  数：
返回值：

功  能：
*******************************************************************/
//获取状态
int PrtGetStatus()
{
    if (m_Run == false) return -1;
    qDebug()<<"begin GetStatus";
    GetStatus();
    qDebug()<<"end GetStatus";
    return m_Status;
}
