#ifndef GUNKNWN_H
#define GUNKNWN_H


/**********************************************************************
 *
 *  广联达接口定义单元
 *
 *  设计：linw 2012.03.26
 *  备注：
 *  审核：
 *
 *  Copyright (c) 1994-2012 GlodonSoft Corporation
 *
 **********************************************************************/

//TODO: #define INITGUID
#if defined(WIN32) || defined(WIN64) 
#   include <unknwn.h>
#   include <guiddef.h>
#else
#   include "GLDObjBase.h"
#   include "GLDGuidDef.h"
#   include "GLDWinErrorDef.h"
#endif    //WIN32

#ifndef WIN32
inline unsigned long wcharToULong(wchar_t *p, unsigned char size)
{
    unsigned long result = 0;
    for (unsigned char i = 0; i != size; ++i)
    {
        wchar_t ch = *p;
        if (ch >= L'a')
            ch -= 32;
        if (ch >= L'A')
            ch -= 7;
        ch -= L'0';
        result = result << 4;
        result += ch;
        ++p;
    }
    return result;
}

inline HRESULT CLSIDFromString(LPCOLESTR lpsz, LPCLSID pclsid)
{
    wchar_t *p = (wchar_t *)lpsz + 1; // '{'
    pclsid->Data1 = wcharToULong(p, 8);
    p += 9; // '-'
    pclsid->Data2 = (unsigned short)wcharToULong(p, 4);
    p += 5; // '-'
    pclsid->Data3 = (unsigned short)wcharToULong(p, 4);
    p += 5; // '-'
    pclsid->Data4[0] = (unsigned char)wcharToULong(p, 2);
    p += 2;
    pclsid->Data4[1] = (unsigned char)wcharToULong(p, 2);
    p += 3; // '-'
    pclsid->Data4[2] = (unsigned char)wcharToULong(p, 2);
    p += 2;
    pclsid->Data4[3] = (unsigned char)wcharToULong(p, 2);
    p += 2;
    pclsid->Data4[4] = (unsigned char)wcharToULong(p, 2);
    p += 2;
    pclsid->Data4[5] = (unsigned char)wcharToULong(p, 2);
    p += 2;
    pclsid->Data4[6] = (unsigned char)wcharToULong(p, 2);
    p += 2;
    pclsid->Data4[7] = (unsigned char)wcharToULong(p, 2);
    return 0;
}
#endif    //WIN32

/****************************************************************************
 * __uuidof
 ****************************************************************************/
inline GUID GUIDFromString(LPOLESTR lpsz)
{
    GUID guid;
    CLSIDFromString(lpsz, &guid);
    return guid;
}

#if defined(WIN32) || defined(WIN64)
//Microsoft OS Platform
#   define _DEFINE_UUID(Class, uid) \
    struct __declspec(uuid(uid)) Class;
#else
    template <class Class>
    struct GuidTraits {};
#   define _DEFINE_UUID(Class, uuid)                        \
    template <>                                             \
    struct GuidTraits<Class> {                              \
        static const GUID& Guid() {                         \
        static const wchar_t *str = L ## uuid;              \
        static GUID guid = GUIDFromString((LPOLESTR)str);   \
        return guid;                                        \
        }                                                   \
    }
#   ifdef __MINGW32__
#       undef __uuidof
#   endif
#   define __uuidof(Class) GuidTraits<Class>::Guid()
#endif //OS Platform

#define DEFINE_CLSID(Class, guid)   \
class Class;                        \
_DEFINE_UUID(Class, guid)

#define DEFINE_IID(Interface, iid)  \
struct Interface;                   \
_DEFINE_UUID(Interface, iid)

//struct __declspec(uuid(iid)) Interface

#define GLD_OK S_OK
#define GLD_FALSE S_FALSE

#define GLDMETHODCALLTYPE STDMETHODCALLTYPE
#define GLDMETHOD(type, method) virtual type GLDMETHODCALLTYPE method

