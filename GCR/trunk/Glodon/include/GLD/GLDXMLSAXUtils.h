#ifndef GLDSAXXMLUTILS_H
#define GLDSAXXMLUTILS_H

#include <QXmlStreamReader>
#include "GLDString.h"
#include "GLDFile.h"
#include "GLDXML_Global.h"

class GLDXMLSAXReaderPrivate;
class GLDXMLSHARED_EXPORT GLDXMLSAXReader
{
public:
    explicit GLDXMLSAXReader();
    explicit GLDXMLSAXReader(const GString &fileName);
    explicit GLDXMLSAXReader(QIODevice *device);
    virtual ~GLDXMLSAXReader();
public:
    static GString tr(const char *sourceText, const char *disambiguation = 0, int n = -1);
public:
    GString readNextNode(const GString &nodeName = "");
    GStringRef readNextNodeNameRef(const GLatin1String &nodeName);
    GStringRef readNextNodeNameRef();
    GString readAttribute(const GString &attributeName) const;
    bool hasAttribute(const GString &attribute) const;
    QXmlStreamAttributes attributes() const;
    GString readElementText(QXmlStreamReader::ReadElementTextBehaviour behaviour = QXmlStreamReader::ErrorOnUnexpectedElement);

    GStringRef text() const;
    GString name() const;
    GStringRef nameRef() const;

    bool atEnd() const;
    bool isEndElement() const;
    bool isEndDocument() const;
    bool isStartElement() const;
    bool isStartDocument() const;
    bool isCharacters() const;
    bool isWhitespace() const;
    bool isCDATA() const;
    bool isComment() const;
    bool isDTD() const;
    bool isEntityReference() const;
    bool isProcessingInstruction() const;
    void skipCurrentElement();
    void reset();

    void setDevice(QIODevice *device);
    void setFilename(const GString &fileName);
    void closeFile();
    QXmlStreamReader& reader();
private:
    GLDXMLSAXReaderPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GLDXMLSAXReader);
};

#endif // GLDSAXXMLUTILS_H
