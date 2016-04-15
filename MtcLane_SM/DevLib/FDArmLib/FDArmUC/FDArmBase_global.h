#ifndef FDARMUC_GLOBAL_H
#define FDARMUC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FDARMUC_LIBRARY)
#  define FDARMUCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FDARMUCSHARED_EXPORT Q_DECL_IMPORT
#endif
#define FDAPI extern "C" FDARMUCSHARED_EXPORT
#endif // FDARMUC_GLOBAL_H
