#ifndef GLDXMLCORE_H
#define GLDXMLCORE_H

#include "GLDComptr.h"
#include "GLDXMLInterface.h"

class GLDXMLNode;
class GLDXMLNodeList;
class GLDXMLNodeAttribute;
class GLDXMLNodeAttributes;

class GLDXMLSHARED_EXPORT GLDXMLDocument : public GComPtr<IGLDXMLDocument> // TXMLIntfObject未找到Qt中对应结构
{
public:
    GLDXMLDocument();
    inline GLDXMLDocument(IGLDXMLDocument *lp) : GComPtr<IGLDXMLDocument>(lp) {}
    inline GLDXMLDocument(IGLDXMLDocument *lp, bool /*dummy*/) : GComPtr<IGLDXMLDocument>(lp, false){}
    template <typename Q>
    inline GLDXMLDocument(const GComPtr<Q> &lp) : GComPtr<IGLDXMLDocument>(lp){}

public:
    GLDXMLNode createElement(const GString &name);
    GLDXMLNode createNode(const GString &name, EnXMLNodeType type = ELEMENT);
    bool isEmpty();
    void loadFromFile(const GString &fileName);
    void saveToFile(const GString &fileName);
    void loadFromStream(GStream *stream);
    void loadFromStream(GStream *stream, bool aUseUtf8Optimize);
    void saveToStream(GStream *stream);

    bool autoIndent();
    void setAutoIndent(bool value);
    GString displayText();
    GLDXMLNode documentElement() const;
    GString encoding();
    void setEncoding(const GString &value);
    GString fileName();
    void setFileName(const GString &value);
    bool includeHeader();
    void setIncludeHeader(bool value);
    EnTXMLOptions options();
    void setOptions(EnTXMLOptions xmlOptionsSet);
    GString version();
    void setVersion(const GString &value);
    GString xml();
    void setXML(const GString &value);
    GLDXMLNode root();
    void setRoot(const GLDXMLNode &root);

    /*Qt XML*/
    GLDXMLNode appendChild(GLDXMLNode newChild);
    bool setContent(GStream* dev, GString *errorMsg=0, int *errorLine=0, int *errorColumn=0 );
    bool setContent(const GString& text, bool namespaceProcessing, GString *errorMsg=0, int *errorLine=0, int *errorColumn=0 );
    bool setContent(const GString& text, GString *errorMsg=0, int *errorLine=0, int *errorColumn=0 );
    bool setContent(const QByteArray& text, GString *errorMsg=0, int *errorLine=0, int *errorColumn=0 );
    GLDXMLNode firstChild() const;
    void save(GStream* stream, int indent/*, EncodingPolicy=QDomNode::EncodingFromDocument*/);
    GString toString(int indent = 1) const;
    GLDXMLNode elementById(const GString &elementId);
    GLDXMLNodeAttribute createAttribute(const GString& name);
    GLDXMLNode firstChildElement(const GString &tagName = GString()) const;
    GLDXMLNode createTextNode(const GString& data);
    GLDXMLNodeList childNodes() const;
    void clear();
    GLDXMLNode toElement() const;
    bool isNull() const;

private:
    void innerCreateXMLDocument();
};

class GLDXMLSHARED_EXPORT GLDXMLNode : public GComPtr<IGLDXMLNode>
{
public:
    inline GLDXMLNode() : GComPtr<IGLDXMLNode> () {}
    inline GLDXMLNode(IGLDXMLNode *lp) : GComPtr<IGLDXMLNode>(lp) {}
    inline GLDXMLNode(IGLDXMLNode *lp, bool /*dummy*/) : GComPtr<IGLDXMLNode>(lp, false){}
    template <typename Q>
    inline GLDXMLNode(const GComPtr<Q> &lp) : GComPtr<IGLDXMLNode>(lp){}

public:
    GLDXMLNode addChild(const GString &strName);
    GLDXMLNode cloneNode(bool deep = true);
    bool hasAttribute(const GString &strName) const;
    GLDXMLNode nextSibling();
    GLDXMLNode prevSibling();

    bool asBoolean();
    double asFloat();
    gint64 asInt64();
    int asInteger();
    GString asString();
    GVariant asVariant();
    GLDXMLNodeAttributes attributes() const;
    GLDXMLNodeList childNodes() const;
    int level();
    GString name();
    EnXMLNodeType nodeType();
    GLDXMLNode parent() const;
    GString text() const;
    GString xml();

    void setAsBoolean(bool value);
    void setAsFloat(float value);
    void setAsInt64(gint64 value);
    void setAsInteger(int value);
    void setAsString(const GString &value);
    void setAsVariant(const GVariant value);
    void setName(const GString &value);
    void setNodeType(const EnXMLNodeType value);
    void setParent(const GLDXMLNode &parent);
    void setText(const GString &value);
    void setXML(const GString &value);
    void loadFromStream(GStream *pStream);
    void saveToStream(GStream *pStream);

