#include "GLDZipFile.h"

#include "GLDZlib.h"
#include "GLDObject.h"
#include "GLDZipEnc.h"
#include "GLDStrings.h"
#include "GLDStrUtils.h"
#include "GLDSysUtils.h"
#include "GLDMathUtils.h"
#include "GLDException.h"
#include "GLDStreamUtils.h"
#ifdef WIN32
#include "LzmaLib.h"
#endif

interface IGLDZipPersistent;

#pragma pack(push, 1)
struct GLDCommonFileHeader
{
    unsigned short VersionNeededToExtract;      // 2 bytes
    unsigned short GeneralPurposeBitFlag;       // 2 bytes
    unsigned short CompressionMethod;           // 2 bytes
    unsigned int LastModFileTimeDate;           // 4 bytes
    unsigned int CRC32;                         // 4 bytes
    unsigned int CompressedSize;                // 4 bytes
    unsigned int UncompressedSize;              // 4 bytes
    unsigned short FilenameLength;              // 2 bytes
    unsigned short ExtraFieldLength;            // 2 bytes
};

#pragma pack(pop)

#pragma pack(push, 1)
struct GLDCentralFileHeader
{
    unsigned int CentralFileHeaderSignature;    // 4 bytes (0x02014b50, 'PK'12)
    unsigned short VersionMadeBy;               // 2 bytes
    GLDCommonFileHeader CommonFileHeader;         //
    unsigned short FileCommentLength;           // 2 bytes
    unsigned short DiskNumberStart;             // 2 bytes
    unsigned short InternalFileAttributes;      // 2 bytes
    unsigned int ExternalFileAttributes;        // 4 bytes
    unsigned int RelativeOffsetOfLocalHeader;   // 4 bytes
};

#pragma pack(pop)

#pragma pack(push, 1)
struct GLDEndOfCentralDirectory
{
    unsigned int EndOfCentralDirSignature;      // 4 bytes (0x06054b50, 'PK'56)
    unsigned short NumberOfThisDisk;                  // 2 bytes
    unsigned short NumberOfTheDiskWithTheStart;       // 2 bytes
    unsigned short TotalNumberOfEntriesOnThisDisk;    // 2 bytes
    unsigned short TotalNumberOfEntries;              // 2 bytes
    unsigned int SizeOfTheCentralDirectory;     // 4 bytes
    unsigned int OffsetOfStartOfCentralDirectory; // 4 bytes
    unsigned short ZipfileCommentLength;              // 2 bytes
};

#pragma pack(pop)

#pragma pack(push, 1)
struct GLDZipDataDescriptor
{
    unsigned int DataDescSignature;             // 4 bytes (0x08074B50, 'PK'78)
    unsigned int CRC32;                         // 4 bytes
    unsigned int CompressedSize;                // 4 bytes
    unsigned int UncompressedSize;              // 4 bytes
};

#pragma pack(pop)

#pragma pack(push, 1)
struct GLDZipFileCompressedData
{
    GStream *stream;                    // 4 bytes (0x08074B50, 'PK'78)
    long CRC32;                         // 4 bytes
    long CompressedSize;                // 4 bytes
    long UncompressedSize;              // 4 bytes
};

#pragma pack(pop)

DEFINE_IID(IGLDZipPersistent, "{5090E978-509E-43F6-8C97-32B9A49B6E3B}");
DECLARE_INTERFACE_(IGLDZipPersistent, IUnknown)
{
    GLDMETHOD(void, loadCentralDirectory)(GStream *stream) PURE;
    GLDMETHOD(void, loadDataDescriptor)(GStream *stream) PURE;
    GLDMETHOD(void, loadLocalFileHeader)(GStream *stream) PURE;
    GLDMETHOD(void, saveCentralDirectory)(GStream *stream) PURE;
    GLDMETHOD(void, saveDataDescriptor)(GStream *stream) PURE;
    GLDMETHOD(void, saveLocalFileHeader)(GStream *stream) PURE;
};

class CGLDZipFile;

class CGLDZipFileEntry: public GInterfaceObject, public IGLDZipFileEntry, public IGLDZipPersistent
{
public:
    CGLDZipFileEntry(CGLDZipFile *zipFile, const GString &name = "", unsigned int AttriBute = 0, double timeStamp = 0, const GString &dirname = "", unsigned short CompressionMethod = 8); //c_mDeflated = 8
    virtual ~CGLDZipFileEntry();
public:
    DECLARE_IUNKNOWN
    int GLDMETHODCALLTYPE attriButes();
    unsigned long GLDMETHODCALLTYPE compressedSize();
    unsigned short compressionMethod();
    unsigned long GLDMETHODCALLTYPE crc32();

    // 该接口会有GByteArray溢出情况，因此废弃该接口，请使用GStream *data()
    //    GByteArray GLDMETHODCALLTYPE data();
    GStream *GLDMETHODCALLTYPE data();

    double GLDMETHODCALLTYPE dateTime();
    GByteArray extrafield();
    GString fileComment();
    unsigned short internalAttributes();
    bool GLDMETHODCALLTYPE isEncrypted();
    GLDZipCompressionLevel level();
    GString GLDMETHODCALLTYPE name();
    IGLDZipFile* GLDMETHODCALLTYPE owner();
    unsigned long GLDMETHODCALLTYPE uncompressedSize();

    void GLDMETHODCALLTYPE setAttriButes(const int value);

    // 该接口会有GByteArray溢出情况，建议使用setData(GStream *value)
    void GLDMETHODCALLTYPE setData(GByteArray value);
    void GLDMETHODCALLTYPE setData(GStream *value);

    void GLDMETHODCALLTYPE setDateTime(const double value);

    void GLDMETHODCALLTYPE loadFromFile(const GString file);
    void GLDMETHODCALLTYPE saveToFile(const GString file);
    void GLDMETHODCALLTYPE loadFromStream(GStream *stream);
    void GLDMETHODCALLTYPE saveToStream(GStream *stream);
    void GLDMETHODCALLTYPE compressFromFile(const GString file);
    void GLDMETHODCALLTYPE decompressToFile(const GString file);

    /*IZipPersistent */
    void GLDMETHODCALLTYPE loadCentralDirectory(GStream *stream);
    void GLDMETHODCALLTYPE loadDataDescriptor(GStream *stream);
    void GLDMETHODCALLTYPE loadLocalFileHeader(GStream *stream);
    void GLDMETHODCALLTYPE saveCentralDirectory(GStream *stream);
    void GLDMETHODCALLTYPE saveDataDescriptor(GStream *stream);
    void GLDMETHODCALLTYPE saveLocalFileHeader(GStream *stream);

protected:
    HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject);

public:
    unsigned short versionMadeBy();
    void setExtrafield(const GByteArray &value);
    void setFileComment(const GString &value);
    void setLevel(const GLDZipCompressionLevel Value);

protected:
    IGLDZipFileEntry *intf();
    bool hasPassWord();

private:
    bool buildDecryptStream(GStream *stream, GLDZipDecryptStream **decrypter);
    bool buildEncryptStream(GStream *stream, GLDZipEncryptStream **encrypter);

    bool decryptZipData(GStream *data, GBlockMemoryStream *result);
    bool encryptZipData(GStream *data, GBlockMemoryStream *result);
    bool decompressZLibData(GStream *data, int size, GBlockMemoryStream *result);
#ifdef WIN32
    bool compress7ZData(GStream *result, GStream *data);
#endif
    bool compressZLibData(GStream *data, GStream *result);
    bool decompressZLibFile(GStream *data, const GString &file);
    bool compressZLibFile(const GString &file, GLDZipFileCompressedData *data);

    GStream *bZip2Data();
    GStream *storedData();
    GStream *zLibData();
    GByteArray zLibStreamHeader();

    void parseEntry();
    void setBZip2Data(GStream *data);
    void setStoredData(GStream *data);
    void setZLibData(GStream *data);
#ifdef WIN32
    void set7zData(GStream *data);
#endif
    CGLDZipFileEntry *intfObj();

	// add
	unsigned long zipUpdateCRC32FromStream(GStream *stream);
	bool encryptZipStream(GStream *stream);
//	void bZip2Stream(GStream *streamRult);
    void storedStream(GStream *streamResult);
    void zLibStream(GStream *streamRult);
#ifdef WIN32
    void store7zStream(GStream *streamRult);
#endif
    bool decompressToStream(GStream *instream, GStream *outStream);
#ifdef WIN32
    bool decompressTo7zStream(GStream *instream, GStream *outStream);
#endif
    bool decryptZipStream(GStream *streamResult);

private:
    CGLDZipFile *m_zipFile;
    GString m_fileName;
    GString m_dirName;
    GLDCentralFileHeader m_centralDirectory;
    GLDCommonFileHeader m_commonFileHeader;
    //GByteArray存在开辟失败问题
//    GByteArray m_compressedData;
    GStream *m_compressedData;

    GByteArray m_extrafield;
    GString m_fileComment;
    GStream *m_memStream;
    GLDZipCompressionLevel m_level;
};

typedef GInterfaceObjectList<CGLDZipFileEntry *> GLDZipFileEntryList;

class CGLDZipList
{
public:
    CGLDZipList();
    ~CGLDZipList();
public:
    int add(CGLDZipFileEntry *item);
    CGLDZipFileEntry* find(const GString &name);
    int indexOf(CGLDZipFileEntry *item);
    int indexOf(const GString &name);
    GString fileSuffixToLower(const GString &name);
    int remove(CGLDZipFileEntry *item);
    void clear();
    void deleteItem(int index);
    void insert(int index, CGLDZipFileEntry *item);
    int count();
    void setItem(int index, CGLDZipFileEntry *value);
    CGLDZipFileEntry* item(int index);
private:
    GLDZipFileEntryList m_list;
};

class CGLDZipFile: public GInterfaceObject, public IGLDZipFile
{
public:
    CGLDZipFile();
    ~CGLDZipFile();
public:
    DECLARE_IUNKNOWN
    /*IZipFile */
    int GLDMETHODCALLTYPE count();
    GByteArray GLDMETHODCALLTYPE customFileHeader();
    GString GLDMETHODCALLTYPE fileComment();
    IGLDZipFileEntry* GLDMETHODCALLTYPE items(int index);
    GLDZipCompressionLevel GLDMETHODCALLTYPE level();
    GString GLDMETHODCALLTYPE password();
    void GLDMETHODCALLTYPE setCustomFileHeader(GByteArray value);
    void GLDMETHODCALLTYPE setFileComment(const GString value);
    void GLDMETHODCALLTYPE setLevel(const GLDZipCompressionLevel value);
    void GLDMETHODCALLTYPE setPassword(const GString value);
    IGLDZipFileEntry* GLDMETHODCALLTYPE add(const GString name) /*overload */;
    IGLDZipFileEntry* GLDMETHODCALLTYPE add(const GString name, unsigned int attriBute, double timeStamp) /*overload */;
    int GLDMETHODCALLTYPE addFiles(const GString folder, const GString base = "", bool recursion = true,
                                   const GString fileMask = "", int searchAttr = 0);
    IGLDZipFileEntry* GLDMETHODCALLTYPE addFromBuffer(const GString &name, void *buffer, int count);
    IGLDZipFileEntry* GLDMETHODCALLTYPE addFromFile(const GString &name, const GString &file);
    IGLDZipFileEntry* GLDMETHODCALLTYPE addFromStream(const GString &name, GStream *stream);
    int GLDMETHODCALLTYPE indexOf(const GString name);
    IGLDZipFileEntry* GLDMETHODCALLTYPE find(const GString name);
    void GLDMETHODCALLTYPE clear();
    void GLDMETHODCALLTYPE deleteItem(int index);
    void GLDMETHODCALLTYPE extractFiles(const GString folder, const GString nameMask = "");
    void GLDMETHODCALLTYPE extractToBuffer(const GString name, int *buffer, int count);
    void GLDMETHODCALLTYPE extractToStream(const GString &name, GStream *stream);
    void GLDMETHODCALLTYPE extractToString(const GString name, GByteArray *text);
    void GLDMETHODCALLTYPE loadFromFile(const GString file);
    void GLDMETHODCALLTYPE saveToFile(const GString file);
    void GLDMETHODCALLTYPE loadFromStream(GStream *stream);
    void GLDMETHODCALLTYPE saveToStream(GStream *stream);
    IGLDZipGetPassWordEvent* GLDMETHODCALLTYPE getOnGetPassword();
    void GLDMETHODCALLTYPE setOnGetPassword(IGLDZipGetPassWordEvent *value);
protected:
    HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject);
public:
    CGLDZipFileEntry* itemObjs(int index);
    CGLDZipFileEntry* findObj(const GString &name);
    CGLDZipFileEntry* addObj(const GString &name);
    CGLDZipFileEntry* addObj(const GString &name, unsigned int attriBute, double timeStamp);
    CGLDZipFileEntry* addObj(const GString &name, const GString &dirname);
    CGLDZipFileEntry* addObj(const GString &name, unsigned int attriBute, double timeStamp, const GString &dirname);
    CGLDZipFileEntry* addFromBufferObj(const GString &name, void *buffer, int count);
    CGLDZipFileEntry* addFromFileObj(const GString &name, const GString &file);
    CGLDZipFileEntry* addFromStreamObj(const GString &name, GStream *stream);
    GString findPassWord(CGLDZipFileEntry *entry);
