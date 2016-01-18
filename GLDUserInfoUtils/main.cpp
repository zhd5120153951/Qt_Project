#include <QCoreApplication>
#include "GLDCpuInfoUtils.h"
#include "GLDDiskInfoUtils.h"

using namespace GlodonDiskInfo;
using namespace GlodonCpuInfo;

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


    GLDDiskInfo disk;
    DiskInfomation diskInfo;

    FS fileSystem = disk.getFileSystemType("C:\\");
    qDebug() << "FS is" << fileSystem;

    QString voluName = disk.getSystemVolumeName();
    qDebug() << "system volume name is" << voluName;

    QString currentVolName = disk.getCurrentVolumeName();
    qDebug() << "current volume name is" << currentVolName;

    QString serialNumber = disk.getPhysicalDriveSerialNumber();
    qDebug() << "serialNumber is" << serialNumber;

    ulong volNumber = disk.getVolumeNum();
    qDebug() << "volNumber is" << volNumber;

    QString volType = disk.getVolumeTypeItem("D:\\");
    qDebug() << "volType is" << volType;

    qulonglong ri64FreeBytesToCaller;
    qulonglong ri64TotalBytes;
    disk.getVolumeSpace("E:\\", ri64FreeBytesToCaller, ri64TotalBytes);
    qDebug() << "E volume's free space is" << ri64FreeBytesToCaller;
    qDebug() << "E volume's totle space is" << ri64TotalBytes;

    qulonglong diskFreeBytesToCaller;
    qulonglong diskTotalBytes;
    disk.getDiskSpaceInfo(diskFreeBytesToCaller, diskTotalBytes);
    qDebug() << "Disk free space is" << diskFreeBytesToCaller;
    qDebug() << "Disk totle space is" << diskTotalBytes;

    QVector<DiskInfomation> diskInfoVet = disk.getAllVolumeInfo();
    foreach(DiskInfomation disk , diskInfoVet)
    {
        qDebug() << "disk name is" << disk.m_strDiskName;
        qDebug() << "file system is " << disk.m_strFileSystem;

        qDebug() << "disk type name is" << disk.m_strTypeName;
        qDebug() << "totle space is" << disk.m_dwTotalMBytes;
        qDebug() << "free space is" << disk.m_dwFreeMBytes;
    }

    return a.exec();
}
