#include "GLDGlobal.h"

#ifndef _MSC_VER
#   include <string.h>
#endif

void *gMalloc(size_t size)
{
#ifdef GDP_QT
    return ::malloc(size);
#else
    return ::malloc(size);
#endif
}

void gFree(void *ptr)
{
#ifdef GDP_QT
    ::free(ptr);
#else
    ::free(ptr);
#endif
}

void *gRealloc(void *ptr, size_t size)
{
#ifdef GDP_QT
    return ::realloc(ptr, size);
#else
    return ::realloc(ptr, size);
#endif
}

//void *gMallocAligned(size_t size, size_t alignment)
//{
//#ifdef GDP_QT
//    return qMallocAligned(size, alignment);
//#else
//    return qMallocAligned(size, alignment);
//#endif
//}

//void *gReallocAligned(void *ptr, size_t size, size_t oldsize, size_t alignment)
//{
//#ifdef GDP_QT
//    return qReallocAligned(ptr, size, oldsize, alignment);
//#else
//    return qReallocAligned(ptr, size, oldsize, alignment);
//#endif
//}

//void gFreeAligned(void *ptr)
//{
//#ifdef GDP_QT
//    qFreeAligned(ptr);
//#else
//    qFreeAligned(ptr);
//#endif
//}

//void freeAndNil(void **p)
//{
//    if (*p)
//        delete (*p);
//    *p = nullptr;
//}

void *gMemCopy(void *dest, const void *src, size_t n)
{
#ifdef GDP_QT
    return ::memcpy(dest, src, n);
#else
    return ::memcpy(dest, src, n);
#endif
}

void *gMemSet(void *dest, int c, size_t n)
{
#ifdef GDP_QT
    return memset(dest, c, n);
#else
    return ::memset(dest, c, n);
#endif
}

void gMemMove(void *dest, const void *src, size_t n)
{
#ifdef GDP_QT
    ::memmove(dest, src, n);
#else
    ::memmove(dest, src, n);
#endif
}

#ifdef TEST_GSP
int g_sumTime = 0;
int g_sumCount = 0;
#endif
