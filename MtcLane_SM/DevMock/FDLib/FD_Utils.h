#ifndef FD_UTILS_H
#define FD_UTILS_H
#include <QList>

class QOperationStack
{
public:
        void Push(int n)
        {
                m_List.push_front(n);
        }
        void Remove(int n)
        {
                m_List.remove(n);
        }
        int Top()
        {
                if (m_List.empty())
                {
                        return -1;
                }
                return m_List.front();
        }
        void Clear()
        {
                m_List.clear();
        }
protected:
        std::list<int> m_List;
};

enum ENUM_OP_TYPE
{
        otClear,
        otVehClass,
        otWeight,
        otFare,
        otBalanceBeforeDeduct,
        otBalanceAfterDeduct,
        otEntrySta,
        otText
};

class QFD_Data
{
public:
        // 车道类型
        int m_nLaneType;
        // 是否计重收费
        bool m_bTollByWeight;
        // 最近一次的操作类型
        QOperationStack m_OpTypeStack;
        // 车型
        int m_nVehClass;
        // 车货总重（kg）
        int m_nTotalWeight;
        // 限载（kg）
        int m_nWeightLimit;
        // 超限率（千分数）
        int m_nOverweightRate;
        // 应收金额
        int m_nFare;
        // 扣款前余额
        int m_nBalanceBeforeDeduct;
        // 实收金额
        int m_nFarePaid;
        // 扣款后余额
        int m_nBalanceAfterDeduct;
        // 是否为记账卡
        bool m_bIsTallyCard;
        // 入口站
        char m_szEntrySta[64];
        // 文字
        char m_szText[256];
        // 文字颜色
        int m_nTextColor;
public:
        QFD_Data()
        {
                m_nLaneType=0;
                m_bTollByWeight = true;
                m_nTextColor= 0;
                ClearData();
        }
        void ClearData()
        {
                m_OpTypeStack.Clear();
                m_nVehClass=-1;
                m_nTotalWeight=-1;
                m_nWeightLimit=-1;
                m_nOverweightRate=-1;
                m_nFare=-1;
                m_nBalanceBeforeDeduct=-1;
                m_nFarePaid=-1;
                m_nBalanceAfterDeduct=-1;
                m_bIsTallyCard= false;
                memset(m_szEntrySta, 0, sizeof(m_szEntrySta));
                memset(m_szText, 0, sizeof(m_szText));
        }
};

int MakeMultiLineText(char *szOutput, int nColCount, int nRowCount, const char *szInput);
//在原先FD_ETC_UC.cpp中
void AddEntryStaLine(int &nLineId, char *szLine, size_t nLineBufSize);
void AddVehClassLine(int &nLineId, char *szLine, size_t nLineBufSize);
void AddFarePaidLine(int &nLineId, char *szLine, size_t nLineBufSize);
void AddBalanceAfterDeductLine(int &nLineId, char *szLine, size_t nLineBufSize);
void AddBalanceBeforeDeductLine(int &nLineId, char *szLine, size_t nLineBufSize);
void AddFareDueLine(int &nLineId, char *szLine, size_t nLineBufSize);
void AddTotalWeightLine(int &nLineId, char *szLine, size_t nLineBufSize);
void AddOverweightRateLine(int &nLineId, char *szLine, size_t nLineBufSize);

extern QFD_Data g_FD_Data;

#endif // FD_UTILS_H
