#include "GLDDiskInfoUtils.h"

namespace GLDDISKINFO
{
    QString GLDDiskInfo::getDiskSerialNo()
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

    QVector<DiskInfomation> GLDDiskInfo::getAllDriversInfo()
    {
        QVector<DiskInfomation> diskInfoVect;
        QVector<QString> drvNameVct;
        qulonglong dwDrvNum = getDriverNum();

        if (getAllDriverName(dwDrvNum, drvNameVct))
        {
            QVector<QString>::const_iterator iter = drvNameVct.begin();
            for (; iter != drvNameVct.end(); ++iter)
            {
                //deal every driver
                DiskInfomation dskinf;
                //add disk name
                dskinf.m_strDiskName = *iter;
                //add disk type
                dskinf.m_strTypeName = getDriverTypeItem(*iter);
                //add filesystem type
                switch (getFileSystemType(*iter))
                {
                case FAT32:
                    dskinf.m_strFileSystem = "FAT32";
                    break;
                case NTFS:
                    dskinf.m_strFileSystem = "NTFS";
                    break;
                default:
                    dskinf.m_strFileSystem = "OTHER_FORMAT";
                    break;
                }
                //add space information
                if (dskinf.m_strTypeName == QStringLiteral("磁盘分区") && dskinf.m_strFileSystem != "OTHER_FORMAT")
                {
                    qint64 i64TotalBytes = 0;
                    qint64 i64FreeBytes = 0;

                    if (getFreeSpace(*iter, i64FreeBytes, i64TotalBytes))
                    {
                        dskinf.m_dwFreeMBytes = DWORD(i64FreeBytes / 1024 / 1024);
                        dskinf.m_dwTotalMBytes = DWORD(i64TotalBytes / 1024 / 1024);
                    }
                    else
                    {
                        dskinf.m_dwFreeMBytes = 0;
                        dskinf.m_dwTotalMBytes = 0;
                    }
                    //make vector data
                    diskInfoVect.push_back(dskinf);
                }
            }
        }

        return diskInfoVect;
    }

    ulong GLDDiskInfo::getDriverNum()
    {
        ulong diskCount = 0;
        DWORD DiskInfo = GetLogicalDrives();
        //利用GetLogicalDrives()函数可以获取系统中逻辑驱动器的数量，函数返回的是一个32位无符号整型数据。
        while(DiskInfo)//通过循环操作查看每一位数据是否为1，如果为1则磁盘为真,如果为0则磁盘不存在。
        {
            if(DiskInfo & 1)//通过位运算的逻辑与操作，判断是否为1
            {
                ++diskCount;
            }

            DiskInfo = DiskInfo >> 1;//通过位运算的右移操作保证每循环一次所检查的位置向右移动一位。
        }

        return diskCount;
    }

    //n个driver，以A：\null的形式存放的话，需4n个字符的数组，猜想，实际获得4n＋1个字符的数组，可见末尾是以nullnull结束字符数组
    bool GLDDiskInfo::getAllDriverName(ulong dwDrvNum, QVector<QString> & driveNameVct)
    {
        //通过GetLogicalDriveStrings()函数获取所有驱动器字符串信息长度。
        DWORD dwLength = GetLogicalDriveStringsA(0,NULL);

        //用获取的长度在堆区创建一个c风格的字符串数组
        char* DStr = new char[dwLength];
        dwLength = GetLogicalDriveStringsA(dwLength, (LPSTR)DStr);
        if (!dwLength)
        {
            delete [] DStr;
            return false;
        }

        DWORD dwIndex = 0;

        while (dwIndex < dwDrvNum)
        {
            QString tmp(DStr + 4 * dwIndex);
            driveNameVct.push_back(tmp);
            dwIndex++;
        }

        delete [] DStr;
        return true;
    }

    void GLDDiskInfo::toLittleEndian(PUSHORT pWords, int nFirstIndex, int nLastIndex, LPTSTR pBuf)
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

    void GLDDiskInfo::trimStart(LPTSTR pBuf)
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

