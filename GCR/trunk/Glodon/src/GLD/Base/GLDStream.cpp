#include "GLDStream.h"

#include "GLDGlobal.h"
#include "GLDSysUtils.h"
#include "GLDFileUtils.h"
#include "private/qiodevice_p.h"

//块的大小
const int c_nBlockSize = 3800;

class GMemoryStreamPrivate
{
public:
  GMemoryStreamPrivate(GMemoryStream *parent)
    : q_ptr(parent)
  {
  }

private:
  GMemoryStream * const q_ptr;
  Q_DECLARE_PUBLIC(GMemoryStream);
  bool m_ownerBuf;
  GByteArray *m_buf;
};

/* GMemoryStream */

GMemoryStream::GMemoryStream(GStream::OpenMode flags)
    : GBuffer(), d_ptr(new GMemoryStreamPrivate(this))
{
    Q_D(GMemoryStream);
    d->m_ownerBuf = false;
    open(flags);
}

GMemoryStream::GMemoryStream(GByteArray *buf, bool ownerBuf, GStream::OpenMode flags)
    : GBuffer(buf), d_ptr(new GMemoryStreamPrivate(this))
{
    Q_D(GMemoryStream);
    d->m_ownerBuf = ownerBuf;
    d->m_buf = buf;
    open(flags);
}

GMemoryStream::~GMemoryStream()
{
    Q_D(GMemoryStream);
    if (d->m_ownerBuf && d->m_buf)
    {
        delete d->m_buf;
    }

    freePtr(d);
}

/*
gint64 GMemoryStream::read(char *data, gint64 maxlen)
{
    return GBuffer::read(data, maxlen);
}

GByteArray GMemoryStream::read(gint64 maxlen)
{
    return GBuffer::read(maxlen);
}

gint64 GMemoryStream::readData(char *data, gint64 maxlen)
{
    return GBuffer::readData(data, maxlen);
}

gint64 GMemoryStream::writeData(const char *data, gint64 len)
{
    return GBuffer::writeData(data, len);
}

gint64 GMemoryStream::write(const GByteArray &data)
{
    return GBuffer::write(data);
}

gint64 GMemoryStream::write(const char *data, gint64 len)
{
    return GBuffer::write(data, len);
}

gint64 GMemoryStream::write(const char *data)
{
    return GBuffer::write(data);
}

bool GMemoryStream::seek(gint64 pos)
{
    return GBuffer::seek(pos);
}

gint64 GMemoryStream::pos() const
{
    return GBuffer::pos();
}

gint64 GMemoryStream::size() const
{
    return GBuffer::size();
}

GByteArray GMemoryStream::readAll()
{
    return GBuffer::readAll();
}

void GMemoryStream::close()
{
    GBuffer::close();
}

bool GMemoryStream::open(GStream::OpenMode mode)
{
    return GBuffer::open(mode);
}

bool GMemoryStream::reset()
{
    return GBuffer::reset();
}*/

/* GBlockMemoryStream */
class GBlockMemoryStreamPrivate : public QIODevicePrivate
{
public:
  GBlockMemoryStreamPrivate()
    : m_size(0), m_position(0)/*, m_blockSize(3800)*/
    , m_blockCount(0), m_curBlockIndex(0), m_curBlockPosition(0)
  {
  }

private:
  Q_DECLARE_PUBLIC(GBlockMemoryStream);

  GObjectList<GByteArray*> m_buf;
  gint64 m_size;
  gint64 m_position;
//    gint64 m_blockSize;
  gint64 m_blockCount;
  gint64 m_curBlockIndex;
  gint64 m_curBlockPosition;
};


GBlockMemoryStream::GBlockMemoryStream(OpenMode flags)
    : GStream(*(new GBlockMemoryStreamPrivate))
{
    open(flags);
}

GBlockMemoryStream::~GBlockMemoryStream()
{
    clearData();
}

GObjectList<GByteArray*> &GBlockMemoryStream::buffer()
{
    Q_D(GBlockMemoryStream);
    return d->m_buf;
}