    /**Qt XML*/
    GLDXMLNode firstChildElement(const GString &tagName = GString()) const;
    GString nodeName() const;
    GLDXMLNode nextSiblingElement(const GString &taName = GString()) const;
    GLDXMLDocument ownerDocument() const;
    GLDXMLNode appendChild(const GLDXMLNode &newChild);
    GString attribute(const GString& name, const GString& defValue = GString()) const;
    void setAttribute(const GString& name, const GString& value);
    void setAttribute(const GString& name, int value);
    GLDXMLNode insertBefore(const GLDXMLNode &newChild, const GLDXMLNode &refChild);
    GLDXMLNode removeChild(const GLDXMLNode &oldChild);
    bool hasChildNodes() const;
    GLDXMLNode parentNode() const;
    void setNodeValue(const GString& value);
    GLDXMLNodeAttribute attributeNode(const GString& name);
    void clear();
    void setTagName(const GString& name); // Qt extension
    GString tagName() const;
    GLDXMLNode toElement() const;
    GLDXMLNodeList elementsByTagName(const GString& tagname) const;
    GLDXMLNode firstChild() const;
    GString nodeValue() const;
    GLDXMLNode lastChild() const;
    GLDXMLNode toText() const;
    GString data() const;
    void removeAttribute(const GString& name);
    GLDXMLNodeAttribute setAttributeNode(GLDXMLNodeAttribute& newAttr);
    bool isNull() const;
    bool isElement() const;

    GLDXMLNode replaceChild(const GLDXMLNode &newChild, const GLDXMLNode &oldChild);
    GLDXMLNode insertAfter(const GLDXMLNode &newChild, const GLDXMLNode &refChild);
    GLDXMLNode namedItem(const GString& name) const;
};

class GLDXMLSHARED_EXPORT GLDXMLNodeList : public GComPtr<IGLDXMLNodeList>
{
public:
    inline GLDXMLNodeList() : GComPtr<IGLDXMLNodeList> () {}
    inline GLDXMLNodeList(IGLDXMLNodeList *lp) : GComPtr<IGLDXMLNodeList>(lp) {}
    inline GLDXMLNodeList(IGLDXMLNodeList *lp, bool /*dummy*/) : GComPtr<IGLDXMLNodeList>(lp, false){}
    template <typename Q>
    inline GLDXMLNodeList(const GComPtr<Q> &lp) : GComPtr<IGLDXMLNodeList>(lp){}

public:
    int add(const GLDXMLNode &node);
    int deleteNode(const GString &name);
    void deleteNode(int index);
    int indexOf(const GLDXMLNode &node);
    int indexOf(const GString &name);
    int remove(const GLDXMLNode &node);
    void clear();
    void exChange(int cureIndex, int newIndex);
    void insert(int index, const GLDXMLNode &node);
    void move(int cureIndex, int newIndex);
    GLDXMLNode findNode(const GString &name);

    int count() const;// read m_count
    GLDXMLNode node(int index);// read m_nodes
    GLDXMLNode nodeByName(const GString &name);// read m_byNameB

    /*Qt XML*/
    GLDXMLNode at(int index) const;
    int size() const { return count(); }
    GLDXMLNode item(int index);
};

class GLDXMLSHARED_EXPORT GLDXMLNodeAttribute : public GComPtr<IGLDXMLNodeAttribute>
{
public:
    inline GLDXMLNodeAttribute() : GComPtr<IGLDXMLNodeAttribute> () {}
    inline GLDXMLNodeAttribute(IGLDXMLNodeAttribute *lp) : GComPtr<IGLDXMLNodeAttribute>(lp) {}
    inline GLDXMLNodeAttribute(IGLDXMLNodeAttribute *lp, bool /*dummy*/) : GComPtr<IGLDXMLNodeAttribute>(lp, false){}
    template <typename Q>
    inline GLDXMLNodeAttribute(const GComPtr<Q> &lp) : GComPtr<IGLDXMLNodeAttribute>(lp){}

public:
    bool asBoolean();
    double asFloat();
    gint64 asInt64();
    int asInteger();
    GString asString();
    GVariant asVariant();
    int hashData();
    GString name();
    GString text();
    GString xml();

    void setAsBoolean(bool value);
    void setAsFloat(double value);
    void setAsInt64(gint64 value);
    void setAsInteger(int value);
    void setAsString(const GString &value);
    void setAsVariant(const GVariant value);
    void setName(const GString &strName);
    void setText(const GString &strText);
    void setXml(const GString &strXML);

    /*Qt XML*/
    void setNodeValue(const GString &value);
    GString nodeValue() const;
    GLDXMLNodeAttribute toAttr() const;
    GString value() const;
    GString nodeName() const;
};

class GLDXMLSHARED_EXPORT GLDXMLNodeAttributes : public GComPtr<IGLDXMLNodeAttributes>
{
public:
    inline GLDXMLNodeAttributes() : GComPtr<IGLDXMLNodeAttributes> () {}
    inline GLDXMLNodeAttributes(IGLDXMLNodeAttributes *lp) : GComPtr<IGLDXMLNodeAttributes>(lp) {}
    inline GLDXMLNodeAttributes(IGLDXMLNodeAttributes *lp, bool /*dummy*/) : GComPtr<IGLDXMLNodeAttributes>(lp, false){}
    template <typename Q>
    inline GLDXMLNodeAttributes(const GComPtr<Q> &lp) : GComPtr<IGLDXMLNodeAttributes>(lp){}

public:
    void clear();
    void remove(int nIndex);
    void setXml(const GString &value);

    int count();
    int indexOf(const GString &strName);

    GString xml();
    GLDXMLNodeAttribute add(const GString &strName);
    GLDXMLNodeAttribute item(int nIndex);
    GLDXMLNodeAttribute itemsByName(const GString &strName);

    /*Qt XML*/
    GLDXMLNodeAttribute namedItem(const GString& name) const;
    bool contains(const GString& name);
};
#endif // GLDXMLCORE_H