    QString GLDDiskInfo::getDriverTypeItem(const QString& dir)
    {
        UINT uiType = GetDriveTypeA(dir.toStdString().c_str());
        switch (uiType)
        {
        case DRIVE_UNKNOWN:
            return QStringLiteral("未知设备");
        case DRIVE_REMOVABLE:
            return QStringLiteral("可移动设备");
        case DRIVE_FIXED:
            return QStringLiteral("磁盘分区");
        case DRIVE_REMOTE:
            return QStringLiteral("网络磁盘");
        case DRIVE_CDROM:
            return QStringLiteral("光驱");
        case DRIVE_RAMDISK:
            return QStringLiteral("虚拟磁盘");
        default:
            return QStringLiteral("无效路径");
        }
    }

    FS GLDDiskInfo::getFileSystemType(const QString &dir)
    {
        //对于光驱信息的需排除之
        QString temp = dir[0];
        temp += ":";
        temp += "\\";

        char Volumelabel[255];
        DWORD SerialNumber;
        DWORD MaxCLength;
        DWORD FileSysFlag;
        char FileSysName[10];
        GetVolumeInformationA(temp.toStdString().c_str(), Volumelabel, 255, &SerialNumber,
                              &MaxCLength, &FileSysFlag, FileSysName,255);

        if (strcmp(FileSysName, "NTFS") == 0 )
        {
            return NTFS;
        }
        else if (strcmp(FileSysName, "FAT32") == 0 )
        {
            return FAT32;
        }
        else
        {
            return OTHER_FORMAT;
        }
    }

