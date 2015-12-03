#ifndef GLDSTRUTILS_H
#define GLDSTRUTILS_H

#if defined(WIN32) || defined(WIN64)
#include "wtypes.h"
#endif

#include "GLDChar.h"
#include "GLDString.h"
#include "GLDDateTime.h"
#include "GLDByteArray.h"
#include "GLDUuid.h"
#include "GLDStringList.h"
#include "GLDVariantList.h"
#include "GLDIODevice.h"
#include "GLDSystem.h"
#include "GLDGlobal.h"
#include "GLDEnum.h"
#include "GLDResourceDef.h"

#if defined(WIN32) || defined(WIN64)
    extern GLDCOMMONSHARED_EXPORT const GString sLineBreak;
#else
    extern GLDCOMMONSHARED_EXPORT const GString sLineBreak;
#endif

/*
//获取C风格字符串（\0结尾的字符串）的长度
size_t CStrLength(const char *sz);
size_t CStrLength(const wchar_t *wsz);
size_t WideCStrLength(const wchar_t *wsz);
wchar_t *WideLowerCase(wchar_t **ppWsz);
//比较两个字符串，大小写敏感
bool SameStr(const char *szA, const char *szB);
bool WideSameStr(const wchar_t *wszA, const wchar_t *wszB);
//比较两个字符串，大小写不敏感
bool SameText(const char *szA, const char *szB);
bool WideSameText(const wchar_t *wszA, const wchar_t *wszB);

wchar_t *charToWchar(const char *str);
char *wcharToChar(const wchar_t *pwstr);
*/
GLDCOMMONSHARED_EXPORT GString utf8ToUnicode(const char *in, int length = -1);
GLDCOMMONSHARED_EXPORT GByteArray unicodeToUTF8(const GChar *in, int length = -1);
inline GByteArray unicodeToUTF8(const GString &in) { return unicodeToUTF8(in.constData(), in.length()); }

GLDCOMMONSHARED_EXPORT GString asciiToUnicode(const char *in, int length = -1, const char *name = NULL);
GLDCOMMONSHARED_EXPORT GByteArray unicodeToAscii(const GChar *in, int length = -1, const char *name = NULL);
inline GByteArray unicodeToAscii(const GString &in, const char *name = NULL) { return unicodeToAscii(in.constData(), in.length(), name); }

GLDCOMMONSHARED_EXPORT GString gbkToUnicode(const char *in, int length = -1);
GLDCOMMONSHARED_EXPORT GByteArray unicodeToGBK(const GChar *in, int length = -1);
inline GByteArray unicodeToGBK(const GString &in) { return unicodeToGBK(in.constData(), in.length()); }

GLDCOMMONSHARED_EXPORT GString ansiUpperCase(const GString &value);
GLDCOMMONSHARED_EXPORT GString ansiLowerCase(const GString &value);

GLDCOMMONSHARED_EXPORT GString upperCase(const GString &value);
GLDCOMMONSHARED_EXPORT GString lowerCase(const GString &value);

GLDCOMMONSHARED_EXPORT char upperCase(const char ch);
GLDCOMMONSHARED_EXPORT char lowerCase(const char ch);

GLDCOMMONSHARED_EXPORT wchar_t upperCase(const wchar_t ch);
GLDCOMMONSHARED_EXPORT wchar_t lowerCase(const wchar_t ch);

GLDCOMMONSHARED_EXPORT int compareStr(const GStringRef &s1, const GStringRef &s2);
GLDCOMMONSHARED_EXPORT int compareStr(const GStringRef &s1, const GString &s2);
GLDCOMMONSHARED_EXPORT int compareStr(const GString &s1, const GString &s2);
GLDCOMMONSHARED_EXPORT int compareStr(const GByteArray &s1, const GByteArray &s2);

GLDCOMMONSHARED_EXPORT int compareText(const GStringRef &s1, const GStringRef &s2);
GLDCOMMONSHARED_EXPORT int compareText(const GStringRef &s1, const GString &s2);
GLDCOMMONSHARED_EXPORT int compareText(const GString &s1, const GString &s2);
GLDCOMMONSHARED_EXPORT int compareText(const GByteArray &s1, const GByteArray &s2);

GLDCOMMONSHARED_EXPORT GLDValueRelationship compareGVariant(const GVariant &v1, const GVariant &v2);
GLDCOMMONSHARED_EXPORT bool sameGVariant(const GVariant &v1, const GVariant &v2);

GLDCOMMONSHARED_EXPORT bool sameStr(const GString &s1, const GString &s2);
// 不区分大小写
inline bool sameText(const GString &s1, const GString &s2)
{
    return 0 == s1.compare(s2, Qt::CaseInsensitive);
}
// 不区分大小写
inline bool sameText(const GString &s1, const GLatin1String &s2)
{
    return 0 == s1.compare(s2, Qt::CaseInsensitive);
}

