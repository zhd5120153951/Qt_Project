#ifndef GLDWIDGET_GLOBAL_H
#define GLDWIDGET_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef GLD_FULLSOURCE
    #if defined(GLDWIDGET_LIBRARY)
    #  define GLDWIDGETSHARED_EXPORT Q_DECL_EXPORT
    #else
    #  define GLDWIDGETSHARED_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define GLDWIDGETSHARED_EXPORT
#endif

#endif // GLDWIDGET_GLOBAL_H
