#ifndef GLDCOMMON_GLOBAL_H
#define GLDCOMMON_GLOBAL_H

#include <QtGlobal>

#ifndef GLD_FULLSOURCE
    #if defined(GLDCOMMON_LIBRARY)
    #  define GLDCOMMONSHARED_EXPORT Q_DECL_EXPORT
    #else
    #  define GLDCOMMONSHARED_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define GLDCOMMONSHARED_EXPORT
#endif

#endif // GLDCOMMON_GLOBAL_H
