/*************************************************************************
*                                                                        *
*   广联达 对象定义单元 H                                   *
*                                                                        *
*   设计：Zhangsk 2012.05.23                                             *
*   备注：                                                               *
*   审核：                                                               *
*                                                                        *
*   Copyright (c) 2012-2013 Glodon Corporation                           *
*                                                                        *
*************************************************************************/

#ifndef GLDOBJECT_H
#define GLDOBJECT_H

#include "GLDUnknwn.h"
#include "GLDVector.h"
#include "GLDObjectList.h"
#include "GLDGlobal.h"

#define DECLARE_IUNKNOWN                                                        \
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) {   \
        return _QueryInterface(riid, ppvObject);                                \
    };                                                                            \
    ULONG STDMETHODCALLTYPE AddRef() {                                            \
        return _AddRef();                                                       \
    };                                                                            \
    ULONG STDMETHODCALLTYPE Release() {                                            \
        return _Release();                                                      \
    };

#define DECLARE_IDISPATCH                                                        \
    DECLARE_IUNKNOWN \
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo) { \
        return _GetTypeInfoCount(pctinfo);                                \
    } \
    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) { \
        return _GetTypeInfo(iTInfo, lcid, ppTInfo);                                \
    } \
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) { \
        return _GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);                                \
    } \
    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, \
                                     VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) { \
        return _Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr); \
    }

/*! 查询接口映射宏，使用在头文件和cpp文件中加略显麻烦，主要是针对编译器（怕有的编译器把_QueryInterface编译成内联函数） */
// 头文件中添加的宏
#define GLD_DECLARE_INTERFACE_MAP                                                       \
    protected:                                                                          \
        HRESULT STDMETHODCALLTYPE _QueryInterface(const IID & riid, void ** ppvObject)  
/*! 源文件中添加的宏 */
#define GLD_INTERFACE_MAP_BEGIN(_ClassName)                                                         \
        HRESULT STDMETHODCALLTYPE _ClassName::_QueryInterface(const IID & riid, void ** ppvObject)  \
        {                                                                                           \
            HRESULT hr = NOERROR;                                                                   \
            do                                                                                      \
            {                                                                           
#define GLD_INTERFACE_MAP_ITFC(_Inteface)                                                           \
                if (riid == __uuidof(_Inteface))                                                    \
                {                                                                                   \
                    *ppvObject = static_cast<_Inteface *>(this);                                    \
                    break;                                                                          \
                }
#define GLD_INTERFACE_MAP_CLASS(_ClassName)                                                         \
                hr = _ClassName::_QueryInterface(riid, ppvObject);                                  \
                if (nullptr != *ppvObject)                                                          \
                {                                                                                   \
                    return hr;                                                                      \
                }
#define GLD_INTERFACE_MAP_END                                                                       \
                *ppvObject = nullptr;                                                               \
                hr = E_NOINTERFACE;                                                                 \
            } while (false);                                                                        \
            if (nullptr != *ppvObject)                                                              \
            {                                                                                       \
                this->AddRef();                                                                     \
            }                                                                                       \
            return hr;                                                                              \
        }

class GLDCOMMONSHARED_EXPORT GInterfaceObject : public IUnknown
{
public:
    GInterfaceObject();
    virtual ~GInterfaceObject();

public:
    DECLARE_IUNKNOWN

public:
    inline void doAddRef() { m_cRef.ref(); }
    inline void doRelease() { m_cRef.deref(); }
    inline LONG refCount() const { return m_cRef.load(); }

protected:
    virtual HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG STDMETHODCALLTYPE _AddRef(void);
    virtual ULONG STDMETHODCALLTYPE _Release(void);

    void beforeDestruction();
    void afterConstruction();

private:
    QBasicAtomicInteger<LONG> m_cRef;
};