#define GSPMETHODCALLTYPE STDMETHODCALLTYPE
#define GSPMETHOD(type, method) virtual type GSPMETHODCALLTYPE method

#define GRPMETHODCALLTYPE STDMETHODCALLTYPE
#define GRPMETHOD(type, method) virtual type GRPMETHODCALLTYPE method

#ifdef GDP_QT
#   define GSPMETHODIMP void GSPMETHODCALLTYPE
#   define GSP_OK
#   define GSP_FALSE
#else
#   define GSPMETHODIMP STDMETHODIMP
#   define GSP_OK GLD_OK
#   define GSP_FALSE GLD_FALSE
#endif

DEFINE_IID(IUnknown, "{00000000-0000-0000-C000-000000000046}");
//DEFINE_IID(IClassFactory, "{00000001-0000-0000-C000-000000000046}");
//DEFINE_IID(ITypeInfo, "{00020401-0000-0000-C000-000000000046}");
//DEFINE_IID(IDispatch, "00020400-0000-0000-C000-000000000046");
//DEFINE_IID(ISequentialStream, "{0C733A30-2A1C-11CE-ADE5-00AA0044773D}");
//DEFINE_IID(IStream, "{0000000C-0000-0000-C000-000000000046}");

#ifndef WIN32

const IID IID_IUnknown =
        { 0x00000000, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
const IID IID_IClassFactory =
        { 0x00000001, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
const IID IID_ITypeInfo =
        { 0x00020401, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
const IID IID_IDispatch =
        { 0x00020400, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
const IID IID_ISequentialStream =
        { 0x0C733A30, 0x2A1C, 0x11CE, { 0xAD, 0xE5, 0x00, 0xAA, 0x00, 0x44, 0x77, 0x3D } };
const IID IID_IStream =
        { 0x0000000C, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };

#define __RPC__out
#define __RPC__in
#define __RPC__deref_out_opt
#define __RPC__in_ecount_full(cNames)
#define __RPC__in_range(a, b)
#define _In_reads_bytes_(a)
#define __RPC__out_ecount_full(cNames)
#define _Out_writes_bytes_to_(a, b)
#define _In_
#define _Out_opt_

//COM定义IUnknown, Factory接口

//#ifdef __cplusplus
//extern "C"{
//#endif


//+-------------------------------------------------------------------------
//
//  Modi By Microsoft Windows
//
//--------------------------------------------------------------------------

#ifndef __IUnknown_INTERFACE_DEFINED__
#define __IUnknown_INTERFACE_DEFINED__

/*EXTERN_C*/

DECLARE_INTERFACE(IUnknown)
{
public:
    BEGIN_INTERFACE

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
                /* [in] */ REFIID riid,
                /* [iid_is][out] */ void **ppvObject) = 0;

    virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;

    virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

    END_INTERFACE
};

typedef /* [unique] */ IUnknown *LPUNKNOWN;

#endif     /* __IUnknown_INTERFACE_DEFINED__ */



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifndef __IClassFactory_INTERFACE_DEFINED__
#define __IClassFactory_INTERFACE_DEFINED__

DECLARE_INTERFACE_(IClassFactory, IUnknown)
{
public:
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE CreateInstance(
                /* [unique][in] */ IUnknown *pUnkOuter,
                /* [in] */ REFIID riid,
                /* [iid_is][out] */ void **ppvObject) = 0;

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE LockServer(
                /* [in] */ BOOL fLock) = 0;
};

typedef /* [unique] */ IClassFactory  *LPCLASSFACTORY;

#endif     /* __IClassFactory_INTERFACE_DEFINED__ */



//#ifdef __cplusplus
//}
//#endif

DECLARE_INTERFACE_(ITypeInfo, IUnknown)
{
public:
//    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetTypeAttr(
//        /* [out] */ TYPEATTR **ppTypeAttr) = 0;

//    virtual HRESULT STDMETHODCALLTYPE GetTypeComp(
//        /* [out] */ __RPC__deref_out_opt ITypeComp **ppTComp) = 0;

//    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetFuncDesc(
//        /* [in] */ UINT index,
//        /* [out] */ FUNCDESC **ppFuncDesc) = 0;

//    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetVarDesc(
//        /* [in] */ UINT index,
//        /* [out] */ VARDESC **ppVarDesc) = 0;

//    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetNames(
//        /* [in] */ MEMBERID memid,
//        /* [length_is][size_is][out] */ BSTR *rgBstrNames,
//        /* [in] */ UINT cMaxNames,
//        /* [out] */ UINT *pcNames) = 0;

//    virtual HRESULT STDMETHODCALLTYPE GetRefTypeOfImplType(
//        /* [in] */ UINT index,
//        /* [out] */ __RPC__out HREFTYPE *pRefType) = 0;

//    virtual HRESULT STDMETHODCALLTYPE GetImplTypeFlags(
//        /* [in] */ UINT index,
//        /* [out] */ __RPC__out INT *pImplTypeFlags) = 0;

//    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetIDsOfNames(
//        /* [annotation][size_is][in] */
//        __RPC__in_ecount(cNames)  LPOLESTR *rgszNames,
//        /* [in] */ UINT cNames,
//        /* [size_is][out] */ MEMBERID *pMemId) = 0;

//    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
//        /* [in] */ PVOID pvInstance,
//        /* [in] */ MEMBERID memid,
//        /* [in] */ WORD wFlags,
//        /* [out][in] */ DISPPARAMS *pDispParams,
//        /* [out] */ VARIANT *pVarResult,
//        /* [out] */ EXCEPINFO *pExcepInfo,
//        /* [out] */ UINT *puArgErr) = 0;

//    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetDocumentation(
//        /* [in] */ MEMBERID memid,
//        /* [out] */ BSTR *pBstrName,
//        /* [out] */ BSTR *pBstrDocString,
//        /* [out] */ DWORD *pdwHelpContext,
//        /* [out] */ BSTR *pBstrHelpFile) = 0;

//    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetDllEntry(
//        /* [in] */ MEMBERID memid,
//        /* [in] */ INVOKEKIND invKind,
//        /* [out] */ BSTR *pBstrDllName,
//        /* [out] */ BSTR *pBstrName,
//        /* [out] */ WORD *pwOrdinal) = 0;

//    virtual HRESULT STDMETHODCALLTYPE GetRefTypeInfo(
//        /* [in] */ HREFTYPE hRefType,
//        /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo) = 0;

//    virtual /* [local] */ HRESULT STDMETHODCALLTYPE AddressOfMember(
//        /* [in] */ MEMBERID memid,
//        /* [in] */ INVOKEKIND invKind,
//        /* [out] */ PVOID *ppv) = 0;

//    virtual /* [local] */ HRESULT STDMETHODCALLTYPE CreateInstance(
//        /* [in] */ IUnknown *pUnkOuter,
//        /* [in] */ REFIID riid,
//        /* [iid_is][out] */ PVOID *ppvObj) = 0;

//    virtual HRESULT STDMETHODCALLTYPE GetMops(
//        /* [in] */ MEMBERID memid,
//        /* [out] */ __RPC__deref_out_opt BSTR *pBstrMops) = 0;

//    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetContainingTypeLib(
//        /* [out] */ ITypeLib **ppTLib,
//        /* [out] */ UINT *pIndex) = 0;

//    virtual /* [local] */ void STDMETHODCALLTYPE ReleaseTypeAttr(
//        /* [in] */ TYPEATTR *pTypeAttr) = 0;

//    virtual /* [local] */ void STDMETHODCALLTYPE ReleaseFuncDesc(
//        /* [in] */ FUNCDESC *pFuncDesc) = 0;

//    virtual /* [local] */ void STDMETHODCALLTYPE ReleaseVarDesc(
//        /* [in] */ VARDESC *pVarDesc) = 0;
};

typedef struct tagSTATSTG
{
    LPOLESTR pwcsName;
    DWORD type;
    ULARGE_INTEGER cbSize;
    FILETIME mtime;
    FILETIME ctime;
    FILETIME atime;
    DWORD grfMode;
    DWORD grfLocksSupported;
    CLSID clsid;
    DWORD grfStateBits;
    DWORD reserved;
} STATSTG;

DECLARE_INTERFACE_(IDispatch, IUnknown)
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
        /* [out] */ __RPC__out UINT *pctinfo) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
        /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(
        /* [in] */ __RPC__in REFIID riid,
        /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
        /* [range][in] */ __RPC__in_range(0,16384) UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) = 0;

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
        /* [annotation][in] */
        _In_  DISPID dispIdMember,
        /* [annotation][in] */
        _In_  REFIID riid,
        /* [annotation][in] */
        _In_  LCID lcid,
        /* [annotation][in] */
        _In_  WORD wFlags,
        /* [annotation][out][in] */
        _In_  DISPPARAMS *pDispParams,
        /* [annotation][out] */
        _Out_opt_  VARIANT *pVarResult,
        /* [annotation][out] */
        _Out_opt_  EXCEPINFO *pExcepInfo,
        /* [annotation][out] */
        _Out_opt_  UINT *puArgErr) = 0;

};

