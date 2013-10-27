#ifndef SNDTHREAD_GLOBAL_H
#define SNDTHREAD_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtDebug>

#if defined(SNDTHREAD_LIBRARY)
#  define SNDTHREADSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SNDTHREADSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SNDTHREAD_GLOBAL_H
