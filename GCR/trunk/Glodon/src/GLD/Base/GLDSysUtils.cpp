#include "GLDSysUtils.h"

#include <QStandardPaths>

#include "GLDGlobal.h"
#include "GLDVector.h"
#include "GLDStrUtils.h"
#include "GLDSettings.h"
#include "GLDDir.h"

#ifdef WIN32
#   include <windows.h>
#   include <nb30.h>
#   include <ShlObj.h>
#   ifdef _MSC_VER
#       pragma comment(lib, "NETAPI32.LIB")
#   endif
#else
#   include <unistd.h>
//#   include <sys/sysconf.h>
#   include <sys/ioctl.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <net/if.h>
#   include <stdio.h>
#endif

#if defined(__GNUC__)    // GCC
#include <cpuid.h>
#endif

#if _MSC_VER >=1400    // VC2005才支持intrin.h
#include <intrin.h>    // 所有Intrinsics函数
#endif

#ifdef WIN32
const GChar c_GLDPathDelim = 0x5c; // '\\'
const GChar c_GLDPathDelim_Linux = '/'; // '\\'
const GChar c_GLDDriveDelim = 0x3a; // ':'
#else
const GChar c_GLDPathDelim = 0x2f; // '/'
const GChar c_GLDPathDelim_Linux = '/'; // '\\'
const GChar c_GLDDriveDelim = 0x00; // ''
#endif

GChar pathDelim()
{
    return c_GLDPathDelim;
}

GChar driveDelim()
{
    return c_GLDDriveDelim;
}

GChar backSlashDelim()
{
    return 0x2f; // '/'
}

bool isPathDelimiter(const GString &s, int index)
{
    return (index > 0) && (index < s.length()) && ((s.at(index) == c_GLDPathDelim) || (s.at(index) == c_GLDPathDelim_Linux));
}

GString includeTrailingBackslash(const GString &s)
{
    return includeTrailingPathDelimiter(s);
}

GString includeTrailingPathDelimiter(const GString &s)
{
    GString result = s;
    if (!isPathDelimiter(s, s.length() - 1))
    {
        result += c_GLDPathDelim;
    }
    return result;
}

GString excludeTrailingBackslash(const GString &s)
{
    return excludeTrailingPathDelimiter(s);
}

GString excludeTrailingPathDelimiter(const GString &s)
{
    GString result = s;
    if (isPathDelimiter(s, s.length() - 1))
        result.chop(1);
    return result;
}

double fileDateToDateTime(int fileDate)
{
    int nYear(0);
    int nMonth(0);
    int nDay(0);
    int nHour(0);
    int nMin(0);
    int nSec(0);
    int nHi(0);
    int nLow(0);
    nHi = (fileDate & 0xffff0000) >> 16;
    nYear = (nHi >> 9) + 1980;
    nMonth = (nHi >> 5) & 15;
    nDay = (nHi & 31);
    nLow = fileDate & 0x0000ffff;
    nHour = nLow >> 11;
    nMin = (nLow >> 5) & 63;
    nSec = (nLow & 31) << 1;
    return dateTimeToDouble(GDateTime(GDate(nYear, nMonth, nDay), GTime(nHour, nMin, nSec)));
}

int DateTimeToFileDate(double value)
{
    int nResult = 0;
    int nYear(0);
    int nMonth(0);
    int nDay(0);
    int nHour(0);
    int nMin(0);
    int nSec(0);
    GDateTime dateTime = doubleToDateTime(value);
    GDate date = dateTime.date();
    GTime time = dateTime.time();
    if ((yearOf(value) < 1980) || (yearOf(value) > 2107))
        nResult = 0;
    else
    {
        nYear = date.year();
        nMonth = date.month();
        nDay = date.day();
        nHour = time.hour();
        nMin = time.minute();
        nSec = time.second();
        nResult |= ((nSec >> 1) | (nMin << 5) | (nHour << 11)) & 0x0000ffff;
        nResult |= ((nDay | (nMonth << 5) | (nYear - 1980) << 9)) << 16;
    }
    return nResult;
}

