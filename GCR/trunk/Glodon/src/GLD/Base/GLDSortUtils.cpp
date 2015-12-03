#include "GLDSortUtils.h"

void fast_sort(int *ptr, int begin, int end)
{
    if (!ptr)
    {
        return;
    }
    int nTemp = *(ptr + begin);//设置初始比较基准数据
    int nIt1 = begin + 1;
    int nIt2 = end;
    int nCurPosition = begin;//定义开头和结尾的I j
    bool bDirection = false;
    while (nIt1 <= nIt2)
    {
        if (bDirection)
        {
            if (*(ptr + nIt1) < nTemp)//如果当前数据小于基准数据 那么换位置 改当前位置
            {
                *(ptr + nCurPosition) = *(ptr + nIt1);
                nCurPosition = nIt1;
                bDirection = false;
            }
            nIt1++;
        }
        else
        {
            //先从后到前比较数据
            if (*(ptr + nIt2) > nTemp)
            {//如果最后一个大于基准 那么最后一个数据赋值给当前基准数据的那个位置 调整基准数据的位置
                *(ptr + nCurPosition) = *(ptr + nIt2);//
                nCurPosition = nIt2;
                bDirection = true;
            }
            nIt2--;
        }
    }

    *(ptr + nCurPosition) = nTemp;
    if (nCurPosition - begin > 1)//前面小的比较
    {
        fast_sort(ptr, begin, nCurPosition - 1);
    }
    if (end - nCurPosition > 1)//后面大的比较
    {
        fast_sort(ptr, nCurPosition + 1, end);
    }
}
