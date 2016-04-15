#include "fdlib.h"
#include "FD_Utils.h"
#include <QDebug>
#include "TcpClient/qtcpclient.h"
QByteArray Content;
int length;
QTcpClient* getTcpClient()
{
    static QTcpClient client;
    return &client;
}
int SynData()
{
    length = Content.size();
    int num = getTcpClient()->SendMessage(length,Content.data());
    Content.clear();
    return num;
}
// 设置费显工作模式
int FD_SetMode(int nLaneType, bool bTollByWeight)
{
    g_FD_Data.m_nLaneType = nLaneType;
    g_FD_Data.m_bTollByWeight = bTollByWeight;
    return 0;
}

// 关闭设备，释放占用的资源
int FD_CloseDev()
{
    getTcpClient()->Disconnected();
    return 0;
}
int FD_ClearAll()
{
    Content = QByteArray("清空费显");
    //发送
    return SynData();
}

int FD_DisPlayText(char* szText, int nColor)
{
    char szFmtText[200];
    // NOTE: MakeMultiLineText内部已进行了memset
    //	U创费显一行能显示16个半角字符（8个汉字字符）
    const int nCharsPerLine=16;
    int nLeftChar=MakeMultiLineText(szFmtText, nCharsPerLine, 4, szText);
    qDebug()<<szFmtText<<"szFmtText"<<szText;
    char szLine1[32], szLine2[32], szLine3[32], szLine4[32];
    int nPosition=0;
    memcpy(szLine1, szFmtText+nPosition, nCharsPerLine); szLine1[nCharsPerLine]=0; nPosition+=nCharsPerLine;
    memcpy(szLine2, szFmtText+nPosition, nCharsPerLine); szLine2[nCharsPerLine]=0; nPosition+=nCharsPerLine;
    memcpy(szLine3, szFmtText+nPosition, nCharsPerLine); szLine3[nCharsPerLine]=0; nPosition+=nCharsPerLine;
    memcpy(szLine4, szFmtText+nPosition, nCharsPerLine); szLine4[nCharsPerLine]=0; nPosition+=nCharsPerLine;
    return FD_DisplayLine(szLine1, szLine2, szLine3,szLine4);
}
//设置亮度，U创未实现
int FD_Brightness(int nBrightness)
{
    return -1;
}
// 初始化
int FD_OpenDev(char* lpszConnPara)
{
    if (!getTcpClient()->ConnecttoServer(lpszConnPara))
    {
        getTcpClient()->LogMsg("open fare display fail");
        return -1;
    }
    getTcpClient()->SetType(DEV_FD);

    getTcpClient()->SetFilename("./LogFD.log");
    getTcpClient()->LogMsg("open fare display success");
    return 0;
}

// 黄闪报警灯控制
int FD_AlarmLight(bool bStatus)
{
    // NOTE: 此设备声、光报警为1路
    return FD_AlarmSound(bStatus);
}

// 报警声音控制
int FD_AlarmSound(bool bStatus)
{
    if(bStatus == true)
        Content = QByteArray("报警声音控制为真");
//       strcat(Content,"报警声音控制为真");

    //发送
    return SynData();
}

// 通行信号灯控制
int FD_PassLight(bool bIsGreen)
{
    if(bIsGreen ==true)
    {
        Content.append("通行信号灯为绿色");
//        strcat(Content,"通行信号灯为绿色");
    }
    else
    {
        Content.append("通行信号灯为其他色");
//        strcat(Content,"通行信号灯为其他色");
    }
    //发送
    return SynData();
}

// 显示车型：nVehClass小于0表示清空车型信息（若超出费额显示器显示范围，应清空，不应显示错误值）
int  FD_VehClass(int nVehClass)
{
    g_FD_Data.m_nVehClass=nVehClass;
    if (nVehClass<0)
    {
        g_FD_Data.m_OpTypeStack.Remove(otVehClass);
    }
    else
    {
        g_FD_Data.m_OpTypeStack.Push(otVehClass);
    }
    return 0;
}

// 显示车货总重（kg）、限载（kg）、超限率（千分数）
 int  FD_Weight(int nTotalWeight, int nWeightLimit, int nOverweightRate)
 {
     g_FD_Data.m_nTotalWeight=nTotalWeight;
     g_FD_Data.m_nWeightLimit=nWeightLimit;
     g_FD_Data.m_nOverweightRate=nOverweightRate;
     if (nTotalWeight<0)
     {
         g_FD_Data.m_OpTypeStack.Remove(otWeight);
     }
     else
     {
         g_FD_Data.m_OpTypeStack.Push(otWeight);
     }
     return 0;
 }

