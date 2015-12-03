#ifndef GLDXMLINTERFACE_H
#define GLDXMLINTERFACE_H

#include "GLDString.h"
#include "GLDVariant.h"
#include "GLDStream.h"
#include "GLDObject.h"
#include "GLDStrings.h"
#include "GLDXML_Global.h"

interface IGLDXMLDocument;
interface IGLDXMLNodeList;
interface IGLDXMLNodeAttribute;
interface IGLDXMLNodeAttributes;

typedef byte EnTXMLOptions;

enum EnXMLNodeType
{
    ELEMENT,
    TEXT,
    CDATA,
    COMMENT
};


enum EnTXMLOption
{
    XO_DECODE_CRLF,
    XO_ENCODE_CRLF,
    XO_IGNORE_CDATA,
    XO_IGNORE_COMMENT
};

DEFINE_IID(IGLDXMLNode, "06468216-F087-47CA-AD19-A47393AFFF56");
DECLARE_INTERFACE_(IGLDXMLNode, IUnknown)
{
    GLDMETHOD(IGLDXMLNode *, addChild)(GString name) PURE; //返回自身类型需要返回指针类型
    GLDMETHOD(IGLDXMLNode *, cloneNode)(bool bDeep) PURE;
    GLDMETHOD(bool, hasAttribute)(const GString &nme) const PURE;
    GLDMETHOD(IGLDXMLNode *, nextSibling)() PURE;
    GLDMETHOD(IGLDXMLNode *, prevSibling)() PURE;    

    GLDMETHOD(bool, asBoolean)() PURE;
    GLDMETHOD(double, asFloat)() PURE;
    GLDMETHOD(int, asInteger)() PURE;
    GLDMETHOD(GString, asString)() PURE;
    GLDMETHOD(GVariant, asVariant)() PURE;
    GLDMETHOD(IGLDXMLNodeAttributes *, attributes)() const PURE;
    GLDMETHOD(IGLDXMLNodeList *, childNodes)() const PURE;
    GLDMETHOD(bool, hasChildNodes)()const PURE;
    GLDMETHOD(int, level)() PURE;
    GLDMETHOD(GString, name)() PURE;
    GLDMETHOD(EnXMLNodeType, nodeType)() PURE;
    GLDMETHOD(IGLDXMLNode *, parent)() const PURE;
    GLDMETHOD(GString, text)() const PURE;
    GLDMETHOD(GString, xml)() PURE;

    GLDMETHOD(void, setAsBoolean)(bool value) PURE;
    GLDMETHOD(void, setAsFloat)(double value) PURE;
    GLDMETHOD(void, setAsInteger)(int value) PURE;
    GLDMETHOD(void, setAsString)(const GString &value) PURE;
    GLDMETHOD(void, setAsVariant)(const GVariant value) PURE;
    GLDMETHOD(void, setName)(const GString &value) PURE;
    GLDMETHOD(void, setNodeType)(const EnXMLNodeType value) PURE;
    GLDMETHOD(void, setParent)(IGLDXMLNode *value) PURE;
    GLDMETHOD(void, setText)(const GString &value) PURE;
    GLDMETHOD(void, setXML)(const GString &value) PURE;

    GLDMETHOD(void, loadFromStream)(GStream *stream) PURE;
    GLDMETHOD(void, saveToStream)(GStream *stream) PURE;

    GLDMETHOD(gint64, asInt64)() PURE;
    GLDMETHOD(void, setAsInt64)(gint64) PURE;

    /*Qt 兼容*/
    GLDMETHOD(IGLDXMLNode*, replaceChild)(IGLDXMLNode* newChild, IGLDXMLNode* oldChild) PURE;
    GLDMETHOD (IGLDXMLNode*, insertAfter)(IGLDXMLNode* newChild, IGLDXMLNode* refChild) PURE;
    GLDMETHOD(IGLDXMLNode *, firstChildElement)(const GString &tagName = GString()) const PURE;
    GLDMETHOD(IGLDXMLNode *, insertBefore)(IGLDXMLNode *newChild, IGLDXMLNode *refChild) PURE;
    GLDMETHOD(IGLDXMLNodeAttribute *, attributeNode)(const QString &name) PURE;
    GLDMETHOD(GString, nodeName)() PURE;
    GLDMETHOD(IGLDXMLNode *, nextSiblingElement)(const GString &value) PURE;
    GLDMETHOD(IGLDXMLNode *, appendChild)(IGLDXMLNode *value) PURE;
    GLDMETHOD(IGLDXMLNode *, removeChild)(IGLDXMLNode *value) PURE;
    GLDMETHOD(void, setTagName)(const GString &value) PURE;
    GLDMETHOD(GString, tagName)() PURE;
    GLDMETHOD(IGLDXMLNodeList *, elementsByTagName)(const GString &value) PURE;
    GLDMETHOD(IGLDXMLNode *, toText)() PURE;
    GLDMETHOD(GString, data)() PURE;
    GLDMETHOD(void, setAttribute)(const GString &name, const GString &value) PURE;
    GLDMETHOD(void, setAttribute)(const GString &name, int value) PURE;
    GLDMETHOD(GString, attribute)(const GString &name, const GString &defValue) PURE;
    GLDMETHOD(void, clear)() PURE;
    GLDMETHOD(GString, nodeValue)() const PURE;
    GLDMETHOD(IGLDXMLDocument*, ownerDocument)() const PURE;
    GLDMETHOD(IGLDXMLNode*, toElement)() const PURE;
    GLDMETHOD(bool, isElement)() const PURE;
    GLDMETHOD(bool, isNull)() const PURE;
    GLDMETHOD(IGLDXMLNode*, parentNode)() const PURE;
    GLDMETHOD(void, setNodeValue)(const GString &value) PURE;
    GLDMETHOD(IGLDXMLNode*, firstChild)()const PURE;
    GLDMETHOD(IGLDXMLNode*, lastChild)()const PURE;
    GLDMETHOD(void, removeAttribute)(const GString &name) PURE;
    GLDMETHOD(IGLDXMLNodeAttribute*, setAttributeNode)(IGLDXMLNodeAttribute* newAttr) PURE;
    GLDMETHOD(IGLDXMLNode *, namedItem)(const GString &name) const PURE;
};

