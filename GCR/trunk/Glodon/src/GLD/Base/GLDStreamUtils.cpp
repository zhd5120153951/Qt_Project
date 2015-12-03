#include "GLDStreamUtils.h"

#include <QtEndian>
#include "GLDGlobal.h"
#include "GLDStrUtils.h"
#include "private/qiodevice_p.h"

const unsigned char c_GCharSize = 2;

long long streamLength(IStream *stream)
{
    long long result;
    LARGE_INTEGER lMov;
    lMov.QuadPart = 0;
    ULARGE_INTEGER ulEnd;
    ULARGE_INTEGER ulBegin;
    ULARGE_INTEGER ulPos;
    // 记录当前位置
    stream->Seek(lMov, STREAM_SEEK_CUR, &ulPos);
    try
    {
        stream->Seek(lMov, STREAM_SEEK_END, &ulEnd);
        stream->Seek(lMov, STREAM_SEEK_SET, &ulBegin);
        result = ulEnd.QuadPart - ulBegin.QuadPart;
    }
    catch (...)
    {
        lMov.QuadPart = ulPos.QuadPart;
        stream->Seek(lMov, STREAM_SEEK_SET, &ulPos);
        throw;
    }
    lMov.QuadPart = ulPos.QuadPart;
    stream->Seek(lMov, STREAM_SEEK_SET, &ulPos);
    return result;
}

GByteArray readByteArrayFromStream(GStream *stream, char *buf, uint length)
{
    stream->read(buf, length);
    buf[length] = '\0';
    return GByteArray(buf, length);
}

GByteArray readByteArrayFromStream(GStream *stream, uint length)
{
    GByteArray result;
    if (0 != length)
    {
        char *buf = new char[length + 1];
        try
        {
            result = readByteArrayFromStream(stream, buf, length);
        }
        catch (...)
        {
            delete buf;
            throw;
        }
        delete buf;
    }
    return result;
}

GString readFromStream(GStream *stream, uint length, bool useUnicode)
{
    if (0 == length)
        return GString();
    if (useUnicode)
    {
        GString result(length, Qt::Uninitialized);
        char *buf = (char *)(result.constData());
        stream->read(buf, length * c_GCharSize);
        return result;
    }
    else
    {
        GString result;
        if (0 != length)
        {
            char *buf = new char[length + 1];
            try
            {
                stream->read(buf, length);
                buf[length] = '\0';
                result = gbkToUnicode(buf, length);
            }
            catch (...)
            {
                delete buf;
                throw;
            }
            delete buf;
        }
        return result;
    }
}

// =====================================================
// 从流中读出一个字符串
// 参数: pStream 保存创建的流
//      value 字符串
// 返回值:
// 备注：按GSP存字符串留格式，每个字符串由长度，加字符串本身构成
// =====================================================
void readStrFromStream(GStream *stream, GString &value, bool useUnicode)
{
    if (useUnicode)
    {
        unsigned int ucInts = 0;
        readUIntFromStream(stream, ucInts);
        value = readFromStream(stream, ucInts, useUnicode);
    }
    else
    {
        unsigned char ucInts = 0;
        readByteFromStream(stream, ucInts);
        value = readFromStream(stream, ucInts, useUnicode);
    }
}

void readByteArrayFromStream(GStream *stream, GByteArray &value)
{
    uint uInts = 0;
    readUIntFromStream(stream, uInts);
    value = readByteArrayFromStream(stream, uInts);
}

GByteArray readByteArrayFromStream(GStream *stream)
{
    uint uInts = 0;
    readUIntFromStream(stream, uInts);
    GByteArray value = readByteArrayFromStream(stream, uInts);
    return value;
}

void readMemoFromStream(GStream *stream, GString &value, bool useUnicode)
{
    uint uInts = 0;
    readUIntFromStream(stream, uInts);
    value = readFromStream(stream, uInts, useUnicode);
}

