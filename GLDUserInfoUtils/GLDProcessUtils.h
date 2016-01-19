#ifndef GLDPROCESSFUNC_H
#define GLDPROCESSFUNC_H

#include <qt_windows.h>
#include <WinNT.h>

#include <QList>
#include <QString>
#include <QStringList>

namespace GlodonProcessInfo
{
    class GLDProcessFunc
    {
    public:
        static ULONGLONG getCpuUsage(DWORD processID);
        static ULONGLONG getCpuUsage(const QString &processName);

        // 当前指定进程的占用的工作集(内存),KB为单元
        static SIZE_T getCurrentWorkingSet(DWORD processID);
        static SIZE_T getCurrentWorkingSet(const QString &processName);

        // 当前指定进程的占用的峰值工作集(内存),KB为单元
        static SIZE_T getPeekWorkingSet(DWORD processID);
        static SIZE_T getPeekWorkingSet(const QString &processName);

        // 当前指定进程的占用的专用工作集(内存),KB为单元
        static ulong getPrivateWorkingSet(DWORD processID);
        static SIZE_T getPrivateWorkingSet(const QString &processName);

        // 当前指定进程的占用的共享工作集(内存),KB为单元
        static ulong getSharedWorkingSet(DWORD processID);
        static SIZE_T getSharedWorkingSet(const QString &processName);


        static QString getNameByID(DWORD processID);
    private:
        static DWORD getIDByName(const QString &processName);

        static SIZE_T getCurrentWorkingSet(HANDLE handle);
        static SIZE_T getPeekWorkingSet(HANDLE handle);

        static HANDLE getHandleByName(const QString &processName);
        static HANDLE getHandleByID(DWORD processId);

    public:
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
