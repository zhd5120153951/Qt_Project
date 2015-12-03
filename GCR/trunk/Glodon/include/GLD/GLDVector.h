#ifndef GLDVECTOR_H
#define GLDVECTOR_H

#include <vector>

using namespace std;

template<class T>
class GLDVector : public vector<T>
{
public:
    typedef typename vector<T>::size_type size_type;
    typedef typename vector<T>::const_iterator const_iterator;
    typedef typename vector<T>::iterator iterator;
public:
    GLDVector() : vector<T>()
    {
    }

    GLDVector(size_type count) : vector<T>(count)
    {
    }

    inline int count() const { return (int)vector<T>::size(); }

    int count(const T &value)
    {
        iterator cit = std::remove(begin(), end(), value);
        int nCount =  std::distance(cit, vector<T>::end());
        return nCount;
    }

    const_iterator find(const T &value, int from = 0) const
    {
        const_iterator it = vector<T>::begin() + from;
        for (; it != vector<T>::end(); ++it)
        {
            if (*it == value)
                break;
        }
        return it;
    }

    int indexOf(const T &value, int from = 0) const
    {
        const_iterator itor = find(value, from);
        if (itor != vector<T>::end())
            return int(itor - vector<T>::begin());
        else
            return -1;
    }

    int lastIndexOf(const T &value, int from = -1) const
    {
        if (from < 0)
        {
            from += size();
        }
        else if (from >= int(size()))
        {
            from = size() - 1;
        }
        else
        {
            const_iterator b = vector<T>::begin();
            const_iterator n = vector<T>::begin() + from + 1;
            while (n != b) {
                if (*--n == value)
                    return n - b;
            }
        }
        return -1;
    }

    void insert(int index, const T &t)
    {
        vector<T>::insert(vector<T>::begin() + index, t);
    }

    void Delete(int index)
    {
        vector<T>::erase(vector<T>::begin() + index);
    }

    int remove(const T &value)
    {
#ifdef _MSC_VER
        for (const_iterator it = vector<T>::begin(); it != vector<T>::end(); ++it)
#else
        for (iterator it = vector<T>::begin(); it != vector<T>::end(); ++it)
#endif
        {
            if (*it == value)
            {
                int result = int(it - vector<T>::begin());
                vector<T>::erase(it);
                return result;
            }
        }
        return -1;
    }

    int removeAll(const T &value)
    {
        const iterator cit = std::remove(vector<T>::begin(), vector<T>::end(), value);
        int nCount =  std::distance(cit, vector<T>::end());
        vector<T>::erase(cit, vector<T>::end());
        return nCount;
    }

    void move(int from, int to)
    {
        if (from == to)
            return;
        iterator itorFrom = vector<T>::begin() + from;
        iterator itorTo = vector<T>::begin() + to;
        const T c_value = *itorFrom;
        if (from > to)
        {
            for (; itorFrom != itorTo; itorFrom--)
            {
                *itorFrom = *(itorFrom - 1);
            }
        }
        else if (from < to)
        {
            for (; itorFrom != itorTo; itorFrom++)
            {
                *itorFrom = *(itorFrom + 1);
            }
        }
        *itorTo = c_value;
    }

    inline GLDVector<T> &operator<< (const T &t)
    {
        push_back(t);
        return *this;
    }

#ifdef CPP11PRE
    inline const_iterator cbegin() const { return begin(); }
    inline const_iterator cend() const { return end(); }
    inline const_reverse_iterator crbegin() const { return rbegin(); }
    inline const_reverse_iterator crend() const { return rend(); }
#endif
};

#endif // GLDVECTOR_H
