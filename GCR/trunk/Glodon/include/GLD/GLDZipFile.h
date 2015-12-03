#ifndef GLDZIPFILE_H
#define GLDZIPFILE_H

#include "GLDTypes.h"
#include "GLDUnknwn.h"
#include "GLDComptr.h"
#include "GLDStream.h"
#include "GLDZip_Global.h"

enum GLDZipCompressionLevel
{
    ctNone,
    ctFastest,
    ctDefault,
    ctMax,
    ctUnknown
};

interface IGLDZipFile;
interface IGLDZipFileEntry;
interface IGLDZipGetPassWordEvent;
class GLDZipFile;
class GLDZipFileEntry;
class GLDZipGetPassWordEvent;
class GStreamStrategy;

DEFINE_IID(IGLDZipGetPassWordEvent, "{8E879058-AD61-44CE-92C6-2DCBAEB301D5}");
DECLARE_INTERFACE_(IGLDZipGetPassWordEvent, IUnknown)
{
    GLDMETHOD(void, getPasswordEvent)(IGLDZipFileEntry *entry, GString *password);
};

DEFINE_IID(IGLDZipFileEntry, "{F96F15DA-C9CE-4466-B64F-06DE79642F30}");
DECLARE_INTERFACE_(IGLDZipFileEntry, IUnknown)
{
    GLDMETHOD(int, attriButes)() PURE;
    GLDMETHOD(unsigned long, compressedSize)() PURE;
    GLDMETHOD(unsigned long, crc32)() PURE;

    //返回值存在内存开辟失败问题
//    GLDMETHOD(GByteArray, data)() PURE;
    GLDMETHOD(GStream *, data)() PURE;

    GLDMETHOD(double, dateTime)() PURE;
    GLDMETHOD(bool, isEncrypted)() PURE;
    GLDMETHOD(GString, name)() PURE;
    GLDMETHOD(IGLDZipFile *, owner)() PURE;
    GLDMETHOD(unsigned long, uncompressedSize)() PURE;
    GLDMETHOD(void, setAttriButes)(const int value) PURE;

    //GByteArray存在内存开辟失败问题，建议使用GStream
    GLDMETHOD(void, setData)(GByteArray value) PURE;
    GLDMETHOD(void, setData)(GStream *value) PURE;

    GLDMETHOD(void, setDateTime)(const double value) PURE;

    GLDMETHOD(void, loadFromFile)(const GString file) PURE;
    GLDMETHOD(void, saveToFile)(const GString file) PURE;

    GLDMETHOD(void, loadFromStream)(GStream *stream) PURE;
    GLDMETHOD(void, saveToStream)(GStream *stream) PURE;

    GLDMETHOD(void, compressFromFile)(const GString file) PURE;
    GLDMETHOD(void, decompressToFile)(const GString file) PURE;
};

