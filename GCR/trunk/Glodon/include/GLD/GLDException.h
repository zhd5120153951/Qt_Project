#ifndef GLDEXCEPTION_H
#define GLDEXCEPTION_H

#include <stdexcept>
#include "GLDStrUtils.h"

#ifdef __APPLE__
#   define _GLIBCXX_USE_NOEXCEPT throw()
#endif
#ifdef _MSC_VER
#   define _GLIBCXX_USE_NOEXCEPT
#else
#   define __CLR_OR_THIS_CALL
#   define _THROW0()
#endif

class GLDException : public std::exception
{
public:
    __CLR_OR_THIS_CALL GLDException() _THROW0()
        : exception(), m_errorCode(-1)
    {
    }

    __CLR_OR_THIS_CALL GLDException(const GString &message) _THROW0()
        : exception(), m_message(message), m_errorCode(-1)
    {
    }

    __CLR_OR_THIS_CALL GLDException(const GString &message, int errorCode) _THROW0()
        : exception(), m_message(message), m_errorCode(errorCode)
    {
    }

    virtual ~GLDException() _GLIBCXX_USE_NOEXCEPT {}

public:
    inline GString message() const { return m_message; }
    inline void setMessage(const GString &value) { m_message = value; }
    inline int errorCode() const { return m_errorCode; }
    inline void setErrorCode(int value) { m_errorCode = value; }

private:
    GString m_message;
    int m_errorCode;
};

class GLDCOMMONSHARED_EXPORT GLDFieldSettingException : public GLDException
{
public:
    __CLR_OR_THIS_CALL GLDFieldSettingException(const GString &message, int tableIndex, int col, int errorCode) _THROW0()
        : GLDException(message, errorCode), m_tableIndex(tableIndex), m_col(col)
    {
    }

private:
    int m_tableIndex;
    int m_col;
};

typedef GLDFieldSettingException GLDFieldNameException;
typedef GLDFieldSettingException GLDSaveFieldNameException;
typedef GLDException GLDColSettingException;

inline void gldError(const GString &message, int errorCode = -1)
{
    throw GLDException(message, errorCode);
}

inline void gldErrorFmt(const GString &message, const GString &a, int errorCode = -1)
{
    throw GLDException(format(message, a), errorCode);
}

inline void gldErrorFmt(const GString &message, int a, int errorCode = -1)
{
    throw GLDException(format(message, a), errorCode);
}

inline void gldControlError(const GString &message, int errorCode = -1)
{
    throw GLDException(message, errorCode);
}

inline void gldFieldNameError(const GString &message, int tableIndex, int col, int errorCode = -1)
{
    throw GLDFieldNameException(message, tableIndex, col, errorCode);
}

inline void gldSaveFieldNameError(const GString &message, int tableIndex, int col, int errorCode = -1)
{
    throw GLDSaveFieldNameException(message, tableIndex, col, errorCode);
}

#endif // GLDEXCEPTION_H
