#ifndef VEHPLATEFUNC_H
#define VEHPLATEFUNC_H
#pragma once
#include <QtGui>
#include "LaneType.h"

// 获取车牌长度，汉字视为1位
int GetVehPlateLen(const char *szVehPlate);
// 进行车牌比较
bool CompareVehPlate(const char *szVehPlate1, const char *szVehPlate2);
// 输入车牌颜色代码，返回车牌颜色对应的汉字
const char *GetVehPlateColorName(int nVehPlateColor);
// 获取全车牌
void GetFullVehPlate(const char *szVehPlate, int nColor, QString &strFullVehPlate);
// 将全车牌转换为按旧系统人工车牌输入规则输入的车牌
void FullVehPlate2OldInputVehPlate(char *szOldInputVehPlate,const char *szFullVehPlate);
// 去除车牌特殊符号；为兼容旧系统,'*'作为正常字符
void RemovePlateSpecChar(char *szDest, int nDestSize, const char *szSrc, int nMaxVehPlateLen=MAX_VEHPLATE_LEN);

int TransVehPlateColor(const QString &sColor);

// 将车牌识别结果按格式要求进行转换
bool ConvertGDWPlate(int &nColor, QString &sPlate, const QString &sFullPlate);

#endif // VEHPLATEFUNC_H
