#ifndef TAKAO_GLOBAL_HPP
#define TAKAO_GLOBAL_HPP

#include <QtCore/qglobal.h>

#if defined(TAKAO_LIBRARY)
#  define TAKAOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TAKAOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TAKAO_GLOBAL_HPP
