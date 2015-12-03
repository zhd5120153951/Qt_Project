/*************************************************************************
*                                                                        *
*   广联达文件类相关公共函数单元 CPP                                          *
*                                                                        *
*   设计：Zhangsk 2012.05.21                                              *
*   备注：                                                                *
*   审核：                                                                *
*                                                                        *
*   Copyright (c) 2012-2013 Glodon Corporation                           *
*                                                                        *
*************************************************************************/

#include "GLDFileUtils.h"
#include <QCoreApplication>
#include <QProcess>
#include <QTemporaryFile>
#include "GLDDir.h"
#include "GLDFile.h"
#include "GLDFileInfo.h"
#include "GLDException.h"
#include "GLDGlobal.h"
#include "GLDSysUtils.h"

#ifdef WIN32
#include <ShlObj.h>
#endif

G_GLODON_BEGIN_NAMESPACE
G_GLODON_END_NAMESPACE

/*************************************************************************
  作者： yanyq-a 2013-07-31
  参数： 无
  返回： GString
  功能： 获取exe文件完整名称
*************************************************************************/
GString exeFullName()
{
#ifdef WIN32
    // 不支持长路径??
    wchar_t szModuleName[MAX_PATH + 1] = {0};
    GetModuleFileName(NULL, szModuleName, MAX_PATH + 1);
    GString strPath = GString::fromUtf16((ushort *)szModuleName);
    return strPath;
#else
    return qApp->applicationName();
#endif
}

/*************************************************************************
  说明：返回当前运行EXE文件的路径
  参数：无
  返回：当前运行EXE文件的路径
*************************************************************************/
GString exePath()
{
    return exeDir() + pathDelim(); //获取当前应用程序路径
}

GString exeDir()
{
#ifdef WIN32
    GString strPath = exeFullName();
    int nIndex = strPath.lastIndexOf('\\');
    if (nIndex < 0)
    {
        nIndex = strPath.lastIndexOf('/');
        if (nIndex < 0)
        {
            return GString();
        }
    }
    strPath = strPath.left(nIndex);
    return strPath;
#else
    return QCoreApplication::applicationDirPath(); //获取当前应用程序路径
#endif
}

GString moduleName(void *module)
{
    // todo
    return exeFullName();
    G_UNUSED(module)
}

GString getCurrentDir()
{
    return GDir::currentPath();
}

bool isLibrary()
{
    const GString strModuleName = moduleName(NULL);
    const GString strModuleExt = extractFileExt(strModuleName);

    return GString::compare(strModuleExt, ".dll", Qt::CaseInsensitive) == 0;
}

bool fileExists(const GString &fileName)
{
    GFileInfo fileInfo(fileName);

    if (fileInfo.isDir())
    {
        return false;
    }

    return GFile::exists(fileName);
}

GStream *createFileStream(const GString &fileName)
{
    return new GFileStream(fileName);
}

GString extractFileExt(const GString &fileName)
{
    int nIndex = rPos('.', fileName);
    if (nIndex >= 0 && fileName[nIndex] == '.')
    {
        return copy(fileName, nIndex);
    }
    else
    {
        return GString();
    }
}

GString changeFileExt(const GString &fileName, const GString &extension)
{
    GString sFileName = fileName;
    GFileInfo info(sFileName);
    GString sResult(sFileName);
    sResult = sResult.replace(info.suffix(), extension);
    return sResult;
}

/*************************************************************************
 *作者： zhouyl-a 2013-8-8
 *参数：
 *  1）oldFile : 被拷贝的文件名
 *  2）newFile : 新文件名
 *  3）failIfExists : 操作标识; failIfExists == true 则是如果文件存在就失败；
 *                            failIfExists == false则是如果文件存在就覆盖；
 *返回：true 成功, false 失败
*************************************************************************/
bool copyFile(const GString &oldFile, const GString &newFile, bool failIfExists)
{
    // 如果 failIfExists 为真,且newFile存在,直接返回
    if (failIfExists && fileExists(newFile))
    {
        return false;
    }

    // 如果 failIfExists 为假,且newFile存在
    if (!failIfExists && fileExists(newFile))
    {
        //删除newFile
        GFile::remove(newFile);
    }

    return (GFile::copy(oldFile, newFile));

}

bool deleteFile(const GString &fileName)
{
    if (fileExists(fileName))
    {
        return GFileStream::remove(fileName);
    }
    return false;
}

bool deleteTree(const GString &path)
{
    if (path.isEmpty())
        return false;
    GString strPath = includeTrailingPathDelimiter(path);
    GDir dir(strPath);
    if (!dir.exists())
        return true;

    GFileInfoList fileList = dir.entryInfoList(GDir::AllEntries | GDir::Hidden);
    GString strFileName;
    GString strFile;
    foreach (GFileInfo fileInfo, fileList)
    {
        strFileName = fileInfo.fileName();
        if ((strFileName == GString("..")) || (strFileName == GString(".")))
            continue;
        if (fileInfo.isDir())
        {
            strFile = strPath + strFileName + "/";
            deleteTree(strFile);
        }
        else
        {
            strFile = strPath + strFileName;
            if (!fileInfo.permission(GFileStream::WriteOwner))
            {
                GFileStream file(strFile);
                file.setPermissions(GFileStream::WriteOwner);
            }
            GFileStream::remove(strFile);
        }
    }
    return dir.rmdir(strPath);
}

