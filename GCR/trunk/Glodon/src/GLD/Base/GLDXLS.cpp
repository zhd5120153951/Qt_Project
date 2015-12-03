#include "GLDXLS.h"
#include "GLDStrUtils.h"
const GString c_LibXlUser = TRANS_STRING("ÑÓÆ½ ¼Ö");

libxl::Book *createXLSBook(bool isXls)
{
    libxl::Book* result;
    if (isXls)
    {
        result = xlCreateBook();
    }
    else
    {
        result = xlCreateXMLBook();
    }
#ifdef _UNICODE
    result->setKey((const wchar_t *)c_LibXlUser.constData(), L"windows-232f2f0705c9e4006db5636aaao7dbt8");
#else
    result->setKey("ÑÓÆ½ ¼Ö", "windows-232f2f0705c9e4006db5636aaao7dbt8");
#endif
    return result;
}

Qt::Alignment XLSHorzAlignmentToQtAlignment(libxl::AlignH align)
{
    switch (align)
    {
    case libxl::ALIGNH_GENERAL:
        return 0;
    case libxl::ALIGNH_LEFT:
        return Qt::AlignLeft | Qt::AlignJustify;
    case libxl::ALIGNH_RIGHT:
        return Qt::AlignRight | Qt::AlignJustify;
    case libxl::ALIGNH_CENTER:
    case libxl::ALIGNH_MERGE:
    case libxl::ALIGNH_DISTRIBUTED:
        return Qt::AlignHCenter | Qt::AlignJustify;
    default:
        return 0;
    }
}

Qt::Alignment XLSVertAlignmentToQtAlignment(libxl::AlignV align)
{
    switch (align)
    {
    case libxl::ALIGNV_TOP:
        return Qt::AlignTop | Qt::AlignJustify;
    case libxl::ALIGNV_BOTTOM:
        return Qt::AlignBottom | Qt::AlignJustify;
    case libxl::ALIGNV_CENTER:
    case libxl::ALIGNV_DISTRIBUTED:
        return Qt::AlignVCenter | Qt::AlignJustify;
    default:
        return 0;
    }
}

Qt::Alignment XLSAlignmentToQtAlignment(libxl::AlignH horzAlignment, libxl::AlignV vertAlignment)
{
    return XLSHorzAlignmentToQtAlignment(horzAlignment) | XLSVertAlignmentToQtAlignment(vertAlignment);
}

libxl::AlignV QtAlignmentToXLSVertAlignment(Qt::Alignment align)
{
    if (Qt::AlignTop == (Qt::AlignTop & align))
    {
        return libxl::ALIGNV_TOP;
    }
    else if (Qt::AlignBottom == (Qt::AlignBottom & align))
    {
        return libxl::ALIGNV_BOTTOM;
    }
    else if (Qt::AlignVCenter == (Qt::AlignVCenter & align))
    {
        return libxl::ALIGNV_CENTER;
    }
    else if (Qt::AlignJustify == (Qt::AlignJustify & align))
    {
        return libxl::ALIGNV_JUSTIFY;
    }
    else
        return libxl::ALIGNV_CENTER;
}

libxl::AlignH QtAlignmentToXLSHorzAlignment(Qt::Alignment align)
{
    if (0x00 == (0x0f & align))
    {
        return libxl::ALIGNH_GENERAL;
    }
    else if (Qt::AlignRight == (Qt::AlignRight & align))
    {
        return libxl::ALIGNH_RIGHT;
    }
    else if (Qt::AlignLeft == (Qt::AlignLeft & align))
    {
        return libxl::ALIGNH_LEFT;
    }
    else if (Qt::AlignJustify == (Qt::AlignJustify & align))
    {
        return libxl::ALIGNH_JUSTIFY;
    }
    else
        return libxl::ALIGNH_CENTER;
}
