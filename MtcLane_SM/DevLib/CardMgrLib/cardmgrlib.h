#ifndef CARDMGRLIB_H
#define CARDMGRLIB_H

#include "CardMgrLib_global.h"
class QObject;
//初始化函数
API bool Init(const char* sCom, int nBps, QObject* pFunc);

//出卡
API bool UpCard(int nMgrIndex);

//坏卡操作
API bool BadCard(int nMgrIndex);

//设置卡总数
API bool SetCardCount(int nMgrIndex, int nCardCount);

//清除所有坏卡
API void ClearAllBadCard();

//获得卡盒信息
API bool GetBoxInfo(int nBoxCode, int& nBoxId, int& nCount);

//获得频道信息
API bool GetChannelInfo(int nBoxCode, int& nBoxId, int& nCount, bool& bHasCardOn, char& curStatus);

#endif // CARDMGRLIB_H
