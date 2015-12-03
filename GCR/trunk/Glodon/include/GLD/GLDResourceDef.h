#ifndef GLDRESOURCEDEF
#define GLDRESOURCEDEF
#include "GLDWidget_Global.h"

#define DEF_RESOURCESTRING(name, value) extern const GString name
#define DEF_RESOURCESTRING2(name) extern const char *name
#define DEF_RESOURCESTRING3(name, value) extern const char *name
#define DEF_RESOURCESTRING4(name) extern const GString name
#define DEF_RESOURCESTRING5(name) extern GLDCOMMONSHARED_EXPORT const char *name
//#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
//#   define TRANS_STRING(value) GString::fromLocal8Bit(value)
//#else
#   if defined(WIN32) || defined(WIN64)
#       define GStringLiteral(str) QStringLiteral(str)
#   elif defined(__APPLE__)
#       define GStringLiteral(str) gbkToUnicode(str)
#   else
#       define GStringLiteral(str) gbkToUnicode(str)
#   endif //WIN32
#   define TRANS_STRING(value) GStringLiteral(value)
//#endif

//#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
//#   define DEFINE_RESOURCESTRING(name, value) const GString name = GString::fromLocal8Bit(value)
//#else
#   define DEFINE_RESOURCESTRING(name, value) const GString name = TRANS_STRING(value)
//#endif

#define DEF_CONSTSTRING(name, value) extern const GString name
//#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
//#   define DEFINE_CONSTSTRING(name, value) static GString name = GString::fromLocal8Bit(value)
//#else
#   define DEFINE_CONSTSTRING(name, value) const GString name = TRANS_STRING(value)
//#endif

//#ifdef QT_NO_DEPRECATED
//#  define GLD_DECLARE_DEPRECATED_TR_FUNCTIONS(context)
//#else
#  define GLD_DECLARE_DEPRECATED_TR_FUNCTIONS(context) \
    QT_DEPRECATED static inline GString trUtf8(const char *sourceText, const char *disambiguation = 0, int n = -1) \
        { return QCoreApplication::translate(#context, sourceText, disambiguation, n); }
//#endif

#define GLD_DECLARE_TR_FUNCTIONS(context) \
public: \
    static inline GString tr(const char *sourceText, const char *disambiguation = 0, int n = -1) \
        { return QCoreApplication::translate(#context, sourceText, disambiguation, n); } \
    GLD_DECLARE_DEPRECATED_TR_FUNCTIONS(context) \
private:

#endif // GLDRESOURCEDEF