DEFINE_IID(IGLDZipFile, "{0C5D3108-B7B9-4FFE-A233-AA19E6A5FCD0}");
DECLARE_INTERFACE_(IGLDZipFile, IUnknown)
{
    GLDMETHOD(int, count)() PURE;
    GLDMETHOD(GByteArray, customFileHeader)() PURE;
    GLDMETHOD(GString, fileComment)() PURE;
    GLDMETHOD(IGLDZipFileEntry *, items)(int index) PURE;
    GLDMETHOD(GLDZipCompressionLevel, level)() PURE;
    GLDMETHOD(GString, password)() PURE;

    GLDMETHOD(void, setCustomFileHeader)(GByteArray value) PURE;
    GLDMETHOD(void, setFileComment)(const GString value) PURE;
    GLDMETHOD(void, setLevel)(const GLDZipCompressionLevel value) PURE;
    GLDMETHOD(void, setPassword)(const GString value) PURE;

    GLDMETHOD(IGLDZipFileEntry *, add)(const GString name) PURE;
    GLDMETHOD(int, addFiles)(const GString folder, const GString base = "",
                             bool recursion = true, const GString fileMask = "", int searchAttr = 0) PURE;
    GLDMETHOD(IGLDZipFileEntry *, addFromBuffer)(const GString &name, void *buffer, int count) PURE;
    GLDMETHOD(IGLDZipFileEntry *, addFromFile)(const GString &name, const GString &file) PURE;
    GLDMETHOD(IGLDZipFileEntry *, addFromStream)(const GString &name, GStream *stream) PURE;

    GLDMETHOD(int, indexOf)(const GString name) PURE;

    GLDMETHOD(void, clear)() PURE;
    GLDMETHOD(void, deleteItem)(int index) PURE;

    GLDMETHOD(void, extractFiles)(const GString folder, const GString nameMask = "") PURE;
    GLDMETHOD(void, extractToBuffer)(const GString name, int *buffer, int count) PURE;
    GLDMETHOD(void, extractToStream)(const GString &name, GStream *stream) PURE;
    GLDMETHOD(void, extractToString)(const GString name, GByteArray *text) PURE;

    GLDMETHOD(void, loadFromFile)(const GString file) PURE;
    GLDMETHOD(void, saveToFile)(const GString file) PURE;

    GLDMETHOD(void, loadFromStream)(GStream *stream) PURE;
    GLDMETHOD(void, saveToStream)(GStream *stream) PURE;

    GLDMETHOD(IGLDZipGetPassWordEvent*, getOnGetPassword)() PURE;
    GLDMETHOD(void, setOnGetPassword)(IGLDZipGetPassWordEvent *value) PURE;
};

class GLDZIPSHARED_EXPORT GLDZipGetPassWordEvent : public GComPtr<IGLDZipGetPassWordEvent>
{
public:
    inline GLDZipGetPassWordEvent() : GComPtr<IGLDZipGetPassWordEvent> () {}
    inline GLDZipGetPassWordEvent(IGLDZipGetPassWordEvent *lp) : GComPtr<IGLDZipGetPassWordEvent>(lp) {}
    inline GLDZipGetPassWordEvent(IGLDZipGetPassWordEvent *lp, bool /*dummy*/) : GComPtr<IGLDZipGetPassWordEvent>(lp, false){}
    template <typename Q>
    inline GLDZipGetPassWordEvent(const GComPtr<Q> &lp) : GComPtr<IGLDZipGetPassWordEvent>(lp){}
    void getPasswordEvent(GLDZipFileEntry entry, GString *password);
};

class GLDZIPSHARED_EXPORT GLDZipFileEntry : public GComPtr<IGLDZipFileEntry>
{
public:
    inline GLDZipFileEntry() : GComPtr<IGLDZipFileEntry> () {}
    inline GLDZipFileEntry(IGLDZipFileEntry *lp) : GComPtr<IGLDZipFileEntry>(lp) {}
    inline GLDZipFileEntry(IGLDZipFileEntry *lp, bool /*dummy*/) : GComPtr<IGLDZipFileEntry>(lp, false){}

    template <typename Q>
    inline GLDZipFileEntry(const GComPtr<Q> &lp) : GComPtr<IGLDZipFileEntry>(lp){}

    int attriButes();
    unsigned long compressedSize();
    unsigned long crc32();

    //返回值存在内存开辟失败问题
    //GByteArray data();
    GStream *data();

    double dateTime();
    bool isEncrypted();
    GString name();
    GLDZipFile owner();
    unsigned long uncompressedSize();
    void setAttriButes(const int value);

    //GByteArray存在内存开辟失败问题，建议使用GStream
    void setData(GByteArray &value);
    void setData(GStream *value);

    void setDateTime(const double value);
    void loadFromFile(const GString &file);
    void saveToFile(const GString &file);
    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    void compressFromFile(const GString &file);
    void decompressToFile(const GString &file);
};

class GLDZIPSHARED_EXPORT GLDZipFile : public GComPtr<IGLDZipFile>
{
public:
    inline GLDZipFile() : GComPtr<IGLDZipFile> () {}
    inline GLDZipFile(IGLDZipFile *lp) : GComPtr<IGLDZipFile>(lp) {}
    inline GLDZipFile(IGLDZipFile *lp, bool /*dummy*/) : GComPtr<IGLDZipFile>(lp, false){}

