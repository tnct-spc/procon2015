#ifndef TENGU_GLOBAL_H
#define TENGU_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TENGU_LIBRARY)
#  define TENGUSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TENGUSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TENGU_GLOBAL_H
