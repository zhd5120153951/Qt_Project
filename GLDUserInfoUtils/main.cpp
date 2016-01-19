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

    QString str = "chrome.exe";
    ulong id = getIDByName(str);
    return a.exec();
}
