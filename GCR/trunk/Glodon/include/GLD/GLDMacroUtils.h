
/**********************************************************************
 *
 *  广联达 宏工具单元
 *
 *  设计：Zhangsk 2012.04.17
 *  备注：
 *  审核：
 *  注意：
 *
 *  Copyright (c) 2012-2013 GlodonSoft Corporation
 *
 **********************************************************************/

#ifndef GLDMACROUTILS_H
#define GLDMACROUTILS_H



//字符化操作符。
#define makechar(x) #@x

//把宏参数变为一个字符串。
#define makestring(x) #x

//符号连接操作符。将宏定义的多个形参成一个实际参数名。
#define token_pasting(n) num##n

//#define IID_PPV_ARGS


extern "C++"
{
    template<typename T> void** IID_PPV_ARGS_Helper(T** pp) 
    {
        // make sure everyone derives from IUnknown
        static_cast<IUnknown*>(*pp);

        return reinterpret_cast<void**>(pp);
    }
}

#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)

#define IID_PPV_ARGS_Base(Type, Expr) IID_##Type, \
        reinterpret_cast<void**>(static_cast<Type **>(Expr))

//todo: 辅助宏帮助支持#pragma once的编译器
//#if defined (_MSC_VER) && (_MSC_VER >= 1020) && !defined(__midl)
//#pragma once
//#endif

#endif  //GLDMACROUTILS_H
