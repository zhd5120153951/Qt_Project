#ifndef GLDCPUINFOUTILS_H
#define GLDCPUINFOUTILS_H

#include <windows.h>
#include <QString>

namespace GlodonCpuInfo
{
    typedef struct CpuCoreCount
    {
        int m_cpuLogicalCoreCount;   // CPU核心数
        int m_cpuCoreCount;          // 逻辑CPU数
    } CoreCount;

    struct CpuInfo
    {
        QString   m_cpuBrand;              // CPU商标
        int       m_processorPackageCount; // 物理cpu个数
        CoreCount m_coreCount;             // 核数
        ulong     m_processSpeed;          // cpu最大速度(单位MHZ)
    };

    QString getCpuBrand();

    ulong getCpuSpeed();

    ulong getCpuCount();

    CoreCount getCpuCoreCount();

    CpuInfo getCpuInfo();
}

#endif // GLDCPUINFOUTILS_H
