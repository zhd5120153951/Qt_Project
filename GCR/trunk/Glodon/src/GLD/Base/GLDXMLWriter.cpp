#include "GLDXMLWriter.h"

#include "GLDGlobal.h"
#include "GLDStrUtils.h"
#include "GLDException.h"
#include "GLDStreamUtils.h"
#include "GLDStrings.h"

//  "不支持类型为 %d 的可变数据类型"
const char *g_rsErrUnsupportVariantType	= QT_TRANSLATE_NOOP("GLD", "Does not support the type of variable data type %d");// "不支持类型为 %d 的可变数据类型");
#define GrandXMLWriter_rsErrUnsupportVariantType System::loadResourceString(&_rsErrUnsupportVariantType)

#ifdef WIN32
static const GLatin1String g_LineBreak("\r\n");
#else
static const GLatin1String g_LineBreak("\r");
#endif

const GString c_defUtf8Encoding = "utf-8";

void xmlWriterError(const GString &msg, int errorCode)
{
    throw GLDException(msg, errorCode);
}

class CGrandXMLNodeStackPrivate
{
public:
  CGrandXMLNodeStackPrivate(CGrandXMLNodeStack *parent)
    : q_ptr(parent)
  {
  }

private:
  CGrandXMLNodeStack * const q_ptr;
  Q_DECLARE_PUBLIC(CGrandXMLNodeStack)

  // 这里采用std::vector, 因QVector跟QLatin1String有兼容问题 zhangjq 2015.09.29
  std::vector<GLatin1String> m_list;
  bool m_isEmptyElement;
};

/*CGrandXMLNodeStack*/
CGrandXMLNodeStack::CGrandXMLNodeStack() : d_ptr(new CGrandXMLNodeStackPrivate(this))
{
    Q_D(CGrandXMLNodeStack);
    d->m_isEmptyElement = false;
}

CGrandXMLNodeStack::~CGrandXMLNodeStack()
{
    Q_D(CGrandXMLNodeStack);
    d->m_list.clear();
    freePtr(d);
}

int CGrandXMLNodeStack::count()
{
    Q_D(CGrandXMLNodeStack);
    return d->m_list.size();
}

bool CGrandXMLNodeStack::isEmptyElement() const
{
    Q_D(const CGrandXMLNodeStack);
    return d->m_isEmptyElement;
}

void CGrandXMLNodeStack::setIsEmptyElement(const bool value)
{
    Q_D(CGrandXMLNodeStack);
    d->m_isEmptyElement = value;
}

GLatin1String CGrandXMLNodeStack::push(const GLatin1String &s)
{
    Q_D(CGrandXMLNodeStack);
    d->m_list.push_back(s);
    d->m_isEmptyElement = true;
    return s;
}

GLatin1String CGrandXMLNodeStack::pop()
{
    Q_D(CGrandXMLNodeStack);
    GLatin1String result = peek();
    d->m_list.erase(d->m_list.cbegin() + count() - 1);
    d->m_isEmptyElement = false;
    return result;
}

GLatin1String CGrandXMLNodeStack::peek()
{
    Q_D(CGrandXMLNodeStack);
    return d->m_list[count() - 1];
}

class GLDXMLWriterPrivate
{
public:
  GLDXMLWriterPrivate(GLDXMLWriter *parent)
    : q_ptr(parent)
  {
  }

private:
  GLDXMLWriter * const q_ptr;
  Q_DECLARE_PUBLIC(GLDXMLWriter)

  CXMLEncode m_encoding;
  GStream *m_stream;
  CGrandXMLNodeStack m_nodeStack;
  bool m_bAutoIndent;
  int m_currentLevel;
};

/*CGrandXMLWriter*/

GLDXMLWriter::GLDXMLWriter(GStream *stream, bool autoIndent):d_ptr(new GLDXMLWriterPrivate(this))
{
    Q_D(GLDXMLWriter);
    d->m_encoding = xeUtf8;
    d->m_stream = stream;
    d->m_bAutoIndent = autoIndent;
    d->m_currentLevel = 0;

    assert(stream != NULL);
}

GLDXMLWriter::~GLDXMLWriter()
{
    Q_D(GLDXMLWriter);
    freePtr(d);
}

