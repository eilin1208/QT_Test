#ifndef IOLIB_H
#define IOLIB_H
#include "IOLib_global.h"
#include "TCPClient/qtcpclient.h"
typedef void (*IOCallBack)(unsigned int status);
typedef struct {
    unsigned int a:1;//Loop1
    unsigned int b:1;//Loop2
    unsigned int c:1;//up
    unsigned int d:1;//down
    unsigned int e:1;//front
    unsigned int f:1;//back
    unsigned int g:1;//alarm
    unsigned int h:1;//setout2

    unsigned int i:1;//canopygreen
    unsigned int j:1;//canopyred
    unsigned int k:1;//passgreen
    unsigned int l:1;//passred
    unsigned int m:1;//down
    unsigned int n:1;//up
    unsigned int o:1;//light
    unsigned int p:1;//sound
    unsigned char aaa;
    unsigned char bbb;
}bitint;
typedef union{
    bitint bit_data;
    uint data;
}Status;
IOAPI int IO_Init(IOCallBack Func,int nType);
IOAPI void IO_Close();
IOAPI void IO_SetAlarm(int var);
IOAPI void IO_SetCanopy(int var);
IOAPI void IO_SetBar(int var);
IOAPI void IO_SetFogLamp(int var);
IOAPI void IO_SetLaneLamp(int var);
IOAPI void IO_SetOut2(int var);
void callBack(QByteArray &data);
#endif // IOLIB_H
