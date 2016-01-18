#include "GLDCpuInfoUtils.h"

#include <tchar.h>
#include <Winioctl.h>

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "User32.lib")

namespace GlodonCpuInfo
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
        ulong dwMHz = _MAX_PATH;
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

    ulong getCpuCount()
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
            //switch (ptr->Relationship)
            //{
            //case RelationProcessorCore:
            //    processorCoreCount++;

            //    // A hyperthreaded core supplies more than one logical processor.
            //    logicalProcessorCount += countSetBits(ptr->ProcessorMask);
            //    break;

            //default:
            //    _tprintf("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n");
            //    break;
            //}
            if (RelationProcessorCore == ptr->Relationship)
            {
                processorCoreCount++;

                // A hyperthreaded core supplies more than one logical processor.
                logicalProcessorCount += countSetBits(ptr->ProcessorMask);
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
}