DEFINE_IID(IGLDXMLNodeList, "9152F99B-EA65-480A-98F2-FCFF212E0F23");
DECLARE_INTERFACE_(IGLDXMLNodeList, IUnknown)
{
    GLDMETHOD(int, add)(IGLDXMLNode *node) PURE;
    GLDMETHOD(int, deleteNode)(const GString &name) PURE;// deleteNode -> Delete
    GLDMETHOD(void, deleteNode)(int index) PURE;
    GLDMETHOD(IGLDXMLNode *, findNode)(const GString &name) PURE;// strName -> AName
    GLDMETHOD(int, indexOf)(IGLDXMLNode *node) const PURE;
    GLDMETHOD(int, indexOf)(const GString &name) const PURE;
    GLDMETHOD(int, remove)(IGLDXMLNode *node) PURE;
    GLDMETHOD(void, clear)() PURE;
    GLDMETHOD(void, insert)(int nIndex, IGLDXMLNode *node) PURE;
    GLDMETHOD(void, move)(int currIndex, int newIndex) PURE;
    GLDMETHOD(void, exChange)(int index1, int index2) PURE;

    GLDMETHOD(int, count)()const PURE;// read count;
    GLDMETHOD(IGLDXMLNode *, node)(int index) PURE;// read nodes;
    GLDMETHOD(IGLDXMLNode *, nodeByName)(const GString &name) PURE;// read byNameNode;

    /*Qt 兼容*/
    GLDMETHOD(IGLDXMLNode *, at)(int index) PURE;
    GLDMETHOD(IGLDXMLNode *, item)(int index) PURE;
};

