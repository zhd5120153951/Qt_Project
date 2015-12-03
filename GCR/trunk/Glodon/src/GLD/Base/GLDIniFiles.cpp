#include <string>
#include "GLDStream.h"
#include "GLDFile.h"
#include "GLDTextStream.h"
#include "GLDIniFiles.h"
#include "GLDStrUtils.h"
#include "GLDFileUtils.h"
#include "GLDException.h"

class GLDCustomIniFilePrivate
{
public:
  GLDCustomIniFilePrivate(GLDCustomIniFile *parent = 0)
    : q_ptr(parent)
  {
  }

private:
  GLDCustomIniFile * const q_ptr;
  Q_DECLARE_PUBLIC(GLDCustomIniFile);
  GString m_fileName;
};

/*GLDCustomIniFile*/
GLDCustomIniFile::GLDCustomIniFile(const GString &fileName):d_ptr(new GLDCustomIniFilePrivate(this))
{
    Q_D(GLDCustomIniFile);
    d->m_fileName = fileName;
}

GLDCustomIniFile::GLDCustomIniFile(const GString &fileName, GLDCustomIniFilePrivate &d_private)
    : d_ptr(&d_private)
{
    Q_D(GLDCustomIniFile);
    d->m_fileName = fileName;
}

GLDCustomIniFile::~GLDCustomIniFile()
{
    Q_D(GLDCustomIniFile);
    delete d;
}

bool GLDCustomIniFile::sectionExists(const GString &section)
{
    GStringObjectList strObjList;
    bool result = false;
    try
    {
        readSection(section, &strObjList);
        result = strObjList.size() > 0;
    }
    catch(...)
    {
        strObjList.clear();
        throw;
    }
    strObjList.clear();
    return result;
}

long GLDCustomIniFile::readInteger(const GString &section, const GString &ident, long defVal)
{
    GString intStr = readString(section, ident, "");
    if ((length(intStr) > 2) && (intStr[0].toLatin1() == '0')
            && ((intStr[1].toLatin1() == 'X') || (intStr[1].toLatin1() == 'x')))
    {
        intStr = "$" + copy(intStr, 2);
    }
    return strToIntDef(intStr, defVal);
}

void GLDCustomIniFile::writeInteger(const GString &section, const GString &ident, long value)
{
    writeString(section, ident, intToStr(value));
}

bool GLDCustomIniFile::readBool(const GString &section, const GString &ident, bool defVal)
{
    return readInteger(section, ident, defVal) != 0;
}

void GLDCustomIniFile::writeBool(const GString &section, const GString &ident, bool value)
{
    writeString(section, ident, boolToStr(value, false));
}

int GLDCustomIniFile::readBinaryStream(const GString &section, const GString &name, GStream *value)
{
    GString text = readString(section, name, "");
    int result = 0;
    if (text != "")
    {
        int nPos = value->pos();
        GByteArray array = GByteArray::fromHex(text.toLatin1());
        value->write(array);
        result = value->size() - nPos;
    }
    return result;
}

void GLDCustomIniFile::writeBinaryStream(const GString &section, const GString &name, GStream *value)
{
    int nSize = value->size() - value->pos();
    GString text(nSize * 2, Qt::Uninitialized);
    if (length(text) > 0)
    {
        text = GString::fromLatin1(value->read(nSize).toHex());
    }
    writeString(section, name, text);
}

GDate GLDCustomIniFile::readDate(const GString &section, const GString &name, GDate &defVal)
{
    GString dateStr = readString(section, name, "");
    GDate result = defVal;
    if (dateStr != "")
    {
        try
        {
            result = GDate::fromString(dateStr);
        }
        catch(...)
        {
            throw;
        }
    }
    return result;
}

void GLDCustomIniFile::writeDate(const GString &section, const GString &name, GDate &value)
{
    writeString(section, name, value.toString());
}

GDateTime GLDCustomIniFile::readDateTime(const GString &section, const GString &name, GDateTime &defVal)
{
    GString dateStr = readString(section, name, "");
    GDateTime result = defVal;
    if (dateStr != "")
    {
        try
        {
            result = strToDateTime(dateStr);
        }
        catch(...)
        {
            throw;
        }
    }
    return result;
}

void GLDCustomIniFile::writeDateTime(const GString &section, const GString &name, GDateTime &value)
{
    writeString(section, name, dateTimeToStr(value));
}

