#ifndef GLDCRYPT_GLOBAL_H
#define GLDCRYPT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef GLD_FULLSOURCE
    #if defined(GLDCRYPT_LIBRARY)
    #  define GLDCRYPTSHARED_EXPORT Q_DECL_EXPORT
    #else
    #  define GLDCRYPTSHARED_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define GLDCRYPTSHARED_EXPORT
#endif

#endif // GLDCRYPT_GLOBAL_H
