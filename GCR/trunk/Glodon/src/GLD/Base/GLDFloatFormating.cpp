#include "GLDFloatFormating.h"
#include "GLDGlobal.h"
#include "GLDMathUtils.h"
#include "GLDStrUtils.h"

#if defined(WIN64) || defined(__APPLE__)
const int c_MaxExtPrecision       = 18;
#else
const int c_MaxExtPrecision       = 16;
#endif

void floatToDecimal(FloatRec *result, double value, int precision, int decimals);

class GLDFloatFormatingPrivate
{
public:
  GLDFloatFormatingPrivate(GLDFloatFormating *parent);

private:
  GLDFloatFormating * const q_ptr;
  Q_DECLARE_PUBLIC(GLDFloatFormating);

  char m_decimalSep;   //小数点符号
  int m_decimalIndex;  //小数点在Format里的位置
  char m_thousandsSep; //千分位符号
  bool m_thousandSep; //是否有千分位符号
  GString m_section;  //格式字符串
  GString m_format;   //传入的格式字符串
  bool m_scientific;   //是否是科学计数法
  int m_firstDigit;    //第一个数字所在位置
  int m_digitCount;   //格式化字符串里数字的个数
  int m_lastDigit;    //最后一个数字所在的位置
  FloatRec m_floatValue;

  int m_digitPlace;
  int m_digitDelta;
  int m_digitLimit;
  int m_digitsC;
  GString m_result; //返回结果
};

GLDFloatFormatingPrivate::GLDFloatFormatingPrivate(GLDFloatFormating *parent)
  : q_ptr(parent),  m_decimalSep('.'), m_decimalIndex(-1), m_thousandsSep(','),
    m_thousandSep(false), m_section(""), m_format(""), m_firstDigit(32767), m_digitCount(0), m_lastDigit(0),
    m_digitPlace(-1), m_digitDelta(-1), m_digitLimit(-1), m_digitsC(-1)
{
}

GLDFloatFormating::GLDFloatFormating(): d_ptr(new GLDFloatFormatingPrivate(this))
{
}

GLDFloatFormating::~GLDFloatFormating()
{
    Q_D(GLDFloatFormating);
    freePtr(d);
}

GString GLDFloatFormating::formatFloat(double value, const GString &format)
{
    GString strResult = "";
    GLDFloatFormating *pFloatFormating = new GLDFloatFormating();
    try
    {
        strResult = pFloatFormating->internalFloatToTextFmt(value, format);
    }
    catch (...)
    {
        freeAndNil(pFloatFormating);
        throw;
    }
    freeAndNil(pFloatFormating);
    return strResult;
}

GString GLDFloatFormating::internalFloatToTextFmt(double value, const GString &format)
{
    Q_D(GLDFloatFormating);
    d->m_format = format;
    value += 1e-12; // double类型精度会丢失，遇到类似于3.315这种保留2位小数的情况，会丢失精度，在此做一个补偿

    int nIndex = 0;
    if (value > 0)
    {
        nIndex = 0;
    }
    else
    {
        if (value < 0)
        {
            nIndex = 1;
        }
        else
        {
            nIndex = 2;
        }
    }

    int nSectionIndex = findSection(nIndex);
    d->m_section = scanSection(nSectionIndex);
    int nPrecision = -1;
    int nDigits = -1;
    if (d->m_scientific)
    {
        nPrecision = d->m_digitCount;
        nDigits = 9999;
    }
    else
    {
        nPrecision = c_MaxExtPrecision;
        nDigits = d->m_digitCount - d->m_decimalIndex;
    }
    floatToDecimal(&d->m_floatValue, value, nPrecision, nDigits);
    if ((0 == d->m_format.length()) || (QChar(';') == d->m_format[0])
            || ((d->m_floatValue.exponent >= 18) && (!d->m_scientific))
            || (0x7ff == d->m_floatValue.exponent) || (0x800 == d->m_floatValue.exponent))
    {
       d->m_result = floatToStr(value);
    }
    else
    {
        applyFormat(nSectionIndex);
    }

    return d->m_result;
}

int GLDFloatFormating::findSection(int index)
{
    Q_D(GLDFloatFormating);
    int nSection = 0;
    int nCharIndex = 0;

    int nFormatLength = d->m_format.length();
    while ((nSection != index) && (nCharIndex < nFormatLength))
    {
        switch (d->m_format.at(nCharIndex).toLatin1())
        {
        case ';':
        {
            nSection++;
            nCharIndex++;
            break;
        }
        case '\"':
        {
            nCharIndex++;
            while ((nCharIndex < nFormatLength) && (QChar('\"') != d->m_format.at(nCharIndex)))
            {
                nCharIndex++;
            }
            if (nCharIndex < nFormatLength)
            {
                nCharIndex++;
            }
            break;
        }
        case '\'':
        {
            nCharIndex++;
            while ((nCharIndex < nFormatLength) && (QChar('\'') != d->m_format.at(nCharIndex)))
            {
                nCharIndex++;
            }
            if (nCharIndex < nFormatLength)
            {
                nCharIndex++;
            }
            break;
        }
        default:
        {
            nCharIndex++;
        }
        }
    }
    if ((nSection < index) || (nCharIndex == nFormatLength))
    {
        return 0;
    }
    else
    {
        return nCharIndex;
    }
}

