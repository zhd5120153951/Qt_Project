#include "GLDObject.h"

#include "GLDGlobal.h"

/* GInterfaceObject */

GInterfaceObject::GInterfaceObject()
{
    m_cRef.store(0);
}

GInterfaceObject::~GInterfaceObject()
{
}

HRESULT GInterfaceObject::_QueryInterface(const IID &riid, void **ppvObject)
{
    if (riid == IID_IUnknown)
    {
        this->AddRef();
        *ppvObject = static_cast<IUnknown *>(this);
        return NOERROR;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG GInterfaceObject::_AddRef()
{
    ULONG result = m_cRef.ref();
    return result;
}

ULONG GInterfaceObject::_Release()
{
    ULONG result = m_cRef.deref();
    if (result == 0)
    {
        delete this;
        return ULONG(0);
    }
    else
    {
        return result;
    }
}

void GInterfaceObject::beforeDestruction()
{
    m_cRef.ref();
}

void GInterfaceObject::afterConstruction()
{
    m_cRef.deref();
}

/* GNoRefInterfaceObject */
GNoRefInterfaceObject::GNoRefInterfaceObject()
{
}

GNoRefInterfaceObject::~GNoRefInterfaceObject()
{
}

HRESULT GNoRefInterfaceObject::_QueryInterface(const IID &riid, void **ppvObject)
{
    if (riid == IID_IUnknown)
    {
        this->AddRef();
        *ppvObject = static_cast<IUnknown *>(this);
        return NOERROR;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG GNoRefInterfaceObject::_AddRef()
{
    return -1;
}

ULONG GNoRefInterfaceObject::_Release()
{
    return -1;
}

/* GAutoIntfObject */
HRESULT GAutoIntfObject::_GetTypeInfoCount(UINT *pctinfo)
{
    return NOERROR;
    G_UNUSED(pctinfo);
}

HRESULT GAutoIntfObject::_GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
    return NOERROR;
    G_UNUSED(iTInfo);
    G_UNUSED(lcid);
    G_UNUSED(ppTInfo);
}

HRESULT GAutoIntfObject::_GetIDsOfNames(const IID &riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
    return NOERROR;
    G_UNUSED(riid);
    G_UNUSED(rgszNames);
    G_UNUSED(cNames);
    G_UNUSED(lcid);
    G_UNUSED(rgDispId);
}

HRESULT GAutoIntfObject::_Invoke(DISPID dispIdMember, const IID &riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
                                 VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    return NOERROR;
    G_UNUSED(dispIdMember);
    G_UNUSED(riid);
    G_UNUSED(lcid);
    G_UNUSED(wFlags);
    G_UNUSED(pDispParams);
    G_UNUSED(pVarResult);
    G_UNUSED(pExcepInfo);
    G_UNUSED(puArgErr);
}

HRESULT STDMETHODCALLTYPE GAutoIntfObject::_QueryInterface(REFIID riid, void **ppvObject)
{
    if (riid == IID_IDispatch)
    {
        this->AddRef();
        *ppvObject = static_cast<IDispatch *>(this);
        return GLD_OK;
    }
    else
        return GInterfaceObject::_QueryInterface(riid, ppvObject);
}
