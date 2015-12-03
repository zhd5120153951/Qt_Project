#ifndef GLDSTREAM_H
#define GLDSTREAM_H

#include "GLDTypes.h"
#include "GLDVector.h"
#include "GLDObject.h"

enum GSeekOrigin
{
    soBeginning = 0,
    soCurrent = 1,
    soEnd = 2
};

class GMemoryStreamPrivate;
class GLDCOMMONSHARED_EXPORT GMemoryStream : public GBuffer
{
#ifndef QT_NO_QOBJECT
    Q_OBJECT
#endif
public:
    GMemoryStream(OpenMode flags = GStream::ReadWrite);
    explicit GMemoryStream(GByteArray *buf, bool ownerBuf, OpenMode flags = GStream::ReadWrite);
    virtual ~GMemoryStream();
/*    gint64 read(char *data, gint64 maxlen);
    GByteArray read(gint64 maxlen);
    gint64 readData(char *data, gint64 maxlen);
    gint64 writeData(const char *data, gint64 len);
    gint64 write(const char *data, gint64 len);
    gint64 write(const char *data);
    gint64 write(const GByteArray &data);
    bool seek(gint64 pos);
    gint64 pos() const;
    gint64 size() const;
    GByteArray readAll();
    void close();
    bool open(OpenMode mode);
    bool reset();*/
private:
    GMemoryStreamPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GMemoryStream)
};

class GBlockMemoryStreamPrivate;
class GLDCOMMONSHARED_EXPORT GBlockMemoryStream : public GStream
{
#ifndef QT_NO_QOBJECT
    Q_OBJECT
#endif
public:
    explicit GBlockMemoryStream(GStream::OpenMode flags = GStream::ReadWrite);
    ~GBlockMemoryStream();

    GObjectList<GByteArray*> &buffer();
    void setBuffer(GObjectList<GByteArray*> *a);

    bool open(GStream::OpenMode openMode);
    void close();
    gint64 size() const;
    gint64 pos() const;
    bool seek(gint64 off);
    bool atEnd() const;
    bool canReadLine() const;
    gint64 seek(gint64 nOffset, int nFlag);
    bool reset();
    void clearData();

    void loadFromStream(GStream *stream, gint64 len = -1);
    void saveToStream(GStream *stream);
    void loadFromFile(const GString &fileName);
    void saveToFile(const GString &fileName);

protected:
    gint64 readData(char *data, gint64 maxlen);
    gint64 writeData(const char *data, gint64 len);
private:
    void doSetPosition(gint64 nPos);
    void setCapacity(gint64 nNewCapacity);
    void setSize(gint64 nNewSize);
    void doRead(char* data, gint64 nReadCount);
    void doWrite(const char* data, gint64 nWriteCount);

private:
    //GBlockMemoryStreamPrivate *  d_ptr;
    Q_DECLARE_PRIVATE(GBlockMemoryStream);
};

class GReadBufferedStreamPrivate;
class GLDCOMMONSHARED_EXPORT GReadBufferedStream : public GStream
{
#ifndef QT_NO_QOBJECT
    Q_OBJECT
#endif

public:
    GReadBufferedStream(GStream *baseStream, int bufferSize, bool autoDestroyWrappedStream = false);
    ~GReadBufferedStream();

    gint64 size() const;
    gint64 pos() const;
    bool seek(gint64 off);
    gint64 seek(gint64 offset, GSeekOrigin origin);

protected:
    gint64 readData(char *data, gint64 maxlen);
    gint64 writeData(const char *data, gint64 len);

private:
    bool autoDestroyWrappedStream();
    void setAutoDestroyWrappedStream(bool value);
    GStream *wrappedStream();
private:
    gint64 currentPosition() const;
    gint64 internalSeek(gint64 offset);
private:
    Q_DECLARE_PRIVATE(GReadBufferedStream);
};

class GWriteBufferedStreamPrivate;
class GLDCOMMONSHARED_EXPORT GWriteBufferedStream : public GStream
{
#ifndef QT_NO_QOBJECT
    Q_OBJECT
#endif

public:
    GWriteBufferedStream(GStream *baseStream, int bufferSize, bool autoDestroyWrappedStream = false);
    ~GWriteBufferedStream();

    gint64 size() const;
    gint64 pos() const;
    bool seek(gint64 off);

    gint64 seek(const gint64 offset, GSeekOrigin origin);
protected:
    gint64 readData(char *data, gint64 maxlen);
    gint64 writeData(const char *data, gint64 len);
    bool autoDestroyWrappedStream();
    void setAutoDestroyWrappedStream(bool value);
    GStream *wrappedStream();

private:
    gint64 currentPosition() const;
    gint64 currentSize() const;
    gint64 internalSeek(gint64 offset);
    void flushStream();
private:
     Q_DECLARE_PRIVATE(GWriteBufferedStream);
};

#endif // GLDSTREAM_H
