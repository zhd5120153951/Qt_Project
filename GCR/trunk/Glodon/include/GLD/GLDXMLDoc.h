#ifndef GLDXMLDOC_H
#define GLDXMLDOC_H

#include "GLDChar.h"
#include "GLDXMLInterface.h"
#include "GLDXML_Global.h"

class CGLDXMLNode;
class CGLDXMLNodeAttribute;
class CGLDXMLDocument;
class CGLDXMLNodeAttributes;
class CGLDXMLNodeList;

extern const GString c_uft8Encoding;
extern const GString c_gdefNodeName[4];
extern const GString c_gconCDataPosx;
extern const GString c_gconCDataPrex;

class CGLDXMLNodePrivate;
class GLDXMLSHARED_EXPORT CGLDXMLNode : public GInterfaceObject, public IGLDXMLNode, public IGLDXMLJSONPersistent
{
public:
    CGLDXMLNode(IGLDXMLNode *pParentNode, EnTXMLOptions enOptions, EnXMLNodeType enType, IGLDXMLDocument *ownerDocument = NULL);
    virtual ~CGLDXMLNode();

public:
    DECLARE_IUNKNOWN
    IGLDXMLNode* GLDMETHODCALLTYPE addChild(GString strName);
    IGLDXMLNode* GLDMETHODCALLTYPE cloneNode(bool deep);
    bool GLDMETHODCALLTYPE hasAttribute(const GString &strName) const;
    IGLDXMLNode* GLDMETHODCALLTYPE nextSibling();
    IGLDXMLNode* GLDMETHODCALLTYPE prevSibling();

    bool GLDMETHODCALLTYPE asBoolean();
    double GLDMETHODCALLTYPE asFloat();
    gint64 GLDMETHODCALLTYPE asInt64();
    int GLDMETHODCALLTYPE asInteger();
    GString GLDMETHODCALLTYPE asString();
    GVariant GLDMETHODCALLTYPE asVariant();
    IGLDXMLNodeAttributes* GLDMETHODCALLTYPE attributes() const;
    IGLDXMLNodeList* GLDMETHODCALLTYPE childNodes() const;
    int GLDMETHODCALLTYPE level();
    QString GLDMETHODCALLTYPE name();
    EnXMLNodeType GLDMETHODCALLTYPE nodeType();
    IGLDXMLNode* GLDMETHODCALLTYPE parent() const;
    QString GLDMETHODCALLTYPE text() const;
    QString GLDMETHODCALLTYPE xml();
    void GLDMETHODCALLTYPE setAsBoolean(bool value);
    void GLDMETHODCALLTYPE setAsFloat(double value);
    void GLDMETHODCALLTYPE setAsInt64(gint64 value);
    void GLDMETHODCALLTYPE setAsInteger(int value);
    void GLDMETHODCALLTYPE setAsString(const GString &value);
    void GLDMETHODCALLTYPE setAsVariant(const GVariant value);
    void GLDMETHODCALLTYPE setName(const GString &value);
    void GLDMETHODCALLTYPE setNodeType(const EnXMLNodeType value);
    void GLDMETHODCALLTYPE setParent(IGLDXMLNode *value);
    void GLDMETHODCALLTYPE setText(const GString &value);
    void GLDMETHODCALLTYPE setXML(const GString &value);

    void GLDMETHODCALLTYPE loadFromStream(GStream *pStream);
    void GLDMETHODCALLTYPE saveToStream(GStream *pStream);

    /*IGLDXMLJSONPersistent*/
    QString GLDMETHODCALLTYPE asJSON();
    void GLDMETHODCALLTYPE setAsJSON(const GString &strValue);

    /*Qt ºÊ»›*/
    IGLDXMLNode* GLDMETHODCALLTYPE firstChildElement(const GString &tagName = GString()) const;
    IGLDXMLNode* GLDMETHODCALLTYPE insertBefore(IGLDXMLNode *newChild, IGLDXMLNode *refChild);

    QString GLDMETHODCALLTYPE nodeValue() const;
    IGLDXMLNodeAttribute* GLDMETHODCALLTYPE attributeNode(const QString &name);
    IGLDXMLNode* GLDMETHODCALLTYPE replaceChild(IGLDXMLNode* newChild, IGLDXMLNode* oldChild);
    IGLDXMLNode* GLDMETHODCALLTYPE insertAfter(IGLDXMLNode* newChild, IGLDXMLNode* refChild);