double GLDCustomIniFile::readFloat(const GString &section, const GString &name, double defVal)
{
    GString floatStr = readString(section, name, "");
    double result = defVal;
    if (floatStr != "")
    {
        try
        {
            result = strToFloat(floatStr);
        }
        catch(...)
        {
            throw;
        }
    }
    return result;
}

void GLDCustomIniFile::writeFloat(const GString &section, const GString &name, double value)
{
    writeString(section, name, floatToStr(value));
}

GTime GLDCustomIniFile::readTime(const GString &section, const GString &name, GTime &defVal)
{
    GString timeStr = readString(section, name, "");
    GTime result = defVal;
    if (timeStr != "")
    {
        try
        {
            result = GTime::fromString(timeStr);
        }
        catch(...)
        {
            throw;
        }
    }
    return result;
}

void GLDCustomIniFile::writeTime(const GString &section, const GString &name, GTime &value)
{
    writeString(section, name, value.toString());
}

void GLDCustomIniFile::readSections(const GString &section, GStringObjectList *strings)
{
    readSections(strings);
    for (int i = strings->count() - 1; i >= 0; i--)
    {
        if (!sameText(section, copy(strings->at(i)->str, 0, length(section))))
            strings->removeAt(i);
    }
}

bool GLDCustomIniFile::valueExists(const GString &section, const GString &ident)
{
    GStringObjectList strTmp;
    readSection(section, &strTmp);
    return strTmp.indexOf(ident) > -1;
}

void GLDCustomIniFile::setFileName(const GString &fileName)
{
    Q_D(GLDCustomIniFile);
    d->m_fileName = fileName;
}

GString GLDCustomIniFile::fileName()
{
    Q_D(GLDCustomIniFile);
    return d->m_fileName;
}

/*GLDIniFile*/
GLDIniFile::GLDIniFile(const GString &fileName) : GLDCustomIniFile(fileName)
{
}

GLDIniFile::~GLDIniFile()
{
    updateFile();
}

GString GLDIniFile::readString(const GString &section, const GString &ident, const GString &defVal)
{
    const int c_BufSize = 2048;
    char *buffer = new char[c_BufSize];
    memset(buffer, 0, c_BufSize);

    int nSize = getPrivateProfileString(section, ident, defVal, buffer, c_BufSize, fileName());
    char *newBuffer = new char[1024];
    memcpy(newBuffer, buffer, nSize);
    newBuffer[nSize] = '\0';
    GString result = GString::fromLocal8Bit(newBuffer);
    delete newBuffer;
    delete buffer;
    return result;
}

void GLDIniFile::writeString(const GString &section, const GString &ident, const GString &value)
{
    if (!writePrivateProfileString(section, ident, value, fileName()))
        throw GLDException(format("Unable to write to %s", fileName()));//??tr
}

void GLDIniFile::readSection(const GString &section, GStringObjectList *strings)
{
    const int c_BufSize = 16384;
    char *buffer = new char[c_BufSize];
    memset(buffer, 0, c_BufSize);
    try
    {
        strings->clear();
        if (getPrivateProfileString(section, "", "", buffer, c_BufSize, fileName()) != 0)
        {
            char *tmpP = buffer;
            while (*tmpP != '\0')
            {
                strings->add(GString::fromLocal8Bit(tmpP));
                tmpP += strlen(tmpP) + 1;
            }
        }
    }
    catch(...)
    {
        freeAndNil(buffer);
        throw;
    }
    freeAndNil(buffer);
}

void GLDIniFile::readSections(GStringObjectList *strings)
{
    const int c_BufSize = 16384;
    char *buffer = new char[c_BufSize];
    memset(buffer, 0, c_BufSize);
    try
    {
        strings->clear();
        if (getPrivateProfileString("", "", "", buffer, c_BufSize, fileName()) != 0)
        {
            char *tmpP = buffer;
            while (*tmpP != '\0')
            {
                strings->add(GString::fromLocal8Bit(tmpP));
                tmpP += strlen(tmpP) + 1;
            }
        }
    }
    catch(...)
    {
        freeAndNil(buffer);
        throw;
    }
    freeAndNil(buffer);
}