DEFINE_IID(IGLDXMLDocument, "A773026D-A728-46A8-A181-372D49FF00CE");
DECLARE_INTERFACE_(IGLDXMLDocument, IUnknown)
{
    GLDMETHOD(IGLDXMLNode*, createElement)(const GString &name) PURE;
    GLDMETHOD(IGLDXMLNode*, createNode)(const GString &name, EnXMLNodeType type = ELEMENT) PURE;
    GLDMETHOD(bool, autoIndent)() PURE;
    GLDMETHOD(GString, displayText)() PURE;
    GLDMETHOD(IGLDXMLNode*, documentElement)() const PURE;

    GLDMETHOD(GString, encoding)() PURE;
    GLDMETHOD(GString, fileName)() PURE;
    GLDMETHOD(bool, includeHeader)() PURE;
    GLDMETHOD(EnTXMLOptions, options)() PURE;
    GLDMETHOD(GString, version)() PURE;
    GLDMETHOD(GString, xml)() PURE;

    GLDMETHOD(bool, isEmpty)() PURE;
    GLDMETHOD(void, loadFromFile)(const GString &fileName) PURE;
    GLDMETHOD(void, loadFromStream)(GStream *stream) PURE;
    GLDMETHOD(void, saveToFile)(const GString &fileName) PURE;
    GLDMETHOD(void, saveToStream)(GStream *stream) PURE;
    GLDMETHOD(void, setAutoIndent)(bool value) PURE;
    GLDMETHOD(void, setEncoding)(const GString &value) PURE;
    GLDMETHOD(void, setFileName)(const GString &value) PURE;
    GLDMETHOD(void, setIncludeHeader)(bool value) PURE;
    GLDMETHOD(void, setOptions)(EnTXMLOptions xmlOptionsSet) PURE;
    GLDMETHOD(void, setVersion)(const GString &value) PURE;
    GLDMETHOD(void, setXML)(const GString &value) PURE;

    /*Qt 兼容*/
    GLDMETHOD(IGLDXMLNode*, appendChild)(IGLDXMLNode *newChild) PURE;
    GLDMETHOD(IGLDXMLNode*, firstChild)() const PURE;
    GLDMETHOD(void, save)(GStream *stream, int indent) const PURE;
    GLDMETHOD(bool, setContent)(GStream* dev, GString *errorMsg=0, int *errorLine=0, int *errorColumn=0 ) PURE;
    GLDMETHOD(bool, setContent)(const GString& text, bool namespaceProcessing,
                                GString *errorMsg=0, int *errorLine=0, int *errorColumn=0 ) PURE;
    GLDMETHOD(bool, setContent)(const QString& text, QString *errorMsg=0, int *errorLine=0, int *errorColumn=0 ) PURE;
    GLDMETHOD(bool, setContent)(const QByteArray& text, QString *errorMsg=0, int *errorLine=0, int *errorColumn=0 ) PURE;
    GLDMETHOD(IGLDXMLNodeAttribute*, createAttribute)(const QString& name) PURE;
    GLDMETHOD(IGLDXMLNode*, createTextNode)(const GString& data) PURE;
    GLDMETHOD(IGLDXMLNodeList*, childNodes)() const PURE;
    GLDMETHOD(void, clear)() PURE;

    GLDMETHOD(IGLDXMLNode *, root)() PURE;
    GLDMETHOD(void, setRoot)(IGLDXMLNode *root) PURE;
    GLDMETHOD(GString, toString)(int indent = 1) const PURE;
    GLDMETHOD(IGLDXMLNode*, toElement)()const PURE;
    GLDMETHOD(IGLDXMLNode*, elementById)(const GString &elementID) PURE;
    GLDMETHOD(IGLDXMLNode*, firstChildElement)(const GString &tagName) const PURE;
    GLDMETHOD(bool, isNull)() const PURE;

};

