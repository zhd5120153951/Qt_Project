/*!
 * \file GLDRichTextEditGlobal.h
 *
 * \author xg
 * \date ���� 2014
 *
 * 
 */
#ifndef __GLDRichTextEditGlobal_H__
#define __GLDRichTextEditGlobal_H__

#include <objbase.h>
#include <assert.h>
#pragma warning(disable:4996)


#include <QDebug>
#include <QString>

#define GLD_TEXT(text) (L##text)

#define GLD_REICH_TEXT(text) QString::fromStdWString(GLD_TEXT(text))

#define GLD_RICH_ERROR_POS(x) qDebug() << QString("%1:%2 %3").arg(__FILE__).arg(__LINE__).arg(x)

// ����ļ��������  
// strExt: Ҫ������չ��(����: ".txt")  
// strAppKey: ExeName��չ����ע����еļ�ֵ(����: "txtfile")  
// ����TRUE: ��ʾ�ѹ�����FALSE: ��ʾδ����  
bool checkFileRelation(const wchar_t *strExt, const wchar_t *strAppKey);
// ע���ļ�����  
// strExe: Ҫ������չ��(����: ".txt")  
// strAppName: Ҫ������Ӧ�ó�����(����: "C:/MyApp/MyApp.exe")  
// strAppKey: ExeName��չ����ע����еļ�ֵ(����: "txtfile")  
// strDefaultIcon: ��չ��ΪstrAppName��ͼ���ļ�(����: "C:/MyApp/MyApp.exe,0")  
// strDescribe: �ļ���������  
void  registerFileRelation(wchar_t *strExt, wchar_t *strAppName, wchar_t *strAppKey, wchar_t *strDefaultIcon, wchar_t *strDescribe);

bool checkInstallOfficeWord();
bool checkInstallWpsWord();


// This function will register a component in the Registry.
// The component calls this function from its DllRegisterServer function.
HRESULT registerServer(HMODULE hModule,
    const CLSID& clsid,
    LPCTSTR szFriendlyName,
    LPCTSTR szVerIndProgID,
    LPCTSTR szProgID);
// This function will unregister a component.  Components
// call this function from their DllUnregisterServer function.
HRESULT unregisterServer(const CLSID& clsid,
    LPCTSTR szVerIndProgID,
    LPCTSTR szProgID);

BOOL dllRegisterServer(LPCWSTR lpszFileName, BOOL bUnregister);

HBITMAP loadImageFromFile(const QString & pszBitmapFile, bool bMonochrome);

#endif/*__GLDRichTextEditGlobal_H__*/
