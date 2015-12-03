#include "GLDXMLDoc.h"
#include "GLDStrUtils.h"
#include "GLDFile.h"
#include "GLDXMLParser.h"
#include "GLDXMLUtils.h"
#include "GLDStreamUtils.h"
#include "GLDXMLBuilder.h"

GChar g_CurrentDecimalSeparator;
const GString c_gdefXMLDataHeader = "<?xml";
const char c_gdefXMLUtf8Header[] = "\xef\xbb\xbf";
const GChar c_gdefXMLDecimalSeparator = '.';
const GString c_gdefEncodingPrex  = "encoding=\"";
const GString c_gconCDataPrex = "<![CDATA[";
const GString c_gconCDataPosx = "]]>";
const GString c_gconCommentPrex = "<!--";
const GString c_gconCommentPosx = "-->";
const GString c_gconSpecificationFmt = "<?xml %1 ?>";
const GString c_gconSpecificationFmtEx = "<?xml %1 %2?>";
const GString c_gconAttribFmt = "%1=\"%2\"";
const GString c_gconEncoding = "encoding";
const GString c_gconVersion = "version";
const GString c_uft8Encoding = "utf-8";
const GString c_gdefVersion = "1.0";
const GString c_gconLtTag = "&lt;";
const GString c_gconGtTag  = "&gt;";
const GString c_gconAmpTag  = "&amp;";
const GString c_gconAposTag  = "&apos;";
const GString c_gconQuotTag  = "&quot;";

const GString c_gconXATag  = "&#xA;";
const GString c_gconXDTag   = "&#xD;";
const GString c_gconXaTag  = "&#xa;";
const GString c_gconXdTag   = "&#xd;";

const GString c_grsErrInvalidRootNode = TRANS_STRING("XML 文档只能有一个顶层节点");
const GString c_gdefNodeName[4] = {"#element","#text","#cdata-section","#comment"};

void replaceText(const GString &subStr, GChar aChar, GString &result)
{
    int nPos = 0;
    nPos = posXMLString(subStr, result, nPos);

    while (nPos != -1)
    {
        result.replace(nPos, subStr.length(), aChar);
        nPos = posXMLString(subStr, result, nPos + 1);
    }
}

GString decodeXMLString(const GString &value, bool decodeCrLf)
{
    int nTemp;
    GString strResult = "";
    int nLength = value.length();

    if (0 == nLength)
    {
        return strResult;
    }
    GString strBuffer = value;
    bool bHasTag = false;

    for (nTemp = 0; nTemp < nLength; ++nTemp)
    {
        GChar aChar = strBuffer[nTemp];
        if (aChar == '&')
        {
            bHasTag = true;
            break;
        }
    }

    if (bHasTag)
    {
        replaceText(c_gconLtTag, '<', strBuffer);
        replaceText(c_gconGtTag, '>', strBuffer);
        replaceText(c_gconAmpTag, '&', strBuffer);
        replaceText(c_gconAposTag, '\'', strBuffer);
        replaceText(c_gconQuotTag, '\"', strBuffer);
        if (decodeCrLf)
        {
            replaceText(c_gconXATag, '\n', strBuffer);
            replaceText(c_gconXDTag, '\r', strBuffer);
            replaceText(c_gconXaTag, '\n', strBuffer);
            replaceText(c_gconXdTag, '\r', strBuffer);
        }
    }

    strResult = strBuffer;
    return strResult;

}

int adjustUtf8BufferSize(int nPos, int nReadLen, GStream *inStream)
{
    int result = 0;
    char *arrLastWord = NULL;
    byte cLast;
    byte cPenult;
    if (nReadLen < 2)
    {
        return result;
    }
    inStream->seek(nPos + nReadLen - 2);
    inStream->read(&arrLastWord[0], 2);
    cLast = arrLastWord[1];
    cPenult = arrLastWord[0];
    if (cLast <= 0X7F)
    {
        result = 0;
    }
    else if ((cLast > 0X7F) && (cLast <= 0XBF))
    {
        if (cPenult > 0XDF && cPenult <= 0XEF)
        {
            result = 2;
        }
        else
        {
            result = 0;
        }
    }
    else if (cLast > 0XBF && cLast < 0XDF)
    {
        result = 1;
    }
    else if (cLast > 0XDF && cLast <= 0XEF)
    {
        result = 1;
    }
    else
    {
        result = 1;
    }
    return result;
}

GString utf8StreamDecode(GStream* inStream)
{
    if (inStream->size() == 0)
    {
        return "";
    }
    GString result = "";
    int nMaxBufSize = 1 * 1024 * 1024;
    GString strDest;
    int nReadLen = 0;
    int nPos = 0;
    int nUtf8Delta = 0;
    int nIndex = 0;
    nIndex = inStream->size();
    if (nIndex == 0)
    {
        return result;
    }
    inStream->seek(0);
    if (nIndex < nMaxBufSize)
    {
        //QString strBuffer;
        //inStream->read(strToByteArray(strBuffer).data(), inStream->size());
        //result = strBuffer;
        result = readFromStream(inStream,inStream->size());
    }
    else
    {
        nPos = 0;
        while (nPos < nIndex)
        {
            if (nPos + nMaxBufSize <= nIndex)
            {
                nReadLen = nMaxBufSize;
            }
            else
            {
                nReadLen = nIndex - nPos;
            }
            nUtf8Delta = adjustUtf8BufferSize(nPos, nReadLen, inStream);
            if (nUtf8Delta == -1)
            {
                //QException.raise();
                return "";
            }

            nReadLen = nReadLen - nUtf8Delta;
            inStream->seek(nPos);
            //QString strBuffer;
            //inStream->read(strToByteArray(strBuffer).data(), nReadLen);
            //strDest = utf8ToUnicode(strToByteArray(strBuffer).data());
            strDest = readFromStream(inStream, nReadLen);
            result = result + strDest;
            nPos += nReadLen;
        }
    }
    return result;
}

int posExcludeQuotationMark(const GChar &xmlChar, const GString &xmlString, int offset = 0)  // 注意数组下标问题
{
    int nPos = -1;

    if (xmlChar.isNull() || xmlString.isEmpty())
    {
        return nPos;
    }

    int nLength = xmlString.length();
    int nIndex = offset;

    if (nIndex < 0)
    {
        nIndex = 0;
    }
    else if (nIndex >= nLength)
    {
        nIndex = nLength - 1;
    }

    int nQuoTimes = 0;
    int nAposTimes = 0;

    while (nIndex < nLength)
    {
        const GChar indexChar = xmlString[nIndex];
        if ('\"' == indexChar)
        {
            ++nQuoTimes;
        }
        else if ('\'' == indexChar)
        {
            ++nAposTimes;
        }
        else if ((indexChar == xmlChar) && (nQuoTimes % 2 == 0) && (nAposTimes % 2 == 0))
        {
            nPos = nIndex;
            break;
        }
        ++nIndex;
    }
    return nPos;
}

GString copyTrimXMLString(GString &value, int start, int end)
{
    GString strResult = "";
    int nLength = value.length();
    if (end == nLength - 1)
    {
        end = nLength - 1;
    }
    while (start <=  end && value[start] <= ' ')
    {
        ++start;
    }
    while ( end >= start && value[end] <= ' ')
    {
        --end;
    }
    if (start >  end)
    {
        strResult = "";
    }
    else
    {
        nLength =  end - start  +  1;
        strResult = value.mid(start, nLength);
    }
    return strResult;
}

class CGLDXMLNodePrivate
{
public:
    CGLDXMLNodePrivate::CGLDXMLNodePrivate(CGLDXMLNode *parent): q_ptr(parent)
    {
    }

private:
    CGLDXMLNode * const q_ptr;
    Q_DECLARE_PUBLIC(CGLDXMLNode);

    EnXMLNodeType m_enNodeType;
    GString m_strName;
    GString m_strValue;
    EnTXMLOptions m_enOptions;
    CGLDXMLNode *m_pParent;
    CGLDXMLNodeAttributes *m_pAttributes;
    CGLDXMLNodeList *m_pNodeList;
    CGLDXMLDocument *m_pOwnerDocument;
};

/* CGLDXMLNode */
CGLDXMLNode::CGLDXMLNode(IGLDXMLNode *pParentNode, EnTXMLOptions enOptions,
                         EnXMLNodeType enType, IGLDXMLDocument *ownerDocument)
    : d_ptr(new CGLDXMLNodePrivate(this)), GInterfaceObject()
{
    Q_D(CGLDXMLNode);
    d->m_enNodeType = enType;
    d->m_enOptions = enOptions;
    d->m_pParent = dynamic_cast<CGLDXMLNode*> (pParentNode);
    d->m_pOwnerDocument = dynamic_cast<CGLDXMLDocument*>(ownerDocument);

    d->m_pAttributes = new CGLDXMLNodeAttributes(enOptions);
    d->m_pAttributes->AddRef();
    d->m_pNodeList = new CGLDXMLNodeList(this);
    d->m_pNodeList->AddRef();
}

