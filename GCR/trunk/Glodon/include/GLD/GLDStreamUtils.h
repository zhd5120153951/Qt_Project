/**********************************************************************
 *
 *  广联达 streamhelper IStream流辅助单元
 *
 *  设计：Linw 2012.03.31
 *  备注：
 *  审核：
 *  注意：
 *
 *  Copyright (c) 1994-2012 GlodonSoft Corporation
 *
 **********************************************************************/

#ifndef GLDSTREAMHELPER_HPP
#define GLDSTREAMHELPER_HPP

#ifdef WIN32
#   include "objidl.h"
#endif
#include "GLDIODevice.h"
#include "GLDString.h"
#include "GLDStream.h"
#include "GLDVariant.h"
#include "GLDUnknwn.h"

// 基本数据类型
enum GBaseDataType
{
    gbdtUnknown,                         // 未知 0
    gbdtInteger,                         // 整数 1
    gbdtFloat,                           // 浮点数 2
    gbdtString,                          // 字符串 3
    gbdtBoolean,                         // 布尔
    gbdtDateTime,                        // 日期时间
    gbdtTable,                           // 嵌套表
    gbdtInt64,                           // 长整数
    gbdtBlob,                            // 二进制OLE
    gbdtGUID,                            // GUID
    gbdtUInt64                           // 无符号长整形
};

class GStreamImplPrivate;
class GLDCOMMONSHARED_EXPORT GStreamImpl : public GStream
{
public:
    explicit GStreamImpl(IStream *stream = NULL, GStream::OpenMode flags = GStream::ReadWrite);
    virtual ~GStreamImpl();
    gint64 size() const;
    gint64 pos() const;
    bool seek(gint64 off);
protected:
    gint64 readData(char *data, gint64 maxlen);
    gint64 writeData(const char *data, gint64 len);
public:
    IStream *streamIntf() const;

private:
    Q_DECLARE_PRIVATE(GStreamImpl)
};

class GIStreamImplPrivate;
class GLDCOMMONSHARED_EXPORT GIStreamImpl : public GInterfaceObject, public IStream
{
public:
    GIStreamImpl(GStream *stream = NULL);
    virtual ~GIStreamImpl();
public:
    DECLARE_IUNKNOWN

    virtual HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead);

    virtual HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten);

    virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin,
                                           ULARGE_INTEGER *plibNewPosition);

    virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize);

    virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm, ULARGE_INTEGER cb,
                                             ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);

    virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);

    virtual HRESULT STDMETHODCALLTYPE Revert();

    virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
                                                 DWORD dwLockType);

    virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
                                                   DWORD dwLockType);

    virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag);

    virtual HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm);
protected:
    HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject);
public :
    GStream *stream() const;

private:
    GIStreamImplPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GIStreamImpl)
};

GLDCOMMONSHARED_EXPORT long long streamLength(IStream *stream);

GLDCOMMONSHARED_EXPORT GString readFromStream(GStream *stream, uint length, bool useUnicode = false);
GLDCOMMONSHARED_EXPORT GByteArray readByteArrayFromStream(GStream *stream, uint length);
GLDCOMMONSHARED_EXPORT void readByteArrayFromStream(GStream *stream, GByteArray &value);
GLDCOMMONSHARED_EXPORT GByteArray readByteArrayFromStream(GStream *stream);

GLDCOMMONSHARED_EXPORT void readStrFromStream(GStream *stream, GString &value, bool useUnicode = false);
GLDCOMMONSHARED_EXPORT void readMemoFromStream(GStream *stream, GString &value, bool useUnicode = false);
GLDCOMMONSHARED_EXPORT void readStreamFromStream(GStream *src, GStream *dst, unsigned int length);
GLDCOMMONSHARED_EXPORT unsigned int readStreamFromStream(GStream *src, GStream *dst);

inline void readBoolFromStream(GStream *stream, bool &value)
{
    stream->read((char*)(&value), sizeof(bool));
}

inline void readByteFromStream(GStream *stream, unsigned char &value)
{
    stream->read((char*)(&value), sizeof(unsigned char));
}

inline void readWordFromStream(GStream *stream, unsigned short &value)
{
    stream->read((char*)(&value), sizeof(unsigned short));
}

inline void readUIntFromStream(GStream *stream, unsigned int &value)
{
    stream->read((char*)(&value), sizeof(unsigned int));
}

inline void readIntFromStream(GStream *stream, int &value)
{
    stream->read((char*)(&value), sizeof(int));
}

inline void readDoubleFromStream(GStream *stream, double &value)
{
    stream->read((char*)(&value), sizeof(double));
}

GLDCOMMONSHARED_EXPORT void readColorFromStream(GStream *stream, TColor &color);

inline void readInt64FromStream(GStream *stream, long long &value)
{
    stream->read((char*)(&value), sizeof(long long));
}

inline void readUInt64FromStream(GStream *stream, guint64 &value)
{
    stream->read((char*)(&value), sizeof(guint64));
}

