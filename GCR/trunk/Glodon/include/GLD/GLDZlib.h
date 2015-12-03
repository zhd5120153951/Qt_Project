#ifndef GLDZLIB_H
#define GLDZLIB_H

#include "GLDTypes.h"
#include "GLDStream.h"
#include "GLDZip_Global.h"

#define FRAGMENT_LENGTH_ZIP 0x100000  //16M, 当文件大于16M时，创建文件流缓存，否则是内存流
#define FRAGMENT_LENGTH_7Z  0x10000   //7z压缩解压缩的分片大小
#define ZLIB_HEADER_LENGTH  2         //zlib头长度

////////////////////////////////////////////////////////////////////////////////
//设计:创建一个策略类，根据用户传入参数，用工厂方法动态创建对象，屏蔽差异，用户不感知，方便扩展
//功能:根据用户传入的参数(文件名/文件大小)，创建文件流（当文件大小大于16M时）或者内存流
////////////////////////////////////////////////////////////////////////////////
class GStreamStrategy
{
public:
    void setStreamFragmentSize(gint64 nFragmentSize)
    {
        m_nFragMentSize = nFragmentSize;
    }
    gint64 streamFragmentSize()
    {
        return m_nFragMentSize;
    }
    GStream * stream()
    {
        return m_stream;
    }
    static void copyStreamByFragment(GStream *outStream, GStream *inStream, uint nFragmentLength, gint64 nCopySize);
    void createFileInDir(const GString &strFileName);

private:
    GStreamStrategy(const GString &strFileName, const GString &strDirName, const GString &strSuffix = "", bool bUseFileStreamFlag = true, gint64 nFragSize = 0x1000000);//16M
    GStreamStrategy(const GString &strFileName, gint64 streamSize, const GString &strSuffix = "", bool bUseFileStreamFlag = true, gint64 nFragSize = 0x1000000);

private:
    GStream *m_stream ; // 流指针
    gint64  m_nFragMentSize; //分片大小，用户可以指定，超过此大小的流需要创建文件流，否则创建块内存
    friend class CGLDZlib;
    friend class CGLDZipFileEntry;
};

class GLDZIPSHARED_EXPORT CGLDZlib
{
public:
    CGLDZlib();
    virtual ~CGLDZlib();
    static bool compress(GStream *inStream, GStream *outStream);
    static GStream* compress(GStream *inStream, bool bUseFileStreamFlag = true);
    static bool unCompress(GStream *inStream, GStream *outStream, GByteArray zlibHeader = "");
    static GStream* unCompress(GStream *inStream, bool bUseFileStreamFlag = true);
    static int newCompress(GStream *inStream, GStream *outStream, bool hasZlibHeaderFlag = true);
};

#ifdef WIN32
class GLDZIPSHARED_EXPORT CGLD7Zip
{
public:
    CGLD7Zip() {}
    virtual ~CGLD7Zip() {}
    // 需外部创建outStream
    static GStream* compress(GStream *inStream);
    static GStream* unCompress(GStream *inStream, int unCompressedSize = 0);
private:
    static bool compress(GStream *instream, GStream *outStream);
    static bool unCompress(GStream *instream, GStream *outStream, int unCompressedSize = 0);
    friend class CGLDAutoCompress;
};
#endif

class GLDZIPSHARED_EXPORT CGLDAutoCompress
{
public:
    CGLDAutoCompress() {}
    virtual ~CGLDAutoCompress() {}
    // 需外部创建outStream
    static GStream* compress(GStream *inStream);
    static GStream* unCompress(GStream *inStream);
};

#endif // GLDZLIB_H
