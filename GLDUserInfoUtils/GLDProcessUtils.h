#ifndef GLDPROCESSFUNC_H
#define GLDPROCESSFUNC_H

#include <qt_windows.h>
#include <WinNT.h>

#include <QList>
#include <QString>
#include <QStringList>

namespace GlodonProcessInfo
{
    ULONGLONG getCpuUsage(DWORD processID);
    ULONGLONG getCpuUsage(const QString &processName);

    // 当前指定进程的占用的工作集(内存),KB为单元
    SIZE_T getCurrentWorkingSet(DWORD processID);
    SIZE_T getCurrentWorkingSet(const QString &processName);

    // 当前指定进程的占用的峰值工作集(内存),KB为单元
    SIZE_T getPeekWorkingSet(DWORD processID);
    SIZE_T getPeekWorkingSet(const QString &processName);

    // 当前指定进程的占用的专用工作集(内存),KB为单元
    ulong getPrivateWorkingSet(DWORD processID);
    SIZE_T getPrivateWorkingSet(const QString &processName);

    // 当前指定进程的占用的共享工作集(内存),KB为单元
    ulong getSharedWorkingSet(DWORD processID);
    SIZE_T getSharedWorkingSet(const QString &processName);

    QString getNameByID(DWORD processID);
    DWORD getIDByName(const QString &processName);

    SIZE_T getCurrentWorkingSet(HANDLE handle);
    SIZE_T getPeekWorkingSet(HANDLE handle);

    HANDLE getHandleByName(const QString &processName);
    HANDLE getHandleByID(DWORD processId);

    HANDLE getCurrentID();
}


#endif // GLDPROCESSFUNC_H