void readStreamFromStream(GStream *src, GStream *dst, unsigned int length)
{
    // todo 分块
    char *buf = new char[length];
    src->read(buf, length);
    dst->write(buf, length);
    delete buf;
}

unsigned int readStreamFromStream(GStream *src, GStream *dst)
{
    unsigned int uLength(0);
    readUIntFromStream(src, uLength);
    if (0 != uLength)
    {
        readStreamFromStream(src, dst, uLength);
    }
    return uLength;
}

#define GBSWAP(value) value
#define GBSWAPVALUE
//#define GBSWAP(value) qbswap(value)
//#define GBSWAPVALUE value = GBSWAP(value);

void readColorFromStream(GStream *stream, TColor &color)
{
    int nValue;
    //转用整形操作是为了同DELPHI产生的流兼容
    readIntFromStream(stream, nValue);
    color = (TColor)nValue;
}

GString readStrFromStream(GStream *stream, bool useUnicode)
{
    if (useUnicode)
    {
        int nLength = 0;
        readIntFromStream(stream, nLength);
        if (nLength > 0)
        {
            GString result(nLength, Qt::Uninitialized);
            char *buf = (char *)(result.constData());
            stream->read(buf, nLength * c_GCharSize);
            return result;
        }
        else
            return GString();
    }
    else
    {
        unsigned char ucN = 0;
        readByteFromStream(stream, ucN);
        return readFromStream(stream, ucN, useUnicode);
    }
}

GString readMemoFromStream(GStream *stream, bool useUnicode)
{
    GString result;
    readMemoFromStream(stream, result, useUnicode);
    return result;
}

TColor readColorFromStream(GStream *stream)
{
    TColor result(0);
    readColorFromStream(stream, result);
    return result;
}

GVariant readGVariantFromStream(GStream *stream)
{
    GVariant var;
    readGVariantFromStream(stream, var);
    return var;
}

void readGVariantFromStream(GStream *stream, GVariant &result)
{
    short nType = readShortFromStream(stream);
    switch (nType)
    {
    case gbdtUnknown:
        assert(false);
        break;
    case gbdtInteger:
        result.setValue(readIntFromStream(stream));
        break;
    case gbdtFloat:
        result.setValue(readDoubleFromStream(stream));
        break;
    case gbdtString:
        result.setValue(readStrFromStream(stream));
        break;
    case gbdtBoolean:
        result.setValue(readBoolFromStream(stream));
        break;
    case gbdtDateTime:
        result.setValue(readDoubleFromStream(stream));
        break;
    case gbdtTable:
        assert(false);
        break;
    case gbdtInt64:
        result.setValue(readInt64FromStream(stream));
        break;
    case gbdtUInt64:
        result.setValue(readUInt64FromStream(stream));
        break;
    case gbdtBlob:
        result.setValue(readByteArrayFromStream(stream));
        break;
    case gbdtGUID:
        result.setValue(GUIDToVariant(readGUIDFromStream(stream)));
        break;
    default:
        assert(false);
        break;
    }
}

void writeGVariantToStream(GStream *stream, const GVariant &value)
{
    switch (value.type())
    {
    case gbdtUnknown:
        assert(false);
        break;
    case GVariant::Int:
        writeShortToStream(stream, short(gbdtInteger));
        writeIntToStream(stream, value.toInt());
        break;
//    case GVariant::Short:
//        writeShortToStream(stream, short(gbdtInt64));
//        writeDoubleToStream(stream, value.toDouble());
//        break;
    case GVariant::String:
        writeShortToStream(stream, short(gbdtString));
        writeStrToStream(stream, value.toString());
        break;
    case GVariant::Bool:
        writeShortToStream(stream, short(gbdtBoolean));
        writeBoolToStream(stream, value.toBool());
        break;
    case GVariant::Double:
        writeShortToStream(stream, short(gbdtFloat));
        writeDoubleToStream(stream, value.toDouble());
        break;
//    case GVariant::Int:
//        assert(false);
//        break;
    case GVariant::LongLong:
        writeShortToStream(stream, short(gbdtInt64));
        writeInt64ToStream(stream, value.toLongLong());
        break;
    case GVariant::ULongLong:
        writeShortToStream(stream, short(gbdtUInt64));
        writeUInt64ToStream(stream, value.toULongLong());
        break;
    case GVariant::ByteArray:
    {
        writeShortToStream(stream, short(gbdtGUID));
        GByteArray byteAarray = value.toByteArray();
        writeByteArrayToStream(stream, byteAarray.constData(), byteAarray.size());
        break;
    }
    case GVariant::Uuid:
    {
        writeShortToStream(stream, short(gbdtGUID));
        writeGUIDToStream(stream, variantToGUID(value));
        break;
    }
    default:
        assert(false);
        break;
    }
}