void GLDIniFile::readSectionValues(const GString &section, GStringObjectList *strings)
{
    GStringObjectList keyList;
    readSection(section, &keyList);
    strings->clear();
    for (int i = 0; i < keyList.count(); i++)
    {
        strings->add(keyList.at(i)->str + "=" + readString(section, keyList.at(i)->str, ""));
    }
}

void GLDIniFile::eraseSection(const GString &section)
{
    if (!writePrivateProfileString(section, "", "", fileName()))
    {
        throw GLDException(format("Unable to write to %s", fileName()));
    }
}

void GLDIniFile::deleteKey(const GString &section, const GString &ident)
{
    writePrivateProfileString(section, ident, "", fileName());
}

void GLDIniFile::updateFile()
{
    writePrivateProfileString("", "", "", fileName());
}

int GLDIniFile::strCopy(const GString &source, char *dest, int size)
{
    GByteArray temp = source.toLocal8Bit();
    //拷贝字符串；
    //size合适，返回不加'/0'的字符串长度
    int nStrLength = temp.size();
    if ((size == -1) || (nStrLength <= size - 1))
    {
        memcpy(dest, temp.data(), nStrLength);
        dest[nStrLength] = '\0';
        return nStrLength;
    }
    //拷贝字符串；
    //size过小，返回size-1
    else
    {
        memcpy(dest, temp.data(), size - 1);
        dest[size - 1] = '\0';
        return size - 1;
    }
}

int GLDIniFile::getPrivateProfileString(const GString &strSectionName, const GString &strKeyName,
                                        GString strDefault, char *pReturnedValue, int size, const GString &strFileName)
{
    //设置默认返回字符串
    if (strDefault == NULL)
    {
        strDefault = "";
    }
    //文件不存在，返回默认字符串及其长度
    if (!GFileStream::exists(strFileName))
    {
        return strCopy(strDefault, pReturnedValue, size);
    }

    GFileStream readFile(strFileName);

    //打开失败，返回默认字符串及其长度
    if (!readFile.open(GStream::ReadOnly))
    {
        return strCopy(strDefault, pReturnedValue, size);
    }
    GTextStream readStream(&readFile);

    if ((strSectionName != "") && (strKeyName != ""))
    {
        GString strSection("[" + strSectionName + "]");
        GString strKey(strKeyName + "=");

        while (!readStream.atEnd())
        {
            GString line(readStream.readLine());
            if (line.indexOf(strSection) == 0) //找到Section
            {
                line = readStream.readLine();
                while (line.indexOf(strKey) != 0 && line.indexOf("[") != 0 && !readStream.atEnd())
                {
                    line = readStream.readLine();
                }
                if (line.indexOf(strKey) == 0) //找到Key
                {
                    GString source(line.mid(strKey.length()));
                    readFile.close();
                    return strCopy(source, pReturnedValue, size);
                }
            }
        }
        readFile.close();
        //未找到，返回默认值及其长度
        return strCopy(strDefault, pReturnedValue, size);
    }
    else
    {
        char *tmpP = pReturnedValue;
        int nPos = 0;
        if (strSectionName == "") //读取所有小结名
        {
            while (!readStream.atEnd())
            {
                GString line(readStream.readLine());
                if (line.indexOf("[") == 0) // 找到section
                {
                    line = line.mid(1, line.length() - 2);
                    nPos = strCopy(line, tmpP);
                    tmpP[++nPos] = '\0';
                    tmpP += nPos;
                }
            }
        }
        else if (strKeyName == "")
        {
            GString strSection("[" + strSectionName + "]");
            while (!readStream.atEnd())
            {
                GString line(readStream.readLine());
                if (line.indexOf(strSection) == 0) //找到Section
                {
                    line = readStream.readLine();
                    while (line.indexOf("[") != 0)
                    {
                        GString sKey = copy(line, 0, pos("=", line));
                        nPos = strCopy(sKey, tmpP);
                        tmpP[++nPos] = '\0';
                        tmpP += nPos;
                        if (readStream.atEnd())
                            break;
                        line = readStream.readLine();
                    }
                }
            }
        }
        tmpP[++nPos] = '\0';
        readFile.close();
        return nPos;
    }
}

