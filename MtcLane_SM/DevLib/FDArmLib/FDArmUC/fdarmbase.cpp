#include "fdarmbase.h"
#include "fdinterface.h"
//GET BCC校验码
char GetBcc(const char* szSrc, int nLength)
{
    char nRlt = szSrc[0];
    for(int i = 1; i < nLength; i++)
    {
        nRlt = nRlt ^ szSrc[i];
    }
    return nRlt;
}


// 设置费显工作模式
int FD_SetMode(int nLaneType, bool bTollByWeight)
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->setModeImpl(nLaneType, bTollByWeight);
}

// 关闭设备，释放占用的资源
int FD_CloseDev()
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->closeDevImpl();
}
int FD_ClearAll()
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->clearAllImpl();
}

//设置亮度，U创未实现
int FD_Brightness(int nBrightness)
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->brightnessImpl(nBrightness);
}

// 初始化
int FD_OpenDev(char* lpszConnPara)
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->openDevImpl(lpszConnPara);
}

// 黄闪报警灯控制
int FD_AlarmLight(bool bStatus)
{
    // NOTE: 此设备声、光报警为1路
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->alarmLightImpl(bStatus);
}

// 报警声音控制
int FD_AlarmSound(bool bStatus)
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->alarmSoundImpl(bStatus);
}

// 通行信号灯控制
int FD_PassLight(bool bIsGreen)
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->passLightImpl(bIsGreen);
}

// 显示车型：nVehClass小于0表示清空车型信息（若超出费额显示器显示范围，应清空，不应显示错误值）
int  FD_VehClass(int nVehClass)
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->vehClassImpl(nVehClass);
}

// 显示车货总重（kg）、限载（kg）、超限率（千分数）
int  FD_Weight(int nTotalWeight, int nWeightLimit, int nOverweightRate)
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->vehWeightImpl(nTotalWeight, nWeightLimit, nOverweightRate);
}

// 显示应收金额：nFare小于0表示清空金额信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
int  FD_Fare(int nFare)
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->fareImpl(nFare);
}

// 显示扣款前余额：小于0表示清空该段信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
int  FD_BalanceBeforeDeduct(int nBalanceBeforeDeduct, bool bIsTallyCard)
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->balanceBeforeDeductImpl(nBalanceBeforeDeduct, bIsTallyCard);
}

// 显示实收金额和扣款后余额：小于0表示清空该段信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
int  FD_BalanceAfterDeduct(int nFarePaid, int nBalanceAfterDeduct, bool bIsTallyCard)//BOOL
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->balanceAfterDeductImpl(nFarePaid, nBalanceAfterDeduct, bIsTallyCard);
}

// 显示入口站
int  FD_EntrySta(const char* lpszEntrySta)
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->entryStaImpl(lpszEntrySta);
}


int  FD_Text( char* lpszText, int nColor)
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->textImpl(lpszText, nColor);
}

int FD_SynData()
{
    if(getInstance() == NULL)
    {
        return -1;
    }
    return getInstance()->synDataImpl();
}