void writeStrToStream(GStream *stream, const GString &value, bool useUnicode)
{
    if (useUnicode)
    {
        int nSize = value.length();
        stream->write((const char *)(&nSize), sizeof(int));
        const char *buf = (const char *)(value.constData());
        stream->write(buf, nSize * c_GCharSize);
        return;
    }
    else
    {
        GByteArray byteArray = unicodeToGBK(value);
        writeStrToStream(stream, byteArray.constData(), byteArray.length(), useUnicode);
    }
}

void writeStrToStream(GStream *stream, const char *value, int size, bool useUnicode)
{
    if (useUnicode)
    {
        int nSize = size / (c_GCharSize);
        writeUIntToStream(stream, nSize);
    }
    else
    {
        writeByteToStream(stream, size);
    }
    if (size > 0)
    {
        stream->write(value, size);
    }
}

void writeByteArrayToStream(GStream *stream, const char *value, int size)
{
    writeIntToStream(stream, size);
    if (size > 0)
    {
        stream->write(value, size);
    }
}

void writeByteArrayToStream(GStream *stream, const GByteArray &value)
{
    writeByteArrayToStream(stream, value.constData(), value.size());
}

void writeMemoToStream(GStream *stream, const char *value, int size, bool useUnicode)
{
    if (useUnicode)
    {
        int nSize = size / (c_GCharSize);
        writeUIntToStream(stream, nSize);
    }
    else
    {
        writeUIntToStream(stream, size);
    }
    if (size > 0)
    {
        stream->write(value, size);
    }
}

void writeMemoToStream(GStream *stream, const GString &value, bool useUnicode)
{
    if (useUnicode)
    {
        uint nSize = value.length();
        writeUIntToStream(stream, nSize);
        const char *buf = (const char *)(value.constData());
        stream->write(buf, nSize * c_GCharSize);
        return;
    }
    else
    {
        GByteArray byteArray = unicodeToGBK(value);
        writeMemoToStream(stream, byteArray.constData(), byteArray.length(), useUnicode);
    }
}

void writeColorToStream(GStream *stream, const TColor color)
{
    int nValue = (int)(color);
    //转用整形操作是为了同DELPHI产生的流兼容
    writeIntToStream(stream, nValue);
}

GMemoryStream *createMemoryStream()
{
    return new GMemoryStream();
}

class GIStreamImplPrivate
{
public:
  GIStreamImplPrivate(GIStreamImpl *parent)
    : q_ptr(parent)
  {
  }

private:
  GIStreamImpl * const q_ptr;
  Q_DECLARE_PUBLIC(GIStreamImpl);

  GStream *m_stream;
  bool m_ownStream;
};

/* GStreamImpl */

GIStreamImpl::GIStreamImpl(GStream *stream) : d_ptr(new GIStreamImplPrivate(this))
{
    Q_D(GIStreamImpl);
    if (stream)
    {
        d->m_stream = stream;
        d->m_ownStream = false;
    }
    else
    {
        d->m_stream = new GBlockMemoryStream();
        d->m_ownStream = true;
    }
}

GIStreamImpl::~GIStreamImpl()
{
    Q_D(GIStreamImpl);
    if (d->m_ownStream)
    {
        d->m_stream->close();
        freeAndNil(d->m_stream)
    }

    freePtr(d);
}

