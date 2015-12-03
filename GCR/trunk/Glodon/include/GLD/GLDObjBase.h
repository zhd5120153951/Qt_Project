#ifndef GLDOBJBASE_H
#define GLDOBJBASE_H

//¶¨Òåºê

#define interface                struct

#ifdef WIN32
#    define STDMETHODCALLTYPE        __stdcall
#    define WINAPI                    __stdcall
#    define STDAPICALLTYPE            __stdcall
#    define APIENTRY                WINAPI
#else
#    define STDMETHODCALLTYPE
#    define WINAPI
#    define STDAPICALLTYPE
#    define APIENTRY                WINAPI
#endif

#define STDAPI                  EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type)           EXTERN_C type STDAPICALLTYPE
#define STDMETHOD(method)       virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type,method) virtual type STDMETHODCALLTYPE method
#define STDMETHODIMP            HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type)     type STDMETHODCALLTYPE
#define PURE                    = 0
#define DECLSPEC_NOVTABLE
#define DECLARE_INTERFACE(iface)    interface DECLSPEC_NOVTABLE iface
#define DECLARE_INTERFACE_(iface, baseiface)    interface DECLSPEC_NOVTABLE iface : public baseiface

#define DECLSPEC_IMPORT __declspec(dllimport)
#define WINBASEAPI DECLSPEC_IMPORT


#define BEGIN_INTERFACE
#define END_INTERFACE



#ifdef __cplusplus
#define EXTERN_C    extern "C"
#else
#define EXTERN_C    extern
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

typedef char                CHAR;
typedef char                CCHAR;
typedef unsigned char       BYTE;

typedef short               SHORT;
typedef unsigned short      WORD;
typedef unsigned short      USHORT;
typedef short               VARIANT_BOOL;

typedef long                LONG;
typedef long                HRESULT;
typedef unsigned long       DWORD;
typedef DWORD               *PDWORD;
typedef DWORD               ULONG;
typedef LONG                SCODE;

typedef int                 BOOL;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef long long           LONGLONG;
typedef unsigned long long  ULONGLONG;

typedef float               FLOAT;
typedef FLOAT               *PFLOAT;

typedef double              DOUBLE;
typedef double              DATE;

typedef void                *HANDLE;
typedef HANDLE                HINSTANCE;
typedef void                *PVOID;
typedef void                *LPVOID;

typedef wchar_t             *LPOLESTR;
typedef const wchar_t       *LPCOLESTR;
typedef const wchar_t       *LPCTSTR;
typedef wchar_t             OLECHAR;
typedef OLECHAR             *BSTR;

typedef DWORD               LCID;
typedef PDWORD              PLCID;
typedef WORD                LANGID;

typedef LONG                DISPID;

typedef union tagCY
{
    struct
    {
        unsigned long Lo;
        long Hi;
    } DUMMYSTRUCTNAME;
    LONGLONG int64;
} CY;

typedef struct tagSAFEARRAYBOUND
{
    ULONG cElements;
    LONG lLbound;
}     SAFEARRAYBOUND;

typedef struct tagSAFEARRAY
{
    USHORT cDims;
    USHORT fFeatures;
    ULONG cbElements;
    ULONG cLocks;
    PVOID pvData;
    SAFEARRAYBOUND rgsabound[ 1 ];
}     SAFEARRAY;

typedef struct tagDEC
{
    USHORT wReserved;
    union
    {
        struct
        {
            BYTE scale;
            BYTE sign;
        } DUMMYSTRUCTNAME;
        USHORT signscale;
    } DUMMYUNIONNAME;
    ULONG Hi32;
    union
    {
        struct
        {
            ULONG Lo32;
            ULONG Mid32;
        } DUMMYSTRUCTNAME2;
        ULONGLONG Lo64;
    } DUMMYUNIONNAME2;
} DECIMAL;

#define _VARIANT_BOOL    /##/
typedef unsigned short VARTYPE;
typedef /* [wire_marshal] */ struct tagVARIANT VARIANT;
struct tagVARIANT
{
    union
    {
        struct __tagVARIANT
        {
            VARTYPE vt;
            WORD wReserved1;
            WORD wReserved2;
            WORD wReserved3;
            union
            {
                LONGLONG llVal;
                LONG lVal;
                BYTE bVal;
                SHORT iVal;
                FLOAT fltVal;
                DOUBLE dblVal;
                VARIANT_BOOL boolVal;
                //_VARIANT_BOOL bool;
                SCODE scode;
                CY cyVal;
                DATE date;
                BSTR bstrVal;
                //IUnknown *punkVal;
                //IDispatch *pdispVal;
                SAFEARRAY *parray;
                BYTE *pbVal;
                SHORT *piVal;
                LONG *plVal;
                LONGLONG *pllVal;
                FLOAT *pfltVal;
                DOUBLE *pdblVal;
                VARIANT_BOOL *pboolVal;
                //_VARIANT_BOOL *pbool;
                SCODE *pscode;
                CY *pcyVal;
                DATE *pdate;
                BSTR *pbstrVal;
                //IUnknown **ppunkVal;
                //IDispatch **ppdispVal;
                SAFEARRAY **pparray;
                VARIANT *pvarVal;
                PVOID byref;
                CHAR cVal;
                USHORT uiVal;
                ULONG ulVal;
                ULONGLONG ullVal;
                INT intVal;
                UINT uintVal;
                DECIMAL *pdecVal;
                CHAR *pcVal;
                USHORT *puiVal;
                ULONG *pulVal;
                ULONGLONG *pullVal;
                INT *pintVal;
                UINT *puintVal;
                //struct __tagBRECORD
                //{
                //    PVOID pvRecord;
                //    IRecordInfo *pRecInfo;
                //}     __VARIANT_NAME_4;
            }     __VARIANT_NAME_3;
        }     __VARIANT_NAME_2;
        DECIMAL decVal;
    }     __VARIANT_NAME_1;
} ;
typedef VARIANT *LPVARIANT;