    void GLDMETHODCALLTYPE clear();
    void GLDMETHODCALLTYPE setAttribute(const GString &name, const GString &value);
    void GLDMETHODCALLTYPE setAttribute(const GString &name, int value);
    GString GLDMETHODCALLTYPE attribute(const GString &name, const GString &defValue = "attribute does not exist");
    IGLDXMLDocument* GLDMETHODCALLTYPE ownerDocument() const;

    IGLDXMLNode* GLDMETHODCALLTYPE toElement() const;
    bool GLDMETHODCALLTYPE isElement() const;
    bool GLDMETHODCALLTYPE isNull() const;
    bool GLDMETHODCALLTYPE hasChildNodes() const;
    IGLDXMLNode* GLDMETHODCALLTYPE parentNode() const;
    void GLDMETHODCALLTYPE setNodeValue(const GString& value);
    IGLDXMLNode* GLDMETHODCALLTYPE firstChild() const;
    IGLDXMLNode* GLDMETHODCALLTYPE lastChild() const;
    void GLDMETHODCALLTYPE removeAttribute(const GString& name);
    IGLDXMLNodeAttribute* GLDMETHODCALLTYPE setAttributeNode(IGLDXMLNodeAttribute *newAttr);

    GString GLDMETHODCALLTYPE nodeName();
    IGLDXMLNode* GLDMETHODCALLTYPE nextSiblingElement(const GString &tagName);
    IGLDXMLNode* GLDMETHODCALLTYPE appendChild(IGLDXMLNode *node);
    IGLDXMLNode* GLDMETHODCALLTYPE removeChild(IGLDXMLNode *node);
    void GLDMETHODCALLTYPE setTagName(const GString &name);
    GString GLDMETHODCALLTYPE tagName();
    IGLDXMLNodeList* GLDMETHODCALLTYPE elementsByTagName(const GString &value);
    IGLDXMLNode* GLDMETHODCALLTYPE toText();
    GString GLDMETHODCALLTYPE data();
    IGLDXMLNode* GLDMETHODCALLTYPE namedItem(const GString& name) const;

public:
    CGLDXMLNode* parentObj() const;
    CGLDXMLNodeList* childNodesObj() const;
    CGLDXMLNode* nextSiblingObj();
    CGLDXMLNode *prevSiblingObj();
    CGLDXMLNodeAttributes* attributesObj();
    CGLDXMLNode* cloneNodeObj(bool deep);
    CGLDXMLNode* toElementObj() const;

private:
    int getFirstNonCharIdx(const GString str, const GChar ch, int nOffSet, bool isReverse = false);
    int getFirstCharIdx(const GString str, const GChar ch, int nOffSet, bool isReverse = false);
    void elementsByTagName(CGLDXMLNodeList *list, CGLDXMLNode *parent, const GString &value);

private:
    CGLDXMLNodePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(CGLDXMLNode);
};

class CGLDXMLNodeAttributePrivate;
class GLDXMLSHARED_EXPORT CGLDXMLNodeAttribute : public GInterfaceObject, public IGLDXMLNodeAttribute
{
public:
    CGLDXMLNodeAttribute(const GString &name, EnTXMLOptions options);
    virtual ~CGLDXMLNodeAttribute();

public:
    DECLARE_IUNKNOWN
    bool GLDMETHODCALLTYPE asBoolean();
    double GLDMETHODCALLTYPE asFloat();
    gint64 GLDMETHODCALLTYPE asInt64();
    int GLDMETHODCALLTYPE asInteger();
    GString GLDMETHODCALLTYPE asString();
    GVariant GLDMETHODCALLTYPE asVariant();
    int GLDMETHODCALLTYPE hashData();
    GString GLDMETHODCALLTYPE name();
    GString GLDMETHODCALLTYPE text();
    GString GLDMETHODCALLTYPE xml();
    void GLDMETHODCALLTYPE setAsBoolean(bool value);
    void GLDMETHODCALLTYPE setAsFloat(double value);
    void GLDMETHODCALLTYPE setAsInt64(gint64 value);
    void GLDMETHODCALLTYPE setAsInteger(int value);
    void GLDMETHODCALLTYPE setAsString(const GString &value);
    void GLDMETHODCALLTYPE setAsVariant(const GVariant value);
    void GLDMETHODCALLTYPE setName(const GString &strName);
    void GLDMETHODCALLTYPE setText(const GString &strText);
    void GLDMETHODCALLTYPE setXml(const GString &strXML);

    /*QT ºÊ»›*/
    GString GLDMETHODCALLTYPE nodeName() const;
    CGLDXMLNodeAttribute* GLDMETHODCALLTYPE toAttr();
    GString GLDMETHODCALLTYPE value();
    GString GLDMETHODCALLTYPE nodeValue();
    void GLDMETHODCALLTYPE setNodeValue(const GString& value);

private:
    GString copyTrimSpaceAndQuotaionMark(const GString &str, int index);

private:
    CGLDXMLNodeAttributePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(CGLDXMLNodeAttribute);
};

