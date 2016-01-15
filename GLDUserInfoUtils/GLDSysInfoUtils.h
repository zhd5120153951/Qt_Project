#ifndef GLDUSERINFOUTILS_H
#define GLDUSERINFOUTILS_H

#include <windows.h>
#include <QString>

namespace GLDSysInfo
{
    namespace GLDCpuInfo
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

        int getCpuCount();

        CoreCount getCpuCoreCount();

        CpuInfo getCpuInfo();

        int getCpuUsage();
    }



    namespace GLDDiskInfo
    {
        typedef struct DiskSpaceInfo
        {
            double m_totalSpace;
            double m_availSpace;
            double m_usedSpace;
        } DiskSpaceInfo;

        typedef struct PartitionInfo
        {
            char                    chDrive;
            PARTITION_INFORMATION   info;
        } PartitionInfo, *LPPartitionInfo;


        typedef struct DiskInfo
        {
            int                        iPartitionSize;
            PPARTITION_INFORMATION     pPartitions;
        } DiskInfo, *LPDiskInfo;


        QString getDiskSerialNo();

        int getAllDiskPartitionInfo(LPDiskInfo* lpDisks);

        DiskSpaceInfo getDiskFreeSpace(QString driver);
    }
}



#endif // GLDUSERINFOUTILS_H