void GBlockMemoryStream::setBuffer(GObjectList<GByteArray*> *byteArray)
{
    if (isOpen())
    {
        qWarning("QBuffer::setBuffer: Buffer is open");
        return;
    }
    clearData();
    if (byteArray)
    {
        // todo复制数据
    //    d->buf = byteArray;
    }
}

gint64 GBlockMemoryStream::readData(char *data, gint64 len)
{
    gint64 nResult = 0;

    Q_D(GBlockMemoryStream);
    if ((d->m_position >= 0) && (len > 0))
    {
        nResult = d->m_size - d->m_position;
        if (nResult > len)
        {
            nResult = len;
        }
        doRead(data, nResult);
        d->m_position += nResult;
    }
    return nResult;
}

gint64 GBlockMemoryStream::writeData(const char *data, gint64 len)
{
    Q_D(GBlockMemoryStream);
    if ((d->m_position >= 0) && (len > 0))
    {
        gint64 nPos = d->m_position + len;
        if (nPos > d->m_size)
        {
            if (nPos > (c_nBlockSize * d->m_blockCount))
            {
                setCapacity(nPos);
            }
            d->m_size = nPos;
        }
        doWrite(data, len);
        d->m_position = nPos;
        return len;
    }
    return 0;
}

bool GBlockMemoryStream::open(GStream::OpenMode flags)
{
    if ((flags & (Append | Truncate)) != 0)
    {
        flags = GStream::OpenMode(flags | WriteOnly);
    }
    if ((flags & (GStream::ReadOnly | WriteOnly)) == 0)
    {
        qWarning("QBuffer::open: Buffer access not specified");
        return false;
    }

    if ((flags & Truncate) == Truncate)
    {
        // todo
    //    d->buf->resize(0);
    }
    return GStream::open(flags);
}

void GBlockMemoryStream::close()
{
    GStream::close();
}

gint64 GBlockMemoryStream::size() const
{
    Q_D(const GBlockMemoryStream);
    return gint64(d->m_size);
}

gint64 GBlockMemoryStream::pos() const
{
    return GStream::pos();
}

bool GBlockMemoryStream::seek(gint64 pos)
{
    seek(pos, 0);
    return GStream::seek(pos);
}

bool GBlockMemoryStream::atEnd() const
{
    return GStream::atEnd();
}

bool GBlockMemoryStream::canReadLine() const
{
//    G_D(GBlockMemoryStream);
    if (!isOpen())
    {
        return false;
    }
    // todo
    return true;
   // return d->buf->indexOf('\n', int(pos())) != -1 || GStream::canReadLine();
}

gint64 GBlockMemoryStream::seek(gint64 nOffset, int nFlag)
{
    Q_D(GBlockMemoryStream);
    switch (nFlag)
    {
    case soBeginning:
        doSetPosition(nOffset);
        break;
    case soCurrent:
        doSetPosition(d->m_position + nOffset);
        break;
    case soEnd:
        doSetPosition(d->m_size + nOffset);
        break;
    }
    return d->m_position;
}

bool GBlockMemoryStream::reset()
{
    clearData();
    return seek(0);
}

void GBlockMemoryStream::clearData()
{
    Q_D(GBlockMemoryStream);
    d->m_size = 0;
    d->m_blockCount = 0;
    d->m_buf.clear();
}

void GBlockMemoryStream::loadFromStream(GStream *stream, gint64 len)
{
    if (!stream->isOpen())
    {
        stream->open(GStream::ReadOnly);
    }

    //分块读，节约内存
    gint64 nReadLength = -1;
    if (len < 0)
    {
        nReadLength = stream->size();
    }
    else
    {
        nReadLength = len;
    }

    int nCount = nReadLength / c_nBlockSize;
    for (int i = 0; i < nCount; ++i)
    {
        write(stream->read(c_nBlockSize));
    }
    write(stream->read(nReadLength % c_nBlockSize));
}

