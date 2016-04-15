#ifndef WTLIB_GLOBAL_H
#define WTLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WTLIB_LIBRARY)
#  define WTLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define WTLIBSHARED_EXPORT Q_DECL_IMPORT
#endif
#define WTAPI extern "C" WTLIBSHARED_EXPORT
#endif // WTLIB_GLOBAL_H