CGLDXMLNode::~CGLDXMLNode()
{
    Q_D(CGLDXMLNode);
    freeAndNilIntf(d->m_pNodeList);
    freeAndNilIntf(d->m_pAttributes);
    freePtr(d);
}

IGLDXMLNode *CGLDXMLNode::addChild(GString strName)
{
    Q_D(CGLDXMLNode);
    CGLDXMLNode *node = new CGLDXMLNode(this, d->m_enOptions, ELEMENT, d->m_pOwnerDocument);
    node->setName(strName);
    d->m_pNodeList->add(node);
    if (node != NULL)
    {
        node->AddRef();
    }
    return node;
}

IGLDXMLNode *CGLDXMLNode::cloneNode(bool deep)
{
    CGLDXMLNode *pNode = cloneNodeObj(deep);
    if (pNode != NULL)
    {
        pNode->AddRef();
    }
    return pNode;
}

bool CGLDXMLNode::hasAttribute(const GString &strName) const
{
    Q_D(const CGLDXMLNode);
    return d->m_pAttributes->indexOf(strName) > -1;
}

IGLDXMLNode *CGLDXMLNode::nextSibling()
{
    CGLDXMLNode *pNode = nextSiblingObj();
    if (pNode != NULL)
    {
        pNode->AddRef();
    }
    return pNode;
}

IGLDXMLNode *CGLDXMLNode::prevSibling()
{
    CGLDXMLNode *pNode = prevSiblingObj();
    if (pNode != NULL)
    {
        pNode->AddRef();
    }
    return pNode;
}

IGLDXMLNode *CGLDXMLNode::firstChildElement(const GString &tagName) const
{
    Q_D(const CGLDXMLNode);
    CGLDXMLNode *pNode = d->m_pNodeList->findNodeObj(tagName);
    if (pNode != NULL)
    {
        pNode->AddRef();
    }
    return pNode;
}

IGLDXMLNode *CGLDXMLNode::insertBefore(IGLDXMLNode *newChild, IGLDXMLNode *refChild)
{
    Q_D(CGLDXMLNode);
    CGLDXMLNode *pNewChild = dynamic_cast<CGLDXMLNode*>(newChild);
    CGLDXMLNode *pRefChild = dynamic_cast<CGLDXMLNode*>(refChild);
    if (!pNewChild)
    {
        return NULL;
    }
    else
    {
        int nRefIndex = d->m_pNodeList->indexOf(pRefChild);
        int nNewIndex = d->m_pNodeList->indexOf(pNewChild);
        if (-1 == nRefIndex)
        {
            if (-1 == nNewIndex)
            {
                d->m_pNodeList->insert(0, pNewChild);
            }
            else
            {
                d->m_pNodeList->move(nNewIndex, 0);
            }
        }
        else
        {
            if (-1 == nNewIndex)
            {
                d->m_pNodeList->insert(nRefIndex, pNewChild);
            }
            else
            {
                d->m_pNodeList->move(nNewIndex, nRefIndex);
            }
        }
    }

    if (pNewChild != NULL)
    {
        pNewChild->AddRef();
    }
    return pNewChild;
}

bool CGLDXMLNode::asBoolean()
{
    Q_D(CGLDXMLNode);
    return sameText("true", d->m_strValue);
}

double CGLDXMLNode::asFloat()
{
    Q_D(CGLDXMLNode);
    return strToFloat(d->m_strValue);
}

gint64 CGLDXMLNode::asInt64()
{
    Q_D(CGLDXMLNode);
    return strToInt64(d->m_strValue);
}

int CGLDXMLNode::asInteger()
{
    Q_D(CGLDXMLNode);
    return strToInt(d->m_strValue);
}

GString CGLDXMLNode::asString()
{
    Q_D(CGLDXMLNode);
    //用byte（char）存储的枚举  给枚举赋值是  char c=9 不能为 char c='9'（变成了arsii码）,
    return decodeXMLString(d->m_strValue, contains(d->m_enOptions, XO_DECODE_CRLF));
}

GVariant CGLDXMLNode::asVariant()
{
    // TODO
    return GVariant();
//    return base64Decode(d->m_strValue.toLocal8Bit());
}

IGLDXMLNodeAttributes *CGLDXMLNode::attributes() const
{
    Q_D(const CGLDXMLNode);
    if (d->m_pAttributes)
    {
        d->m_pAttributes->AddRef();
    }
    return d->m_pAttributes;
}

IGLDXMLNodeList *CGLDXMLNode::childNodes() const
{
    Q_D(const CGLDXMLNode);
    if (d->m_pNodeList)
    {
        d->m_pNodeList->AddRef();
    }
    return d->m_pNodeList;
}

int CGLDXMLNode::level()
{
    Q_D(CGLDXMLNode);
    int result = 0;
    CGLDXMLNode *pParent = d->m_pParent;
    while (pParent != NULL)
    {
        pParent = pParent->parentObj();
        ++result;
    }
    return result;
}

QString CGLDXMLNode::name()
{
    Q_D(CGLDXMLNode);
    return d->m_strName;
}

EnXMLNodeType CGLDXMLNode::nodeType()
{
    Q_D(CGLDXMLNode);
    return d->m_enNodeType;
}

IGLDXMLNode *CGLDXMLNode::parent() const
{
    Q_D(const CGLDXMLNode);
    if (d->m_pParent)
    {
        d->m_pParent->AddRef();
    }
    return d->m_pParent;
}

QString CGLDXMLNode::text() const
{
    Q_D(const CGLDXMLNode);
    return d->m_strValue;
}

QString CGLDXMLNode::xml()
{
    Q_D(CGLDXMLNode);
    switch (d->m_enNodeType)
    {
    case TEXT:
        return d->m_strValue;
    case CDATA:
        return GString(c_gconCDataPrex).append(d->m_strValue).append(c_gconCDataPosx);
    case COMMENT:
        return GString(c_gconCommentPrex).append(d->m_strValue).append(c_gconCommentPosx);
    default:
        if (d->m_pAttributes->count() > 0)
        {
            return GString("<").append(d->m_strName).append(" ").append(d->m_pAttributes->xml()).append(">");
        }
        else
        {
            return GString("<").append(d->m_strName).append(">");
        }
    }

    return "";
}

QString CGLDXMLNode::nodeValue() const
{
    return const_cast<CGLDXMLNode*>(this)->asString();
}

IGLDXMLNodeAttribute *CGLDXMLNode::attributeNode(const QString &name)
{
    Q_D(CGLDXMLNode);
    if(d->m_pAttributes->indexOf(name) == -1)
        return NULL;

    return d->m_pAttributes->itemsByName(name);
}

IGLDXMLNode *CGLDXMLNode::replaceChild(IGLDXMLNode *newChild, IGLDXMLNode *oldChild)
{
    Q_D(CGLDXMLNode);
    if (!newChild)
    {
        return NULL;
    }
    else
    {
        int nValue = d->m_pNodeList->indexOf(oldChild);
        int nTemp = d->m_pNodeList->indexOf(newChild);

        if (-1 == nValue)
        {
            return NULL;
        }
        else
        {
            if (-1 == nTemp)
            {
                d->m_pNodeList->insert(nValue, newChild);
                d->m_pNodeList->deleteNode(nValue + 1);
            }
            else
            {
                d->m_pNodeList->deleteNode(newChild->name());
                d->m_pNodeList->insert(nValue, newChild);
                d->m_pNodeList->deleteNode(nValue + 1);
            }
        }
    }

    if (newChild != NULL)
    {
        newChild->AddRef();
    }
    return newChild;
}

IGLDXMLNode *CGLDXMLNode::insertAfter(IGLDXMLNode *newChild, IGLDXMLNode *refChild)
{
    Q_D(CGLDXMLNode);
    if (!newChild)
    {
        return NULL;
    }
    else
    {
        int nValue = d->m_pNodeList->indexOf(refChild);
        int nTemp = d->m_pNodeList->indexOf(newChild);
        if (-1 == nValue)
        {
            if (-1 == nTemp)
            {
                d->m_pNodeList->insert(0, newChild);
            }
            else
            {
                d->m_pNodeList->deleteNode(newChild->name());
                d->m_pNodeList->insert(0, newChild);
            }
        }
        else
        {
            if (-1 == nTemp)
            {
                d->m_pNodeList->insert(nValue + 1, newChild);
            }
            else
            {
                d->m_pNodeList->deleteNode(newChild->name());
                d->m_pNodeList->insert(nValue + 1, newChild);
            }
        }
    }

    if (newChild != NULL)
    {
        newChild->AddRef();
    }
    return newChild;
}