GString getUserNameDef()
{
#ifdef _MSC_VER
    char *pBuff;
    size_t nLen;

    if (0 == _dupenv_s(&pBuff, &nLen, "USERNAME"))
    {
        GString sName(pBuff);
        free(pBuff);

        return sName;
    }

    free(pBuff);
    return GString("");
#else
    return GString(getenv("USER"));
#endif
}

GString getHostName()
{
    char szAddr[255] = { '\0' };

#ifdef Q_OS_WIN
    unsigned long uSize = 255;
    GetComputerNameA(szAddr, &uSize);
#else
    gethostname(szAddr, sizeof(szAddr));
#endif

    return GString(szAddr);
}

unsigned getCoreCount()
{
    unsigned uCount = 1; // 至少一个

#ifdef Q_OS_WIN
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    uCount = si.dwNumberOfProcessors;
#else
    uCount = sysconf(_SC_NPROCESSORS_CONF);
#endif

    return uCount;
}

/****************************************************************************************************
  作者： ruanlz 2015-07-14
  函数名称：getCPUIdString
  返回： GString
  功能： 获取cpuID
  备注： 使用__cpuid函数获取cpuid,对于不支持__cpuid函数的win32下的编译器使用了内联汇编实现__cpuid函数
        在windows下使用wmic命令做了验证，非windows平台在ubuntu 12.04.01和maxos 10.9上做了验证但因为
        是在虚拟机上运行，硬件信息不能验证,所以准确性不能定论。
  注意： cpu序列号不唯一，相同型号的cpu的cpu序列号可能相同
  处理器：Intel 486以上和AMD am486以上的X86架构的处理器，不支持除X86以外架构的处理器
****************************************************************************************************/

#if defined(_WIN64)
// 64位下不支持内联汇编. 应使用__cpuid、__cpuidex等Intrinsics函数。
#elif defined(WIN32)
#if _MSC_VER < 1600    // VS2010. 据说VC2008 SP1之后才支持__cpuidex
void __cpuidex(INT32 CPUInfo[4], INT32 InfoType, INT32 ECXValue)
{
    if (NULL==CPUInfo)    return;
    _asm{
        // load. 读取参数到寄存器
        mov edi, CPUInfo;    // 准备用edi寻址CPUInfo
        mov eax, InfoType;
        mov ecx, ECXValue;
        // CPUID
        cpuid;
        // save. 将寄存器保存到CPUInfo
        mov    [edi], eax;
        mov    [edi+4], ebx;
        mov    [edi+8], ecx;
        mov    [edi+12], edx;
    }
}
#endif    // #if _MSC_VER < 1600    // VS2010. 据说VC2008 SP1之后才支持__cpuidex

#if _MSC_VER < 1400    // VC2005才支持__cpuid
void __cpuid(INT32 CPUInfo[4], INT32 InfoType)
{
    __cpuidex(CPUInfo, InfoType, 0);
}
#endif    // #if _MSC_VER < 1400    // VC2005才支持__cpuid

#endif    // #if defined(_WIN64)

GString getCPUIdString()
{
#ifdef Q_OS_WIN
    GString sCpuid;//存储cpuid的字符串
    INT32 nCpuid[4];//存储寄存器数据

    //获取cpuid
    __cpuid(nCpuid,0x01);

    //把结果格式化进入字符串
    char pbufer[17];
    sprintf_s(pbufer,17,"%.8X%.8X",nCpuid[3],nCpuid[0]);//格式化字符串
    sCpuid = GString(GLatin1String(pbufer));//将char转换成GString
    return sCpuid;
#else
    /*在ubuntu 12.04.1上测试可以获取到cpuid号，但因为在虚拟机上运行的Linux，硬件是虚拟的，不能保证绝对正确*/
//#define cpuid(in,a,b,c,d) asm("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(in));
    //定义AT&T风格的汇编代码，用来完成cpuid命令。
    int eax,ebx,ecx,edx;//存储寄存器的内容
    GString sCpuid;//存储cpuid的字符串
    __cpuid(1,eax,ebx,ecx,edx);
    char pbufer[17];
    sprintf(pbufer, "%.8X%.8X",edx,eax);//格式化字符串
    sCpuid = GString(GLatin1String(pbufer));//将char转换成GString
    return sCpuid;
#endif
}

