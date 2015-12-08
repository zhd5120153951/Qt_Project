#include "GLDZlib.h"

#include <QTemporaryFile>
#include <QDebug>
#ifndef WIN32
#   undef FAR
#   define FAR
#endif
#include "zlib.h"
#include "GLDStream.h"
#include "GLDSysUtils.h"
#include "GLDException.h"
#include "GLDStreamUtils.h"

const int c_fragment_length_7z = 0x100000;   //7z压缩解压缩的分片大小

////////////////////////////////////////////////////////////////////////////////
//设计:创建一个策略类，根据用户传入参数，用工厂方法动态创建对象，屏蔽差异，用户不感知，方便扩展
//功能:根据用户传入的参数(文件名/文件大小)，创建文件流（缺省当文件大小大于16M时）或者内存流
//参数：strFileName 输入文件名
//     strDirName  输入目录名
//     strFileName 输入文件后缀
//     nFragSize   分片大小，当超过此大小的文件分配文件流，此参数在bUseFileStreamFlag生效后起作用
//     bUseFileStreamFlag  是否启动临时文件缓存机制
////////////////////////////////////////////////////////////////////////////////
GStreamStrategy::GStreamStrategy(const GString &strFileName, const GString &strDirName, const GString &strSuffix, bool bUseFileStreamFlag, gint64 nFragSize) //16M
{
    if (false == bUseFileStreamFlag)
    {
        m_stream = new GBlockMemoryStream(GStream::ReadWrite);
        m_stream->open(GStream::ReadWrite);
        return;
    }

    setStreamFragmentSize(nFragSize);

    GFileStream fileStream(strDirName + strFileName);
    if (fileStream.exists())
    {
         fileStream.open(GFileStream::ReadOnly);
    }

    gint64 nStreamSize = fileStream.size();

    if (nStreamSize == 0)
    {
        m_stream = NULL;
    }
    else if (nStreamSize <= streamFragmentSize())
    {
        m_stream = new GBlockMemoryStream(GStream::ReadWrite);
        m_stream->open(GStream::ReadWrite);
    }
    else
    {
        //在系统临时目录创建临时文件
        this->createFileInDir(strFileName + strSuffix);
    }
}
////////////////////////////////////////////////////////////////////////////////
//设计:创建一个策略类，根据用户传入参数，用工厂方法动态创建对象，屏蔽差异，用户不感知，方便扩展
//功能:根据用户传入的参数(文件名/文件大小)，创建文件流（当文件大小大于16M时）或者内存流
//参数：strFileName 输入文件名
//     nStreamSize 输入文件大小
//     strSuffix   输入文件后缀
//     nFragSize   分片大小，当超过此大小的文件分配文件流，此参数在bUseFileStreamFlag生效后起作用
//     bUseFileStreamFlag  是否启动临时文件缓存机制
////////////////////////////////////////////////////////////////////////////////
GStreamStrategy::GStreamStrategy(const GString &strFileName, gint64 nStreamSize, const GString &strSuffix, bool bUseFileStreamFlag, gint64 nFragSize)
{
    if (false == bUseFileStreamFlag)
    {
        m_stream = new GBlockMemoryStream(GStream::ReadWrite);
        m_stream->open(GStream::ReadWrite);
        return;
    }

    setStreamFragmentSize(nFragSize);

    if (nStreamSize <= streamFragmentSize())
    {
        m_stream = new GBlockMemoryStream(GStream::ReadWrite);
    }
    else
    {
        this->createFileInDir(strFileName + strSuffix);
    }
}

////////////////////////////////////////////////////////////////////////////////
//设计: 根据用户指定的分片大小，分片拷贝流
//功能: 根据用户指定的分片大小，分片拷贝输入流的指定大小数据到输出流中
//参数： inStream          输入流
//      OutStream         输出流
//      nFragmentLength   分片长度
//      nCopySize         拷贝的长度
////////////////////////////////////////////////////////////////////////////////
void GStreamStrategy::copyStreamByFragment(GStream *outStream, GStream *inStream, uint nFragmentLength, gint64 nCopySize)
{
    assert(nFragmentLength != 0);

    uint nCount  = nCopySize / nFragmentLength;

    for (uint i = 0; i < nCount; ++i)
    {
        outStream->write(inStream->read(nFragmentLength));
    }
    outStream->write(inStream->read(nCopySize % nFragmentLength));
}

