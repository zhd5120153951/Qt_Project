#ifndef GLDSORTUTILS_H
#define GLDSORTUTILS_H

#include "GLDVariant.h"
#include "GLDMathUtils.h"
#include "GLDInterfaceObject.h"

enum GSeekOption
{
    soFirst,                          // 返回第一条匹配的记录下标
    soLast,                           // 返回最后一条匹配的记录下标
    soMatch                           // 返回二分查找时首次匹配的记录下标
};

class GLDCOMMONSHARED_EXPORT CSortCompareEvent
{
public:
    virtual ~CSortCompareEvent() {}

    virtual CompareResult compare(void *item1, void *item2) = 0;
};

class GLDCOMMONSHARED_EXPORT ISearchCompareEvent
{
public:
    virtual CompareResult compare(void *item1, const GVariant &keyvalue) = 0;
};

inline CompareResult doSortCompare(void *item1, void *item2, CSortCompareEvent *compare, bool ascend)
{
    CompareResult result = compare->compare(item1, item2);
    if (!ascend)
    {
        if (result == crEqual)
        {
            return result;
        }
        if (result == crGreaterThan)
        {
            result = crLessThan;
        }
        else
        {
            result = crGreaterThan;
        }
    }
    return result;
}

template <typename T>
void merge(GLDVector<T> &source, GLDVector<T> &dest, int low, int mid, int high, CSortCompareEvent *compare, bool ascend)
{
    int nFirst = low;              // 第一段的游标
    int nSecnd = mid + 1;          // 第二段的游标
    int nResult = low;              // 结果的游标
    // 只要在段中存在I和J，则不断进行归并
    while ((nFirst <= mid) && (nSecnd <= high))
    {
        if (doSortCompare(source.at(nFirst), source.at(nSecnd), compare, ascend) != crGreaterThan)
        {
            dest[nResult] = source[nFirst];
            nResult++;
            nFirst++;
        }
        else
        {
            dest[nResult] = source[nSecnd];
            nResult++;
            nSecnd++;
        }
    }
    // 考虑余下的部分
    if (nFirst > mid)
    {
        for (int i = nSecnd; i <= high; ++i)
        {
            dest[nResult] = source[i];
            nResult++;
        }
    }
    else
    {
        for (int i = nFirst; i <= mid; ++i)
        {
            dest[nResult] = source[i];
            nResult++;
        }
    }
}

template <typename T>
void mergePass(GLDVector<T> &source, GLDVector<T> &dest, int subSection, int n, CSortCompareEvent *compare, bool ascend)
{
    int nIndex = 0;
    // 归并两个大小为SubSection的相邻段
    while (nIndex <= n - 2 * subSection)
    {
        merge(source, dest, nIndex, nIndex + subSection - 1, nIndex + 2 * subSection - 1, compare, ascend);
        nIndex = nIndex + 2 * subSection;
    }
    // 剩下不足两个元素
    if (nIndex + subSection < n)
    {
        merge(source, dest, nIndex, nIndex + subSection - 1, n - 1, compare, ascend);
    }
    else          // 把最后一段复制到ADest
    {
        for (int j = nIndex; j <= n - 1; ++j)
        {
            dest[j] = source[j];
        }
    }
}

template <typename T>
void doSort(int lowIndex, int highIndex, GLDVector<T> &sortList, CSortCompareEvent *compare, bool ascend)
{
    int nIt1 = 0;
    int nIt2 = 0;
    PtrInt pInt1 = 0;
    PtrInt pInt2 = 0;
    do
    {
        nIt1 = lowIndex;
        nIt2 = highIndex;
        void *ptr = (sortList.at((lowIndex + highIndex) >> 1));
        pInt1 = (PtrInt)ptr;
        do
        {
            while (doSortCompare(sortList.at(nIt1), (void*)(pInt1), compare, ascend) == crLessThan)
            {
                nIt1++;
            }
            while (doSortCompare(sortList.at(nIt2), (void*)(pInt1), compare, ascend) == crGreaterThan)
            {
                nIt2--;
            }
            if (nIt1 <= nIt2)
            {
                pInt2 = PtrInt(sortList.at(nIt1));
                sortList[nIt1] = sortList[nIt2];
                sortList[nIt2] = (T)(pInt2);
                nIt1++;
                nIt2--;
            }
        } while (nIt1 <= nIt2);
        if (lowIndex < nIt2)
        {
            doSort(lowIndex, nIt2, sortList, compare, ascend);
        }
        lowIndex = nIt1;
    } while (nIt1 < highIndex);
}

