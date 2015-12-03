/*************************************************************************
*                                                                        *
*   广联达文件类相关公共函数单元 H                                       *
*                                                                        *
*   设计：Zhangsk 2012.05.21                                             *
*   备注：                                                               *
*   审核：                                                               *
*                                                                        *
*   Copyright (c) 2012-2013 Glodon Corporation                           *
*                                                                        *
* ********************************************************************** *
*                                                                        *
*   修改：                                                               *
*                                                                        *
*   20XX.05.01 - XXX                                                     *
*                                                                        *
*     1、XXXXXX                *
*                                                                        *
*                                                                        *
*************************************************************************/

#ifndef GLDFILEUTILS_H
#define GLDFILEUTILS_H

#include "GLDString.h"
#include "GLDStrUtils.h"
#include "GLDNameSpace.h"

#include "GLDWidget_Global.h"

G_GLODON_BEGIN_NAMESPACE
G_GLODON_END_NAMESPACE

GLDCOMMONSHARED_EXPORT GString exeFullName();
GLDCOMMONSHARED_EXPORT GString exePath();
GLDCOMMONSHARED_EXPORT GString exeDir();

GLDCOMMONSHARED_EXPORT GString moduleName(void *module);

GLDCOMMONSHARED_EXPORT GString getCurrentDir();
GLDCOMMONSHARED_EXPORT GString getProductPath(const GString &path, const GString &productName, const GString &version);

GLDCOMMONSHARED_EXPORT GString getCommonDocumentsPath();

GLDCOMMONSHARED_EXPORT GString getTempFile(const GString &prefix, const GString &path);

GLDCOMMONSHARED_EXPORT bool isLibrary();

//实现Win32平台函数BOOL CopyFile(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,BOOL bFailIfExists );
GLDCOMMONSHARED_EXPORT bool copyFile(const GString &oldFile, const GString &newFile, bool failIfExists);

GLDCOMMONSHARED_EXPORT bool deleteFile(const GString &fileName);
GLDCOMMONSHARED_EXPORT bool deleteTree(const GString &path);

GLDCOMMONSHARED_EXPORT bool fileExists(const GString &fileName);
GLDCOMMONSHARED_EXPORT GStream *createFileStream(const GString &fileName);
GLDCOMMONSHARED_EXPORT GString extractFileExt(const GString &fileName);
GLDCOMMONSHARED_EXPORT GString changeFileExt(const GString &fileName, const GString &extension);

GLDCOMMONSHARED_EXPORT void findSubFolders(const GString &path, GStrings &folders);
GLDCOMMONSHARED_EXPORT void findFiles(const GString &path, const GStrings &nameFilters, GStrings &files);
GLDCOMMONSHARED_EXPORT void findFiles(const GString &path, const GString &nameFilter, GStrings &files);
GLDCOMMONSHARED_EXPORT void getFiles(const GString &path, GStrings &files, const GString &nameFilter = "");
GLDCOMMONSHARED_EXPORT void getFiles(const GString &path, const GStrings &nameFilters, GStrings &files, bool includeDir = false);
GLDCOMMONSHARED_EXPORT GString getValidFileName(const GString &fileName);

GLDCOMMONSHARED_EXPORT bool createDir(const GString &path);
GLDCOMMONSHARED_EXPORT bool forceDirectories(const GString &path);
GLDCOMMONSHARED_EXPORT bool directoryExists(const GString &path);

GLDCOMMONSHARED_EXPORT GString extractFilePath(const GString &fileName);
GLDCOMMONSHARED_EXPORT GString extractFileDir(const GString &fileName);
GLDCOMMONSHARED_EXPORT GString extractFileName(const GString &fileName);
GLDCOMMONSHARED_EXPORT GString extractFileNameOnly(const GString &fileName);
GLDCOMMONSHARED_EXPORT GString expandFileName(const GString &fileName);
GLDCOMMONSHARED_EXPORT int pathDelimPos(const GString &fileName);
GLDCOMMONSHARED_EXPORT void fileSetReadOnly(const GString &fileName, bool readOnly = true);

GLDCOMMONSHARED_EXPORT gint64 getFileSize(const GString &fileName);

// 取得运行命令行的输出:默认的codepage为Local8Bit(中文操作系统为936)
GLDCOMMONSHARED_EXPORT GString getCommandLineOutput(
        const GString &commandLine, const GString &workDir, int &exitCode);

GLDCOMMONSHARED_EXPORT GString loadQssFile(const GString &fileName);
#endif // GLDFILEUTILS_H
