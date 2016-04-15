#ifndef IOBOARDDEF_H
#define IOBOARDDEF_H
#include <QtCore>
typedef void(*IOCallBack)(unsigned int status);
typedef struct {
    unsigned int a:1;
    unsigned int b:1;
    unsigned int c:1;
    unsigned int d:1;
    unsigned int e:1;
    unsigned int f:1;
    unsigned int g:1;
    unsigned int h:1;

    unsigned int i:1;
    unsigned int j:1;
    unsigned int k:1;
    unsigned int l:1;
    unsigned int m:1;
    unsigned int n:1;
    unsigned int o:1;
    unsigned int p:1;
    unsigned char aaa;
    unsigned char bbb;
}bitint;
typedef union{
    bitint bit_data;
    uint data;
}Status;

enum enDevice
{
    enBar,
    enCanLight,
    enCrossLight
};
enum emInputDevice
{
    LineCap =0x08,	//抓拍线圈
    LinePass =0x04,	//通过线圈
    LineHgihtBar = 0x01	//光栅信号
};

enum DIGITAL_IN
{
    DI_LoopTrig1,	//感应线圈
    DI_LoopTrig2,	//存在线圈
    DI_LoopFront,       //抓拍线圈
    DI_LoopBack,        //通过线圈
    DI_BarrierDown,
    DI_BarrierUp,
    DI_AlarmDetector
};

enum DIGITAL_OUT
{
    DO_CanopyLightGreen,
    DO_CanopyLightRed,
    DO_PassLightGreen,
    DO_PassLightRed,
    DO_BarrierDown,
    DO_BarrierUp,
    DO_AlarmLight,
    DO_AlarmSound
};

enum INPUTCHANGE
{
    FLOOPCAR = 1,
    BLOOPCAR,
    LOOPTrig1CAR,
    LOOPTrig2CAR,
    BARDOWN,
    BARUP,
    FLOOPNOCAR,
    BLOOPNOCAR,
    LOOPTrig1NOCAR,
    LOOPTrig2NOCAR,
    BARUPING,
    BARDOWNING
};

typedef void (*IO_Close)(void);
typedef void (*IO_SetAlarm)(int var);
typedef void (*IO_SetCanopy)(int var);
typedef void (*IO_SetBar)(int var);
typedef void (*IO_SetFogLamp)(int var);
typedef void (*IO_SetLaneLamp)(int var);
typedef void (*IO_SetOut2)(int var);
typedef int (*IO_Init)(IOCallBack Func,int nType);
#endif // IOBOARDDEF_H
