#ifndef GLDEXCELGRIDINTF_H
#define GLDEXCELGRIDINTF_H

#include "GLDString.h"
#include "GLDVariant.h"
#include "GLDComptr.h"
#include "GLDUnknwn.h"

interface IGEGExcelGrid;
interface IGEGRecordIterator;
interface IGEGFieldIterator;
interface IGEGRecord;
interface IGEGField;

enum GEGDataType
{
    GEGdtEmply,
    GEGdtNumber,
    GEGdtString,
    GEGdtBool,
    GEGdtBlank,
    GEGdtError
};

struct GEGValue
{
    GVariant value;
    GEGDataType type;
};

// GEGExcelGrid 接口
DEFINE_IID(IGEGExcelGrid, "{A0AC57BF-112A-4859-A900-1CA7A497AED7}");
DECLARE_INTERFACE_(IGEGExcelGrid, IUnknown)
{
    GLDMETHOD(IGEGRecordIterator *, recordIterator)() PURE;
    GLDMETHOD(IGEGFieldIterator *, fieldIterator)() PURE;
    GLDMETHOD(IGEGRecordIterator *, createRecordIterator)() PURE;
    GLDMETHOD(IGEGFieldIterator *, createFieldIterator)() PURE;
};

// GEGRecordIterator 接口
DEFINE_IID(IGEGRecordIterator, "{9983BCDD-0F29-421E-AD3C-34F7B600A417}");
DECLARE_INTERFACE_(IGEGRecordIterator, IUnknown)
{
    GLDMETHOD(void, first)() PURE;
    GLDMETHOD(void, next)() PURE;
    GLDMETHOD(void, prev)() PURE;
    GLDMETHOD(void, last)() PURE;
    GLDMETHOD(IGEGRecord *, current)() PURE;
    GLDMETHOD(bool, isDone)() PURE;
};

// GEGFieldIterator 接口
DEFINE_IID(IGEGFieldIterator, "{590DEA04-4469-46E6-92FD-58A0E5E62297}");
DECLARE_INTERFACE_(IGEGFieldIterator, IUnknown)
{
    GLDMETHOD(void, first)() PURE;
    GLDMETHOD(void, next)() PURE;
    GLDMETHOD(void, prev)() PURE;
    GLDMETHOD(void, last)() PURE;
    GLDMETHOD(IGEGField *, current)() PURE;
    GLDMETHOD(bool, isDone)() PURE;
};

// GEGRecord 接口
DEFINE_IID(IGEGRecord, "{ECCB0561-7E07-4DE1-B5E1-5B45F8777135}");
DECLARE_INTERFACE_(IGEGRecord, IUnknown)
{
    GLDMETHOD(GString, code)() PURE;
    GLDMETHOD(void, setCode)(const GString &value) PURE;
    GLDMETHOD(bool, isChecked)() PURE;
    GLDMETHOD(void, setIsChecked)(bool isChecked) PURE;
    GLDMETHOD(GEGValue, fullValues)(const GString &fieldName) PURE;
    GLDMETHOD(bool, addValues)(const GString &fieldName, const GEGValue &value) PURE;
    GLDMETHOD(int, excelRowNo)() PURE;
    GLDMETHOD(void, setExcelRowNo)(int rowNo) PURE;
};

// GEGField 接口
DEFINE_IID(IGEGField, "{550359AA-838C-4572-A8CC-12EB1E41F6A1}");
DECLARE_INTERFACE_(IGEGField, IUnknown)
{
    GLDMETHOD(GString, code)() PURE;
    GLDMETHOD(GEGDataType, dataType)() PURE;
    GLDMETHOD(void, setCode)(const GString &code) PURE;
    GLDMETHOD(void, setDataType)(GEGDataType type) PURE;
    GLDMETHOD(GString, name)() PURE;
    GLDMETHOD(void, setName)(const GString &name) PURE;
};

#endif // GLDEXCELGRIDINTF_H
