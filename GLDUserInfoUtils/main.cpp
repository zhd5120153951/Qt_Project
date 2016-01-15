#include <QCoreApplication>
#include "GLDCpuInfoUtils.h"
#include "GLDDiskInfoUtils.h"

using namespace GLDDISKINFO;

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    GLDDiskInfo disk;

    DiskInfomation diskInfo;

    disk.getFileSystemType("C:\\");

    ULONGLONG llOfSectors;
    DWORD dwDiskNum = 2;
    disk.getDiskSize(llOfSectors, dwDiskNum);


    disk.getVolumeSize(llOfSectors, "C:\\");

    disk.getFileSystemType("C:\\");
    QString no = disk.getPhysicalDriveSerialNumber();
    QString name1 = disk.getVolumeTypeItem("C:\\");

    QString name = disk.getSystemVolumeName();
    qDebug() << "system disk name is" << name;
    QString diskname = disk.getCurrentVolumeName();
    qDebug() << "current disk name is" << diskname;
    return a.exec();
}
