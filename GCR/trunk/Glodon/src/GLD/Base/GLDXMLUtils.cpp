#include "GLDXMLUtils.h"

#include "GLDFile.h"
#include "GLDTextStream.h"
#include "GLDGlobal.h"
#include "GLDStrUtils.h"
#include "GLDMathUtils.h"
#include "GLDException.h"
#include "GLDXMLDoc.h"

#define DEFXMLUTF8HEADER_LENGTH 3
const GString c_defXMLDataHeader = "<?xml ";
const char c_defXMLUtf8Header[DEFXMLUTF8HEADER_LENGTH] = {239 - 256, 187 - 256, 191 - 256};

bool isChar(const QChar &ch)
{
    if ((ch >= 'a') && (ch <= 'z'))
    {
        return true;
    }
    else if ((ch >= 'A') && (ch <= 'Z'))
    {
        return true;
    }
    return false;
}

bool ucstricmp(const QChar *a, int length_a, const QChar *b, int )
{
    if (a == b)
    {
        return true;
    }
    for (int i = 0; i < length_a; ++i)
    {
        QChar qchar1 = a[i];
        QChar qchar2 = b[i];
        if ((isChar(qchar1)) && (isChar(qchar2)))
        {
            char char1 = qchar1.toLatin1();
            char char2 = qchar2.toLatin1();
            if ((char1 == char2) || (char1 == char2 + 32) || (char2 == char1 + 32))
            {
                continue;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (qchar1 == qchar2)
            {
                continue;
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

bool innerSameText(const GString &s1, const GString &s2)
{
    int nLength1 = s1.length();
    int nLength2 = s2.length();
    if ((nLength1 != nLength2) || (nLength2 == 0) || (nLength1 == 0))
    {
        return false;
    }
    return ucstricmp(s1.data(), nLength1, s2.data(), nLength2);
}

GXMLNode firstChildNode(const GXMLNode &node, const GString &tagName)
{
    return node.firstChildElement(tagName);
}

GXMLNode findChildNode(const GXMLNode &node, const GString &nodeName)
{
    GXMLNode child = node.firstChildElement();
    while (!child.isNull())
    {
        if (innerSameText(child.nodeName(), nodeName))
        {
            return child;
        }
        child = child.nextSiblingElement();
    }
    return child;
}

GXMLNode addChild(GXMLNode &node, const GString &nodeName)
{
    GXMLNode child = node.ownerDocument().createElement(nodeName);
    node.appendChild(child);
    return child;
}

GString readStrFromXML(const GXMLNode &node, const GString &nodeName, const GString &defVal)
{
    GXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return child.text();
    else
        return defVal;
}

bool readBoolFromXML(const GXMLNode &node, const GString &nodeName, bool defVal)
{
    GXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return sameText(child.text(), "True");
    else
        return defVal;
}

int readIntFromXML(const GXMLNode &node, const GString &nodeName, int defVal)
{
    GXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return strToIntDef(child.text(), 0);
    else
        return defVal;
}

long long readInt64FromXML(const GXMLNode &node, const GString &nodeName, long long defVal)
{
    GXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return strToInt64Def(child.text(), defVal);
    else
        return defVal;
}

guint64 readUInt64FromXML(const GXMLNode &node, const GString &nodeName, guint64 defVal)
{
    GXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return strToUInt64Def(child.text(), defVal);
    else
        return defVal;
}

double readFloatFromXML(const GXMLNode &node, const GString &nodeName, double defVal)
{
    GXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return strToFloatDef(child.text(), defVal);
    else
        return defVal;
}

GString readStrFromXMLAttr(const GXMLNode &node, const GString &attrName, const GString &defVal)
{
    if (node.hasAttribute(attrName))
        return node.attribute(attrName);
    else
        return defVal;
}

bool readBoolFromXMLAttr(const GXMLNode &node, const GString &attrName, bool defVal)
{
    if (node.hasAttribute(attrName))
        return sameText(node.attribute(attrName), "True");
    else
        return defVal;
}

int readIntFromXMLAttr(const GXMLNode &node, const GString &attrName, int defVal)
{
    if (node.hasAttribute(attrName))
        return strToIntDef(node.attribute(attrName), defVal);
    else
        return defVal;
}

gint64 readInt64FromXMLAttr(const GXMLNode &node, const GString &attrName, gint64 defVal)
{
    if (node.hasAttribute(attrName))
        return strToInt64Def(node.attribute(attrName), defVal);
    else
        return defVal;
}

guint64 readUInt64FromXMLAttr(const GXMLNode &node, const GString &attrName, guint64 defVal)
{
    if (node.hasAttribute(attrName))
        return strToUInt64Def(node.attribute(attrName), defVal);
    else
        return defVal;
}

double readFloatFromXMLAttr(const GXMLNode &node, const GString &attrName, double defVal)
{
    if (node.hasAttribute(attrName))
        return strToFloatDef(node.attribute(attrName), defVal);
    else
        return defVal;
}

void writeStrToXML(GXMLNode &node, const GString &nodeName, const GString &value, const GString &defVal)
{
    if ((value == defVal) && findChildNode(node, nodeName).isNull())
        return;

    GXMLNode child = addChild(node, nodeName);
#ifdef GLD_XML
    child.setAsString(value);
#else
    GDomText text = node.ownerDocument().createTextNode(value);
    child.appendChild(text);
#endif
}

void writeBoolToXML(GXMLNode &node, const GString &nodeName, bool value, bool defVal)
{
    if ((value == defVal) && findChildNode(node, nodeName).isNull())
        return;

    if (value)
        writeStrToXML(node, nodeName, "True");
    else
        writeStrToXML(node, nodeName, "False");

}

void writeIntToXML(GXMLNode &node, const GString &nodeName, int value, int defVal)
{
    if ((value == defVal) && findChildNode(node, nodeName).isNull())
        return;

    writeStrToXML(node, nodeName, intToStr(value));
}

void writeInt64ToXML(GXMLNode &node, const GString &nodeName, long long value, long long defVal)
{
    if ((value == defVal) && findChildNode(node, nodeName).isNull())
        return;

    writeStrToXML(node, nodeName, int64ToStr(value));
}

void writeUInt64ToXML(GXMLNode &node, const GString &nodeName, guint64 value, guint64 defVal)
{
    if ((value == defVal) && findChildNode(node, nodeName).isNull())
        return;

    writeStrToXML(node, nodeName, uint64ToStr(value));
}

void writeFloatToXML(GXMLNode &node, const GString &nodeName, double value, double defVal)
{
    if (sameValue(value, defVal) && findChildNode(node, nodeName).isNull())
        return;

    writeStrToXML(node, nodeName, floatToStr(value));
}

void writeStrToXMLAttr(GXMLNode &node, const GString &attrName, const GString &value, const GString &defVal)
{
    if ((value == defVal) && (!node.hasAttribute(attrName)))
        return;

    node.setAttribute(attrName, value);
}

void writeBoolToXMLAttr(GXMLNode &node, const GString &attrName, bool value, bool defVal)
{
    if ((value == defVal) && (!node.hasAttribute(attrName)))
        return;

    if (value)
        writeStrToXMLAttr(node, attrName, "True");
    else
        writeStrToXMLAttr(node, attrName, "False");
}

void writeIntToXMLAttr(GXMLNode &node, const GString &attrName, int value, int defVal)
{
    if ((value == defVal) && (!node.hasAttribute(attrName)))
        return;

    node.setAttribute(attrName, value);
}

void writeUIntToXMLAttr(GXMLNode &node, const GString &attrName, uint value, uint defVal)
{
    if ((value == defVal) && (!node.hasAttribute(attrName)))
        return;

    node.setAttribute(attrName, value);
}

void writeFloatToXMLAttr(GXMLNode &node, const GString &attrName, double value, double defVal)
{
    if (sameValue(value, defVal) && (!node.hasAttribute(attrName)))
        return;

    node.setAttribute(attrName, floatToStr(value));
}

GLDXMLNode firstChildNode(const GLDXMLNode &node, const GString &tagName)
{
    return node.firstChildElement(tagName);
}

GLDXMLNode findChildNode(const GLDXMLNode &node, const GString &nodeName)
{
//    GDomNode result = node.namedItem(nodeName);
//    if (result.isNull())
//        return GLDXMLNode();
//    else
//        return result.toElement();
    GLDXMLNode child = node.firstChildElement();
    while (!child.isNull())
    {
        if (sameText(child.nodeName(), nodeName))
        {
            return child;
        }
        child = child.nextSiblingElement();
    }
    return child;
}

GLDXMLNode addChild(GLDXMLNode &node, const GString &nodeName)
{
    GLDXMLNode child = node.ownerDocument().createElement(nodeName);
    node.appendChild(child);
    return child;
}

GString readStrFromXML(const GLDXMLNode &node, const GString &nodeName, const GString &defVal)
{
    GLDXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return child.text();
    else
        return defVal;
}

bool readBoolFromXML(const GLDXMLNode &node, const GString &nodeName, bool defVal)
{
    GLDXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return sameText(child.text(), "True");
    else
        return defVal;
}

int readIntFromXML(const GLDXMLNode &node, const GString &nodeName, int defVal)
{
    GLDXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return strToIntDef(child.text(), 0);
    else
        return defVal;
}

long long readInt64FromXML(const GLDXMLNode &node, const GString &nodeName, long long defVal)
{
    GLDXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return strToInt64Def(child.text(), defVal);
    else
        return defVal;
}

guint64 readUInt64FromXML(const GLDXMLNode &node, const GString &nodeName, guint64 defVal)
{
    GLDXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return strToUInt64Def(child.text(), defVal);
    else
        return defVal;
}

double readFloatFromXML(const GLDXMLNode &node, const GString &nodeName, double defVal)
{
    GLDXMLNode child = findChildNode(node, nodeName);
    if (!child.isNull())
        return strToFloatDef(child.text(), defVal);
    else
        return defVal;
}

GString readStrFromXMLAttr(const GLDXMLNode &node, const GString &attrName, const GString &defVal)
{
    if (node.hasAttribute(attrName))
        return node.attribute(attrName);
    else
        return defVal;
}

bool readBoolFromXMLAttr(const GLDXMLNode &node, const GString &attrName, bool defVal)
{
    if (node.hasAttribute(attrName))
        return sameText(node.attribute(attrName), "True");
    else
        return defVal;
}

int readIntFromXMLAttr(const GLDXMLNode &node, const GString &attrName, int defVal)
{
    if (node.hasAttribute(attrName))
        return strToIntDef(node.attribute(attrName), defVal);
    else
        return defVal;
}

gint64 readInt64FromXMLAttr(const GLDXMLNode &node, const GString &attrName, gint64 defVal)
{
    if (node.hasAttribute(attrName))
        return strToInt64Def(node.attribute(attrName), defVal);
    else
        return defVal;
}

guint64 readUInt64FromXMLAttr(const GLDXMLNode &node, const GString &attrName, guint64 defVal)
{
    if (node.hasAttribute(attrName))
        return strToUInt64Def(node.attribute(attrName), defVal);
    else
        return defVal;
}

double readFloatFromXMLAttr(const GLDXMLNode &node, const GString &attrName, double defVal)
{
    if (node.hasAttribute(attrName))
        return strToFloatDef(node.attribute(attrName), defVal);
    else
        return defVal;
}

void writeStrToXML(GLDXMLNode &node, const GString &nodeName, const GString &value, const GString &defVal)
{
    if ((value == defVal) && findChildNode(node, nodeName).isNull())
        return;

    GLDXMLNode child = addChild(node, nodeName);
    child.setAsString(value);
}

void writeBoolToXML(GLDXMLNode &node, const GString &nodeName, bool value, bool defVal)
{
    if ((value == defVal) && findChildNode(node, nodeName).isNull())
        return;

    if (value)
        writeStrToXML(node, nodeName, "True");
    else
        writeStrToXML(node, nodeName, "False");

}

void writeIntToXML(GLDXMLNode &node, const GString &nodeName, int value, int defVal)
{
    if ((value == defVal) && findChildNode(node, nodeName).isNull())
        return;

    writeStrToXML(node, nodeName, intToStr(value));
}

void writeInt64ToXML(GLDXMLNode &node, const GString &nodeName, long long value, long long defVal)
{
    if ((value == defVal) && findChildNode(node, nodeName).isNull())
        return;

    writeStrToXML(node, nodeName, int64ToStr(value));
}

void writeUInt64ToXML(GLDXMLNode &node, const GString &nodeName, guint64 value, guint64 defVal)
{
    if ((value == defVal) && findChildNode(node, nodeName).isNull())
        return;

    writeStrToXML(node, nodeName, uint64ToStr(value));
}

void writeFloatToXML(GLDXMLNode &node, const GString &nodeName, double value, double defVal)
{
    if (sameValue(value, defVal) && findChildNode(node, nodeName).isNull())
        return;

    writeStrToXML(node, nodeName, floatToStr(value));
}

void writeStrToXMLAttr(GLDXMLNode &node, const GString &attrName, const GString &value, const GString &defVal)
{
    if ((value == defVal) && (!node.hasAttribute(attrName)))
        return;

    node.setAttribute(attrName, value);
}

void writeBoolToXMLAttr(GLDXMLNode &node, const GString &attrName, bool value, bool defVal)
{
    if ((value == defVal) && (!node.hasAttribute(attrName)))
        return;

    if (value)
        writeStrToXMLAttr(node, attrName, "True");
    else
        writeStrToXMLAttr(node, attrName, "False");
}

void writeIntToXMLAttr(GLDXMLNode &node, const GString &attrName, int value, int defVal)
{
    if ((value == defVal) && (!node.hasAttribute(attrName)))
        return;

    node.setAttribute(attrName, value);
}

void writeUIntToXMLAttr(GLDXMLNode &node, const GString &attrName, uint value, uint defVal)
{
    if ((value == defVal) && (!node.hasAttribute(attrName)))
        return;

    node.setAttribute(attrName, value);
}

void writeFloatToXMLAttr(GLDXMLNode &node, const GString &attrName, double value, double defVal)
{
    if (sameValue(value, defVal) && (!node.hasAttribute(attrName)))
        return;

    node.setAttribute(attrName, floatToStr(value));
}

GXMLDocument createXMLDocument(bool aIncludeHeader, bool autoIndent)
{
#ifdef GLD_XML

#else
    G_UNUSED(aIncludeHeader);
    G_UNUSED(autoIndent);
    GXMLDocument doc;
    GDomProcessingInstruction instruction = doc.createProcessingInstruction(
                "xml", "version=\"1.0\" encoding=\"utf-8\"");
    doc.appendChild(instruction);
    return doc;
#endif
}

GXMLDocument loadXMLDocument(const GString &fileName)
{
    GFileStream file(fileName);
    if (!file.open(GStream::ReadOnly))
        return GXMLDocument();
    GXMLDocument result;
    QString errorMsg;
    int nRow = 0;
    int nColumn = 0;
    if (!result.setContent(&file, &errorMsg, &nRow, &nColumn))
    {
        //qWarning() << "load xml error: " << errorMsg << ", Row: " << nRow << ", Column: " << nColumn;
        file.close();
        return GXMLDocument();
    }
    else
    {
        file.close();
        return result;
    }
}

GXMLDocument loadXMLDocument(GStream *stream)
{
    GXMLDocument result;
    if (!result.setContent(stream))
        return GXMLDocument();
    else
        return result;
}

void saveXMLDocument(GXMLDocument &doc, const GString &fileName)
{
    GFileStream file(fileName);
    if (!file.open(GStream::WriteOnly))
    {
        return;
    }
    if (!doc.firstChild().isProcessingInstruction())
    {
        GDomProcessingInstruction xmlInfoNode = doc.createProcessingInstruction(
                    "xml", "version=\"1.0\" encoding=\"utf-8\" ");
        doc.insertBefore(xmlInfoNode, doc.documentElement());
    }
    GTextStream oStream(&file);
    doc.save(oStream, 1);
    file.close();
}

void saveXMLDocument(GXMLDocument &doc, GStream *stream)
{
    if (!doc.firstChild().isProcessingInstruction())
    {
        GDomProcessingInstruction xmlInfoNode = doc.createProcessingInstruction(
                    "xml", "version=\"1.0\" encoding=\"utf-8\" ");
        doc.insertBefore(xmlInfoNode, doc.documentElement());
    }
    GTextStream oStream(stream);
    doc.save(oStream, 1);
}

//设计：Linc 2004.12.13
//功能：判断一个流是否 XML 流
bool isXMLStream(GStream *stream)
{
    char cBuffer;
    assert(stream != NULL);
    bool result = false;
    int nPos = 0;
    try
    {
        nPos = stream->pos();
        int nSize = DEFXMLUTF8HEADER_LENGTH;
        GByteArray byteArray = stream->read(nSize);
        int nRead = byteArray.length();
        // 跳过 XML UTF-8 的 BOM 标记
        if ((nRead != nSize) || (0 != compareText(byteArray, GByteArray(c_defXMLUtf8Header, DEFXMLUTF8HEADER_LENGTH))))
        {
            stream->seek(nPos);
        }
        do
        {
            nRead = stream->read(&cBuffer, sizeof(char));
        } while ((nRead != 0)
                 && ((cBuffer == 10) || (cBuffer == 13) || (cBuffer == 9) || (cBuffer == ' ')));//跳过空白行
        stream->seek(stream->pos() - 1);
        nSize = length(c_defXMLDataHeader);

        GString strHeader = utf8ToUnicode(stream->read(nSize).constData(), nSize);
        nRead = length(strHeader);
        result = (nRead == nSize) && (strHeader == c_defXMLDataHeader);
    }
    catch(...)
    {
        stream->seek(nPos);
        throw;
    }
    stream->seek(nPos);
    return result;
}

void clearNodes(GXMLNode &node)
{
    GXMLNode child = node.firstChildElement();
    while (!child.isNull())
    {
        node.removeChild(child);
        child = child.nextSiblingElement();
    }
}

int posXMLString(GString strSub, GString strValue, int nStart)
{
    int nLen1 = strSub.length();
    int nLen2 = strValue.length();
    int result = -1;
    if (nLen1 == 0 || (nStart + nLen1 > nLen2 + 1))
    {
        return result;
    }
    int nIndex = nStart;
    int nPos = 0;
    while (nIndex < nLen2)
    {
        nPos = 0;
        while (nPos < nLen1 && (strSub.at(nPos) == strValue.at(nIndex)))
        {
            ++nPos;
            ++nIndex;
        }
        if (nPos == nLen1)
        {
            result = nIndex - nLen1;
            break;
        }
        else
        {
            ++nIndex;
        }
    }

    return result;
}


void xmlDocError(const GString &msg)
{
    throw GLDException(msg);
}

void xmlDocError(const GString &msg, const GVariantList &variantList)
{
    throw GLDException(format(msg, variantList));
}


GLDXMLDocument loadGLDXMLDocument(const GString &fileName)
{
    GFileStream file(fileName);
    if (!file.open(GStream::ReadOnly))
        return GLDXMLDocument();
    GLDXMLDocument result;
    if (!result.setContent(&file))
    {
        file.close();
        return GLDXMLDocument();
    }
    else
    {
        file.close();
        return result;
    }
}


GLDXMLDocument createGLDXMLDocument(bool aIncludeHeader, bool autoIndent)
{
    CGLDXMLDocument *xmlDocument = new CGLDXMLDocument();
    xmlDocument->setAutoIndent(autoIndent);
    xmlDocument->setIncludeHeader(aIncludeHeader);
    return xmlDocument;
}


GLDXMLDocument loadGLDXMLDocument(GStream *stream)
{
    GLDXMLDocument result;
    if (!result.setContent(stream))
        return GLDXMLDocument();
    else
        return result;
}


void saveGLDXMLDocument(GLDXMLDocument &doc, const GString &fileName)
{
    GFileStream file(fileName);
    if (!file.open(GStream::WriteOnly))
    {
        return;
    }
    doc.saveToStream(&file);
    file.close();
}


void saveGLDXMLDocument(GLDXMLDocument &doc, GStream *stream)
{
    doc.saveToStream(stream);
}