void GBlockMemoryStream::saveToStream(GStream *stream)
{
    seek(0);

    int nCount = size() / c_nBlockSize;

    for (int i = 0; i < nCount; ++i)
    {
        stream->write(read(c_nBlockSize));
    }
    stream->write(read(size() % c_nBlockSize));
}

void GBlockMemoryStream::loadFromFile(const GString &fileName)
{
    GFileStream *fileStream = new GFileStream(fileName);
    if (!fileStream->exists())
    {
        freeAndNil(fileStream)
        return;
    }
    loadFromStream(fileStream);
    freeAndNil(fileStream);
}

void GBlockMemoryStream::saveToFile(const GString &fileName)
{
    GString SDirName = extractFileDir(fileName);
    if (!directoryExists(SDirName))
    {
        forceDirectories(SDirName);
    }

    GFileStream *stream = new GFileStream(fileName);
    stream->open(GStream::ReadWrite);
    saveToStream(stream);
    stream->close();
    freeAndNil(stream)
}

void GBlockMemoryStream::doSetPosition(gint64 nPos)
{
    Q_D(GBlockMemoryStream);
    d->m_position = nPos;
    d->m_curBlockPosition = nPos % c_nBlockSize;
    d->m_curBlockIndex = nPos / c_nBlockSize;
}

void GBlockMemoryStream::setCapacity(gint64 nNewCapacity)
{
    Q_D(GBlockMemoryStream);
    gint64 nCount = (nNewCapacity / c_nBlockSize) + 1;
    for (gint64 i = d->m_blockCount - 1; i >= nCount; --i)
    {
        d->m_buf.removeAt(i);
    }
    for (gint64 i = d->m_blockCount; i < nCount; ++i)
    {
        GByteArray *data = new GByteArray;
        data->resize(c_nBlockSize);
        d->m_buf.push_back(data);
    }
    d->m_blockCount = nCount;
}

void GBlockMemoryStream::setSize(gint64 nNewSize)
{
    Q_D(GBlockMemoryStream);
    if (d->m_size < 0)
    {
        d->m_size = 0;
    }
    gint64 nOldPosition = d->m_position;
    setCapacity(nNewSize);
    d->m_size = nNewSize;
    if (nOldPosition > nNewSize)
    {
        seek(0, soEnd);
    }
}

void GBlockMemoryStream::doRead(char *data, gint64 nReadCount)
{
    gint64 nCount = 0;
    gint64 nBufSize = 0;

    Q_D(GBlockMemoryStream);
    while (nCount < nReadCount)
    {
        gint64 nSize = c_nBlockSize - d->m_curBlockPosition;
        if (nReadCount - nCount >= nSize)
        {
            nBufSize = nSize;
        }
        else
        {
            nBufSize = nReadCount - nCount;
        }
        const char* bufPtr = d->m_buf[d->m_curBlockIndex]->constData();
        memcpy(data + nCount, bufPtr + d->m_curBlockPosition, nBufSize);
        if (nReadCount - nCount >= nSize)
        {
            d->m_curBlockIndex++;
            d->m_curBlockPosition = 0;
        }
        else
        {
            d->m_curBlockPosition += nBufSize;
        }
        nCount += nBufSize;
    }
}

void GBlockMemoryStream::doWrite(const char *data, gint64 nWriteCount)
{
    gint64 nCount = 0;
    gint64 nBufSize = 0;

    Q_D(GBlockMemoryStream);
    while (nCount < nWriteCount)
    {
        gint64 count = c_nBlockSize - d->m_curBlockPosition;
        if (nWriteCount - nCount >= count)
        {
            nBufSize = count;
        }
        else
        {
            nBufSize = nWriteCount - nCount;
        }
        char* bufPtr = (char*)(d->m_buf.at(d->m_curBlockIndex)->constData());
        memcpy(bufPtr + d->m_curBlockPosition, data + nCount, nBufSize);
        if (nWriteCount - nCount >= count)
        {
            d->m_curBlockIndex++;
            d->m_curBlockPosition = 0;
        }
        else
        {
            d->m_curBlockPosition += nBufSize;
        }
        nCount += nBufSize;
    }
}

