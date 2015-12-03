#ifdef WIN32
#   include <Windows.h>
#else

#endif
#include <string>

#include <QLocale>
#include <QTextCodec>

#include "GLDStrUtils.h"
#include "GLDSysUtils.h"
#include "GLDMathUtils.h"
#include "GLDException.h"
#include "GLDStrings.h"
#include "GLDStream.h"

typedef QTextCodec GTextCodec;

const int c_HoursPerDay   = 24;
const int c_MinsPerHour   = 60;
const int c_SecsPerMin    = 60;
const int c_MSecsPerSec   = 1000;
const int c_MinsPerDay    = c_HoursPerDay * c_MinsPerHour;
const int c_SecsPerDay    = c_MinsPerDay * c_SecsPerMin;
const int c_MSecsPerDay   = c_SecsPerDay * c_MSecsPerSec;

static GTextCodec *s_gldGBKCodec = NULL;
static GTextCodec *s_gldUTF8Codec = NULL;

#ifdef WIN32
    DEFINE_RESOURCESTRING(sLineBreak, "\r\n");
#else
    DEFINE_RESOURCESTRING(sLineBreak, "\r");
#endif

class GLDTextCodec
{
public:
    static GTextCodec *codecForGBK();
    static GTextCodec *codecForUTF8();
    static GTextCodec *codecForLocale();
};

/* GLDTextCodec */

GTextCodec *GLDTextCodec::codecForGBK()
{
    if (!s_gldGBKCodec)
    {
        s_gldGBKCodec = GTextCodec::codecForName("GBK");
        assert(s_gldGBKCodec);
    }
    return s_gldGBKCodec;
}

GTextCodec *GLDTextCodec::codecForUTF8()
{
    if (!s_gldUTF8Codec)
    {
        s_gldUTF8Codec = GTextCodec::codecForName("UTF-8");
        assert(s_gldUTF8Codec);
    }
    return s_gldUTF8Codec;
}

GTextCodec *GLDTextCodec::codecForLocale()
{
    return GTextCodec::codecForLocale();
}

// public
/*
//获取C风格字符串（\0结尾的字符串）的长度
size_t CStrLength(const char *sz)
{
    int i = 0;
    while (sz[i])
    {
        ++i;
    }
    return i;
}

size_t CStrLength(const wchar_t *wsz)
{
    return WideCStrLength(wsz);
}

size_t WideCStrLength(const wchar_t *wsz)
{
    int i = 0;
    while (wsz[i])
    {
        i++;
    }
    return i;
}

wchar_t *WideLowerCase(wchar_t **ppWsz)
{
    GString s((const GChar *)(*ppWsz));
    s = s.toLower();
    wstring ws = s.toLower().toStdWString();

    wchar_t *wCh = new wchar_t[ws.size() + 1];
    wCh[ws.size()] = '\0';
    for (int i = 0; i < int(ws.size()); ++i)
    {
        wCh[i] = ws[i];
    }
    return wCh;
}

//比较两个字符串，大小写敏感
bool SameStr(const char *szA, const char *szB)
{
    return 0 == strcmp(szA, szB);
}

//比较两个字符串，大小写敏感，宽字符版本
bool WideSameStr(const wchar_t *wszA, const wchar_t *wszB)
{
    return 0 == wcscmp(wszA, wszB);
}

//比较两个字符串，大小写不敏感
bool SameText(const char *szA, const char *szB)
{
#ifdef WIN32
    return 0 == _stricmp(szA, szB);
#else
    return 0 == strcasecmp(szA, szB);
#endif
}

//比较两个字符串，大小写不敏感，宽字符版本
bool WideSameText(const wchar_t *wszA, const wchar_t *wszB)
{
#ifdef WIN32
    return 0 == _wcsicmp(wszA, wszB);
#else
    return 0 == sameText(GString((const GChar *)wszA),
                         GString((const GChar *)wszB));
#endif
}

wchar_t *charToWchar(const char *str)
{
    wchar_t* buffer;
    if(str)
    {
        size_t nu = strlen(str);
        size_t n =(size_t)MultiByteToWideChar(CP_ACP,0,(const char *)str,int(nu),NULL,0);
        buffer=0;
        buffer = new wchar_t[n+1];
        ::MultiByteToWideChar(CP_ACP,0,(const char *)str,int(nu),buffer,int(n));
        buffer[n] = 0;
    }
    return buffer;
    delete buffer;
}

char *wcharToChar(const wchar_t *pwstr)
{
    int nlength = (int)wcslen(pwstr);
    //获取转换后的长度
    int nbytes = WideCharToMultiByte( 0,         // specify the code page used to perform the conversion
                                      0,         // no special flags to handle unmapped characters
                                      pwstr,     // wide character string to convert
                                      nlength,   // the number of wide characters in that string
                                      NULL,      // no output buffer given, we just want to know how long it needs to be
                                      0,
                                      NULL,      // no replacement character given
                                      NULL );    // we don't want to know if a character didn't make it
                                                 // through the translation
    char *pcstr = new char[nbytes + 1];
    // 通过以上得到的结果，转换unicode 字符为ascii 字符
    WideCharToMultiByte( 0,         // specify the code page used to perform the conversion
                         0,         // no special flags to handle unmapped characters
                         pwstr,     // wide character string to convert
                         nlength,   // the number of wide characters in that string
                         pcstr,     // put the output ascii characters at the end of the buffer
                         nbytes,    // there is at least this much space there
                         NULL,      // no replacement character given
                         NULL );
    pcstr[nbytes] = 0;
    return pcstr;
}
*/

GString utf8ToUnicode(const char *in, int length)
{
    return GLDTextCodec::codecForUTF8()->toUnicode(
                in, (-1 != length) ? length : (int)strlen(in));
}

GByteArray unicodeToUTF8(const GChar *in, int length)
{
    return GLDTextCodec::codecForUTF8()->fromUnicode(
                in, (-1 != length) ? length : (int)wcslen((const wchar_t *)in));
}

GString asciiToUnicode(const char *in, int length, const char *name)
{
    if (name == NULL)
    {
        return GLDTextCodec::codecForLocale()->toUnicode(
                    in, (-1 != length) ? length : (int)strlen(in));
    }
    else
    {
        return GTextCodec::codecForName(name)->toUnicode(
                    in, (-1 != length) ? length : (int)strlen(in));
    }

}

GByteArray unicodeToAscii(const GChar *in, int length, const char *name)
{
    if (name == NULL)
    {
        return GLDTextCodec::codecForGBK()->fromUnicode(
                    in, (-1 != length) ? length : (int)wcslen((const wchar_t *)in));
    }
    else
    {
        return GTextCodec::codecForName(name)->fromUnicode(
                    in, (-1 != length) ? length : (int)wcslen((const wchar_t *)in));
    }
}