void CGLDXMLNode::setAsBoolean(bool value)
{
    Q_D(CGLDXMLNode);
    if (value)
    {
        d->m_strValue = "true";
    }
    else
    {
        d->m_strValue = "false";
    }
}

void CGLDXMLNode::setAsFloat(double value)
{
    Q_D(CGLDXMLNode);
    d->m_strValue = floatToStr(value);
}

void CGLDXMLNode::setAsInt64(gint64 value)
{
    Q_D(CGLDXMLNode);
    d->m_strValue = int64ToStr(value);
}

void CGLDXMLNode::setAsInteger(int value)
{
    Q_D(CGLDXMLNode);
    d->m_strValue = intToStr(value);
}

void CGLDXMLNode::setAsString(const GString &value)
{
    Q_D(CGLDXMLNode);
    d->m_strValue = encodeXMLString(value, contains(d->m_enOptions, XO_ENCODE_CRLF));
}

void CGLDXMLNode::setName(const GString &value)
{
    Q_D(CGLDXMLNode);
    d->m_strName = value;
}

void CGLDXMLNode::setNodeType(const EnXMLNodeType value)
{
    Q_D(CGLDXMLNode);
    if (value != d->m_enNodeType)
    {
        d->m_enNodeType = value;
        if (value != ELEMENT)
        {
            d->m_strName = c_gdefNodeName[d->m_enNodeType];
        }
    }
}

void CGLDXMLNode::setParent(IGLDXMLNode *value)
{
    Q_D(CGLDXMLNode);
    if (d->m_pParent != value)
    {
        d->m_pParent = dynamic_cast<CGLDXMLNode*>(value);
    }
}

void CGLDXMLNode::setXML(const GString &value)
{
    Q_D(CGLDXMLNode);
    int nLength = 0;
    int nStart = 0;
    int nEnd = value.length() - 1;

    nStart = getFirstNonCharIdx(value, ' ', nStart);
    if (value.at(nStart) == '<')
    {
        ++nStart;
    }
    nEnd = getFirstNonCharIdx(value, ' ', nEnd, true);

    if (value.at(nEnd) == '>')
    {
        --nEnd;
    }

    if (value.at(nEnd) == '/')
    {
        --nEnd;
    }

    nStart = getFirstNonCharIdx(value, ' ', nStart);
    nEnd = getFirstNonCharIdx(value, ' ', nEnd, true);

    int nSpacePos = 0;
    nSpacePos = getFirstCharIdx(value, ' ', nStart);

    if ((nSpacePos > nStart) && (nSpacePos < nEnd))
    {
        nLength = nSpacePos - nStart;
        d->m_strName = value.mid(nStart, nLength);
        d->m_pAttributes->setXml(value.mid(nSpacePos + 1, nEnd - nSpacePos));
    }
    else
    {
        nLength = (nEnd - nStart) + 1;
        d->m_strName = value.mid(nStart, nLength);
        d->m_pAttributes->clear();
    }
}

void CGLDXMLNode::loadFromStream(GStream *pStream)
{
    Q_D(CGLDXMLNode);
    assert(pStream != NULL);

    d->m_pNodeList->clear();
    d->m_pAttributes->clear();

    QString strBuffer = pStream->readAll();
    GXMLParser::execute(strBuffer, this, d->m_enOptions);
}

void CGLDXMLNode::saveToStream(GStream *pStream)
{
    GXMLBuilder::saveNode(this, pStream);
}

void CGLDXMLNode::setAttribute(const GString &name, const GString &value)
{
    Q_D(CGLDXMLNode);
    CGLDXMLNodeAttribute *pAttr = d->m_pAttributes->itemsByNameObj(name);
    pAttr->setAsString(value);
}

void CGLDXMLNode::setAttribute(const GString &name, int value)
{
    Q_D(CGLDXMLNode);
    CGLDXMLNodeAttribute *pAttr = d->m_pAttributes->itemsByNameObj(name);
    pAttr->setAsInteger(value);
}

GString CGLDXMLNode::attribute(const GString &name, const GString &defValue)
{
    Q_D(CGLDXMLNode);
    if (hasAttribute(name))
    {
        CGLDXMLNodeAttribute* oAttribute =  d->m_pAttributes->itemsByNameObj(name);
        return oAttribute->asString();
    }
    else
    {
        return defValue;
    }
}

IGLDXMLDocument *CGLDXMLNode::ownerDocument() const
{
    Q_D(const CGLDXMLNode);
    if (d->m_pOwnerDocument != NULL)
    {
        d->m_pOwnerDocument->AddRef();
    }
    return d->m_pOwnerDocument;
}

void CGLDXMLNode::setAsVariant(const GVariant value)
{
    Q_D(CGLDXMLNode);
    //TODO
    //d->m_strValue = GXMLUtils::base64Encode(value.toByteArray());
    d->m_strValue = value.toString();
}

void CGLDXMLNode::setText(const GString &value)
{
    Q_D(CGLDXMLNode);
    d->m_strValue = value;
}

IGLDXMLNode *CGLDXMLNode::toElement() const
{
    CGLDXMLNode *pNode = toElementObj();
    if (pNode != NULL)
    {
        pNode->AddRef();
    }
    return pNode;
}

