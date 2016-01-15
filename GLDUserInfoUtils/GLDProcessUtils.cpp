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

void errorMsg(LPTSTR lpszFunction);
BOOL setPrivilege(HANDLE hProcess, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Shell32.lib")

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

    if (m_hProcess == 0)
    {
        errorMsg(TEXT("OpenProcess"));
    }

    ZeroMemory(&m_ftPrevSysKernel, sizeof(FILETIME));
    ZeroMemory(&m_ftPrevSysUser, sizeof(FILETIME));

    ZeroMemory(&m_ftPrevProcKernel, sizeof(FILETIME));
    ZeroMemory(&m_ftPrevProcUser, sizeof(FILETIME));
}

ULONGLONG CpuUsage::subtractTimes(const FILETIME &ftA, const FILETIME &ftB)
{
    LARGE_INTEGER liA, liB;
    liA.LowPart = ftA.dwLowDateTime;
    liA.HighPart = ftA.dwHighDateTime;

    liB.LowPart = ftB.dwLowDateTime;
    liB.HighPart = ftB.dwHighDateTime;

    return liA.QuadPart - liB.QuadPart;
}

ULONGLONG CpuUsage::addTimes(const FILETIME &ftA, const FILETIME &ftB)
{
    LARGE_INTEGER liA, liB;
    liA.LowPart = ftA.dwLowDateTime;
    liA.HighPart = ftA.dwHighDateTime;

    liB.LowPart = ftB.dwLowDateTime;
    liB.HighPart = ftB.dwHighDateTime;

    return liA.QuadPart + liB.QuadPart;
}

bool CpuUsage::enoughTimePassed()
{
    const int c_minElapsedMS = 250;//milliseconds

    ULONGLONG dwCurrentTickCount = GetTickCount();
    return (dwCurrentTickCount - m_dwLastRun) > c_minElapsedMS;
}
#ifndef USE_DEPRECATED_FUNCS

ULONGLONG CpuUsage::getSystemNonIdleTimes()
{
    FILETIME ftSysIdle, ftSysKernel, ftSysUser;
    if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser))
    {
        errorMsg(TEXT("GetSystemTimes"));
        return 0;
    }
    return addTimes(ftSysKernel, ftSysUser);
}

ULONGLONG CpuUsage::getProcessNonIdleTimes()
{
    FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;
    if (!GetProcessTimes(m_hProcess, &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser) && false)
    {
        errorMsg(TEXT("GetProcessNonIdleTimes"));
        return 0;
    }
    return addTimes(ftProcKernel, ftProcUser);
}
#endif

ULONGLONG CpuUsage::getUsageEx()
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

BOOL setPrivilege(HANDLE hProcess, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    HANDLE hToken;
    if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        errorMsg(TEXT("OpenProcessToken"));
        return FALSE;
    }
    LUID luid;
    if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid))
    {
        errorMsg(TEXT("LookupPrivilegeValue"));
        return FALSE;
    }
    TOKEN_PRIVILEGES tkp;
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luid;
    tkp.Privileges[0].Attributes = (bEnablePrivilege) ? SE_PRIVILEGE_ENABLED : FALSE;
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
    {
        errorMsg(TEXT("AdjustTokenPrivileges"));
        return FALSE;
    }
    return TRUE;
}

void errorMsg(LPTSTR lpszFunction)
{
    Q_UNUSED(lpszFunction)
    // 处理错误消息不正确，不要弹出MessageBox，不要ExitProcess退出进程，
    // 先把代码注释掉

    // Retrieve the system error message for the last-error code

//    LPVOID lpMsgBuf;
//    LPVOID lpDisplayBuf;
//    DWORD dw = GetLastError();

//    FormatMessage(
//        FORMAT_MESSAGE_ALLOCATE_BUFFER |
//        FORMAT_MESSAGE_FROM_SYSTEM |
//        FORMAT_MESSAGE_IGNORE_INSERTS,
//        NULL,
//        dw,
//        LANG_USER_DEFAULT,
//        (LPTSTR) &lpMsgBuf,
//        0, NULL );

//    // Display the error message

//    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
//                                      (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
//    StringCchPrintf((LPTSTR)lpDisplayBuf,
//                    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
//                    TEXT("%s failed with error %d: %s"),
//                    lpszFunction, dw, lpMsgBuf);
//    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);


//    LocalFree(lpMsgBuf);
//    LocalFree(lpDisplayBuf);
//    ExitProcess(dw);
}

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
    return pmc.PagefileUsage/1024;
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
    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    if (hProcess != NULL)
    {
        TCHAR* procName = new TCHAR[MAX_PATH];
        GetModuleFileNameEx(hProcess, NULL, procName, MAX_PATH);    
        return ""/*extractFileName(QString::fromWCharArray(procName))*/;
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
                continue;

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
    HANDLE hHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  //为当前系统进程建立快照
    DWORD dwId = ::GetCurrentProcessId();     //当前进程的Id
    if (INVALID_HANDLE_VALUE !=hHandle)      //如果快照建立成功
    {
         PROCESSENTRY32 stEntry;
         stEntry.dwSize = sizeof(PROCESSENTRY32);
         if (Process32First(hHandle, &stEntry))     //在快照中查找一个进程,stEntry返回进程相关属性和信息
         {
             do{
                 if (wcsstr(stEntry.szExeFile, szEXEName))   //比较该进程名称是否与strProcessName相符
                 {
                    if (dwId != stEntry.th32ProcessID)       //如果相等，且该进程的Id与当前进程不相等，则找到
                    {
                        HANDLE h_Process = OpenProcess(PROCESS_QUERY_INFORMATION |
                                                     PROCESS_VM_READ,
                                                     FALSE, stEntry.th32ProcessID);
                        if (h_Process != NULL)
                        {
                            WCHAR name[MAX_PATH+1] = {0};
                            GetModuleFileNameEx(h_Process, NULL, name, MAX_PATH+1);
                            retList.append(QString::fromWCharArray(name));
                            CloseHandle(h_Process);
                        }
                    }
                 }
            }while (Process32Next(hHandle, &stEntry));   //再快照中查找下一个进程。
         }
//       CloseToolhelp32Snapshot(hHandle);             //释放快照句柄。
    }
    return retList;
}