typedef VARIANT VARIANTARG;

typedef VARIANT *LPVARIANTARG;

typedef VARIANT             VARIANTARG;

typedef union _LARGE_INTEGER
{
    struct
    {
        DWORD LowPart;
        LONG HighPart;
    } DUMMYSTRUCTNAME;
    struct
    {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;

typedef LARGE_INTEGER *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
    struct
    {
        DWORD LowPart;
        DWORD HighPart;
    } DUMMYSTRUCTNAME;
    struct
    {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

typedef ULARGE_INTEGER *PULARGE_INTEGER;

typedef struct _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct tagDISPPARAMS
{
    /* [size_is] */ VARIANTARG *rgvarg;
    /* [size_is] */ DISPID *rgdispidNamedArgs;
    UINT cArgs;
    UINT cNamedArgs;
} DISPPARAMS;

typedef struct tagEXCEPINFO {
    WORD  wCode;
    WORD  wReserved;
    BSTR  bstrSource;
    BSTR  bstrDescription;
    BSTR  bstrHelpFile;
    DWORD dwHelpContext;
    PVOID pvReserved;
    void *pfnDeferredFillIn;//HRESULT (__stdcall *pfnDeferredFillIn)(struct tagEXCEPINFO *); // todo
    SCODE scode;
} EXCEPINFO, * LPEXCEPINFO;

typedef
enum tagSTREAM_SEEK
{
    STREAM_SEEK_SET    = 0,
    STREAM_SEEK_CUR    = 1,
    STREAM_SEEK_END    = 2
}     STREAM_SEEK;

typedef
enum tagSTGTY
{
    STGTY_STORAGE    = 1,
    STGTY_STREAM    = 2,
    STGTY_LOCKBYTES    = 3,
    STGTY_PROPERTY    = 4
}     STGTY;

typedef
enum tagLOCKTYPE
{
    LOCK_WRITE    = 1,
    LOCK_EXCLUSIVE    = 2,
    LOCK_ONLYONCE    = 4
}     LOCKTYPE;


/*
WINBASEAPI
LONG
WINAPI
InterlockedIncrement (
                      __inout LONG volatile *lpAddend
                      );

WINBASEAPI
LONG
WINAPI
InterlockedDecrement (
                      __inout LONG volatile *lpAddend
                      );
*/

#ifndef DECLSPEC_UUID
#if (_MSC_VER >= 1100) && defined (__cplusplus)
#define DECLSPEC_UUID(x)    __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif
#endif

#ifndef DECLSPEC_NOVTABLE
#if (_MSC_VER >= 1100) && defined(__cplusplus)
#define DECLSPEC_NOVTABLE   __declspec(novtable)
#else
#define DECLSPEC_NOVTABLE
#endif
#endif

#ifndef DECLSPEC_SELECTANY
#if (_MSC_VER >= 1100)
#define DECLSPEC_SELECTANY  __declspec(selectany)
#else
#define DECLSPEC_SELECTANY
#endif
#endif

/****************************************************************************
 * VC COM support
 ****************************************************************************/

#ifndef DECLSPEC_SELECTANY
#if (_MSC_VER >= 1100)
#define DECLSPEC_SELECTANY __declspec(selectany)
#else
#define DECLSPEC_SELECTANY
#endif
#endif

#ifndef DECLSPEC_NOVTABLE
#if (_MSC_VER >= 1100) && defined(__cplusplus)
#define DECLSPEC_NOVTABLE __declspec(novtable)
#else
#define DECLSPEC_NOVTABLE
#endif
#endif

#ifndef DECLSPEC_UUID
#if (_MSC_VER >= 1100) && defined(__cplusplus)
#define DECLSPEC_UUID(x) __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif
#endif

#define MIDL_INTERFACE(x)   struct DECLSPEC_UUID(x) DECLSPEC_NOVTABLE

#if _MSC_VER >= 1100
#define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8)  \
  EXTERN_C const IID DECLSPEC_SELECTANY itf = {l1,s1,s2,{c1,c2,c3,c4,c5,c6,c7,c8}}
#else
#define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8) EXTERN_C const IID itf
#endif

#endif  // GLDOBJBASE_H