GStream *GIStreamImpl::stream() const
{
    Q_D(const GIStreamImpl);
    return d->m_stream;
}

HRESULT GIStreamImpl::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
    if (pv == NULL)
        return STG_E_INVALIDPOINTER;
    try
    {
        Q_D(GIStreamImpl);
        *pcbRead = d->m_stream->read((char *)pv, cb);
        return S_OK;
    }
    catch(...)
    {
        return S_FALSE;
    }
}

HRESULT GIStreamImpl::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
{
    if (pv == NULL)
        return STG_E_INVALIDPOINTER;
    try
    {
        Q_D(GIStreamImpl);
        *pcbWritten = d->m_stream->write((char *)pv, cb);
        return S_OK;
    }
    catch(...)
    {
        return S_FALSE;
    }
}

HRESULT GIStreamImpl::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
    try
    {
        Q_D(GIStreamImpl);
        if ((dwOrigin > STREAM_SEEK_END))//(dwOrigin < STREAM_SEEK_SET) ||
            return STG_E_INVALIDFUNCTION;
        switch (dwOrigin)
        {
        case STREAM_SEEK_SET:
        {
            d->m_stream->seek(dlibMove.QuadPart);
            break;
        }
        case STREAM_SEEK_CUR:
        {
            long long llPos = d->m_stream->pos();
            d->m_stream->seek(llPos + dlibMove.QuadPart);
            break;
        }
        case STREAM_SEEK_END:
        {
            long long llPos = d->m_stream->size();
            d->m_stream->seek(llPos + dlibMove.QuadPart);
            break;
        }
        default:
            break;
        }
        plibNewPosition->QuadPart = d->m_stream->pos();
        return S_OK;
    }
    catch(...)
    {
        return STG_E_INVALIDPOINTER;
    }
}

HRESULT GIStreamImpl::SetSize(ULARGE_INTEGER libNewSize)
{
    return S_OK;
    G_UNUSED(libNewSize)
}

HRESULT GIStreamImpl::CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
    char *buf = new char[cb.QuadPart];
    HRESULT result = S_OK;
    try
    {
        Q_D(GIStreamImpl);
        int nOldPos = d->m_stream->pos();
        pcbRead->QuadPart = d->m_stream->read(buf, cb.QuadPart);
        pstm->Write(buf, cb.QuadPart, (ULONG *)&(pcbWritten->QuadPart));
        d->m_stream->seek(nOldPos);
        result = S_OK;
    }
    catch(...)
    {
        result = E_UNEXPECTED;
    }
    freeAndNil(buf);
    return result;
}

HRESULT GIStreamImpl::Commit(DWORD grfCommitFlags)
{
    return S_OK;
    G_UNUSED(grfCommitFlags)
}

HRESULT GIStreamImpl::Revert()
{
    Q_D(GIStreamImpl);
    d->m_stream->reset();
    return STG_E_REVERTED;
}

HRESULT GIStreamImpl::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
    return STG_E_INVALIDFUNCTION;
    G_UNUSED(libOffset);
    G_UNUSED(cb);
    G_UNUSED(dwLockType);
}

HRESULT GIStreamImpl::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
    return STG_E_INVALIDFUNCTION;
    G_UNUSED(libOffset);
    G_UNUSED(cb);
    G_UNUSED(dwLockType);
}

HRESULT GIStreamImpl::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
    HRESULT result = S_OK;
    try
    {
        if (pstatstg != NULL)
        {
            Q_D(GIStreamImpl);
            pstatstg->type = STGTY_STREAM;
            pstatstg->cbSize.QuadPart = d->m_stream->size();
            pstatstg->mtime.dwLowDateTime = 0;
            pstatstg->mtime.dwHighDateTime = 0;
            pstatstg->ctime.dwLowDateTime = 0;
            pstatstg->ctime.dwHighDateTime = 0;
            pstatstg->atime.dwLowDateTime = 0;
            pstatstg->atime.dwHighDateTime = 0;
            pstatstg->grfLocksSupported = LOCK_WRITE;
        }
    }
    catch(...)
    {
        result = E_UNEXPECTED;
        throw;
    }
    return result;
    G_UNUSED(grfStatFlag)
}