    template <typename Q>
    inline GLDZipFile(const GComPtr<Q> &lp) : GComPtr<IGLDZipFile>(lp){}

    int count();
    GByteArray customFileHeader();
    GString fileComment();
    GLDZipFileEntry items(int index);
    GLDZipCompressionLevel level();
    GString password();

    void setCustomFileHeader(const GByteArray &value);
    void setFileComment(const GString &value);
    void setLevel(const GLDZipCompressionLevel &value);
    void setPassword(const GString &value);
    GLDZipFileEntry add(const GString &name);

    int addFiles(const GString &folder, const GString &base = "",
                 const bool recursion = true, const GString &fileMask = "", const int searchAttr = 0);
    GLDZipFileEntry addFromBuffer(const GString &name, void *buffer, const int count);
    GLDZipFileEntry addFromFile(const GString &name, const GString &file);
    GLDZipFileEntry addFromStream(const GString &name, GStream *stream);

    int indexOf(const GString &name);
    void clear();
    void deleteItem(const int index);

    void extractFiles(const GString &folder, const GString &nameMask = "");
    void extractToBuffer(const GString &name, int *buffer, const int count);
    void extractToStream(const GString &name, GStream *stream);
    void extractToString(const GString &name, GByteArray *text);

    void loadFromFile(const GString &file);
    void saveToFile(const GString &file);

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    GLDZipGetPassWordEvent getOnGetPassword();
    void setOnGetPassword(GLDZipGetPassWordEvent &value);
};

GLDZIPSHARED_EXPORT void zipFileError(const GString Msg);
GLDZIPSHARED_EXPORT IGLDZipFile *createZipFile(const GString &password = "");
GLDZIPSHARED_EXPORT GLDZipFile createZipFileSPtr(const GString &password = "");

GLDZIPSHARED_EXPORT IGLDZipFile *loadZipFile(const GString &file, const GString &password = "");
GLDZIPSHARED_EXPORT IGLDZipFile *loadZipFile(GStream *stream, const GString &password = "");
GLDZIPSHARED_EXPORT GLDZipFile loadZipFileSPtr(const GString &file, const GString &password = "");
GLDZIPSHARED_EXPORT GLDZipFile loadZipFileSPtr(GStream *stream, const GString &password = "");

GLDZIPSHARED_EXPORT GString fileNameToZipName(const GString &name);
GLDZIPSHARED_EXPORT GString zipNameToFileName(const GString &name);

GLDZIPSHARED_EXPORT bool isZipFile(const GString &file);
GLDZIPSHARED_EXPORT bool isZipStream(GStream *stream);

GLDZIPSHARED_EXPORT bool isEncryptedZipFile(const GString &file);
GLDZIPSHARED_EXPORT bool isEncryptedZipStream(GStream *stream);

//返回值存在内存开辟失败问题
//GByteArray zipText(const GByteArray &s);
//GByteArray unzipText(const GByteArray &s);

GLDZIPSHARED_EXPORT GStream *zipText(const GByteArray &s);
GLDZIPSHARED_EXPORT GStream *unzipText(const GByteArray &s);

GLDZIPSHARED_EXPORT bool zipStream(GStream *srcStream, GStream *desStream, const GString &password = "");
GLDZIPSHARED_EXPORT bool zipStream(GStream *stream, const GString &password);

GLDZIPSHARED_EXPORT bool unZipStream(GStream *srcStream, GStream *desStream, const GString &password = "");
GLDZIPSHARED_EXPORT bool unZipStream(GStream *stream, const GString &password = "");

GLDZIPSHARED_EXPORT int compressZipFiles(const GString &file, const GString &folder, const GString &nameMask = "");
GLDZIPSHARED_EXPORT int extractZipFiles(const GString &file, const GString &folder, const GString &nameMask = "");
GLDZIPSHARED_EXPORT IGLDZipFileEntry* createZipFileEntry(IGLDZipFile *zipFile, const GString &zipFileName);
GLDZIPSHARED_EXPORT void zipFileError(const GString Msg);

#endif // GLDZIPFILE_H