/****************************************************************************************************
  作者： ruanlz 2015-07-14
  函数名称：getDiskSerialNoString
  返回： GString
  功能： 获取系统的第一块硬盘的序列号，并不一定是c盘或者系统盘所在的硬盘
  注意： 获取硬盘id时，程序必须有管理员权限才能获取，否则返回空
  操作系统：支持windows95及以上版本的windows操作系统，暂不支持其他操作系统。
****************************************************************************************************/
#ifdef Q_OS_WIN
//把WORD数组调整字节序为little-endian，并滤除字符串结尾的空格。
void ToLittleEndian(PUSHORT pWords, int nFirstIndex, int nLastIndex, LPTSTR pBuf)
{
    int index;
    LPTSTR pDest = pBuf;
    for(index = nFirstIndex; index <= nLastIndex; ++index)
    {
        pDest[0] = pWords[index] >> 8;
        pDest[1] = pWords[index] & 0xFF;
        pDest += 2;
    }
    *pDest = 0;

    //trim space at the endof string; 0x20: _T(' ')
    --pDest;
    while(*pDest == 0x20)
    {
        *pDest = 0;
        --pDest;
    }
}

//滤除字符串起始位置的空格
void TrimStart(LPTSTR pBuf)
{
    if(*pBuf != 0x20)
    {
        return;
    }

    LPTSTR pDest = pBuf;
    LPTSTR pSrc = pBuf + 1;
    while(*pSrc == 0x20)
        ++pSrc;

    while(*pSrc)
    {
        *pDest = *pSrc;
        ++pDest;
        ++pSrc;
    }
    *pDest = 0;
}

