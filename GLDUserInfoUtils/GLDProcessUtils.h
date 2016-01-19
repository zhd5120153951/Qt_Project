#ifndef GLDPROCESSFUNC_H
#define GLDPROCESSFUNC_H

#include "CBBNameSpace.h"

#include <qt_windows.h>
#include <WinNT.h>

#include <QList>
#include <QString>
#include <QStringList>

CBB_GLODON_BEGIN_NAMESPACE

    /**
     * @brief 获取某个进程的CPU使用率
     * @param processID  进程PID
     * @return
     */
    ULONGLONG getCpuUsage(DWORD processID);

    /**
     * @brief 获取某个进程的CPU使用率
     * @param processID  进程名
     * @return
     */
    ULONGLONG getCpuUsage(const QString &processName);

    /**
     * @brief 当前指定进程的占用的工作集(内存),KB为单元
     * @param processID  进程PID
     * @return
     */
    ulong getCurrentWorkingSetByHandle(DWORD processID);

    /**
     * @brief 当前指定进程的占用的工作集(内存),KB为单元
     * @param processID  进程名
     * @return
     */
    ulong getCurrentWorkingSetByHandle(const QString &processName);

    /**
     * @brief 当前指定进程的占用的峰值工作集(内存),KB为单元
     * @param processID  进程PID
     * @return
     */
    ulong getPeekWorkingSetByHandle(DWORD processID);

    /**
     * @brief 当前指定进程的占用的峰值工作集(内存),KB为单元
     * @param processID  进程名
     * @return
     */
    ulong getPeekWorkingSetByHandle(const QString &processName);

    /**
     * @brief 当前指定进程的占用的专用工作集(内存),KB为单元
     * @param processID  进程PID
     * @return
     */
    ulong getPrivateWorkingSet(DWORD processID);

    /**
     * @brief 当前指定进程的占用的专用工作集(内存),KB为单元
     * @param processName  进程名
     * @return
     */
    ulong getPrivateWorkingSet(const QString &processName);

    /**
     * @brief 当前指定进程的占用的共享工作集(内存),KB为单元
     * @param processID  进程PID
     * @return
     */
    ulong getSharedWorkingSet(DWORD processID);

    /**
     * @brief 当前指定进程的占用的共享工作集(内存),KB为单元
     * @param processID  进程名
     * @return
     */
    ulong getSharedWorkingSet(const QString &processName);

    /**
     * @brief 根据进程PID获取进程名
     * @param processID  进程PID
     * @return
     */
    QString getNameByID(DWORD processID);

    /**
     * @brief 根据进程名获取进程PID
     * @param processID  进程名
     * @return
     */
    ulong getIDByName(const QString &processName);

    /**
     * @brief 根据句柄获取当前工作集
     * @param handle  句柄
     * @return
     */
    ulong getCurrentWorkingSetByHandle(HANDLE handle);

    /**
     * @brief 根据句柄获取峰值工作集
     * @param handle  句柄
     * @return
     */
    ulong getPeekWorkingSetByHandle(HANDLE handle);

    /**
     * @brief 根据进程名获取句柄
     * @param processName  进程名
     * @return
     */
    HANDLE getHandleByName(const QString &processName);

    /**
     * @brief 根据进程PID获取句柄
     * @param processId  进程PID
     * @return
     */
    HANDLE getHandleByID(DWORD processId);

    /**
     * @brief 获取当前进程句柄
     * @return
     */
    HANDLE getCurrentID();

CBB_GLODON_END_NAMESPACE


#endif // GLDPROCESSFUNC_H
