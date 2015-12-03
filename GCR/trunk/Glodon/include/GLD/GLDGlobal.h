#ifndef GLDGLOBAL_H
#define GLDGLOBAL_H

#include "GLDCommon_Global.h"

#ifdef GDP_QT
#   include <QtGlobal>
#   define G_ASSERT(cond) Q_ASSERT(cond)
#   ifndef assert
#       define assert(cond) Q_ASSERT(cond)
#   endif
#   define G_ASSERT_X(cond, where, what) Q_ASSERT_X(cond, where, what)
#   define G_UNUSED(x) (void)x;
#else
#   define G_ASSERT(cond) Q_ASSERT(cond)
#   define assert(cond) Q_ASSERT(cond)
#   define G_ASSERT_X(cond, where, what) Q_ASSERT_X(cond, where, what)
#   define G_UNUSED(x) (void)x;
#endif

#define freePtr(p) {if (p) {delete p;}}
#define freeAndNil(p) {if (p) {delete p; p = NULL;}}
#define freeAndNilIntf(p) {if (p) {p->Release(); p = NULL;}}
#define freeAndNilArray(p) {if (p) {delete[] p; p = NULL;}}
#define freeAndNilByGFree(p) {if (p) {gFree(p); p = NULL;}}
#define doNothingMacro()

#define MaxInt      0x7FFFFFFF
#define MinInt      0x80000000
#define MaxListSize 0x07FFFFFF
#define MINCHAR     0x80
#define MAXCHAR     0x7f
#define MINSHORT    0x8000
#define MAXSHORT    0x7fff
#define MINLONG     0x80000000
#define MAXLONG     0x7fffffff
#define MINDOUBLE   2.2250738585072014e-308
#define MAXDOUBLE   1.7976931348623158e+308
#define MAXBYTE     0xff
#define MAXWORD     0xffff
#define MAXDWORD    0xffffffff

const long long c_MaxInt64 = 0x7FFFFFFFFFFFFFFF;
const long long c_MinInt64 = 0x8000000000000000;

GLDCOMMONSHARED_EXPORT void *gMalloc(size_t size);
GLDCOMMONSHARED_EXPORT void gFree(void *ptr);
GLDCOMMONSHARED_EXPORT void *gRealloc(void *ptr, size_t size);
GLDCOMMONSHARED_EXPORT void *gMemCopy(void *dest, const void *src, size_t n);
GLDCOMMONSHARED_EXPORT void *gMemSet(void *dest, int c, size_t n);
GLDCOMMONSHARED_EXPORT void gMemMove(void *dest, const void *src, size_t n);

//void *gMallocAligned(size_t size, size_t alignment);
//void *gReallocAligned(void *ptr, size_t size, size_t oldsize, size_t alignment);
//void gFreeAligned(void *ptr);
//void freeAndNil(void **p);

#ifdef TEST_GSP
#include <Windows.h>
#include <QDebug>

extern int g_sumTime;
extern int g_sumCount;
#define TEST_START \
    DWORD startTime = GetTickCount();

#define TEST_END \
    g_sumTime += GetTickCount() - startTime; \
    ++g_sumCount;

#else
#define TEST_START
#define TEST_END
#endif


#endif // GLDGLOBAL_H