GString getDiskSerialNoString()
{
    TCHAR  szSerialNo[24];
    BYTE IdentifyResult[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
    DWORD dwBytesReturned;
    GETVERSIONINPARAMS get_version;
    SENDCMDINPARAMS send_cmd = { 0 };

    HANDLE hFile = CreateFile(L"\\\\.\\PHYSICALDRIVE0", GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        return GString("");
    }

    //get version
    DeviceIoControl(hFile, SMART_GET_VERSION, NULL, 0,
                    &get_version, sizeof(get_version), &dwBytesReturned, NULL);
    //identify device

    //send_cmd.irDriveRegs.bCommandReg = (get_version.bIDEDeviceMap & 0x10)? ATAPI_ID_CMD : ID_CMD;
    //说明： 当以ATAPI_ID_CMD的形式去获取硬盘信息时会获取不到，ID_CMD则可以获取，暂时测得电脑上还没有测出错误，
    //      暂时对这个问题没有更好的解决方法，所以暂时先改为都已ID_CMD的命令格式去获取硬盘信息。
    send_cmd.irDriveRegs.bCommandReg = (get_version.bIDEDeviceMap & 0x10)? ID_CMD : ID_CMD;
    DeviceIoControl(hFile, SMART_RCV_DRIVE_DATA, &send_cmd, sizeof(SENDCMDINPARAMS) - 1,
                    IdentifyResult, sizeof(IdentifyResult), &dwBytesReturned, NULL);
    CloseHandle(hFile);

    //获取结果
    PUSHORT pWords = (USHORT*)(((SENDCMDOUTPARAMS*)IdentifyResult)->bBuffer);

    ToLittleEndian(pWords, 10, 19, szSerialNo);
    TrimStart(szSerialNo);
    QString sDiskSerialNo;
    sDiskSerialNo = QString::fromWCharArray(szSerialNo);
    return sDiskSerialNo;
}
#else
//非windows操作系统统一返回空字符串
GString getDiskSerialNoString()
{
    return GString("");
}
#endif

/****************************************************************************************************
  作者： ruanlz 2015-07-14
  函数名称：getComputerGUID
  返回： GString
  功能： 获取电脑唯一标示，返回结果是32位的字符串
  注意： 因为依赖于getDiskSerialNoString函数，所以在没有管理员权限下返回的磁盘序列号可能
        是空的，在有管理员权限和没有管理员权限的情况下，此函数的返回值可能不一样。
****************************************************************************************************/
#ifdef Q_OS_WIN
GString getComputerGUID()
{
    GString sCpuid,sDiskSerialNo;
    GString sComputerGUID;

    //获取cpuid和磁盘序列号
    sCpuid = getCPUIdString();
    sDiskSerialNo = getDiskSerialNoString();
    sComputerGUID = sCpuid + sDiskSerialNo;
    return sComputerGUID;
}
#endif


#ifdef Q_OS_WIN
unsigned getNetworkCardCount()
{
    LANA_ENUM leAdapt;
    NCB ncb;
    memset(&ncb, 0, sizeof(ncb));

    ncb.ncb_command = NCBENUM;
    ncb.ncb_buffer = (unsigned char*)&leAdapt;
    ncb.ncb_length = sizeof(leAdapt);

    if (Netbios(&ncb) == 0)
    {
        return leAdapt.length;
    }

    return 0;
}

#endif

GString getMacString()
{
#ifdef Q_OS_WIN

    //int nCount = getNetworkCardCount();
    NCB ncb;
    ADAPTER_STATUS adapt;

    GLDVector<GString> vecRes;
    memset(&ncb, 0, sizeof(ncb));

    UCHAR uRetCode;
    LANA_ENUM   lenum;
    ncb.ncb_command  =  NCBENUM;
    ncb.ncb_buffer  =  (UCHAR  * ) & lenum;
    ncb.ncb_length  =   sizeof (lenum);
    uRetCode  =  Netbios(  & ncb );

    for (int i = 0; i < lenum.length; ++i)
    {
        memset(  & ncb,  0 ,  sizeof (ncb) );
        ncb.ncb_command  =  NCBRESET;
        ncb.ncb_lana_num  =  lenum.lana[i];
        uRetCode  =  Netbios(&ncb);
        if (NRC_GOODRET != uRetCode)
        {
            continue;
        }
        memset(&ncb, 0 , sizeof(ncb));
        ncb.ncb_command = NCBASTAT;
        ncb.ncb_lana_num = lenum.lana[i];
        strcpy_s((char*)ncb.ncb_callname, sizeof(ncb.ncb_callname), "*");
        ncb.ncb_buffer = (unsigned char*)&adapt;
        ncb.ncb_length = sizeof(adapt);

        char szBuff[128] = { '\0' };

        Netbios(&ncb);
#ifdef _MSC_VER
        sprintf_s(
#else
        sprintf(
#endif
                szBuff, "%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                adapt.adapter_address[0],
                adapt.adapter_address[1],
                adapt.adapter_address[2],
                adapt.adapter_address[3],
                adapt.adapter_address[4],
                adapt.adapter_address[5]);

        vecRes.push_back(GString(szBuff));
    }

    if (vecRes.size() > 0)
    {
        return vecRes[0];
    }
#else
    struct ifreq ifreq;
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) > 0)
    {
         strcpy(ifreq.ifr_name, "");
         if (ioctl(sock, SIOCGIFADDR, &ifreq) > 0)
         {
             char szBuff[128] = { '\0' };
             sprintf(szBuff, "%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                     (unsigned char)ifreq.ifr_ifru.ifru_addr.sa_data[0],
                     (unsigned char)ifreq.ifr_ifru.ifru_addr.sa_data[1],
                     (unsigned char)ifreq.ifr_ifru.ifru_addr.sa_data[2],
                     (unsigned char)ifreq.ifr_ifru.ifru_addr.sa_data[3],
                     (unsigned char)ifreq.ifr_ifru.ifru_addr.sa_data[4],
                     (unsigned char)ifreq.ifr_ifru.ifru_addr.sa_data[5]);

             return GString(szBuff);
         }
    }
#endif

    return GString("");
}

