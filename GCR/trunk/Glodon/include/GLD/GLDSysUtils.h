/*************************************************************************
*                                                                        *
*   广联达系统核心公共函数单元 H                                         *
*                                                                        *
*   设计：Zhangsk 2012.05.23                                             *
*   备注：                                                               *
*   审核：                                                               *
*                                                                        *
*   Copyright (c) 2012-2013 Glodon Corporation                           *
*                                                                        *
*************************************************************************/

#ifndef GLDSYSUTILS_H
#define GLDSYSUTILS_H
#include "GLDNameSpace.h"
#include "GLDChar.h"
#include "GLDString.h"
#include "GLDSystem.h"
#include "GLDCommon_Global.h"

G_GLODON_BEGIN_NAMESPACE
//G_SYSUTILS_BEGIN_NAMESPACE

//G_SYSUTILS_END_NAMESPACE
G_GLODON_END_NAMESPACE

GLDCOMMONSHARED_EXPORT GString includeTrailingBackslash(const GString &s);
GLDCOMMONSHARED_EXPORT GString includeTrailingPathDelimiter(const GString &s);
GLDCOMMONSHARED_EXPORT GString excludeTrailingBackslash(const GString &s);
GLDCOMMONSHARED_EXPORT GString excludeTrailingPathDelimiter(const GString &s);
GLDCOMMONSHARED_EXPORT GChar pathDelim();
GLDCOMMONSHARED_EXPORT GChar driveDelim();
GLDCOMMONSHARED_EXPORT GChar backSlashDelim();
GLDCOMMONSHARED_EXPORT bool isPathDelimiter(const GString &s, int index);

GLDCOMMONSHARED_EXPORT double fileDateToDateTime(int fileDate);
GLDCOMMONSHARED_EXPORT int DateTimeToFileDate(double value);

GLDCOMMONSHARED_EXPORT GString getUserNameDef();
GLDCOMMONSHARED_EXPORT GString getHostName();
GLDCOMMONSHARED_EXPORT unsigned getCoreCount();
GLDCOMMONSHARED_EXPORT GString getMacString();
GLDCOMMONSHARED_EXPORT GString getCPUIdString();
GLDCOMMONSHARED_EXPORT GString getDiskSerialNoString();
GLDCOMMONSHARED_EXPORT GString getComputerGUID();
GLDCOMMONSHARED_EXPORT GString valueFromRegistry(const GString &regPath, const GString &regKey);
GLDCOMMONSHARED_EXPORT GString environmentVariable(const GString &name);
GLDCOMMONSHARED_EXPORT bool setEnvironmentVariable(const GString &name, const GString &value);
GLDCOMMONSHARED_EXPORT GString getUserDocumentsPath();
GLDCOMMONSHARED_EXPORT GString getUserAppDataPath();
GLDCOMMONSHARED_EXPORT gint64 getAvailPhysMem();
GLDCOMMONSHARED_EXPORT unsigned long memoryUsage();

GLDCOMMONSHARED_EXPORT GString getSpecialFolderPath(int CSIDL);

#endif // GLDSYSUTILS_H
