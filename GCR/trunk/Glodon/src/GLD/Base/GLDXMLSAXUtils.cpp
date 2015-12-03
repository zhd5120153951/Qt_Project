#include "GLDXMLSAXUtils.h"

#include "GLDStrUtils.h"
#include "GLDException.h"
#include "GLDXML_Global.h"
#include <QCoreApplication>

class GLDXMLSAXReaderPrivate
{
public:
  GLDXMLSAXReaderPrivate(GLDXMLSAXReader *parent)
    : q_ptr(parent), m_file(NULL)
  {
  }

private:
  GLDXMLSAXReader * const q_ptr;
  Q_DECLARE_PUBLIC(GLDXMLSAXReader);

  GFileStream *m_file;
  QXmlStreamReader m_reader;
};

GLDXMLSAXReader::GLDXMLSAXReader(): d_ptr(new GLDXMLSAXReaderPrivate(this))
{
}

GLDXMLSAXReader::GLDXMLSAXReader(const GString &fileName): d_ptr(new GLDXMLSAXReaderPrivate(this))
{
    setFilename(fileName);
}

GLDXMLSAXReader::GLDXMLSAXReader(QIODevice *device): d_ptr(new GLDXMLSAXReaderPrivate(this))
{
    setDevice(device);
}

GLDXMLSAXReader::~GLDXMLSAXReader()
{
    Q_D(GLDXMLSAXReader);
    closeFile();
    freePtr(d);
}

GString GLDXMLSAXReader::readAttribute(const GString &attributeName) const
{
    Q_D(const GLDXMLSAXReader);
    if (d->m_reader.isStartElement())
    {
        QXmlStreamAttributes attributes = d->m_reader.attributes();
        return attributes.value(attributeName).toString();
    }
    return GString();
}

bool GLDXMLSAXReader::hasAttribute(const GString &attribute) const
{
    Q_D(const GLDXMLSAXReader);
    if (!d->m_reader.atEnd())
    {
        QXmlStreamAttributes attributes = d->m_reader.attributes();
        return !attributes.value(attribute).isEmpty();
    }
    return false;
}

QXmlStreamAttributes GLDXMLSAXReader::attributes() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.attributes();
}

GString GLDXMLSAXReader::readElementText(QXmlStreamReader::ReadElementTextBehaviour behaviour)
{
    Q_D(GLDXMLSAXReader);
    return d->m_reader.readElementText(behaviour);
}

GStringRef GLDXMLSAXReader::text() const
{
    Q_D(const GLDXMLSAXReader);
    if (!d->m_reader.atEnd())
    {
        d->m_reader.text();
    }
    return GStringRef();
}

GString GLDXMLSAXReader::name() const
{
    Q_D(const GLDXMLSAXReader);
    GString name = d->m_reader.name().toString();
    if (d->m_reader.prefix().length() > 0)
    {
        return d->m_reader.prefix().toString() + ":" + name;
    }
    return name;
}

GStringRef GLDXMLSAXReader::nameRef() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.name();
}

bool GLDXMLSAXReader::atEnd() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.atEnd();
}

bool GLDXMLSAXReader::isEndElement() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.isEndElement();
}

bool GLDXMLSAXReader::isEndDocument() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.isEndDocument() || d->m_reader.atEnd();
}

bool GLDXMLSAXReader::isStartElement() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.isStartElement();
}

bool GLDXMLSAXReader::isStartDocument() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.isStartDocument();
}

bool GLDXMLSAXReader::isCharacters() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.isCharacters();
}

bool GLDXMLSAXReader::isWhitespace() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.isWhitespace();
}

bool GLDXMLSAXReader::isCDATA() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.isCDATA();
}

bool GLDXMLSAXReader::isComment() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.isComment();
}

bool GLDXMLSAXReader::isDTD() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.isDTD();
}

bool GLDXMLSAXReader::isEntityReference() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.isEntityReference();
}

bool GLDXMLSAXReader::isProcessingInstruction() const
{
    Q_D(const GLDXMLSAXReader);
    return d->m_reader.isProcessingInstruction();
}

void GLDXMLSAXReader::skipCurrentElement()
{
    Q_D(GLDXMLSAXReader);
    return d->m_reader.skipCurrentElement();
}

GString GLDXMLSAXReader::tr(const char *sourceText, const char *disambiguation, int n)
{
    return QCoreApplication::translate("GLDXMLSAXReader", sourceText, disambiguation, n);
}

void GLDXMLSAXReader::setDevice(QIODevice *device)
{
    assert(device);
    Q_D(GLDXMLSAXReader);
    d->m_reader.setDevice(device);
    d->m_reader.setNamespaceProcessing(false);
    QXmlStreamReader::TokenType type = d->m_reader.readNext();
    if (type == QXmlStreamReader::StartDocument)
    {
        d->m_reader.readNext();
    }
}

void GLDXMLSAXReader::setFilename(const GString &fileName)
{
    Q_D(GLDXMLSAXReader);
    freeAndNil(d->m_file);
    d->m_file = new GFileStream();
    d->m_file->setFileName(fileName);
    if (d->m_file->open(QIODevice::ReadOnly))
    {
        setDevice(d->m_file);
    }
    else
    {
        gldError(tr("File not exist"));//TRANS_STRING("文件不存在"));
    }
}

void GLDXMLSAXReader::closeFile()
{
    Q_D(GLDXMLSAXReader);
    if (d->m_file)
    {
        d->m_file->close();
        freeAndNil(d->m_file);
    }
    else
    {
        // do nothing
    }
}

void GLDXMLSAXReader::reset()
{
    Q_D(GLDXMLSAXReader);
    if (d->m_file)
    {
        GString fileName = d->m_file->fileName();
        d->m_reader.clear();
        closeFile();
        setFilename(fileName);
    }
    else
    {
        QIODevice *device = d->m_reader.device();
        device->seek(0);
        d->m_reader.clear();
        setDevice(device);
    }
}

GString GLDXMLSAXReader::readNextNode(const GString &nodeName)
{
    Q_D(GLDXMLSAXReader);
    while (!d->m_reader.isEndDocument())
    {
        d->m_reader.readNextStartElement();
        GString name = d->m_reader.name().toString();
        if (d->m_reader.prefix().length() > 0)
        {
            name = GString(d->m_reader.prefix().toString()).append(":").append(name);
        }
        if (nodeName.isEmpty())
            return name;
        else
        {
            if (0 == name.compare(nodeName, Qt::CaseInsensitive))
                return name;
            else
            {
                //针对没有数据块，但为了读取数据块又指定了nodeName的情形
                QXmlStreamReader::TokenType tokey = d->m_reader.tokenType();
                if (QXmlStreamReader::Invalid == tokey || QXmlStreamReader::NoToken == tokey)
                    return name;
            }
        }
    }
    return GString();
}

GStringRef GLDXMLSAXReader::readNextNodeNameRef(const GLatin1String &nodeName)
{
    GStringRef result = GStringRef();
    while (!sameText(result, nodeName))
    {
        result = readNextNodeNameRef();
    }
    return result;
}

GStringRef GLDXMLSAXReader::readNextNodeNameRef()
{
    Q_D(GLDXMLSAXReader);
    while (!d->m_reader.isEndDocument())
    {
        d->m_reader.readNextStartElement();
        return d->m_reader.name();
    }
    return GStringRef();
}
