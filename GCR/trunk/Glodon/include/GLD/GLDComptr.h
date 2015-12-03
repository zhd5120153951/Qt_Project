#ifndef GLDCOMPTR_H
#define GLDCOMPTR_H

#include <stddef.h>
//#include "sal.h"
#include "GLDUnknwn.h"

#ifndef NULL
#   define NULL 0
#endif

template <class T>

class GComPtr
{
public:
    inline GComPtr()
    {
        p = NULL;
    }

    inline GComPtr(T *lp)
    {
        p = lp;
        if (p != NULL)
            p->AddRef();
    }

    inline GComPtr(const GComPtr<T> &lp)
    {
        p = lp.p;
        if (p != NULL)
            p->AddRef();
    }

    //inline GComPtr(GComPtr<T>&& lp)
    //{
    //    p = lp.p;
    //    lp.p = NULL;
    //}

    inline GComPtr(T* lp, bool /*dummy*/)
    {
        p = lp;
    }

    template <typename Q>
    inline GComPtr(const GComPtr<Q> &lp)
    {
        if (NULL == lp.p)
        {
            p = NULL;
        }
        else
        {
            lp.p->QueryInterface(__uuidof(T), (void**)&p);
            //if (p != NULL) p->AddRef(); // QueryInterface已经增加引用计数了
        }
    }

    template <typename Q>
    inline GComPtr(const GComPtr<Q> &lp, bool /*dummy*/)
    {
        if (NULL == lp.p)
        {
            p = NULL;
        }
        else
        {
            lp.p->QueryInterface(__uuidof(T), (void**)&p);
            if (p != NULL) p->Release(); // QueryInterface已经增加引用计数了
        }
    }

    inline ~GComPtr()
    {
        if (p)
            p->Release();
    }

    inline operator T*() const
    {
        return p;
    }

    inline T& operator*() const
    {
        //ATLENSURE(p != NULL);
        return *p;
    }

    //The assert on operator& usually indicates a bug.  If this is really
    //what is needed, however, take the address of the p member explicitly.
    inline T** operator&()
    {
        //assert(p == NULL);
        return &p;
    }

    bool operator!() const
    {
        return (p == NULL);
    }

    bool operator<(T* pT) const
    {
        return p < pT;
    }

    bool operator!=(T* pT) const
    {
        return !operator==(pT);
    }

    bool operator==(T* pT) const
    {
        return p == pT;
    }

    T* operator=(T* lp)
    {
        if (*this!=lp)
        {
            if (lp != NULL)
                lp->AddRef();
            if (p)
                p->Release();
            p = lp;
            return lp;
        }
        return *this;
    }

    T* operator=(const GComPtr<T>& lp)
    {
        if (*this != lp)
        {
            if (p)
                p->Release();
            if (lp != NULL && !lp.isNull())
            {
                lp.p->AddRef();
                p = lp.p;
            }
            else
            {
                p = NULL;
            }
            return p;
        }
        return *this;
    }

    /*T* operator=(GComPtr<T>&& lp)
    {
        if (*this != lp)
        {
            if (p != NULL)
                p->Release();

            p = lp.p;
            lp.p = NULL;
        }
        return *this;
    }*/

    template <typename Q>
    T* operator=(const GComPtr<Q>& lp)
    {
        /*if ( !IsEqualObject(lp) )
        {
            return static_cast<T*>(AtlComQIPtrAssign((IUnknown**)&p, lp, __uuidof(T)));
        }*/
        if (NULL == lp.p)
        {
            p = NULL;
        }
        else
        {
            if (p != NULL)
                p->Release();
            lp.p->QueryInterface(__uuidof(T), (void**)&p);
            if (p != NULL)
                p->AddRef();
        }
        return *this;
    }

    inline bool isNull() const { return !p; }

    void AddRef()
    {
        if (!p)
            p->AddRef();
    }

    // Release the interface and set to NULL
    void Release()
    {
        T* pTemp = p;
        if (pTemp)
        {
            p = NULL;
            pTemp->Release();
        }
    }

    // Compare two objects for equivalence
    /*bool IsEqualObject(IUnknown* pOther)
    {
        if (p == NULL && pOther == NULL)
            return true;    // They are both NULL objects

        if (p == NULL || pOther == NULL)
            return false;    // One is NULL the other is not

        GComPtr<IUnknown> punk1;
        GComPtr<IUnknown> punk2;
        p->QueryInterface(__uuidof(IUnknown), (void**)&punk1);
        pOther->QueryInterface(__uuidof(IUnknown), (void**)&punk2);
        return punk1 == punk2;
    }*/

    // Attach to an existing interface (does not AddRef)
    void Attach(T* p2)
    {
        if (p)
            p->Release();
        p = p2;
    }

    // Detach the interface (does not Release)
    T* Detach()
    {
        T* pt = p;
        p = NULL;
        return pt;
    }

    /*HRESULT CopyTo(T** ppT)
    {
        //assert(ppT != NULL);
        if (ppT == NULL)
            return E_POINTER;
        *ppT = p;
        if (p)
            p->AddRef();
        return S_OK;
    }*/

    template <class Q>
    HRESULT QueryInterface(Q** pp) const
    {
        //assert(pp != NULL);
        return p->QueryInterface(__uuidof(Q), (void**)pp);
    }

    T* p;
};

#endif // GLDCOMPTR_H