class GReadBufferedStreamPrivate :  public QIODevicePrivate
{
public:
  GReadBufferedStreamPrivate(GReadBufferedStream *parent = 0)
    : q_ptr(parent)
  {
  }

private:
  GReadBufferedStream * const q_ptr;
  Q_DECLARE_PUBLIC(GReadBufferedStream);

  bool m_autoDestroy;
  GStream *m_baseStream;
  gint64 m_baseSize;
  char *m_buffer;
  int m_bufferSize;
  int m_bufferData;
  gint64 m_basePosition;
  gint64 m_bufferOffset;
  char *m_bufferPtr;
};
/*CReadBufferedStream*/

GReadBufferedStream::GReadBufferedStream(GStream *baseStream, int bufferSize, bool autoDestroyWrappedStream):
    GStream(*(new GReadBufferedStreamPrivate(this)))
{
    Q_D(GReadBufferedStream);
    d->m_autoDestroy = autoDestroyWrappedStream;
    d->m_baseStream = baseStream;
    d->m_baseSize = baseStream->size();
    d->m_bufferSize = bufferSize;
    d->m_bufferData = 0;
    d->m_basePosition = baseStream->pos();
    d->m_bufferOffset = baseStream->pos();

    d->m_buffer = new char[bufferSize + 1];
    d->m_bufferPtr = d->m_buffer;
    // cuidc 添加  | GStream::Unbuffered 使其绕过缓存，也就是不需要将来将d->m_buffer中的内容复制到 GStream 的内存中。
    open(GStream::ReadOnly | GStream::Unbuffered);
}

GReadBufferedStream::~GReadBufferedStream()
{
    Q_D(GReadBufferedStream);
    freeAndNil(d->m_buffer);
    if (d->m_autoDestroy)
    {
        freeAndNil(d->m_baseStream);
    }
    close();

}

gint64 GReadBufferedStream::size() const
{
    Q_D(const GReadBufferedStream);
    return d->m_baseSize;
}

gint64 GReadBufferedStream::readData(char *data, gint64 maxlen)
{
    Q_D(GReadBufferedStream);
    if ((maxlen = min(maxlen, gint64(size() - currentPosition()))) <= 0)
    {
        return gint64(0);
    }
    if (maxlen >= d->m_bufferSize)
    {
        maxlen = d->m_bufferSize / 2;
    }
    gint64 result = 0;
    char *pOut = data;
    while (maxlen > 0)
    {
        int nCanMove = maxlen;
        if (d->m_bufferData < nCanMove)
        {
            nCanMove = d->m_bufferData;
        }
        if (nCanMove > 0)
        {
            gMemMove(pOut, d->m_bufferPtr, nCanMove);
            d->m_bufferPtr += nCanMove;
            pOut += nCanMove;
            result += nCanMove;
            d->m_bufferData -= nCanMove;
            maxlen -= nCanMove;
        }
        if (maxlen >= d->m_bufferSize)
        {
            d->m_bufferOffset = d->m_basePosition;
            int nDataRead = d->m_baseStream->read(pOut, maxlen);
            d->m_basePosition += nDataRead;
            result += nDataRead;
            break;
        }
        else if (maxlen > 0)
        {
            d->m_bufferOffset = d->m_basePosition;
            d->m_bufferData = d->m_baseStream->read(d->m_buffer, d->m_bufferSize);
            d->m_basePosition += d->m_bufferData;
            d->m_bufferPtr = d->m_buffer;
            if (d->m_bufferData == 0)
            {
                break;
            }
        }
    }
    return result;
}

gint64 GReadBufferedStream::writeData(const char *data, gint64 len)
{
    Q_D(GReadBufferedStream);
    gint64 oldPos = pos();
    d->m_baseStream->seek(oldPos);
    gint64 result = d->m_baseStream->write(data, len);
    d->m_bufferData = 0;
    d->m_basePosition = oldPos + result;
    d->m_bufferPtr = d->m_buffer;
    d->m_bufferOffset = oldPos + result;
    return result;
}