// 不区分大小写
inline bool sameText(const GStringRef &s1, const GLatin1String &s2)
{
    return 0 == s1.compare(s2, Qt::CaseInsensitive);
}

GLDCOMMONSHARED_EXPORT int length(const GString &s);
GLDCOMMONSHARED_EXPORT int pos(const GString &subs, const GString &s);
GLDCOMMONSHARED_EXPORT int pos(const GChar &subs, const GString &s);
GLDCOMMONSHARED_EXPORT int pos(const GString &subs, const GString &s, int nFrom);
GLDCOMMONSHARED_EXPORT int rPos(const GString &subs, const GString &s, int times = 1);
GLDCOMMONSHARED_EXPORT int rPos(const GChar &subs, const GString &s, int times = 1);
GLDCOMMONSHARED_EXPORT int rPosEx(const GChar &subs, const GString &s, int offset);
GLDCOMMONSHARED_EXPORT GString trim(const GString &s);
GLDCOMMONSHARED_EXPORT GString trimRight(const GString &s);
GLDCOMMONSHARED_EXPORT GString trimLeft(const GString &s);
GLDCOMMONSHARED_EXPORT GString copy(const GString &s, int position, int n = -1);
GLDCOMMONSHARED_EXPORT GString copyForDelphi(const GString &s, int position, int n = MaxInt);
GLDCOMMONSHARED_EXPORT GString stringReplace(const GString &s, const GString &before, const GString &after);
GLDCOMMONSHARED_EXPORT bool containsText(const GString &text, const GString subText);
GLDCOMMONSHARED_EXPORT GString leftStr(const GString &text, int count);
GLDCOMMONSHARED_EXPORT GString rightStr(const GString &text, int count);
GLDCOMMONSHARED_EXPORT GStringList split(const GString &s, GChar sep);
GLDCOMMONSHARED_EXPORT GStringList split(const GString &s, const GString &sep);

GLDCOMMONSHARED_EXPORT bool isInt(const GString &s);
GLDCOMMONSHARED_EXPORT bool isInt64(const GString &s);
GLDCOMMONSHARED_EXPORT bool isUInt64(const GString &s);
GLDCOMMONSHARED_EXPORT bool isNumeric(const GString &s);
GLDCOMMONSHARED_EXPORT bool isDateTime(const GString &s);

GLDCOMMONSHARED_EXPORT bool charIsDigit(const GChar &ch);

GLDCOMMONSHARED_EXPORT bool variantTypeIsByte(const GVariant::Type type);
GLDCOMMONSHARED_EXPORT bool variantTypeIsShort(const GVariant::Type type);
GLDCOMMONSHARED_EXPORT bool variantTypeIsInt(const GVariant::Type type);
inline bool variantTypeIsInt64(const GVariant::Type type)
{ return (type == GVariant::LongLong || type == GVariant::ULongLong); }
GLDCOMMONSHARED_EXPORT bool variantTypeIsDigit(const GVariant::Type type);
GLDCOMMONSHARED_EXPORT bool variantTypeIsFloat(const GVariant::Type type);
GLDCOMMONSHARED_EXPORT bool variantTypeIsNumeric(const GVariant::Type type);
GLDCOMMONSHARED_EXPORT bool variantTypeIsDateTime(const GVariant::Type type);
GLDCOMMONSHARED_EXPORT bool variantTypeIsUnsigned(const GVariant::Type type);

#ifdef WIN32
GLDCOMMONSHARED_EXPORT GString BSTRToGString(const BSTR & s);
GLDCOMMONSHARED_EXPORT BSTR GStringToBSTR(const GString & s);
GLDCOMMONSHARED_EXPORT void freeBSTR(BSTR & s);
#endif

GLDCOMMONSHARED_EXPORT GString format(const GString &s, const GVariantList &a);
GLDCOMMONSHARED_EXPORT GString format(const GString &s, const GString &a, const GString &before = GString("%s"));
GLDCOMMONSHARED_EXPORT GString format(const GString &s, int a);
GLDCOMMONSHARED_EXPORT GString format(const GString &s, long a);
GLDCOMMONSHARED_EXPORT GString format(const GString &s, long long a);
GLDCOMMONSHARED_EXPORT GString format(const GString &s, double a);
GLDCOMMONSHARED_EXPORT GString format(const GString &s, const GChar &a, const GString &before = GString("%s"));
//GString format(const GString &s, const GVariant &a);

