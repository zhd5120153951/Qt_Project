/*************************************************************************/
/*                                                                       */
/*  广联达 XML 直接编写对象实现单元                                          */
/*                                                                       */
/*  设计：Linc 2012.06.05                                                 */
/*  备注：                                                                */
/*  审核：                                                                */
/*                                                                       */
/*  copyright(c) 1994-2012 GrandSoft Corporation                         */
/*                                                                       */
/*************************************************************************/
#ifndef GLDXMLWRITER_H
#define GLDXMLWRITER_H

#include "GLDString.h"
#include "GLDVariant.h"
#include "GLDIODevice.h"
#include "GLDVector.h"
#include "GLDXML_Global.h"

class CGrandXMLNodeStack;
class GLDXMLWriter;

enum CXMLEncode
{
    xeUtf8
};

class CGrandXMLNodeStackPrivate;
class GLDXMLSHARED_EXPORT CGrandXMLNodeStack
{
public:
    CGrandXMLNodeStack();
    virtual ~CGrandXMLNodeStack();
    int count();
    bool isEmptyElement() const;
    void setIsEmptyElement(const bool value);

    GLatin1String push(const GLatin1String &s);
    GLatin1String pop();
    GLatin1String peek();
private:
    CGrandXMLNodeStackPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(CGrandXMLNodeStack);
};

class GLDXMLWriterPrivate;
class GLDXMLSHARED_EXPORT GLDXMLWriter
{
public:
    GLDXMLWriter(GStream *stream, bool autoIndent = false);
    virtual ~GLDXMLWriter();
    void createProcessingInstruction(const GString &version = "", const GString &Encoding = "",
                                     const GString &standalone = "");
    void beginNode(const GLatin1String &name, GLDVector<GString> *attNames, GLDVector<GString> *attValues);
    void beginNode(const GLatin1String &name);
    void endNode(bool indent = true);

    void addAttr(const GString &name, bool value);
    void addAttr(const GString &name, double value);
    void addAttr(const GString &name, long long value);
    void addAttr(const GString &name, unsigned long long value);
    void addAttr(const GString &name, int value);
    void addAttr(const GString &name, GVariant *value);
    void addAttr(const GString &name, const GString &value);

    void createFullNode(const GLatin1String &name, const GString &text, GLDVector<GString> *attNames,
                        GLDVector<GString> *attValues);
    void createFullNode(const GLatin1String &name, GLDVector<GString> *attNames, GLDVector<GString> *attValues);
    void createFullNode(const GLatin1String &name, const GString &text = "");

    GStream *stream();
    CXMLEncode encoding();
    void setIsEmptyElement(bool value);
private:
    void addAttrVarRec(const GString &name, const GVariant &v);
    void write(const GString &s);
    void write(const GLatin1String &s);
    void writeLn(const GString &s);
    void writeLn(const GLatin1String &s);
    void writeIndent(int offset);
    CXMLEncode getEncoding(const GString &encode);
private:
    GLDXMLWriterPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GLDXMLWriter)
};

#endif // GLDXMLWRITER_H