/*************************************************************************
  作者： jiangjb 2013-06-04
  参数： const GString &, GStrings &
  返回： 无
  功能： 得到所有子目录
*************************************************************************/
void findSubFolders(const GString &path, GStrings &folders)
{
    GDir oDir(path);

    oDir.setFilter(GDir::Dirs | GDir::NoDotAndDotDot);
    folders.clear();
    folders = oDir.entryList();
}

/*************************************************************************
  作者： jiangjb 2013-06-26
  参数： const GString &, const GStrings &, GStrings &
  返回： 无
  功能： 通过后缀名寻找目录下以及当前目录下所有子目录的所有文件
*************************************************************************/
void findFiles(const GString &path, const GStrings &nameFilters, GStrings &files)
{
    GDir oDir(path);
    oDir.setFilter(GDir::Files | GDir::NoDotAndDotDot);

    if (nameFilters.size() == 1)
    {
        if (!nameFilters.at(0).isEmpty())
        {
            oDir.setNameFilters(nameFilters);
        }
    }
    else
    {
        oDir.setNameFilters(nameFilters);
    }

    GStrings oFiles = oDir.entryList();
    for (int i = 0; i < oFiles.size(); i++)
    {
        files.append(includeTrailingPathDelimiter(path) + oFiles.at(i));
    }

    GStrings oSubFileDirs;
    findSubFolders(path, oSubFileDirs);
    for (int i = 0; i < oSubFileDirs.size(); i++)
    {
        findFiles((includeTrailingPathDelimiter(path) + oSubFileDirs.at(i)), nameFilters, files);
    }
}

/*************************************************************************
  作者： jiangjb 2013-06-26
  参数： const GString &, GStrings &, const GString &
  返回： 无
  功能： 通过后缀名寻找目录下以及当前目录下所有子目录的所有文件
*************************************************************************/
void findFiles(const GString &path, const GString &nameFilter, GStrings &files)
{
    GStrings oNameFilters;
    if (nameFilter.isEmpty())
    {
        oNameFilters.append("*.*");

    }
    else
    {
        oNameFilters.append(nameFilter);
    }

    findFiles(path, oNameFilters, files);
}

/*************************************************************************
  作者： jiangjb 2013-06-26
  参数： const GString &, GStrings &, const GString &, bool
  返回： 无
  功能： 通过后缀名寻找当前目录下所有文件
*************************************************************************/
void getFiles(const GString &path, GStrings &files, const GString &nameFilter)
{
    GStrings oNameFilters;
    if (nameFilter.isEmpty())
    {
        oNameFilters.append("*.*");

    }
    else
    {
        oNameFilters.append(nameFilter);
    }

    getFiles(path, oNameFilters, files);
}

void getFiles(const GString &path, const GStrings &nameFilters, GStrings &files, bool includeDir)
{
    GDir oDir(path);
    if (includeDir)
    {
        oDir.setFilter(GDir::Files | GDir::Dirs | GDir::NoDotAndDotDot);
    }
    else
    {
        oDir.setFilter(GDir::Files | GDir::NoDotAndDotDot);
    }

    if (nameFilters.size() == 1)
    {
        if (!nameFilters.at(0).isEmpty())
        {
            oDir.setNameFilters(nameFilters);
        }
    }
    else
    {
        oDir.setNameFilters(nameFilters);
    }

    files.append(oDir.entryList());
}

/*************************************************************************
  作者： yanyq-a 2013-07-31
  参数： GString&
  返回： GString
  功能： 获取操作系统可接受的文件名(对应原GetValidFileName)
*************************************************************************/
GString getValidFileName(const GString &fileName)
{
    GStrings oInvalidChars = (GStrings() << "\\" << "/" << ":" << "\"" << ">" << "<" << "\r\n" << "|" << "?" << "*");
    GString sValidFileName(fileName);
    for (int i = 0; i < oInvalidChars.count(); ++i)
    {
        sValidFileName.replace(oInvalidChars[i], "_", Qt::CaseInsensitive);
    }

    return sValidFileName;
}

bool createDir(const GString &path)
{
    GDir oDir;
    return oDir.mkdir(path);
}

bool forceDirectories(const GString &path)
{
    if (path.isEmpty())
        return false;
    GString strDir = excludeTrailingPathDelimiter(path);
    GDir oDir(strDir);
    if (oDir.exists())
        return true;
    return oDir.mkpath(strDir);
//    GString strParentDir = extractFilePath(strDir);
//    if (sameText(strParentDir, strDir))
//        return true;
//    return forceDirectories(strParentDir) && oDir.mkdir(strDir);
}

