#ifndef GXMLBUILDER_H
#define GXMLBUILDER_H

#include "GLDString.h"
#include "GLDIODevice.h"
#include "GLDXML_Global.h"

class CGLDXMLDocument;
class CGLDXMLNode;

enum EnXMLEncode
{
    XE_ANSI,
    XE_UTF8
};

class GXMLBuilderPrivate;
class GLDXMLSHARED_EXPORT GXMLBuilder
{
public:
    explicit GXMLBuilder(GStream *stream);
    ~GXMLBuilder();

public:
    static GString saveDocText(CGLDXMLDocument *doc, bool replaceChars);
    static void saveDoc(CGLDXMLDocument *doc, GStream *stream);
    static void saveNode(CGLDXMLNode *node, GStream *stream);
    void saveXMLDocument(CGLDXMLDocument *doc, bool replaceChars, bool ignoreEncode);// –Ë¿Ì«Â¬ﬂº≠

private:
    GString getNodeOpenTag(CGLDXMLNode *node, bool end = false);
    GString getNodeCloseTag(CGLDXMLNode *node);

    void recursionWriteNode(CGLDXMLNode *node);
    void writeIndentText(int level);
    void writeText(const GString &text, bool addLineBreak = true);

private:
    GXMLBuilderPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GXMLBuilder);
};

#endif // GXMLBUILDER_H
