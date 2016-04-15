#ifndef FDARM_GLOBAL_H
#define FDARM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FDARM_LIBRARY)
#  define FDARMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FDARMSHARED_EXPORT Q_DECL_IMPORT
#endif
#define FDAPI extern "C" FDARMSHARED_EXPORT
#endif // FDARM_GLOBAL_H
