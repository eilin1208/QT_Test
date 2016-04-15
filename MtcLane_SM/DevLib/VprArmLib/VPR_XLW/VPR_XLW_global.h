#ifndef VPR_XLW_GLOBAL_H
#define VPR_XLW_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(VPR_XLW_LIBRARY)
#  define VPR_XLWSHARED_EXPORT Q_DECL_EXPORT
#else
#  define VPR_XLWSHARED_EXPORT Q_DECL_IMPORT
#endif
#define DLLEXPORT extern "C" VPR_XLWSHARED_EXPORT
#endif // VPR_XLW_GLOBAL_H
#ifndef S_OK
#define S_OK							((HRESULT)0x00000000L)
#endif