GString gbkToUnicode(const char *in, int length)
{
    return GLDTextCodec::codecForGBK()->toUnicode(
                in, (-1 != length) ? length : (int)strlen(in));
}

GByteArray unicodeToGBK(const GChar *in, int length)
{
    return GLDTextCodec::codecForGBK()->fromUnicode(
                in, (-1 != length) ? length : (int)wcslen((const wchar_t *)in));
}

GString ansiUpperCase(const GString &value)
{
    return value.toUpper();
}

GString ansiLowerCase(const GString &value)
{
    return value.toLower();
}

GString upperCase(const GString &value)
{
    return value.toUpper();
}

GString lowerCase(const GString &value)
{
    return value.toLower();
}

char upperCase(const char ch)
{
    GChar chStr(ch);
    return chStr.toUpper().toLatin1();
}

char lowerCase(const char ch)
{
    GChar chStr(ch);
    return chStr.toLower().toLatin1();
}

wchar_t upperCase(const wchar_t ch)
{
    if ((int)ch > 256)
    {
        return ch;
    }
    GChar chStr(ch);
    return chStr.toUpper().toLatin1();
}

wchar_t lowerCase(const wchar_t ch)
{
    if ((int)ch > 256)
    {
        return ch;
    }
    GChar chStr(ch);
    return chStr.toLower().toLatin1();
}

int compareStr(const GStringRef &s1, const GStringRef &s2)
{
    return GStringRef::compare(s1, s2, Qt::CaseSensitive);
}

int compareStr(const GStringRef &s1, const GString &s2)
{
    return s1.compare(s2, Qt::CaseSensitive);
}

// call CompareString : 'A' > 'a' > '9' > '0'
int compareStr(const GString &s1, const GString &s2)
{
    return GString::localeAwareCompare(s1, s2);
}

int compareStr(const GByteArray &s1, const GByteArray &s2)
{
    return strcmp(s1.constData(), s2.constData());
}

int compareText(const GStringRef &s1, const GStringRef &s2)
{
    return s1.compare(s1, s2, Qt::CaseInsensitive);
}

int compareText(const GStringRef &s1, const GString &s2)
{
    return s1.compare(s2, Qt::CaseInsensitive);
}

int compareText(const GString &s1, const GString &s2)
{
    return GString::localeAwareCompare(s1.toUpper(), s2.toUpper());
}

int compareText(const GByteArray &s1, const GByteArray &s2)
{
#ifdef WIN32
    return _stricmp(s1.constData(), s2.constData());
#else
    return strcasecmp(s1.constData(), s2.constData());
#endif
}

// 如果类型不匹配则抛异常
GLDValueRelationship compareGVariant(const GVariant &v1, const GVariant &v2)
{
    if (!v1.isValid() || !v2.isValid() || v1.isNull() || v2.isNull())
    {
        if (!v1.isValid() || !v2.isValid())
        {
            return v1.isValid() == v2.isValid() ? gvrEqualsValue : (v1.isValid() ? gvrGreaterThanValue : gvrLessThanValue);
        }
        else
        {
            return v1.isNull() == v2.isNull() ? gvrEqualsValue : (!v1.isNull() ? gvrGreaterThanValue : gvrLessThanValue);
        }
    }
    GVariant::Type t1 = v1.type();
    GVariant::Type t2 = v2.type();
    if (t1 == t2)
    {
        if (variantTypeIsUnsigned(t1))
        {
            unsigned long long n1 = v1.toULongLong();
            unsigned long long n2 = v2.toULongLong();
            return compareDigit(n1, n2);
        }
        else if (variantTypeIsDigit(t1))
        {
            long long ll1 = v1.toLongLong();
            long long ll2 = v2.toLongLong();
            return compareDigit(ll1, ll2);
        }
        else if (variantTypeIsDateTime(t1))
        {
            GDateTime n1 = v1.toDateTime();
            GDateTime n2 = v2.toDateTime();
            return compareDigit(n1, n2);
        }
        else if (variantTypeIsNumeric(t1) && variantTypeIsNumeric(t2))
        {
            double d1 = v1.toDouble();
            double d2 = v2.toDouble();
            if (sameValue(d1, d2))
            {
                return gvrEqualsValue;
            }
            return compareDigit(d1, d2);
        }
        else if (t1 == GVariant::ByteArray)
        {
            return GLDValueRelationship(compareStr(v1.toByteArray(), v2.toByteArray())); // todo 优化
        }
        else if (t1 == GVariant::String)
        {
            return GLDValueRelationship(compareStr(v1.toString(), v2.toString()));
        }
        else if (t1 == GVariant::Uuid)
        {
            return GLDValueRelationship(compareGUID(variantToGUID(v1.toUuid()), variantToGUID(v2.toUuid())));
        }
        else if (t1 == GVariant::Bool)
        {
            if (v1 == v2)
            {
                return gvrEqualsValue;
            }
            else if (v1.toBool())
            {
                return gvrGreaterThanValue;
            }
            else
            {
                return gvrLessThanValue;
            }
        }
        else
        {
            if (v1 == v2)
            {
                return gvrEqualsValue;
            }
            gldError(getGLDi18nStr(g_InvalidTypeCompare));
            return gvrEqualsValue;
        }
    }
    else if (variantTypeIsDigit(t1) && variantTypeIsDigit(t2))
    {
        if (variantTypeIsUnsigned(t1) || variantTypeIsUnsigned(t2))
        {
            unsigned long long n1 = v1.toULongLong();
            unsigned long long n2 = v2.toULongLong();
            return compareDigit(n1, n2);
        }
        else
        {
            long long ll1 = v1.toLongLong();
            long long ll2 = v2.toLongLong();
            return compareDigit(ll1, ll2);
        }
    }
    else if (variantTypeIsNumeric(t1) && variantTypeIsNumeric(t2))
    {
        double d1 = v1.toDouble();
        double d2 = v2.toDouble();
        if (sameValue(d1, d2))
        {
            return gvrEqualsValue;
        }
        return compareDigit(d1, d2);
    }
    else
    {
        if (v1 == v2)
        {
            return gvrEqualsValue;
        }
        else
        {
            // no exception
            return gvrGreaterThanValue;
        }
    }
}