void GLDXMLWriter::createProcessingInstruction(const GString &version, const GString &Encoding,
        const GString &standalone)
{
    Q_D(GLDXMLWriter);
    GString strTemp = "";

    if (version.length() == 0)
    {
        strTemp.append("version=\"1.0\" ");
    }
    else
    {
        strTemp.append(format("version=\"%s\" ", version));
    }

    if (Encoding.length() == 0)
    {
        strTemp.append("encoding=\"utf-8\" ");
    }
    else
    {
        strTemp.append(format("encoding=\"%s\" ", Encoding));
    }

    if (standalone != "")
    {
        strTemp.append(format("standalone=\"%s\" ", standalone));
    }

    writeLn(GString("<?xml ").append(strTemp).append("?>"));
    d->m_encoding = getEncoding(Encoding);
}

void GLDXMLWriter::beginNode(const GLatin1String &name, GLDVector<GString> *attNames,
                             GLDVector<GString> *attValues)
{
    assert(attNames->count() == attValues->count());
    beginNode(name);

    for (int i = 0; i != attNames->count(); i++)
    {
        addAttr(attNames->at(i), attValues->at(i));
    }
}

void GLDXMLWriter::beginNode(const GLatin1String &name)
{
    Q_D(GLDXMLWriter);
    assert(name.size() > 0);

    if ((d->m_nodeStack.count() > 0) && d->m_nodeStack.isEmptyElement())
    {
        writeLn(GLatin1String(">"));
    }

    writeIndent(0);
    write(GLatin1String("<"));
    write(name);
    d->m_nodeStack.push(name);
    d->m_currentLevel++;
}

void GLDXMLWriter::endNode(bool indent)
{
    Q_D(GLDXMLWriter);
    if (d->m_nodeStack.isEmptyElement())
    {
        d->m_nodeStack.pop();
        writeLn(GLatin1String("/>"));
    }
    else
    {
        GLatin1String strTemp = d->m_nodeStack.pop();

        if (indent)
        {
            writeIndent(-1);
        }
        write(GLatin1String("</"));
        write(strTemp);
        write(GLatin1String(">"));
        if (d->m_bAutoIndent)
        {
            write(g_LineBreak);
        }
    }

    --d->m_currentLevel;
}

void GLDXMLWriter::addAttr(const GString &name, bool value)
{
    GString strName = boolToXMLString(value);
    addAttr(name, strName);
}

void GLDXMLWriter::addAttr(const GString &name, double value)
{
    GString strName = floatToXMLString(value);
    addAttr(name, strName);
}

void GLDXMLWriter::addAttr(const GString &name, long long value)
{
    GString strTemp = int64ToStr(value);
    addAttr(name, strTemp);
}

void GLDXMLWriter::addAttr(const GString &name, unsigned long long value)
{
    GString strName = uint64ToStr(value);
    addAttr(name, strName);
}

void GLDXMLWriter::addAttr(const GString &name, int value)
{
    GString strName = intToStr(value);
    addAttr(name, strName);
}

void GLDXMLWriter::addAttr(const GString &name, GVariant *value)
{
    GByteArray strName = base64Encode(value->toByteArray());
    addAttr(name, (GString)strName);
}

void GLDXMLWriter::addAttr(const GString &name, const GString &value)
{
    assert(name != "");
    GString strValue = encodeXMLString(value, false);
    write(GString(' ').append(GString(name)).append("=\"").append(strValue).append('\"'));
}

void GLDXMLWriter::createFullNode(const GLatin1String &name, const GString &text,
                                  GLDVector<GString> *attNames, GLDVector<GString> *attValues)
{
    Q_D(GLDXMLWriter);
    beginNode(name, attNames, attValues);

    if (text.length() > 0)
    {
        d->m_nodeStack.setIsEmptyElement(false);
        GString strText = encodeXMLString(text, false);
        write(GLatin1String(">"));
        write(strText);
    }

    endNode(false);
}

void GLDXMLWriter::createFullNode(const GLatin1String &name, GLDVector<GString> *attNames,
                                  GLDVector<GString> *attValues)
{
    Q_D(GLDXMLWriter);
    beginNode(name, attNames, attValues);
    d->m_nodeStack.setIsEmptyElement(true);
    endNode();
}

