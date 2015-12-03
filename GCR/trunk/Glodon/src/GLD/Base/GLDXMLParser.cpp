#include "GLDXMLParser.h"
#include "GLDXMLUtils.h"

const int c_gconCDataMinLength = c_gconCDataPosx.length() + c_gconCDataPrex.length();
const GString c_grsErrTagNotMatch = TRANS_STRING("XML 标识符号不匹配");
const GString c_grsErrParserMsgFmt = TRANS_STRING("%1，错误位置 %2");

class GXMLParserPrivate
{
public:
  GXMLParserPrivate(GXMLParser *parent)
    : q_ptr(parent), m_base(NULL), m_pos(0), m_currentNode(NULL)
    , m_index(-1), m_temp(NULL), m_document(NULL)
  {
  }

private:
  GXMLParser * const q_ptr;
  Q_DECLARE_PUBLIC(GXMLParser);

  CGLDXMLNode *m_base;
  int m_pos;
  CGLDXMLNode *m_currentNode;
  GString m_data;
  int m_index;
  GString m_encoding;
  GString m_version;
  EnTXMLOptions m_options;
  CGLDXMLNode *m_temp;
  CGLDXMLDocument *m_document;
};

GXMLParser::GXMLParser(const GString strData, IGLDXMLNode *pBase, EnTXMLOptions vOptions)
    : d_ptr(new GXMLParserPrivate(this))
{
    Q_D(GXMLParser);
    d->m_base = dynamic_cast<CGLDXMLNode*>(pBase);
    d->m_options = vOptions;
    d->m_currentNode = NULL;
    d->m_data = strData;
    d->m_encoding = "utf-8";
    d->m_version = "1.0";
}

GXMLParser::~GXMLParser()
{
    Q_D(GXMLParser);
    d->m_base = NULL;
    freePtr(d);
}

CGLDXMLNode *GXMLParser::addOpenTag(const GString &text)
{
    Q_D(GXMLParser);
    if ((d->m_currentNode == NULL) && (d->m_base != NULL))
    {
        d->m_base->setXML(text);
        d->m_currentNode = d->m_base;
        return d->m_base;
    }
    else
    {
        d->m_temp = new CGLDXMLNode(d->m_currentNode, d->m_options, ELEMENT, d->m_document);
        d->m_temp->setXML(text);
        if (d->m_currentNode == NULL)
        {
            d->m_base = d->m_temp;
        }
        else
        {
            d->m_currentNode->childNodesObj()->add(d->m_temp);
        }
        d->m_currentNode = d->m_temp;
        return d->m_temp;
    }
}

void GXMLParser::addCloseTag()
{
    Q_D(GXMLParser);
    if (d->m_currentNode != NULL)
    {
        d->m_currentNode = d->m_currentNode->parentObj();
    }
    else
    {
        xmlDocErrorInParser(TRANS_STRING("XML标识符不匹配"));
    }
}

void GXMLParser::addDataNode(const GString &data, EnXMLNodeType nodeType)
{
    Q_D(GXMLParser);
    CGLDXMLNode *pNode = NULL;
    if ((d->m_currentNode == NULL) && (d->m_base != NULL))
    {
        xmlDocErrorInParser(TRANS_STRING("节点内容非法"));
    }
    else
    {
        pNode = new CGLDXMLNode(d->m_currentNode, d->m_options, nodeType, d->m_document);
        pNode->setName(c_gdefNodeName[nodeType]);
        pNode->setText(data);
        if (d->m_currentNode == NULL)
        {
            d->m_base = pNode;
        }
        else
        {
            d->m_currentNode->childNodesObj()->add(pNode);
        }
    }
}

void GXMLParser::addProcessingInstruction(const GString &data)
{
    Q_D(GXMLParser);
    GString strText = data;
    CGLDXMLNode *pNode = new CGLDXMLNode(NULL, d->m_options, ELEMENT, NULL);
    strText.replace(1, 1, "");
    strText.replace(strText.length() - 2, 1, "");
    pNode->setXML(strText);
    if (pNode->attributesObj()->indexOf("encoding") != -1)
    {
        d->m_encoding = "utf-8";
    }
    if (pNode->attributesObj()->indexOf("version") != -1)
    {
        d->m_version = "1.0";
    }
    freeAndNil(pNode);
}