bool sameGVariant(const GVariant &v1, const GVariant &v2)
{
    if (variantTypeIsFloat(v1.type()) || variantTypeIsFloat(v2.type()))
    {
        // 只要有一个不是数字则不等
        if (!variantTypeIsNumeric(v1.type()) || !variantTypeIsNumeric(v2.type()))
        {
            return false;
        }
        double d1 = v1.toDouble();
        double d2 = v2.toDouble();
        if (sameValue(d1, d2))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return v1 == v2;
}

// 区分大小写
bool sameStr(const GString &s1, const GString &s2)
{
    return s1 == s2;
}

int length(const GString &s)
{
    return s.length();
}

int pos(const GString &subs, const GString &s)
{
    if ((subs.length() == 0) || (s.length() == 0))
    {
        return -1;
    }
    return s.indexOf(subs);
}

int pos(const GChar &subs, const GString &s)
{
    if (s.length() == 0)
    {
        return -1;
    }
    return s.indexOf(subs);
}

int pos(const GString &subs, const GString &s, int nFrom)
{
    if ((subs.length() == 0) || (s.length() == 0))
    {
        return -1;
    }
    return s.indexOf(subs, nFrom);
}

int rPos(const GString &subs, const GString &s, int times)
{
    if ((subs.length() == 0) || (s.length() == 0))
    {
        return -1;
    }
    assert(times >= 1);
    int result = 0;
    for (int i = 1; i <= times; ++i)
    {
        result = s.lastIndexOf(subs, result - 1);
        if (-1 == result)
            break;
    }
    return result;
}

int rPos(const GChar &subs, const GString &s, int times)
{
    if (s.length() == 0)
    {
        return -1;
    }
    assert(times >= 1);
    int result = 0;
    for (int i = 1; i <= times; ++i)
    {
        result = s.lastIndexOf(subs, result - 1);
        if (-1 == result)
            break;
    }
    return result;
}

int rPosEx(const GChar &subs, const GString &s, int offset)
{
    if (s.length() == 0)
    {
        return -1;
    }
    return s.lastIndexOf(subs, offset);
}

GString trim(const GString &s)
{
    return s.trimmed();
}

GString trimRight(const GString &s)
{
    int nIter = s.length() - 1;
    while ((nIter >= 0) && (s[nIter] <= ' '))
    {
        nIter--;
    }
    return copy(s, 0, nIter + 1);
}

GString trimLeft(const GString &s)
{
    int nLen = s.length() - 1;
    int nIter = 0;
    while ((nIter <= nLen) && (s[nIter] <= ' '))
    {
        nIter++;
    }
    return copy(s, nIter, nLen + 1);
}

GString copy(const GString &s, int position, int n)
{
    if (position < 0)
    {
        position = 0;
    }
    if (n == MaxInt)
    {
        n = -1;
    }
    return s.mid(position, n);
}

GString stringReplace(const GString &s, const GString &before, const GString &after)
{
    GString result = s;
    result.replace(before, after);
    return result;
}

bool containsText(const GString &text, const GString subText)
{
    return pos(upperCase(subText), upperCase(text)) >= 0;
}

GString leftStr(const GString &text, int count)
{
    return text.left(count);
}

GString rightStr(const GString &text, int count)
{
    return text.right(count);
}

GStringList split(const GString &s, GChar sep)
{
    if (s == "")
        return GStringList();
    else
        return s.split(sep);
}

GStringList split(const GString &s, const GString &sep)
{
    if (s == "")
        return GStringList();
    else
        return s.split(sep);
}

bool isInt(const GString &s)
{
    bool result = false;
    s.toInt(&result);
    return result;
}

bool isInt64(const GString &s)
{
    bool result = false;
    s.toLongLong(&result);
    return result;
}

bool isNumeric(const GString &s)
{
    bool result = false;
    if (s.contains('e', Qt::CaseInsensitive))
    {
        GString value = s.toLower();
        // 找到最后一个e
        int nIndex = rPos('e', value);
        // 找到e后的最后一个点
        nIndex = rPos('.', value);
        if (nIndex > 0)
        {
            bool bIncludeNoZero = false;
            // 判断最后一个点后是否全都是非0
            for (int i = nIndex + 1; i < value.length(); ++i)
            {
                if (value[i] != '0')
                {
                    bIncludeNoZero = true;
                    break;
                }
            }
            // 如果最后一个点后全都是0，则去掉点和0
            if (!bIncludeNoZero)
            {
                value = value.mid(0, nIndex);
            }
            // 重新检查是否合法
            value.toDouble(&result);
            return result;
        }
    }
    s.toDouble(&result);
    return result;
}

bool isDateTime(const GString &s)
{
    bool result = false;
    s.toDouble(&result);
    return result;
}

bool charIsDigit(const GChar &ch)
{
    return ch.isDigit();
}

bool variantTypeIsByte(const GVariant::Type type)
{
    return (type == GVariant::Char)
            || (type == (GVariant::Type)QMetaType::UChar)
            || (type == (GVariant::Type)QMetaType::SChar);
}

bool variantTypeIsShort(const GVariant::Type type)
{
    return (type == (GVariant::Type)QMetaType::Short)
            || (type == (GVariant::Type)QMetaType::UShort);
}

bool variantTypeIsInt(const GVariant::Type type)
{
    return (type == GVariant::Int) || (type == GVariant::UInt)
            || (type == (GVariant::Type)QMetaType::Long)
            || (type == (GVariant::Type)QMetaType::ULong);
}

bool variantTypeIsDigit(const GVariant::Type type)
{
    return (type >= QVariant::Int && type <= QVariant::ULongLong)
            || (type >= (GVariant::Type)QMetaType::Long && type <= (GVariant::Type)QMetaType::UChar)
            || (type == (GVariant::Type)QMetaType::SChar);
    //return variantTypeIsByte(type) || variantTypeIsShort(type) ||
    //        variantTypeIsInt(type) || variantTypeIsInt64(type);
}

bool variantTypeIsFloat(const GVariant::Type type)
{
    return (type == GVariant::Double) || (type == (GVariant::Type)QMetaType::Float);
}

bool variantTypeIsNumeric(const GVariant::Type type)
{
    return (type >= QVariant::Int && type <= QVariant::Double)
            || (type >= (GVariant::Type)QMetaType::Long && type <= (GVariant::Type)QMetaType::Float)
            || (type == (GVariant::Type)QMetaType::SChar);
    //return variantTypeIsDigit(type) || variantTypeIsFloat(type);
}

bool variantTypeIsDateTime(const GVariant::Type type)
{
    return (type == GVariant::Date) || (type == GVariant::Time) || (type == GVariant::DateTime);
}

bool variantTypeIsUnsigned(const QVariant::Type type)
{
    return (type == (GVariant::Type)QMetaType::UChar)
            || (type == (GVariant::Type)QMetaType::UShort)
            || (type == (GVariant::Type)QMetaType::ULong)
            || (type == GVariant::UInt)
            || (type == GVariant::ULongLong);
}

#ifdef WIN32
size_t getBSTRLen(const BSTR & s)
{
    return ::SysStringLen(s);
}

GString BSTRToGString(const BSTR &s)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return GString::fromUtf16((ushort *)s, (int)getBSTRLen(s));
#else
    return GString::fromUtf16(s, BSTRLen(s));
#endif
}