class GLDCOMMONSHARED_EXPORT GAutoIntfObject : public GInterfaceObject, public IDispatch
{
public:
    DECLARE_IDISPATCH
#if defined(GLD_DLL)
public:
    GAutoIntfObject() {}
    ~GAutoIntfObject() {}
#endif

protected:
    HRESULT STDMETHODCALLTYPE _GetTypeInfoCount(UINT *pctinfo);
    HRESULT STDMETHODCALLTYPE _GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    HRESULT STDMETHODCALLTYPE _GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
    HRESULT STDMETHODCALLTYPE _Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
                                     VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
protected:
    virtual HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject);
};

typedef GInterfaceObject GObject;

class GLDCOMMONSHARED_EXPORT GNoRefInterfaceObject : public IUnknown
{
public:
    GNoRefInterfaceObject();
    virtual ~GNoRefInterfaceObject();
public:
    DECLARE_IUNKNOWN

protected:
    virtual HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG STDMETHODCALLTYPE _AddRef(void);
    virtual ULONG STDMETHODCALLTYPE _Release(void);
};

template <typename T>
class GInterfaceObjectList
{
public:
    typedef typename GLDVector<T>::const_iterator const_iterator;
    typedef typename GLDVector<T>::iterator iterator;
public:
    inline GInterfaceObjectList(bool ownerObject = true) { m_ownerObject = ownerObject; }
    inline GInterfaceObjectList(const GInterfaceObjectList<T> &l) { clear(); m_ownerObject = l.m_ownerObject; append(l); }
    virtual ~GInterfaceObjectList();
    GInterfaceObjectList<T> &operator=(const GInterfaceObjectList<T> &l)
    {
        clear();
        m_ownerObject = l.m_ownerObject;
        append(l);
        return *this;
    }

    /*
    GInterfaceListObject<T> &operator=(const GInterfaceListObject<T> &l);
#ifdef Q_COMPILER_RVALUE_REFS
    inline GInterfaceListObject &operator=(GInterfaceListObject &&other)
    { qSwap(d, other.d); return *this; }
#endif
    inline void swap(GInterfaceListObject<T> &other) { qSwap(d, other.d); }
#ifdef Q_COMPILER_INITIALIZER_LISTS
    inline GInterfaceListObject(std::initializer_list<T> args) : d(&GInterfaceListObjectData::shared_null)
    { d->ref.ref(); qCopy(args.begin(), args.end(), std::back_inserter(*this)); }
#endif
    bool operator==(const GInterfaceListObject<T> &l) const;
    inline bool operator!=(const GInterfaceListObject<T> &l) const { return !(*this == l); }
*/
    inline size_t size() const { return m_list.size(); }
/*
    inline void detach() { if (d->ref != 1) detach_helper(); }

    inline void detachShared()
    {
        // The "this->" qualification is needed for GCCE.
        if (d->ref != 1 && this->d != &GInterfaceListObjectData::shared_null)
            detach_helper();
    }

    inline bool isDetached() const { return d->ref == 1; }
    inline void setSharable(bool sharable) { if (!sharable) detach(); d->sharable = sharable; }
    inline bool isSharedWith(const GInterfaceListObject<T> &other) const { return d == other.d; }
*/

    inline bool isEmpty() const { return m_list.empty(); }

    void clear();

    const T &at(int i) const;
    const T &operator[](int i) const;
    T &operator[](int i);
/*
    void reserve(int size);
*/
    void push_back(const T &t);
    void append(const T &t);
    void append(const GInterfaceObjectList<T> &t);
    void prepend(const T &t);
    void insert(int i, const T &t);
    void replace(int i, const T &t);
    void Delete(int index);
    inline void removeAt(int i)
    {
        Delete(i);
    }
    int removeAll(const T &t);
    int removeOne(const T &t);
    inline int remove(const T &t)
    {
        return removeOne(t);
    }
    T takeAt(int i);
    T takeFirst();
    T takeLast();
    void move(int from, int to);
    void swap(int i, int j);
    int indexOf(const T &t, int from = 0) const;
    int lastIndexOf(const T &t, int from = -1) const;
    bool contains(const T &t) const;
    int count(const T &t) const;

