#ifndef GLDDZIP_GLOBAL_H
#define GLDDZIP_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef GLD_FULLSOURCE
    #if defined(GLDZIP_LIBRARY)
    #  define GLDZIPSHARED_EXPORT Q_DECL_EXPORT
    #else
    #  define GLDZIPSHARED_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define GLDZIPSHARED_EXPORT
#endif

#endif // GLDDZIP_GLOBAL_H