BSTR GStringToBSTR(const GString & s)
{
    BSTR result = NULL;
    if (length(s) != 0)
    {
        result = ::SysAllocString((wchar_t *)s.unicode());
    }
    return result;
}

void freeBSTR(BSTR & s)
{
    ::SysFreeString(s);
}

#endif

void checkFormatDataType(QChar ch, GVariant &v)
{
    ch = ch.toLower();
    GVariant::Type type = v.type();
    if (ch == QLatin1Char('s'))
    {
        if (type != GVariant::String)
        {
            gldError("format '%s' invalid or incompatible with argument.");
        }
    }
    else if (ch == QLatin1Char('d'))
    {
        if (!variantTypeIsDigit(type))
        {
            gldError("format '%d' invalid or incompatible with argument.");
        }
    }
    else if (ch == QLatin1Char('u'))
    {
        if (!variantTypeIsDigit(type))
            gldError("format '%u' invalid or incompatible with argument.");
        v = GVariant((unsigned int)(v.toInt()));
    }
    else if (ch == QLatin1Char('f'))
    {
        if (!variantTypeIsDigit(type) && !variantTypeIsFloat(type))
            gldError("format '%f' invalid or incompatible with argument.");
        v = GString::number(v.toDouble(), 14, 2);
    }
    else if (ch == QLatin1Char('g'))
    {
        if (!variantTypeIsDigit(type) && !variantTypeIsFloat(type))
            gldError("format '%g' invalid or incompatible with argument.");
    }
    else if (ch == QLatin1Char('n'))
    {
        if (!variantTypeIsDigit(type) && !variantTypeIsFloat(type))
            gldError("format '%n' invalid or incompatible with argument.");
    }
    else if (ch == QLatin1Char('m'))
    {
        if (!variantTypeIsDigit(type) && !variantTypeIsFloat(type))
            gldError("format '%m' invalid or incompatible with argument.");
    }
    else if (ch == QLatin1Char('x'))
    {
        if (!variantTypeIsDigit(type))
            gldError("format '%x' invalid or incompatible with argument.");
        v = GString::number(v.toLongLong(), 16).toUpper();
    }
    else if (!ch.isDigit())
    {
        gldError("format '%' invalid or incompatible with argument.");
    }
}

GString format(const GString &s, const GVariantList &a)
{
    GString result;
    int nLength = s.length();
    const QChar *data = s.constData();
    int nBefore = 0;
    int nIndex = 0;
    for (int i = 0; i < nLength; ++i)
    {
        if (data[i] == QLatin1Char('%'))
        {
            result.append(s.mid(nBefore, i - nBefore));
            ++i;
            if (i == nLength)
            {
                nBefore = nLength;
                break;
            }

            nBefore = i + 1;
            QChar ch = data[i];
            if (ch == QLatin1Char('%'))
            {
                result.append(QLatin1Char('%'));
            }
            else
            {
                GVariant var = a.at(nIndex);
                if (nIndex == a.count())
                    gldError("format '%' invalid or incompatible with argument.");
                checkFormatDataType(ch, var);
                result.append(var.toString());
                ++nIndex;
            }
        }
    }
    result.append(s.mid(nBefore, nLength - nBefore));
    return result;
}

GString format(const GString &s, const GString &after, const GString &before)
{
    GString result = s;
    int nIndex = result.indexOf(before);
    if (nIndex >= 0)
    {
        //result.replace(i, before.length(), after);
        result.remove(nIndex, length(before));
        result.insert(nIndex, after);
    }
    else
        result = result.arg(after);
    return result;
}

GString format(const GString &s, const GChar &after, const GString &before)
{
    return format(s, GString(after), before);
}

GString format(const GString &s, int a)
{
    return format(s, intToStr(a), "%d");
}

GString format(const GString &s, long a)
{
    return format(s, intToStr(a), "%d");
}

GString format(const GString &s, ULONGLONG a)
{
    return format(s, uint64ToStr(a), "%ld");
}

GString format(const GString &s, long long a)
{
    return format(s, int64ToStr(a), "%ld");
}

GString format(const GString &s, double a)
{
    return format(s, floatToStr(a), "%lf");
}

//GString format(const GString &s, const GVariant &a)
//{
//    switch (a.type())
//    {
//    case GVariant::UInt:
//    case GVariant::Int:
//        return format(s, a.toInt());
//    case GVariant::LongLong:
//        return format(s, a.toLongLong());
//    case GVariant::ULongLong:
//        return format(s, a.toULongLong());
//    case GVariant::Double:
//        return format(s, a.toDouble());
//    case GVariant::String:
//        return format(s, a.toString());
//    case GVariant::ByteArray:
//        return format(s, GString(a.toByteArray()));
//    case GVariant::Uuid:
//        return format(s, GString(a.toString()));
//    default:
//        break;
//    }
//    return GString();
//}

GString boolToStr(bool a, bool useBoolStrs)
{
    if (useBoolStrs)
    {
        if (a)
            return "True";
        else
            return "False";
    }
    else
    {
        if (a)
            return "1";
        else
            return "0";
    }
}

bool strToBool(const GString &s)
{
    if (s.isEmpty())
        return false;
    else if (s[0].toUpper() == 'T')
        return true;
    else if (s[0].toUpper() == '1')
        return true;
    else
        return false;
}

bool strToBoolDef(const GString &s, bool def)
{
    if (s.isEmpty())
        return def;
    else if (s[0].toUpper() == 'T')
        return true;
    else if (s[0].toUpper() == 'F')
        return false;
    else if (s[0].toUpper() == '1')
        return true;
    else if (s[0].toUpper() == '0')
        return false;
    else
        return def;
}

GString intToStr(int a)
{
#ifdef _WIN32
    char str[20];
    memset(str, '\0', 20);
    _itoa_s(a, str, 10);
    GString result = QString::fromLatin1(str);
    return result;
#else
    return GString::number(a, 10);
#endif
}

GString int64ToStr(gint64 a)
{
    return GString::number(a, 10);
}

GString uint64ToStr(guint64 a)
{
    return GString::number(a, 10);
}

int strToInt(const GString &s)
{
    bool bOk;
    int result = s.toInt(&bOk);
    if (bOk)
        return result;
    else
    {
        gldError(format(getGLDi18nStr(g_InvalidInteger), s));
        return 0;
    }
}

int strToIntDef(const GString &s, int def)
{
    bool bOk;
    int result = s.toInt(&bOk);
    if (bOk)
        return result;
    else
        return def;
}

gint64 strToInt64(const GString &s, int base)
{
    bool bOk;
    gint64 result = s.toLongLong(&bOk, base);
    if (bOk)
        return result;
    else
        throw GLDException(format(getGLDi18nStr(g_InvalidInteger), s));
}

