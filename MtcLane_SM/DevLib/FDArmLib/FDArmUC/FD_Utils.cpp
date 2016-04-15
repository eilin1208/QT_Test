#include "FD_Utils.h"
#include <QTextCodec>
#include <QByteArray>
QFD_Data g_FD_Data;
//转换编码方式
const char* UTF8toGB2312(const char* str, int length = -1)
{
    str = QTextCodec::codecForName("GB2312")->fromUnicode(QTextCodec::codecForName("UTF-8")->toUnicode(str));
    return str;
}
bool judge = false;
bool JudgeStaLine()
{
    for(int i = 0; i< sizeof(g_FD_Data.m_szEntrySta);i++)
    {
        if(g_FD_Data.m_szEntrySta[i] != 0)
        {
          return true;
        }
    }
    return false;
}
void AddEntryStaLine(int &nLineId, char *szLine, size_t nLineBufSize)
{
    judge = JudgeStaLine();
    if(judge == true)
    {
        qsnprintf(szLine, nLineBufSize, "入口%8s", g_FD_Data.m_szEntrySta);
        nLineId++;
    }
}

void AddVehClassLine(int &nLineId, char *szLine, size_t nLineBufSize)
{
        if (g_FD_Data.m_nVehClass > 0 && g_FD_Data.m_nVehClass < 10)
        {
            char sVeh[6];
            if(g_FD_Data.m_nVehClass<10 && g_FD_Data.m_nVehClass>0)
            {
                qsnprintf(sVeh, sizeof(sVeh), "%d%s", g_FD_Data.m_nVehClass % 10, UTF8toGB2312("客"));
            }
            else if(g_FD_Data.m_nVehClass > 10 && g_FD_Data.m_nVehClass < 20)
            {
                qsnprintf(sVeh, sizeof(sVeh), "%d%s", g_FD_Data.m_nVehClass, UTF8toGB2312("货"));
            }
            else
            {
                qsnprintf(sVeh, sizeof(sVeh), "%d%s", g_FD_Data.m_nVehClass, UTF8toGB2312("未知"));
            }
            qsnprintf(szLine, nLineBufSize,UTF8toGB2312("车型%8s"),sVeh);
            nLineId++;
        }
}

void AddFarePaidLine(int &nLineId, char *szLine, size_t nLineBufSize)
{
        if (g_FD_Data.m_nFarePaid>0)
        {
                // 注意：实收金额不显示0
            QString sFare;
            sFare = QString::number( g_FD_Data.m_nFarePaid / 100 );
            qsnprintf(szLine, nLineBufSize, UTF8toGB2312("实收%8s"), sFare.toStdString().c_str());
            nLineId++;
        }
}

void AddBalanceAfterDeductLine(int &nLineId, char *szLine, size_t nLineBufSize)
{
        if (g_FD_Data.m_nBalanceAfterDeduct>=0)
        {
            QString sFare;
            sFare = QString::number( g_FD_Data.m_nBalanceAfterDeduct / 100 );
            qsnprintf(szLine, nLineBufSize, UTF8toGB2312("余额%8s"), sFare.toLocal8Bit().data());
            nLineId++;
        }
        else if (g_FD_Data.m_bIsTallyCard)
        {
            qsnprintf(szLine, nLineBufSize,UTF8toGB2312("余额%8s"),"记账卡");
             nLineId++;
        }
}

void AddBalanceBeforeDeductLine(int &nLineId, char *szLine, size_t nLineBufSize)
{
        if (g_FD_Data.m_nBalanceBeforeDeduct>=0)
        {
            QString sFare;
            sFare = QString::number( g_FD_Data.m_nBalanceBeforeDeduct / 100 );
            qsnprintf(szLine, nLineBufSize, UTF8toGB2312("余额%8s"), sFare.toLocal8Bit().data());
            nLineId++;
        }
        else if (g_FD_Data.m_bIsTallyCard)
        {
            qsnprintf(szLine, nLineBufSize, UTF8toGB2312("余额%8s"),"记账卡");
            nLineId++;
        }
}

void AddFareDueLine(int &nLineId, char *szLine, size_t nLineBufSize)
{
        if (g_FD_Data.m_nFare > 0)
        {
                // 注意：应收金额不显示0
                int nDue=g_FD_Data.m_nFare/100;
                if (0!=g_FD_Data.m_nFare%100) nDue++;
                QString sFare;
                sFare = QString::number(nDue);
                qsnprintf(szLine, nLineBufSize, UTF8toGB2312("应收%8s"), sFare.toLocal8Bit().data());
                nLineId++;
        }
}

void AddTotalWeightLine(int &nLineId, char *szLine, size_t nLineBufSize)
{
        if (g_FD_Data.m_nTotalWeight>0)
        {
                // NOTE: 传入的重量精度应该是100kg，不必考虑取整问题
            char buf[128];
            qsnprintf(buf, sizeof(buf),"%d.%d吨",
                    g_FD_Data.m_nTotalWeight/1000, (g_FD_Data.m_nTotalWeight%1000)/100);
            qsnprintf(szLine, nLineBufSize, UTF8toGB2312("总重%8s"), buf);
            nLineId++;
        }
}

void AddOverweightRateLine(int &nLineId, char *szLine, size_t nLineBufSize)
{
        if (g_FD_Data.m_nOverweightRate>0)
        {
            char buf[128];
            qsnprintf(buf, sizeof(buf),  UTF8toGB2312("%d.%d%%"),
                        g_FD_Data.m_nOverweightRate/10, g_FD_Data.m_nOverweightRate%10);
            qsnprintf(szLine, nLineBufSize, UTF8toGB2312("超限%8s"), buf);
            nLineId++;
        }
}


// 构造字符串
/*
nColCount：	列数，至少为2
nRowCount：	行数
szOutput：	输出，已分配空间应大于nColCount*nRowCount+1
szInput：	输入
返回：		被截断字符个数
*/
//将char* 分为4行
int MakeMultiLineText(char *szOutput, int nColCount, int nRowCount, const char *szInput)
{
        int nMaxOutputLen=nColCount*nRowCount;
        // NOTE: memset是必须的，以方便调用者取不同行的数据
        memset(szOutput, 0, nMaxOutputLen+1);
        int nInputLen=strlen(szInput);
        if (nColCount<2 || nRowCount<1)
        {
                return nInputLen;
        }
        int nInIndex=0, nOutIndex=0;
        char c;
        while (nInIndex<nInputLen && nOutIndex<nMaxOutputLen)
        {
                c=szInput[nInIndex];
                if (c>0)
                {
                        szOutput[nOutIndex]=c;
                        nOutIndex++;
                        nInIndex++;
                }
                else
                {
                        int nLeftSpace=nMaxOutputLen-nOutIndex;
                        if (0==(nOutIndex+1)%nColCount)
                        {
                                if (nLeftSpace<3) break;
                                // 遇到行尾需要补充一个空格，以免显示半个汉字
                                szOutput[nOutIndex]=' ';
                                nOutIndex++;
                        }
                        else
                        {
                                if (nLeftSpace<2) break;
                        }
                        // 双字节字符连着复制2个
                        szOutput[nOutIndex]=c;
                        nOutIndex++;
                        nInIndex++;
                        c=szInput[nInIndex];
                        szOutput[nOutIndex]=c;
                        nOutIndex++;
                        nInIndex++;
                }
        }
        // 返回剩余字节数
        if (nInputLen>nInIndex)
        {
                return nInputLen-nInIndex;
        }
        else
        {
                return 0;
        }
}
