#ifndef GLDDISKINFOUTILS_H
#define GLDDISKINFOUTILS_H

#include <qt_windows.h>
#include <bitset>
#include <iostream>
#include <algorithm>
#include <utility>
#include <Winioctl.h>
#include <string>
#include <map>
#include <QVector>
#include <QString>
#include <QHash>

using namespace std;

namespace GLDDISKINFO
{
    enum FS
    {
        OTHER_FORMAT = 0,
        FAT32        = 1,
        NTFS         = 2
    };

    typedef struct SDiskInfomation
    {
        SDiskInfomation()
        {
            m_dwFreeMBytes = 0;
            m_dwTotalMBytes = 0;
        }

        QString m_strDiskName;   // 分区名(盘符)
        QString m_strTypeName;   // 分区类型
        QString m_strFileSystem; // 分区格式
        ulong   m_dwTotalMBytes; // 总空间
        ulong   m_dwFreeMBytes;  // 可用空间

    }DiskInfomation;

    class GLDDiskInfo
    {
    public:
        /**
         * @brief 获取硬盘序列号
         * @return
         */
        QString getDiskSerialNo();

        /**
         * @brief 获取分区个数
         * @return
         */
        ulong getDriverNum();

        /**
         * @brief 获取所有分区信息
         * @return
         */
        QVector<DiskInfomation> getAllDriversInfo();

        /**
         * @brief 获取分区格式
         * @param dir  分区名
         * @return
         */
        FS getFileSystemType(const QString& dir);

        /**
         * @brief 获取分区类型
         * @param dir   分区名
         * @return
         */
        QString getDriverTypeItem(const QString& dir);

        bool getLastVolumeInfo(DiskInfomation &diskInfo);
        bool getDiskSize(quint64 &llOfSectors, ulong dwDiskNum = 0);
        bool getDiskSize2(quint64 &llOfSectors, ulong dwDiskNum = 0);
        bool getVolumeSize(quint64 &llOfSectors, string volName);
        bool getFreeSpace(const QString& dir, qint64& ri64FreeBytesToCaller, qint64& ri64TotalBytes);

    private:
        bool getAllDriverName(ulong dwDrvNum, QVector<QString> & driveNameVct);
        void toLittleEndian(PUSHORT pWords, int nFirstIndex, int nLastIndex, LPTSTR pBuf);
        void trimStart(LPTSTR pBuf);
    };
}

#endif // GLDDISKINFOUTILS_H
