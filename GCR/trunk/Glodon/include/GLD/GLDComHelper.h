/*************************************************************************
*                                                                        *
*   广联达COM辅助函数单元 H                                              *
*                                                                        *
*   设计：Zhangsk 2012.05.23                                             *
*   备注：                                                               *
*   审核：                                                               *
*                                                                        *
*   Copyright (c) 2012-2013 GLD Corporation                           *
*                                                                        *
*************************************************************************/

#ifndef GLDCOMHELPER_H
#define GLDCOMHELPER_H

#include "GLDUnknwn.h"
#include "GLDComptr.h"
#include "GLDObject.h"
#include "GLDNameSpace.h"

G_GLODON_BEGIN_NAMESPACE
G_COMHELPER_BEGIN_NAMESPACE

template <class T>
inline bool Supports(IUnknown *pInstance, const IID &iid, T **pIntf)
{
    return (pInstance != NULL) && (GLD_OK == pInstance->QueryInterface(iid, (void **)pIntf));
}

inline bool Supports(IUnknown *pInstance, const IID &iid)
{
    GComPtr<IUnknown> pUnk;
    return Supports(pInstance, iid, &pUnk);
}

template <class T>
inline bool Supports(IUnknown *pInstance, T **pIntf)
{
    return Supports(pInstance, __uuidof(T), pIntf);
}

template <class T>
inline bool Supports(GObject *pObj, const IID &iid, T **pIntf)
{
    GComPtr<IUnknown> pUnk = NULL;
    if (pObj->QueryInterface(iid, (void **)&pUnk) >= 0)
    {
        return Supports(pUnk, iid, pIntf);
    }
    else
    {
        return false;
    }
}

inline bool Supports(GObject *pObj, const IID &iid)
{
    GComPtr<IUnknown> pOut = NULL;
    return Supports(pObj, iid, &pOut);
}

template <class T>
inline bool Supports(GObject *pObj, T **pIntf)
{
    return Supports(pObj, __uuidof(T), pIntf);
}




template <class T>
struct _SupportsHelper
{
    static bool supports(IUnknown *pInstance)
    {
        GComPtr<T> p;
        return (pInstance != NULL) && (GLD_OK == pInstance->QueryInterface( __uuidof(T), (void **)&p));
    }
};

#define SUPPORTS(pInstance, T) _SupportsHelper<T>::supports(pInstance)


G_COMHELPER_END_NAMESPACE
G_GLODON_END_NAMESPACE
#endif // GLDCOMHELPER_H