private:
    void loadEndOfCentralDirectory(GStream *stream);
    void saveEndOfCentralDirectory(GStream *stream, long centralDirectoryOffset);

    bool findSignature(GStream *stream, long signature, long endSignature = 0);
    bool findCustomFileHeader(GStream *stream);
    bool findCustomFileHeaderBySignature(GStream *stream);
private:
    CGLDZipList *m_files;
    GLDZipCompressionLevel m_level;
    GString m_fileComment, m_PassWord;
    GLDEndOfCentralDirectory m_endOfCentralDir;
    GByteArray m_customFileHeader;
    IGLDZipGetPassWordEvent *m_onGetPassword;
};

class CGLDBufferedStreamReader: public GStream
{
public:
    CGLDBufferedStreamReader(GStream *stream, int bufferSize = 1024);
    virtual ~CGLDBufferedStreamReader();
public:
    long read(long &buffer, long count);
    long write(const long buffer, long count);
    long seek(long offset, long origin);
private:
    void updateBufferFromPosition(int startPos);
private:
    GStream *m_stream;
    int m_streamSize;
    GByteArray m_buffer;
    int m_bufferSize;
    int m_bufferStartPosition;
    int m_virtualPosition;
};

//class CGLDBufferedFileReader: public CGLDBufferedStreamReader
//{
//public:
//    CGLDBufferedFileReader(const GString &fileName);
//    ~CGLDBufferedFileReader();
//};

//i18n
const char *rsErrEncryptedFile =    QT_TRANSLATE_NOOP("GLD", "Can`t handle the compressed file");//"文件被加密，无法处理");
const char *rsErrEncryptedZipData = QT_TRANSLATE_NOOP("GLD", "Compressed data errors");//"压缩数据出错");
const char *rsErrFileCrc32 =        QT_TRANSLATE_NOOP("GLD", "File %s CRC32 check error");//"文件 %s CRC32 检验出错");
const char *rsErrFilePassWord =     QT_TRANSLATE_NOOP("GLD", "File password is incorrect");//"文件密码不正确");
const char *rsErrFolderNotExist =   QT_TRANSLATE_NOOP("GLD", "The directory %s does not exist");//"目录 %s 不存在");
const char *rsErrIndexOutOfBand =   QT_TRANSLATE_NOOP("GLD", "Access index out of bounds");//"访问索引越界");
const char *rsErrUnSupportMethod =  QT_TRANSLATE_NOOP("GLD", "Unknown compression method %d");//"未知的压缩方法 %d");

const char *rsErrCompressFromFile = QT_TRANSLATE_NOOP("GLD", "Algorithm can only be the ZLIB, and can not be"\
                                                      "encrypted, and is mainly used for the compression of large"\
                                                      "files, and save memory");
//"压缩算法只能是 ZLIB，而且不能加密，主要用于超大文件的压缩，节省内存");

const char *rsErrDecompressFile = QT_TRANSLATE_NOOP("GLD", "Decompress algorithm only is the ZLIB and can not"\
                                                    "be encrypted, and is mainly used for the compression of "\
                                                    "large files, and save memory");
//"解压缩算法只能是 ZLIB，而且不能加密，主要用于超大文件的压缩，节省内存");

const char *rsErrDecompressToFile = QT_TRANSLATE_NOOP("GLD", "Extracting data to a file error");//"解压数据到文件出错");

const char *rsErrFileHeader = QT_TRANSLATE_NOOP("GLD", "Compressed file formats are illegal."\
                                                "We can not read file header information");
        //"压缩文件格式非法，无法读取文件头信息");

const char *rsErrExtractPassWord = QT_TRANSLATE_NOOP("GLD", "Incorrect extraction password");//"解压密码不正确");
const char *rsErrMaxLenPassWord = QT_TRANSLATE_NOOP("GLD", "Compression password longer than 80 characters");//"压缩密码长度超过 80 字符");

const GString c_defZipFileName = "=^0^=";

const int c_defFileIsEncrypted = 0x0001;
const int c_defHasDataDescriptor = 0x0008;
const long c_CentralFileHeaderSignature = 0x02014b50;
const long c_LocalFileHeaderSignature = 0x04034B50;
const long c_EndOfCentralDirSignature = 0x06054B50;
const long c_DataDescSignature = 0x08074B50;

const long c_FileHeaderSignature = 0x20444C47;
const int c_defMaxFileHeaderSize = 64;
const int c_defMaxPassWordLen = 80;

const int c_mStored              = 0;
//const int c_mShrunk              = 1;
//const int c_mReduced1            = 2;
//const int c_mReduced2            = 3;
//const int c_mReduced3            = 4;
//const int c_mReduced4            = 5;
//const int c_mImploded            = 6;
//const int c_mTokenizingReserved  = 7;
const int c_mDeflated            = 8;
//const int c_mDeflated64          = 9;
//const int c_mDCLImploding        = 10;
//const int c_mPKWAREReserved      = 11;
const int c_mBZip2               = 12;
const int c_7Zip                 = 15;
////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.05.10
//功能: 创建一个压缩文件子对象
//参数：
////////////////////////////////////////////////////////////////////////////////
IGLDZipFileEntry* createZipFileEntry(IGLDZipFile *zipFile, const GString &zipFileName)
{
    CGLDZipFileEntry *result = new CGLDZipFileEntry(dynamic_cast<CGLDZipFile *>(zipFile), zipFileName);
    return result;
}
////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.29
//功能: 创建一个压缩文件对象
//参数：
////////////////////////////////////////////////////////////////////////////////
IGLDZipFile* createZipFile(const GString &passWord)
{
    CGLDZipFile *result = new CGLDZipFile();
    result->setPassword(passWord);
    if (result)
        result->AddRef();
    return result;
}

