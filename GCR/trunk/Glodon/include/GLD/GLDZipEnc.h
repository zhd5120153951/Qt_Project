#ifndef GLDZIPENC_H
#define GLDZIPENC_H

#include "GLDTypes.h"
#include "GLDZip_Global.h"
#include "GLDStream.h"

class GLDZIPSHARED_EXPORT GLDZipDecryptEngine
{
public:
    GLDZipDecryptEngine();
    virtual ~GLDZipDecryptEngine();
public:
    unsigned char decode(unsigned char ch);
    void decodeBuffer(char *buffer, int count);
    bool verifyHeader(const unsigned char header[12], const GString &passPhrase, long checkValue);
protected:
    void zdeInitState(const GString &passPhras);
private:
    bool m_ready;
    long m_state[3];
};

class GLDZIPSHARED_EXPORT GLDZipDecryptStream : public GStream
{
public:
    GLDZipDecryptStream(GStream *stream, long checkValue, const GString passPhrase);
    virtual ~GLDZipDecryptStream();
public:
    bool isValid();
    long seek(long offset, GSeekOrigin origin);

    gint64 size() const;
    gint64 pos() const;
    bool seek(gint64 off);
protected:
    gint64 readData(char *data, gint64 maxlen);
    gint64 writeData(const char *data, gint64 len);
private:
    long m_checkValue;
    GLDZipDecryptEngine *m_Engine;
    GString m_passPhrase;
    bool m_Ready;
    GStream *m_stream;
};

class GLDZIPSHARED_EXPORT GLDZipEncryptEngine
{
public:
    GLDZipEncryptEngine();
    virtual ~GLDZipEncryptEngine();
public:
    unsigned char encode(unsigned char ch);
    void encodeBuffer(char *buffer, int count);
    void createheader(unsigned char *header, const GString passphrase, long checkValue);
protected:
    void zeeInitState(const GString passPhrase);
private:
    bool m_ready;
    long m_state[3];
};

class GLDZIPSHARED_EXPORT GLDZipEncryptStream : public GStream
{
public:
    GLDZipEncryptStream(GStream *stream, long checkValue, const GString passPhrase);
    virtual ~GLDZipEncryptStream();
public:
    long seek(long offset, GSeekOrigin origin);

    gint64 size() const;
    gint64 pos() const;
    bool seek(gint64 off);
protected:
    gint64 readData(char *data, gint64 maxlen);
    gint64 writeData(const char *data, gint64 len);
private:
    char* m_buffer;
    int m_bufSize;
    GLDZipEncryptEngine *m_Engine;
    GStream *m_stream;
};

GLDZIPSHARED_EXPORT unsigned long zipCRC32(GByteArray &data);
//÷ÿ‘ÿΩ”ø⁄
GLDZIPSHARED_EXPORT unsigned long zipCRC32(GStream *stream);

GLDZIPSHARED_EXPORT unsigned long zipUpdateCRC32(unsigned char curByte, unsigned long curCrc);
//void zipUpdateKeys(unsigned short c, unsigned long &keys[3]);

#endif // GLDZIPENC_H
