#ifndef MUEN_ZUKA_GLOBAL_H
#define MUEN_ZUKA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MUEN_ZUKA_LIBRARY)
#  define MUEN_ZUKASHARED_EXPORT Q_DECL_EXPORT
#else
#  define MUEN_ZUKASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MUEN_ZUKA_GLOBAL_H