class CGLDXMLDocumentPrivate;
class GLDXMLSHARED_EXPORT CGLDXMLDocument : public GInterfaceObject, public IGLDXMLDocument, public IGLDXMLJSONPersistent, public IGLDXMLJSONPersistentFile // TXMLIntfObjectŒ¥’“µΩQt÷–∂‘”¶Ω·ππ
{
public:
    explicit CGLDXMLDocument();
    ~CGLDXMLDocument();

public:
    DECLARE_IUNKNOWN
    IGLDXMLNode* GLDMETHODCALLTYPE createElement(const GString &name);
    IGLDXMLNode* GLDMETHODCALLTYPE createNode(const GString &name, EnXMLNodeType type = ELEMENT);
    bool GLDMETHODCALLTYPE isEmpty();
    void GLDMETHODCALLTYPE loadFromFile(const GString &fileName);
    void GLDMETHODCALLTYPE saveToFile(const GString &fileName);
    void GLDMETHODCALLTYPE loadFromStream(GStream *stream);
    void GLDMETHODCALLTYPE loadFromStream(GStream *stream, bool aUseUtf8Optimize);
    void GLDMETHODCALLTYPE saveToStream(GStream *stream);
    GString GLDMETHODCALLTYPE getXMLHeader();
    IGLDXMLNode* GLDMETHODCALLTYPE appendChild(IGLDXMLNode *newChild);
    IGLDXMLNode* GLDMETHODCALLTYPE firstChild() const;
    void GLDMETHODCALLTYPE save(GStream* stream, int indent/*, EncodingPolicy=QDomNode::EncodingFromDocument*/) const;
    bool GLDMETHODCALLTYPE setContent(GStream* dev, GString *errorMsg=0, int *errorLine=0, int *errorColumn=0 );
    bool GLDMETHODCALLTYPE setContent(const GString& text, bool namespaceProcessing, GString *errorMsg=0, int *errorLine=0, int *errorColumn=0 );
    bool GLDMETHODCALLTYPE setContent(const QString& text, QString *errorMsg=0, int *errorLine=0, int *errorColumn=0 );
    bool GLDMETHODCALLTYPE setContent(const QByteArray& text, QString *errorMsg=0, int *errorLine=0, int *errorColumn=0 );
    IGLDXMLNodeAttribute* GLDMETHODCALLTYPE createAttribute(const QString& name);
    IGLDXMLNode* GLDMETHODCALLTYPE createTextNode(const GString& data);
    IGLDXMLNodeList* GLDMETHODCALLTYPE childNodes() const;
    void GLDMETHODCALLTYPE clear();

    bool GLDMETHODCALLTYPE autoIndent();
    void GLDMETHODCALLTYPE setAutoIndent(bool value);
    GString GLDMETHODCALLTYPE displayText();
    IGLDXMLNode* GLDMETHODCALLTYPE documentElement() const;
    GString GLDMETHODCALLTYPE encoding();
    void GLDMETHODCALLTYPE setEncoding(const GString &value);
    GString GLDMETHODCALLTYPE fileName();
    void GLDMETHODCALLTYPE setFileName(const GString &value);
    bool GLDMETHODCALLTYPE includeHeader();
    void GLDMETHODCALLTYPE setIncludeHeader(bool value);
    EnTXMLOptions GLDMETHODCALLTYPE options();
    void GLDMETHODCALLTYPE setOptions(EnTXMLOptions xmlOptionsSet);
    GString GLDMETHODCALLTYPE version();
    void GLDMETHODCALLTYPE setVersion(const GString &value);
    GString GLDMETHODCALLTYPE xml();
    void GLDMETHODCALLTYPE setXML(const GString &value);
    IGLDXMLNode* GLDMETHODCALLTYPE root();
    void GLDMETHODCALLTYPE setRoot(IGLDXMLNode *root);
    GString GLDMETHODCALLTYPE toString(int indent = 1) const;
    IGLDXMLNode* GLDMETHODCALLTYPE elementById(const GString &elementID);
    IGLDXMLNode* GLDMETHODCALLTYPE firstChildElement(const GString &tagName) const;
    IGLDXMLNode* GLDMETHODCALLTYPE toElement() const;
    bool GLDMETHODCALLTYPE isNull() const;

    //GIXMLJSONPsst
    GString GLDMETHODCALLTYPE asJSON(); //∂¡m_asJSON
    void GLDMETHODCALLTYPE setAsJSON(const GString &value);//–¥m_asJSON