bool GLDProcessFunc::killProcessByAbPath(const QString &lpProcessPath)
{
    bool bRet = false;
    QFileInfo fileInfo(lpProcessPath);
    TCHAR szEXEName[MAX_PATH] = {0};
    fileInfo.fileName().toWCharArray(szEXEName);
    HANDLE hHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  //为当前系统进程建立快照
    DWORD dwId = ::GetCurrentProcessId();     //当前进程的Id
    if (INVALID_HANDLE_VALUE !=hHandle)      //如果快照建立成功
    {
        PROCESSENTRY32 stEntry;
        stEntry.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hHandle, &stEntry))     //在快照中查找一个进程,stEntry返回进程相关属性和信息
        {
            do{
                if (wcsstr(stEntry.szExeFile, szEXEName))   //比较该进程名称是否与strProcessName相符
                {
                    if (dwId != stEntry.th32ProcessID)       //如果相等，且该进程的Id与当前进程不相等，则找到
                    {
                        HANDLE h_Process = OpenProcess(PROCESS_ALL_ACCESS, TRUE, stEntry.th32ProcessID);
                        if (h_Process != NULL)
                        {
                            WCHAR name[MAX_PATH+1] = {0};
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
            }while (Process32Next(hHandle, &stEntry));   //再快照中查找下一个进程。
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
                    processIDList.append( pe.th32ProcessID);
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
        QString strExePath = dir.absolutePath();        //去除路径中的。。
        ShellExecute(0, L"open", strExePath.toStdWString().c_str(), NULL, NULL, SW_SHOW);
    }
}

HANDLE GLDProcessFunc::getCurrentID()
{
    return GetCurrentProcess();
}

bool GLDProcessFunc::isProcessRunning(TCHAR *szEXEName)
{
    HANDLE hHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  //为当前系统进程建立快照
    DWORD dwId = ::GetCurrentProcessId();     //当前进程的Id
    if (INVALID_HANDLE_VALUE != hHandle)      //如果快照建立成功
    {
         PROCESSENTRY32 stEntry;
         stEntry.dwSize = sizeof(PROCESSENTRY32);
         if (Process32First(hHandle, &stEntry))     //在快照中查找一个进程,stEntry返回进程相关属性和信息
         {
          do{
                 if (wcsstr(stEntry.szExeFile, szEXEName))   //比较该进程名称是否与strProcessName相符
                 {
                    if (dwId != stEntry.th32ProcessID)       //如果相等，且该进程的Id与当前进程不相等，则找到
                        return true;
                 }
            }while (Process32Next(hHandle, &stEntry));   //再快照中查找下一个进程。
         }
    //   CloseToolhelp32Snapshot(hHandle);             //释放快照句柄。
    }
    return false;
}

bool GLDProcessFunc::isProcessRunning(const QStringList &exeNameList)
{
    HANDLE hHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  //为当前系统进程建立快照
    DWORD dwId = ::GetCurrentProcessId();     //当前进程的Id
    if (INVALID_HANDLE_VALUE !=hHandle)      //如果快照建立成功
    {
         PROCESSENTRY32 stEntry;
         stEntry.dwSize = sizeof(PROCESSENTRY32);

         if (Process32First(hHandle, &stEntry))     //在快照中查找一个进程,stEntry返回进程相关属性和信息
         {
          do{
                 if(exeNameList.contains(QString::fromStdWString(stEntry.szExeFile), Qt::CaseInsensitive))
                 {
                    if (dwId != stEntry.th32ProcessID)       //如果相等，且该进程的Id与当前进程不相等，则找到
                        return true;
                 }
            }while (Process32Next(hHandle, &stEntry));   //再快照中查找下一个进程。
         }
    //   CloseToolhelp32Snapshot(hHandle);             //释放快照句柄。
    }
    return false;
}

bool GLDProcessFunc::isProcessRunning(const QString &processName)
{
    return GLDProcessFunc::isProcessRunning((TCHAR*)processName.toStdWString().c_str());
}