GLDCOMMONSHARED_EXPORT void readGUIDFromStream(GStream *stream, GUID &value);

inline void readCharFromStream(GStream *stream, signed char &value)
{
    stream->read((char*)(&value), sizeof(signed char));
}

inline void readShortFromStream(GStream *stream, short &value)
{
    stream->read((char*)(&value), sizeof(short));
}

GLDCOMMONSHARED_EXPORT GString readStrFromStream(GStream *stream, bool useUnicode = false);
GLDCOMMONSHARED_EXPORT GString readMemoFromStream(GStream *stream, bool useUnicode = false);

inline bool readBoolFromStream(GStream *stream)
{
    bool result;
    readBoolFromStream(stream, result);
    return result;
}

inline unsigned char readByteFromStream(GStream *stream)
{
    unsigned char ucResult(0);
    readByteFromStream(stream, ucResult);
    return ucResult;
}

inline unsigned short readWordFromStream(GStream *stream)
{
    unsigned short uResult(0);
    readWordFromStream(stream, uResult);
    return uResult;
}

inline uint readUIntFromStream(GStream *stream)
{
    uint result(0);
    readUIntFromStream(stream, result);
    return result;
}

inline int readIntFromStream(GStream *stream)
{
    int result = 0;
    readIntFromStream(stream, result);
    return result;
}

GLDCOMMONSHARED_EXPORT TColor readColorFromStream(GStream *stream);

inline double readDoubleFromStream(GStream *stream)
{
    double result;
    readDoubleFromStream(stream, result);
    return result;
}

GLDCOMMONSHARED_EXPORT GVariant readGVariantFromStream(GStream *stream);
GLDCOMMONSHARED_EXPORT void readGVariantFromStream(GStream *stream, GVariant &result);

GLDCOMMONSHARED_EXPORT void writeGVariantToStream(GStream *stream, const GVariant &value);

inline gint64 readInt64FromStream(GStream *stream)
{
    long long result;
    readInt64FromStream(stream, result);
    return result;
}

inline guint64 readUInt64FromStream(GStream *stream)
{
    guint64 result(0);
    readUInt64FromStream(stream, result);
    return result;
}

inline GUID readGUIDFromStream(GStream *stream)
{
    GUID result;
    readGUIDFromStream(stream, result);
    return result;
}

inline char readCharFromStream(GStream *stream)
{
    signed char result(0);
    readCharFromStream(stream, result);
    return result;
}

inline short readShortFromStream(GStream *stream)
{
    short result;
    readShortFromStream(stream, result);
    return result;
}

GLDCOMMONSHARED_EXPORT void writeByteArrayToStream(GStream *stream, const GByteArray &value);
GLDCOMMONSHARED_EXPORT void writeByteArrayToStream(GStream *stream, const char *value, int size);
GLDCOMMONSHARED_EXPORT void writeStrToStream(GStream *stream, const GString &value, bool useUnicode = false);
GLDCOMMONSHARED_EXPORT void writeStrToStream(GStream *stream, const char *value, int size, bool useUnicode = false);
GLDCOMMONSHARED_EXPORT void writeMemoToStream(GStream *stream, const char *value, int size, bool useUnicode = false);
GLDCOMMONSHARED_EXPORT void writeMemoToStream(GStream *stream, const GString &value, bool useUnicode = false);

inline void writeBoolToStream(GStream *stream, const bool value)
{
    stream->write((const char *)(&value), sizeof(bool));
}

inline void writeByteToStream(GStream *stream, const unsigned char value)
{
    stream->write((const char *)(&value), sizeof(unsigned char));
}

inline void writeWordToStream(GStream *stream, const unsigned short value)
{
    stream->write((const char *)(&value), sizeof(unsigned short));
}

inline void writeUIntToStream(GStream *stream, const unsigned int value)
{
    stream->write((const char *)(&value), sizeof(unsigned int));
}

inline void writeIntToStream(GStream *stream, const int value)
{
    stream->write((const char *)(&value), sizeof(int));
}

inline void writeDoubleToStream(GStream *stream, const double value)
{
    stream->write((const char *)(&value), sizeof(double));
}

inline void writeInt64ToStream(GStream *stream, const long long value)
{
    stream->write((const char *)(&value), sizeof(long long));
}

inline void writeUInt64ToStream(GStream *stream, const guint64 value)
{
    stream->write((const char *)(&value), sizeof(guint64));
}

GLDCOMMONSHARED_EXPORT void writeGUIDToStream(GStream *stream, GUID value);

inline void writeCharToStream(GStream *stream, const char value)
{
    stream->write((const char *)(&value), sizeof(char));
}

inline void writeShortToStream(GStream *stream, const short value)
{
    stream->write((const char *)(&value), sizeof(short));
}

GLDCOMMONSHARED_EXPORT void writeColorToStream(GStream *stream, const TColor color);

GLDCOMMONSHARED_EXPORT GMemoryStream *createMemoryStream();

GLDCOMMONSHARED_EXPORT void copyStream(GStream *dst, GStream *src);
#endif    /* GLDSTREAMHELPER_HPP */
