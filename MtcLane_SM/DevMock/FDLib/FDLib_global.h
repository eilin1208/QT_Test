#ifndef FDLIB_GLOBAL_H
#define FDLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FDLIB_LIBRARY)
#  define FDLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FDLIBSHARED_EXPORT Q_DECL_IMPORT
#endif
#define FDAPI extern "C" FDLIBSHARED_EXPORT
#endif // FDLIB_GLOBAL_H
