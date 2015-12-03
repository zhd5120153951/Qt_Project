#include "GLDXMLBuilder.h"
#include "GLDStrUtils.h"
#include "GLDXMLDoc.h"

const GString c_gconCDataFmt = "<![CDATA[%1]]>";
const GString c_gconCommentFmt = "<!--%1-->";

const char* c_gdefIndentText = "    ";
const GByteArray c_glineBreak = "\r\n";

class GXMLBuilderPrivate
{
public:
  GXMLBuilderPrivate(GXMLBuilder *parent);
private:
  GXMLBuilder * const q_ptr;
  Q_DECLARE_PUBLIC(GXMLBuilder);

  GStream *m_pStream;
  bool m_bAutoIndent;
  EnXMLEncode m_enEncoding;
  QByteArray m_strLineBreak;
  bool m_bReplaceChars;
};

GXMLBuilderPrivate::GXMLBuilderPrivate(GXMLBuilder *parent)
  : q_ptr(parent), m_bAutoIndent(false), m_enEncoding(XE_UTF8),
    m_strLineBreak(c_glineBreak), m_bReplaceChars(false)
{
}

GXMLBuilder::GXMLBuilder(GStream *stream) : d_ptr(new GXMLBuilderPrivate(this))
{
    Q_D(GXMLBuilder);
    assert(stream != NULL);
    d->m_pStream = stream;
}

GXMLBuilder::~GXMLBuilder()
{
    Q_D(GXMLBuilder);
    d->m_pStream = NULL;
    freePtr(d);
}

GString GXMLBuilder::saveDocText(CGLDXMLDocument *doc, bool replaceChars)
{
    GBlockMemoryStream *pStream = new GBlockMemoryStream;
    GXMLBuilder *pBuilder = new GXMLBuilder(pStream);
    GByteArray strTemp;
    try
    {
        pBuilder->saveXMLDocument(doc, replaceChars, false);
        pStream->seek(0);
        strTemp = pStream->readAll();
    }
    catch(...)
    {
        freeAndNil(pBuilder);
        freeAndNil(pStream);
        throw;
    }
    freeAndNil(pBuilder);
    freeAndNil(pStream);
    return GString::fromUtf8(strTemp);
}

void GXMLBuilder::saveDoc(CGLDXMLDocument *doc, GStream *stream)
{
    GXMLBuilder *pXMLBuilder = new GXMLBuilder(stream);
    try
    {
        pXMLBuilder->saveXMLDocument(doc, true, false);
    }
    catch (...)
    {
        freeAndNil(pXMLBuilder);
        throw;
    }
    freeAndNil(pXMLBuilder);
}

void GXMLBuilder::saveNode(CGLDXMLNode *node, GStream *stream)
{
    GXMLBuilder *pBuilder = new GXMLBuilder(stream);
    try
    {
        pBuilder->d_ptr->m_bAutoIndent = false;
        pBuilder->d_ptr->m_enEncoding = XE_ANSI;
        pBuilder->d_ptr->m_strLineBreak = "";
        pBuilder->d_ptr->m_bReplaceChars = true;
        pBuilder->recursionWriteNode(node);
    }
    catch(...)
    {
        freeAndNil(pBuilder);
        throw;
    }
    freeAndNil(pBuilder);
}

GString GXMLBuilder::getNodeOpenTag(CGLDXMLNode *node, bool end)
{
    assert(node != NULL);

    GString strOpenTag;
    if (node->attributesObj()->count() == 0)
    {
        strOpenTag = GString("<%1>").arg(node->name());
    }
    else
    {
        strOpenTag = GString("<%1 %2>").arg(node->name(), node->attributesObj()->xml());
    }

    if (end)
    {
        strOpenTag.insert(strOpenTag.length() - 1, '/');
    }
    return strOpenTag;
}

GString GXMLBuilder::getNodeCloseTag(CGLDXMLNode *node)
{
    assert(node != NULL);
    return GString("</%1>").arg(node->name());
}