bool GLDIniFile::writePrivateProfileString(const GString &strSectionName, const GString &strKeyName,
                                           const GString &strValue, const GString &strFileName)
{
    bool result = false;

    //section是null则直接返回false
    if (strSectionName.length() == 0)
    {
        return result;
    }

    bool bFlagFindSection = false;//是否找到了section
    GString strSection("[" + strSectionName + "]");
    GString strKey(strKeyName + "=");

    //文件不存在，则创建，且直接写入
    if (!GFileStream::exists(strFileName))
    {
        GFileStream createFile(strFileName);
        if (!createFile.open(GStream::WriteOnly))
        {
            return result;
        }
        //如果key不是null，才写内容
        if (!strKeyName.isEmpty())
        {
            GTextStream createStream(&createFile);
            createStream << strSection << "\r\n";
            createStream << strKey << strValue << "\r\n";
            createStream.flush();

            result = true;
        }
        createFile.close();
        return result;
    }

    GFileStream readFile(strFileName);

    if (!readFile.open(GStream::ReadOnly))
    {
        result = false;
        return result;
    }

    GFileStream writeFile(strFileName);

    //读入流和写入流
    //写入流文件在最后才WriteOnly打开
    GTextStream readStream(&readFile);
    GTextStream writeStream(&writeFile);

    //查找每一行是否包含section
    while (!readStream.atEnd())
    {
        GString line(readStream.readLine());

        if (line.indexOf(strSection) == -1) //该行不包含section，直接写入流
        {
            writeStream << line << "\r\n";
        }
        else
        {
            bFlagFindSection = true; //查到section
            result = true;

            //key是null，则跳过该section,写入其余section
            if (strKeyName.isEmpty())
            {
                do //跳过该section
                {
                    line = GString(readStream.readLine());
                } while (line.indexOf("[") != 0 && !readStream.atEnd());

                if (readStream.atEnd())
                {
                    break;
                }
                else //写入其余section
                {
                    writeStream << line << "\r\n";
                    while (!readStream.atEnd())
                    {
                        writeStream << readStream.readLine() << "\r\n";
                    }
                    break;
                }
            }

            writeStream << line << "\r\n";//section写入流
            line = GString(readStream.readLine());
            while (line.indexOf(strKey) != 0 && line.indexOf("[") != 0)
            {
                writeStream << line << "\r\n";
                if (!readStream.atEnd())
                    line = GString(readStream.readLine());
                else
                    break;
            }
            if (readStream.atEnd())//文件末尾，若value!=null则直接加上key
            {
                //直接加上
                if (!strValue.isEmpty())
                {
                    writeStream << strKey << strValue << "\r\n";
                }
            }
            else if (line.indexOf("[") == 0)//查到下一个section，若value!=null则在下一个section前直接加上key
            {
                if (!strValue.isEmpty())
                {
                    writeStream << strKey << strValue << "\r\n";
                }
                writeStream << line << "\r\n";
                while (!readStream.atEnd()) //剩余行写入流中
                {
                    writeStream << readStream.readLine() << "\r\n";
                }
                break;
            }
            else if (line.indexOf(strKeyName) == 0)//查到key，若value!=null则修改value
            {
                if (!strValue.isEmpty())
                {
                    line = line.mid(0, line.indexOf("=") + 1) + strValue;
                    writeStream << line << "\r\n";
                }
                while (!readStream.atEnd()) //剩余行写入流中
                {
                    writeStream << readStream.readLine() << "\r\n";
                }
                break;
            }
        }
    }
    if (!bFlagFindSection)//若未查到该section，且key和value！=null，写入section和key=value
    {
        if (!strKeyName.isEmpty())
        {
            writeStream << strSection << "\r\n";
            writeStream << strKey << strValue << "\r\n";
            result = true;
        }
    }
    readFile.close();

    //写打开文件
    if (!writeFile.open(GStream::WriteOnly))
    {
        result = false;
        return result;
    }
    writeStream.flush();//写入流到文件

    writeFile.close();
    return result;
}

class GLDMemIniFilePrivate : public GLDCustomIniFilePrivate
{
public:
  GLDMemIniFilePrivate(GLDMemIniFile *parent = 0)
    : q_ptr(parent),  m_sections(new GHashedStringObjectList), m_allowValueHasSpace(false)
  {
       //m_sections = new GHashedStringObjectList;
       //m_allowValueHasSpace = false;
  }

private:
  GLDMemIniFile * const q_ptr;
  Q_DECLARE_PUBLIC(GLDMemIniFile);
  GStringObjectList *m_sections;
  bool m_allowValueHasSpace;
};