bool CGLDXMLNode::isElement() const
{
    Q_D(const CGLDXMLNode);
    if (d->m_enNodeType == ELEMENT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CGLDXMLNode::isNull() const
{
    //DO Nothing
    return false;
}

bool CGLDXMLNode::hasChildNodes() const
{
    Q_D(const CGLDXMLNode);
    return d->m_pNodeList->count() > 0;
}

IGLDXMLNode* CGLDXMLNode::parentNode() const
{
    return parent();
}

void CGLDXMLNode::setNodeValue(const GString &value)
{
    setAsString(value);
}

IGLDXMLNode* CGLDXMLNode::firstChild() const
{
    Q_D(const CGLDXMLNode);
    if (hasChildNodes())
    {
        return d->m_pNodeList->node(0);
    }
    return NULL;
}

IGLDXMLNode *CGLDXMLNode::lastChild() const
{
    Q_D(const CGLDXMLNode);
    if (hasChildNodes())
    {
        return d->m_pNodeList->node(d->m_pNodeList->count() - 1);
    }
    return NULL;
}

void CGLDXMLNode::removeAttribute(const GString &name)
{
    Q_D(CGLDXMLNode);
    int nIndex = d->m_pAttributes->indexOf(name);
    if (nIndex > -1)
    {
        d->m_pAttributes->remove(nIndex);
    }
}

IGLDXMLNodeAttribute *CGLDXMLNode::setAttributeNode(IGLDXMLNodeAttribute *newAttr)
{
    Q_D(CGLDXMLNode);
    GString attrName = newAttr->name();
    IGLDXMLNodeAttribute *pAttr = NULL;
    if (hasAttribute(attrName))
    {
        pAttr = d->m_pAttributes->itemsByNameObj(attrName);
        pAttr->setAsString(newAttr->asString());
    }
    else
    {
        pAttr = d->m_pAttributes->add(newAttr->name());
        pAttr->setAsString(newAttr->asString());
    }

    if (pAttr != NULL)
    {
        pAttr->AddRef();
    }
    return pAttr;
}

void CGLDXMLNode::clear()
{
    //Do Nothing
}

//setXML中调用
int CGLDXMLNode::getFirstNonCharIdx(const GString str, const GChar ch, int nOffSet, bool isReverse)
{
    int nLength = str.length();

    assert(nOffSet >= 0);
    assert(nOffSet < nLength);
    if (!isReverse)
    {
        for (int i = nOffSet; i < nLength; ++i)
        {
            if (str.at(i) != ch)
                return i;
        }
    }
    else
    {
        for (int i = nOffSet; i >= 0; --i)
        {
            if (str.at(i) != ch)
                return i;
        }
    }
    return nOffSet;
}

int CGLDXMLNode::getFirstCharIdx(const GString str, const GChar ch, int nOffSet, bool isReverse)
{
    int nLength = str.length();

    assert(nOffSet >= 0);
    assert(nOffSet < nLength);
    if (!isReverse)
    {
        for (int i = nOffSet; i < nLength; ++i)
        {
            if (str.at(i) == ch)
                return i;
        }
    }
    else
    {
        for (int i = nOffSet; i >= 0; --i)
        {
            if (str.at(i) == ch)
                return i;
        }
    }
    return nOffSet;
}

void CGLDXMLNode::elementsByTagName(CGLDXMLNodeList *list, CGLDXMLNode *parent, const GString &value)
{
    list->add(parent);
    if (parent->hasChildNodes())
    {
        for (int i = 0; i < parent->childNodesObj()->count(); ++i)
        {
            if (parent->childNodesObj()->nodeObj(i)->tagName() == value)
            {
                elementsByTagName(list, parent->childNodesObj()->nodeObj(i), value);
            }
        }
    }
}

CGLDXMLNode *CGLDXMLNode::nextSiblingObj()
{
    Q_D(CGLDXMLNode);
    int nIndex = 0;
    if (d->m_pParent == NULL)
        return NULL;
    else
    {
        nIndex = d->m_pParent->childNodesObj()->indexOf(this);
        if (nIndex < d->m_pParent->childNodesObj()->count() - 1)
        {
            return d->m_pParent->childNodesObj()->nodeObj(nIndex + 1);
        }
        else
        {
            return NULL;
        }
    }
    return NULL;
}

CGLDXMLNode *CGLDXMLNode::prevSiblingObj()
{
    Q_D(CGLDXMLNode);
    int nIndex = 0;
    if (d->m_pParent == NULL)
    {
        return NULL;
    }
    else
    {
        nIndex = d->m_pParent->childNodesObj()->indexOf(this);
        if (nIndex > 0)
        {
            return d->m_pParent->childNodesObj()->nodeObj(nIndex - 1);
        }
        else
        {
            return NULL;
        }
    }
    return NULL;
}

CGLDXMLNodeAttributes *CGLDXMLNode::attributesObj()
{
    Q_D(CGLDXMLNode);
    return d->m_pAttributes;
}

CGLDXMLNode *CGLDXMLNode::cloneNodeObj(bool deep)
{
    Q_D(CGLDXMLNode);
    CGLDXMLNode *pNode = new CGLDXMLNode(NULL, d->m_enOptions, d->m_enNodeType, d->m_pOwnerDocument);
    pNode->setName(d->m_strName);
    pNode->setText(d->m_strValue);
    pNode->attributesObj()->setXml(d->m_pAttributes->xml());
    if (!deep)
    {
        return pNode;
    }
    for (int i = 0; i < d->m_pNodeList->count(); ++i)
    {
        CGLDXMLNode *pSrcNode = d->m_pNodeList->nodeObj(i);
        CGLDXMLNode *pDesNode = pSrcNode->cloneNodeObj(true);
        pNode->childNodesObj()->add(pDesNode);
    }
    return pNode;
}

CGLDXMLNode *CGLDXMLNode::toElementObj() const
{
    if (isElement())
    {
        return const_cast<CGLDXMLNode *>(this);
    }
    else
    {
        return NULL;
    }
}

CGLDXMLNode *CGLDXMLNode::parentObj() const
{
    Q_D(const CGLDXMLNode);
    return d->m_pParent;
}

CGLDXMLNodeList *CGLDXMLNode::childNodesObj() const
{
    Q_D(const CGLDXMLNode);
    return d->m_pNodeList;
}

QString CGLDXMLNode::asJSON()
{
    //TODO
    return "";
}

void CGLDXMLNode::setAsJSON(const GString &strValue)
{
    //TODO
    G_UNUSED(strValue);
}

GString CGLDXMLNode::nodeName()
{
    Q_D(CGLDXMLNode);
   return d->m_strName;
}

IGLDXMLNode* CGLDXMLNode::nextSiblingElement(const GString &tagName)
{
    for (CGLDXMLNode *pSib = nextSiblingObj(); pSib != NULL; pSib = pSib->nextSiblingObj()) {
        if (pSib->isElement()) {
            CGLDXMLNode *pElt = pSib->toElementObj();
            if (tagName.isEmpty() || sameStr(pElt->tagName(), tagName))
            {
                pElt->AddRef();
                return pElt;
            }
        }
    }
    return NULL;
}

IGLDXMLNode* CGLDXMLNode::appendChild(IGLDXMLNode *node)
{
    Q_D(CGLDXMLNode);
    d->m_pNodeList->add(node);
    if (node != NULL)
    {
        node->AddRef();
    }
    return node;
}

IGLDXMLNode* CGLDXMLNode::removeChild(IGLDXMLNode *node)
{
    Q_D(CGLDXMLNode);
    for (int i = 0; i < d->m_pNodeList->count(); i++)
    {
        IGLDXMLNode *pNode = d->m_pNodeList->at(i);
        if (pNode == node)
        {
            d->m_pNodeList->deleteNode(i);
            break;
        }
        pNode->Release();
    }
    return node;
}

void CGLDXMLNode::setTagName(const GString &name)
{
    Q_D(CGLDXMLNode);
    d->m_strName = name;
}

GString CGLDXMLNode::tagName()
{
    Q_D(CGLDXMLNode);
    return d->m_strName;
}

IGLDXMLNodeList* CGLDXMLNode::elementsByTagName(const GString &value)
{
    Q_D(CGLDXMLNode);
    CGLDXMLNodeList *pList = new CGLDXMLNodeList();
    for (int i = 0; i < d->m_pNodeList->count(); ++i)
    {
        if (d->m_pNodeList->nodeObj(i)->tagName() == value)
        {
            elementsByTagName(pList, d->m_pNodeList->nodeObj(i), value);
        }
    }

    if (pList != NULL)
    {
        pList->AddRef();
    }
    return pList;
}

IGLDXMLNode* CGLDXMLNode::toText()
{
    AddRef();
    return this;
}

GString CGLDXMLNode::data()
{
    return nodeValue();
}

IGLDXMLNode *CGLDXMLNode::namedItem(const GString &name) const
{
    Q_D(const CGLDXMLNode);
    for (int i = 0; i < d->m_pNodeList->count(); ++i)
    {
        if (d->m_pNodeList->nodeObj(i)->name() == name)
        {
            return d->m_pNodeList->at(i);
        }
    }
    return NULL;
}
/**********************/

double xmlStringToFloat(const GString &str)
{
    int nPos = 0;
    GString sBuffer = "";
    double result = 0.0;

    if (str.isEmpty())
    {
        return 0.0;
    }
    // TODO
    else if (g_CurrentDecimalSeparator != c_gdefXMLDecimalSeparator)
    {
        sBuffer = str;
        nPos = posXMLString(c_gdefXMLDecimalSeparator, sBuffer);

        if (0 != nPos)
        {
            result = sBuffer.toDouble();
        }
    }
    else
    {
        result = str.toDouble();
    }
    return result;
}

int hashOf(const GString &value)
{
    int nResult = 0;
    for (int nTemp = 0; nTemp < value.length(); ++nTemp)
    {
        GChar aChar;
        aChar = value.at(nTemp);
        if (aChar >= 'A' && aChar <= 'Z')
        {
            aChar.toLower();
        }
        nResult = nResult<<2 | nResult>>(sizeof(nResult) * 8 - 2) ^ int (aChar.toLatin1());
    }
    return nResult;
}

class CGLDXMLNodeAttributePrivate
{
public:
  CGLDXMLNodeAttributePrivate(CGLDXMLNodeAttribute *parent)
    : q_ptr(parent)
  {
  }

private:
  CGLDXMLNodeAttribute * const q_ptr;
  Q_DECLARE_PUBLIC(CGLDXMLNodeAttribute);

  GString m_strName;
  GString m_strValue;
  int m_hashData;
  EnTXMLOptions m_options;
};

CGLDXMLNodeAttribute::CGLDXMLNodeAttribute(const GString &name, EnTXMLOptions options)
    : d_ptr(new CGLDXMLNodeAttributePrivate(this))
{
    Q_D(CGLDXMLNodeAttribute);
    d->m_strName = name;
    d->m_strValue = "";
    d->m_options = options;
    d->m_hashData = hashOf(name);
}

CGLDXMLNodeAttribute::~CGLDXMLNodeAttribute()
{
    Q_D(CGLDXMLNodeAttribute);
    freePtr(d);
}

void CGLDXMLNodeAttribute::setXml(const GString &strXML)
{
    Q_D(CGLDXMLNodeAttribute);
    int nPos = posExcludeQuotationMark('=', strXML);

    if (-1 == nPos)
    {
        d->m_strName = strXML;
        d->m_strValue = "";
    }
    else
    {
        d->m_strName = strXML.mid(0, nPos - 1);
        d->m_strName = d->m_strName.trimmed();
        d->m_strValue = copyTrimSpaceAndQuotaionMark(strXML, nPos + 1);
    }
    d->m_hashData = hashOf(d->m_strName);
}

void CGLDXMLNodeAttribute::setNodeValue(const GString &value)
{
    setAsString(value);
}

GString CGLDXMLNodeAttribute::copyTrimSpaceAndQuotaionMark(const GString &str, int index)
{
    int nLength = str.length();
    int nIndex = index;
    GString result;

    while ((nIndex <= nLength) && str[nIndex] <= ' ')
    {
        nIndex++;
    }
    if ((nIndex <= nLength) && (str[nIndex] == '"'))
    {
        nIndex++;
    }
    if (nIndex > nLength)
    {
        result = "";
    }
    else
    {
        while (str[nLength - 1] <= ' ')
        {
            nLength--;
        }
        if ((nLength > 0) && (str[nLength - 1] == '"'))
        {
            nLength--;
        }
        result = str.mid(nIndex, nLength - nIndex + 1);
    }
    result = result.trimmed();

    return result;
}

void CGLDXMLNodeAttribute::setText(const GString &strText)
{
    Q_D(CGLDXMLNodeAttribute);
    d->m_strValue = strText;
}

void CGLDXMLNodeAttribute::setName(const GString &strName)
{
    Q_D(CGLDXMLNodeAttribute);
    d->m_strName = strName.trimmed();
    d->m_hashData = hashOf(strName);
}

void CGLDXMLNodeAttribute::setAsVariant(const GVariant value)
{
    Q_D(CGLDXMLNodeAttribute);
    d->m_strValue = base64Encode(value.toByteArray());
}

void CGLDXMLNodeAttribute::setAsString(const GString &value)
{
    Q_D(CGLDXMLNodeAttribute);
    d->m_strValue = encodeXMLString(value, contains(d->m_options, XO_ENCODE_CRLF));
}

void CGLDXMLNodeAttribute::setAsInteger(int value)
{
    Q_D(CGLDXMLNodeAttribute);
    d->m_strValue = intToStr(value);
}

void CGLDXMLNodeAttribute::setAsInt64(gint64 value)
{
    Q_D(CGLDXMLNodeAttribute);
    d->m_strValue = int64ToStr(value);
}

void CGLDXMLNodeAttribute::setAsFloat(double value)
{
    Q_D(CGLDXMLNodeAttribute);
    d->m_strValue = floatToStr(value);
}

void CGLDXMLNodeAttribute::setAsBoolean(bool value)
{
    Q_D(CGLDXMLNodeAttribute);
    d->m_strValue = boolToXMLString(value);
}

GString CGLDXMLNodeAttribute::xml()
{
    Q_D(const CGLDXMLNodeAttribute);
    return GString("%1=\"%2\"").arg(d->m_strName).arg(d->m_strValue);
}

GString CGLDXMLNodeAttribute::nodeName() const
{
    Q_D(const CGLDXMLNodeAttribute);
    return d->m_strName;
}

CGLDXMLNodeAttribute* CGLDXMLNodeAttribute::toAttr()
{
    AddRef();
    return this;
}

GString CGLDXMLNodeAttribute::value()
{
    return asString();
}

GString CGLDXMLNodeAttribute::nodeValue()
{
    return asString();
}

GString CGLDXMLNodeAttribute::text()
{
    Q_D(CGLDXMLNodeAttribute);
    return d->m_strValue;
}

GString CGLDXMLNodeAttribute::name()
{
    Q_D(CGLDXMLNodeAttribute);
    return d->m_strName;
}

int CGLDXMLNodeAttribute::hashData()
{
    Q_D(CGLDXMLNodeAttribute);
    return d->m_hashData;
}

GVariant CGLDXMLNodeAttribute::asVariant()
{
    Q_D(CGLDXMLNodeAttribute);
    return base64Decode(d->m_strValue.toLocal8Bit());
}

GString CGLDXMLNodeAttribute::asString()
{
    Q_D(CGLDXMLNodeAttribute);
    return decodeXMLString(d->m_strValue, contains(d->m_options, XO_DECODE_CRLF));
}

int CGLDXMLNodeAttribute::asInteger()
{
    Q_D(CGLDXMLNodeAttribute);
    return strToInt(d->m_strValue);
}

gint64 CGLDXMLNodeAttribute::asInt64()
{
    Q_D(CGLDXMLNodeAttribute);
    return strToInt64(d->m_strValue);
}

double CGLDXMLNodeAttribute::asFloat()
{
    Q_D(CGLDXMLNodeAttribute);
    return strToFloat(d->m_strValue);
}

bool CGLDXMLNodeAttribute::asBoolean()
{
    Q_D(CGLDXMLNodeAttribute);
    return strToBool(d->m_strValue);
}

class CGLDXMLDocumentPrivate
{
public:
  CGLDXMLDocumentPrivate(CGLDXMLDocument *parent)
    : q_ptr(parent), m_pRoot(NULL), m_bAutoIndent(true), m_bIncludeHeader(true),
      m_strEncoding("utf-8"), m_strVersion("1.0"), m_enOptionsSet(0)
  {
  }

private:
  CGLDXMLDocument * const q_ptr;
  Q_DECLARE_PUBLIC(CGLDXMLDocument);

  CGLDXMLNode *m_pRoot;
  bool m_bAutoIndent;
  bool m_bIncludeHeader;
  GString m_strEncoding;
  GString m_strFileName;
  GString m_strVersion;
  EnTXMLOptions m_enOptionsSet;
};

CGLDXMLDocument::CGLDXMLDocument() : d_ptr(new CGLDXMLDocumentPrivate(this))
{
}

CGLDXMLDocument::~CGLDXMLDocument()
{
    Q_D(CGLDXMLDocument);
    freeAndNilIntf(d->m_pRoot);

    freePtr(d);
}

IGLDXMLNode *CGLDXMLDocument::createElement(const GString &name)
{
    CGLDXMLNode *pXMLNode = createNodeObj(name);
    if (pXMLNode)
    {
        pXMLNode->AddRef();
    }
    return pXMLNode;
}

IGLDXMLNode *CGLDXMLDocument::createNode(const GString &name, EnXMLNodeType type )
{
    CGLDXMLNode *pXMLNode = createNodeObj(name, type);
    if (pXMLNode)
    {
        pXMLNode->AddRef();
    }
    return pXMLNode;
}

bool CGLDXMLDocument::isEmpty()
{
    Q_D(CGLDXMLDocument);
    return !d->m_pRoot;
}

void CGLDXMLDocument::loadFromFile(const GString &fileName)
{
    Q_D(CGLDXMLDocument);
    GFileStream oStream(fileName);
    if (oStream.open(QIODevice::ReadOnly))
    {
        try
        {
            oStream.seek(0);
            loadFromStream(&oStream);
            d->m_strFileName = fileName;
        }
        catch(...)
        {
            oStream.close();
            throw;
        }
        oStream.close();
    }
}

void CGLDXMLDocument::saveToFile(const GString &fileName)
{
    GFileStream oStream(fileName);
    if (oStream.open(QIODevice::WriteOnly))
    {
        try
        {
            saveToStream(&oStream);
        }
        catch (...)
        {
            oStream.close();
            throw;
        }
        oStream.close();
    }
}

void CGLDXMLDocument::loadFromStream(GStream *stream)
{
    loadFromStream(stream, false);
}

void CGLDXMLDocument::loadFromStream(GStream *stream, bool useUtf8Optimize)
{
    assert(stream != NULL);
    assert(stream->isOpen());
    // TODO
    GString strData;
    GString strEncoding;
    bool bBOM = false;

    Q_D(CGLDXMLDocument);
    int nSize = stream->size() - stream->pos();
    GByteArray strBuffer;
    strBuffer.resize(nSize);
    stream->read(strBuffer.data(), nSize);

    bool bEncode = findEncoding(strBuffer, bBOM, strEncoding);
    if (bEncode && (!strEncoding.isEmpty()))
    {
        d->m_strEncoding = strEncoding;
    }
//    else if (!bEncode && (strEncoding.isEmpty()))
//    {
//        d->m_strEncoding = "";
//    }
    if (sameText(d->m_strEncoding, c_uft8Encoding))
    {
        if (useUtf8Optimize)
        {
            strBuffer = "";
            stream->seek(0);
            strData = utf8StreamDecode(stream);
        }
        else
        {
            strData = utf8ToUnicode(strBuffer.data());
        }
    }
    else
    {
        strData = asciiToUnicode(strBuffer);
    }
    strBuffer = "";
    parse(strData, 0);
//    if (d->m_strEncoding.length() == 0)
//    {
//        d->m_strEncoding = "utf-8";
//    }
}

void CGLDXMLDocument::saveToStream(GStream *stream)
{
    GXMLBuilder::saveDoc(this, stream);
}

GString CGLDXMLDocument::getXMLHeader()
{
    GString strVersion;

    Q_D(CGLDXMLDocument);
    if (d->m_strVersion.isEmpty())
    {
        strVersion = c_gconAttribFmt.arg(c_gconVersion, c_gdefVersion);
    }
    else
    {
        strVersion = c_gconAttribFmt.arg(c_gconVersion, d->m_strVersion);
    }

    GString strEncoding;
    if (!d->m_strEncoding.isEmpty())
    {
        strEncoding = c_gconAttribFmt.arg(c_gconEncoding, d->m_strEncoding);
    }
    else
    {
        strEncoding = "";
    }

    if (strEncoding.isEmpty())
        return c_gconSpecificationFmt.arg(strVersion);
    else
        return c_gconSpecificationFmtEx.arg(strVersion, strEncoding);
}

IGLDXMLNode *CGLDXMLDocument::appendChild(IGLDXMLNode *newChild)
{
    //TODO
    Q_D(CGLDXMLDocument);
    if (d->m_pRoot)
    {
        xmlDocError(c_grsErrInvalidRootNode);
    }
    else
    {
        d->m_pRoot = dynamic_cast<CGLDXMLNode *>(newChild);
        if (d->m_pRoot)
        {
            d->m_pRoot->AddRef();
            d->m_pRoot->AddRef();
        }
        return d->m_pRoot;
    }
    return NULL;
}

IGLDXMLNode *CGLDXMLDocument::firstChild() const
{
    Q_D(const CGLDXMLDocument);
    d->m_pRoot->AddRef();
    return d->m_pRoot;
}

void CGLDXMLDocument::save(GStream *stream, int indent) const
{
    G_UNUSED(indent);
    GXMLBuilder::saveDoc(const_cast<CGLDXMLDocument *>(this), stream);
}

bool CGLDXMLDocument::setContent(GStream *dev, GString *errorMsg, int *errorLine, int *errorColumn)
{
    G_UNUSED(errorMsg);
    G_UNUSED(errorLine);
    G_UNUSED(errorColumn);
    try
    {
        loadFromStream(dev);
    }
    catch (...)
    {
        throw;
    }
    return true;
}

bool CGLDXMLDocument::setContent(const GString &text, bool namespaceProcessing, GString *errorMsg, int *errorLine, int *errorColumn)
{
    G_UNUSED(namespaceProcessing);
    return setContent(text.toUtf8(), errorMsg, errorLine, errorColumn);
}

bool CGLDXMLDocument::setContent(const QString &text, QString *errorMsg, int *errorLine, int *errorColumn)
{
    return setContent(text, false, errorMsg, errorLine, errorColumn);
}

bool CGLDXMLDocument::setContent(const QByteArray &text, QString *errorMsg, int *errorLine, int *errorColumn)
{
    G_UNUSED(errorMsg);
    G_UNUSED(errorLine);
    G_UNUSED(errorColumn);
    GBlockMemoryStream pMemoryStream;
    pMemoryStream.write(text.data(), text.length());
    pMemoryStream.seek(0);
    loadFromStream(&pMemoryStream);
    return true;
}

IGLDXMLNodeAttribute* CGLDXMLDocument::createAttribute(const QString &name)
{
    CGLDXMLNodeAttribute *pNodeAttributr = new CGLDXMLNodeAttribute(name, ELEMENT);
    if (pNodeAttributr != NULL)
    {
        pNodeAttributr->AddRef();
    }
    return pNodeAttributr;
}

IGLDXMLNode *CGLDXMLDocument::createTextNode(const GString &data)
{
    Q_D(CGLDXMLDocument);
    CGLDXMLNode *pNode = new CGLDXMLNode(NULL, d->m_enOptionsSet, TEXT, this);
    pNode->setName("#text");
    pNode->setAsString(data);

    if (pNode != NULL)
    {
        pNode->AddRef();
    }
    return pNode;
}

IGLDXMLNodeList *CGLDXMLDocument::childNodes() const
{
    Q_D(const CGLDXMLDocument);
    return d->m_pRoot->childNodes();
}

void CGLDXMLDocument::clear()
{
    //Do Nothing
}

bool CGLDXMLDocument::autoIndent()
{
    Q_D(CGLDXMLDocument);
    return d->m_bAutoIndent;
}

void CGLDXMLDocument::setAutoIndent(bool value)
{
    Q_D(CGLDXMLDocument);
    d->m_bAutoIndent = value;
}

GString CGLDXMLDocument::displayText()
{
    return GXMLBuilder::saveDocText(this, false);
}

IGLDXMLNode *CGLDXMLDocument::documentElement() const
{
    Q_D(const CGLDXMLDocument);
    if (d->m_pRoot != NULL)
    {
        d->m_pRoot->AddRef();
    }
    return d->m_pRoot;
}

GString CGLDXMLDocument::encoding()
{
    Q_D(CGLDXMLDocument);
    return d->m_strEncoding;
}

void CGLDXMLDocument::setEncoding(const GString &value)
{
    Q_D(CGLDXMLDocument);
    d->m_strEncoding = value;
}

GString CGLDXMLDocument::fileName()
{
    Q_D(CGLDXMLDocument);
    return d->m_strFileName;
}

void CGLDXMLDocument::setFileName(const GString &value)
{
    Q_D(CGLDXMLDocument);
    d->m_strFileName = value;
}

bool CGLDXMLDocument::includeHeader()
{
    Q_D(CGLDXMLDocument);
    return d->m_bIncludeHeader;
}

void CGLDXMLDocument::setIncludeHeader(bool value)
{
    Q_D(CGLDXMLDocument);
    d->m_bIncludeHeader = value;
}

EnTXMLOptions CGLDXMLDocument::options()
{
    Q_D(CGLDXMLDocument);
    return EnTXMLOptions(d->m_enOptionsSet);
}

void CGLDXMLDocument::setOptions(EnTXMLOptions xmlOptionsSet)
{
    Q_D(CGLDXMLDocument);
    d->m_enOptionsSet = xmlOptionsSet;
}

GString CGLDXMLDocument::version()
{
    Q_D(CGLDXMLDocument);
    return d->m_strVersion;
}

void CGLDXMLDocument::setVersion(const GString &value)
{
    Q_D(CGLDXMLDocument);
    d->m_strVersion = value;
}

GString CGLDXMLDocument::xml()
{
    return GXMLBuilder::saveDocText(this, true);
}

void CGLDXMLDocument::setXML(const GString &value)
{
    parse(value, 0);
}

IGLDXMLNode *CGLDXMLDocument::root()
{
    return documentElement();
}

void CGLDXMLDocument::setRoot(IGLDXMLNode *root)
{
    Q_D(CGLDXMLDocument);
    d->m_pRoot = dynamic_cast<CGLDXMLNode*>(root);
    if (d->m_pRoot != NULL)
    {
        d->m_pRoot->AddRef();
    }
}

IGLDXMLNode* CGLDXMLDocument::toElement() const
{
    return NULL;
}

bool CGLDXMLDocument::isNull() const
{
    Q_D(const CGLDXMLDocument);
    //DO Nothing
    if (d->m_pRoot == NULL)
    {
        return true;
    }
    return false;
}

GString CGLDXMLDocument::toString(int indent) const
{
    GBlockMemoryStream pStream;
    save(&pStream, indent);
    pStream.seek(0);
    QString str = GString::fromUtf8(pStream.readAll());

    return str;
}

IGLDXMLNode *CGLDXMLDocument::elementById(const GString &elementID)
{
    G_UNUSED(elementID);
    //DO Nothing
    return NULL;
}

IGLDXMLNode *CGLDXMLDocument::firstChildElement(const GString &tagName) const
{
    Q_D(const CGLDXMLDocument);
    if (tagName.isEmpty() || sameText(d->m_pRoot->tagName(), tagName))
    {
        d->m_pRoot->AddRef();
        return d->m_pRoot;
    }
    else
    {
        return NULL;
    }
}

GString CGLDXMLDocument::asJSON()
{
    //TODO
    return NULL;
}

void CGLDXMLDocument::setAsJSON(const GString &value)
{
    //TODO
    G_UNUSED(value);
}

void CGLDXMLDocument::loadFromJSONFile(const GString &fileName)
{
    //TODO
    G_UNUSED(fileName);
}

void CGLDXMLDocument::loadFromJSONStream(GStream *stream)
{
    //TODO
    G_UNUSED(stream);
}

void CGLDXMLDocument::saveAsJSONToFile(const GString &fileName)
{
    //TODO
    G_UNUSED(fileName);
}

void CGLDXMLDocument::saveAsJSONToStream(GStream *stream)
{
    //TODO
    G_UNUSED(stream);
}

CGLDXMLNode *CGLDXMLDocument::createNodeObj(const GString &name, EnXMLNodeType type)
{
    Q_D(CGLDXMLDocument);
    CGLDXMLNode *pNode = new CGLDXMLNode(NULL, d->m_enOptionsSet, type, this);
    pNode->setName(name);
    return pNode;
}

void CGLDXMLDocument::setRootObj(CGLDXMLNode *root)
{
    Q_D(CGLDXMLDocument);
    d->m_pRoot = root;
}

CGLDXMLNode *CGLDXMLDocument::rootObj()
{
    Q_D(CGLDXMLDocument);
    return d->m_pRoot;
}

bool CGLDXMLDocument::findEncoding(const char *data, bool &bom, GString &enCoding)
{
    //TODO
    bom = false;
    GString strData = GString::fromLatin1(data);

    if (strData.isEmpty())
    {
        return false;
    }

    int nIndex = 0;
    int nLength = strData.length();
    QString str = QString::fromLatin1(c_gdefXMLUtf8Header);

    if (strData.startsWith(str))
    {
        enCoding = c_uft8Encoding;
        bom = true;
        return true;
    }

    while (nIndex < nLength && ((strData.at(nIndex) == '\n')
                                || (strData.at(nIndex) == '\t')
                                || (strData.at(nIndex) == '\r')
                                || (strData.at(nIndex) == ' ')))
    {
        ++nIndex;
    }

    if (nIndex >= nLength)
    {
        return false;
    }

    if (strData.indexOf(c_gdefXMLDataHeader, nIndex) != nIndex)
    {
        return false;
    }

    int nStart = strData.indexOf(c_gdefEncodingPrex, nIndex);

    if (-1 == nStart)
    {
        return true;
    }
    else
    {
        nStart = nStart + c_gdefEncodingPrex.length();
    }

    int nEnd = strData.indexOf("\"", nStart + 1); // to test enconding=""测试空还是未找到

    if (-1 == nEnd)
    {
        return true;
    }

    enCoding = strData.mid(nStart, nEnd - nStart);
    return true;
}

void CGLDXMLDocument::parse(const GString &data, int pos)
{
    Q_D(CGLDXMLDocument);
    d->m_pRoot = NULL;
    d->m_strVersion = c_gdefVersion;
    GXMLParser::execute(data, this, pos);
}

class CGLDXMLNodeAttributesPrivate
{
public:
  CGLDXMLNodeAttributesPrivate(CGLDXMLNodeAttributes *parent)
    : q_ptr(parent)
  {
  }

private:
  CGLDXMLNodeAttributes * const q_ptr;
  Q_DECLARE_PUBLIC(CGLDXMLNodeAttributes);

  EnTXMLOptions m_options;
  GInterfaceObjectList<CGLDXMLNodeAttribute*> m_list;
};

CGLDXMLNodeAttributes::CGLDXMLNodeAttributes(EnTXMLOptions enOptions)
    : d_ptr(new CGLDXMLNodeAttributesPrivate(this))
{
    Q_D(CGLDXMLNodeAttributes);
    d->m_options = enOptions;
}

CGLDXMLNodeAttributes::~CGLDXMLNodeAttributes()
{
}

/*新增属性*/
IGLDXMLNodeAttribute *CGLDXMLNodeAttributes::add(const GString &strName)
{
    CGLDXMLNodeAttribute *pAttribute = addObj(strName);
    if (pAttribute != NULL)
    {
        pAttribute->AddRef();
    }
    return pAttribute;
}

/*通过属性名查找相应的索引*/
int CGLDXMLNodeAttributes::indexOf(const GString &strName)
{
    Q_D(CGLDXMLNodeAttributes);
    CGLDXMLNodeAttribute *pXmlNodeAttribute = NULL;
    int nResult = -1;
    if (!d->m_list.isEmpty())
    {
        for (nResult = 0; nResult < d->m_list.length(); ++nResult)
        {
            pXmlNodeAttribute = itemObj(nResult);
            if (strName == pXmlNodeAttribute->name())
            {
                return nResult;
            }
        }
        return -1;
    }
    else
    {
        return -1;
    }
}

/*属性的个数*/
int CGLDXMLNodeAttributes::count()
{
    Q_D(CGLDXMLNodeAttributes);
    return d->m_list.count();
}

/*清空属性个数*/
void CGLDXMLNodeAttributes::clear()
{
    Q_D(CGLDXMLNodeAttributes);
    d->m_list.clear();
}

/*删除某个属性*/
void CGLDXMLNodeAttributes::remove(int index)
{
    Q_D(CGLDXMLNodeAttributes);
    if (index >=0 && index < d->m_list.count())
    {
        d->m_list.removeAt(index);
    }
}

/*取得某个属性*/
IGLDXMLNodeAttribute *CGLDXMLNodeAttributes::item(int index)
{
    CGLDXMLNodeAttribute *pAttribute = itemObj(index);
    if (pAttribute != NULL)
    {
        pAttribute->AddRef();
    }
    return pAttribute;
}

/*通过属性名取得相应属性*/
IGLDXMLNodeAttribute *CGLDXMLNodeAttributes::itemsByName(const GString &strName)
{
    CGLDXMLNodeAttribute *pXmlNodeAttribute = itemsByNameObj(strName);
    if (pXmlNodeAttribute != NULL)
    {
        pXmlNodeAttribute->AddRef();
    }
    return pXmlNodeAttribute;
}

bool CGLDXMLNodeAttributes::contains(const GString &name)
{
    return indexOf(name) != -1;
}

IGLDXMLNodeAttribute *CGLDXMLNodeAttributes::namedItem(const GString &name)
{
    return itemsByName(name);
}

/*读取属性值*/
GString CGLDXMLNodeAttributes::xml()
{
    Q_D(CGLDXMLNodeAttributes);
    GString result;
    for (int i = 0; i < d->m_list.length(); ++i)
    {
        result.append(d->m_list.at(i)->xml()).append(' ');
    }
    if (!result.isEmpty())
    {
        result = result.mid(0, result.length() - 1);
    }
    return result;
}

/*设置属性值*/
void CGLDXMLNodeAttributes::setXml(const GString &value)
{
    int nPos = 0;
    int nIndex = 0;
    int nLength = value.length();
    GString sValue = trimSpecialChar(value);
    GString strText = "";
    clear();
    while (nIndex < nLength)
    {
        nPos = posExcludeQuotationMark('=', sValue, nIndex);
        if (nPos != 0)
        {
            GString strName = copyTrimXMLString(sValue, nIndex, nPos - 1);
            nIndex = nPos + 1;
            if(findQuotationStr(sValue, nIndex, strText, nPos))
            {
                doAddAttribute(strName, strText);
                nIndex = nPos + 1;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
}

/*取出末尾的特殊字符*/
GString CGLDXMLNodeAttributes::trimSpecialChar(const GString &value)
{
    GString sValue = value;
    int nLength = sValue.length() - 1;
    while (nLength >= 0 && (sValue[nLength] == '\r' || sValue[nLength] == '\n' || sValue[nLength] == ' '
                            || sValue[nLength] == '\t'))
    {
        --nLength;
    }
    return sValue.mid(0, nLength + 1);
}

int CGLDXMLNodeAttributes::hashOf(const GString &strName)
{
    int nResult = 0;
    GChar cChar ;
    for (int i = 0; i < strName.length(); ++i)
    {
        cChar = strName[i].toLatin1();
        if (cChar >= 'A' && cChar <= 'Z')
        {
            cChar = cChar.toLower() ;
        }
        nResult = (nResult << 2) | (nResult >> (sizeof(nResult) * 8 - 2)) ^ cChar.digitValue();
    }
    return nResult;
}

/*新增属性*/
void CGLDXMLNodeAttributes::doAddAttribute(const GString &strName, const GString &strText)
{
    Q_D(CGLDXMLNodeAttributes);
    CGLDXMLNodeAttribute *pXmlNodeAttribute = NULL;
    if (strName.length() == 0)
    {
        return;
    }
    int nIndex = indexOf(strName);
    if (nIndex != -1)
    {
        pXmlNodeAttribute = d->m_list.at(nIndex);
    }
    else
    {
        pXmlNodeAttribute = new CGLDXMLNodeAttribute(strName, d->m_options);
        d->m_list.append(pXmlNodeAttribute);
    }
    pXmlNodeAttribute->setText(strText);

}

/*处理字符 '与"*/
bool CGLDXMLNodeAttributes::findQuotationStr(const GString &str, int nstartPos, GString &squotedString, int &nendPos)
{
    //检查参数合法性
    if (str.isEmpty())
    {
        return false;
    }
    if (nstartPos < 0)
    {
        nstartPos = 0;
    }
    else if (nstartPos > str.length())
    {
        nstartPos = str.length();
    }

    //找到第一个引号的位置，并确定是单引号还是双引号
    GChar cStartChar = '\0';
    int nStartPos = nstartPos;
    for(int i = nstartPos; i < str.length(); i++)
    {
        if(str[i] == '\"' || str[i] == '\'')
        {
            cStartChar = str[i];
            nStartPos = i;
            break;
        }
    }
    if(cStartChar == '\0')
        return false;
    for(int i = nStartPos + 1; i < str.length(); i++)
    {
        //确定单引号还是双引号之后，另一种引号被忽略
        if(str[i] == cStartChar)
        {
            nendPos = i;
            squotedString = str.mid(nStartPos + 1, nendPos - nStartPos - 1);
            return true;
        }
    }
    return false;
}

CGLDXMLNodeAttribute *CGLDXMLNodeAttributes::itemsByNameObj(const GString &strName)
{
    Q_D(CGLDXMLNodeAttributes);
    CGLDXMLNodeAttribute *pXmlNodeAttribute = NULL;
    int nIndex = indexOf(strName);
    if (nIndex != -1)
    {
        pXmlNodeAttribute = d->m_list.at(nIndex);
    }
    else
    {
        pXmlNodeAttribute = new CGLDXMLNodeAttribute(strName, d->m_options);
        d->m_list.append(pXmlNodeAttribute);
    }
    return pXmlNodeAttribute;
}

CGLDXMLNodeAttribute *CGLDXMLNodeAttributes::addObj(const GString &strName)
{
    Q_D(CGLDXMLNodeAttributes);
    CGLDXMLNodeAttribute *pXmlNodeAttribute = NULL;
    int nIndex = indexOf(strName);  //查找是否已经存在strName的属性;
    if (nIndex != -1)   //判断,有则赋值,没有则new一个;
    {
        pXmlNodeAttribute = d->m_list.at(nIndex);
    }
    else
    {
        pXmlNodeAttribute = new CGLDXMLNodeAttribute(strName, d->m_options);
        d->m_list.append(pXmlNodeAttribute);
    }
    return pXmlNodeAttribute;
}

CGLDXMLNodeAttribute *CGLDXMLNodeAttributes::itemObj(int nIndex)
{
    Q_D(CGLDXMLNodeAttributes);
    if (d->m_list.isEmpty())
    {
        return NULL;
    }
    else
    {
        CGLDXMLNodeAttribute *pAttribute = d->m_list.at(nIndex);
        return pAttribute;
    }
}

class CGLDXMLNodeListPrivate
{
public:
  CGLDXMLNodeListPrivate(CGLDXMLNodeList *parent)
    : q_ptr(parent), m_parent(NULL)
  {
  }

private:
  CGLDXMLNodeList * const q_ptr;
  Q_DECLARE_PUBLIC(CGLDXMLNodeList);

  GInterfaceObjectList<CGLDXMLNode *> m_list;
  CGLDXMLNode *m_parent;
};

CGLDXMLNodeList::CGLDXMLNodeList(IGLDXMLNode *parent) : d_ptr(new CGLDXMLNodeListPrivate(this))
{
    Q_D(CGLDXMLNodeList);
    d->m_parent = dynamic_cast<CGLDXMLNode*>(parent);
}

CGLDXMLNodeList::~CGLDXMLNodeList()
{
    Q_D(CGLDXMLNodeList);
    freePtr(d);
}

int CGLDXMLNodeList::add(IGLDXMLNode *node)
{
    Q_D(CGLDXMLNodeList);
    if (node == NULL)
    {
        return -1;
    }
    else
    {
        d->m_list.append(dynamic_cast<CGLDXMLNode*>(node));
        node->setParent(d->m_parent);
        return d->m_list.count() - 1;
    }
}

int CGLDXMLNodeList::deleteNode(const GString &name)
{
    int nIndex = indexOf(name);
    if (nIndex != -1)
    {
        deleteNode(nIndex);
    }
    return nIndex;
}

void CGLDXMLNodeList::deleteNode(int index)
{
    Q_D(CGLDXMLNodeList);
    d->m_list.removeAt(index);
}

IGLDXMLNode *CGLDXMLNodeList::findNode(const GString &name)
{
    CGLDXMLNode *piNode = findNodeObj(name);
    if (piNode != NULL)
    {
        piNode->AddRef();
    }
    return piNode;
}

IGLDXMLNode *CGLDXMLNodeList::at(int index)
{
    if(count() <= index)
    {
        return NULL;
    }

    CGLDXMLNode *pNode = nodeObj(index);
    if (pNode != NULL)
    {
        pNode->AddRef();
    }
    return pNode;
}

IGLDXMLNode *CGLDXMLNodeList::item(int index)
{
    if(count() <= index)
    {
        return NULL;
    }

    CGLDXMLNode *pNode = nodeObj(index);
    if (pNode != NULL)
    {
        pNode->AddRef();
    }
    return pNode;
}

int CGLDXMLNodeList::indexOf(IGLDXMLNode *node) const
{
    Q_D(const CGLDXMLNodeList);
    return d->m_list.indexOf(dynamic_cast<CGLDXMLNode *>(node));
}

int CGLDXMLNodeList::indexOf(const GString &name) const
{
    Q_D(const CGLDXMLNodeList);
    int result = -1;
    int j = d->m_list.count();
    for (int i = 0; i < j; ++i)
    {
        if (name.isEmpty() || sameText(name, d->m_list.at(i)->name()))
        {
            result = i;
            break;
        }
    }
    return result;
}

int CGLDXMLNodeList::remove(IGLDXMLNode *node)
{
    Q_D(CGLDXMLNodeList);
    CGLDXMLNode *pNode = dynamic_cast<CGLDXMLNode*>(node);
    if (NULL == pNode)
    {
        return -1;
    }
    int result = -1;
    result = d->m_list.indexOf(pNode);
    if (result > -1)
    {
        d->m_list.removeAt(result);
        return result;
    }
    return result;
}

void CGLDXMLNodeList::clear()
{
    Q_D(CGLDXMLNodeList);
    for (int i = d->m_list.count()-1; i >= 0; --i)
    {
        deleteNode(i);
    }
}

void CGLDXMLNodeList::exChange(int cureIndex, int newIndex)
{
    Q_D(CGLDXMLNodeList);
    if ((cureIndex > -1) && (cureIndex < d->m_list.count()))
    {
        if ((newIndex > -1) && (newIndex < d->m_list.count()))
        {
            d->m_list.swap(cureIndex, newIndex);
        }
    }
}

void CGLDXMLNodeList::insert(int index, IGLDXMLNode* node)
{
    Q_D(CGLDXMLNodeList);
    CGLDXMLNode *pNode = dynamic_cast<CGLDXMLNode*>(node);
    if (pNode != NULL)
    {
        d->m_list.insert(index, pNode);
        pNode->setParent(d->m_parent);
    }
}

int CGLDXMLNodeList::count() const
{
    Q_D(const CGLDXMLNodeList);
    return d->m_list.count();
}

IGLDXMLNode *CGLDXMLNodeList::node(int index)
{
    CGLDXMLNode *pNode = nodeObj(index);
    if (pNode != NULL)
    {
        pNode->AddRef();
    }
    return pNode;
}

IGLDXMLNode *CGLDXMLNodeList::nodeByName(const GString &name)
{
    CGLDXMLNode *pNode = findNodeObj(name);
    if (pNode != NULL)
    {
        pNode->AddRef();
    }
    return pNode;
}

CGLDXMLNode *CGLDXMLNodeList::nodeObj(int index)
{
    Q_D(CGLDXMLNodeList);
    if ((index < d->m_list.count()) && (index > -1))
    {
        return d->m_list.at(index);
    }
    return NULL;
}

CGLDXMLNode *CGLDXMLNodeList::findNodeObj(const GString &name) const
{
    Q_D(const CGLDXMLNodeList);
    int nIndex = indexOf(name);
    if (-1 == nIndex)
    {
        return NULL;
    }
    else
    {
        return d->m_list.at(nIndex);
    }
}

void CGLDXMLNodeList::move(int cureIndex, int newIndex)
{
    Q_D(CGLDXMLNodeList);
    if ((cureIndex > -1) && (cureIndex < d->m_list.count()))
    {
        if ((newIndex > -1) && (newIndex < d->m_list.count()))
        {
            d->m_list.move(cureIndex, newIndex);
        }
    }
}
