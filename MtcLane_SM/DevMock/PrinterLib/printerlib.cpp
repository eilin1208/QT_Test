#include "printerlib.h"
#include "TcpClient/qtcpclient.h"
bool g_Send;
int length;
QByteArray Content;
//转换编码方式
const char* GB2312toUTF8(const char* str, int length = -1)
{
     QByteArray temp,temp2;
     temp = QByteArray(str);
     QString message;
     message = QTextCodec::codecForName("UTF-8")->fromUnicode(QTextCodec::codecForName("GB2312")->toUnicode(temp));
     temp2 = message.toLocal8Bit();
     str = temp2.data();
     return str;
}
QTcpClient* getTcpClient()
{
    static QTcpClient client;
    return &client;
}
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
/*******************************************************************
名  称：int PrtInit(const char* port, const char* settings, bool bUsePort)
参  数：

返回值：成功，返回0
       失败，返回负值

功  能：
       初始化设备（打印机）
备注：由于应用tcp传输数据所以没有端口等，只借用其函数形式
*******************************************************************/
int PrtInit(const char* port, const char* settings, bool bUsePort)
{
    if(getTcpClient()->isConnected())
    {
        g_Send = false;
        return 1;
    }
    else
    {
        getTcpClient()->ConnecttoServer(port);
        getTcpClient()->SetFilename(QString("./LogPrt.log"));
        getTcpClient()->SetType(DEV_PRT);
        if (!getTcpClient()->isConnected())
            return -1;
        g_Send = false;
        return 1;
    }
}
/*******************************************************************
函数声明：int PrtStop()

参数说明：无

返回值：成功，返回0

功  能： 停止发送消息，释放线程资源

备注：仅返回0,并将g_Send设为false
*******************************************************************/
int PrtStop()
{
    g_Send = false;
    Content.clear();
    return 0;
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
    if(getTcpClient()->isConnected() == true)
    {
        return 0;
    }
    else
    {
        return -1;
    }
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
    getTcpClient()->Disconnected();
    return 0;
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
    g_Send = true;
    if(getTcpClient()->isConnected() == false)
    {
        return -1;
    }
    char Ls_Veh[4] = "";   //车型}
    char Ls_Fare_chn[32] = "";  //金额}
    char Ls_DateTime[32] = "";  //日期时间}
    char Ls_SerialNum[16] = "";  //收费员
    char Ls_EnStationName[32] = "";
    char Ls_ExStationName[32] = "";
    char Ls_sweight[10] = "",Ls_limitw[10]="", Ls_Over[10] = "";  // 重量
    char Ls_One[100] = "",Ls_Two[100] = "",Ls_Three[100] = "",Ls_Four[100] = "",Ls_Five[100]="";
    int i,nLen;
    // 车型
    strcpy(Ls_Veh, GB2312toUTF8(NoteContent.VehClass));
    // 打印小写金额
    if (NoteContent.sMark == 1)	 // 1测试，0非测试
        qsnprintf(Ls_Fare_chn,32, "%.2d测试", NoteContent.Fare_Num/100);
    else
        qsnprintf(Ls_Fare_chn,32, "%.2d", NoteContent.Fare_Num/100);
    // 日期
    strcpy(Ls_DateTime, NoteContent.szDateTime);
    // 收费员
    strcpy(Ls_SerialNum, NoteContent.CollectorID);
    //入口站
    strcpy(Ls_EnStationName,GB2312toUTF8(NoteContent.EnStationName));
    //出口站
    strcpy(Ls_ExStationName,GB2312toUTF8(NoteContent.ExStationName));
    // 重量
    strcpy(Ls_sweight, NoteContent.weight);
    //限载
    strcpy(Ls_limitw,NoteContent.LimitW);
    // 超限率
    strcpy(Ls_Over, NoteContent.OverP);
    //////// 整理打印内容  ////////
    //第一行 车型
    strcat(Ls_One,"车型：");
    strcat(Ls_One, Ls_Veh);
    for (i = 1; i<=6; i++)
    {
        strcat(Ls_One, " ");
    }
    strcat(Ls_One,"入口站： ");
    strcat(Ls_One,Ls_EnStationName);
    for (i = 1; i<=6; i++)
    {
        strcat(Ls_One, " ");
    }
    strcat(Ls_One,"出口站： ");
    strcat(Ls_One,Ls_ExStationName);

    //第二行 收费金额和收费员号
    for (i = 1; i<=4; i++)
    {
        strcat(Ls_Two, " ");
    }
    strcat(Ls_Two,"金额： ");
    strcat(Ls_Two,Ls_Fare_chn);
    nLen = 100 - sizeof(Ls_SerialNum);
    for (i = 1; i<=4; i++)
    {
        strcat(Ls_Two, " ");
    }
    strcat(Ls_Two,"收费员： ");
    strcat(Ls_Two,Ls_SerialNum);
    //第三行 计重
    if (NoteContent.iFeeType == 1)
    {
        for (i = 1; i<=4; i++)
        {
            strcat(Ls_Three, " ");
        }
        strcat(Ls_Three,"限载： ");
        strcat(Ls_Three,Ls_limitw);
        for (i = 1; i<=7; i++)
        {
            strcat(Ls_Three, " ");
        }
        strcat(Ls_Three,"总重： ");
        strcat(Ls_Three,Ls_sweight);
        //第四行
        for (i = 1; i<=4; i++)
        {
            strcat(Ls_Four, " ");
        }
        strcat(Ls_Four,"总重： ");
        strcat(Ls_Four,Ls_sweight);
        for (i = 1; i<=7; i++)
        {
            strcat(Ls_Four, " ");
        }
        strcat(Ls_Four,"超限率： ");
        strcat(Ls_Four,Ls_Over);
    }
    //第五行
    strcat(Ls_Five,"时间： ");
    strcat(Ls_Five,Ls_DateTime);
    //////// 打印内容  ////////
    //打印第一行
    Content.clear();
    Content.append(Ls_One);
    Content.append("\n");
    Content.append(Ls_Two);
    Content.append("\n");
    Content.append(Ls_Three);
    Content.append("\n");
    Content.append(Ls_Four);
    Content.append("\n");
    Content.append(Ls_Five);
    Content.append("\n");
    getTcpClient()->LogMsg(Content.data());
    length = Content.size();
    getTcpClient()->SendMessage(length,Content.data());
    Content.clear();
    return 1;
}
int PrtGetStatus()
{
    if(getTcpClient()->isConnected() == true)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}
