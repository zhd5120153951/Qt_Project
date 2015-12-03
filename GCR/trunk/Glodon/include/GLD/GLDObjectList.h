#ifndef GLDOBJECTLIST_H
#define GLDOBJECTLIST_H

#include "GLDVector.h"
#include "GLDGlobal.h"

#define GLD_OUTOFLINE_TEMPLATE

template <typename T>
class GObjectList
{
public:
    inline GObjectList(bool ownerObject = true) { m_ownerObject = ownerObject; }
    inline GObjectList(const GObjectList<T> &l) { clear(); m_ownerObject = l.m_ownerObject; append(l); }
    ~GObjectList();
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
    size_t size() const { return m_list.size(); }

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
    void append(const GObjectList<T> &t);
    void prepend(const T &t);
    void insert(int i, const T &t);
    void replace(int i, const T &t);
    void Delete(int i);
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
    int count(const T &t);

    inline int count() const { return int(m_list.size()); }
    inline int length() const { return int(m_list.size()); }
    inline GLDVector<T> &list() { return m_list; }
private:
    GLDVector<T> m_list;
    bool m_ownerObject;
};

/* GObjectList */

template <typename T>
GLD_OUTOFLINE_TEMPLATE GObjectList<T>::~GObjectList()
{
    clear();
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GObjectList<T>::push_back(const T &t)
{
    m_list.push_back(t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GObjectList<T>::append(const T &t)
{
    m_list.push_back(t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GObjectList<T>::prepend(const T &t)
{
    m_list.insert(0, t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GObjectList<T>::insert(int i, const T &t)
{
    if (i >= 0)
        m_list.insert(i, t);
    else
        m_list.push_back(t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GObjectList<T>::replace(int i, const T &t)
{
    if (m_ownerObject)
    {
        delete m_list[i];
    }
    m_list[i] = t;
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GObjectList<T>::Delete(int i)
{
    if (m_ownerObject)
        delete m_list[i];
    m_list.Delete(i);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE int GObjectList<T>::removeAll(const T &t)
{
    int n = m_list.removeAll(t);
    if (m_ownerObject && (n > 0))
    {
        delete t;
    }
    return n;
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE int GObjectList<T>::removeOne(const T &t)
{
    return m_list.remove(t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE T GObjectList<T>::takeAt(int i)
{
    return m_list.at(i);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE T GObjectList<T>::takeFirst()
{
    return m_list.at(0);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE T GObjectList<T>::takeLast()
{
    return m_list.at(size() - 1);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GObjectList<T>::move(int from, int to)
{
    m_list.move(from, to);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GObjectList<T>::swap(int i, int j)
{
    T tObj = m_list[i];
    m_list[i] = m_list[j];
    m_list[j] = tObj;
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE int GObjectList<T>::indexOf(const T &t, int from) const
{
    return m_list.indexOf(t); // todo from
    G_UNUSED(from)
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE int GObjectList<T>::lastIndexOf(const T &t, int from) const
{
    return m_list.lastIndexOf(t, from);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE bool GObjectList<T>::contains(const T &t) const
{
    return m_list.find(t) != m_list.end();
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE int GObjectList<T>::count(const T &t)
{
    return m_list.count(t);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GObjectList<T>::append(const GObjectList<T> &t)
{
    for (int i = 0; i != (int)t.m_list.size(); ++i)
    {
        append(t.m_list[i]);
    }
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE void GObjectList<T>::clear()
{
    if (m_ownerObject)
    {
        for (int i = 0; i != (int)m_list.size(); ++i)
        {
            T tObj = m_list[i];
            delete tObj;
        }
    }
    m_list.clear();
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE const T &GObjectList<T>::at(int i) const
{
    return m_list.at(i);
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE const T &GObjectList<T>::operator [](int i) const
{
    return m_list[i];
}

template <typename T>
GLD_OUTOFLINE_TEMPLATE T &GObjectList<T>::operator [](int i)
{
    return m_list[i];
}

#endif // GLDOBJECTLIST_H