/****************************************************************************************************
  作者： yanyq-a 2013-07-30
  参数： const GString&, const GString&
  返回： GString
  功能： 读取注册表相应键值(对应原GrandFileUtils::GetShellFolderPath)
****************************************************************************************************/
GString valueFromRegistry(const GString &regPath, const GString &regKey)
{
    GSettings oSetting(regPath, GSettings::NativeFormat);
    GString result = oSetting.value(regKey, "").toString();
    if ((result.length() > 0) && !((result.right(1) == "\\") || result.right(1) == "/"))
    {
        result += "/";
    }
    return GDir::fromNativeSeparators(result);
}

GString environmentVariable(const GString &envVAR)
{
    QByteArray vardir = qgetenv(envVAR.toStdString().data());

    if (vardir.size() > 0 )
    {
        return vardir;
    }

    return envVAR;
}

bool setEnvironmentVariable(const GString &name, const GString &value)
{
    // todo
    return false;
    G_UNUSED(name)
    G_UNUSED(value)
}

/****************************************************************************************************
  作者： yanyq-a 2013-07-30
  参数： 无
  返回： GString
  功能： 获取当前用户的文档目录
****************************************************************************************************/
GString getUserDocumentsPath()
{
    return QDir::fromNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).append('/'));
}

/****************************************************************************************************
  作者： yanyq-a 2013-07-30
  参数： 无
  返回： GString
  功能： 获取当前用户的应用程序数据目录
****************************************************************************************************/
GString getUserAppDataPath()
{
#ifdef WIN32
    return getSpecialFolderPath(CSIDL_APPDATA);
#else
    return GString();
#endif
}

/****************************************************************************************************
  作者： yanyq-a 2013-08-09
  参数： 无
  返回： gint64
  功能： 可用物理内存
****************************************************************************************************/
gint64 getAvailPhysMem()
{
#ifdef Q_OS_WIN
    MEMORYSTATUSEX oMemory;
    oMemory.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&oMemory);
    return oMemory.ullAvailPhys;
#else
    FILE *pfd;
    char *pend;
    gint64 nFree = 0;
    char szBuff[256] = { '\0' };

    pfd = fopen("/proc/meminfo", "r");

    while (true)
    {
        memset(szBuff, 0, 256);
        pend = fgets(szBuff, 256, pfd);

        if (NULL == pend) break;
        if (strncmp(szBuff, "MemFree", 7) == 0)
        {
            nFree = atoll(szBuff);
            break;
        }
    }

    return nFree;
#endif
}

/**************************************************************************
  作者： yanyq-a 2013-08-09
  参数： 无
  返回： unsigned long
  功能： 物理内存使用率
**************************************************************************/
unsigned long memoryUsage()
{
#ifdef Q_OS_WIN
    MEMORYSTATUSEX oMemory;
    oMemory.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&oMemory);
    return oMemory.dwMemoryLoad;
#else
    FILE *pfd;
    char *pend;
    gint64 nTotal = 0;
    gint64 nFree = 0;
    char szBuff[256] = { '\0' };

    pfd = fopen("/proc/meminfo", "r");

    while (true)
    {
        memset(szBuff, 0, 256);
        pend = fgets(szBuff, 256, pfd);

        if (NULL == pend) break;
        if (strncmp(szBuff, "MemTotal", 8) == 0)
        {
            nTotal = atoll(szBuff);
        }
        if (strncmp(szBuff, "MemFree", 7) == 0)
        {
            nFree = atoll(szBuff);
        }
    }

    if (nTotal <= 0)
    {
        return 0;
    }
    else
    {
        return ((nTotal - nFree) * 100) / nTotal;
    }
#endif
}

GString getSpecialFolderPath(int CSIDL)
{
#ifdef WIN32
    wchar_t path[MAX_PATH];
    if (SHGetSpecialFolderPath(0, path, CSIDL, FALSE))
        return QDir::fromNativeSeparators(QString::fromWCharArray(path).append('/'));
    else
        return GString();
#else
    return GString();
#endif
}
