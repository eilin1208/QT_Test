#ifndef FDARM_H
#define FDARM_H

#include "FDArm_global.h"

// 初始化
FDAPI int FD_OpenDev(char* lpszConnPara);
// 设置费显工作模式
FDAPI int FD_SetMode(int nLaneType, bool bTollByWeight);
// 关闭设备，释放占用的资源
FDAPI int FD_CloseDev();
// 显示车型：nVehClass小于0表示清空车型信息（若超出费额显示器显示范围，应清空，不应显示错误值）
FDAPI int FD_VehClass(int nVehClass);
// 显示车货总重（kg）、限载（kg）、超限率（千分数）
FDAPI int FD_Weight(int nTotalWeight, int nWeightLimit, int nOverweightRate);
// 显示应收金额：nFare小于0表示清空金额信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
FDAPI int FD_Fare(int nFare);
// 显示扣款前余额：小于0表示清空该段信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
FDAPI int FD_BalanceBeforeDeduct(int nBalanceBeforeDeduct, bool bIsTallyCard = false);
// 显示实收金额和扣款后余额：小于0表示清空该段信息（单位为分，若超出费额显示器显示范围，应清空，不应显示错误值）
FDAPI int FD_BalanceAfterDeduct(int nFarePaid, int nBalanceAfterDeduct, bool bIsTallyCard = false);
// 显示入口站
FDAPI int FD_EntrySta(char* lpszEntrySta);
// 显示文字 - 若费额显示器支持编号短语，需在动态连接库内部进行优化，颜色分别为SM_Blue、SM_Red、SM_Yellow
FDAPI int FD_Text(char* lpszText, int nColor);
// 同步信息 - 完成实际数据发送
FDAPI int FD_SynData();
// 清空所有信息
FDAPI int FD_ClearAll();
// 设置亮度 - 0最暗，100最亮
FDAPI int FD_Brightness(int nBrightness);
// 黄闪报警灯控制
FDAPI int FD_AlarmLight(bool bStatus);
// 报警声音控制
FDAPI int FD_AlarmSound(bool bStatus);
// 通行信号灯控制
FDAPI int FD_PassLight(bool bIsGreen);
#endif // FDARM_H
