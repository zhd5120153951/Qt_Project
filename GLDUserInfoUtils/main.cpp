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
    //disk.getAllVolumeInfo();

    qulonglong i64TotalBytes = 0;
    qulonglong i64FreeBytes = 0;
    disk.getVolumeSpace("C:\\", i64FreeBytes, i64TotalBytes);

    disk.getFileSystemType("C:\\");
    QString no = disk.getPhysicalDriveSerialNumber();
    qDebug() << "disk Serial Number is" << no;
    QString name1 = disk.getVolumeTypeItem("C:\\");

    QString name = disk.getSystemVolumeName();
    qDebug() << "system disk name is" << name;
    QString diskname = disk.getCurrentVolumeName();
    qDebug() << "current disk name is" << diskname;
    return a.exec();
}
