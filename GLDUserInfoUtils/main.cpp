#include <QCoreApplication>
#include "GLDSysInfoUtils.h"
#include "GLDDiskInfoUtils.h"

using namespace GLDDISKINFO;

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    GLDDiskInfo disk;

    DiskInfomation diskInfo;
    disk.getLastVolumeInfo(diskInfo);

    disk.getFileSystemType("C:\\");

    ULONGLONG llOfSectors;
    DWORD dwDiskNum = 2;
    disk.getDiskSize(llOfSectors, dwDiskNum);


    disk.getVolumeSize(llOfSectors, "C:\\");

    return a.exec();
}
