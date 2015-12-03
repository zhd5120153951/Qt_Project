#include "GLDMathUtils.h"

#ifdef WIN32
#include "WTypes.h"
#endif

#include <time.h>
#include <qmath.h>

#include "GLDGlobal.h"
#include "GLDStrUtils.h"
#include "GLDFloatFormating.h"

//const int CFuzzFactor = 1000;
//const int CExtendedResolution = 1e-19 * CFuzzFactor;
//const int CDoubleResolution   = 1e-15 * CFuzzFactor;
//const int CSingleResolution   = 1e-7 * CFuzzFactor;

const double c_defDoubleEpsilon = 1e-12;

double ln(double v)
{
    assert(v > 0);
    return qLn(v);
}

// Log.N(X) := Log.2(X) / Log.2(N)
double logN(double x, double n)
{
    assert(x > 0);
    assert(n > 0);
    assert(n != 1);
    return ln(x) / ln(n);
}

double arcSin(double v)
{
    return qAsin(v);
}

double arcCos(double v)
{
    return qAcos(v);
}

double arctan(double v)
{
    return qAtan(v);
}

double sqr(double v)
{
    return v * v;
}

bool sameFloat(float firstNum, float secondNum)
{
    return (fabs(firstNum - secondNum) < 0.00001 ? true : false);
}

bool sameValue(double a, double b, double epsilon)
{
    return fabs(a - b) < epsilon;
}

GLDValueRelationship compareValue(double a, double b, double epsilon)
{
    if (sameValue(a, b, epsilon))
    {
        return gvrEqualsValue;
    }
    else if (a < b)
    {
        return gvrLessThanValue;
    }
    else
    {
        return gvrGreaterThanValue;
    }
}

// 去尾取整 trunc(5.6) == 5; trunc(-5.6) == -5
long long itrunc(double v)
{
    return (long long)(v);
}

// 四舍六入五留双 round(2.5) == 2; round(3.5) == 4; round(-2.5) == -2; round(-3.5) == -4;
long long iround(double v)
{
    // todo 浮点数+1
    double dFabsVal = fabs(v);
    long long llTmp = itrunc(dFabsVal);
    if (dFabsVal - llTmp > 0.5)
    {
        ++llTmp;
    }
    else if (dFabsVal - llTmp == 0.5)
    {
        if ((llTmp % 2) != 0)
        {
            ++llTmp;
        }
    }
    if (v < 0)
        return -llTmp;
    else
        return llTmp;
}

////////////////////////////////////////////////////////////////////////////////
// 说明：浮点数取指定位小数精度
// 参数：F -- 浮点数
//       ADecimal -- 小数位数
// 返回：F按ADecimal位小数精度四舍五入后的浮点数
// 注意：FRound( 518.775, 2) = 518.78
//       FRound(-518.775, 2) = -518.78
//       FRound(-518.775, -1) = -520
//       为了避免误差问题，根据 IEEE 的规定直接在 F 尾数上加 1，可能 2 或 4 等
// 备注：添加 ADecimal 为负数时的处理，即整数部分的精度计算； zhangjq 2011.12.13
////////////////////////////////////////////////////////////////////////////////
double fRound(double v, int decimal, double epsilon)
{
    const static double c_DecBase[10] = {1, 1E1, 1E2, 1E3, 1E4, 1E5, 1E6, 1E7, 1E8, 1E9};
    if ((v > c_MaxInt64) || (v < c_MinInt64))
        return v;
    else if (decimal < 0)
    {
        // todo
        return v;
    }
    else if (decimal <= 8)
    {
        //当数值超过10位时，+512带来的误差会比较大，但是能保证数比较小的时候是正确的－by huangp 2015.9.6
        long long *ptmp = (long long *)(&v);
        *ptmp += 512; // 支持512个相同的有误差的数相加后fRound
        long long llIntVal = itrunc(v);
        long long llDecimalVal = itrunc((v - llIntVal) * c_DecBase[decimal + 1]);
        long long llModVal = llDecimalVal % 10;
        if (llModVal >= 5)
        {
            llDecimalVal += 10;
        }
        else if (llModVal <= -5)
        {
            llDecimalVal -= 10;
        }
        llDecimalVal = llDecimalVal / 10;
        return llIntVal + llDecimalVal / c_DecBase[decimal];
    }
    else
    {
        double result = strToFloat(
                    formatFloat("0." + stringOfChar(char('#'), decimal), v + epsilon));
        if (sameValue(result, 0.0))
        {
            result = 0;
        }
        return result;
    }
}