GLDMemIniFile::GLDMemIniFile(const GString &fileName)
    : GLDCustomIniFile(fileName, (*new GLDMemIniFilePrivate(this)))
{
#ifndef WIN32
    Q_D(GLDMemIniFile);
    d->m_sections->setCaseSensitive(true);
#endif
    loadValues();
}

GLDMemIniFile::~GLDMemIniFile()
{
    Q_D(GLDMemIniFile);
    if (d->m_sections)
    {
        clear();
    }
    freeAndNil(d->m_sections);
}

void GLDMemIniFile::clear()
{
    Q_D(GLDMemIniFile);
    for (int i = 0; i < d->m_sections->count(); ++i)
    {
        GStringObjectList* list = (GStringObjectList*)(d->m_sections->object(i));
        freeAndNil(list);
    }
    d->m_sections->clear();
}

void GLDMemIniFile::deleteKey(const GString &section, const GString &ident)
{
    Q_D(GLDMemIniFile);
    int nIndex = d->m_sections->indexOf(section);
    if (nIndex >= 0)
    {
        GStringObjectList *strings = (GStringObjectList*)(d->m_sections->object(nIndex));
        int nNameIndex = strings->indexOfName(ident);
        if (nNameIndex >= 0)
        {
            strings->Delete(nNameIndex);
        }
    }
}

void GLDMemIniFile::eraseSection(const GString &section)
{
    Q_D(GLDMemIniFile);
    int nIndex = d->m_sections->indexOf(section);
    if (nIndex >= 0)
    {
        GStringObjectList *strings = (GStringObjectList*)(d->m_sections->object(nIndex));
        freeAndNil(strings);
        d->m_sections->Delete(nIndex);
    }
}

void GLDMemIniFile::strings(GStringObjectList *list)
{
    list->beginUpdate();
    try
    {
        Q_D(GLDMemIniFile);
        for (int i = 0; i < d->m_sections->count(); ++i)
        {
            list->add("[" + d->m_sections->at(i)->str + "]");
            GStringObjectList *strings = (GStringObjectList*)(d->m_sections->object(i));
            for (int j = 0; j < strings->count(); ++j)
            {
                list->add(strings->at(j)->str);
            }
            list->add("");
        }
    }
    catch(...)
    {
        list->endUpdate();
        throw;
    }
    list->endUpdate();
}

void GLDMemIniFile::readSection(const GString &section, GStringObjectList *strings)
{
    strings->beginUpdate();
    try
    {
        strings->clear();
        Q_D(GLDMemIniFile);
        int nIndex = d->m_sections->indexOf(section);
        if (nIndex >= 0)
        {
            GStringObjectList *sectionStrings = (GStringObjectList*)(d->m_sections->object(nIndex));
            for (int j = 0; j < sectionStrings->count(); ++j)
            {
                strings->add(sectionStrings->names(j));
            }
        }
    }
    catch(...)
    {
        strings->endUpdate();
        throw;
    }
}

void GLDMemIniFile::readSections(GStringObjectList *strings)
{
    Q_D(GLDMemIniFile);
    strings->assign(d->m_sections);
}

void GLDMemIniFile::readSectionValues(const GString &section, GStringObjectList *strings)
{
    strings->beginUpdate();
    try
    {
        strings->clear();
        Q_D(GLDMemIniFile);
        int nIndex = d->m_sections->indexOf(section);
        if (nIndex >= 0)
        {
            strings->assign((GStringObjectList*)(d->m_sections->object(nIndex)));
        }
    }
    catch(...)
    {
        strings->endUpdate();
        throw;
    }
    strings->endUpdate();
}

GString GLDMemIniFile::readString(const GString &section, const GString &ident, const GString &Default)
{
    Q_D(GLDMemIniFile);
    int nIndex = d->m_sections->indexOf(section);
    if (nIndex >= 0)
    {
        GStringObjectList *strings = (GStringObjectList*)(d->m_sections->object(nIndex));
        int nIndex = strings->indexOfName(ident);
        if (nIndex >= 0)
        {
            return copy(strings->at(nIndex)->str, ident.length() + 1, MaxInt);
        }
    }
    return Default;
}

void GLDMemIniFile::rename(const GString &fileName, bool reload)
{
    setFileName(fileName);
    if (reload)
    {
        loadValues();
    }
}