bool directoryExists(const GString &path)
{
    if (path.isEmpty())
    {
        return false;
    }
    GDir oDir(path);
    return oDir.exists();
}

GString extractFilePath(const GString &fileName)
{
    int nIndex = pathDelimPos(fileName);
    if (nIndex >= 0)
        return copy(fileName, 0, nIndex + 1);
    else
    {
        return GString();
    }
}

/*************************************************************************
  作者： jiangjb 2013-06-20
  参数： const GString &
  返回： GString
  功能： 从文件名中获取目录名(文件不在根目录下时取得的值后没有“/”，在根目录时一样，都是盘符，例如“C:/”)
*************************************************************************/
GString extractFileDir(const GString &fileName)
{
    int nIndex = pathDelimPos(fileName);
    if (nIndex >= 0)
    {
        if ((nIndex >= 1) && (fileName[nIndex - 1] == driveDelim()))
        {
            ++nIndex;
        }
        return copy(fileName, 0, nIndex);
    }
    else
        return GString();
}

GString extractFileName(const GString &fileName)
{
    int nIndex = pathDelimPos(fileName);
    return copy(fileName, nIndex + 1);
}

GString extractFileNameOnly(const GString &fileName)
{
    GString result = extractFileName(fileName);
    int nDotPos = rPos(GChar('.'), result);
    if (nDotPos >= 0)
        return copy(result, 0, nDotPos);
    else
        return result;
}

// 相对路径转换为绝对路径
GString expandFileName(const GString &fileName)
{
    GFileInfo oFileInfo(fileName);
#ifdef WIN32
    return oFileInfo.absoluteFilePath().replace(GChar('/'), pathDelim());
#else
    return oFileInfo.absoluteFilePath();
#endif
}

/*************************************************************************
  作者： jiangjb 2013-06-14
  参数： const GString &, bool
  返回： 无
  功能： 设置文件只读
*************************************************************************/
void fileSetReadOnly(const GString &fileName, bool readOnly)
{
    if (readOnly)
    {
        GFileStream::setPermissions(fileName, GFileStream::ReadOther);
    }
    else
    {
        GFileStream::setPermissions(fileName, GFileStream::WriteOther | GFileStream::ReadOther);
    }
}

/*************************************************************************

  作者：liudi 2013 原作者：
  参数：const GString &fileName
  返回：gint64
  功能: 返回文件大小

  这个函数原来在TGBQ4ZipFileUtils中
*************************************************************************/
gint64 getFileSize(const GString &fileName)
{
    GFileInfo fileInfo(fileName);

    return fileInfo.size();
}

/*************************************************************************
  作者： zhangjq 2013-09-25
  参数： path：基础路径；productName：当前产品名称；version：当前产品版本
        xxxx\CompanyName\ProductName\ProductVersion
  返回：
  功能： 返回符合公司规范的文件路径
*************************************************************************/
GString getProductPath(const GString &path, const GString &productName, const GString &version)
{
    return GString("%1GrandSoft\\%2\\%3").arg(includeTrailingPathDelimiter(path), productName, version);
}

GString getCommonDocumentsPath()
{
#ifdef Q_OS_WIN
    return getSpecialFolderPath(CSIDL_COMMON_DOCUMENTS);
#else
    return GString("");
#endif
}

GString getTempFile(const GString &prefix, const GString &path)
{
    QTemporaryFile tempFile(includeTrailingPathDelimiter(path) + prefix + "XXXXXX.tmp");
    tempFile.setAutoRemove(false);
    if (tempFile.open())
    {
        GString result = tempFile.fileName();
        tempFile.close();
        return result;
    }
    return GString();
}

int pathDelimPos(const GString &fileName)
{
    int nPathDelimPos = rPos(pathDelim(), fileName);
    int nBackSlashPos = rPos(backSlashDelim(), fileName);
    return (nPathDelimPos > nBackSlashPos ? nPathDelimPos : nBackSlashPos);
}

GString getCommandLineOutput(
        const GString &commandLine, const GString &workDir, int &exitCode)
{
    QProcess process;
    //process.setReadChannel(QProcess::StandardOutput);
    //process.setReadChannelMode(ForwardedChannels);
    process.setWorkingDirectory(workDir);
    process.start(commandLine);
    if (!process.waitForFinished(-1))
    {
        exitCode = -2;
        return GString();
    }
    if (process.exitStatus() == QProcess::NormalExit)
    {
        exitCode = process.exitCode();
        GByteArray array = process.readAllStandardOutput();
        return GString::fromLocal8Bit(array.constData());
    }
    else
    {
        exitCode = -1;
        return GString();
    }
}

GString loadQssFile(const GString &fileName)
{
    GFile oQssFile(fileName);
    if (!oQssFile.open(QIODevice::ReadOnly))
    {
        return QString();
    }
    else
    {
        return oQssFile.readAll();
    }
}
