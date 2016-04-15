#ifndef IOLIB_GLOBAL_H
#define IOLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(IOLIB_LIBRARY)
#  define IOLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define IOLIBSHARED_EXPORT Q_DECL_IMPORT
#endif
#define IOAPI extern "C" IOLIBSHARED_EXPORT
#endif // IOLIB_GLOBAL_H