HRESULT GIStreamImpl::Clone(IStream **ppstm)
{
    GIStreamImpl *iStream = new GIStreamImpl();
    iStream->AddRef();
    *ppstm = iStream;
    try
    {
        Q_D(GIStreamImpl);
        int nPos = d->m_stream->pos();
        d->m_stream->seek(0);
        GByteArray array = d->m_stream->readAll();
        ULONG written = 0;
        (*ppstm)->Write(array.constData(), array.length(), &written);
        d->m_stream->seek(nPos);
    }
    catch(...)
    {
       return S_FALSE;
    }
    return S_OK;
}

HRESULT GIStreamImpl::_QueryInterface(const IID &riid, void **ppvObject)
{
    if (riid == IID_IStream)
    {
        *ppvObject = static_cast<IStream *>(this);
        this->AddRef();
        return NOERROR;
    }
    else
        return GInterfaceObject::_QueryInterface(riid, ppvObject);
}

class GStreamImplPrivate : public QIODevicePrivate
{
public:
  GStreamImplPrivate(GStreamImpl *parent)
    : q_ptr(parent)
  {
  }

private:
  GStreamImpl * const q_ptr;
  Q_DECLARE_PUBLIC(GStreamImpl);

  IStream *m_stream;
  bool m_ownStream;
};

/* GIStream */
GStreamImpl::GStreamImpl(IStream *stream, QIODevice::OpenMode flags)
    : GStream(*(new GStreamImplPrivate(this)))
{
    Q_D(GStreamImpl);
    if (stream == NULL)
    {
        d->m_stream = new GIStreamImpl();
        d->m_stream->AddRef();
        d->m_ownStream = true;
    }
    else
    {
        d->m_stream = stream;
        d->m_ownStream = false;
    }
    open(flags);
}

GStreamImpl::~GStreamImpl()
{
    Q_D(GStreamImpl);
    if (d->m_ownStream)
    {
        freeAndNilIntf(d->m_stream)
    }
}

gint64 GStreamImpl::size() const
{
    Q_D(const GStreamImpl);
    return streamLength(d->m_stream);
}

gint64 GStreamImpl::pos() const
{
    return GStream::pos();
}

bool GStreamImpl::seek(gint64 off)
{
    Q_D(GStreamImpl);
    ULARGE_INTEGER newPositon;
    LARGE_INTEGER offset;
    offset.QuadPart = off;
    d->m_stream->Seek(offset, STREAM_SEEK_SET, &newPositon);
    return GStream::seek(off);
}

gint64 GStreamImpl::readData(char *data, gint64 maxlen)
{
    Q_D(GStreamImpl);
    if (data == NULL)
        return 0;
    ULONG lRead = 0;
    d->m_stream->Read(data, maxlen, &lRead);
    return lRead;
}

gint64 GStreamImpl::writeData(const char *data, gint64 len)
{
    Q_D(GStreamImpl);
    if (data == NULL)
        return 0;
    ULONG lWrite(0);
    d->m_stream->Write(data, len, &lWrite);
    return lWrite;
}

IStream *GStreamImpl::streamIntf() const
{
    Q_D(const GStreamImpl);
    return d->m_stream;
}

void readGUIDFromStream(GStream *stream, GUID &value)
{
    GByteArray baValue;
    readByteArrayFromStream(stream, baValue);

    value = byteArrayToGUID(baValue);
}

void writeGUIDToStream(GStream *stream, GUID value)
{
    writeByteArrayToStream(stream, GUIDToByteArray(value));
}

void copyStream(GStream *dst, GStream *src)
{
    const int nMaxReadSize = 4096;
    while(src->pos() < src->size())
    {
        dst->write(src->read(nMaxReadSize));
    }
}