gint64 strToInt64Def(const GString &s, gint64 def)
{
    bool bOk;
    gint64 result = s.toLongLong(&bOk);
    if (bOk)
        return result;
    else
        return def;
}

GString floatToStr(double a)
{
    // 为了跟fRound匹配, 浮点数显示14位有效数字
    // 为了跟delphi保持一致, 浮点数显示15位有效数字
    if ((fabs(a) < 2e9) && (sameValue(itrunc(a), a, 1e-307)))
    {
        return intToStr(a);
    }
    else
    {
        //把结果格式化进入字符串
        char pbufer[32];
        sprintf_s(pbufer, 32, "%.15g", a);//格式化字符串
        QString result = GString(GLatin1String(pbufer));//将char转换成GString
        return result;

//        QString result;
//        result.setNum(a, 'g', 15);
//        return result;
    }
}

double strToFloat(const GString &s)
{
    bool bOk;
    double result = s.toDouble(&bOk);
    if (bOk)
        return result;
    else
        throw GLDException(format(getGLDi18nStr(g_InvalidFloat), s));
}

double strToFloatDef(const GString &s, double def)
{
    bool bOk;
    double result = s.toDouble(&bOk);
    if (bOk)
        return result;
    else
        return def;
}

GString dateTimeToStr(const GDateTime &datetime, GString format)
{
    if (datetime.time() == GTime(0, 0, 0))
        format = format.mid(0, format.indexOf(' '));//日期为整时 yyyy-MM-dd hh:mm:ss --->yyyy-MM-dd
    return datetime.toString(format);
}

GDateTime strToDateTime(const GString &s, GString format)
{
    if (s.indexOf(' ') < 0)
    {
        int nIndex = format.indexOf(' ');
        if (nIndex >= 0)
            format = format.mid(0, nIndex);
    }
    GDateTime result = GDateTime::fromString(s, format);
    if (!result.isValid())
    {
        result = GDateTime::fromString(s, Qt::ISODate);
    }
    return result;
}

GString byteArrayToStr(const GByteArray &a)
{
    return GString(a);
}

GByteArray strToByteArray(const GString &s)
{
    return s.toLocal8Bit();
}

GString intToHex(int value, int digits)
{
    GString val = GString::number(value, 16).toUpper();
    if (val.length() < digits)
        val.push_front('0');
    return val;
}

GString intToColorHex(int value, int length)
{
    GString val = GString::number(value, 16).toUpper();
    for (int i = val.length(); i < length; ++i)
    {
        val.push_front('0');
    }
    val.push_front('#');
    return val;
}

GString stuffString(const GString &text, int nStart, int nLength, const GString &subText)
{
    return copy(text, 0, nStart - 1).append(subText).append(copy(text, nStart + nLength - 1, MaxInt));
}

////////////////////////////////////////////////////////////////////////////////
//创建：Tu Jianhua 2004.1.18
//功能：计算分隔符在字符串中出现的次数
//参数：delimiter -- 分隔符
//     srcStr -- 字符串串
//返回：delimiter在字符串ASrcStr中出现的次数
////////////////////////////////////////////////////////////////////////////////
int occurs(const char delimiter, const GString &srcStr)
{
    int nCount = 0;
    for (int i = 0; i < srcStr.length(); ++i)
    {
        if (delimiter == srcStr[i].unicode())
        {
            nCount++;
        }
    }
    return nCount;
}

GByteArray base64Encode(const GByteArray &in)
{
    return in.toBase64();
}

GStream* base64Encode(GStream *in)
{
    if (!in)
    {
        return NULL;
    }
    return new GMemoryStream(new GByteArray(in->readAll().toBase64()), true);
}

GByteArray base64Decode(const GByteArray &in)
{
    return GByteArray::fromBase64(in);
}

GStream* base64Decode(GStream *in)
{
    if (!in)
    {
        return NULL;
    }
    return new GMemoryStream(new GByteArray(GByteArray::fromBase64(in->readAll())), true);
}

GString quotedStr(const GString &str, const char quote)
{
    GString result = str;
    GString quotes(2, quote);
    result.replace(quote, quotes);
    result.push_front(quote);
    result.push_back(quote);
    return result;
}

GByteArray quotedStr(const GByteArray &str, const char quote)
{
    GByteArray result = str;
    GByteArray quotes(2, quote);
    result.replace(quote, quotes);
    result.push_front(quote);
    result.push_back(quote);
    return result;
}

GString dequotedStr(const GString &str, const char quote)
{
    GString result = str;
    if (result[0] == quote)
        result.remove(0, 1);
    if (result[result.size() - 1] == quote)
        result.remove(result.size() - 1, 1);
    GString quotes(2, quote);
    result.replace(quotes, GString(1, quote));
    return result;
}

GString extractQuotedStr(GString &str, const char quote)
{
    str = dequotedStr(str, quote);
    return str;
}

GByteArray dequotedStr(const GByteArray &str, const char quote)
{
    GByteArray result = str;
    if (result[0] == quote)
        result.remove(0, 1);
    if (result[result.size() - 1] == quote)
        result.remove(result.size() - 1, 1);
    GByteArray quotes(2, quote);
    result.replace(quotes, GByteArray(1, quote));
    return result;
}

GStream* stringToStream(const GString &in)
{
    return new GMemoryStream(new GByteArray(in.toLocal8Bit()), true);
}

GString streamToString(GStream* in)
{
    if (!in)
    {
        return GString();
    }
    return GString(in->readAll());
}

GStream* byteArrayToStream(const GByteArray &in)
{
    if (0 == in.size())
    {
        return NULL;
    }
    return new GMemoryStream(new GByteArray(in), true);
}

GByteArray streamToByteArray(GStream* in)
{
    if (!in)
    {
        return GByteArray();
    }
    return in->readAll();
}

GDate intToDate(int d)
{
    return GDate(1899, 12, 30).addDays(d);
}

int dateToInt(const GDate &date)
{
    return GDate(1899, 12, 30).daysTo(date);
}

GTime doubleToTime(double t)
{
    t = frac(t);
    int nMsec = iround(t * double(c_MSecsPerDay));
    int nSecond = itrunc(double(nMsec) / double(c_MSecsPerSec));
    nMsec = nMsec % c_MSecsPerSec;
    int nMinute = itrunc(double(nSecond) / double(c_SecsPerMin));
    nSecond = nSecond % c_SecsPerMin;
    int nHour = itrunc(double(nMinute) / double(c_MinsPerHour));
    nMinute = nMinute % c_MinsPerHour;
    return GTime(nHour, nMinute, nSecond, nMsec);
}

double timeToDouble(const GTime &time)
{
    return (0.0 + ((time.hour() * c_MinsPerHour + time.minute()) * c_SecsPerMin + time.second())
            * c_MSecsPerSec + time.msec()) / c_MSecsPerDay;
}