    inline int count() const { return int(m_list.size()); }
    inline int length() const { return int(m_list.size()); }
//    GLDVector<T>::const_iterator begin() const { return m_list.begin(); }
//    GLDVector<T>::const_iterator end() const { return m_list.end(); }
    inline GLDVector<T> &list() { return m_list; }
private:
    GLDVector<T> m_list;
    bool m_ownerObject;
};

/* GInterfaceObjectList */

template <typename T>
GLD_OUTOFLINE_TEMPLATE GInterfaceObjectList<T>::~GInterfaceObjectList()
{
    clear();
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GInterfaceObjectList<T>::push_back(const T &t)
{
    if (t != NULL)
    {
        if (m_ownerObject)
        {
            t->AddRef();
        }
    }
    m_list.push_back(t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GInterfaceObjectList<T>::append(const T &t)
{
    push_back(t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GInterfaceObjectList<T>::prepend(const T &t)
{
    if (m_ownerObject)
        t->AddRef();
    m_list.insert(m_list.cbegin(), t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GInterfaceObjectList<T>::insert(int i, const T &t)
{
    if (m_ownerObject)
    {
        t->AddRef();
    }
    m_list.insert(i, t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GInterfaceObjectList<T>::replace(int i, const T &t)
{
    if (m_ownerObject)
    {
        t->AddRef();
        m_list[i]->Release();
    }
    m_list[i] = t;
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GInterfaceObjectList<T>::Delete(int i)
{
    if (m_ownerObject)
    {
        m_list[i]->Release();
    }
    m_list.Delete(i);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE int GInterfaceObjectList<T>::removeAll(const T &t)
{
    int n = m_list.removeAll(t);
    if (m_ownerObject)
    {
        for (int i = 0; i < n; ++i)
            t->Release();
    }
    return n;
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE int GInterfaceObjectList<T>::removeOne(const T &t)
{
#ifdef _MSC_VER
    for (const_iterator it = m_list.begin(); it != m_list.end(); ++it)
#else
    for (iterator it = m_list.begin(); it != m_list.end(); ++it)
#endif
    {
        if (*it == t)
        {
            int result = it - m_list.begin();
            m_list.erase(it);
            if (m_ownerObject)
                t->Release();
            return result;
        }
    }
    return -1;
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE T GInterfaceObjectList<T>::takeAt(int i)
{
    return m_list.at(i);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE T GInterfaceObjectList<T>::takeFirst()
{
    return m_list.at(0);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE T GInterfaceObjectList<T>::takeLast()
{
    return m_list.at(size() - 1);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GInterfaceObjectList<T>::move(int from, int to)
{
    m_list.move(from, to);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GInterfaceObjectList<T>::swap(int i, int j)
{
    const T c_value = m_list[i];
    m_list[i] = m_list[j];
    m_list[j] = c_value;
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE int GInterfaceObjectList<T>::indexOf(const T &t, int from) const
{
    return m_list.indexOf(t, from);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE int GInterfaceObjectList<T>::lastIndexOf(const T &t, int from) const
{
    return m_list.lastIndexOf(t, from);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE bool GInterfaceObjectList<T>::contains(const T &t) const
{
    return m_list.contains(t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE int GInterfaceObjectList<T>::count(const T &t) const
{
    return m_list.count(t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GInterfaceObjectList<T>::append(const GInterfaceObjectList<T> &t)
{
    for (int i = 0; i != (int)t.m_list.size(); ++i)
    {
        append(t.m_list[i]);
    }
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GInterfaceObjectList<T>::clear()
{
    if (m_ownerObject)
    {
        for (int i = 0; i != (int)m_list.size(); ++i)
        {
            if (m_list[i] != NULL)
            {
                m_list[i]->Release();
            }
        }
    }
    m_list.clear();
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE const T &GInterfaceObjectList<T>::at(int i) const
{
    return m_list.at(i);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE const T &GInterfaceObjectList<T>::operator [](int i) const
{
    return m_list[i];
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE T &GInterfaceObjectList<T>::operator [](int i)
{
    return m_list[i];
}

#endif  // GLDOBJECT_H