GLDCOMMONSHARED_EXPORT GString boolToStr(bool a, bool useBoolStrs = false);
GLDCOMMONSHARED_EXPORT bool strToBool(const GString &s);
GLDCOMMONSHARED_EXPORT bool strToBoolDef(const GString &s, bool def = false);
GLDCOMMONSHARED_EXPORT GString intToStr(int a);
GLDCOMMONSHARED_EXPORT GString int64ToStr(gint64 a);
GLDCOMMONSHARED_EXPORT GString uint64ToStr(guint64 a);
GLDCOMMONSHARED_EXPORT int strToInt(const GString &s);
GLDCOMMONSHARED_EXPORT int strToIntDef(const GString &s, int def);
GLDCOMMONSHARED_EXPORT gint64 strToInt64(const GString &s, int base = 10);
GLDCOMMONSHARED_EXPORT gint64 strToInt64Def(const GString &s, gint64 def);
GLDCOMMONSHARED_EXPORT guint64 strToUInt64(const GString &s);
GLDCOMMONSHARED_EXPORT guint64 strToUInt64Def(const GString &s, guint64 def);
GLDCOMMONSHARED_EXPORT GString floatToStr(double a);
GLDCOMMONSHARED_EXPORT double strToFloat(const GString &s);
GLDCOMMONSHARED_EXPORT double strToFloatDef(const GString &s, double def);
GLDCOMMONSHARED_EXPORT GString dateTimeToStr(const GDateTime &datetime, GString format = "yyyy-MM-dd hh:mm:ss");
GLDCOMMONSHARED_EXPORT GDateTime strToDateTime(const GString &s, GString format = "yyyy-M-d hh:mm:ss");
GLDCOMMONSHARED_EXPORT GString byteArrayToStr(const GByteArray &a);
GLDCOMMONSHARED_EXPORT GByteArray strToByteArray(const GString &s);
GLDCOMMONSHARED_EXPORT GString intToHex(int value, int digits);

GLDCOMMONSHARED_EXPORT GString stuffString(const GString &text, int nStart, int nLength, const GString &subText);
GLDCOMMONSHARED_EXPORT int occurs(const char delimiter, const GString &srcStr);

GLDCOMMONSHARED_EXPORT GByteArray base64Encode(const GByteArray &in);
GLDCOMMONSHARED_EXPORT GStream* base64Encode(GStream *in);

GLDCOMMONSHARED_EXPORT GByteArray base64Decode(const GByteArray &in);
GLDCOMMONSHARED_EXPORT GStream* base64Decode(GStream *in);

GLDCOMMONSHARED_EXPORT GString quotedStr(const GString &str, const char quote = '\'');
GLDCOMMONSHARED_EXPORT GByteArray quotedStr(const GByteArray &str, const char quote);

GLDCOMMONSHARED_EXPORT GString dequotedStr(const GString &str, const char quote);
GLDCOMMONSHARED_EXPORT GByteArray dequotedStr(const GByteArray &str, const char quote);

GLDCOMMONSHARED_EXPORT GString extractQuotedStr(GString &str, const char quote);

GLDCOMMONSHARED_EXPORT GStream* stringToStream(const GString &in);
GLDCOMMONSHARED_EXPORT GString streamToString(GStream* in);

GLDCOMMONSHARED_EXPORT GStream* byteArrayToStream(const GByteArray &in);
GLDCOMMONSHARED_EXPORT GByteArray streamToByteArray(GStream* in);

GLDCOMMONSHARED_EXPORT GDate intToDate(int d);
GLDCOMMONSHARED_EXPORT int dateToInt(const GDate &date);

GLDCOMMONSHARED_EXPORT GTime doubleToTime(double t);
GLDCOMMONSHARED_EXPORT double timeToDouble(const GTime &time);

GLDCOMMONSHARED_EXPORT GDateTime doubleToDateTime(double d);
GLDCOMMONSHARED_EXPORT double dateTimeToDouble(const GDateTime &dateTime);

GLDCOMMONSHARED_EXPORT GString intToColorHex(int value, int length = 6);

GLDCOMMONSHARED_EXPORT int yearOf(double d);
GLDCOMMONSHARED_EXPORT int monthOf(double d);
GLDCOMMONSHARED_EXPORT int weekOf(double d);
GLDCOMMONSHARED_EXPORT int dayOf(double d);
GLDCOMMONSHARED_EXPORT int hourOf(double d);
GLDCOMMONSHARED_EXPORT int minuteOf(double d);
GLDCOMMONSHARED_EXPORT int secondOf(double d);
GLDCOMMONSHARED_EXPORT int milliSecondOf(double d);