bool GReadBufferedStream::autoDestroyWrappedStream()
{
    Q_D(GReadBufferedStream);
    return d->m_autoDestroy;
}

void GReadBufferedStream::setAutoDestroyWrappedStream(bool value)
{
    Q_D(GReadBufferedStream);
    d->m_autoDestroy = value;
}

GStream *GReadBufferedStream::wrappedStream()
{
    Q_D(GReadBufferedStream);
    return d->m_baseStream;
}

gint64 GReadBufferedStream::currentPosition() const
{
    Q_D(const GReadBufferedStream);
    return d->m_bufferPtr - d->m_buffer + d->m_bufferOffset;
}

gint64 GReadBufferedStream::internalSeek(gint64 offset)
{
    Q_D(GReadBufferedStream);
    gint64 bufferEnd = currentPosition() + d->m_bufferData;    //last AOffset in Buffer
    if ((offset < d->m_bufferData) || (offset > bufferEnd))
    {
        d->m_bufferData = 0;
        d->m_baseStream->seek(offset);
        d->m_basePosition = offset;
        d->m_bufferPtr = d->m_buffer;
        d->m_bufferOffset = offset;
    }
    else
    {
        d->m_bufferPtr = d->m_buffer + (offset - d->m_bufferOffset);
        d->m_bufferData = bufferEnd - currentPosition();
    }
    return currentPosition();
}

gint64 GReadBufferedStream::pos() const
{
    return currentPosition();
}

bool GReadBufferedStream::seek(gint64 off)
{
    GStream::seek(off);
    return 0 != seek(off, soBeginning);
}

gint64 GReadBufferedStream::seek(gint64 offset, GSeekOrigin origin)
{
    Q_D(GReadBufferedStream);
    switch (origin)
    {
    case soBeginning:
        return internalSeek(offset);
    case soEnd:
        return internalSeek(d->m_baseStream->size() + offset);
    default:
        return internalSeek(currentPosition() + offset);
    }
}

class GWriteBufferedStreamPrivate : public QIODevicePrivate
{
public:
  GWriteBufferedStreamPrivate(GWriteBufferedStream *parent = 0)
    : q_ptr(parent)
  {
  }

private:
  GWriteBufferedStream * const q_ptr;
  Q_DECLARE_PUBLIC(GWriteBufferedStream);

  bool m_autoDestroy;
  GStream *m_baseStream;
  gint64 m_baseSize;
  gint64 m_basePosition;
  int m_bufferData;
  int m_bufferSize;
  char *m_buffer;
};
/*GWriteBufferedStream*/

GWriteBufferedStream::GWriteBufferedStream(GStream *baseStream, int bufferSize, bool autoDestroyWrappedStream):
    GStream(*(new GWriteBufferedStreamPrivate(this)))
{
    Q_D(GWriteBufferedStream);

    assert(baseStream != NULL);
    assert(bufferSize >= 8);
    d->m_buffer = new char[bufferSize + 1];
    d->m_autoDestroy = autoDestroyWrappedStream;
    d->m_baseStream = baseStream;
    d->m_baseSize = baseStream->size();
    d->m_bufferSize = bufferSize;
    d->m_bufferData = 0;
    d->m_basePosition = baseStream->pos();

    // cuidc 添加  | GStream::Unbuffered 使其绕过缓存，也就是不需要将来将d->m_buffer中的内容复制到 GStream 的内存中。
    open(GStream::ReadWrite | GStream::Unbuffered);
}

GWriteBufferedStream::~GWriteBufferedStream()
{
    Q_D(GWriteBufferedStream);
    flushStream();
    freeAndNil(d->m_buffer);

    if (d->m_autoDestroy)
    {
        freeAndNil(d->m_baseStream);
    }
    close();
}

void GWriteBufferedStream::flushStream()
{
    Q_D(GWriteBufferedStream);
    if (d->m_bufferData > 0)
    {
        d->m_basePosition = d->m_basePosition + d->m_baseStream->write(d->m_buffer, d->m_bufferData);
        d->m_bufferData = 0;
        if (d->m_baseSize < d->m_basePosition)
        {
            d->m_baseSize = d->m_basePosition;
        }
    }
}