double frac(double v)
{
    return v - itrunc(v);
}

double drandom()
{
#ifdef WIN32
    srand(GetTickCount());
    return double(rand()) / double(RAND_MAX);
#else
    return drand48();
#endif
}

int random(int n)
{
    if (n <= 0)
    {
        return 0;
    }
    srand(time(0));
    return rand() % (n);
}

int randomRange(int from, int to)
{
    if (sameValue(from, to))
    {
        return from;
    }
    else if (from > to)
    {
        return random(from - to) + to;
    }
    else
    {
        return random(to - from) + from;
    }
}

double radToDeg(double radians)
{
    return radians * 180.0 / PI;
}

double degToRad(double degrees)
{
    return degrees * PI / 180.0;
}

double power(double x, double y)
{
    return qPow(x, y);
}

bool isZero(double value, double epsilon)
{
    if (epsilon == 0)
      epsilon = 1E-19 * 1000;;
    return fabs(value) <= epsilon;
}

bool varIsNullEx(const GVariant &value)
{
    return value.isNull();
}

GVariant varArrayOf(GLDVector<GVariant> &values)
{
    GVariantList result;
    for (int i = 0; i != values.count(); ++i)
    {
        result << values.at(i);
    }
    return GVariant(result);
}

GVariant inc(const GVariant &value)
{
    GVariant result;
    switch (value.type())
    {
    case GVariant::LongLong:
        result.setValue(value.toLongLong() + 1);
        break;
    case GVariant::ULongLong:
        result.setValue(value.toULongLong() + 1);
        break;
    case GVariant::Int:
        result.setValue(value.toInt() + 1);
        break;
    case GVariant::UInt:
        result.setValue(value.toUInt() + 1);
        break;
    case GVariant::Double:
        result.setValue(value.toDouble() + 1);
        break;
    case GVariant::ByteArray:
        result.setValue(GUIDToByteArray(createGUID()));
        break;
    case GVariant::String:
        result.setValue(GUIDToVariant(createGUID()));
        break;
    case GVariant::Uuid:
        result.setValue(GUIDToVariant(createGUID()));
        break;
    default:
        assert(false);
        break;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// 说明：数字转换为汉字数字
// 参数: n -- 数字(1~99)
// 返回: 数字对应的汉字
////////////////////////////////////////////////////////////////////////////////
GString num2CChar(int n)
{
    GString result;
    if ((n < 1) || (n > 99))
    {
        return "";
    }
    int nNumber = n % 10;
    switch (nNumber)
    {
    case 0:
        result = "";
        break;
    case 1:
        result = TRANS_STRING("一");
        break;
    case 2:
        result = TRANS_STRING("二");
        break;
    case 3:
        result = TRANS_STRING("三");
        break;
    case 4:
        result = TRANS_STRING("四");
        break;
    case 5:
        result = TRANS_STRING("五");
        break;
    case 6:
        result = TRANS_STRING("六");
        break;
    case 7:
        result = TRANS_STRING("七");
        break;
    case 8:
        result = TRANS_STRING("八");
        break;
    case 9:
        result = TRANS_STRING("九");
        break;
    default:
        assert(false);
        break;
    }
    nNumber = n / 10;
    switch (nNumber)
    {
    case 1:
        result = TRANS_STRING("十") + result;
        break;
    case 2:
        result = TRANS_STRING("二十") + result;
        break;
    case 3:
        result = TRANS_STRING("三十") + result;
        break;
    case 4:
        result = TRANS_STRING("四十") + result;
        break;
    case 5:
        result = TRANS_STRING("五十") + result;
        break;
    case 6:
        result = TRANS_STRING("六十") + result;
        break;
    case 7:
        result = TRANS_STRING("七十") + result;
        break;
    case 8:
        result = TRANS_STRING("八十") + result;
        break;
    case 9:
        result = TRANS_STRING("九十") + result;
        break;
    }
    return result;
}

// 将阿拉伯数字转成中文数字字串
// 示例: Num2CNum(10002.34) ==> 一万零二点三四
GString num2ChsNum(double arabic)
{
    const GString c_ChineseNumeric = TRANS_STRING("零一二三四五六七八九");
    GString result;
    GString sIntArabic;
    GString strSectionArabic;
    GString strSection;
    int nDigit = 0;
    bool bInZero = true;
    bool bMinus = false;

    GString strArabic = floatToStr(arabic);    // 将数字转成阿拉伯数字字串
    if ('-' == strArabic.at(0).toLatin1())
    {
        bMinus = true;
        strArabic = copy(strArabic, 1);
    }
    int nPosOfDecimalPoint = pos('.', strArabic);    // 取得小数点的位置
    // 先处理整数的部分
    if (nPosOfDecimalPoint < 0)
    {
        sIntArabic = reverseString(strArabic);
    }
    else
    {
        sIntArabic = reverseString(copy(strArabic, 0, nPosOfDecimalPoint));
    }
    int nSection = 0;
    // 从个位数起以每四位数为一小节
    for (nSection = 0; nSection <= (sIntArabic.length() - 1) / 4; ++nSection)
    {
        strSectionArabic = copy(sIntArabic, nSection * 4, 4);
        strSection = "";
        // 以下的 i 控制: 个十百千位四个位数
        for (int i = 0; i != strSectionArabic.length(); ++i)
        {
            nDigit = strSectionArabic.at(i).toLatin1() - 48;
            if (0 == nDigit)
            {
                // 1. 避免 '零' 的重覆出现
                // 2. 个位数的 0 不必转成 '零'
                if ((!bInZero) && (i != 0))
                {
                    strSection = TRANS_STRING("零") + strSection;
                }
                bInZero = true;
            }
            else
            {
                switch (i)
                {
                case 1:
                    strSection = TRANS_STRING("十") + strSection;
                    break;
                case 2:
                    strSection = TRANS_STRING("百") + strSection;
                    break;
                case 3:
                    strSection = TRANS_STRING("千") + strSection;
                    break;
                default:
                    break;
                }
                strSection = c_ChineseNumeric[nDigit] + strSection;
                bInZero = false;
            }
        }
        // 加上该小节的位数
        if (0 == strSection.length())
        {
            if ((result.length() > 0) && (copy(result, 0, 2)) != TRANS_STRING("零"))
            {
                result = TRANS_STRING("零") + result;
            }
        }
        else
        {
            switch (nSection)
            {
            case 0:
                result = strSection;
                break;
            case 1:
                result = strSection + TRANS_STRING("万") + result;
                break;
            case 2:
                result = strSection + TRANS_STRING("亿") + result;
                break;
            case 3:
                result = strSection + TRANS_STRING("兆") + result;
                break;
            default:
                break;
            }
        }
    }
    // 处理小数点右边的部分
    if (nPosOfDecimalPoint >= 0)
    {
        result = result + TRANS_STRING("点");
        for (int i = nPosOfDecimalPoint + 1; i != strArabic.length(); ++i)
        {
            nDigit = strArabic.at(i).toLatin1() - 48;
            result = result + c_ChineseNumeric[nDigit];
        }
    }

    // 其他例外状况的处理
    if (0 == result.length())
    {
        result = TRANS_STRING("零");
    }
    if (copy(result, 0, 2) == TRANS_STRING("一十"))
    {
        result = copy(result, 1);
    }
    if (copy(result, 0, 1) == TRANS_STRING("点"))
    {
        result = TRANS_STRING("零") + result;
    }

    // 是否为负数
    if (bMinus)
    {
        result = TRANS_STRING("负") + result;
    }
    return result;
}

GString fourNumToChnNum(const GString &str, const GString &chnNum, bool &pre)
{
    const GString c_Digits = TRANS_STRING("零壹贰叁肆伍陆柒捌玖");
    GString result;
    int nLen = str.length();
    for (int i = 0; i != nLen; ++i)
    {
        int nOffset = str.at(i).toLatin1() - '0';
        if (0 == nOffset)
        {
            pre = true;
        }
        else
        {
            if (pre)
            {
                result = result + c_Digits[0];
            }
            result = result + c_Digits[nOffset] + copy(chnNum, nLen - i - 1, 1);
            pre = false;
        }
    }
    return stringReplace(result, TRANS_STRING("币"), "");
}

//将格式化好的小写串转换为大写串
GString stringToChnNum(const GString &str)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    const GString c_chnNum1 = GString("元万亿兆");
#else
    const GString c_chnNum1 = TRANS_STRING("元万亿兆");
#endif
    GString result;
    GString strTmp;
    GString strTmp1;
    int nLen = pos(".", str);
    int nLevel = (nLen + 3) / 4;
    int nLen1 = nLen % 4;
    if (0 == nLen1)
    {
        nLen1 = 4;
    }
    int nStart = 0;
    bool bPre = false;
    for (int i = 1; i <= nLevel; ++i)
    {
        bPre = false;
        strTmp = copy(str, nStart, nLen1);
        strTmp1 = fourNumToChnNum(strTmp, TRANS_STRING("币拾佰仟"), bPre);    // 两个空格改为币占位符
        if ((strTmp1 != "") || ((i > 1) && (i == nLevel)))
        {
            result = result + strTmp1 + copy(c_chnNum1, nLevel - i, 1);
        }
        nStart = nStart + nLen1;
        nLen1 = 4;
    }
    bPre = false;
    strTmp1 = fourNumToChnNum(copy(str, nLen + 1, 2), TRANS_STRING("分角"), bPre);
    if (strTmp1.length() == 0)
    {
        strTmp1 = TRANS_STRING("整");
    }
    return result + strTmp1;
}

// 说明：将小写金额转换为大写
GString currToChnNum(double currnum)
{
    if (fabs(currnum) < c_defDoubleEpsilon)
    {
        return TRANS_STRING("零元整");
    }
    GString result;
    if (currnum < c_defDoubleEpsilon)
    {
        result = TRANS_STRING("负");
    }
    GString strTmp = GString::number(fabs(currnum), 'f', 2);
    return result + stringToChnNum((trim(strTmp)));
}

int rightZeroNumber(const GString &str)
{
    int nCount = 0;
    for (int i = str.length() - 1; i >= 0; --i)
    {
        if (str.at(i) == QChar('0'))
        {
            ++nCount;
        }
        else if (str.at(i) == QChar('.'))
        {
            ++nCount;
            break;
        }
        else
        {
            break;
        }
    }
    return nCount;
}

int leftZeroNumber(const GString &str)
{
    int nCount = 0;
    for (int i = 0; i <= str.length() - 1; ++i)
    {
        if (str.at(i) == QChar('0'))
        {
            ++nCount;
        }
        else
        {
            break;
        }
    }
    return nCount;
}

int rightZeroNumberNextDot(const GString &str)
{
    int result = 0;
    for (int i = str.length() - 1; i >= 0; i--)
    {
        if (str.at(i) == QChar('0'))
        {
            ++result;
        }
        else if (str.at(i) == QChar('.'))
        {
            break;
        }
    }
    return result;
}

int zeroNumberAfterDot(const GString &str)
{
    int result = 0;
    int nPos = str.indexOf('.');
    if (nPos < 0)
    {
        return result;
    }
    for (int i = nPos + 1; i < str.length() - 1; ++i)
    {
        if (str.at(i) == QChar('0'))
        {
            ++result;
        }
        else
        {
            break;
        }
    }
    return result;
}

/*!
 * \brief 补齐format的后面的0的个数
 * \param format
 * \param value
 * \param nAfter
 * \return
 */
GString makeUpZero(const GString &format, const GString &value, int nAfter)
{
    GString result = value;
    int nRightZeroNum = 0;
    if (format.at(format.length() - 1) == QChar('0'))//最后一位是0, 后面补齐0
    {
        nRightZeroNum = nAfter;
    }
    else
    {
        nRightZeroNum = rightZeroNumberNextDot(format);
    }
    if ((result.length() <= nRightZeroNum) && (nRightZeroNum > 0))
    {
        int nCount = result.length();
        for (int i = 0; i < nRightZeroNum - nCount; i++)
        {
            result += '0';
        }
    }
    return result;
}

GString formatFloat(const GString &format, double value)
{
    return GLDFloatFormating::formatFloat(value, format);

//    GString sValue = GString::number(value, 'g', 14);
//    if (sValue.contains('e') || sValue.contains('E'))
//    {
//        sValue = GString::number(value, 'f', 15);
//    }
//    value = sValue.toDouble();
//    GString result;
//    if (format.toLower().contains('e'))
//    {
//        // todo
//    }
//    else if (format.contains(','))
//    {
//        int nPos = pos(".", format);
//        int nAfter = format.length() - 1 - nPos;
//        int nCommaPos = format.lastIndexOf(',');
//        GString strAfterDot;
//        if (nPos > 0)
//        {
//            int nIndex = sValue.indexOf('.');
//            if (nIndex > 0)
//            {
//                strAfterDot = copy(sValue, sValue.indexOf('.') + 1);
//            }
//            else
//            {
//                strAfterDot = "";
//            }
//            sValue = GString::number(trunc(abs(value)));
//        }
//        else
//        {
//            sValue = GString::number(qRound(abs(value)));
//            strAfterDot = "";
//        }
//        if (nPos > 0)//小数点后的格式处理
//        {
//            if (nAfter >= strAfterDot.length())
//            {
//                strAfterDot = makeUpZero(format, strAfterDot, nAfter);
//            }
//            else //截取小数,四舍五入处理
//            {
//                double dValue = strToInt64(strAfterDot);
//                long long llValue = pow(10.0, strAfterDot.length() - nAfter);
//                dValue = dValue / llValue;
//                long long nTrancValue = trunc(dValue);
//                GString strTrancValue = GString::number(nTrancValue);
//                strAfterDot = GString::number(qRound(dValue));
//                if (strTrancValue.length() < strAfterDot.length())
//                {
//                    strAfterDot = "";
//                    sValue = GString::number(strToInt64(sValue) + 1);
//                    strAfterDot = makeUpZero(format, strAfterDot, nAfter);
//                }
//                else
//                {
//                    int nLen = nAfter - strAfterDot.length();
//                    for (int i = 0; i < nLen; i++)
//                    {
//                        strAfterDot = '0' + strAfterDot;
//                    }
//                }
//            }
//        }
//        if (sValue != "")//小数点前的格式处理
//        {
//            int nLength;
//            if (nPos > 0)
//            {
//                nLength = nPos - nCommaPos - 1;
//            }
//            else
//            {
//                nLength = format.length() - nCommaPos - 1;
//            }
//            int nSum = sValue.length() / nLength;
//            result = sValue;
//            if (nSum > 0)
//            {
//                int nInsert = sValue.length() % nLength;
//                if (nInsert > 0)
//                {
//                    result = result.insert(nInsert, ',');
//                }
//                for (int i = 0; i < nSum - 1; i++)
//                {
//                    nInsert = nInsert + nLength + 1;
//                    result = result.insert(nInsert, ',');
//                }
//            }
//        }
//        if (value < 0)
//        {
//            result = "-" + result;
//        }
//        if ("" == strAfterDot)
//        {
//            return result;
//        }
//        else
//        {
//            return GString("%1.%2").arg(result).arg(strAfterDot);
//        }
//    }
//    else if (format.contains('.'))
//    {
//        int nPos = pos(".", format);
//        int nPosByValue = pos(".", sValue);
//        int nBefore = nPos;
//        int nAfter = format.length() - 1 - nPos;
//        double dAfterDot = fabs(frac(value));
//        int nAfterDot = int(dAfterDot * power(10, nAfter));
//        int nAfterByValue = sValue.length() - pos('.', sValue) - 1;

//        // value为整数补0
//        if (!sValue.contains('.'))
//        {
//            result = GString("%1.%2").arg(trunc(value), nBefore, 'f', 0, '0').arg(nAfterDot, nAfter, 'f', 0, '0');
//        }
//        else
//        {
//            // value的有效长度较小补0
//            if (nAfterByValue < nAfter)
//            {
//                GString strTmp(nAfter - nAfterByValue, '0');
//                result = GString("%1").arg(trunc(value), nBefore, 'f', 0, '0') + sValue.mid(nPosByValue) + strTmp;
//            }
//            else if (nAfter <= zeroNumberAfterDot(sValue))
//            {
//                result = GString("%1").arg(trunc(value), nBefore, 'f', 0, '0');
//            }
//            // 直接删除尾数
//            else
//            {
//                long long lValue = pow(10.0, nAfter);
//                double dValue = sValue.toDouble();
//                dValue = dValue * lValue;
//                GString strTrans = QString::number(trunc(dValue));
//                int nRoundValue = qRound(dValue);
//                GString strRound = GString::number(nRoundValue);
//                dValue = nRoundValue * 1.0 / lValue;
//                sValue = GString::number(dValue);
//                if (strTrans.length() < strRound.length())
//                {
//                    sValue = "";
//                    result = GString("%1").arg(trunc(value + 1), nBefore, 'f', 0, '0');
//                }
//                else
//                {
//                    result = GString("%1").arg(trunc(value), nBefore, 'f', 0, '0');
//                }
//                nPosByValue = 0;
//                if (sValue != "")
//                {
//                    nPosByValue = pos(".", sValue);
//                    if (nPosByValue > 0)
//                    {
//                        sValue = sValue.mid(nPosByValue + 1);
//                    }
//                }
//                int nCount = rightZeroNumber(format) - sValue.length() - 1;
//                for (int i = 0; i < nCount; i++)
//                {
//                    sValue += '0';
//                }
//                if (sValue != "")
//                {
//                    result = GString("%1.%2").arg(result).arg(sValue);
//                }
//                else
//                {
//                    return result;
//                }
//            }
//        }

//        if (format.at(format.length() - 1) == '#')
//        {
//            result = copy(result, 0, result.length() - rightZeroNumber(result));
//        }
//        if ((result.length() > 1) && ((format.at(0) == '#') || (format.at(0) == '.')))
//        {
//            result = copy(result, leftZeroNumber(result));
//        }
//        if (value < 0)
//        {
//            result = "-" + result;
//        }
//        return result;
//    }
//    else
//    {
//        // todo
//    }
//    return result;
}


GRgb turnRgb(GRgb rgb)
{
    return ((rgb & 0xff) << 16) | (rgb & 0xff00) | ((rgb & 0xff0000) >> 16);
}
