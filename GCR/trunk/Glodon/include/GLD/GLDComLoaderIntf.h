/**********************************************************************
 *
 *  广联达 COM加载器接口定义
 *
 *  设计：Zhangsk 2012.05.25
 *  备注：
 *  审核：
 *  注意：
 *
 *  Copyright (c) 2012-2013 GlodonSoft Corporation
 *
 **********************************************************************/

#ifndef GLDCOMLOADERINTF_H
#define GLDCOMLOADERINTF_H

#include "GLDUnknwn.h"
//#include "GLDObject.h"

DEFINE_IID(IComLoader, "{A4CD87EA-E1DE-4588-A2E0-277C3AE2B536}");
DECLARE_INTERFACE_(IComLoader, IUnknown)
{
    /*! 安装一个应用的类厂
        \param pszName        对象的名称，建议方式：com.grand.gsp
        \param pszVersion   对象的版本
        \param pFactory     类厂对象
        \return S_OK S_FALSE
    */
    STDMETHOD(InstallApplication)( wchar_t *pszName , wchar_t *pszVersion, IUnknown* pFactory ) PURE;
};

#endif // GLDCOMLOADERINTF_H
