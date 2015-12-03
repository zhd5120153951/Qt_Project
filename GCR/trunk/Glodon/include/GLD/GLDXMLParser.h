#ifndef GXMLPARSER_H
#define GXMLPARSER_H

#include "GLDStrUtils.h"
#include "GLDXMLInterface.h"
#include "GLDXMLDoc.h"

class CGLDXMLDocument;

class GXMLParserPrivate;
class GLDXMLSHARED_EXPORT GXMLParser
{
public:
    explicit GXMLParser(const GString strData, IGLDXMLNode *pBase, EnTXMLOptions  Options );
    ~GXMLParser();

public:
    CGLDXMLNode *addOpenTag(const GString &text);
    void addCloseTag();
    void addDataNode(const GString &data, EnXMLNodeType nodeType);
    void addProcessingInstruction(const GString &data);
    void doParse();
    void xmlDocErrorInParser(const GString &msg);
    CGLDXMLNode *base();
    const CGLDXMLNode *current();
    GString encoding();
    GString version();
    void setPos(int pos);
    void setDocument(CGLDXMLDocument *document);
    static void execute(const GString &data, CGLDXMLDocument *pdoc, int iPos);
    static void execute(const GString &data, CGLDXMLNode *pBase, EnTXMLOptions options);

private:
    GXMLParserPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GXMLParser);
};

#endif // GXMLPARSER_H