bool GWriteBufferedStream::seek(gint64 off)
{
    GStream::seek(off);
    return 0 != seek(off, soBeginning);
}

gint64 GWriteBufferedStream::seek(const gint64 offset, GSeekOrigin origin)
{
    gint64 result = 0;
    switch (origin)
    {
    case soBeginning:
        result = internalSeek(offset);
        break;
    case soEnd:
        result = internalSeek(currentSize() + offset);
        break;
    case soCurrent:
        if (offset != 0)
        {
            result = internalSeek(currentPosition() + offset);
        }
        else
        {
            result = currentPosition();
        }
        break;
    default:
        result = 0;
        assert(false);
        break;
    }
    return result;
}

gint64 GWriteBufferedStream::readData(char *data, gint64 maxlen)
{
    Q_D(GWriteBufferedStream);
    if ((maxlen = min(maxlen, gint64(size() - currentPosition()))) <= 0)
    {
        return gint64(0);
    }
    flushStream(); 
    gint64 result = d->m_baseStream->read(data, maxlen);
    d->m_basePosition = d->m_basePosition + result;
    return result;
}

gint64 GWriteBufferedStream::writeData(const char *data, gint64 len)
{
    Q_D(GWriteBufferedStream);
    const char *pOut = data;
    char *pBufferPtr = d->m_buffer + d->m_bufferData;
    int nCanMove = d->m_bufferData + len;
    if (nCanMove < d->m_bufferSize)
    {
        gMemMove(pBufferPtr, pOut, len);
        d->m_bufferData += len;
    }
    else if (nCanMove == d->m_bufferSize)
    {
        gMemMove(pBufferPtr, pOut, len);
        d->m_bufferData = d->m_bufferSize;
        flushStream();
    }
    else if (nCanMove < d->m_bufferSize * 2)
    {
        int nCount = d->m_bufferSize - d->m_bufferData;
        gMemMove(pBufferPtr, pOut, nCount);
        pOut += nCount;
        nCount = len - nCount;
        d->m_bufferData = d->m_bufferSize;
        flushStream();
        gMemMove(d->m_buffer, pOut, nCount);
        d->m_bufferData += nCount;
    }
    else
    {
        flushStream();
        d->m_baseStream->write(data, len);
        d->m_basePosition = d->m_basePosition + len;
        if (d->m_baseSize < d->m_basePosition)
        {
                d->m_baseSize = d->m_basePosition;
        }
    }
    return len;
}

bool GWriteBufferedStream::autoDestroyWrappedStream()
{
    Q_D(GWriteBufferedStream);
    return d->m_autoDestroy;
}

void GWriteBufferedStream::setAutoDestroyWrappedStream(bool value)
{
    Q_D(GWriteBufferedStream);
    d->m_autoDestroy = value;
}

GStream *GWriteBufferedStream::wrappedStream()
{
    Q_D(GWriteBufferedStream);
    return d->m_baseStream;
}

gint64 GWriteBufferedStream::size() const
{
    return currentSize();
}

gint64 GWriteBufferedStream::pos() const
{
    return currentPosition();
}

gint64 GWriteBufferedStream::currentPosition() const
{
    Q_D(const GWriteBufferedStream);
    return d->m_basePosition + d->m_bufferData;
}

gint64 GWriteBufferedStream::currentSize() const
{
    Q_D(const GWriteBufferedStream);
    gint64 result = currentPosition();
    if (result < d->m_baseSize)
    {
        return d->m_baseSize;
    }
    return result;
}

gint64 GWriteBufferedStream::internalSeek(gint64 offset)
{
    Q_D(GWriteBufferedStream);
    gint64 result = currentPosition();
    if (offset != result)
    {
        flushStream();
        if (d->m_baseStream->seek(offset))
        {
            result = d->m_baseStream->pos();
            d->m_basePosition = result;
        }
    }
    return result;
}

