#ifndef VPRLIB_GLOBAL_H
#define VPRLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(VPRLIB_LIBRARY)
#  define VPRLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define VPRLIBSHARED_EXPORT Q_DECL_IMPORT
#endif
#define DLLEXPORT extern "C" VPRLIBSHARED_EXPORT
#endif // VPRLIB_GLOBAL_H