void GLDXMLWriter::createFullNode(const GLatin1String &name, const GString &text)
{
    Q_D(GLDXMLWriter);
    beginNode(name);

    if (text.length() > 0)
    {
        d->m_nodeStack.setIsEmptyElement(false);
        GString strText = encodeXMLString(text, false);
        write(GLatin1String(">"));
        write(strText);
    }

    endNode(false);
}

GStream *GLDXMLWriter::stream()
{
    Q_D(GLDXMLWriter);
    return d->m_stream;
}

CXMLEncode GLDXMLWriter::encoding()
{
    Q_D(GLDXMLWriter);
    return d->m_encoding;
}

void GLDXMLWriter::setIsEmptyElement(bool value)
{
    Q_D(GLDXMLWriter);
    if (d->m_nodeStack.isEmptyElement() == value)
    {
        return;
    }

    if (!value)
    {
        d->m_nodeStack.setIsEmptyElement(value);
        write(GLatin1String(">"));
    }
}

void GLDXMLWriter::addAttrVarRec(const GString &name, const GVariant &v)
{
    GString strAttr;

    switch (v.type())
    {
        case GVariant::Int:
            strAttr = intToStr(v.toInt());
            break;

        case GVariant::Bool:
            strAttr = boolToXMLString(v.toBool());
            break;

        case GVariant::Char:
            strAttr = GString(v.toChar());
            break;

        case GVariant::Double:
            strAttr = floatToXMLString(v.toDouble());
            break;

        case GVariant::String:
            strAttr = v.toString();
            break;

            //todo Lipl
            //    case vtPointer:
            //        assert(false);
            //        break;
        case GVariant::UInt:
            strAttr = v.toUInt();
            break;

            //    case vtObject:
            //        assert(false);
            //        break;
            //    case vtAnsiString:
            //        strTemp = string(VAnsiString);
            //        break;
            //    case vtCurrency:
            //        strTemp = currToStr((*VCurrency));
            //        break;
        case GVariant::Invalid:
            assert(false);
            break;

            //    case vtWideString:
            //        strTemp = wideString(VWideString);
            //        break;
        case GVariant::LongLong:
            strAttr = int64ToStr(v.toLongLong());
            break;

        case GVariant::ULongLong:
            strAttr = uint64ToStr(v.toULongLong());
            break;

        default:
            xmlWriterError(getGLDi18nStr(g_rsErrUnsupportVariantType), v.type());
    }

    G_UNUSED(name)
}

void GLDXMLWriter::write(const GString &s)
{
    Q_D(GLDXMLWriter);
    GByteArray data = s.toUtf8();
    d->m_stream->write(data.constData(), data.size());
}

void GLDXMLWriter::write(const GLatin1String &s)
{
    Q_D(GLDXMLWriter);
    d->m_stream->write(s.data(), s.size());
}

void GLDXMLWriter::writeLn(const GString &s)
{
    Q_D(GLDXMLWriter);
    write(s);
    if (d->m_bAutoIndent)
    {
        write(g_LineBreak);
    }
}

void GLDXMLWriter::writeLn(const GLatin1String &s)
{
    Q_D(GLDXMLWriter);
    write(s);
    if (d->m_bAutoIndent)
    {
        write(g_LineBreak);
    }
}

void GLDXMLWriter::writeIndent(int offset)
{
    Q_D(GLDXMLWriter);
    if (d->m_bAutoIndent)
    {
        int nLength = (d->m_currentLevel + offset) * 4;
        const int nMaxLength = 1024;
        if (nLength >= nMaxLength)
        {
            char *szTemp = new char[nLength + 1];
            memset(szTemp, ' ', nLength);
            szTemp[nLength] = '\0';
            write(QLatin1String(szTemp, nLength));
            delete[] szTemp;
        }
        else
        {
            char szTemp[nMaxLength] = {' '};
            memset(szTemp, ' ', nMaxLength);
            szTemp[nLength] = '\0';
            write(QLatin1String(szTemp, nLength));
        }
    }
}

CXMLEncode GLDXMLWriter::getEncoding(const GString &encode)
{
    if (encode.length() == 0)
    {
        return xeUtf8;
    }
    else if (sameText(encode, c_defUtf8Encoding))
    {
        return xeUtf8;
    }

    return xeUtf8;
}
