#ifndef GLDMEMORYINFOUTILS_H
#define GLDMEMORYINFOUTILS_H

#include <windows.h>

namespace GLDMemoryInfo
{
    typedef struct PhysicalMemoryInfo
    {
        __int64     m_totalPhys;     // 总内存数,单位M
        __int64     m_availPhys;     // 可用内存数,单位M
        int         m_usedPhys;      // 已用内存百分比,范围:0-100
    } PhysMemInfo;

    typedef struct VirtualMemoryInfo
    {
        __int64     m_totalVirtual;  // 总虚拟内存数,单位M
        __int64     m_availVirtual;  // 可用虚拟内存数,单位M
    } VirMemInfo;

    PhysMemInfo getPhysMemInfo();

    VirMemInfo getVirMemInfo();
}

#endif // GLDMEMORYINFOUTILS_H
