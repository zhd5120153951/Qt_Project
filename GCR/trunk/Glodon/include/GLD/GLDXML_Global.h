#ifndef GLDXML_GLOBAL_H
#define GLDXML_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef GLD_FULLSOURCE
    #if defined(GLDXML_LIBRARY)
    #  define GLDXMLSHARED_EXPORT Q_DECL_EXPORT
    #else
    #  define GLDXMLSHARED_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define GLDXMLSHARED_EXPORT
#endif

#endif // GLDXML_GLOBAL_H
