#include "cardmgrlib.h"
#include "datadef.h"
#include "cardmgrhandler.h"
CardMgrHandler g_handler;
bool Init(const char *sCom, int nBps, QObject *pFunc)
{
    return g_handler.initImpl(sCom, nBps, pFunc);
}

bool UpCard(int nMgrIndex)
{
    return g_handler.upCardImpl(nMgrIndex);
}

bool BadCard(int nMgrIndex)
{
    return g_handler.badCardImpl(nMgrIndex);
}

bool SetCardCount(int nMgrIndex, int nCardCount)
{
    return g_handler.setCardCountImpl(nMgrIndex, nCardCount);
}

void ClearAllBadCard()
{
    g_handler.clearAllBadCardImpl();
}

bool GetBoxInfo(int nBoxCode, int &nBoxId, int &nCount)
{
    return g_handler.getBoxInfoImpl(nBoxCode, nBoxId, nCount);
}

bool GetChannelInfo(int nBoxCode, int &nBoxId, int &nCount, bool &bHasCardOn, char &curStatus)
{
    return g_handler.getChannelInfoImpl(nBoxCode, nBoxId, nCount, bHasCardOn, curStatus);
}