// 显示应收金额：nFare小于0表示清空金额信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
 int  FD_Fare(int nFare)
 {
     g_FD_Data.m_nFare=nFare;
     if (nFare<0)
     {
         g_FD_Data.m_OpTypeStack.Remove(otFare);
     }
     else
     {
         g_FD_Data.m_OpTypeStack.Push(otFare);
     }
     return 0;
 }

// 显示扣款前余额：小于0表示清空该段信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
 int  FD_BalanceBeforeDeduct(int nBalanceBeforeDeduct, bool bIsTallyCard)
{
        g_FD_Data.m_nBalanceBeforeDeduct=nBalanceBeforeDeduct;
        g_FD_Data.m_bIsTallyCard=bIsTallyCard;
        if (nBalanceBeforeDeduct<0 && !bIsTallyCard)
        {
                g_FD_Data.m_OpTypeStack.Remove(otBalanceBeforeDeduct);
        }
        else
        {
                g_FD_Data.m_OpTypeStack.Push(otBalanceBeforeDeduct);

        }
        return 0;
}

// 显示实收金额和扣款后余额：小于0表示清空该段信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
 int  FD_BalanceAfterDeduct(int nFarePaid, int nBalanceAfterDeduct, bool bIsTallyCard)//BOOL
{
        g_FD_Data.m_nFarePaid=nFarePaid;
        g_FD_Data.m_nBalanceAfterDeduct=nBalanceAfterDeduct;
        g_FD_Data.m_bIsTallyCard=bIsTallyCard;
        if (nFarePaid<0 && !bIsTallyCard)
        {
                g_FD_Data.m_OpTypeStack.Remove(otBalanceAfterDeduct);
        }
        else
        {
                g_FD_Data.m_OpTypeStack.Push(otBalanceAfterDeduct);
        }
        return 0;
}

// 显示入口站
int  FD_EntrySta(char* lpszEntrySta)
{

    strcpy(g_FD_Data.m_szEntrySta, lpszEntrySta);
    if (sizeof(lpszEntrySta) == 0)
    {
        g_FD_Data.m_OpTypeStack.Remove(otEntrySta);
    }
    else
    {
        g_FD_Data.m_OpTypeStack.Push(otEntrySta);
    }
    return 0;
}

// 显示文字 - 若费额显示器支持编号短语，需在动态连接库内部进行优化,,LPCSTR
int  FD_Text( char* lpszText, int nColor)
{

//    qsnprintf(g_FD_Data.m_szText, sizeof(g_FD_Data.m_szText),"%s", lpszText);
    strcpy(g_FD_Data.m_szText,lpszText);
    g_FD_Data.m_nTextColor= nColor;
    if (sizeof(lpszText)==0)
    {
        g_FD_Data.m_OpTypeStack.Remove(otText);
    }
    else
    {
        g_FD_Data.m_OpTypeStack.Push(otText);
    }
    return 0;
}
 //逐行显示数据
 int FD_DisplayLine(const char *szLine1, const char *szLine2, const char *szLine3, const char *szLine4)
 {
     size_t nStrLen;
     nStrLen=strlen(szLine1);
     if (nStrLen>0)
     {
         Content.append(szLine1);
//         strcat(Content,szLine4);
     }
     nStrLen=strlen(szLine2);
     if (nStrLen>0)
     {
         Content.append(szLine2);
//         strcat(Content,szLine3);
     }
     nStrLen=strlen(szLine3);
     if (nStrLen>0)
     {
         Content.append(szLine3);
//         strcat(Content,szLine2);
     }
     nStrLen=strlen(szLine4);
     if (nStrLen>0)
     {
         Content.append(szLine4);
//         strcat(Content,szLine1);
     }
     //发送
     return SynData();

 }
 //Etc情况下
int FD_SynData_Etc()
 {
     switch(g_FD_Data.m_OpTypeStack.Top())
     {
     case otVehClass:
     case otWeight:
     case otBalanceAfterDeduct:
     case otEntrySta:
             {
                     int nLineId=0;
                     char szLine[4][128];
                     for (int i=0;i<4;i++) szLine[i][0]=0;
                     AddEntryStaLine(nLineId, szLine[nLineId], sizeof(szLine[nLineId]));
                     AddVehClassLine(nLineId, szLine[nLineId], sizeof(szLine[nLineId]));
                     AddFarePaidLine(nLineId, szLine[nLineId], sizeof(szLine[nLineId]));
                     AddBalanceAfterDeductLine(nLineId, szLine[nLineId], sizeof(szLine[nLineId]));
                     return FD_DisplayLine(szLine[0], szLine[1], szLine[2], szLine[3]);
             }
             break;
     case otFare:
     case otBalanceBeforeDeduct:
             {
                     int nLineId=0;
                     char szLine[4][128];
                     for (int i=0;i<4;i++) szLine[i][0]=0;
                     AddEntryStaLine(nLineId, szLine[nLineId], sizeof(szLine[nLineId]));
                     AddVehClassLine(nLineId, szLine[nLineId], sizeof(szLine[nLineId]));
                     AddFareDueLine(nLineId, szLine[nLineId], sizeof(szLine[nLineId]));
                     AddBalanceBeforeDeductLine(nLineId, szLine[nLineId], sizeof(szLine[nLineId]));
                     return FD_DisplayLine(szLine[0], szLine[1], szLine[2], szLine[3]);
             }
             break;
     case otText:
             {
                     // 发送短语
                     return FD_DisPlayText(g_FD_Data.m_szText, g_FD_Data.m_nTextColor);
             }
             break;
     default:
             {
                     // 未知操作，清屏
                     return FD_ClearAll();
             }
             break;
     }
 }
