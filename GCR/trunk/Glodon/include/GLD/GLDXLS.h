#ifndef GLDXLS_H
#define GLDXLS_H

#include "libxl.h"
#include <qnamespace.h>
#include "GLDTableView_Global.h"

GLDTABLEVIEWSHARED_EXPORT libxl::Book *createXLSBook(bool isXls);

GLDTABLEVIEWSHARED_EXPORT Qt::Alignment XLSHorzAlignmentToQtAlignment(libxl::AlignH align);
GLDTABLEVIEWSHARED_EXPORT Qt::Alignment XLSVertAlignmentToQtAlignment(libxl::AlignV align);
GLDTABLEVIEWSHARED_EXPORT Qt::Alignment XLSAlignmentToQtAlignment(libxl::AlignH horzAlignment, libxl::AlignV vertAlignment);
GLDTABLEVIEWSHARED_EXPORT libxl::AlignV QtAlignmentToXLSVertAlignment(Qt::Alignment align);
GLDTABLEVIEWSHARED_EXPORT libxl::AlignH QtAlignmentToXLSHorzAlignment(Qt::Alignment align);

#endif // GLDXLS_H