    bool GLDDiskInfo::getFreeSpace(const QString& dir, qint64& ri64FreeBytesToCaller, qint64& ri64TotalBytes)
    {
        typedef bool (WINAPI *PGETDISKFREESPACEEX)(LPCSTR,
            PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

        PGETDISKFREESPACEEX pGetDiskFreeSpaceEx = NULL;
        qint64 i64FreeBytes;

        bool fResult = false;

        pGetDiskFreeSpaceEx = (PGETDISKFREESPACEEX) GetProcAddress(
            GetModuleHandleA("kernel32.dll"),
            "GetDiskFreeSpaceExA");

        if (pGetDiskFreeSpaceEx)
        {
            fResult = pGetDiskFreeSpaceEx (dir.toStdString().c_str(), (PULARGE_INTEGER)&ri64FreeBytesToCaller,
                (PULARGE_INTEGER)&ri64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes);

            return fResult;
        }

        return fResult;
    }

    bool GLDDiskInfo::getLastVolumeInfo(DiskInfomation &diskInfo)
    {
        QHash<qint64, QString> mapVolumeByStartAddress;
        QVector<DiskInfomation> diskInfoVec = getAllDriversInfo();
        QVector<DiskInfomation>::const_iterator cit = diskInfoVec.begin();
        for (; cit != diskInfoVec.end(); ++cit)
        {
            //such as "\\.\C"
            QString volName = cit->m_strDiskName;

            volName = volName[0];
            volName += ":";
            volName = "\\\\.\\" + volName;

            HANDLE hVolum = CreateFileA(volName.toStdString().c_str(), GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE , NULL, OPEN_EXISTING,
                0, NULL);
            if (INVALID_HANDLE_VALUE == hVolum)
            {
                return false;
            }

            bool bIsOk;
            DWORD dwReturnBytes = 0;
            VOLUME_DISK_EXTENTS vde;

            bIsOk = DeviceIoControl(hVolum, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS , NULL, 0,
                &vde, sizeof(vde), &dwReturnBytes, NULL);
            if (bIsOk)
            {
                mapVolumeByStartAddress.insert(vde.Extents[0].StartingOffset.QuadPart, cit->m_strDiskName);
            }
            else if (GetLastError() == ERROR_MORE_DATA)
            {
                mapVolumeByStartAddress.insert(vde.Extents[0].StartingOffset.QuadPart, cit->m_strDiskName);
            }
            else
            {
                if (hVolum != INVALID_HANDLE_VALUE)
                {
                    CloseHandle(hVolum);
                }

                return false;
            }

            if (hVolum != INVALID_HANDLE_VALUE)
            {
                CloseHandle(hVolum);
            }
        }

        for (cit = diskInfoVec.begin(); cit != diskInfoVec.end(); ++cit)
        {
            if (mapVolumeByStartAddress.values().contains(cit->m_strDiskName))
            {
                diskInfo = *cit;
            }
        }

        return true;
    }

    bool GLDDiskInfo::getDiskSize(quint64 &llOfSectors, ulong dwDiskNum/* = */)
    {
        string strDiskName = "\\\\.\\PHYSICALDRIVE";
        char szindx[2] = {'\0', '\0'};
        _itoa_s(dwDiskNum, szindx, 2, 10);
        strDiskName += szindx;

        HANDLE hDisk = CreateFileA(strDiskName.c_str(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, NULL, NULL);
        if (INVALID_HANDLE_VALUE == hDisk)
        {
            GetLastError();
            return false;
        }

        bool bIsOk;
        DWORD dwReturnBytes = 0;
        GET_LENGTH_INFORMATION gli;

        bIsOk = DeviceIoControl(hDisk, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &gli, sizeof(gli), &dwReturnBytes, NULL);
        if (!bIsOk)
        {
            GetLastError();
            if (INVALID_HANDLE_VALUE != hDisk)
            {
                CloseHandle(hDisk);
            }
            return false;
        }

        llOfSectors = gli.Length.QuadPart / 512;

        if (INVALID_HANDLE_VALUE != hDisk)
        {
            CloseHandle(hDisk);
        }
        return true;
    }

    bool GLDDiskInfo::getDiskSize2(quint64 &llOfSectors, ulong dwDiskNum /* = 0 */)
    {
        string strDiskName = "\\\\.\\PHYSICALDRIVE";
        char szindx[2] = {'\0', '\0'};
        _itoa_s(dwDiskNum, szindx, 2, 10);
        strDiskName += szindx;

        HANDLE hDisk = CreateFileA(strDiskName.c_str(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, NULL, NULL);
        if (INVALID_HANDLE_VALUE == hDisk)
        {
            GetLastError();
            return false;
        }

        bool bIsOk;
        DWORD dwReturnBytes = 0;
        DISK_GEOMETRY_EX dge;

        bIsOk = DeviceIoControl(hDisk, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, &dge, sizeof(dge), &dwReturnBytes, NULL);
        if (!bIsOk)
        {
            if (GetLastError() != ERROR_MORE_DATA)
            {
                if (INVALID_HANDLE_VALUE != hDisk)
                {
                    CloseHandle(hDisk);
                }
                return false;
            }
        }

        llOfSectors = dge.DiskSize.QuadPart / 512;

        if (INVALID_HANDLE_VALUE != hDisk)
        {
            CloseHandle(hDisk);
        }
        return true;
    }

    bool GLDDiskInfo::getVolumeSize(quint64 &llOfSectors, string volName)
    {
        char szchar[2] = {volName[0], '\0'};
        string strVolumeName = szchar;
        strVolumeName = "\\\\.\\" + strVolumeName;
        strVolumeName += ":";

        HANDLE hVolume = CreateFileA(strVolumeName.c_str(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, NULL, NULL);
        if (INVALID_HANDLE_VALUE == hVolume)
        {
            GetLastError();
            return false;
        }

        bool bIsOk;
        DWORD dwReturnBytes = 0;
        GET_LENGTH_INFORMATION gli;

        bIsOk = DeviceIoControl(hVolume, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &gli, sizeof(gli),&dwReturnBytes, NULL);
        if (!bIsOk)
        {
            GetLastError();
            if (INVALID_HANDLE_VALUE != hVolume)
            {
                CloseHandle(hVolume);
            }
            return false;
        }

        llOfSectors = gli.Length.QuadPart / 512;

        if (INVALID_HANDLE_VALUE != hVolume)
        {
            CloseHandle(hVolume);
        }
        return true;
    }
}


