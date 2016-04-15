#ifndef PRINTERARM_H
#define PRINTERARM_H
#include "BaseInterface/devbaseinterface.h"
struct TNoteContent
{
  int iFeeType;          // 计费类型 // 0车型 1 计重
  char VehClass[4];            // 车型 －－ 打印内容参数决定，仅车型计费时打印
  int Fare_Num;          // 小写金额(元) －－ 打印带单位元字样
  char Fare_chn[32];          // 大写金额
  char szDateTime[32];          // 日期时间－－打印06.12.18字样的日期
  char SerialNum[16];         // 打印号 （无）
  char EnStationName[32];       // 站名   （无）
  char ExStationName[32];       // 站名   （无）
  char Lane[8];               // 车道号  （无）
  char CollectorID[12];       // 收费员代码 －－打印后三位，已经存储为后三位
  char weight[10];       // 重量  －－ 打印34.34T字样，仅计重时打印
  char LimitW[10];
  char OverP[10];       // 超限率  －－ 打印30.1字样，仅计重时打印
  int sMark;              // 1测试，0非测试
};

typedef int (*Func_PrtInit)(const char* port, const char* settings, bool bUsePort);
typedef int (*Func_PrtStart)();
typedef int (*Func_PrtStop)();
typedef int (*Func_PrtNote)(TNoteContent NoteContent);
typedef int (*Func_PrtGetStatus)();
typedef int (*Func_PrtClose)();


class PrinterArm : public DevBaseInterface
{
    Q_OBJECT
public:
    explicit PrinterArm(const QString& sName,QObject *parent = 0);


signals:
    void printSuccess();
    void printFail();
public slots:
public:
    bool Print();
    bool RePrint();
    bool PrintClose();
    int GetPrintStatus();
    TNoteContent m_NoteContent;

protected:
    virtual bool LoadFunction();
    virtual bool ReleaseFunction();
    virtual bool Init();
    virtual bool Close();
private:
    bool PrintStop();
    bool PrintStart();
    bool PrintInit();
private:
    Func_PrtInit m_pInit;
    Func_PrtStop m_pStop;
    Func_PrtStart m_pStart;
    Func_PrtNote m_pNote;
    Func_PrtGetStatus m_pGetStatus;
    Func_PrtClose m_pClose;
};

#endif // PRINTERARM_H