GDateTime doubleToDateTime(double d)
{
    GDate date = intToDate(itrunc(d));
    GTime time = doubleToTime(d);
    return GDateTime(date, time);
}

double dateTimeToDouble(const GDateTime &dateTime)
{
    return dateToInt(dateTime.date()) + timeToDouble(dateTime.time());
}

int yearOf(double d)
{
    GDateTime dataTime = doubleToDateTime(d);
    return dataTime.date().year();
}

int monthOf(double d)
{
    GDateTime dataTime = doubleToDateTime(d);
    return dataTime.date().month();
}

int weekOf(double d)
{
    GDateTime dataTime = doubleToDateTime(d);
    return dataTime.date().weekNumber();
}

int dayOf(double d)
{
    GDateTime dataTime = doubleToDateTime(d);
    return dataTime.date().day();
}

int hourOf(double d)
{
    GTime time = doubleToTime(d);
    return time.hour();
}

int minuteOf(double d)
{
    GTime time = doubleToTime(d);
    return time.minute();
}

int secondOf(double d)
{
    GTime time = doubleToTime(d);
    return time.second();
}

int milliSecondOf(double d)
{
    GTime time = doubleToTime(d);
    return time.msec();
}

//// 返回中文平台下的时间日期格式
//// XP 的标准样式，和 VISTA 的不同
//GFormatSettings g_DefFormatSettings = {
//    0,
//    2,
//    ',',
//    '.',
//    2,
//    '-',
//    ':',
//    ',',
//    "￥", "yyyy-MM-dd",
//    "yyyy年M月d日",
//    "上午",
//    "下午",
//    "hh:mm",
//    "hh:mm:ss",
//    {
//        "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"
//    },
//    {
//        "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"
//    },
//    {
//        "日", "一", "二", "三", "四", "五", "六"
//    },
//    {
//        "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"
//    },
//    14180
//};

//// 非标准样式，使用 / 分隔时间
//GFormatSettings g_DefFormatSettings1 = {
//    0,
//    2,
//    ',',
//    '.',
//    2,
//    '/',
//    ':',
//    ',',
//    "￥",
//    "yyyy/MM/dd",
//    "yyyy年M月d日",
//    "上午",
//    "下午",
//    "hh:mm",
//    "hh:mm:ss",
//    {
//        "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"
//    },
//    {
//        "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"
//    },
//    {
//        "日", "一", "二", "三", "四", "五", "六"
//    },
//    {
//        "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"
//    },
//    14180,
//};

//// 非标准样式，使用 \ 分隔时间
//GFormatSettings g_DefFormatSettings2 = {
//    0,
//    2,
//    ',',
//    '.',
//    2,
//    '\\',
//    ':',
//    ',',
//    "￥",
//    "yyyy\\MM\\dd",
//    "yyyy年M月d日",
//    "上午",
//    "下午",
//    "hh:mm",
//    "hh:mm:ss",
//    {
//        "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"
//    },
//    {
//        "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"
//    },
//    {
//        "日", "一", "二", "三", "四", "五", "六"
//    },
//    {
//        "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"
//    },
//    14180,
//};

/*///////////////////////////////////////////////////////////////////////////////
//设计: Liuxd 2007.02.02
//功能: 使用中文平台时间日期格式进行类型转换
//参数：
//注意：同时兼容 -、/、\、三种时间分隔符
///////////////////////////////////////////////////////////////////////////////*/
double chsStrToDateTime(const GString &s)
{
//    GDateTime result;
    // todo
//    if ((!tryStrToDateTime(s, result, g_DefFormatSettings)) &&
//            (!tryStrToDateTime(s, result, g_DefFormatSettings1)) &&
//            (!tryStrToDateTime(s, result, g_DefFormatSettings2)) &&
//            (!tryStrToDateTime(s, result)))
//    {
//        gldErrorFmt(rsInvalidTimeFormat, s);
//    }
    GDateTime date = strToDateTime(s);
    return dateTimeToDouble(date);
}

GString dateTimeToChsStr(double dateTime)
{
    GDateTime date = doubleToDateTime(dateTime);
    return dateTimeToStr(date);
}

/*//////////////////////////////////////////////////////////////////////////////
//设计: Linw 2008.03.19
//功能: 判断是否是使用中文平台时间日期格式
//注意：同时兼容 -、/、\、三种时间分隔符
///////////////////////////////////////////////////////////////////////////////*/
bool isChsDateTime(const GString &s)
{
    GDateTime dValue;
    // todo duanb
    return true;
    G_UNUSED(s)
//    return tryStrToDateTime(s, dValue, g_DefFormatSettings) ||
//            tryStrToDateTime(s, dValue, g_DefFormatSettings1) ||
//            tryStrToDateTime(s, dValue, g_DefFormatSettings2);
}

bool tryStrToDateTime(const GString &s, GDateTime &value)
{
    Q_UNUSED(s);
    Q_UNUSED(value);
    // todo
//    int nPos = 1;
//    GDateTime date, time;
    bool result = true;
//Time := 0;
//    if not ScanDate(S, Pos, Date) or
//            not ((Pos > Length(S)) or ScanTime(S, Pos, Time)) then

//            // Try time only
//            Result := TryStrToTime(S, Value)
//      else
//      if Date >= 0 then
//      Value := Date + Time
//      else
//      Value := Date - Time;
    return result;
}

bool tryStrToDateTime(const GString &s, GDateTime &value,
  const GFormatSettings &formatSettings)
{
    Q_UNUSED(s);
    Q_UNUSED(value);
    Q_UNUSED(formatSettings);
    // todo
//    GDateTime date, time;
    bool result = true;
//    int nPos = 1;
    //  Time := 0;
    //  if not ScanDate(S, Pos, Date, FormatSettings) or
    //     not ((Pos > Length(S)) or ScanTime(S, Pos, Time, FormatSettings)) then

    //    // Try time only
    //    Result := TryStrToTime(S, Value, FormatSettings)
    //  else
    //    if Date >= 0 then
    //      Value := Date + Time
    //    else
    //      Value := Date - Time;
    return result;
}

bool sameDateTime(double dateTime1, double dateTime2)
{
    double dHoursPerDay   = 24;
    double dMinsPerHour   = 60;
    double dSecsPerMin    = 60;
    double dMSecsPerSec   = 1000;
    double dMinsPerDay    = dHoursPerDay * dMinsPerHour;
    double dSecsPerDay    = dMinsPerDay * dSecsPerMin;
    double dMSecsPerDay   = dSecsPerDay * dMSecsPerSec;
    double dOneMillisecond = 1.0 / dMSecsPerDay;
    return fabs(dateTime1 - dateTime2) < dOneMillisecond;
}

