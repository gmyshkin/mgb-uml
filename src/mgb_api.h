#ifndef MGB_API_H
#define MGB_API_H

#include <QtCore/qglobal.h>

#ifdef MGB_CORE_BUILD
#  define MGB_API Q_DECL_EXPORT
#else
#  define MGB_API Q_DECL_IMPORT
#endif

#endif // MGB_API_H