template <typename T>
void mergeSort(GLDVector<T> &sortList, CSortCompareEvent *compare, bool ascend = true)
{
    int nSubSection = 1;      // 段的大小
    int nCount = sortList.count();
    GLDVector<T> oList; // 用于中间转换的数组列表
    for (int i = 0; i < nCount; i++)
    {
        oList.push_back(NULL);
    }
    while (nSubSection < nCount)
    {
        mergePass(sortList, oList, nSubSection, nCount, compare, ascend); // 从ASource归并到oList
        nSubSection = nSubSection << 1;
        mergePass(oList, sortList, nSubSection, nCount, compare, ascend); // 从oList归并到ASource
        nSubSection = nSubSection << 1;
    }
}

template <typename T>
void quickSort(GLDVector<T> &sortList, CSortCompareEvent *compare, bool ascend = true)
{
    if (sortList.size() <= 1)
    {
        return;
    }
    doSort(0, int(sortList.size()) - 1, sortList, compare, ascend);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.08.29
  功能：二分法查找
  参数：searchList -- 查找列表
        compare -- 比较函数
       keyValue -- 键值
        *index -- 返回的匹配项在列表中的下标
        seekOption -- 查找选项
  返回：是否找到标志
  说明；假如找不到keyValue对应的项
        如果seekOption = soFirst，*index值为小于AKeyValue在最近项，
        如果seekOption = soLast，*index值为大于AKeyValue在最近项
-----------------------------------------------------------------------------*/
template <typename T>
bool binSearch(GLDVector<T> &searchList, ISearchCompareEvent *compare, const GVariant &keyValue, long *index, GSeekOption seekOption)
{
    bool result = false;
    if (searchList.size() == 0)
    {
        *index = 0;
        return result;
    }
    int nLow = 0;
    int nHigh = int(searchList.size()) - 1;
    *index = nHigh;
    while (nLow <= nHigh)              // 二分法查找
    {
        *index = (nLow + nHigh) / 2;
        switch (compare->compare(searchList.at(*index), keyValue))
        {
        case crLessThan:
            nLow = *index + 1;
            break;
        case crGreaterThan:
            nHigh = *index - 1;
            break;
        default:
            result = true;
            break;
        }
        if (result)
        {
            break;
        }
    }

    if (!result)                  // 处理找不到时的位置
    {
        switch (seekOption)
        {
        case soFirst:
            if (compare->compare(searchList.at(*index), keyValue) == crGreaterThan)
            {
                (*index)--;
            }
            break;
        case soLast:
            if (compare->compare(searchList.at(*index), keyValue) == crLessThan)
            {
                (*index)++;
            }
            break;
        default:
            break;
        }
    }
    else                          // 找到匹配项，处理可能多项相同键值的情况
    {
        switch (seekOption)
        {
        case soFirst:                        // 定位到第一条符合的数据
        {
            for (int i = *index-1; i >= 0; i--)
            {
                if (compare->compare(searchList.at(i), keyValue) == crEqual)
                {
                    *index = i;
                }
                else
                {
                    break;
                }
            }
            break;
        }
        case soLast:                         // 定位到最后一条符合的记录
        {
            for (int i = *index + 1; i != searchList.count(); ++i)
            {
                if (compare->compare(searchList.at(i), keyValue) == crEqual)
                {
                    *index = i;
                }
                else
                {
                    break;
                }
            }
            break;
        }
        default:
            break;
        }
    }
    return result;
}

GLDCOMMONSHARED_EXPORT void fast_sort(int *ptr, int begin, int end);

#endif // GLDSORTUTILS_H
