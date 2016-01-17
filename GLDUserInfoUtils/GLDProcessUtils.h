#ifndef GLDPROCESSFUNC_H
#define GLDPROCESSFUNC_H

#include <qt_windows.h>
#include <WinNT.h>

#include <QList>
#include <QString>
#include <QStringList>

namespace GLDProcessInfo
{
    class GLDProcessFunc
    {
    public:
        static ULONGLONG getCpuUsage(DWORD processID);
        static ULONGLONG getCpuUsage(const QString &processName);

        //当前指定进程的占用的内存KB为单元
        static ULONGLONG getMemoryInfo(DWORD processID);
        static ULONGLONG getMemoryInfo(const QString &processName);

        static DWORD getIDByName(const QString &processName);
        static QString getNameByID(DWORD processID);

        static bool isProcessRunning(TCHAR *szEXEName);
        static bool isProcessRunning(const QStringList &exeNameList);
        static bool isProcessRunning(const QString &processName);
        static bool killProcess(const QString &lpProcessName);
        static QStringList getPathByName(const QString &lpProcessName);
        static bool killProcessByAbPath(const QString &lpProcessPath);
        static QList<DWORD> getProcessIDList(const QStringList &processNameList);

        //运行指定程序,主要是防止路径中有空格造成问题
        static void startProcess(const QString &strExe, const QStringList &params);
        static void startProcess(const QString &strExe);
        static HANDLE getCurrentID();
    };
}


#endif // GLDPROCESSFUNC_H
