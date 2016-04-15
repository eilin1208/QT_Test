#ifndef NOTEPRINTER_H
#define NOTEPRINTER_H

#include "NotePrinter_global.h"
#include <QIODevice>
struct TNoteContent
{
    int iFeeType;        // 计费类型 // 0车型 1 计重 2免费（不打印票据）
    char CarType[4];     // 车型 －－ 打印内容参数决定，仅车型计费时打印
    int Fare_Num;        // 小写金额(元) －－ 打印带单位元字样
    char Fare_chn[32];    // 大写金额 （无）
    char DateTime[32];    // 日期时间－－打印06.12.18字样的日期
    char SerialNum[16];   // 打印号 （无）
    char EnStationName[32]; // 站名 （无）
    char ExStationName[32]; // 站名 （无）
    char Lane[8];         // 车道号 （无）
    char CollectorID[12]; // 收费员代码 －－打印后三位，已经存储为后三位
    char weight[10];        // 重量  －－ 打印34.34T字样，仅计重时打印
    char limitw[10];
    char OverP[10];       // 超限率  －－ 打印30.1字样，仅计重时打印
    int test;             // 1测试，0非测试
};
DLLEXPORT int InitFormat();
DLLEXPORT int PrtInit(const char* port,const char* settings, bool bUsePort);
DLLEXPORT int PrtNote(TNoteContent NoteContent);
DLLEXPORT int PrtStop();
DLLEXPORT int PrtClose();
DLLEXPORT int PrtGetStatus();
DLLEXPORT int PrtStart(void);
int SendDataToPtr(const char* strSendData, bool bWait = false);

QIODevice* getNotePrinterPort(const char* name, const char* settings, bool usePar);

#endif // NOTEPRINTER_H