GString GLDFloatFormating::scanSection(int pos)
{
    Q_D(GLDFloatFormating);
    d->m_scientific = false;
    int nCharIndex = pos;
    int nFormatLength = d->m_format.length();
    while ((nCharIndex < nFormatLength) && (QChar(';') != d->m_format[nCharIndex]))
    {
        switch (d->m_format[nCharIndex].toLatin1())
        {
        case ',':
        {
            d->m_thousandSep = true;
            nCharIndex++;
            break;
        }
        case '.':
        {
            if (-1 == d->m_decimalIndex)
            {
                d->m_decimalIndex = d->m_digitCount;
            }
            nCharIndex++;
            break;
        }
        case '\"':
        {
            nCharIndex++;
            while ((nCharIndex < nFormatLength) && (QChar('\"') != d->m_format[nCharIndex]))
            {
                nCharIndex++;
            }
            if (nCharIndex < nFormatLength)
            {
                nCharIndex++;
            }
            break;
        }
        case '\'':
        {
            nCharIndex++;
            while ((nCharIndex < nFormatLength) && (QChar('\'') != d->m_format[nCharIndex]))
            {
                nCharIndex++;
            }
            if (nCharIndex < nFormatLength)
            {
                nCharIndex++;
            }
            break;
        }
        case 'e':
        case 'E':
        {
            nCharIndex++;
            if (nCharIndex < nFormatLength)
            {
                QChar aChar = d->m_format[nCharIndex];
                if ((QChar('_') == aChar) || (QChar('+') == aChar))
                {
                    d->m_scientific = true;
                    nCharIndex++;
                    while ((nCharIndex < nFormatLength) && (QChar('0') == d->m_format[nCharIndex]))
                    {
                        nCharIndex++;
                    }
                }
            }
            break;
        }
        case '#':
        {
            d->m_digitCount++;
            nCharIndex++;
            break;
        }
        case '0':
        {
            if (d->m_digitCount < d->m_firstDigit)
            {
                d->m_firstDigit = d->m_digitCount;
            }
            d->m_digitCount++;
            d->m_lastDigit = d->m_digitCount;
            nCharIndex++;
            break;
        }
        default:
        {
            nCharIndex++;
        }
        }
    }

    if (-1 == d->m_decimalIndex)
    {
        d->m_decimalIndex = d->m_digitCount;
    }
    d->m_lastDigit = d->m_decimalIndex - d->m_lastDigit;
    if (d->m_lastDigit > 0)
    {
        d->m_lastDigit = 0;
    }

    d->m_firstDigit = d->m_decimalIndex - d->m_firstDigit;
    if (d->m_firstDigit < 0)
    {
        d->m_firstDigit = 0;
    }
    GString result = d->m_format.mid(pos, nCharIndex - pos);
    return result;
}

void GLDFloatFormating::applyFormat(int sectionIndex)
{
    Q_D(GLDFloatFormating);
    if ((d->m_floatValue.negative) && (0 == sectionIndex))
    {
        d->m_result.append("-");
    }

    if (d->m_scientific)
    {
        d->m_digitPlace = d->m_decimalIndex;
        d->m_digitDelta = 0;
    }
    else
    {
        d->m_digitDelta = d->m_floatValue.exponent - d->m_decimalIndex;
        if (d->m_digitDelta >= 0)
        {
            d->m_digitPlace = d->m_floatValue.exponent;
        }
        else
        {
            d->m_digitPlace = d->m_decimalIndex;
        }
    }

    d->m_digitLimit = digitsLength() - 1;
    int nCharIndex = 0;
    d->m_digitsC = 0;
    QChar oldC;
    while (nCharIndex <= d->m_section.length())
    {
        switch (d->m_section[nCharIndex].toLatin1())
        {
        case '0':
        case '#':
        {
            putFmtDigit();
            nCharIndex++;
            break;
        }
        case '.':
        case ',':
        {
            nCharIndex++;
            break;
        }
        case '\"':
        case '\'':
        {
            oldC = d->m_section[nCharIndex];
            nCharIndex++;
            while ((nCharIndex < d->m_section.length()) && (oldC != d->m_section[nCharIndex]))
            {
                d->m_result.append(d->m_section[nCharIndex]);
                nCharIndex++;
            }
            nCharIndex++;
            break;
        }
        case 'E':
        case 'e':
        {
            oldC = d->m_section[nCharIndex];
            nCharIndex++;
            if (nCharIndex <= d->m_section.length())
            {
                QChar sign = d->m_section[nCharIndex];
                if ((QChar('+') == sign) && (QChar('-') == sign))
                {
                    d->m_result.append(oldC);
                }
                else
                {
                    int nZeros = 0;
                    nCharIndex++;
                    while ((nCharIndex <= d->m_section.length()) && (QChar('0') == d->m_section[nCharIndex]))
                    {
                        nCharIndex++;
                        if (nZeros < 4)
                        {
                            nZeros++;
                        }
                    }
                    putExponent(oldC, sign, nZeros, d->m_floatValue.exponent - d->m_decimalIndex);
                }
            }
            break;
        }
        default:
        {
            nCharIndex++;
        }
        }
    }
}