void GXMLBuilder::saveXMLDocument(CGLDXMLDocument *doc, bool replaceChars, bool ignoreEncode)
{
    Q_D(GXMLBuilder);
    d->m_bAutoIndent = doc->autoIndent();
    if (ignoreEncode)
    {
        d->m_enEncoding = XE_ANSI;
    }
    else if (sameText(doc->encoding(), c_uft8Encoding)) // ×Ö·û´®ÓÅ»¯
    {
        d->m_enEncoding = XE_UTF8;
    }
    else
    {
        d->m_enEncoding = XE_ANSI;
    }

    if (doc->autoIndent())
    {
        d->m_strLineBreak = c_glineBreak;
    }
    else
    {
        d->m_strLineBreak = " ";
    }

    d->m_bReplaceChars = replaceChars;
    if (doc->includeHeader())
    {
        writeText(doc->getXMLHeader());
    }
    recursionWriteNode(doc->rootObj());
}

void GXMLBuilder::recursionWriteNode(CGLDXMLNode *node)
{
    Q_D(GXMLBuilder);
    if (NULL == node)
        return;

    int nCount = node->childNodesObj()->count();
    GString strValue;
    GString strLine;

    if (0 == nCount)
    {
        if (d->m_bReplaceChars)
        {
            strValue = node->text();
        }
        else
        {
            strValue = node->asString();
        }

        switch (node->nodeType())
        {
            case TEXT:
            {
                strLine = strValue;
                break;
            }
            case CDATA:
            {
                strLine = c_gconCDataFmt.arg(strValue);
                break;
            }
            case COMMENT:
            {
                strLine = c_gconCommentFmt.arg(strValue);
                break;
            }
            default:
            {
                if (!strValue.isEmpty())
                {
                    strLine = getNodeOpenTag(node, false) + strValue + getNodeCloseTag(node);
                }
                else
                {
                    strLine = getNodeOpenTag(node, true);
                }
                break;
            }
        }

        if (node->nodeType() != TEXT)
        {
            writeIndentText(node->level());
            writeText(strLine, true);
        }
        else
        {
            writeText(strLine, false);
        }
    }
    else
    {
        strLine = getNodeOpenTag(node);
        writeIndentText(node->level());

        if (node->childNodesObj()->nodeObj(0)->nodeType() != TEXT)
        {
            writeText(strLine);
        }
        else
        {
            writeText(strLine, false);
        }

        for (int nNode = 0; nNode < node->childNodesObj()->count(); ++nNode)
        {
            recursionWriteNode(dynamic_cast<CGLDXMLNode*>(node->childNodesObj()->nodeObj(nNode)));
        }

        strLine = getNodeCloseTag(node);
        if (node->childNodesObj()->nodeObj(0)->nodeType() != TEXT)
        {
            writeIndentText(node->level());
        }
        writeText(strLine);
    }
}

void GXMLBuilder::writeIndentText(int level)
{
    Q_D(GXMLBuilder);
    if (d->m_bAutoIndent)
    {
        while (level > 0)
        {
            d->m_pStream->write(c_gdefIndentText, 4);// to test
            --level;
        }
    }
}

void GXMLBuilder::writeText(const GString &text, bool addLineBreak)
{
    Q_D(GXMLBuilder);
    GByteArray strTemp;
    if (d->m_enEncoding == XE_UTF8)
    {
        strTemp = text.toUtf8();
    }
    else
    {
        strTemp = strToByteArray(text);
    }
    d->m_pStream->write(strTemp.data(), strTemp.length()); //to test

    if (addLineBreak && !d->m_strLineBreak.isEmpty())
    {
        d->m_pStream->write(d->m_strLineBreak, d->m_strLineBreak.length());// WriteBuffer(Pointer(FLineBreak)^, Length(FLineBreak) * SizeOf(AnsiChar))
    }

}