GLDZipFile createZipFileSPtr(const GString &password)
{
    CGLDZipFile *result = new CGLDZipFile();
    result->setPassword(password);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.29
//功能: 由文件加载对象
//参数：
////////////////////////////////////////////////////////////////////////////////
IGLDZipFile* loadZipFile(const GString &file, const GString &passWord)
{
    IGLDZipFile *result = createZipFile(passWord);
    result->loadFromFile(file);
    return result;
}

GLDZipFile loadZipFileSPtr(const GString &file, const GString &password)
{
    GLDZipFile result = createZipFileSPtr(password);
    result.loadFromFile(file);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.29
//功能: 由内存流加载对象
//参数：
////////////////////////////////////////////////////////////////////////////////
IGLDZipFile* loadZipFile(GStream *stream, const GString &passWord)
{
    assert(stream != NULL);
    IGLDZipFile* result = createZipFile(passWord);
    result->loadFromStream(stream);
    return result;
}

GLDZipFile loadZipFileSPtr(GStream *stream, const GString &password)
{
    assert(NULL != stream);
    GLDZipFile result = createZipFileSPtr(password);
    result.loadFromStream(stream);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2009.05.13
//功能：压缩文件
//参数：
////////////////////////////////////////////////////////////////////////////////
int compressZipFiles(const GString &file, const GString &folder, const GString &nameMask)
{
    int result = 0;
    GLDZipFile oZip = createZipFileSPtr("");
    try
    {
        oZip.addFiles(folder, "", true, nameMask);
        oZip.saveToFile(file);
        result = oZip.count();
    }
    catch (...)
    {
        result = 0;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2009.05.12
//功能：解压文件
//参数：
////////////////////////////////////////////////////////////////////////////////
int extractZipFiles(const GString &file, const GString &folder, const GString &nameMask)
{
    int result = 0;
    try
    {
        GLDZipFile oZip = loadZipFileSPtr(file, "");
        oZip.extractFiles(folder, nameMask);
        result = oZip.count();
    }
    catch(...)
    {
        result = 0;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.30
//功能: 转换文件路径
//参数：
////////////////////////////////////////////////////////////////////////////////
GString fileNameToZipName(const GString &name)
{
    GString result = stringReplace(name, "\\", "/");
    int nIndex = result.indexOf(":/");
    if (nIndex >= 0)
    {
        result = result.mid(nIndex);
    }
    nIndex = result.indexOf("//");
    if (nIndex >= 0)
    {
        result = result.mid(nIndex);
        nIndex = result.indexOf("/");
        if (nIndex >= 0)
        {
            result = result.mid(nIndex);
            nIndex = result.indexOf("/");
            if (nIndex >= 0)
                result = result.mid(nIndex);
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2010.03.01
//功能：在缓冲区中查找指定内容
//参数：
//注意：偷懒，直接借助 Pos 的实现，多了一个开辟内存的动作
////////////////////////////////////////////////////////////////////////////////
int posData(int data, int len, const GByteArray &s)
{
    GByteArray strData = GByteArray();
    strData = strData.append((char *)&data, len);
    GString res = GString(s);
    GString des = GString(strData);
    return res.indexOf(des);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.30
//功能: 转换文件路径
//参数：
////////////////////////////////////////////////////////////////////////////////
GString zipNameToFileName(const GString &name)
{
    return stringReplace(name, "/", "\\");
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.22
//功能: 判断一个文件是否为压缩文件
//参数：
////////////////////////////////////////////////////////////////////////////////
bool isZipFile(const GString &file)
{
    bool result = false;

    GFileStream fileStream(file);
    try
    {
        if (!fileStream.open(GFileStream::ReadOnly))
        {
            fileStream.close();
            return false;
        }
        fileStream.seek(0);
        result = isZipStream(&fileStream);
    }
    catch(...)
    {
        fileStream.close();
        throw;
    }
    fileStream.close();
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.22
//功能: 判断一个流是否为 ZIP 压缩流
//参数：
//注意：可能需要跳过存在的自定义数据头
//      如果非标准的自定义头，默认只判断开始的 64 字节内容 defMaxFileHeaderSize
//      读流时避免再多读头 4 个字节一次
//      返回 True 时 Position 会定位到 conLocalFileHeaderSignature 之后的位置
////////////////////////////////////////////////////////////////////////////////
bool isZipStream(GStream *stream)
{
    bool bresult = false;
    int nLen(0);
    int nPos(0);
    int nPosition(0);
    unsigned long uSignature;
    assert(stream != NULL);
    try
    {
        stream->read((char *)&uSignature, sizeof(uSignature));
        if (uSignature == (unsigned long)c_LocalFileHeaderSignature)
            bresult = true;
        else if (uSignature == (unsigned long)c_FileHeaderSignature)
        {
            stream->read((char *)&nLen, sizeof(unsigned long));
            nPosition = stream->pos();
            stream->seek(nPosition + nLen);
            stream->read((char *)&uSignature, sizeof(unsigned long));
            bresult = (uSignature == (unsigned long)c_LocalFileHeaderSignature);
        }
        else
        {
            GByteArray strBuffer;
            strBuffer.resize(c_defMaxFileHeaderSize);
            strBuffer.append((char *)&uSignature, sizeof(unsigned long));
            nLen = stream->read(strBuffer.data() + sizeof(unsigned long), strBuffer.length() - sizeof(unsigned long));
            nLen += sizeof(unsigned long);
            strBuffer.resize(nLen);
            nPos = posData(c_LocalFileHeaderSignature, sizeof(unsigned long), strBuffer);
            bresult = nPos > 0;
            if (bresult)
            {
                nPosition = stream->pos();
                stream->seek(nPosition + nPos - nLen + sizeof(unsigned long) - 1);
            }
        }
    }
    catch (...)
    {
        bresult = false;
    }
    return bresult;
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2009.06.23
//功能：判断文件是否加密
//参数：
////////////////////////////////////////////////////////////////////////////////
bool isEncryptedZipFile(const GString &file)
{
    bool result = false;
    GFileStream *stream = new GFileStream(file);
    try
    {
        stream->open(GFileStream::ReadOnly);
        stream->seek(0);
        result = isEncryptedZipStream(stream);
    }
    catch (...)
    {
        stream->close();
        freeAndNil(stream);
        throw;
    }
    stream->close();
    freeAndNil(stream);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2009.06.23
//功能：判断 ZIP 数据流是否加密
//参数：
////////////////////////////////////////////////////////////////////////////////
bool isEncryptedZipStream(GStream *stream)
{
    bool result = isZipStream(stream);
    if (result)
    {
        try
        {
            unsigned short uValue;
            // read 2 bytes for VersionNeededToExtractSize
            stream->read((char *)&uValue, sizeof(unsigned short));
            // read 2 bytes for GeneralPurposeBitFlagSize
            stream->read((char *)&uValue, sizeof(unsigned short));
            // check the encryption flag
            result = ((uValue & c_defFileIsEncrypted) != 0);
        }
        catch (...)
        {
            result = false;
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.18
//功能: 压缩字串
//参数：
////////////////////////////////////////////////////////////////////////////////
GStream *zipText(const GByteArray &s)
{
    GBlockMemoryStream *pResultStream = new GBlockMemoryStream(GStream::ReadWrite);
    GBlockMemoryStream *pStreamTemp = new GBlockMemoryStream(GStream::ReadWrite);

    try
    {
        GLDZipFile cZipFile = createZipFileSPtr();

        GBlockMemoryStream *pData = new GBlockMemoryStream(GStream::ReadWrite);
        pData->write(s);

        cZipFile.add(c_defZipFileName).setData(pData);
        cZipFile.saveToStream(pStreamTemp);
        pStreamTemp->seek(0);
        pResultStream->loadFromStream(pStreamTemp);
    }
    catch (...)
    {
        freeAndNil(pStreamTemp);
        throw;
    }
    freeAndNil(pStreamTemp);
    return pResultStream;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.18
//功能: 解压缩字串
//参数：
////////////////////////////////////////////////////////////////////////////////
GStream *unzipText(const GByteArray &s)
{
    GBlockMemoryStream *pResult = new GBlockMemoryStream(GStream::ReadWrite);
    GLDZipFile cZipFile;
    GBlockMemoryStream *pStream = new GBlockMemoryStream(GStream::ReadWrite);
    try
    {
        pStream->write(s.data(), s.length());
        pStream->seek(0);
        cZipFile = loadZipFile(pStream);
        if (cZipFile.count() > 0)
        {
            pResult->loadFromStream(cZipFile.items(0).data());
        }
        else
        {
            pResult->clearData();
        }
    }
    catch (...)
    {
        freeAndNil(pStream);
        throw;
    }
    freeAndNil(pStream);
    return pResult;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.25
//功能: 压缩流
//参数：
//注意：不能直接压缩 TFileStream，改函数会把处理的结果回写到 Stream 中
////////////////////////////////////////////////////////////////////////////////
bool zipStream(GStream *stream, const GString &passWord)
{
    GBlockMemoryStream *pStreamData = new GBlockMemoryStream(GStream::ReadWrite);

    assert(stream != NULL);
    stream->seek(0);
    GLDZipFile oZipFile = createZipFileSPtr(passWord);
    try
    {
        int nSize = stream->size();
        if (nSize == 0)
        {
            pStreamData->clearData();
        }
        else
        {
            stream->seek(0);
            pStreamData->loadFromStream(stream);
        }
        pStreamData->seek(0);
        oZipFile.add(c_defZipFileName).setData(pStreamData);

        stream->reset();
        oZipFile.saveToStream(stream);
    }
    catch (...)
    {
        freeAndNil(pStreamData);
        return false;
    }
    freeAndNil(pStreamData);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.25
//功能: 解压压缩流
//参数：
//注意：不能直接压缩 TFileStream，改函数会把处理的结果回写到 Stream 中
////////////////////////////////////////////////////////////////////////////////
bool unZipStream(GStream *stream, const GString &passWord)
{
    GBlockMemoryStream *pStreamData = new GBlockMemoryStream(GStream::ReadWrite);

    assert(stream != NULL);
    stream->seek(0);

    GLDZipFile cZipFile = loadZipFileSPtr(stream, passWord);
    try
    {
        pStreamData->loadFromStream(cZipFile.items(0).data());
        stream->reset();
        stream->write(pStreamData->readAll(), pStreamData->size());
    }
    catch(...)
    {
        freeAndNil(pStreamData);
        return false;
    }
    freeAndNil(pStreamData);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.11.20
//功能: 压缩流
//参数：
////////////////////////////////////////////////////////////////////////////////
bool zipStream(GStream *srcStream, GStream *desStream, const GString &passWord)
{
    GBlockMemoryStream *pStreamData = new GBlockMemoryStream(GStream::ReadWrite);

    assert(srcStream != NULL);
    assert(desStream != NULL);
    int nSize = srcStream->size();
    if (nSize == 0)
    {
        pStreamData->clearData();
    }
    else
    {
        srcStream->seek(0);
        pStreamData->loadFromStream(srcStream);
    }

    GLDZipFile cZipFile = createZipFileSPtr(passWord);
    try
    {
        cZipFile.add(c_defZipFileName);
        pStreamData->seek(0);
        cZipFile.items(0).setData(pStreamData);
        desStream->reset();
        cZipFile.saveToStream(desStream);
    }
    catch(...)
    {
        freeAndNil(pStreamData);
        return false;
    }
    freeAndNil(pStreamData);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.11.21
//功能: 解压缩流
//参数：
////////////////////////////////////////////////////////////////////////////////
bool unZipStream(GStream *srcStream, GStream *desStream, const GString &passWord)
{
//    GByteArray strData;
    assert(srcStream != NULL);
    assert(desStream != NULL);
    GLDZipFile oZipFile = createZipFileSPtr(passWord);
    try
    {
        srcStream->seek(0);
        oZipFile.loadFromStream(srcStream);
        desStream->seek(0);
        oZipFile.items(0).saveToStream(desStream);
//        strData = oZipFile.items(0).data();
//        desStream->reset();
//        desStream->write(strData.data(), strData.length());
    }
    catch(...)
    {
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.28
//功能: 抛出 ZIP 异常
//参数：
////////////////////////////////////////////////////////////////////////////////
void zipFileError(const GString Msg)
{
    throw GLDException(Msg);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.28
//功能: 抛出 ZIP 异常
//参数：
////////////////////////////////////////////////////////////////////////////////
//void zipFileError(const GString Msg, const  TVarRec *Args, int Args_size) /*overload */
//{
//    throw EZipFileException createFmt(Msg, Args);
//}

/*TBufferedStreamReader */
CGLDBufferedStreamReader::CGLDBufferedStreamReader(GStream *stream, int bufferSize)
{
    m_stream = stream;
    m_streamSize = stream->size();
    m_bufferSize = bufferSize;
    m_buffer.resize(bufferSize);
    m_bufferStartPosition = -m_bufferSize; /*out of any useful range */
    m_virtualPosition = 0;
}

CGLDBufferedStreamReader::~CGLDBufferedStreamReader()
{
}

long CGLDBufferedStreamReader::read(long &Buffer, long Count)
{
    long result = 0;
    char *Buf;
    int nBytesLeft;
    int nFirstBufferRead;
    int nStreamDirectRead;

    if ((m_virtualPosition >= 0) && (Count >= 0))
    {
        result = m_stream->size() - m_virtualPosition;
        if (result > 0)
        {
            if (result > Count)
                result = Count;
            Buf = (char *)&Buffer;
            nBytesLeft = result;

            nFirstBufferRead = m_bufferStartPosition + m_bufferSize - m_virtualPosition;
            if ((nFirstBufferRead < 0) || (nFirstBufferRead > m_bufferSize))
                nFirstBufferRead = 0;
            nFirstBufferRead = nFirstBufferRead > result?result:nFirstBufferRead;
            if (nFirstBufferRead > 0)
            {
                for (int i = 0; i < nFirstBufferRead; ++i)
                {
                    *(Buf++) = m_buffer.at(m_virtualPosition - m_bufferStartPosition + i);
                }
                nBytesLeft -= nFirstBufferRead;
            }

            if (nBytesLeft > 0)
            {
                nStreamDirectRead = (nBytesLeft / m_bufferSize) * m_bufferSize;
                m_stream->seek(m_virtualPosition + nFirstBufferRead);
                m_stream->read(Buf + nFirstBufferRead, nStreamDirectRead);
                nBytesLeft -= nStreamDirectRead;

                if (nBytesLeft > 0)
                {
                    updateBufferFromPosition(m_stream->pos());
                    for (int i = 0; i < nBytesLeft; ++i)
                    {
                        *(Buf + nFirstBufferRead + nStreamDirectRead + i) = m_buffer.at(i);
                    }
                }
            }
            m_virtualPosition += result;
            return result;
        }
    }
    return 0;
}

long CGLDBufferedStreamReader::seek(long Offset, long Origin)
{
    switch (Origin)
    {
    case soBeginning:
        m_virtualPosition = Offset;
        break;
    case soCurrent:
        m_virtualPosition += Offset;
        break;
    case soEnd:
        m_virtualPosition = m_streamSize + Offset;
        break;
    default:
        assert(false);
    }
    return m_virtualPosition;
}

void CGLDBufferedStreamReader::updateBufferFromPosition(int startPos)
{
    try
    {
        m_stream->seek(startPos);
        m_stream->read(m_buffer.data(), m_bufferSize);
        m_bufferStartPosition = startPos;
    }
    catch(...)
    {
        m_bufferStartPosition -= m_bufferSize;
        throw; /*out of any useful range */
    }
}

long CGLDBufferedStreamReader::write(const long buffer, long count)
{
    zipFileError(getGLDi18nStr("Internal Error: class can not write."));
    G_UNUSED(buffer);
    G_UNUSED(count);
    return 0;
}

/*TBufferedFileReader */
////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2010.03.01
//功能：
//参数：
//注意：不支持大于 2G 的文件，且只支持文件读
////////////////////////////////////////////////////////////////////////////////
//CGLDBufferedFileReader::CGLDBufferedFileReader(const GString &fileName)
//    : CGLDBufferedStreamReader(new GFileStream(fileName))
//{
//}

//CGLDBufferedFileReader::~CGLDBufferedFileReader()
//{
//    // todo check:  inherited::destroy();
//    //  freeAndNil(m_stream);
//}

/*CGLDZipFileEntry */
////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.01
//功能: 返回一个解压数据对象
//参数：
////////////////////////////////////////////////////////////////////////////////
bool CGLDZipFileEntry::buildDecryptStream(GStream *stream, GLDZipDecryptStream **decrypter)
{
    bool result = false;
    unsigned long lEncCRC;
    assert(stream != NULL);
    if ((m_commonFileHeader.GeneralPurposeBitFlag & c_defHasDataDescriptor) != 0)
        lEncCRC = m_commonFileHeader.LastModFileTimeDate << 0x10;
    else
        lEncCRC = m_commonFileHeader.CRC32;

    GString strPassword = m_zipFile->findPassWord(this);
    if (strPassword.length() == 0)
        zipFileError(getGLDi18nStr(rsErrExtractPassWord));
    *decrypter = new GLDZipDecryptStream(stream, lEncCRC, strPassword);
    try
    {
        result = (*decrypter)->isValid();
    }
    catch (...)
    {
        if (!result)
            freeAndNil(*decrypter);
        throw;
    }
    if (!result)
        freeAndNil(*decrypter);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.01
//功能: 返回一个压缩数据处理对象
//参数：
////////////////////////////////////////////////////////////////////////////////
bool CGLDZipFileEntry::buildEncryptStream(GStream *stream, GLDZipEncryptStream **encrypter)
{
    bool result;
    unsigned long lEncCRC;
    assert(stream != NULL);
    if ((m_commonFileHeader.GeneralPurposeBitFlag & c_defHasDataDescriptor) != 0)
        lEncCRC = m_commonFileHeader.LastModFileTimeDate << 0x10;
    else
        lEncCRC = m_commonFileHeader.CRC32;
    try
    {
        *encrypter = new GLDZipEncryptStream(stream, lEncCRC, m_zipFile->password());
        result = true;
    }
    catch (...)
    {
        result = false;
    }
    if (!result)
        freeAndNil(encrypter);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2009.02.17
//功能：直接使用 ZLIB 算法由文件压缩数据
//参数：
//注意：压缩算法只能是 ZLIB，而且不能加密，主要用于超大文件的压缩，节省内存
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::compressFromFile(const GString file)
{
    GLDZipFileCompressedData oData;
    oData.stream = m_memStream;

    if (hasPassWord() || (m_commonFileHeader.CompressionMethod != c_mDeflated))
        zipFileError(getGLDi18nStr(rsErrCompressFromFile));

    if (!compressZLibFile(GString(file), &oData))
        zipFileError(getGLDi18nStr(rsErrEncryptedZipData));

    m_commonFileHeader.CRC32 = oData.CRC32;
    m_commonFileHeader.UncompressedSize = oData.UncompressedSize;
//    m_compressedData = oData.CompressedData.toLocal8Bit();
//	m_memStream->loadFromFile(file);
//	m_memStream->seek(0);
//	m_memStream->write(oData.CompressedData.toLocal8Bit());
//	m_memStream->loadFromStream(oData.CompressedData.toLocal8Bit())
    m_commonFileHeader.CompressedSize = oData.CompressedSize;
    m_centralDirectory.CommonFileHeader = m_commonFileHeader;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.05.29
//功能: 使用 7Z 算法压缩数据
//参数：streamResult 输出流指针，存放压缩后数据
//     data         存放输入数据指针
////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
bool CGLDZipFileEntry::compress7ZData(GStream *streamResult, GStream *data)
{
     size_t nDestLen = FRAGMENT_LENGTH_7Z*2;
     size_t nSizeProp = 5; //LzmaCompress参数aucProp的长度
     int ret;
     unsigned char aucProp[5] = {0};
     GByteArray srcRead;
     unsigned char aucLzma[FRAGMENT_LENGTH_7Z*2] ;

     data->seek(0);
     int nCount  = (data->size() + FRAGMENT_LENGTH_7Z - 1) / FRAGMENT_LENGTH_7Z; //计算分片次数，不足一次算一次
     for (int i = 0; i < nCount; ++i)
     {
         srcRead = data->read(FRAGMENT_LENGTH_7Z);
         nDestLen = FRAGMENT_LENGTH_7Z*2;

         memset(aucProp,0,sizeof(aucProp));

         if (SZ_OK != (ret = LzmaCompress(aucLzma, &nDestLen, (const uchar *)srcRead.data(), srcRead.size(), aucProp,
                                   &nSizeProp, 9, (1 << 24), 3, 0, 2, 32, 2)))
         {
             return  false;
         }

         streamResult->write((const char *)&nDestLen, sizeof(nDestLen));
         streamResult->write((const char *)aucLzma, nDestLen);
     }

     return true;
}
#endif

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.01
//功能: 使用 ZLIB 算法压缩数据
//参数：
////////////////////////////////////////////////////////////////////////////////
bool CGLDZipFileEntry::compressZLibData(GStream *data, GStream *result)
{
    bool bResult ;
    try
    {   
        bResult = CGLDZlib::compress(data, result);
    }
    catch (...)
    {
        throw;
        return false;
    }
    return bResult;
}
////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2008.02.17
//功能: 使用 ZLIB 算法压缩文件
//参数：
////////////////////////////////////////////////////////////////////////////////
bool CGLDZipFileEntry::compressZLibFile(const GString &file, GLDZipFileCompressedData *data)
{
    bool result = false;

    GFileStream *fileStream = new GFileStream(file);
    try
    {
        fileStream->open(GFileStream::ReadOnly);

        if (!compressZLibData(fileStream, data->stream))
        {
            zipFileError(getGLDi18nStr(rsErrEncryptedZipData));
        }

        data->CRC32 = zipCRC32(fileStream);
        data->CompressedSize = data->stream->size();
        data->UncompressedSize = fileStream->size();
        result = true;
    }
    catch (...)
    {
        fileStream->close();
        freeAndNil(fileStream);
        throw;
    }
    fileStream->close();
    freeAndNil(fileStream);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.01
//功能: 初始化
//参数：
////////////////////////////////////////////////////////////////////////////////
CGLDZipFileEntry::CGLDZipFileEntry(CGLDZipFile *zipFile, const GString &name, unsigned int AttriBute, double timeStamp, const GString &dirname, unsigned short CompressionMethod)
{
    //double nTimeStamp;
    m_zipFile = zipFile;

//    if (timeStamp == 0)
//        nTimeStamp = dateTimeToDouble(GDateTime::currentDateTime());
//    else
//        nTimeStamp = timeStamp;

    if (zipFile != NULL)
        m_level = zipFile->level();
    else
        m_level = ctDefault;

    m_fileName = fileNameToZipName(name);
    m_dirName  = dirname;

    GStreamStrategy GStreamStrategyTemp(name, dirname);
    m_memStream = GStreamStrategyTemp.stream();

    m_compressedData = m_memStream;

    m_extrafield = GByteArray();
    m_commonFileHeader.VersionNeededToExtract = 20;
    m_commonFileHeader.GeneralPurposeBitFlag = 0;

    m_commonFileHeader.CompressionMethod = CompressionMethod;
    m_commonFileHeader.LastModFileTimeDate = DateTimeToFileDate(timeStamp);
    m_commonFileHeader.CRC32 = 0;
    m_commonFileHeader.CompressedSize = 0;
    m_commonFileHeader.UncompressedSize = 0;
    m_commonFileHeader.FilenameLength = m_fileName.toLocal8Bit().length();
    m_commonFileHeader.ExtraFieldLength = m_extrafield.length();

    m_centralDirectory.CentralFileHeaderSignature = c_CentralFileHeaderSignature;
    m_centralDirectory.VersionMadeBy = 20;
    m_centralDirectory.CommonFileHeader = m_commonFileHeader;
    m_centralDirectory.FileCommentLength = 0;
    m_centralDirectory.DiskNumberStart = 0;
    m_centralDirectory.InternalFileAttributes = 0;
    m_centralDirectory.ExternalFileAttributes = AttriBute;
    m_centralDirectory.RelativeOffsetOfLocalHeader = 0;
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2009.02.17
//功能：直接使用 ZLIB 算法解压数据到文件
//参数：
//注意：压缩算法只能是 ZLIB，而且不能加密，主要用于超大文件的压缩，节省内存
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::decompressToFile(const GString file)
{
    if (isEncrypted() || (m_commonFileHeader.CompressionMethod != c_mDeflated))
        zipFileError(getGLDi18nStr(rsErrDecompressFile));
     if (!decompressZLibFile(m_memStream, file))
        zipFileError(getGLDi18nStr(rsErrDecompressToFile));
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.01
//功能: 解压缩 ZLIB 压缩数据
//参数：
////////////////////////////////////////////////////////////////////////////////
bool CGLDZipFileEntry::decompressZLibData(GStream *data, int size, GBlockMemoryStream *result)
{
    bool bResult = false;

    result->clearData();
    try
    {
        bResult = CGLDZlib::unCompress(data, result, zLibStreamHeader());
        if (result->size() != 0)
        {
            try
            {
                bResult = (result->size() == size);
                if (!bResult)
                    result->clearData();
            }
            catch (...)
            {
                throw;
            }
        }
    }
    catch (...)
    {
        throw;
    }

    return bResult;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2008.02.17
//功能: 使用 ZLIB 算法解压缩文件
//参数：
////////////////////////////////////////////////////////////////////////////////
bool CGLDZipFileEntry::decompressZLibFile(GStream *data, const GString &file)
{
    bool bResult = false;
    GFileStream* fileStream = new GFileStream(file);

    try
    {
        fileStream->open(GFileStream::ReadWrite);
        try
        {
            bResult = CGLDZlib::unCompress(data, fileStream, zLibStreamHeader());
        }
        catch (...)
        {
            fileStream->close();
            freeAndNil(fileStream);
            throw;
        }
    }
    catch (...)
    {
        fileStream->close();
        freeAndNil(fileStream);
        throw;
    }

    fileStream->close();
    freeAndNil(fileStream);
    return bResult;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.01
//功能: 解密 PKZIP 数据
//参数：
////////////////////////////////////////////////////////////////////////////////
bool CGLDZipFileEntry::decryptZipData(GStream *data, GBlockMemoryStream *result)
{
    bool bResult = false;
    int nSize = data->size();

    result->loadFromStream(data);
    GLDZipDecryptStream *decrypter = NULL;
    try
    {
        data->seek(0);
        if (buildDecryptStream(data, &decrypter))
        {
            try
            {
                nSize = nSize - sizeof(BYTE[12]);
                result->seek(nSize);
                decrypter->seek(0);
                result->loadFromStream(decrypter, nSize);
                bResult = true;
            }
            catch (...)
            {
                freeAndNil(decrypter);
                throw;
            }
            freeAndNil(decrypter);
        }
        else
            zipFileError(getGLDi18nStr(rsErrExtractPassWord));
    }
    catch (...)
    {
        throw;
    }
    return bResult;
}

CGLDZipFileEntry::~CGLDZipFileEntry()
{
    if ((NULL != m_memStream))
    {       
        freeAndNil(m_memStream);
    }
    // todo check:  inherited::destroy();
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.01
//功能: 使用 PKZIP 加密算法处理数据
//参数：
////////////////////////////////////////////////////////////////////////////////
bool CGLDZipFileEntry::encryptZipData(GStream *data, GBlockMemoryStream *result)
{
    bool bResult = false;
    int nSize;
    const int c_defBufferSize = 32768;
    result->loadFromStream(data);

    GLDZipEncryptStream *encrypter = NULL;
    GBlockMemoryStream *pStream = new GBlockMemoryStream(GStream::ReadWrite);
    try
    {
        if (buildEncryptStream(pStream, &encrypter))
        {
            try
            {
                nSize = data->size();
                int nCopySize = 0;
                int nCount = nSize / c_defBufferSize;
                for (int i = 0; i < nCount; ++i)
                {
                    nCopySize += encrypter->write(data->read(c_defBufferSize));
                }
                nCopySize += encrypter->write(data->read(nSize % c_defBufferSize));

                bResult = (nCopySize == nSize);
            }
            catch (...)
            {
                freeAndNil(encrypter);
                throw;
            }
            freeAndNil(encrypter);
        }
        if (bResult)
        {
            result->seek(pStream->size());
        }
        pStream->seek(0);
        result->loadFromStream(pStream);
    }
    catch (...)
    {
        freeAndNil(pStream);
        throw;
    }
    freeAndNil(pStream);
    return bResult;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.29
//功能: 保存的文件属性
//参数：
////////////////////////////////////////////////////////////////////////////////
int CGLDZipFileEntry::attriButes()
{
    return m_centralDirectory.ExternalFileAttributes;
}

/*CGLDZipFileEntryBZip2Processor */
GStream *CGLDZipFileEntry::bZip2Data()
{
    assert(false);
    return new GBlockMemoryStream();//todo
}

unsigned long CGLDZipFileEntry::compressedSize()
{
    return m_commonFileHeader.CompressedSize;
}

unsigned short CGLDZipFileEntry::compressionMethod()
{
    return m_commonFileHeader.CompressionMethod;
}

unsigned long CGLDZipFileEntry::crc32()
{
    return m_commonFileHeader.CRC32;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.30
//功能: 解压数据
//参数：
////////////////////////////////////////////////////////////////////////////////
GStream *CGLDZipFileEntry::data()
{
    GStream *result = NULL;
    long lCrc32;
    switch (m_commonFileHeader.CompressionMethod)
    {
    case c_mStored:
    {
        result = storedData();
        break;
    }
    case c_mDeflated:
    {
        result = zLibData();
        break;
    }
    case c_mBZip2:
    {
        result = bZip2Data();
        break;
    }
    default:
        zipFileError(getGLDi18nStr(rsErrUnSupportMethod));
        break;
    }
    if (!isEncrypted())
    {
        lCrc32 = zipCRC32(result);
        if ((unsigned int)lCrc32 != m_commonFileHeader.CRC32)
            zipFileError(getGLDi18nStr(rsErrFileCrc32));
    }
    result->seek(0);
    return result;
}

double CGLDZipFileEntry::dateTime()
{
    return fileDateToDateTime(m_commonFileHeader.LastModFileTimeDate);
}

GByteArray CGLDZipFileEntry::extrafield()
{
    return m_extrafield;
}

GString CGLDZipFileEntry::fileComment()
{
    return m_fileComment;
}

bool CGLDZipFileEntry::hasPassWord()
{
    return m_zipFile->password() != "";
}

unsigned short CGLDZipFileEntry::internalAttributes()
{
    return m_centralDirectory.InternalFileAttributes;
}

IGLDZipFileEntry *CGLDZipFileEntry::intf()
{
    CGLDZipFileEntry *result = intfObj();
    if (result)
        result->AddRef();
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.29
//功能: 返回是否加密标志位
//参数：
////////////////////////////////////////////////////////////////////////////////
bool CGLDZipFileEntry::isEncrypted()
{
    return ((m_commonFileHeader.GeneralPurposeBitFlag & c_defFileIsEncrypted) != 0);
}

GLDZipCompressionLevel CGLDZipFileEntry::level()
{
    return m_level;
}

GString CGLDZipFileEntry::name()
{
    return m_fileName;
}

IGLDZipFile* CGLDZipFileEntry::owner()
{
    if (m_zipFile)
        m_zipFile->AddRef();
    return m_zipFile;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.01
//功能: 获取直接存储的数据
//参数：
////////////////////////////////////////////////////////////////////////////////
GStream* CGLDZipFileEntry::storedData()
{
    GBlockMemoryStream *pResultStream = new GBlockMemoryStream(GStream::ReadWrite);
    if (m_commonFileHeader.CompressionMethod != c_mStored)
        return pResultStream;
    if (!isEncrypted())
        return m_compressedData;
    else
    {
        if (!decryptZipData(m_compressedData, pResultStream))
        {
            zipFileError(getGLDi18nStr(rsErrFilePassWord));
        }
    }
    return pResultStream;
}

unsigned long CGLDZipFileEntry::uncompressedSize()
{
    return m_commonFileHeader.UncompressedSize;
}

unsigned short CGLDZipFileEntry::versionMadeBy()
{
    return m_centralDirectory.VersionMadeBy;
}

/*CGLDZipFileEntryZLibProcessor */

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.31
//功能: 解压 ZLIB 数据块
//参数：
////////////////////////////////////////////////////////////////////////////////
GStream *CGLDZipFileEntry::zLibData()
{
    GBlockMemoryStream *pResultStream = new GBlockMemoryStream(GStream::ReadWrite);
    GBlockMemoryStream *pStreamData = new GBlockMemoryStream(GStream::ReadWrite);
    pResultStream->clearData();

    if (m_commonFileHeader.CompressionMethod != c_mDeflated)
        return pResultStream;
    if (!(isEncrypted()))
    {
        m_compressedData->seek(0);
        pStreamData->loadFromStream(m_compressedData);
    }
    else
    {
        m_compressedData->seek(0);
        if (!decryptZipData(m_compressedData, pStreamData))
        {
            zipFileError(getGLDi18nStr(rsErrFilePassWord));
            freeAndNil(pStreamData);
            return pResultStream;
        }
    }
    pStreamData->seek(0);
    decompressZLibData(pStreamData, m_commonFileHeader.UncompressedSize, pResultStream);

    freeAndNil(pStreamData);
    return pResultStream;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.31
//功能: 生成压缩流数据头
//参数：
//注意：manufacture a 2 byte header for zlib; 4 byte footer is not required.
//      Result := chr(120) + chr(156);
////////////////////////////////////////////////////////////////////////////////
GByteArray CGLDZipFileEntry::zLibStreamHeader()
/*
  TZLibStreamHeader = packed record
     CMF : Byte;
     m_lG : Byte;
  end;
*/
/*
const
  ZL_DEF_COMPRESSIONINFO    = $7;  // 32k window for Deflate
  ZL_DEF_COMPRESSIONMETHOD  = $8;  // Deflate

  ZL_FASTEST_COMPRESSION    = $0;
  ZL_FAST_COMPRESSION       = $1;
  ZL_DEFAULT_COMPRESSION    = $2;
  ZL_MAXIMUM_COMPRESSION    = $3;

  ZL_PRESET_DICT            = $20;

  ZL_FCHECK_MASK            = $1F;
var
  nCMF, nFLG,
  nCompress: Byte;
  nZLH: Word;
*/
{
    GByteArray result = GByteArray();
    result.append(char(0x78));
    switch (m_commonFileHeader.GeneralPurposeBitFlag & 6)
    {
    case 0:
        result.append(char(0x9c));
        break;
    case 2:
        result.append(char(0xda));
        break;
    case 4:
        result.append(char(0x5e));
        break;
    case 6:
        result.append(char(0x01));
        break;
    default:
        result.append(char(0x9c));
        break;
    }
    return result;
    /*
//  case m_commonFileHeader.generalPurposeBitFlag() and 6 of
//    0:
//      nCompress := ZL_DEFAULT_COMPRESSION;
//    2:
//      nCompress := ZL_MAXIMUM_COMPRESSION;
//    4:
//      nCompress := ZL_FAST_COMPRESSION;
//    6:
//      nCompress := ZL_FASTEST_COMPRESSION;
//  else
//    nCompress := ZL_DEFAULT_COMPRESSION;
//  end;
//  nCMF := ZL_DEF_COMPRESSIONINFO shl 4;
//  nCMF := nCMF or ZL_DEF_COMPRESSIONMETHOD;
//  nFLG := 0;
//  nFLG := nFLG or(nCompress shl 6);
//  nFLG := nFLG and not ZL_PRESET_DICT;
//  nFLG := nFLG and not ZL_FCHECK_MASK;
//  nZLH :=(nCMF *256) + nFLG;
//  inc(nFLG, 31 -(nZLH mod 31));
//  setLength(Result, 2 *sizeOf(Byte));
//  Result[1] := chr(nCMF);
//  Result[2] := chr(nFLG);
//*/
    /*
//  case CompMode of
//    0, 7, 8, 9: Result := $DA78;
//    1, 2: Result := $0178;
//    3, 4: Result := $5E78;
//    5, 6: Result := $9C78;
//  else
//    Result := 0;
//  end;
//*/
    //  return result;
}

void CGLDZipFileEntry::loadCentralDirectory(GStream *stream)
{
    GByteArray strFileName;
    GByteArray strFileComment;
    assert(stream != NULL);
    int nPosition;
    nPosition = stream->pos();
    stream->seek(nPosition - sizeof(c_CentralFileHeaderSignature));
    stream->read((char *)&m_centralDirectory, sizeof(m_centralDirectory));

    strFileName.resize(m_centralDirectory.CommonFileHeader.FilenameLength);
    stream->read(strFileName.data(), m_centralDirectory.CommonFileHeader.FilenameLength);
    m_fileName = GString::fromLocal8Bit(strFileName);
    m_extrafield.resize(m_centralDirectory.CommonFileHeader.ExtraFieldLength);
    strFileComment.resize(m_centralDirectory.FileCommentLength);
    stream->read(strFileComment.data(), m_centralDirectory.FileCommentLength);
    m_fileComment = GString::fromLocal8Bit(strFileComment);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.31
//功能: 加载加密数据描述信息
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::loadDataDescriptor(GStream *stream)
{
    GLDZipDataDescriptor cData;
    assert(stream != NULL);

    if ((m_commonFileHeader.GeneralPurposeBitFlag & c_defHasDataDescriptor) > 0)
    {
        stream->read((char *)&cData, sizeof(cData));
        if (cData.DataDescSignature != (unsigned int)c_DataDescSignature)
        {
            int nPos = stream->pos();
            stream->seek(nPos - sizeof(cData));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.18
//功能: 由文件加载
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::loadFromFile(const GString file)
{
    GFileStream *stream = new GFileStream(file);
    try
    {
        stream->open(GFileStream::ReadOnly);
        loadFromStream(stream);
    }
    catch(...)
    {
        stream->close();
        freeAndNil(stream);
        throw;
    }

    stream->close();
    freeAndNil(stream);
}

////////////////////////////////////////////////////////////////////////////////
//设计: LSUPER 2006.01.29
//功能: 由数据流加载
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::loadFromStream(GStream *stream)
{
    assert(stream != NULL);
    setData(stream);
}

////////////////////////////////////////////////////////////////////////////////
//设计: LSUPER 2006.01.29
//功能: 由数据流初始化内容
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::loadLocalFileHeader(GStream *stream)
{
    const int c_defBufferSize = 32768;
    GByteArray strFileName;
    assert(stream != NULL);
    stream->read((char *)&m_commonFileHeader, sizeof(m_commonFileHeader));
    strFileName.resize(m_commonFileHeader.FilenameLength);
    strFileName = stream->read(m_commonFileHeader.FilenameLength);
    m_fileName = GString::fromLocal8Bit(strFileName);
    m_extrafield.resize(m_commonFileHeader.ExtraFieldLength);
    m_extrafield = stream->read(m_commonFileHeader.ExtraFieldLength);

    if (NULL == m_memStream)
    {
        GStreamStrategy GStreamStrategyTemp(m_fileName, m_commonFileHeader.CompressedSize);
        m_memStream = GStreamStrategyTemp.stream();
    }

    m_memStream->seek(0);
    //m_memStream->write(zLibStreamHeader());

    GStreamStrategy::copyStreamByFragment(m_memStream, stream, c_defBufferSize, m_commonFileHeader.CompressedSize);

    parseEntry();
}
////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.31
//功能: 初始化存储信息
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::parseEntry()
{
    m_level = ctUnknown;
    //*
    //  if bit 3 is set, then the data descriptor record is appended
    //  to the compressed data
    //*/
    if (m_commonFileHeader.CompressionMethod == (unsigned short)c_mDeflated)
    {
        switch (m_commonFileHeader.GeneralPurposeBitFlag & 6)
        {
        case 0:
            m_level = ctDefault;
            break;
        case 2:
            m_level = ctMax;
            break;
        case 4:
            m_level = ctFastest;
            break;
        case 6:
            m_level = ctFastest;
            break;
        default:
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.18
//功能: 由文件流加载数据
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::saveCentralDirectory(GStream *stream)
{
    GByteArray strFileName = m_fileName.toLocal8Bit();
    GByteArray strFileComment = m_fileComment.toLocal8Bit();
    stream->write((char *)&m_centralDirectory, sizeof(m_centralDirectory));
    stream->write(strFileName.data(), strFileName.length());
    stream->write(m_extrafield.data(), m_extrafield.length());
    stream->write(strFileComment.data(), strFileComment.length());
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.31
//功能: 保存加密数据描述信息
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::saveDataDescriptor(GStream *stream)
{
    GLDZipDataDescriptor cData;
    assert(stream != NULL);
    if ((m_commonFileHeader.CompressionMethod == c_mDeflated)
            && ((m_commonFileHeader.GeneralPurposeBitFlag & c_defHasDataDescriptor) > 0))
    {
        //fillChar(cData, sizeof(cData), #0);
        cData.DataDescSignature = c_DataDescSignature;
        cData.CRC32 = m_commonFileHeader.CRC32;
        cData.CompressedSize = m_commonFileHeader.CompressedSize;
        cData.UncompressedSize = m_commonFileHeader.UncompressedSize;
        stream->write((char *)&cData, sizeof(cData));
    }
}

////////////////////////////////////////////////////////////////////////////////
//设计: LSUPER 2006.01.29
//功能: 向数据流保存内容
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::saveLocalFileHeader(GStream *stream)
{
    unsigned long uSignature;
    unsigned short uFlag;
    GByteArray strFileName;
    const int c_defBufferSize = 32768;

    uSignature = c_LocalFileHeaderSignature;
    assert(stream != NULL);
    if (hasPassWord())
    {
        uFlag = m_commonFileHeader.GeneralPurposeBitFlag | c_defFileIsEncrypted;
        m_commonFileHeader.GeneralPurposeBitFlag = uFlag;
    }
    m_centralDirectory.CommonFileHeader = m_commonFileHeader;
    m_centralDirectory.RelativeOffsetOfLocalHeader = stream->pos();

    stream->write((char *)&uSignature, sizeof(uSignature));
    stream->write((char *)&m_commonFileHeader, sizeof(m_commonFileHeader));
    strFileName = m_fileName.toLocal8Bit();
    stream->write(strFileName.data(), m_commonFileHeader.FilenameLength);
    stream->write(m_extrafield.data(), m_commonFileHeader.ExtraFieldLength);
	//    stream->write(m_compressedData.data(), m_commonFileHeader.CompressedSize);


    if (NULL != m_memStream && m_memStream->size() > 0)
    {
        m_memStream->open(GStream::ReadWrite);
        //m_memStream->seek(ZLIB_HEADER_LENGTH);
        //GStreamStrategy::copyStreamByFragment(m_memStream, stream, c_defBufferSize, m_memStream->size() - ZLIB_HEADER_LENGTH);
        m_memStream->seek(0);
        GStreamStrategy::copyStreamByFragment(stream, m_memStream, c_defBufferSize, m_memStream->size());
    }

    return;
}

HRESULT CGLDZipFileEntry::_QueryInterface(const IID &riid, void **ppvObject)
{
    if (riid == __uuidof(IGLDZipFileEntry))
    {
        this->AddRef();
        *ppvObject = static_cast<IGLDZipFileEntry *>(this);
        return NOERROR;
    }
    else if (riid == __uuidof(IGLDZipPersistent))
    {
        this->AddRef();
        *ppvObject = static_cast<IGLDZipPersistent *> (this);
        return NOERROR;
    }
    else
        return GInterfaceObject::_QueryInterface(riid, ppvObject);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.18
//功能: 向文件保存
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::saveToFile(const GString file)
{
    GFileStream *stream = new GFileStream(file);
    try
    {
        stream->open(GFileStream::ReadWrite | GFileStream::Truncate);
        saveToStream(stream);
    }
    catch(...)
    {
        stream->close();
        freeAndNil(stream);
        throw;
    }
    stream->close();
    freeAndNil(stream);
}
////////////////////////////////////////////////////////////////////////////////
//设计: LSUPER 2006.01.29
//功能: 数据解压到流
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::saveToStream(GStream *stream)
{
    //assert(stream != NULL);
    //GByteArray array = data();
    //stream->write(array.data(), array.length());

    unsigned long lCrc32;
	switch (m_commonFileHeader.CompressionMethod)
	{
	case c_mStored:
		{
			storedStream(stream);
			break;
		}
	case c_mDeflated:
		{
			zLibStream(stream);
			break;
		}
	case c_mBZip2:
		{
//			bZip2Stream(resultStream);
			break;
		}
    case c_7Zip:
        {
#ifdef WIN32
            store7zStream(stream);
#else
            assert(false);
#endif
            break;
        }
	default:
        zipFileError(getGLDi18nStr(rsErrUnSupportMethod));
		break;
	}
	if (!isEncrypted())
	{
		lCrc32 = zipUpdateCRC32FromStream(stream);
		if ((unsigned int)lCrc32 != m_commonFileHeader.CRC32)
            zipFileError(getGLDi18nStr(rsErrFileCrc32));
	};
}

void CGLDZipFileEntry::setAttriButes(const int value)
{
    m_centralDirectory.ExternalFileAttributes = value;
}

void CGLDZipFileEntry::setBZip2Data(GStream *data)
{
    G_UNUSED(data);
    //todo
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.18
//功能: 保存数据
//参数：
//注意：需要更新数据域信息
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::setData(GStream *value)
{
    //将value分块读
    m_commonFileHeader.CRC32 = zipCRC32(value);
    m_commonFileHeader.UncompressedSize = value->size();

    switch (m_commonFileHeader.CompressionMethod)
    {
    case c_mStored:
        setStoredData(value);
        break;
    case c_mDeflated:
        setZLibData(value);
        break;
    case c_mBZip2:
        setBZip2Data(value);
        break;
    case c_7Zip:
#ifdef WIN32
        set7zData(value);
#else
        assert(false);
#endif
        break;
    default:
        zipFileError(getGLDi18nStr(rsErrUnSupportMethod));
    }

    //存入zip的头信息里的压缩长度字段需要去掉2字节zlib头，不然解压不出来
    //m_commonFileHeader.CompressedSize = m_memStream->size() - ZLIB_HEADER_LENGTH;
    m_commonFileHeader.CompressedSize = m_memStream->size();
    m_centralDirectory.CommonFileHeader = m_commonFileHeader;
}

void CGLDZipFileEntry::setData(GByteArray value)
{
    GBlockMemoryStream *pBlockStream = new GBlockMemoryStream(GStream::ReadWrite);
    pBlockStream->write(value);
    pBlockStream->seek(0);

    m_commonFileHeader.CRC32 = zipCRC32(pBlockStream);
    m_commonFileHeader.UncompressedSize = pBlockStream->size();

    switch (m_commonFileHeader.CompressionMethod)
    {
    case c_mStored:
        setStoredData(pBlockStream);
        break;
    case c_mDeflated:
        setZLibData(pBlockStream);
        break;
    case c_mBZip2:
        setBZip2Data(pBlockStream);
        break;
    default:
        zipFileError(getGLDi18nStr(rsErrUnSupportMethod));
    }

    m_commonFileHeader.CompressedSize = m_memStream->size();
    m_centralDirectory.CommonFileHeader = m_commonFileHeader;

    freeAndNil(pBlockStream);
}

void CGLDZipFileEntry::setDateTime(const double value)
{
    m_commonFileHeader.LastModFileTimeDate = DateTimeToFileDate(value);
}

void CGLDZipFileEntry::setExtrafield(const GByteArray &value)
{
    m_extrafield = value;
    m_commonFileHeader.ExtraFieldLength = m_extrafield.length();
}

void CGLDZipFileEntry::setFileComment(const GString &value)
{
    GByteArray strFileComment;
    m_fileComment = value;
    strFileComment = m_fileComment.toLocal8Bit();
    m_centralDirectory.FileCommentLength = strFileComment.length();
}

void CGLDZipFileEntry::setLevel(const GLDZipCompressionLevel Value)
{
    m_level = Value;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.01
//功能: 设置存储数据
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::setStoredData(GStream *data)
{
    if (NULL == m_memStream)
    {
        GStreamStrategy GStreamStrategyTemp(m_fileName, data->size());
        m_memStream = GStreamStrategyTemp.stream();
    }

    if (m_commonFileHeader.CompressionMethod != c_mStored)
        return;
    if (!hasPassWord())
	{
		m_memStream->seek(0);
        //m_compressedData = data;
        data->seek(0);
        m_memStream->write(data->read(data->size()));
	}
    else
    {
        if (!encryptZipStream(m_memStream))
            zipFileError(getGLDi18nStr(rsErrEncryptedZipData));
    }
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.01
//功能: 设置压缩数据
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::setZLibData(GStream *data)
{
      if (m_commonFileHeader.CompressionMethod != c_mDeflated)
        return;

    if (NULL == m_memStream)
    {
        GStreamStrategy GStreamStrategyTemp(m_fileName, data->size());
        m_memStream = GStreamStrategyTemp.stream();
    }

    m_memStream->open(GFileStream::ReadWrite);

    if (!compressZLibData(data, m_memStream))
    {
        zipFileError(getGLDi18nStr(rsErrEncryptedZipData));
    }
    else if (!hasPassWord())
    {
        return;
    }
    else if (!encryptZipStream(m_memStream))
    {
        zipFileError(getGLDi18nStr(rsErrEncryptedZipData));
    }
}
////////////////////////////////////////////////////////////////////////////////
//设计: lijb 2015.05.29
//功能: 设置压缩数据
//参数： data 输入的数据流指针
////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
void CGLDZipFileEntry::set7zData(GStream *data)
{
    if (m_commonFileHeader.CompressionMethod != c_7Zip)
        return;

    if (NULL == m_memStream)
    {
        GStreamStrategy GStreamStrategyTemp(m_fileName, data->size());
        m_memStream = GStreamStrategyTemp.stream();
    }

    m_memStream->open(GFileStream::ReadWrite);

    if (!compress7ZData(data, m_memStream))
    {
        zipFileError(getGLDi18nStr(rsErrEncryptedZipData));
    }
    else if (!hasPassWord())
    {
        return;
    }
    else if (!encryptZipStream(m_memStream))
    {
        zipFileError(getGLDi18nStr(rsErrEncryptedZipData));
    }
}
#endif

CGLDZipFileEntry *CGLDZipFileEntry::intfObj()
{
    return this;
}

/*CGLDZipList */
////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2005.07.28
//功能: 添加一项
//参数：
////////////////////////////////////////////////////////////////////////////////
int CGLDZipList::add(CGLDZipFileEntry *item)
{
    m_list.push_back(item);
    return m_list.count() - 1;
}

void CGLDZipList::clear()
{
    m_list.clear();
}

CGLDZipList::CGLDZipList()
{
    //
}

void CGLDZipList::deleteItem(int index)
{
    if ((index >= 0) && (index < count()))
    {
        m_list.Delete(index);
    }
}

CGLDZipList::~CGLDZipList()
{
    m_list.clear();
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.30
//功能: 查找项
//参数：
////////////////////////////////////////////////////////////////////////////////
CGLDZipFileEntry* CGLDZipList::find(const GString &name)
{
    CGLDZipFileEntry *result;
    int nIndex = indexOf(name);
    if (nIndex == -1)
        result = NULL;
    else
        result = m_list.at(nIndex);
    return result;
}

int CGLDZipList::count()
{
    return m_list.count();
}

CGLDZipFileEntry* CGLDZipList::item(int index)
{
    CGLDZipFileEntry *result;
    if ((index >= 0) && (index < m_list.count()))
        result = m_list.at(index);
    else
    {
        zipFileError(getGLDi18nStr(rsErrIndexOutOfBand));
        return NULL;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.30
//功能: 检索
//参数：
////////////////////////////////////////////////////////////////////////////////
int CGLDZipList::indexOf(const GString &name)
{
    int result = m_list.count() - 1;
    while (result >= 0)
    {
        if (fileSuffixToLower(m_list.at(result)->name()) ==
                fileSuffixToLower(name))
            break;
        else
            result--;
    }
    return result;
}

GString CGLDZipList::fileSuffixToLower(const GString &name)
{
    GStringList list = name.split('.');
    GString suffixName = "";
    if (list.count() > 0)
    {
        suffixName = "." + list.value(list.count() - 1).toLower();
    }
    else
    {
        return name;
    }

    GString strName = "";
    for (int i = 0; i < list.count() - 1; i++)
    {
        strName += list.at(i);
    }

    return strName + suffixName;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2005.07.28
//功能: 按指针检索项
//参数：
////////////////////////////////////////////////////////////////////////////////
int CGLDZipList::indexOf(CGLDZipFileEntry *item)
{
    return m_list.indexOf(item);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2005.07.28
//功能: 插入新项
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipList::insert(int index, CGLDZipFileEntry *item)
{
    m_list.insert(index, item);
}

int CGLDZipList::remove(CGLDZipFileEntry *item)
{
    int result = m_list.indexOf(item);
    if (result > -1)
        m_list.removeOne(item);
    return result;
}

void CGLDZipList::setItem(int index, CGLDZipFileEntry *value)
{
    if ((index >= 0) && (index < m_list.count()))
        m_list.replace(index, value);
    else
        zipFileError(getGLDi18nStr(rsErrIndexOutOfBand));
}

/*CGLDZipFile */

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.18
//功能: 添加一个记录
//参数：
////////////////////////////////////////////////////////////////////////////////
IGLDZipFileEntry* CGLDZipFile::add(const GString name)
{
    CGLDZipFileEntry *result = addObj(name);
    if (result)
        result->AddRef();
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.08
//功能: 添加一个记录
//参数：
////////////////////////////////////////////////////////////////////////////////
IGLDZipFileEntry* CGLDZipFile::add(const GString name, unsigned int AttriBute, double timeStamp)
{
    CGLDZipFileEntry *result = addObj(name, AttriBute, timeStamp);
    if (result)
        result->AddRef();
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.31
//功能: 递归添加文件
//参数：
////////////////////////////////////////////////////////////////////////////////
int CGLDZipFile::addFiles(const GString folder, const GString base, bool recursion, const GString fileMask,
                          int searchAttr)
{
    int result = 0;
    GString strName;
    GString strBase;
    GString strFile;
//    int nSearchAttr;
    if (!GDir(folder).exists())
    {
        zipFileError(format(getGLDi18nStr(rsErrFolderNotExist), folder));
        return 0;
    }
    if (base.length() == 0)
        strBase = includeTrailingPathDelimiter(folder);
    else
        strBase = includeTrailingPathDelimiter(base);

//    if (fileMask.length() == 0)
//        strMask = "*.*";
//    else
//        strMask = fileMask;
//    if (searchAttr == 0)
//        nSearchAttr = GDir::Hidden | GDir::System | GDir::Dirs | GDir::Files | GDir::Readable;
//    else
//        nSearchAttr = searchAttr;
    GString strPath = includeTrailingPathDelimiter(folder);
    GDir *dir = new GDir(strPath);
    try
    {
        if (fileMask != "")
            dir->setNameFilters(fileMask.split("|"));//todo
        dir->setFilter(GDir::Hidden | GDir::System | GDir::Dirs | GDir::Files);//todo
        GFileInfoList list = dir->entryInfoList();
        for (int i = 0; i < list.count(); ++i)
        {
            strName = ((GFileInfo)(list.at(i))).fileName();
            if (strName == GString("..") || (strName == GString(".")))
                continue;
            if (GDir(strPath + strName).exists())
            {
                strFile = strPath + strName + "/";
                GDir dirBuff(strBase);
                strFile = dirBuff.relativeFilePath(strFile);
                addObj(strFile, 16, dateTimeToDouble(GDateTime::currentDateTime()));//todo, 16为添加文件夹的选项
                result++;
                if (recursion)
                    result += addFiles(strPath + strName, strBase, recursion, fileMask, searchAttr);
                else
                    continue;
            }
            else
            {
                strFile = strPath + strName;
                GDir dirbuff(strBase);
                strName = dirbuff.relativeFilePath(strFile);
                addObj(strName, strBase)->loadFromFile(strFile);
                result++;
            }
        }
    }
    catch (...)
    {
        freeAndNil(dir);
        throw;
    }
    freeAndNil(dir);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.23
//功能: 由缓冲区直接加载
//参数：
////////////////////////////////////////////////////////////////////////////////
IGLDZipFileEntry* CGLDZipFile::addFromBuffer(const GString &name, void *buffer, int count)
{
    CGLDZipFileEntry *result = addFromBufferObj(name, buffer, count);
    if (result)
        result->AddRef();
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.29
//功能: 由文件添加
//参数：
////////////////////////////////////////////////////////////////////////////////
IGLDZipFileEntry* CGLDZipFile::addFromFile(const GString &name, const GString &file)
{
    CGLDZipFileEntry *result = addFromFileObj(name, file);
//    if (result)
//        result->AddRef();
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计: LSUPER 2006.01.29
//功能: 添加流内容
//参数：
////////////////////////////////////////////////////////////////////////////////
IGLDZipFileEntry* CGLDZipFile::addFromStream(const GString &name, GStream *stream)
{
    CGLDZipFileEntry *result = addFromStreamObj(name, stream);
    if (result)
        result->AddRef();
    return result;
}

void CGLDZipFile::clear()
{
    m_files->clear();
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.29
//功能: 初始化、析构
//参数：
////////////////////////////////////////////////////////////////////////////////
CGLDZipFile::CGLDZipFile()
{
    m_files = new CGLDZipList();
    //  setmem((void*) m_endOfCentralDir, sizeof(m_endOfCentralDir), '\x00') /*todo #include <mem.h> */;
    m_level = ctDefault;
    m_onGetPassword = NULL;
}

void CGLDZipFile::deleteItem(int index)
{
    m_files->deleteItem(index);
}

CGLDZipFile::~CGLDZipFile()
{
    freeAndNil(m_files);
    // todo check:  inherited::destroy();
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.02.20
//功能: 解压缩所有文件到文件夹
//参数：nameMask为需要压缩的文件类型，默认为空，即全部压缩。
//      1.过滤一种文件，可书写方式为:"GSP" "*.GSP" 不区分大小写
//      2.过滤多种文件，用;分开："GSP;ini" "*.ini;gsp"
//      3.不过滤，可以不写参数，或者"*" "*.*"
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFile::extractFiles(const GString folder, const GString nameMask)
{
    GString strDir;
    GString strPath;
    GString strMask;
    GDir dir;
    bool bFilterAll = false;//全部压缩
    QMap<GString, bool> suffixMap;

    strMask = nameMask.trimmed();
    strMask.replace(" ", "");
    QStringList strMaskList;

    if(strMask.isEmpty())
    {
        bFilterAll = true;
    }
    else
    {
         strMaskList = strMask.split(";");
         for(int i = 0; i < strMaskList.size(); ++i)
         {
             QString strSuffix = strMaskList[i];
             int nPos = strSuffix.lastIndexOf(".");
             strSuffix = strSuffix.right(strSuffix.length() - nPos - 1);
             if(strSuffix == "*")
             {
                 bFilterAll = true;
                 break;
             }
             suffixMap.insert(strSuffix.toUpper(), true);
         }
    }

    GString strFolder = excludeTrailingPathDelimiter(folder);
    for (int i = 0; i < m_files->count(); ++i)
    {
        strPath = zipNameToFileName(m_files->item(i)->name());
        if (copy(strPath, 1, 1) == GString("\\"))
            strPath = strFolder + strPath;
        else
            strPath = includeTrailingPathDelimiter(strFolder) + strPath;
        if (m_files->item(i)->attriButes() == 16)
            GDir().mkpath(strPath);
        else
        {
            strDir = GFileInfo(strPath).absolutePath();
            dir = GFileInfo(strPath).dir();
            if (!dir.exists())
                GDir().mkpath(strDir);
            //添加后缀过滤
            if (bFilterAll)
            {
                m_files->item(i)->saveToFile(strPath);
            }
            else
            {
                int nPos = strPath.lastIndexOf(".");
                GString strSuffix = strPath.right(strPath.length() - nPos - 1);
                if(suffixMap.value(strSuffix.toUpper()))
                {
                    m_files->item(i)->saveToFile(strPath);
                }
            }
        }
    }
}

void CGLDZipFile::extractToBuffer(const GString name, int *buffer, int count)
{
    CGLDZipFileEntry *cEntry;
    GBlockMemoryStream *pStream = new GBlockMemoryStream(GStream::ReadWrite);
    int nSize;
    char* pBuf = (char *)buffer;

    cEntry = findObj(name);
    if (cEntry)
    {
        pStream->loadFromStream(cEntry->data());
        nSize = count > pStream->size() ? pStream->size() : count;
        for (int i = 0; i < count; ++i)
        {
            *(buffer++) = 0;
        }
        for (int i = 0; i < nSize; ++i)
        {
            pStream->seek(i);
            *(pBuf++) = pStream->read(1).at(0);
        }
    }
    freeAndNil(pStream);
}

void CGLDZipFile::extractToStream(const GString &name, GStream *stream)
{
    CGLDZipFileEntry *cEntry;
    cEntry = findObj(name);
    if (cEntry != NULL)
        cEntry->saveToStream(stream);
}

void CGLDZipFile::extractToString(const GString name, GByteArray *text)
{
    CGLDZipFileEntry *cEntry = findObj(name);
    if (cEntry != NULL)
        *text = cEntry->data()->readAll();
    else
        *text = GByteArray();
}

IGLDZipFileEntry* CGLDZipFile::find(const GString name)
{
    CGLDZipFileEntry *result = findObj(name);
    if (result)
        result->AddRef();
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2009.12.09
//功能：获取设置的密码
//参数：
////////////////////////////////////////////////////////////////////////////////
GString CGLDZipFile::findPassWord(CGLDZipFileEntry *entry)
{
    GString result = m_PassWord;
    if (m_onGetPassword != NULL)
        m_onGetPassword->getPasswordEvent(entry, &result);
    if (result.length() > c_defMaxPassWordLen)
        zipFileError(getGLDi18nStr(rsErrMaxLenPassWord));
    return result;
}

int CGLDZipFile::count()
{
    return m_files->count();
}

GByteArray CGLDZipFile::customFileHeader()
{
    return m_customFileHeader;
}

GString CGLDZipFile::fileComment()
{
    return m_fileComment;
}

IGLDZipFileEntry* CGLDZipFile::items(int index)
{
    CGLDZipFileEntry *result = itemObjs(index);
    if (result)
        result->AddRef();
    return result;
}

GLDZipCompressionLevel CGLDZipFile::level()
{
    return m_level;
}

GString CGLDZipFile::password()
{
    return m_PassWord;
}

int CGLDZipFile::indexOf(const GString name)
{
    CGLDZipFileEntry *fileEntry = m_files->find(name);
    return m_files->indexOf(fileEntry);
}

////////////////////////////////////////////////////////////////////////////////
//设计: LSUPER 2006.01.29
//功能: 加载文件信息
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFile::loadEndOfCentralDirectory(GStream *stream)
{
    GByteArray strFileComment;
    assert(stream != NULL);
    int nPosition = stream->pos();
    stream->seek(nPosition - sizeof(c_EndOfCentralDirSignature));
    stream->read((char *)&m_endOfCentralDir, sizeof(m_endOfCentralDir));
    strFileComment.resize(m_endOfCentralDir.ZipfileCommentLength);
    stream->read(strFileComment.data(), m_endOfCentralDir.ZipfileCommentLength);
    m_fileComment = GString::fromLocal8Bit(strFileComment);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.18
//功能: 由文件加载
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFile::loadFromFile(const GString file)
{
    GFileStream *stream = new GFileStream(file);
    try
    {
        stream->open(GFileStream::ReadOnly);
        loadFromStream(stream);
    }
    catch (...)
    {
        stream->close();
        freeAndNil(stream);
        throw;
    }
    stream->close();
    freeAndNil(stream);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.18
//功能: 由文件流加载数据
//参数：
//注意：读取 CustomFileHeader 时同时做 conFileHeaderSignature 和
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFile::loadFromStream(GStream *stream)
{
    int nIndex;
    CGLDZipFileEntry *cEntity;
    IGLDZipPersistent *cPersistent;
    assert(stream != NULL);
    m_files->clear();
    m_customFileHeader.clear();
    if ((!findCustomFileHeader(stream)) && (!findCustomFileHeaderBySignature(stream)))
        zipFileError(getGLDi18nStr(rsErrFileHeader));
    while (findSignature(stream, c_LocalFileHeaderSignature, c_CentralFileHeaderSignature))
    {
        cEntity = new CGLDZipFileEntry(this);
        cPersistent = (IGLDZipPersistent *)cEntity;
        cPersistent->loadLocalFileHeader(stream);
        cPersistent->loadDataDescriptor(stream);
        m_files->add(cEntity);
    }
    nIndex = 0;
    while (findSignature(stream, c_CentralFileHeaderSignature, c_EndOfCentralDirSignature))
    {
        cEntity = m_files->item(nIndex);
        cPersistent = (IGLDZipPersistent *)cEntity;
        cPersistent->loadCentralDirectory(stream);
        nIndex++;
    }
    if (findSignature(stream, c_EndOfCentralDirSignature))
    {
        loadEndOfCentralDirectory(stream);
    }
}

////////////////////////////////////////////////////////////////////////////////
//设计: LSUPER 2006.01.29
//功能: 保存文件信息
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFile::saveEndOfCentralDirectory(GStream *stream, long centralDirectoryOffset)
{
    GByteArray strFileComment;
    assert(stream != NULL);
    strFileComment = m_fileComment.toLocal8Bit();

    m_endOfCentralDir.EndOfCentralDirSignature = c_EndOfCentralDirSignature;
    m_endOfCentralDir.NumberOfThisDisk = 0;
    m_endOfCentralDir.NumberOfTheDiskWithTheStart = 0;
    m_endOfCentralDir.TotalNumberOfEntriesOnThisDisk = m_files->count();
    m_endOfCentralDir.TotalNumberOfEntries = m_files->count();
    m_endOfCentralDir.SizeOfTheCentralDirectory = stream->pos() - centralDirectoryOffset;
    m_endOfCentralDir.OffsetOfStartOfCentralDirectory = centralDirectoryOffset;
    m_endOfCentralDir.ZipfileCommentLength = strFileComment.length();

    stream->write((char *)&m_endOfCentralDir, sizeof(m_endOfCentralDir));
    stream->write(strFileComment.data(), strFileComment.length());
}

bool CGLDZipFile::findSignature(GStream *stream, long signature, long endSignature)
{
    bool bResult;
    int nReaded;
    int nPosition;
    unsigned long uSignature;
    bool bMatch;
    do
    {
        uSignature = 0;
        nReaded = stream->read((char *)&uSignature, sizeof(uSignature));
        if (nReaded < (int)sizeof(uSignature))
            return false;
        if (endSignature == 0)
            bMatch = (uSignature == (unsigned long)signature);
        else
            bMatch = ((uSignature == (unsigned long)signature) || (uSignature == (unsigned long)endSignature));
        if (!bMatch)
        {
            nPosition = stream->pos();
            stream->seek(nPosition - 3);
        }
    } while (!bMatch);
    bResult = (uSignature == (unsigned long)signature);
    if ((endSignature != 0) && (!bResult))
    {
        nPosition = stream->pos();
        stream->seek(nPosition - sizeof(uSignature));
    }
    return bResult;
}

bool CGLDZipFile::findCustomFileHeader(GStream *stream)
{
    int nLen(0);
    unsigned long uSignature;
    bool bResult = false;

    int nPos = stream->pos();
    int nReaded = stream->read((char *)&uSignature, sizeof(uSignature));
    if (nReaded < (int)sizeof(uSignature))
        return false;
    if (uSignature != (unsigned long)c_FileHeaderSignature)
    {
        int nPosition = stream->pos();
        stream->seek(nPosition - sizeof(uSignature));
        bResult = false;
        return bResult;
    }
    try
    {
        nReaded = stream->read((char *)&nLen, sizeof(nLen));
        if ((nReaded < (int)sizeof(nLen)) || (stream->pos() + nLen >= stream->size()))
            return bResult;
        m_customFileHeader.resize(nLen);
        stream->read(m_customFileHeader.data(), nLen);
        bResult = true;
    }
    catch (...)
    {
        if (!bResult)
            stream->seek(nPos);
        throw;
    }
    if (!bResult)
        stream->seek(nPos);
    return bResult;
}

bool CGLDZipFile::findCustomFileHeaderBySignature(GStream *stream)
{
    int nPos = (int)stream->pos();
    bool bResult;
    bResult = findSignature(stream, c_LocalFileHeaderSignature);
    if (!bResult)
        return bResult;
    int nReaded = (int)stream->pos() - nPos - sizeof(int);
    stream->seek(nPos);
    m_customFileHeader.resize(nReaded);
    stream->read(m_customFileHeader.data(), nReaded);
    return bResult;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.18
//功能: 向文件保存
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFile::saveToFile(const GString file)
{
    GFileStream *stream = new GFileStream(file);
    try
    {
        if (stream->open(GFileStream::WriteOnly | GFileStream::Truncate))
        {
            saveToStream(stream);
        }
    }
    catch (...)
    {
        stream->close();
        freeAndNil(stream);
        throw;
    }
    stream->close();
    freeAndNil(stream);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2004.08.18
//功能: 向文件流保存
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFile::saveToStream(GStream *stream)
{
    unsigned long uOffset;
    int nLen;
    IGLDZipPersistent *cPersistent;
    assert(stream != NULL);
    if (m_customFileHeader != GByteArray())
    {
        stream->write((char *)&c_FileHeaderSignature, sizeof(c_FileHeaderSignature));
        nLen = m_customFileHeader.length();
        stream->write((char *)&nLen, sizeof(nLen));
        stream->write(m_customFileHeader.data(), nLen);
    }
    for (int i = 0; i < m_files->count(); ++i)
    {
        cPersistent = (IGLDZipPersistent *)(m_files->item(i));
        cPersistent->saveLocalFileHeader(stream);
        cPersistent->saveDataDescriptor(stream);
    }
    uOffset = stream->pos();
    for (int i = 0; i < m_files->count(); ++i)
    {
        cPersistent = (IGLDZipPersistent *)(m_files->item(i));
        cPersistent->saveCentralDirectory(stream);
    }
    saveEndOfCentralDirectory(stream, uOffset);
}

IGLDZipGetPassWordEvent *CGLDZipFile::getOnGetPassword()
{
    return m_onGetPassword;
}

void CGLDZipFile::setOnGetPassword(IGLDZipGetPassWordEvent *value)
{
    m_onGetPassword = value;
}

CGLDZipFileEntry *CGLDZipFile::itemObjs(int index)
{
    if ((index >= 0) && (index < m_files->count()))
        return m_files->item(index);
    else
    {
        zipFileError(getGLDi18nStr(rsErrIndexOutOfBand));
        return NULL;
    }
}

CGLDZipFileEntry *CGLDZipFile::findObj(const GString &name)
{
    return m_files->find(name);
}

CGLDZipFileEntry *CGLDZipFile::addObj(const GString &name)
{
    CGLDZipFileEntry *result = new CGLDZipFileEntry(this, name);
    m_files->add(result);
    return result;
}

CGLDZipFileEntry *CGLDZipFile::addObj(const GString &name, unsigned int attriBute, double timeStamp)
{
    CGLDZipFileEntry *result = new CGLDZipFileEntry(this, name, attriBute, timeStamp);
    m_files->add(result);
    return result;
}
CGLDZipFileEntry *CGLDZipFile::addObj(const GString &name, const GString &dirname)
{
    CGLDZipFileEntry *result = new CGLDZipFileEntry(this, name, 0, 0, dirname);
    m_files->add(result);
    return result;
}

CGLDZipFileEntry *CGLDZipFile::addObj(const GString &name, unsigned int attriBute, double timeStamp, const GString &dirname)
{
    CGLDZipFileEntry *result = new CGLDZipFileEntry(this, name, attriBute, timeStamp, dirname);
    m_files->add(result);
    return result;
}

CGLDZipFileEntry *CGLDZipFile::addFromBufferObj(const GString &name, void *buffer, int count)
{
    GBlockMemoryStream *pStrMemory = new GBlockMemoryStream(GStream::ReadWrite);
    CGLDZipFileEntry *result = addObj(name);
    if (count != 0)
    {
        pStrMemory->write((char *)buffer, count);
        pStrMemory->seek(0);
        dynamic_cast<GBlockMemoryStream *>(result->data())->loadFromStream(pStrMemory);
    }
    freeAndNil(pStrMemory);
    return result;
}

CGLDZipFileEntry *CGLDZipFile::addFromFileObj(const GString &name, const GString &file)
{
    CGLDZipFileEntry *result = NULL;
    if (!GFileStream::exists(file))
        return NULL;
    double dLastModifyTime;
    GFileInfo *info = new GFileInfo(file);
    try
    {
        dLastModifyTime = dateTimeToDouble(info->lastModified());
    }
    catch (...)
    {
        freeAndNil(info);
        throw;
    }
    freeAndNil(info);
    result = addObj(name, 0, dLastModifyTime);
    result->loadFromFile(file);
    return result;//todo
}

CGLDZipFileEntry *CGLDZipFile::addFromStreamObj(const GString &name, GStream *stream)
{
    CGLDZipFileEntry *result;
    //GByteArray byteArray = GByteArray();
    //assert(stream != NULL);
    //if (stream->size() > 0)
    //{
    //    stream->seek(0);
    //    byteArray.resize(stream->size());
    //    stream->read(byteArray.data(), stream->size());
    //}
    result = addObj(name);
//    result->setData(byteArray);
	result->loadFromStream(stream);
    return result;
}

HRESULT CGLDZipFile::_QueryInterface(const IID &riid, void **ppvObject)
{
    if (riid == __uuidof(IGLDZipFile))
    {
        this->AddRef();
        *ppvObject = static_cast<IGLDZipFile *>(this);
        return NOERROR;
    }
    else
        return GInterfaceObject::_QueryInterface(riid, ppvObject);
}

void CGLDZipFile::setCustomFileHeader(GByteArray value)
{
    m_customFileHeader = value;
}

void CGLDZipFile::setFileComment(const GString value)
{
    m_fileComment = value;
}

void CGLDZipFile::setLevel(const GLDZipCompressionLevel value)
{
    m_level = value;
}

void CGLDZipFile::setPassword(const GString value)
{
    if (value.length() > c_defMaxPassWordLen)
        zipFileError(getGLDi18nStr(rsErrMaxLenPassWord));
    m_PassWord = value;
}

/*GLDZipGetPassWordEvent*/
void GLDZipGetPassWordEvent::getPasswordEvent(GLDZipFileEntry entry, GString *password)
{
    p->getPasswordEvent(entry, password);
}

/*GLDZipFileEntry*/
int GLDZipFileEntry::attriButes()
{
    return p->attriButes();
}

unsigned long GLDZipFileEntry::compressedSize()
{
    return p->compressedSize();
}

unsigned long GLDZipFileEntry::crc32()
{
    return p->crc32();
}

GStream *GLDZipFileEntry::data()
{
    return p->data();
}

double GLDZipFileEntry::dateTime()
{
    return p->dateTime();
}

bool GLDZipFileEntry::isEncrypted()
{
    return p->isEncrypted();
}

GString GLDZipFileEntry::name()
{
    return p->name();
}

GLDZipFile GLDZipFileEntry::owner()
{
    return GLDZipFile(p->owner(), false);
}

unsigned long GLDZipFileEntry::uncompressedSize()
{
    return p->uncompressedSize();
}

void GLDZipFileEntry::setAttriButes(const int value)
{
    p->setAttriButes(value);
}

void GLDZipFileEntry::setData(GStream *value)
{
    p->setData(value);
}

void GLDZipFileEntry::setData(GByteArray &value)
{
    p->setData(value);
}

void GLDZipFileEntry::setDateTime(const double value)
{
    p->setDateTime(value);
}

void GLDZipFileEntry::loadFromFile(const GString &file)
{
    p->loadFromFile(file);
}

void GLDZipFileEntry::saveToFile(const GString &file)
{
    p->saveToFile(file);
}

void GLDZipFileEntry::loadFromStream(GStream *stream)
{
    p->loadFromStream(stream);
}

void GLDZipFileEntry::saveToStream(GStream *stream)
{
    p->saveToStream(stream);
}

void GLDZipFileEntry::compressFromFile(const GString &file)
{
    p->compressFromFile(file);
}

void GLDZipFileEntry::decompressToFile(const GString &file)
{
    p->decompressToFile(file);
}

/*GLDZipFile*/
int GLDZipFile::count()
{
    return p->count();
}

GByteArray GLDZipFile::customFileHeader()
{
    return p->customFileHeader();
}

GString GLDZipFile::fileComment()
{
    return p->fileComment();
}

GLDZipFileEntry GLDZipFile::items(int index)
{
    return GLDZipFileEntry(p->items(index), false);
}

GLDZipCompressionLevel GLDZipFile::level()
{
    return p->level();
}

GString GLDZipFile::password()
{
    return p->password();
}

void GLDZipFile::setCustomFileHeader(const GByteArray &value)
{
    p->setCustomFileHeader(value);
}

void GLDZipFile::setFileComment(const GString &value)
{
    p->setFileComment(value);
}

void GLDZipFile::setLevel(const GLDZipCompressionLevel &value)
{
    p->setLevel(value);
}

void GLDZipFile::setPassword(const GString &value)
{
    p->setPassword(value);
}

GLDZipFileEntry GLDZipFile::add(const GString &name)
{
    return GLDZipFileEntry(p->add(name), false);
}

int GLDZipFile::addFiles(const GString &folder, const GString &base, const bool recursion, const GString &fileMask,
                         const int searchAttr)
{
    return p->addFiles(folder, base, recursion, fileMask, searchAttr);
}

GLDZipFileEntry GLDZipFile::addFromBuffer(const GString &name, void *buffer, const int count)
{
    return GLDZipFileEntry(p->addFromBuffer(name, buffer, count), false);
}

GLDZipFileEntry GLDZipFile::addFromFile(const GString &name, const GString &file)
{
    return GLDZipFileEntry(p->addFromFile(name, file));
}

GLDZipFileEntry GLDZipFile::addFromStream(const GString &name, GStream *stream)
{
    return GLDZipFileEntry(p->addFromStream(name, stream), false);
}

int GLDZipFile::indexOf(const GString &name)
{
    return p->indexOf(name);
}

void GLDZipFile::clear()
{
    p->clear();
}

void GLDZipFile::deleteItem(const int index)
{
    p->deleteItem(index);
}

void GLDZipFile::extractFiles(const GString &folder, const GString &nameMask)
{
    p->extractFiles(folder, nameMask);
}

void GLDZipFile::extractToBuffer(const GString &name, int *buffer, const int count)
{
    p->extractToBuffer(name, buffer, count);
}

void GLDZipFile::extractToStream(const GString &name, GStream *stream)
{
    p->extractToStream(name, stream);
}

void GLDZipFile::extractToString(const GString &name, GByteArray *text)
{
    p->extractToString(name, text);
}

void GLDZipFile::loadFromFile(const GString &file)
{
    p->loadFromFile(file);
}

void GLDZipFile::saveToFile(const GString &file)
{
    p->saveToFile(file);
}

void GLDZipFile::loadFromStream(GStream *stream)
{
    p->loadFromStream(stream);
}

void GLDZipFile::saveToStream(GStream *stream)
{
    p->saveToStream(stream);
}

GLDZipGetPassWordEvent GLDZipFile::getOnGetPassword()
{
    return GLDZipGetPassWordEvent(p->getOnGetPassword(), false);
}

void GLDZipFile::setOnGetPassword(GLDZipGetPassWordEvent &value)
{
    p->setOnGetPassword(value);
}

unsigned long CGLDZipFileEntry::zipUpdateCRC32FromStream(GStream *stream)
{	
    const int c_defBufferSize = 32768;
	GByteArray strBuffer;
	unsigned long uCrc = 0xffffffff;
	stream->seek(0);
	do
	{
		strBuffer = stream->read(c_defBufferSize);
        for (int i = 0; i < strBuffer.size(); ++i)
		{
			uCrc = zipUpdateCRC32(strBuffer.at(i), uCrc);
		}
	} while (!stream->atEnd());
	uCrc = uCrc ^ 0xffffffff;
	return uCrc;
}

bool CGLDZipFileEntry::encryptZipStream(GStream *inStream)
{
    const int c_defBufferSize = 32768;
    bool bResult = false;
    GLDZipEncryptStream *encrypter = NULL;

    GStreamStrategy GStreamStrategyTemp(this->m_fileName, inStream->size(),  "_encry");
    GStream *encryptOutStream = GStreamStrategyTemp.stream();
    try
    {
        if (buildEncryptStream(encryptOutStream, &encrypter))
        {
            try
            {
                //偏移掉zlib两个字节头不加密，否则存入zip文件后（需要去掉zlib头），第三方工具解密会失败
                //inStream->seek(ZLIB_HEADER_LENGTH);
                //GStreamStrategy::copyStreamByFragment(inStream, encrypter, c_defBufferSize, inStream->size() - ZLIB_HEADER_LENGTH);
                inStream->seek(0);
                GStreamStrategy::copyStreamByFragment(encrypter, inStream, c_defBufferSize, inStream->size());
                bResult = true;
            }
            catch (...)
            { 
                freeAndNil(encrypter);
                throw;
            }
            freeAndNil(encrypter);
        }
        encryptOutStream->seek(0);

        //将加密数据存到zlib头后
        //m_memStream->seek(ZLIB_HEADER_LENGTH);
        m_memStream->seek(0);
        GStreamStrategy::copyStreamByFragment(m_memStream, encryptOutStream, c_defBufferSize, encryptOutStream->size());
    }
    catch (...)
    {
        throw;
    }

    return bResult;
}

void CGLDZipFileEntry::storedStream(GStream * streamResult)
{
	if (m_commonFileHeader.CompressionMethod != c_mStored)
		return;
	if (!isEncrypted())
	{
		m_memStream->seek(0);
        GStreamStrategy::copyStreamByFragment(streamResult, m_memStream, 3800, m_memStream->size());
	}
	else
	{
        if (!decryptZipStream(streamResult))
            zipFileError(getGLDi18nStr(rsErrFilePassWord));
	}
}
////////////////////////////////////////////////////////////////////////////////
//设计: Linc 2006.01.31
//功能: 解压 ZLIB 数据块
//参数：
////////////////////////////////////////////////////////////////////////////////
void CGLDZipFileEntry::zLibStream(GStream *streamRuslt)
{
    GBlockMemoryStream streamData(GStream::ReadWrite);  // 可能用不上
    GStream *pStreamData = NULL;
	try
	{
		if (m_commonFileHeader.CompressionMethod != c_mDeflated)
			return;

        if (!m_memStream->isOpen())
        {
            m_memStream->open(GFileStream::ReadWrite);
        }

        if (!(isEncrypted()))
		{
            m_memStream->seek(0);
            pStreamData = m_memStream;
		}
		else
		{
            pStreamData = &streamData;
            if (!decryptZipStream(pStreamData))
			{
                zipFileError(getGLDi18nStr(rsErrFilePassWord));
				return;
			}
		}

        pStreamData->seek(0);
        decompressToStream(pStreamData, streamRuslt);
	}
	catch (...)
	{
        throw;
	}  
}
////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.05.21
//功能: 解压 7Z 数据块
//参数：
////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
void CGLDZipFileEntry::store7zStream(GStream *streamRuslt)
{
    GBlockMemoryStream streamData(GStream::ReadWrite);  // 可能用不上
    GStream *pStreamData = NULL;
    try
    {
        if (m_commonFileHeader.CompressionMethod != c_7Zip)
            return;

        if ( !m_memStream->isOpen())
        {
            m_memStream->open(GFileStream::ReadWrite);
        }

        if (!(isEncrypted()))
        {
            m_memStream->seek(0);
            pStreamData = m_memStream;
        }
        else
        {
            pStreamData = &streamData;
            if (!decryptZipStream(pStreamData))
            {
                zipFileError(getGLDi18nStr(rsErrFilePassWord));
                return;
            }
        }

        pStreamData->seek(0);
        decompressTo7zStream(pStreamData, streamRuslt);
    }
    catch (...)
    {
        throw;
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.05.21
//功能: 解压输入流中的7Z压缩数据块到输出流
//参数：instream  输入数据流
//     outStream 输出数据流
////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
bool CGLDZipFileEntry::decompressTo7zStream(GStream *instream, GStream *outStream)
{
    try
    {       
         uint nDestLen ;
         size_t ndead = FRAGMENT_LENGTH_7Z*2;
         size_t nRead;
         int ret;
         GByteArray srcRead;

         unsigned char aucDecomress[FRAGMENT_LENGTH_7Z*2]; //解压缓存
         unsigned char prop[5] = {93,0,0,0,1}; //lzma解压缺省参数，这个需要与压缩时的输出proc参数值一致，否则解压会失败

         instream->seek(0);
         nRead = instream->read((char *)&nDestLen, sizeof(nDestLen));
         while(nRead > 0)
         {
             srcRead = instream->read(nDestLen);
             nRead   = srcRead.size();

             if (SZ_OK != (ret = LzmaUncompress(aucDecomress, &ndead, (const uchar *)srcRead.data(), &nRead, prop, 5)))
             {
                 printf("LzmaUncompress error %d\n",ret);
                 // return  - 1;
             }

             //规避LzmaUncompress接口在解压最后一个分片，在后边多添加一个0的问题，这里需要截断多余字符，否则外层函数校验crc会失败
             if ((outStream->size() + ndead) >= m_commonFileHeader.UncompressedSize)
             {
                 ndead = m_commonFileHeader.UncompressedSize - outStream->size();
                 outStream->write((const char *)aucDecomress, ndead);
                 break;
             }
             outStream->write((const char *)aucDecomress, ndead);
             nRead = instream->read((char *)&nDestLen, sizeof(nDestLen));
         }
    }
    catch (...)
    {
        instream->close();
        throw;
    }

    instream->close();
    return true;
}
#endif

bool CGLDZipFileEntry::decompressToStream(GStream *instream, GStream *outStream)
{   
	try
    {
        m_memStream->seek(0);
        CGLDZlib::unCompress(instream, outStream, zLibStreamHeader());
	}
	catch (...)
	{
		instream->close();
		throw;
	}
	instream->close();
    return true;
}

bool CGLDZipFileEntry::decryptZipStream(GStream *streamResult)
{
    bool bResult = false;
    const int c_defBufferSize = 65535;

    GStreamStrategy GStreamStrategyTemp(this->m_fileName, m_memStream->size(), "decrypt");
    GStream *decryptOutstream = GStreamStrategyTemp.stream();
	try
	{
		GLDZipDecryptStream *decrypter = NULL;

        //m_memStream->seek(ZLIB_HEADER_LENGTH);
        //GStreamStrategy::copyStreamByFragment(m_memStream, DecryptOutstream, c_defBufferSize, m_memStream->size() - ZLIB_HEADER_LENGTH);
        m_memStream->seek(0);
        GStreamStrategy::copyStreamByFragment(decryptOutstream, m_memStream, c_defBufferSize, m_memStream->size());

        //m_memStream->seek(ZLIB_HEADER_LENGTH);
        m_memStream->seek(0);
        decryptOutstream->seek(0);

        if (buildDecryptStream(decryptOutstream, &decrypter))
		{
			try
			{
                gint64 nLen = decrypter->size() - decrypter->pos();
                GStreamStrategy::copyStreamByFragment(streamResult, decrypter, c_defBufferSize, nLen);
                bResult = true;
			}
			catch (...)
			{
				freeAndNil(decrypter);
				throw;
			}
			freeAndNil(decrypter);
		}
		else
            zipFileError(getGLDi18nStr(rsErrExtractPassWord));
	}
	catch (...)
	{
        freeAndNil(decryptOutstream);
		throw;
	}

    freeAndNil(decryptOutstream);

	return bResult;
}
