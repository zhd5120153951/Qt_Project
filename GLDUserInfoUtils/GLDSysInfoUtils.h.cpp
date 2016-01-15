#include "GLDSysInfoUtils.h"

#include <tchar.h>
#include <Winioctl.h>

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "User32.lib")

namespace GLDSysInfo
{
    namespace GLDCpuInfo
    {
        typedef BOOL(WINAPI *LPFN_GLPI)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);

        QString getCpuBrand()
        {
            // get extended ids.
            int cpuInfo[4] = { -1 };
            __cpuid(cpuInfo, 0x80000000);
            unsigned int nExIds = cpuInfo[0];

            // get the information associated with each extended ID.
            char cpuBrandString[0x40] = { 0 };
            for (unsigned int i = 0x80000000; i <= nExIds; ++i)
            {
                __cpuid(cpuInfo, i);

                // interpret CPU brand string and cache information.
                if (i == 0x80000002)
                {
                    memcpy(cpuBrandString, cpuInfo, sizeof(cpuInfo));
                }
                else if (i == 0x80000003)
                {
                    memcpy(cpuBrandString + 16, cpuInfo, sizeof(cpuInfo));
                }
                else if (i == 0x80000004)
                {
                    memcpy(cpuBrandString + 32, cpuInfo, sizeof(cpuInfo));
                }
            }

            return cpuBrandString;
        }

        ulong getCpuSpeed()
        {
            wchar_t Buffer[_MAX_PATH];
            DWORD BufSize = _MAX_PATH;
            DWORD dwMHz = _MAX_PATH;
            HKEY hKey;

            // open the key where the proc speed is hidden:
            long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                0,
                KEY_READ,
                &hKey);

            if (lError != ERROR_SUCCESS)
            {
                // if the key is not found, tell the user why:
                FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    lError,
                    0,
                    Buffer,
                    _MAX_PATH,
                    0);

