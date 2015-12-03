/*************************************************************************
*                                                                        *
*   广联达数学运算公共函数单元                                           *
*                                                                        *
*   设计：duanb 2013.4.10                                                *
*   备注：                                                               *
*   审核：                                                               *
*                                                                        *
*   Copyright (c) 2012-2013 Glodon Corporation                           *
*                                                                        *
* ********************************************************************** *
*                                                                        *
*   修改：                                                                *
*                                                                        *
*   20XX.05.01 - XXX                                                     *
*                                                                        *
*     1、XXXXXX                                                          *
*                                                                        *
*                                                                        *
*************************************************************************/

#ifndef GLDMATHUTILS_H
#define GLDMATHUTILS_H
#include <math.h>
#define PI 3.14159265358979323846

#include "GLDString.h"
#include "GLDVariant.h"
#include "GLDVector.h"
#include "GLDSystem.h"
#include "GLDEnum.h"
#include "GLDGlobal.h"

enum CompareResult
{
    crLessThan    = -1,
    crEqual       = 0,
    crGreaterThan = 1
};

template <class T> T Max(T a, T b)
{
    return (a > b ? a : b);
}

template <class T> T Min(T a, T b)
{
    return (a < b ? a : b);
}

template <class T> T Abs(T a)
{
    return (a > 0 ? a : - a);
}

GLDCOMMONSHARED_EXPORT double ln(double v);
GLDCOMMONSHARED_EXPORT double logN(double x, double y);
GLDCOMMONSHARED_EXPORT double arcSin(double v);
GLDCOMMONSHARED_EXPORT double arcCos(double v);
GLDCOMMONSHARED_EXPORT double arctan(double v);
GLDCOMMONSHARED_EXPORT double sqr(double v);

GLDCOMMONSHARED_EXPORT bool sameFloat(float firstNum, float secondNum);

GLDCOMMONSHARED_EXPORT bool sameValue(double a, double b, double epsilon = 1e-12);
GLDCOMMONSHARED_EXPORT GLDValueRelationship compareValue(double a, double b, double epsilon = 1e-12);
template<typename T>
inline GLDValueRelationship compareDigit(T a, T b)
{ return (a == b) ? gvrEqualsValue : ((a < b) ? gvrLessThanValue : gvrGreaterThanValue); }

GLDCOMMONSHARED_EXPORT long long itrunc(double v);
GLDCOMMONSHARED_EXPORT long long iround(double v);
GLDCOMMONSHARED_EXPORT double fRound(double v, int decimal, double epsilon = 1e-12);
GLDCOMMONSHARED_EXPORT double frac(double v);
GLDCOMMONSHARED_EXPORT double drandom();
GLDCOMMONSHARED_EXPORT int random(int n);

class GlodonMathUtils
{
public:
    static inline long long trunc(double v) { return itrunc(v); }
    static inline long long round(double v) { return iround(v); }
};

#ifdef _MSC_VER
inline double random() { return drandom(); }
#endif

GLDCOMMONSHARED_EXPORT int randomRange(int from, int to);
GLDCOMMONSHARED_EXPORT double radToDeg(double radians);
GLDCOMMONSHARED_EXPORT double degToRad(double degrees);
GLDCOMMONSHARED_EXPORT double power(double x, double y);
GLDCOMMONSHARED_EXPORT bool isZero(double value, double epsilon = 0);

GLDCOMMONSHARED_EXPORT bool varIsNullEx(const GVariant &value);
GLDCOMMONSHARED_EXPORT GVariant varArrayOf(GLDVector<GVariant> &values);
GLDCOMMONSHARED_EXPORT GVariant inc(const GVariant &value);

GLDCOMMONSHARED_EXPORT GString num2CChar(int n);
GLDCOMMONSHARED_EXPORT GString num2ChsNum(double arabic);
GLDCOMMONSHARED_EXPORT GString currToChnNum(double currnum);

GLDCOMMONSHARED_EXPORT GString formatFloat(const GString &format, double value);
/*判断结果是否溢出*/
inline bool isOverflowAddUnsignedInt(unsigned int a, unsigned int b)
{
    return UINT_MAX - a < b;
}

inline bool isOverflowAddInt(int a, int b)
{
    return a >= 0 ? INT_MAX - a < b : INT_MIN - a > b;
}

//inline bool isOverflowMultiplyUnsignedInt(unsigned int a, unsigned int b)
//{
//    return (a == 0 || b == 0) ? false : (UINT_MAX / a < b);
//}

inline bool isOverflowMultiplyInt(int a, int b)
{
    if ((a == 0 || b == 0))
        return false;
    else if (a > 0 && b > 0 )
        return INT_MAX / a < b;
    else if (a < 0 && b < 0)
        return INT_MAX / a > b;
    else if (a > 0 && b < 0)
        return INT_MIN / a > b;
    else
        return INT_MIN / a < b;
}

inline bool isOverflowMultiplyInt64(long long a, long long b)
{
    if ((a == 0 || b == 0))
        return false;
    else if (a > 0 && b > 0 )
        return LLONG_MAX / a < b;
    else if (a < 0 && b < 0)
        return LLONG_MAX / a > b;
    else if (a > 0 && b < 0)
        return LLONG_MIN / a > b;
    else
    {
        if (-1 == a)
        {
            return false;
        }
        else
        {
            return LLONG_MIN / a < b;
        }
    }
}

GLDCOMMONSHARED_EXPORT GRgb turnRgb(GRgb rgb);
inline int unsignedCharToInt(unsigned char top, unsigned char bottom, unsigned char left, unsigned char right)
{
    return top | (bottom << 8) | (left << 16) | (right << 24);
}
#endif // GLDMATHUTILS_H