struct GFormatSettings
{
    byte currencyFormat;
    byte negCurrFormat;
    char thousandSeparator;
    char decimalSeparator;
    byte currencyDecimals;
    char dateSeparator;
    char timeSeparator;
    char listSeparator;
    GString currencyString;
    GString shortDateFormat;
    GString longDateFormat;
    GString timeAMString;
    GString timePMString;
    GString shortTimeFormat;
    GString longTimeFormat;
    GString shortMonthNames[12];
    GString longMonthNames[12];
    GString shortDayNames[7];
    GString longDayNames[7];
    int twoDigitYearCenturyWindow;
};

GLDCOMMONSHARED_EXPORT double chsStrToDateTime(const GString &s);
GLDCOMMONSHARED_EXPORT GString dateTimeToChsStr(double dateTime);
GLDCOMMONSHARED_EXPORT bool isChsDateTime(const GString &s);
GLDCOMMONSHARED_EXPORT bool tryStrToDateTime(const GString &s, GDateTime &value);
GLDCOMMONSHARED_EXPORT bool tryStrToDateTime(const GString &s, GDateTime &value,
                 const GFormatSettings &formatSettings);

GLDCOMMONSHARED_EXPORT bool sameDateTime(double dateTime1, double dateTime2);
GLDCOMMONSHARED_EXPORT GLDValueRelationship compareDateTime(double dateTime1, double dateTime2);

GLDCOMMONSHARED_EXPORT GString getHZPY(const GString &src);
GLDCOMMONSHARED_EXPORT GString firstLetter(int nCode);
GLDCOMMONSHARED_EXPORT GString reverseString(const GString &s);
GLDCOMMONSHARED_EXPORT GString regExprReplace(const GString &text, const GString &regEx, const GString &replacement);

GLDCOMMONSHARED_EXPORT GString getSubString(const GString &srcStr, char delimiter, int index);
GLDCOMMONSHARED_EXPORT GString getSubString(const GString &srcStr, const GString &delimiter, int index);

GLDCOMMONSHARED_EXPORT int posN(const char delimiter, const GString srcStr, int times = 1);
GLDCOMMONSHARED_EXPORT int posN(const GString subStr, const GString srcStr, int times = 1);

GLDCOMMONSHARED_EXPORT GString stringOfChar(char Char, int count);


GLDCOMMONSHARED_EXPORT GString boolToXMLString(bool v);
GLDCOMMONSHARED_EXPORT bool xmlStringToBool(const GString &s);
GLDCOMMONSHARED_EXPORT GString floatToXMLString(double v);
GLDCOMMONSHARED_EXPORT GString encodeXMLString(const GString &value, bool encodeCrLf);

/**
  GUID 相关处理函数
*/
#include "GLDGuidDef.h"
GLDCOMMONSHARED_EXPORT GString createGuidString();
GLDCOMMONSHARED_EXPORT GUID createGUID();
GLDCOMMONSHARED_EXPORT GString GUIDToStr(const GUID &a);
GLDCOMMONSHARED_EXPORT GUID strToGUID(const GString &text);
GLDCOMMONSHARED_EXPORT GVariant GUIDToVariant(const GUID &a);
GLDCOMMONSHARED_EXPORT GUID variantToGUID(const GVariant &text);
GLDCOMMONSHARED_EXPORT GByteArray GUIDToByteArray(const GUID value);
GLDCOMMONSHARED_EXPORT GUID byteArrayToGUID(const GByteArray &ba);
GLDCOMMONSHARED_EXPORT int compareGUID(const GUID &g1, const GUID &g2);
GLDCOMMONSHARED_EXPORT bool isGUID(const GString &s);
GLDCOMMONSHARED_EXPORT bool variantTypeIsGUID(const GVariant::Type type);
GLDCOMMONSHARED_EXPORT GUuid GUIDToGUuid(const GUID &value);
GLDCOMMONSHARED_EXPORT GUID GUuidToGUID(const GUuid &value);

template <typename Set, typename T>
inline void include(Set &set, const T t)
{
    set |= (byte(1) << byte(t));
}

template <typename Set, typename T>
inline void exclude(Set &set, const T t)
{
    set &= ~(byte(1) << byte(t));
}

template <typename Set, typename T>
inline bool contains(const Set &set, const T t)
{
    return set & (byte(1) << byte(t));
}

//type:0(), 1(], 2[), 3[]
template <typename T>
Q_DECL_CONSTEXPR inline bool isInInterval( const T &val, const T &min, const T &max, int type = 3)
{
    return ((1 == (type & 1)) ? (val <= max) : (val < max))
            && ((2 == (type & 2)) ? (val >= min) : (val > min));
}

LONGLONG getStartTime();
double getLastTime(LONGLONG startTime);

#endif // GLDSTRUTILS_H