                wprintf(Buffer);
                return 0;
            }

            // query the key:
            RegQueryValueEx(hKey, L"~MHz", NULL, NULL, (LPBYTE)&dwMHz, &BufSize);

            return dwMHz;
        }

        int getCpuCount()
        {
            SYSTEM_INFO systemInfo;
            GetSystemInfo(&systemInfo);
            return systemInfo.dwNumberOfProcessors;
        }

        DWORD countSetBits(ULONG_PTR bitMask)
        {
            DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
            DWORD bitSetCount = 0;
            ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
            DWORD i;

            for (i = 0; i <= LSHIFT; ++i)
            {
                bitSetCount += ((bitMask & bitTest) ? 1 : 0);
                bitTest /= 2;
            }

            return bitSetCount;
        }

        CoreCount getCpuCoreCount()
        {
            LPFN_GLPI glpi;
            BOOL done = FALSE;
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
            DWORD returnLength = 0;
            DWORD logicalProcessorCount = 0;
            DWORD processorCoreCount = 0;
            DWORD byteOffset = 0;

            glpi = (LPFN_GLPI)GetProcAddress(GetModuleHandle(TEXT("kernel32")),
                    "GetLogicalProcessorInformation");

            if (NULL == glpi)
            {
                _tprintf("\nGetLogicalProcessorInformation is not supported.\n");
            }

            while (!done)
            {
                DWORD rc = glpi(buffer, &returnLength);

                if (FALSE == rc)
                {
                    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                    {
                        if (buffer)
                        {
                            free(buffer);
                        }

                        buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);

                        if (NULL == buffer)
                        {
                            _tprintf("\nError: Allocation failure\n");
                        }
                    }
                    else
                    {
                        _tprintf("\nError %d\n", GetLastError());
                    }
                }
                else
                {
                    done = TRUE;
                }
            }

            ptr = buffer;

            while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
            {
                switch (ptr->Relationship)
                {
                case RelationProcessorCore:
                    processorCoreCount++;

                    // A hyperthreaded core supplies more than one logical processor.
                    logicalProcessorCount += countSetBits(ptr->ProcessorMask);
                    break;

                default:
                    _tprintf("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n");
                    break;
                }

                byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
                ptr++;
            }

            CoreCount coreCount;
            coreCount.m_cpuCoreCount = processorCoreCount;
            coreCount.m_cpuLogicalCoreCount = logicalProcessorCount;

            return coreCount;
        }

        CpuInfo getCpuInfo()
        {
            CpuInfo cpuInfo;
            cpuInfo.m_cpuBrand = getCpuBrand();
            cpuInfo.m_processorPackageCount = getCpuCount();
            cpuInfo.m_coreCount = getCpuCoreCount();
            cpuInfo.m_processSpeed = getCpuSpeed();
            return cpuInfo;
        }

        int getCpuUsage()
        {
            return 0;
        }
    }

    namespace GLDDiskInfo
    {
        void toLittleEndian(PUSHORT pWords, int nFirstIndex, int nLastIndex, LPTSTR pBuf)
        {
            LPTSTR pDest = pBuf;
            for(int index = nFirstIndex; index <= nLastIndex; ++index)
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

        void trimStart(LPTSTR pBuf)
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

        QString getDiskSerialNo()
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
                return "";
            }

            //get version
            DeviceIoControl(hFile, SMART_GET_VERSION, NULL, 0,
                            &get_version, sizeof(get_version), &dwBytesReturned, NULL);

            //identify device
            send_cmd.irDriveRegs.bCommandReg = (get_version.bIDEDeviceMap & 0x10)? ID_CMD : ID_CMD;
            DeviceIoControl(hFile, SMART_RCV_DRIVE_DATA, &send_cmd, sizeof(SENDCMDINPARAMS) - 1,
                            IdentifyResult, sizeof(IdentifyResult), &dwBytesReturned, NULL);
            CloseHandle(hFile);

            //获取结果
            PUSHORT pWords = (USHORT*)(((SENDCMDOUTPARAMS*)IdentifyResult)->bBuffer);

            toLittleEndian(pWords, 10, 19, szSerialNo);
            trimStart(szSerialNo);

            return QString::fromWCharArray(szSerialNo);;
        }


        int getAllDiskPartitionInfo(LPDiskInfo* lpDisks)
        {
            HKEY hKEY;
            long lRet;
            lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                L"SYSTEM\\CurrentControlSet\\Services\\Disk\\Enum",
                                0,
                                KEY_READ,
                                &hKEY);

            if (lRet != ERROR_SUCCESS)
            {
                return 0;
            }

            int size = 0;
            DWORD dwType;
            DWORD dwValue;
            DWORD dwBufLen = sizeof(DWORD);
            __try
            {
                lRet = ::RegQueryValueEx(hKEY, L"Count", NULL, &dwType, (BYTE*)&dwValue, &dwBufLen);

                if(lRet != ERROR_SUCCESS)
                {
                    __leave;//失败
                }

                *lpDisks = (LPDiskInfo) malloc (dwValue * sizeof (DiskInfo));

                for (UINT i = 0; i < dwValue; i++)
                {
                    wchar_t szDiskPos[128] = {0};
                    wsprintf(szDiskPos, L"\\\\.\\PHYSICALDRIVE%u", i);

                    HANDLE   hDevice = NULL;
                    DWORD    nDiskBytesRead = 0;//预设为0，当缓冲区的长度不够时，该值为所需的缓冲区的长度
                    DWORD    nDiskBufferSize = sizeof(DRIVE_LAYOUT_INFORMATION) + sizeof(PARTITION_INFORMATION) * 104;//26*4
                    PDRIVE_LAYOUT_INFORMATION lpDiskPartInfo = (PDRIVE_LAYOUT_INFORMATION)malloc(nDiskBufferSize);

                    if(lpDiskPartInfo == NULL)
                    {
                        free(lpDiskPartInfo);
                        continue;
                    }

                    //将缓冲区lpDiskPartInfo的内容设为nDiskBufferSize个NULL
                    memset(lpDiskPartInfo, 0, nDiskBufferSize);

                    //////////////////////获得所有分区的信息///////////////////////////////////////
                    hDevice = CreateFile(szDiskPos,
                                         GENERIC_READ,
                                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                                         NULL,
                                         OPEN_EXISTING,
                                         0,
                                         NULL);

                    if(hDevice == NULL)
                    {
                        free(lpDiskPartInfo);
                        continue;
                    }

                    /////////////获得某磁盘上的所有分区信息/////////////////////////
                    BOOL fRet = DeviceIoControl(
                                hDevice,
                                IOCTL_DISK_GET_DRIVE_LAYOUT,
                                NULL,
                                0,
                                (LPVOID) lpDiskPartInfo,
                                (DWORD) nDiskBufferSize,
                                (LPDWORD) &nDiskBytesRead,
                                NULL
                                );

                    if (!fRet)
                    {
                        LPVOID lpMsgBuf;
                        FormatMessage(
                                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                    NULL,
                                    GetLastError(),
                                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                    (LPTSTR) &lpMsgBuf,
                                    0,
                                    NULL
                                    );
                        LocalFree(lpMsgBuf);
                        free(lpDiskPartInfo);
                        CloseHandle(hDevice);
                        continue;
                    }

                    //////////////////////////////导出分区信息///////////////////////////////////////
                    DWORD dwPartitionCount = lpDiskPartInfo->PartitionCount;
                    int iPartitions = dwPartitionCount / 4;
                    (*lpDisks)[size].pPartitions = (PPARTITION_INFORMATION)malloc(iPartitions * sizeof (PARTITION_INFORMATION));
                    (*lpDisks)[size].iPartitionSize = 0;

                    //永远是实际的分区数的4倍,不能用的分区将会显示类型PARTITION_ENTRY_UNUSED,即分区类型为0
                    ///////////////////依次获取导出某分区信息,并与目的驱动器进行比较///////////////////////////////////
                    for (UINT j = 0; j < dwPartitionCount; j += 4)//+4是因为只有下标为4的整数倍的值才是正确的引用
                    {
                        memcpy (&((*lpDisks)[size].pPartitions [(*lpDisks)[size].iPartitionSize++]), &lpDiskPartInfo->PartitionEntry [j], sizeof (PARTITION_INFORMATION));
                    }
                    free(lpDiskPartInfo);
                    CloseHandle(hDevice);
                    ++size;
                }
            }
            __finally
            {
                if (hKEY != NULL)
                {
                    RegCloseKey(hKEY);
                }
            }

            return size;
        }

        DiskSpaceInfo getDiskFreeSpace(QString driver)
        {
             LPCWSTR lpcwstrDriver=(LPCWSTR)driver.utf16();

             ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;

             if(GetDiskFreeSpaceEx(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes) )
             {
                  DiskSpaceInfo diskSpaceInfo;
                  diskSpaceInfo.m_availSpace = (quint64) liTotalFreeBytes.QuadPart / 1024 / 1024 / 1024;
                  return diskSpaceInfo;
             }
        }
    }

}