void GLDMemIniFile::setStrings(GStringObjectList *list)
{
    clear();
    GStringObjectList *strings = NULL;

    Q_D(GLDMemIniFile);
    for (int i = 0; i < list->count(); ++i)
    {
        GString strTrim;
        if (d->m_allowValueHasSpace)
        {
            strTrim = trimLeft(list->at(i)->str);
        }
        else
        {
            strTrim = trim(list->at(i)->str);
        }
        if ((strTrim.length() > 0) && (strTrim.at(0).toLatin1() != ';'))
        {
            if ((strTrim.at(0).toLatin1() == '[') && (strTrim.at(strTrim.length() - 1).toLatin1() == ']'))
            {
                strTrim = copy(strTrim, 1, strTrim.length() - 2);
                strings = addSection(trim(strTrim));
            }
            else
            {
                if (strings != NULL)
                {
                    int nPos = pos("=", strTrim);
                    // remove spaces before and after '='
                    if (nPos >= 0)
                    {
                        if (d->m_allowValueHasSpace)
                        {
                            strings->add(trim(copy(strTrim, 0, nPos)) + "=" + copy(strTrim, nPos + 1, MaxInt));
                        }
                        else
                        {
                            strings->add(trim(copy(strTrim, 0, nPos)) + "=" + trim(copy(strTrim, nPos + 1, MaxInt)));
                        }
                    }
                    else
                    {
                        strings->add(strTrim);
                    }
                }
            }
        }
    }
}

void GLDMemIniFile::updateFile()
{
    GStringObjectList *list = new GStringObjectList();
    try
    {
        strings(list);
        list->saveToFile(fileName());
    }
    catch(...)
    {
        freeAndNil(list);
        throw;
    }
    freeAndNil(list);
}

void GLDMemIniFile::writeString(const GString &section, const GString &ident, const GString &value)
{
    GStringObjectList *strings = NULL;

    Q_D(GLDMemIniFile);
    int nIndex = d->m_sections->indexOf(section);
    if (nIndex >= 0)
    {
        strings = (GStringObjectList *)(d->m_sections->object(nIndex));
    }
    else
    {
        strings = addSection(section);
    }
    GString strTmp = ident + "=" + value;
    nIndex = strings->indexOfName(ident);
    if (nIndex >= 0)
    {
        strings->at(nIndex)->str = strTmp;
    }
    else
    {
        strings->add(strTmp);
    }
}

GStringObjectList *GLDMemIniFile::addSection(const GString &section)
{
    GHashedStringObjectList *result = new GHashedStringObjectList();
    try
    {
        Q_D(GLDMemIniFile);
        result->setCaseSensitive(caseSensitive());
        d->m_sections->addObject(section, result);
    }
    catch(...)
    {
        freeAndNil(result);
        throw;
    }
    return result;
}

bool GLDMemIniFile::caseSensitive()
{
    Q_D(GLDMemIniFile);
    return d->m_sections->caseSensitive();
}

void GLDMemIniFile::setCaseSensitive(bool value)
{
    Q_D(GLDMemIniFile);
    if (value != d->m_sections->caseSensitive())
    {
        d->m_sections->setCaseSensitive(value);
        for (int i = 0; i < d->m_sections->count(); ++i)
        {
            GHashedStringObjectList* hashList = (GHashedStringObjectList*)(d->m_sections->object(i));
            hashList->setCaseSensitive(value);
            hashList->changed();
            ((GHashedStringObjectList*)(d->m_sections))->changed();
        }
    }
}

void GLDMemIniFile::loadValues()
{
    if ((fileName().length() > 0) && fileExists(fileName()))
    {
        GStringObjectList *list = new GStringObjectList();
        try
        {
            list->loadFromFile(fileName());
            setStrings(list);
        }
        catch(...)
        {
            freeAndNil(list);
            throw;
        }
        freeAndNil(list);
    }
    else
    {
        clear();
    }
}

bool GLDMemIniFile::allowValueHasSpace()
{
    Q_D(GLDMemIniFile);
    return d->m_allowValueHasSpace;
}

void GLDMemIniFile::setAllowValueHasSpace(bool allowValueHasSpace)
{
    Q_D(GLDMemIniFile);
    d->m_allowValueHasSpace = allowValueHasSpace;
    loadValues();
}