GLDValueRelationship compareDateTime(double dateTime1, double dateTime2)
{
    double dHoursPerDay   = 24;
    double dMinsPerHour   = 60;
    double dSecsPerMin    = 60;
    double dMSecsPerSec   = 1000;
    double dMinsPerDay    = dHoursPerDay * dMinsPerHour;
    double dSecsPerDay    = dMinsPerDay * dSecsPerMin;
    double dMSecsPerDay   = dSecsPerDay * dMSecsPerSec;
    double dOneMillisecond = 1.0 / dMSecsPerDay;
    if (fabs(dateTime1 - dateTime2) < dOneMillisecond)
        return gvrEqualsValue;
    else if (dateTime1 < dateTime2)
        return gvrLessThanValue;
    else
        return gvrGreaterThanValue;
}

GString getHZPY(const GString &src)
{
    unsigned char ucHigh;
    unsigned char ucLow;
    int nCode;
    GString result;
    GByteArray byteArray = unicodeToGBK(src);
    const char *str = byteArray.constData();
    for (int i = 0; i < byteArray.length(); ++i)
    {
        ucHigh = str[i];
        if (ucHigh < 0x80)//英文字母
        {
            result.append(str[i]);
            continue;
        }
        ucLow = str[i + 1];
        if ((ucHigh < 0xa1) || (ucLow < 0xa1))
        {
            continue;
        }
        else
        {
            nCode = (ucHigh - 0xa0) * 100 + ucLow - 0xa0;
        }
        result.append(firstLetter(nCode));
        ++i;
    }
    return result;
}

GString firstLetter(int nCode)
{
    if (nCode >= 1601 && nCode < 1637)
        return "A";
    if (nCode >= 1637 && nCode < 1833)
        return "B";
    if (nCode >= 1833 && nCode < 2078)
        return "C";
    if (nCode >= 2078 && nCode < 2274)
        return "D";
    if (nCode >= 2274 && nCode < 2302)
        return "E";
    if (nCode >= 2302 && nCode < 2433)
        return "F";
    if (nCode >= 2433 && nCode < 2594)
        return "G";
    if (nCode >= 2594 && nCode < 2787)
        return "H";
    if (nCode >= 2787 && nCode < 3106)
        return "J";
    if (nCode >= 3106 && nCode < 3212)
        return "K";
    if (nCode >= 3212 && nCode < 3472)
        return "L";
    if (nCode >= 3472 && nCode < 3635)
        return "M";
    if (nCode >= 3635 && nCode < 3722)
        return "N";
    if (nCode >= 3722 && nCode < 3730)
        return "O";
    if (nCode >= 3730 && nCode < 3858)
        return "P";
    if (nCode >= 3858 && nCode < 4027)
        return "Q";
    if (nCode >= 4027 && nCode < 4086)
        return "R";
    if (nCode >= 4086 && nCode < 4390)
        return "S";
    if (nCode >= 4390 && nCode < 4558)
        return "T";
    if (nCode >= 4558 && nCode < 4684)
        return "W";
    if (nCode >= 4684 && nCode < 4925)
        return "X";
    if (nCode >= 4925 && nCode < 5249)
        return "Y";
    if (nCode >= 5249 && nCode < 5590)
        return "Z";
    return "";
}

GString reverseString(const GString &s)
{
    GString result;
    for (int i = s.length() - 1; i >= 0; i--)
    {
        result.append(s[i]);
    }
    return result;
}

/*-------------------------------------------------------------------------------
设计：zhangjq 2012.10.31
参数：const AText,   源字符串
            ARegEx,  正则表达式
            AReplacement: 用来替换的字符串，支持反向引用 \1,\2??
功能：正则表达式替换函数
-------------------------------------------------------------------------------*/
GString regExprReplace(const GString &text, const GString &regEx, const GString &replacement)
{
    QRegExp regExp;
    regExp.setPattern(regEx);
    //将匹配设置为最小（短）匹配
    regExp.setMinimal(true);
    GString result = text;
    result.replace(regExp,replacement);
    return result;
}

int posN(const GString subStr, const GString srcStr, int times)
{
    int result = -1;
    int nOffSet = 0;
    int nSubLen = length(subStr);
    if (nSubLen == 1)
    {
        result = posN(subStr[0].toLatin1(), srcStr, times);
    }
    else
    {
        for (int i = 1; i <= times; i++)
        {
            nOffSet = srcStr.indexOf(subStr, nOffSet);
            if (nOffSet == -1)
                break;
            if (i == times)
                result = nOffSet;
            else
                nOffSet += nSubLen;
        }
    }
    return result;
}

int posN(const char delimiter, const GString srcStr, int times)
{
    int result = -1;
    int nCounter = 0;
    for (int i = 0; i < length(srcStr); i++)
    {
        if (srcStr[i] == delimiter)
        {
            nCounter++;
            if (nCounter == times)
            {
                result = i;
                break;
            }
        }
    }
    return result;
}

/*-------------------------------------------------------------------------------
创建：Tu Jianhua 2004.1.18
功能：根据指定的分隔符取指定项
参数：ASrcStr -- 字符串
      ADelimiter -- 分隔符字符
      AIndex -- 项号
返回：字符串ASrcStr以Delimiter为分隔符第AIndex项的子串
-------------------------------------------------------------------------------*/
GString getSubString(const GString &srcStr, char delimiter, int index)
{
    int nP1;
    int nP2;
    GString result = "";
    GString strText = srcStr;
    if ((index < 1) || (strText.length() == 0))
        return result;
    if (index == 1)
    {
        nP1 = posN(delimiter, strText, index);
        if (nP1 == -1)
            result = strText;
        else
            result = copy(strText, 0, nP1);
    }
    else
    {
        nP1 = posN(delimiter, strText, index - 1);
        if (nP1 == -1)
            return result;
        nP2 = strText.indexOf(delimiter, nP1 + 1);
        if (nP2 == -1)
            result = copy(strText, nP1 + 1);
        else
            result = copy(strText, nP1 + 1, nP2 - nP1 - 1);
    }
    return trim(result);
}

/*-------------------------------------------------------------------------------
创建：Tu Jianhua 2004.1.18
功能：根据指定的分隔符取指定项
参数：ASrcStr -- 字符串
      ADelimiter -- 分隔符串
      AIndex -- 项号
返回：字符串ASrcStr以Delimiter为分隔符第AIndex项的子串
-------------------------------------------------------------------------------*/
GString getSubString(const GString &srcStr, const GString &delimiter, int index)
{
    int nP1;
    int nP2;
    GString result = "";
    GString strText = srcStr;
    if ((index < 1) || (strText.length() == 0))
        return result;
    int nSize = length(delimiter);
    if (nSize == 0)
        return result;
    else if (nSize == 1)
        result = getSubString(strText, delimiter.at(0).toLatin1(), index);
    else
    {
        if (index == 1)
        {
            nP1 = posN(delimiter, strText, index);
            if (nP1 == -1)
                result = strText;
            else
                result = copy(strText, 0, nP1);
        }
        else
        {
            nP1 = posN(delimiter, strText, index - 1);
            if (nP1 == -1)
                return result;
            nP2 = strText.indexOf(delimiter, nP1 + nSize);
            if (nP2 == -1)
                result = copy(strText, nP1 + nSize);
            else
                result = copy(strText, nP1 + nSize, nP2 - nP1 - nSize);
        }
    }
    return trim(result);
}

