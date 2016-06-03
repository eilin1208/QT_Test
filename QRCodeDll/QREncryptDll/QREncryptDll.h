#ifndef QRENCRYPTDLL_H
#define QRENCRYPTDLL_H

#include "QREncryptDll_global.h"

extern "C" QRENCRYPTDLLSHARED_EXPORT void QRE_Init();
//设置需要生成二维码的内容
extern "C" QRENCRYPTDLLSHARED_EXPORT void QRE_SetQRString(const char *sMsg, int nLen);
//生成二维码图片
extern "C" QRENCRYPTDLLSHARED_EXPORT int QRE_genQrencode(const char *sImageFileName);

#endif // QRENCRYPTDLL_H
