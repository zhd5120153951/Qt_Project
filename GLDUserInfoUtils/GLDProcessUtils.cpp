#include "GLDProcessUtils.h"
//#include <Windows.h>
#include <Psapi.h>
#include <strsafe.h>
#include <Tlhelp32.h>
#include <ShellAPI.h>

#include <QDir>
#include <QThread>
#include <QProcess>
#include <QFileInfo>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Advapi32.lib")

namespace GLDProcessInfo
{
    BOOL setPrivilege(HANDLE hProcess, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
    {
        HANDLE hToken;
        if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
        {
            return FALSE;
        }

        LUID luid;
        if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid))
        {
            return FALSE;
        }

        TOKEN_PRIVILEGES tkp;
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Luid = luid;
        tkp.Privileges[0].Attributes = (bEnablePrivilege) ? SE_PRIVILEGE_ENABLED : FALSE;

        if (!AdjustTokenPrivileges(hToken,
                                   FALSE,
                                   &tkp,
                                   sizeof(TOKEN_PRIVILEGES),
                                   (PTOKEN_PRIVILEGES)NULL,
                                   (PDWORD)NULL))
        {
            return FALSE;
        }

        return TRUE;
    }


    class CpuUsage
    {
    public:
        CpuUsage::CpuUsage(DWORD dwProcessID)
            : m_nCpuUsage(0),
              m_dwLastRun(0),
              m_lRunCount(0),
              m_dwProcessID(dwProcessID),
              m_ullPrevProcNonIdleTime(0),
              m_ullPrevSysNonIdleTime(0)
        {
            HANDLE hProcess = GetCurrentProcess();
            setPrivilege(hProcess, SE_DEBUG_NAME, TRUE);

            m_hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION , TRUE, m_dwProcessID);

            ZeroMemory(&m_ftPrevSysKernel, sizeof(FILETIME));
            ZeroMemory(&m_ftPrevSysUser, sizeof(FILETIME));

            ZeroMemory(&m_ftPrevProcKernel, sizeof(FILETIME));
            ZeroMemory(&m_ftPrevProcUser, sizeof(FILETIME));
        }

        ULONGLONG getUsageEx()
        {
            ULONGLONG nCpuCopy = m_nCpuUsage;

            if (::InterlockedIncrement(&m_lRunCount) == 1)
            {
                if (!enoughTimePassed())
                {
                    ::InterlockedDecrement(&m_lRunCount);
                    return nCpuCopy;
                }

                ULONGLONG ullSysNonIdleTime = getSystemNonIdleTimes();
                ULONGLONG ullProcNonIdleTime = getProcessNonIdleTimes();

                if (!isFirstRun())
                {
                    ULONGLONG ullTotalSys = ullSysNonIdleTime - m_ullPrevSysNonIdleTime;

                    if (ullTotalSys == 0)
                    {
                        ::InterlockedDecrement(&m_lRunCount);
                        return nCpuCopy;
                    }

                    m_nCpuUsage = ULONGLONG((ullProcNonIdleTime - m_ullPrevProcNonIdleTime) * 100.0 / (ullTotalSys));
                    m_ullPrevSysNonIdleTime = ullSysNonIdleTime;
                    m_ullPrevProcNonIdleTime = ullProcNonIdleTime;
                }

                m_dwLastRun = (ULONGLONG)GetTickCount();
                nCpuCopy = m_nCpuUsage;
            }

            ::InterlockedDecrement(&m_lRunCount);

            return nCpuCopy;
        }

        ULONGLONG getSystemNonIdleTimes()
        {
            FILETIME ftSysIdle, ftSysKernel, ftSysUser;

            if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser))
            {
                return 0;
            }

            return addTimes(ftSysKernel, ftSysUser);
        }

        ULONGLONG getProcessNonIdleTimes()
        {
            FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

            BOOL proceTime = GetProcessTimes(m_hProcess,
                                             &ftProcCreation,
                                             &ftProcExit,
                                             &ftProcKernel,
                                             &ftProcUser);
            if (!proceTime && false)
            {
                return 0;
            }

            return addTimes(ftProcKernel, ftProcUser);
        }

    private:
        ULONGLONG subtractTimes(const FILETIME& ftA, const FILETIME& ftB)
        {
            LARGE_INTEGER liA, liB;
            liA.LowPart = ftA.dwLowDateTime;
            liA.HighPart = ftA.dwHighDateTime;

            liB.LowPart = ftB.dwLowDateTime;
            liB.HighPart = ftB.dwHighDateTime;

            return liA.QuadPart - liB.QuadPart;
        }

        ULONGLONG addTimes(const FILETIME& ftA, const FILETIME& ftB)
        {
            LARGE_INTEGER liA, liB;
            liA.LowPart = ftA.dwLowDateTime;
            liA.HighPart = ftA.dwHighDateTime;

            liB.LowPart = ftB.dwLowDateTime;
            liB.HighPart = ftB.dwHighDateTime;

            return liA.QuadPart + liB.QuadPart;
        }

        bool enoughTimePassed()
        {
            const int c_minElapsedMS = 250;//milliseconds

            ULONGLONG dwCurrentTickCount = GetTickCount();
            return (dwCurrentTickCount - m_dwLastRun) > c_minElapsedMS;
        }

        bool isFirstRun() const
        {
            return (m_dwLastRun == 0);
        }

        //system total times
        FILETIME m_ftPrevSysKernel;
        FILETIME m_ftPrevSysUser;

        //process times
        FILETIME m_ftPrevProcKernel;
        FILETIME m_ftPrevProcUser;

        ULONGLONG m_ullPrevSysNonIdleTime;//这个变量和后面的便利记录上次获取的非idle的系统cpu时间和进程cpu时间.
        ULONGLONG m_ullPrevProcNonIdleTime;//这个类只绑定一个进程,在构造函数里面初始化进来

        ULONGLONG m_nCpuUsage;
        ULONGLONG m_dwLastRun;
        DWORD m_dwProcessID;
        HANDLE m_hProcess;
        volatile LONG m_lRunCount;
    };



    ULONGLONG GLDProcessFunc::getCpuUsage(const QString &processName)
    {
        DWORD handle = GLDProcessFunc::getIDByName(processName);

        if (handle != 0)
        {
            return GLDProcessFunc::getCpuUsage(handle);
        }

        return 0;
    }

    ULONGLONG GLDProcessFunc::getCpuUsage(DWORD processID)
    {
        if (0 == processID)
        {
            return 0;
        }

        CpuUsage cpu(processID);
        return cpu.getUsageEx();
    }

    ULONGLONG GLDProcessFunc::getMemoryInfo(const QString &processName)
    {
        DWORD handle = GLDProcessFunc::getIDByName(processName);

        if (handle != 0)
        {
            return GLDProcessFunc::getMemoryInfo(handle);
        }

        return 0;
    }

    ULONGLONG GLDProcessFunc::getMemoryInfo(DWORD processID)
    {
        PROCESS_MEMORY_COUNTERS pmc;
        GetProcessMemoryInfo((HANDLE)processID, &pmc, sizeof(pmc));
        return pmc.PagefileUsage / 1024;
    }

    DWORD GLDProcessFunc::getIDByName(const QString &processName)
    {
        PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

        if (hSnapshot != INVALID_HANDLE_VALUE)
        {
            if (Process32First(hSnapshot, &pe))
            {
                while (Process32Next(hSnapshot, &pe))
                {
                    if (lstrcmpi(processName.toStdWString().c_str(), pe.szExeFile) == 0)
                    {
                        return pe.th32ProcessID;
                    }
                }
            }

            CloseHandle(hSnapshot);
        }

        return 0;
    }

    QString GLDProcessFunc::getNameByID(DWORD processID)
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

        if (hProcess != NULL)
        {
            WCHAR procName[MAX_PATH + 1] = {0};
            GetModuleFileNameEx(hProcess, NULL, procName, MAX_PATH);
            std::wstring ws(procName);
            std::string processName(ws.begin(), ws.end());
            std::size_t pathDelim = processName.find_last_of("/\\");

            if (pathDelim != std::string::npos)
            {
                return QString(processName.substr(pathDelim + 1).c_str());
            }

            return "";
        }

        return "";

    }

    bool GLDProcessFunc::killProcess(const QString &lpProcessName)
    {
        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        if (!Process32First(hSnapShot, &pe))
        {
            return FALSE;
        }

        bool result = false;
        QString strProcessName = lpProcessName;

        while (Process32Next(hSnapShot, &pe))
        {
            QString scTmp = QString::fromUtf16((ushort*)pe.szExeFile);

            if (0 == scTmp.compare(strProcessName, Qt::CaseInsensitive))
            {
                DWORD dwProcessID = pe.th32ProcessID;
                if (dwProcessID == ::GetCurrentProcessId())
                {
                    continue;
                }

                HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
                ::TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
                result = true;
            }
        }

        return result;
    }

    QStringList GLDProcessFunc::getPathByName(const QString &lpProcessName)
    {
        QStringList retList;
        TCHAR szEXEName[MAX_PATH] = {0};
        lpProcessName.toWCharArray(szEXEName);
        //为当前系统进程建立快照
        HANDLE hHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        //当前进程的Id
        DWORD dwId = ::GetCurrentProcessId();
        //如果快照建立成功
        if (INVALID_HANDLE_VALUE !=hHandle)
        {
            PROCESSENTRY32 stEntry;
            stEntry.dwSize = sizeof(PROCESSENTRY32);
            //在快照中查找一个进程,stEntry返回进程相关属性和信息
            if (Process32First(hHandle, &stEntry))
            {
                do{
                    //比较该进程名称是否与strProcessName相符
                    if (wcsstr(stEntry.szExeFile, szEXEName))
                    {
                        //如果相等，且该进程的Id与当前进程不相等，则找到
                        if (dwId != stEntry.th32ProcessID)
                        {
                            HANDLE h_Process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                                           FALSE,
                                                           stEntry.th32ProcessID);
                            if (h_Process != NULL)
                            {
                                WCHAR name[MAX_PATH+1] = {0};
                                GetModuleFileNameEx(h_Process, NULL, name, MAX_PATH+1);
                                retList.append(QString::fromWCharArray(name));
                                CloseHandle(h_Process);
                            }
                        }
                    }
                }while (Process32Next(hHandle, &stEntry));//再快照中查找下一个进程
            }
            // 释放快照句柄
            // CloseToolhelp32Snapshot(hHandle);
        }
        return retList;
    }

    bool GLDProcessFunc::killProcessByAbPath(const QString &lpProcessPath)
    {
        bool bRet = false;
        QFileInfo fileInfo(lpProcessPath);
        TCHAR szEXEName[MAX_PATH] = {0};
        fileInfo.fileName().toWCharArray(szEXEName);
        //为当前系统进程建立快照
        HANDLE hHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        //当前进程的Id
        DWORD dwId = ::GetCurrentProcessId();
        //如果快照建立成功
        if (INVALID_HANDLE_VALUE !=hHandle)
        {
            PROCESSENTRY32 stEntry;
            stEntry.dwSize = sizeof(PROCESSENTRY32);
            //在快照中查找一个进程,stEntry返回进程相关属性和信息
            if (Process32First(hHandle, &stEntry))
            {
                do{
                    //比较该进程名称是否与strProcessName相符
                    if (wcsstr(stEntry.szExeFile, szEXEName))
                    {
                        //如果相等,且该进程的Id与当前进程不相等,则找到
                        if (dwId != stEntry.th32ProcessID)
                        {
                            HANDLE h_Process = OpenProcess(PROCESS_ALL_ACCESS,
                                                           TRUE,
                                                           stEntry.th32ProcessID);

                            if (h_Process != NULL)
                            {
                                WCHAR name[MAX_PATH + 1] = {0};
                                GetModuleFileNameEx(h_Process, NULL, name, MAX_PATH+1);
                                QString path = QString::fromWCharArray(name);

                                if (path.compare(lpProcessPath, Qt::CaseInsensitive)==0)
                                {
                                    TerminateProcess(h_Process, 0);
                                    CloseHandle(h_Process);
                                    bRet = true;
                                }
                            }
                        }
                    }
                }while (Process32Next(hHandle, &stEntry));//再快照中查找下一个进程
            }
        }
        return bRet;
    }

    QList<DWORD> GLDProcessFunc::getProcessIDList(const QStringList &processNameList)
    {
        QList<DWORD> processIDList;
        PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

        if (hSnapshot != INVALID_HANDLE_VALUE)
        {
            if (Process32First(hSnapshot, &pe))
            {
                while (Process32Next(hSnapshot, &pe))
                {
                    if (-1 != processNameList.indexOf(QString::fromStdWString(pe.szExeFile)))
                    {
                        processIDList.append(pe.th32ProcessID);
                    }

                    QThread::usleep(10);
                }
            }

            CloseHandle(hSnapshot);
        }

        return processIDList;
    }

    void GLDProcessFunc::startProcess(const QString &strExe, const QStringList &params)
    {
        QProcess::startDetached("\"" + strExe + "\"", params);
    }

    void GLDProcessFunc::startProcess(const QString &strExe)
    {
        if (!QProcess::startDetached("\"" + strExe + "\""))
        {
            QDir dir(strExe);
            QString strExePath = dir.absolutePath();
            ShellExecute(0, L"open", strExePath.toStdWString().c_str(), NULL, NULL, SW_SHOW);
        }
    }

    HANDLE GLDProcessFunc::getCurrentID()
    {
        return GetCurrentProcess();
    }

    bool GLDProcessFunc::isProcessRunning(TCHAR *szEXEName)
    {
        //为当前系统进程建立快照
        HANDLE hHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        //当前进程的Id
        DWORD dwId = ::GetCurrentProcessId();
        //如果快照建立成功
        if (INVALID_HANDLE_VALUE != hHandle)
        {
            PROCESSENTRY32 stEntry;
            stEntry.dwSize = sizeof(PROCESSENTRY32);
            //在快照中查找一个进程,stEntry返回进程相关属性和信息
            if (Process32First(hHandle, &stEntry))
            {
                do{
                    //比较该进程名称是否与strProcessName相符
                    if (wcsstr(stEntry.szExeFile, szEXEName))
                    {
                        //如果相等,且该进程的Id与当前进程不相等,则找到
                        if (dwId != stEntry.th32ProcessID)
                        {
                            return true;
                        }
                    }
                }while (Process32Next(hHandle, &stEntry));//再快照中查找下一个进程
            }
            // 释放快照句柄
            // CloseToolhelp32Snapshot(hHandle);
        }
        return false;
    }

    bool GLDProcessFunc::isProcessRunning(const QStringList &exeNameList)
    {
        //为当前系统进程建立快照
        HANDLE hHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        //当前进程的Id
        DWORD dwId = ::GetCurrentProcessId();
        //如果快照建立成功
        if (INVALID_HANDLE_VALUE !=hHandle)
        {
            PROCESSENTRY32 stEntry;
            stEntry.dwSize = sizeof(PROCESSENTRY32);
            // 在快照中查找一个进程,stEntry返回进程相关属性和信息
            if (Process32First(hHandle, &stEntry))
            {
                do{
                    if(exeNameList.contains(QString::fromStdWString(stEntry.szExeFile), Qt::CaseInsensitive))
                    {
                        //如果相等,且该进程的Id与当前进程不相等,则找到
                        if (dwId != stEntry.th32ProcessID)
                        {
                            return true;
                        }
                    }
                }while(Process32Next(hHandle, &stEntry));//在快照中查找下一个进程
            }
            // 释放快照句柄
            // CloseToolhelp32Snapshot(hHandle);
        }
        return false;
    }

    bool GLDProcessFunc::isProcessRunning(const QString &processName)
    {
        return GLDProcessFunc::isProcessRunning((TCHAR*)processName.toStdWString().c_str());
    }

}