DEFINE_IID(IGLDXMLNodeAttribute, "59939529-966F-45C9-B581-19EEF1D1FEDE");
DECLARE_INTERFACE_(IGLDXMLNodeAttribute, IUnknown)
{
    GLDMETHOD(bool, asBoolean)() PURE;
    GLDMETHOD(double, asFloat)() PURE;
    GLDMETHOD(int, asInteger)() PURE;
    GLDMETHOD(GString, asString)() PURE;
    GLDMETHOD(GVariant, asVariant)() PURE;
    GLDMETHOD(int, hashData)() PURE;
    GLDMETHOD(GString, name)() PURE;
    GLDMETHOD(GString, text)() PURE;
    GLDMETHOD(GString, xml)()PURE;

    GLDMETHOD(void, setAsBoolean)(bool value) PURE;
    GLDMETHOD(void, setAsFloat)(double value) PURE;
    GLDMETHOD(void, setAsInteger)(int value) PURE;
    GLDMETHOD(void, setAsString)(const GString &value) PURE;
    GLDMETHOD(void, setAsVariant)(const GVariant value) PURE;
    GLDMETHOD(void, setName)(const GString &strName) PURE;
    GLDMETHOD(void, setText)(const GString &strText) PURE;
    GLDMETHOD(void, setXml)(const GString &strXML) PURE;
    GLDMETHOD(gint64, asInt64)() PURE;
    GLDMETHOD(void, setAsInt64)(gint64 value) PURE;

    /*Qt 兼容*/
    GLDMETHOD(GString, value)() PURE;
    GLDMETHOD(IGLDXMLNodeAttribute*, toAttr)() PURE;
    GLDMETHOD(GString, nodeName)() const PURE;

    GLDMETHOD(GString, nodeValue)()PURE;
    GLDMETHOD(void, setNodeValue)(const GString &value) PURE;
};

DEFINE_IID(IGLDXMLNodeAttributes, "6874C84B-2E07-45CA-B463-B03CF031FD8D");
DECLARE_INTERFACE_(IGLDXMLNodeAttributes, IUnknown)
{
    GLDMETHOD(IGLDXMLNodeAttribute*, add)(const GString &strName) PURE;
    GLDMETHOD(int, indexOf)(const GString &strName) PURE;
    GLDMETHOD(void, clear)() PURE;
    GLDMETHOD(void, remove)(int nIndex) PURE;

    GLDMETHOD(int, count)() PURE;
    GLDMETHOD(IGLDXMLNodeAttribute*, item)(int nIndex) PURE;
    GLDMETHOD(IGLDXMLNodeAttribute*, itemsByName)(const GString &strName) PURE;
    GLDMETHOD(void, setXml)(const GString &value) PURE;
    GLDMETHOD(GString, xml)() PURE;

    /*Qt 兼容*/
    GLDMETHOD(bool, contains)(const GString &name) PURE;
    GLDMETHOD(IGLDXMLNodeAttribute*, namedItem)(const GString &name) PURE;
};

DEFINE_IID(IGLDXMLJSONPersistent, "B790D662-8515-4FEB-BE01-B7ADFCB9B5AC");
DECLARE_INTERFACE_(IGLDXMLJSONPersistent, IUnknown)
{
    GLDMETHOD(GString, asJSON)() PURE; //读m_asJSON
    GLDMETHOD(void, setAsJSON)(const GString &value) PURE;//写m_asJSON
};

DEFINE_IID(IGLDXMLJSONPersistentFile, "30DC536F-72D5-4359-A09E-ED4A2E26EFD6");
DECLARE_INTERFACE_(IGLDXMLJSONPersistentFile, IUnknown)
{
    GLDMETHOD(void, loadFromJSONFile)(const GString &fileName) PURE;
    GLDMETHOD(void, loadFromJSONStream)(GStream *stream) PURE;
    GLDMETHOD(void, saveAsJSONToFile)(const GString &fileName) PURE;
    GLDMETHOD(void, saveAsJSONToStream)(GStream *stream) PURE;

};
#endif // GLDXMLINTERFACE_H