void GXMLParser::doParse()
{
    Q_D(GXMLParser);
    int nSize = 0;
    int nFind = 0;
    int nEnd = 0;
    int nCount = 0;
    GString strTag = "";
    GString strValue = "";
    if (d->m_data.isEmpty())
        return;
    else
    {
        nSize = d->m_data.length();
    }
    d->m_index = d->m_pos;

    GString str1 = "<!--";
    GString str2 = "-->";
    int nLenth = str1.length() + str2.length();

    while (d->m_index < nSize)
    {
        if (d->m_data.at(d->m_index) == '<')
        {
            nEnd = d->m_data.indexOf('>', d->m_index + 1);
            if (nEnd == -1)
            {
                xmlDocErrorInParser(TRANS_STRING("XML标识符号不匹配"));
                break;
            }
            nCount = nEnd - d->m_index + 1;
            if (nCount <= 2)
            {
                xmlDocErrorInParser(TRANS_STRING("XML节点内容不能为空"));
                break;
            }

            /*
             *因为类似于下面的标签
             *<o:ExtPropDef Code="ConstraintExpr" Value="(BraceSpanCount >= 0) and (BraceSpanCount &lt;= 6)"/>
             * BraceSpanCount >= 0其实是一个表达式，因为没有转译，所以造成解析错误，为了容错处理，增加判断<XX>（标签）内
             *‘"’的数量，来作为'>'是否是标签关键符号。否子继续找下一个'>'
             *by huy-a.2014-09-23
             */
            while(true)
            {
                int nStrCount = 0;
                GStringRef str = d->m_data.midRef(d->m_index, nEnd - d->m_index + 1);
                nStrCount = str.count(QChar('\"'));
                if (nStrCount % 2 == 0)
                {
                    break;
                }
                else
                {
                    //将>符号替换掉
                    d->m_data[nEnd] = QChar('&');
                    d->m_data.insert(nEnd + 1,"gt;");
                    nEnd = d->m_data.indexOf('>', nEnd + 1);
                    nCount = nEnd - d->m_index + 1;
                }
            }

            if (d->m_data.at(d->m_index + 1) == '/')
            {
                addCloseTag();
            }
            else if (d->m_data.at(d->m_index + 1) == '?')
            {
                strTag = d->m_data.mid(d->m_index, nCount);
                addProcessingInstruction(strTag);
            }
            else if (d->m_data.at(d->m_index + 1) != '!')
            {
                strTag = d->m_data.mid(d->m_index, nCount);
                addOpenTag(strTag);
                if (d->m_data.at(nEnd - 1) == '/')
                {
                    addCloseTag();
                }
            }
            else if ((nCount >= nLenth)
                     && (d->m_data.at(d->m_index + 2) == '-')
                     && (d->m_data.at(d->m_index + 3) == '-'))
            {
                if ((d->m_data.at(nEnd - 2) == '-') && (d->m_data.at(nEnd - 1) == '-'))
                {
                    nFind = nEnd - 2;
                }
                else
                {
                    nFind = posXMLString(str2, d->m_data, d->m_index + 4);
                }
                if (nFind == 0)
                {
                    xmlDocErrorInParser(TRANS_STRING("XML标识符号不匹配"));
                    break;
                }
                nCount = nFind - d->m_index - 4;
                strTag = d->m_data.mid(d->m_index + 4 , nCount);
                if (!contains(d->m_options, XO_IGNORE_COMMENT))
                {
                    addDataNode(strTag, COMMENT);
                }
                nEnd = nFind + 2;
            }
            else if ((nCount >= c_gconCDataMinLength) && (d->m_data.at(d->m_index + 2) == '[')
                     && (d->m_data.at(d->m_index + 3) == 'C') && (d->m_data.at(d->m_index + 4) == 'D')
                     && (d->m_data.at(d->m_index + 5) == 'A') && (d->m_data.at(d->m_index + 6) == 'T')
                     && (d->m_data.at(d->m_index + 7) == 'A') && (d->m_data.at(d->m_index + 2) == '['))
            {
                if ((d->m_data[nEnd - 2] == ']') && (d->m_data[nEnd - 1] == ']'))
                {
                    nFind = nEnd - 2;
                }
                else
                {
                    nFind = posXMLString(c_gconCDataPosx, d->m_data, d->m_index + 9);
                }
                if (nFind == 0)
                {
                    xmlDocErrorInParser(c_grsErrTagNotMatch);
                    break;
                }
                nCount = nFind - d->m_index - 9;
                strTag = d->m_data.mid(d->m_index + 9, nCount);
                if (!contains(d->m_options, XO_IGNORE_CDATA))
                {
                    addDataNode(strTag, CDATA);
                }
                nEnd = nFind + 2;
            }
            d->m_index = nEnd + 1;
        }
        else
        {
            nEnd = d->m_data.indexOf("<", d->m_index);
            if (nEnd == -1)
            {
                nEnd = nSize;
            }
            nCount = nEnd - d->m_index;
            strValue = d->m_data.mid(d->m_index, nCount);
            if (d->m_currentNode != NULL)
            {
                d->m_currentNode->setText(strValue);
            }
            d->m_index = nEnd;
        }
    }
}

void GXMLParser::xmlDocErrorInParser(const GString &msg)
{
    Q_D(GXMLParser);
    xmlDocError(c_grsErrParserMsgFmt.arg(msg, intToStr(d->m_index)));
}

CGLDXMLNode *GXMLParser::base()
{
    Q_D(GXMLParser);
    return d->m_base;
}

const CGLDXMLNode *GXMLParser::current()
{
    Q_D(GXMLParser);
    return d->m_currentNode;
}

GString GXMLParser::encoding()
{
    Q_D(GXMLParser);
    return d->m_encoding;
}

GString GXMLParser::version()
{
    Q_D(GXMLParser);
    return d->m_version;
}

void GXMLParser::setPos(int pos)
{
    Q_D(GXMLParser);
    d->m_pos = pos;
}

void GXMLParser::setDocument(CGLDXMLDocument *document)
{
    Q_D(GXMLParser);
    d->m_document = document;
}

void GXMLParser::execute(const GString &data, CGLDXMLDocument *doc, int nPos)
{
    assert(doc != NULL);

    GXMLParser *pXMLParser = new GXMLParser(data, NULL, doc->options());
    try
    {
        pXMLParser->setDocument(doc);
        pXMLParser->setPos(nPos);
        pXMLParser->doParse();
        doc->setRoot(pXMLParser->base());
        doc->setVersion(pXMLParser->version());
        doc->setEncoding(pXMLParser->encoding());
    }
    catch (...)
    {
        freeAndNil(pXMLParser);
        throw;
    }
    freeAndNil(pXMLParser);
}

void GXMLParser::execute(const GString &data, CGLDXMLNode *pBase, EnTXMLOptions options)
{
    GXMLParser *pXMLParser = new GXMLParser(data, pBase, options);
    try
    {
        pXMLParser->doParse();
    }
    catch (...)
    {
        freeAndNil(pXMLParser);
        throw;
    }
    freeAndNil(pXMLParser);
}