DECLARE_INTERFACE_(ISequentialStream, IUnknown)
{
public:
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
                /* [annotation] */
                _Out_writes_bytes_to_(cb, *pcbRead)  void *pv,
                /* [annotation][in] */
                _In_  ULONG cb,
                /* [annotation] */
                _Out_opt_  ULONG *pcbRead) = 0;

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write(
                /* [annotation] */
                _In_reads_bytes_(cb)  const void *pv,
                /* [annotation][in] */
                _In_  ULONG cb,
                /* [annotation] */
                _Out_opt_  ULONG *pcbWritten) = 0;
};

DECLARE_INTERFACE_(IStream, ISequentialStream)
{
public:
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek(
                /* [in] */ LARGE_INTEGER dlibMove,
                /* [in] */ DWORD dwOrigin,
                /* [annotation] */
                _Out_opt_  ULARGE_INTEGER *plibNewPosition) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetSize(
                /* [in] */ ULARGE_INTEGER libNewSize) = 0;

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE CopyTo(
                /* [annotation][unique][in] */
                _In_  IStream *pstm,
                /* [in] */ ULARGE_INTEGER cb,
                /* [annotation] */
                _Out_opt_  ULARGE_INTEGER *pcbRead,
                /* [annotation] */
                _Out_opt_  ULARGE_INTEGER *pcbWritten) = 0;

    virtual HRESULT STDMETHODCALLTYPE Commit(
                /* [in] */ DWORD grfCommitFlags) = 0;

    virtual HRESULT STDMETHODCALLTYPE Revert( void) = 0;

    virtual HRESULT STDMETHODCALLTYPE LockRegion(
                /* [in] */ ULARGE_INTEGER libOffset,
                /* [in] */ ULARGE_INTEGER cb,
                /* [in] */ DWORD dwLockType) = 0;

    virtual HRESULT STDMETHODCALLTYPE UnlockRegion(
                /* [in] */ ULARGE_INTEGER libOffset,
                /* [in] */ ULARGE_INTEGER cb,
                /* [in] */ DWORD dwLockType) = 0;

    virtual HRESULT STDMETHODCALLTYPE Stat(
                /* [out] */ __RPC__out STATSTG *pstatstg,
                /* [in] */ DWORD grfStatFlag) = 0;

    virtual HRESULT STDMETHODCALLTYPE Clone(
                /* [out] */ __RPC__deref_out_opt IStream **ppstm) = 0;
};

#endif    //WIN32

#endif  // GUNKNWN_H