////////////////////////////////////////////////////////////////////////////////
//设计:根据指定路径创建文件（如果不存在的话）
//功能:根据指定路径创建文件（如果不存在的话），目前两个参数暂时不用，留作扩展，比如指定目录
//     生成指定名字文件
//参数：strFileName  文件名
////////////////////////////////////////////////////////////////////////////////
void GStreamStrategy::createFileInDir(const GString &strFileName)
{
    try
    {
        QTemporaryFile *tempFile = new QTemporaryFile("compressXXXXXX.tmp");
        m_stream = tempFile;
        m_stream->open(GFileStream::ReadWrite);
        tempFile->setAutoRemove(true);
    }

    catch (...)
    {
        throw GLDException("Internal Error: create file:"  + strFileName + "failed!");
    }
}

/* CGLDZlib */
CGLDZlib::CGLDZlib()
{

}

CGLDZlib::~CGLDZlib()
{
}

////////////////////////////////////////////////////////////////////////////////
//设计: 压缩函数，根据用户输入的hasZlibHeaderFlag判断输出带（值为1）或不带zlib压缩头的压缩输出流outStream
//功能: 压缩用户输如的原始数据，输出压缩后的数据，压缩后的数据包含zlib压缩头，
//参数：inStream  输入的原始数据
//     outStream 输出的原始数据
//     hasZlibHeaderFlag  为true时，输出流包含zlib压缩头； false时输出流不包含压缩头
////////////////////////////////////////////////////////////////////////////////
int CGLDZlib::newCompress(GStream *inStream, GStream *outStream, bool hasZlibHeaderFlag)
{
    int nRet = -1;
    int nnflush = -1;
    bool zlibHeaderFlag = hasZlibHeaderFlag;
    z_stream zcpr;
    const int c_DefMaxBufSize = 0x10000;
    Bytef *compressPtr = (Bytef *)gMalloc(sizeof(c_DefMaxBufSize) * (c_DefMaxBufSize + 1));
    unsigned long ninputSize = inStream->size();
    inStream->seek(0);
    /* allocate deflate state */
    zcpr.zalloc = Z_NULL;
    zcpr.zfree = Z_NULL;
    zcpr.opaque = Z_NULL;
    nRet = deflateInit(&zcpr, Z_DEFAULT_COMPRESSION);
    zcpr.next_out = compressPtr;
    if (nRet != Z_OK)
        return nRet;

    while (ninputSize != 0)
    {
        int ntmpSize = 0;
        if (ninputSize > c_DefMaxBufSize)
        {
            ntmpSize = c_DefMaxBufSize;
            ninputSize = ninputSize - c_DefMaxBufSize;
            nnflush = Z_NO_FLUSH;
        }
        else
        {
            ntmpSize = ninputSize;
            nnflush = Z_FINISH;
            ninputSize = 0;
        }

        GByteArray in = inStream->read(ntmpSize);

        zcpr.next_in = (Bytef *)(in.data());
        long lToDo = ntmpSize;
        long lReadBefore = 0;
        long lReadOutBefore = 0;
        do
        {
            lReadBefore = zcpr.total_in;
            lReadOutBefore = zcpr.total_out;
            zcpr.avail_in = lToDo > c_DefMaxBufSize? c_DefMaxBufSize : lToDo;
            zcpr.avail_out = c_DefMaxBufSize;
            nRet = deflate(&zcpr, nnflush);
            if (zlibHeaderFlag)
            {
                outStream->write((const char *)compressPtr, zcpr.total_out - lReadOutBefore);
            }
            else
            {
                outStream->write((const char *)compressPtr + ZLIB_HEADER_LENGTH, zcpr.total_out - lReadOutBefore - ZLIB_HEADER_LENGTH);
                zlibHeaderFlag = true ;
            }

            lToDo -= (zcpr.total_in - lReadBefore);
            zcpr.next_out = compressPtr;

        } while (nRet == Z_OK);
    }
    if (nRet >= 0)
    {
        deflateEnd(&zcpr);
    }
    freeAndNilByGFree(compressPtr);
    return nRet;
}
////////////////////////////////////////////////////////////////////////////////
//设计: 解压缩函数，根据用户输入的zlibHeader对输入流inStream添加或不添加zlib压缩头
//功能: 解压用户输入的压缩数据，输出解压缩数据给用户
//参数：inStream 用户输入的压缩数据
//     outStream 输出的解压缩数据
//     zlibHeader zlib压缩头，输入的inStream已包含压缩头时，该参数填空串
////////////////////////////////////////////////////////////////////////////////
bool CGLDZlib::unCompress(GStream *inStream, GStream *outStream, GByteArray zlibHeader)
{
    bool bResult = false;
    bool zlibHeaderFlag = false;
    int zlibHeaderLen  = ZLIB_HEADER_LENGTH;

    if (inStream == NULL)
        return bResult;

    const uint c_DefMaxBufSize = 0xf000; 
    const uint c_DefMaxInBufSize = 65536;
    z_stream zcpr;
    memset(&zcpr, 0, sizeof(z_stream));
    // 初始化
    inflateInit(&zcpr);
    Bytef *unCompressPtr = (Bytef *)gMalloc(sizeof(c_DefMaxBufSize) * (c_DefMaxBufSize + 1));

    if (zlibHeader.isEmpty())
    {
        zlibHeaderFlag = true;
        zlibHeaderLen  = 0;
    }

    GByteArray oTempArray(zlibHeader);
    zcpr.next_out = unCompressPtr;

    int nRet = Z_OK;
    ULONG lInStreamSize = inStream->size() + zlibHeaderLen;
    ULONG lTempLength = 0;

    // 当输入的流没有读完，则一直循环读取
    // 这里最好不要seek 0
    inStream->seek(0);
    ULONG lReadBefore = 0;
    ULONG lReadOutBefore = 0;
    int i = 0;
    int j = 0;
    while (lInStreamSize > 0)
    {
        i++;
        if (lInStreamSize > c_DefMaxBufSize)
        {
            lTempLength = c_DefMaxBufSize;
            lInStreamSize = lInStreamSize - c_DefMaxBufSize;
            nRet = Z_NO_FLUSH;
        }
        else
        {
            lTempLength = lInStreamSize;
            lInStreamSize = 0;
            nRet = Z_FINISH;
        }

        // 一段一段读取，防止GByteArray溢出
        if (zlibHeaderFlag)
        {
            oTempArray.clear();
            oTempArray = inStream->read(lTempLength);
        }
        else
        {
            oTempArray.append(inStream->read(lTempLength - zlibHeaderLen));
            zlibHeaderFlag = true ;
        }

        zcpr.next_in = (Bytef *)(oTempArray.data());

        long lToDo = lTempLength;
        do
        {
            j++;
            zcpr.avail_out = c_DefMaxBufSize;
            nRet = Z_OK;

            // 上一轮处理到的长度
            lReadBefore = zcpr.total_in;
            lReadOutBefore = zcpr.total_out;

            int nPreOutOfTemp = 0;
            int nPreInOfTemp = 0;
            int nCurOutOfTemp = 0;
            while ((zcpr.avail_out > 0) && (nRet != Z_STREAM_END))
            {
                // 第一进到循环中，可用空间肯定是0，需要分配空间，方便后面处理
                // 但是如果已经处理完，也应该是0，因此需要加上判断
                if (0 == zcpr.avail_in)
                {
                    zcpr.avail_in =
                            lToDo > long(c_DefMaxInBufSize) ? c_DefMaxInBufSize : lToDo;
                }
                // 处理完，直接Break
                if (0 == zcpr.avail_in)
                {
                    break;
                }

                // 已经处理的长度
                nPreInOfTemp = zcpr.total_in - lReadBefore;
                // 已经输出的长度
                nPreOutOfTemp = zcpr.total_out - lReadOutBefore;
                // 当轮处理的长度初始化为0
                nCurOutOfTemp = 0;

                // 解压缩处理
                nRet = inflate(&zcpr, Z_NO_FLUSH);

                if (nRet < 0)
                    break;

                lToDo = lToDo - (zcpr.total_in - lReadBefore);
                nCurOutOfTemp = zcpr.total_out - lReadOutBefore - nPreOutOfTemp;
                zcpr.next_in = (Bytef *)(oTempArray.data() + (lTempLength - lToDo));
            }

            zcpr.next_out = unCompressPtr;
            outStream->write((const char *)unCompressPtr, nCurOutOfTemp);

        } while (zcpr.total_out - lReadOutBefore == c_DefMaxBufSize);
    }

    if (nRet >= 0)
    {
        inflateEnd(&zcpr);
        bResult = true;
    }
    freeAndNilByGFree(unCompressPtr);

    return bResult;
}
////////////////////////////////////////////////////////////////////////////////
//设计: 对外解压缩函数，用户输入流inStream已包含zlib压缩头
//功能: 解压用户输入流中的压缩数据，输出解压缩数据给用户
//参数：inStream 输入的压缩数据流指针
//     bUseFileStreamFlag 是否启用文件缓存方式，当启动此方式否，如果输入文件大于分片大小
//     （缺省16M），则将分配文件流存储解压缩后的数据，返回给用户
////////////////////////////////////////////////////////////////////////////////
GStream* CGLDZlib::unCompress(GStream *inStream, bool bUseFileStreamFlag)
{
    if (inStream == NULL)
    {
        return false;
    }

    GStreamStrategy StreamStrategyTemp("_temp", inStream->size(),  "_temp_unzip", bUseFileStreamFlag);
    GStream *outStream = StreamStrategyTemp.stream();

    //用户输入流inStream已包含zlib压缩头,第三个参数填空串
    unCompress(inStream, outStream, "");
    if (outStream)
    {
        outStream->seek(0);
    }
    return outStream;
}
////////////////////////////////////////////////////////////////////////////////
//设计: 内部压缩函数，压缩输出流outStream不包含zlib头
//功能: 压缩用户输如的原始数据，输出压缩后的数据，压缩后的数据不包含zlib压缩头，主要给zipfile类用，
//     winzip等第三方工具加压时，如果压缩数据包含了zilb头，会解压失败
//参数：inStream  输入的原始数据
//     outStream 输出的原始数据
////////////////////////////////////////////////////////////////////////////////
bool CGLDZlib::compress(GStream *inStream, GStream *outStream)
{
    if (inStream == NULL)
    {
        return false;
    }

    //输出流outStream不需要包含zlib压缩头,第三个参数填false
    newCompress(inStream, outStream, false);
    if (outStream)
    {
        outStream->seek(0);
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////
//设计:对外压缩函数，返回参数输出流包含zlib压缩头
//功能: 压缩用户输如的原始数据，输出压缩后的数据，压缩后的数据包含zlib压缩头，
//参数：inStream  输入的原始数据
//     bUseFileStreamFlag 是否启用文件缓存方式，当启动此方式否，如果输入文件大于分片大小
//     （缺省16M），则将分配文件流存储压缩后的数据，返回给用户
////////////////////////////////////////////////////////////////////////////////
GStream* CGLDZlib::compress(GStream *inStream, bool bUseFileStreamFlag)
{
    if (inStream == NULL)
    {
        return false;
    }

    GStreamStrategy StreamStrategyTemp("_temp", inStream->size(), "_temp_zip", bUseFileStreamFlag);
    GStream *outStream = StreamStrategyTemp.stream();

    //对外压缩函数，返回参数outStream输出流包含zlib压缩头
    newCompress(inStream, outStream, true);
    if (outStream)
    {
        outStream->seek(0);
    }
    return outStream;
}

#ifdef WIN32
#include "LzmaLib.h"
/* CGLD7Zip */
bool CGLD7Zip::unCompress(GStream *instream, GStream *outStream, int unCompressedSize)
{
    int nOldPos = outStream->pos();
    try
    {
        const unsigned char prop[5] = {93, 0, 0, 0, 1}; //lzma解压缺省参数，这个需要与压缩时的输出proc参数值一致，否则解压会失败

        int nSrcSize = readIntFromStream(instream);
        int nDestLen = -1;
        size_t nRead = instream->read((char *)&nDestLen, sizeof(int));
        while(nRead > 0)
        {
            GByteArray srcRead = instream->read(nDestLen);
            nRead = srcRead.size();

            size_t ndead = nSrcSize;
//            size_t ndead = nRead * 2;
            unsigned char *aucDecomress = (unsigned char *)malloc(ndead);
            int ret = -1;
            if (SZ_OK != (ret = LzmaUncompress(aucDecomress, &ndead, (const uchar *)srcRead.data(), &nRead, prop, 5)))
            {
                //申请空间不足
                aucDecomress = (unsigned char *)realloc(aucDecomress, ndead);
                memset(aucDecomress, '\0', ndead);
                if (SZ_OK != (ret = LzmaUncompress(aucDecomress, &ndead, (const uchar *)srcRead.data(), &nRead, prop, 5)))
                {
                    qWarning() << "LzmaUncompress error:" << ret;
                }
            }
            //规避LzmaUncompress接口在解压最后一个分片，在后边多添加一个0的问题，这里需要截断多余字符，否则外层函数校验crc会失败
            if ((outStream->size() + ndead) >= unCompressedSize)
            {
                ndead = unCompressedSize - outStream->size();
                outStream->write((const char *)aucDecomress, ndead);
                break;
            }
            outStream->write((const char *)aucDecomress, ndead);
            nSrcSize = readIntFromStream(instream);
            nRead = instream->read((char *)&nDestLen, sizeof(int));
            free(aucDecomress);
            aucDecomress = NULL;
        }
    }
    catch (...)
    {
        throw;
    }
    outStream->seek(nOldPos);
    return true;
}

GStream *CGLD7Zip::unCompress(GStream *inStream, int unCompressedSize)
{
    GBlockMemoryStream *result = new GBlockMemoryStream;
    unCompress(inStream, result, unCompressedSize);
    return result;    
}

bool CGLD7Zip::compress(GStream *instream, GStream *outStream)
{
    size_t nDestLen = c_fragment_length_7z * 2;
    size_t nSizeProp = 5; //LzmaCompress参数aucProp的长度
    int ret;
    unsigned char aucProp[5] = {0};
    GByteArray srcRead;
    unsigned char *aucLzma = new unsigned char[c_fragment_length_7z * 2] ;

    int nOldPos = outStream->pos();
    int nCount = (instream->size() - instream->pos() + c_fragment_length_7z - 1) / c_fragment_length_7z; //计算分片次数，不足一次算一次
    for (int i = 0; i < nCount; ++i)
    {
        srcRead = instream->read(c_fragment_length_7z);
        nDestLen = c_fragment_length_7z * 2;

        memset(aucProp, 0, sizeof(aucProp));

        int nSrcSize = srcRead.size();
        if (SZ_OK != (ret = LzmaCompress(aucLzma, &nDestLen, (const uchar *)srcRead.data(), nSrcSize,
                                         aucProp, &nSizeProp, 0, (1 << 24), 3, 0, 2, 32, 2)))
        {
            outStream->seek(nOldPos);
            return false;
        }

        writeIntToStream(outStream, nSrcSize);
        writeIntToStream(outStream, nDestLen);
        outStream->write((const char *)aucLzma, nDestLen);
    }
    outStream->seek(nOldPos);
    delete(aucLzma);
    return true;
}

GStream *CGLD7Zip::compress(GStream *inStream)
{
    GBlockMemoryStream *result = new GBlockMemoryStream;
    compress(inStream, result);
    return result;
}
#endif

/* CGLDAutoCompress */
GStream *CGLDAutoCompress::unCompress(GStream *inStream)
{
    return CGLDZlib::unCompress(inStream);

    int nOldPos = inStream->pos();
    char szLength = readByteFromStream(inStream);
    inStream->seek(nOldPos);
    if (szLength == strlen("_7zip_"))
    {
        GString id = readStrFromStream(inStream);
        if (sameText(id, "_7zip_"))
        {
#ifdef WIN32
            return CGLD7Zip::unCompress(inStream, readIntFromStream(inStream));
#else
            assert(false);
#endif
        }
        inStream->seek(nOldPos);
    }
    return CGLDZlib::unCompress(inStream);
}

GStream *CGLDAutoCompress::compress(GStream *inStream)
{
    return CGLDZlib::compress(inStream);

#ifdef WIN32
    GBlockMemoryStream *result = new GBlockMemoryStream;

    writeStrToStream(result, "_7zip_");
    writeIntToStream(result, inStream->size() - inStream->pos());

    CGLD7Zip::compress(inStream, result);

    result->seek(0);
    return result;
#else
    return CGLDZlib::compress(inStream);
#endif
}