GString stringOfChar(char Char, int count)
{
    GString result("");

    for (int i = 0; i < count; ++i)
    {
        result.append(Char);
    }
    return result;
}

GString createGuidString()
{
    GUuid oUuid;
    return oUuid.createUuid().toString();
}

GString boolToXMLString(bool v)
{
    if (v)
        return "True";
    else
        return "False";
}

bool xmlStringToBool(const GString &s)
{
    return sameText(s, "True");
}

GString floatToXMLString(double v)
{
    GString result = floatToStr(double(v));
    //todo Lipl
//    if (CurrentDecimalSeparator != '.')
//    {
//        int nPos = PosXMLString(CurrentDecimalSeparator, result);
//        if (nPos != 0)
//            result[nPos] = '.';
//    }
    return result;
}

GString encodeXMLString(const GString &value, bool encodeCrLf)
{
    int nLength = length(value);
    if (nLength == 0)
    {
        return value;
    }
    GString result = value;
    int nI = nLength - 1;
    while (nI >= 0)
    {
        switch (value[nI].toLatin1())
        {
        case '<':
            result.replace(nI, 1, "&lt;");
            break;
        case '>':
            result.replace(nI, 1, "&gt;");
            break;
        case '&':
            result.replace(nI, 1, "&amp;");
            break;
        case '\'':
            result.replace(nI, 1, "&apos;");
            break;
        case '"':
            result.replace(nI, 1, "&quot;");
            break;
        case '\n':
            if (encodeCrLf)
            {
                result.replace(nI, 1, "&#xA;");
            }
            break;
        case '\r':
            if (encodeCrLf)
            {
                result.replace(nI, 1, "&#xD;");
            }
            break;
        default:
            break;
        }
        --nI;
    }
    return result;
}

/**
 * GUID 操作函数
 */

GUID strToGUID(const GString &text)
{
#if defined(Q_OS_WIN)
    return GUID(GUuid(text));
#else
    return GUuidToGUID(GUuid(text));
#endif
}

GString GUIDToStr(const GUID &a)
{
#if defined(Q_OS_WIN)
    return GUuid(a).toString();
#else
    return GUIDToGUuid(a).toString();
#endif
}

bool isGUID(const GString &s)
{
    return !GUuid(s).isNull();
}

bool variantTypeIsGUID(const QVariant::Type type)
{
    return type == GVariant::Uuid;
}

GUuid GUIDToGUuid(const GUID &value)
{
#if defined(Q_OS_WIN)
    return GUuid(value).toString();
#else
    return QUuid(value.Data1, value.Data2, value.Data3, value.Data4[0], value.Data4[1], value.Data4[2], value.Data4[3],
            value.Data4[4], value.Data4[5], value.Data4[6], value.Data4[7]);
#endif
}

GUID GUuidToGUID(const GUuid &value)
{
#if defined(Q_OS_WIN)
    return GUID(value);
#else
    GUID result;
    result.Data1 = value.data1;
    result.Data2 = value.data2;
    result.Data3 = value.data3;
    result.Data4[0] = value.data4[0];
    result.Data4[1] = value.data4[1];
    result.Data4[2] = value.data4[2];
    result.Data4[3] = value.data4[3];
    result.Data4[4] = value.data4[4];
    result.Data4[5] = value.data4[5];
    result.Data4[6] = value.data4[6];
    result.Data4[7] = value.data4[7];
    return result;
#endif
}

int compareGUID(const GUID &g1, const GUID &g2)
{
    GUuid uuid1 = GUIDToGUuid(g1);
    GUuid uuid2 = GUIDToGUuid(g2);
    if (uuid1 > uuid2)
    {
        return 1;
    }
    else if (uuid1 < uuid2)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

GVariant GUIDToVariant(const GUID &a)
{
    return GVariant(GUIDToGUuid(a));
}

GUID variantToGUID(const GVariant &text)
{
    return GUuidToGUID(text.toUuid());
}

GUID byteArrayToGUID(const GByteArray &ba)
{
    return GUuidToGUID(GUuid(ba));
}

GByteArray GUIDToByteArray(const GUID value)
{
    return GUIDToGUuid(value).toByteArray();
}

GUID createGUID()
{
    GUuid uuid;
    return GUuidToGUID(uuid.createUuid());
}

bool isUInt64(const GString &s)
{
    bool result = false;
    s.toULongLong(&result);
    return result;
}

guint64 strToUInt64(const GString &s)
{
    bool bOK;
    guint64 result;
    result = s.toULongLong(&bOK);
    if (bOK)
    {
        return result;
    }
    else
    {
        throw GLDException(format(getGLDi18nStr(g_InvalidInteger), s));
    }
}

guint64 strToUInt64Def(const GString &s, guint64 def)
{
    bool bOk = false;
    guint64 result;
    result = s.toULongLong(&bOk);
    if (bOk)
    {
        return result;
    }
    else
    {
        return def;
    }
}

/*!
 * \brief 用于兼容delphi,引擎copy函数用
 * \param s
 * \param position
 * \param n
 * \return
 */
GString copyForDelphi(const GString &s, int position, int n)
{
    if (position < 0)
    {
        position = 0;
    }
    if (n < 0)
    {
        return "";
    }

    if (n == MaxInt)
    {
        n = -1;
    }
    return s.mid(position, n);
}

LONGLONG getStartTime()
{
#ifdef WIN32
    LARGE_INTEGER litmp;
    QueryPerformanceCounter(&litmp);
    return litmp.QuadPart;// 获得初始值
#else
    return 0;
#endif
}

double getLastTime(LONGLONG startTime)
{
#ifdef WIN32
    LARGE_INTEGER litmp;
    QueryPerformanceFrequency(&litmp);
    double dfFreq = (double)litmp.QuadPart;// 获得计数器的时钟频率

    QueryPerformanceCounter(&litmp);
    LONGLONG endTime = litmp.QuadPart;// 获得初始值
    double dfMinus = (double)(endTime - startTime) * 1000;
    double result = dfMinus / dfFreq;// 获得对应的时间值，单位为毫秒
    return result;
#else
    return 0;
#endif
}