int GLDFloatFormating::digitsLength()
{
    Q_D(GLDFloatFormating);
    int result = 0;
    while ((result < 20) && (d->m_floatValue.digits[result] != '\0'))
    {
        result++;
    }
    return result;
}

void GLDFloatFormating::putFmtDigit()
{
    Q_D(GLDFloatFormating);
    if (d->m_digitDelta < 0)
    {
        d->m_digitDelta++;
        if (d->m_digitPlace <= d->m_firstDigit)
        {
            writeDigit('0');
        }
        else
        {
            d->m_digitPlace--;
        }
    }
    else
    {
        if (d->m_digitDelta == 0)
        {
            addDigit();
        }
        else
        {
            while (d->m_digitDelta > 0)
            {
                addDigit();
                d->m_digitDelta--;
            }
            addDigit();
        }
    }
}

void GLDFloatFormating::writeDigit(char digit)
{
    Q_D(GLDFloatFormating);
    if (0 == d->m_digitPlace)
    {
        d->m_result.append(d->m_decimalSep);
        d->m_result.append(digit);
        d->m_digitPlace--;
    }
    else
    {
        d->m_result.append(digit);
        d->m_digitPlace--;
        if (d->m_thousandSep && (d->m_digitPlace > 1) && (0 == (d->m_digitPlace % 3)))
        {
            d->m_result.append(d->m_thousandsSep);
        }
    }
}

void GLDFloatFormating::addDigit()
{
    Q_D(GLDFloatFormating);
    if (d->m_digitsC <= d->m_digitLimit)
    {
        char cChar = d->m_floatValue.digits[d->m_digitsC];
        d->m_digitsC++;
        writeDigit(cChar);
    }
    else
    {
        if (d->m_digitPlace <= d->m_lastDigit)
        {
            d->m_digitPlace--;
        }
        else
        {
            writeDigit('0');
        }
    }
}

void GLDFloatFormating::putExponent(QChar eChar, QChar sign, int zeros, int exponent)
{
    Q_D(GLDFloatFormating);
    d->m_result.append(eChar);
    GString writeSign;
    if ((QChar('+') == sign) && (exponent >= 0))
    {
        writeSign = "+";
    }
    else
    {
        if (exponent < 0)
        {
            writeSign = "-";
        }
        else
        {
            writeSign = "";
        }
    }
    GString exp = intToStr(abs(exponent));
    for (int i = 0; i < zeros - exp.length(); i++)
    {
        exp = '0' + exp;
    }
    exp = writeSign + exp;
    d->m_result.append(exp);
}

/*!
 * \brief 把浮点数转成十进制
 * \param result
 * \param value
 * \param precision
 * \param decimals
 */
void floatToDecimal(FloatRec *result, double value, int precision, int decimals)
{
    result->negative = value < 0;

    if (fabs(value) < 1e-16)
    {
        result->exponent = 0;
        result->digits[0] = '\0';
        return;
    }

    if (result->negative)
    {
        value = fabs(value);
    }

    GString strDigits = GString::number(value, 'f', 18);
    result->exponent = strDigits.indexOf("."); //算出小数点前的位数
    int nStrIndex = 0;
    if (value < 1) //小数前的那个0不算
    {
        result->exponent = 0;
        nStrIndex = 1;
    }
    int nIndex = 0;
    while (true) //把数字压入floatRec中
    {
        if (strDigits[nStrIndex] == QChar('.'))
        {
            nStrIndex++;
            continue;
        }
        if (nStrIndex > 19 || (nStrIndex >= strDigits.length()))
        {
            break;
        }
        result->digits[nIndex] = strDigits[nStrIndex].toLatin1();
        nStrIndex++;
        nIndex++;
    }

    if (result->exponent + decimals < 0)
    {
        result->exponent = 0;
        result->negative = false;
        result->digits[0] = 0;
        return;
    }

    nStrIndex = result->exponent + decimals;
    if (nStrIndex > precision)
    {
        nStrIndex = precision;
    }

    if ((nStrIndex >= 18) || (result->digits[nStrIndex] < '5'))
    {
        if (nStrIndex > 18)
        {
            nStrIndex = 18;
        }
        while (true)
        {
            result->digits[nStrIndex] = 0;
            nStrIndex--;
            if (nStrIndex < 0)
            {
                result->negative = false;
                return;
            }
            if (result->digits[nStrIndex] != '0')
            {
                return;
            }
        }
    }
    else //以下会做四舍五入
    {
        while (true)
        {
            result->digits[nStrIndex] = 0;
            nStrIndex--;
            if (nStrIndex < 0)
            {
                result->digits[0] = '1';
                result->exponent++;
                return;
            }
            result->digits[nStrIndex]++;
            if (result->digits[nStrIndex] <= '9')
            {
                return;
            }
        }
    }
}