    //GIXMLJSONPsstFile
    void GLDMETHODCALLTYPE loadFromJSONFile(const GString &fileName);
    void GLDMETHODCALLTYPE loadFromJSONStream(GStream *stream);
    void GLDMETHODCALLTYPE saveAsJSONToFile(const GString &fileName);
    void GLDMETHODCALLTYPE saveAsJSONToStream(GStream *stream);

public:
    CGLDXMLNode* createNodeObj(const GString &name, EnXMLNodeType type = ELEMENT);
    void setRootObj(CGLDXMLNode *root);
    CGLDXMLNode *rootObj();
    bool findEncoding(const char *data, bool &bom, GString &encoding);
    void parse(const GString &data, int pos);

private:
    CGLDXMLDocumentPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(CGLDXMLDocument);
};

class CGLDXMLNodeAttributesPrivate;
class GLDXMLSHARED_EXPORT CGLDXMLNodeAttributes : public GInterfaceObject, public IGLDXMLNodeAttributes
{
public:
    CGLDXMLNodeAttributes(EnTXMLOptions enOptions);
    ~CGLDXMLNodeAttributes();

public:
    DECLARE_IUNKNOWN
    IGLDXMLNodeAttribute* GLDMETHODCALLTYPE add(const GString &strName);
    int GLDMETHODCALLTYPE indexOf(const GString &strName);
    void GLDMETHODCALLTYPE clear();
    void GLDMETHODCALLTYPE remove(int nIndex);

    int GLDMETHODCALLTYPE count();
    IGLDXMLNodeAttribute* GLDMETHODCALLTYPE item(int nIndex);
    IGLDXMLNodeAttribute* GLDMETHODCALLTYPE itemsByName(const GString &strName);
    GString GLDMETHODCALLTYPE xml();
    void GLDMETHODCALLTYPE setXml(const GString &value);

    /*Qt ºÊ»›*/
    int hashOf(const GString &strName);
    bool GLDMETHODCALLTYPE contains(const GString& name);
    IGLDXMLNodeAttribute* GLDMETHODCALLTYPE namedItem(const GString &name);

public:
    CGLDXMLNodeAttribute* itemsByNameObj(const GString &strName);
    CGLDXMLNodeAttribute* addObj(const GString &strName);
    CGLDXMLNodeAttribute* itemObj(int nIndex);

private:
    void doAddAttribute(const GString &strName, const GString &strText);
    bool findQuotationStr(const GString &str, int nstartPos, GString &squotedString, int &nendPos);
    GString trimSpecialChar(const GString &value);

private:
    CGLDXMLNodeAttributesPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(CGLDXMLNodeAttributes);
};

class CGLDXMLNodeListPrivate;
class GLDXMLSHARED_EXPORT CGLDXMLNodeList : public GInterfaceObject, public IGLDXMLNodeList
{
public:
    explicit CGLDXMLNodeList(IGLDXMLNode *parent=0);
    ~CGLDXMLNodeList();

public:
    DECLARE_IUNKNOWN
    int GLDMETHODCALLTYPE add(IGLDXMLNode *node);
    int GLDMETHODCALLTYPE deleteNode(const GString &name);
    void GLDMETHODCALLTYPE deleteNode(int index);
    IGLDXMLNode* GLDMETHODCALLTYPE findNode(const GString &name);
    int GLDMETHODCALLTYPE indexOf(IGLDXMLNode *node) const;
    int GLDMETHODCALLTYPE indexOf(const GString &name) const;
    int GLDMETHODCALLTYPE remove(IGLDXMLNode *node);
    void GLDMETHODCALLTYPE clear();
    void GLDMETHODCALLTYPE exChange(int cureIndex, int newIndex);
    void GLDMETHODCALLTYPE insert(int index, IGLDXMLNode *node);
    void GLDMETHODCALLTYPE move(int cureIndex, int newIndex);

    int GLDMETHODCALLTYPE count()const;
    IGLDXMLNode* GLDMETHODCALLTYPE item(int index);
    IGLDXMLNode* GLDMETHODCALLTYPE node(int index);
    IGLDXMLNode* GLDMETHODCALLTYPE nodeByName(const GString &name);

    /*Qt ºÊ»›*/
    IGLDXMLNode* GLDMETHODCALLTYPE at(int index);
public:
    CGLDXMLNode* nodeObj(int index);
    CGLDXMLNode* findNodeObj(const GString &name) const;

private:
    CGLDXMLNodeListPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(CGLDXMLNodeList);
};
#endif // GLDXMLDOC_H
