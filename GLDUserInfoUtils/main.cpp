#include <QCoreApplication>
#include "GLDCpuInfoUtils.h"
#include "GLDDiskInfoUtils.h"
#include "GLDMemoryInfoUtils.h"
#include "GLDProcessUtils.h"
#include <windows.h>
#include <Psapi.h>

using namespace GlodonDiskInfo;
using namespace GlodonCpuInfo;
using namespace GlodonMemoryInfo;
using namespace GlodonProcessInfo;

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString cpuBrand = getCpuBrand();
    qDebug() << "cpuBrand is" << cpuBrand;

    ulong speed = getCpuSpeed();
    qDebug() << "speed is" << speed;

    ulong count = getCpuCount();
    qDebug() << "count is" << count;

    CoreCount coreCount = getCpuCoreCount();
    qDebug() << "coreCount's cpuCoreCount is" << coreCount.m_cpuCoreCount;
    qDebug() << "coreCount's cpuLogicalCoreCount is" << coreCount.m_cpuLogicalCoreCount;


    CpuInfo cpuInfo = getCpuInfo();
    qDebug() << "cpuInfo is" << cpuInfo.m_cpuBrand;
    qDebug() << "cpuInfo is" << cpuInfo.m_coreCount.m_cpuCoreCount;
    qDebug() << "cpuInfo is" << cpuInfo.m_coreCount.m_cpuLogicalCoreCount;
    qDebug() << "cpuInfo is" << cpuInfo.m_processorPackageCount;
    qDebug() << "cpuInfo is" << cpuInfo.m_processSpeed;


    DiskInfomation diskInfo;

    FS fileSystem = getFileSystemType("C:\\");
    qDebug() << "FS is" << fileSystem;

    QString voluName = getSystemVolumeName();
    qDebug() << "system volume name is" << voluName;

    QString currentVolName = getCurrentVolumeName();
    qDebug() << "current volume name is" << currentVolName;

    QString serialNumber = getPhysicalDriveSerialNumber();
    qDebug() << "serialNumber is" << serialNumber;

    ulong volNumber = getVolumeNum();
    qDebug() << "volNumber is" << volNumber;

    QString volType = getVolumeTypeItem("D:\\");
    qDebug() << "volType is" << volType;

    qulonglong ri64FreeBytesToCaller;
    qulonglong ri64TotalBytes;
    getVolumeSpace("E:\\", ri64FreeBytesToCaller, ri64TotalBytes);
    qDebug() << "E volume's free space is" << ri64FreeBytesToCaller;
    qDebug() << "E volume's totle space is" << ri64TotalBytes;

    qulonglong diskFreeBytesToCaller;
    qulonglong diskTotalBytes;
    getDiskSpaceInfo(diskFreeBytesToCaller, diskTotalBytes);
    qDebug() << "Disk free space is" << diskFreeBytesToCaller;
    qDebug() << "Disk totle space is" << diskTotalBytes;

    QVector<DiskInfomation> diskInfoVet = getAllVolumeInfo();
    foreach(DiskInfomation disk , diskInfoVet)
    {
        qDebug() << "disk name is" << disk.m_strDiskName;
        qDebug() << "file system is " << disk.m_strFileSystem;

        qDebug() << "disk type name is" << disk.m_strTypeName;
        qDebug() << "totle space is" << disk.m_dwTotalMBytes;
        qDebug() << "free space is" << disk.m_dwFreeMBytes;
    }

    PhysMemInfo memInfo = getPhysMemInfo();
    qDebug() << "memInfo's m_totalPhys is" << memInfo.m_totalPhys;
    qDebug() << "memInfo's m_availPhys is " << memInfo.m_availPhys;
    qDebug() << "memInfo's m_usedPhys is " << memInfo.m_usedPhys;

    VirMemInfo virMemInfo = getVirMemInfo();
    qDebug() << "virMemInfo's m_totalVirtual is" << virMemInfo.m_totalVirtual;
    qDebug() << "virMemInfo's m_availVirtual is " << virMemInfo.m_availVirtual;


    QString processName = "notepad++.exe";
    SIZE_T cpuUsage = getCurrentWorkingSet(processName);
    SIZE_T cpuUsageById = getCurrentWorkingSet(23856);


    QString process = getNameByID(8048);
    qDebug() << "process's name is " << process;

    ulong privateWS1 = getPrivateWorkingSet(23576);
    ulong privateWS2 = getPrivateWorkingSet(processName);

    ulong sharedWork = getSharedWorkingSet(processName);

    QString exeName = "GLDTableViewBasicDemo.exe";
    ulong exeNameWs = getCurrentWorkingSet(exeName);
    ulong exeNamePeeKWs = getPeekWorkingSet(exeName);
    ulong exeNamePWs = getPrivateWorkingSet(exeName);

    ulong exeNameSharedWork = getSharedWorkingSet(exeName);

    return a.exec();
}