int FD_SynData_Mtc()
{
    if (g_FD_Data.m_nFarePaid>0 || g_FD_Data.m_nBalanceAfterDeduct>0)
    {
            // 已经完成扣款，显示短语信息，如果还有空余空间，显示实收和扣款后余额
            const int nCharsPerLine=16;
            char szFmtText[nCharsPerLine*2+1];
            int nLeftChar=MakeMultiLineText(szFmtText, nCharsPerLine, 2, g_FD_Data.m_szText);
            if (nLeftChar>0)
            {
                    // 2行内无法显示全部短语，故只整屏发送短语
                    return FD_DisPlayText(szFmtText, g_FD_Data.m_nTextColor);
            }
            else
            {
                    int nLineId=0;
                    char szLine[4][128];
                    for (int i=0;i<4;i++) szLine[i][0]=0;
                    if (sizeof(g_FD_Data.m_szText)>0)
                    {
                            char buf[32];
                            memcpy(buf, szFmtText, nCharsPerLine);
                            buf[nCharsPerLine]=0;
                            qsnprintf(szLine[nLineId], strlen(szLine[nLineId]), "%-16s", buf);
                            nLineId++;
                            qsnprintf(szLine[nLineId], strlen(szLine[nLineId]), "%-16s", szFmtText+nCharsPerLine);
                            nLineId++;
                    }
                    AddFarePaidLine(nLineId, szLine[nLineId], strlen(szLine[nLineId]));
                    AddBalanceAfterDeductLine(nLineId, szLine[nLineId], strlen(szLine[nLineId])); 
                    return FD_DisplayLine(szLine[0], szLine[1], szLine[2], szLine[3]);
            }
    }
    int nLastOperation=g_FD_Data.m_OpTypeStack.Top();
    switch(nLastOperation)
    {
    case otVehClass:
    case otWeight:
    case otFare:
    case otBalanceBeforeDeduct:
    case otBalanceAfterDeduct:
    case otEntrySta:
            {
                    // 按重要程度显示显示余额、应收、总重/超限（或车型）、入口
                    int nLineId=0;
                    char szLine[4][128];
                    for (int i=0;i<4;i++) szLine[i][0]=0;
                    if (nLineId<4) AddBalanceBeforeDeductLine(nLineId, szLine[nLineId], strlen(szLine[nLineId]));
                    if (nLineId<4) AddFareDueLine(nLineId, szLine[nLineId], strlen(szLine[nLineId]));
                    if (2 == g_FD_Data.m_nLaneType &&
                            g_FD_Data.m_bTollByWeight &&
                            g_FD_Data.m_nVehClass>10 && g_FD_Data.m_nVehClass<20)
                    {
                            if (nLineId<4) AddTotalWeightLine(nLineId, szLine[nLineId], strlen(szLine[nLineId]));
                            if (nLineId<4) AddOverweightRateLine(nLineId, szLine[nLineId], strlen(szLine[nLineId]));
                    }
                    else
                    {
                            if (nLineId<4) AddVehClassLine(nLineId, szLine[nLineId], strlen(szLine[nLineId]));

                    }
                    if (nLineId<4) AddEntryStaLine(nLineId, szLine[nLineId], strlen(szLine[nLineId])); 
                    return FD_DisplayLine(szLine[0], szLine[1], szLine[2], szLine[3]);
            }
            break;
    case otText:
            {
                    // 发送短语
                    return FD_DisPlayText(g_FD_Data.m_szText, g_FD_Data.m_nTextColor);
            }
            break;
    default:
            {
                    // 未知操作，清屏
                    return FD_ClearAll();
            }
            break;
    }
}
int FD_SynData()
  {
      if (2 == g_FD_Data.m_nLaneType ||
                      1 == g_FD_Data.m_nLaneType)
      {
            return FD_SynData_Mtc();
      }
      else
      {
            return FD_SynData_Etc();
      }
  }


