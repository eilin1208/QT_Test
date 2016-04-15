﻿#ifndef VPRARMLIB_UC_GLOBAL_H
#define VPRARMLIB_UC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(VPRARMLIB_UC_LIBRARY)
#  define VPRARMLIB_UCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define VPRARMLIB_UCSHARED_EXPORT Q_DECL_IMPORT
#endif
#define DLLEXPORT extern "C" VPRARMLIB_UCSHARED_EXPORT
#endif // VPRARMLIB_UC_GLOBAL_H