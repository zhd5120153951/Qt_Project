#include "GLDXMLCore.h"
#include "GLDXMLDoc.h"

/*GLDXMLDocument*/
GLDXMLDocument::GLDXMLDocument() : GComPtr<IGLDXMLDocument>()
{
}

GLDXMLNode GLDXMLDocument::createElement(const GString &name)
{
    if (NULL == p)
    {
        innerCreateXMLDocument();
    }
    return GLDXMLNode(p->createElement(name), false);
}

GLDXMLNode GLDXMLDocument::createNode(const GString &name, EnXMLNodeType type)
{
    if (NULL == p)
    {
        innerCreateXMLDocument();
    }
    return GLDXMLNode(p->createNode(name, type), false);
}

bool GLDXMLDocument::isEmpty()
{
    if (NULL == p)
    {
        return true;
    }
    return p->isEmpty();
}

void GLDXMLDocument::loadFromFile(const GString &fileName)
{
    if (NULL == p)
    {
        innerCreateXMLDocument();
    }
    p->loadFromFile(fileName);
}

void GLDXMLDocument::saveToFile(const GString &fileName)
{
    assert(p != NULL);
    p->saveToFile(fileName);
}

void GLDXMLDocument::loadFromStream(GStream *stream)
{
    if (NULL == p)
    {
        innerCreateXMLDocument();
    }
    p->loadFromStream(stream);
}

void GLDXMLDocument::saveToStream(GStream *stream)
{
    assert(p != NULL);
    p->saveToStream(stream);
}

bool GLDXMLDocument::autoIndent()
{
    return p->autoIndent();
}

void GLDXMLDocument::setAutoIndent(bool value)
{
    p->setAutoIndent(value);
}

GString GLDXMLDocument::displayText()
{
    return p->displayText();
}

GLDXMLNode GLDXMLDocument::documentElement() const
{
    if (NULL == p)
    {
        return NULL;
    }
    return GLDXMLNode(p->documentElement(), false);
}

GString GLDXMLDocument::encoding()
{
    return p->encoding();
}

void GLDXMLDocument::setEncoding(const GString &value)
{
    p->setEncoding(value);
}

GString GLDXMLDocument::fileName()
{
    return p->fileName();
}

void GLDXMLDocument::setFileName(const GString &value)
{
    p->setFileName(value);
}

bool GLDXMLDocument::includeHeader()
{
    return p->includeHeader();
}

void GLDXMLDocument::setIncludeHeader(bool value)
{
    p->setIncludeHeader(value);
}

EnTXMLOptions GLDXMLDocument::options()
{
    return p->options();
}

void GLDXMLDocument::setOptions(EnTXMLOptions xmlOptionsSet)
{
    p->setOptions(xmlOptionsSet);
}

GString GLDXMLDocument::version()
{
    return p->version();
}

void GLDXMLDocument::setVersion(const GString &value)
{
    p->setVersion(value);
}

GString GLDXMLDocument::xml()
{
    return p->xml();
}

void GLDXMLDocument::setXML(const GString &value)
{
    if (NULL == p)
    {
        innerCreateXMLDocument();
    }
    p->setXML(value);
}

GLDXMLNode GLDXMLDocument::root()
{
    return GLDXMLNode(p->root(), false);
}

void GLDXMLDocument::setRoot(const GLDXMLNode &root)
{
    if (NULL == p)
    {
        innerCreateXMLDocument();
    }
    p->setRoot(root);
}

GLDXMLNode GLDXMLDocument::appendChild(GLDXMLNode newChild)
{
    if (NULL == p)
    {
        return GLDXMLNode();
    }
    return GLDXMLNode(p->appendChild(newChild), false);
}

bool GLDXMLDocument::setContent(GStream *dev, GString *errorMsg, int *errorLine, int *errorColumn)
{
    if (NULL == p)
    {
        innerCreateXMLDocument();
    }
    return p->setContent(dev, errorMsg, errorLine, errorColumn);
}

bool GLDXMLDocument::setContent(const GString &text, bool namespaceProcessing, GString *errorMsg, int *errorLine, int *errorColumn)
{
    G_UNUSED(errorLine);
    if (NULL == p)
    {
        innerCreateXMLDocument();
    }
    return p->setContent(text, namespaceProcessing, errorMsg, errorColumn);
}

bool GLDXMLDocument::setContent(const GString &text, GString *errorMsg, int *errorLine, int *errorColumn)
{
    if (NULL == p)
    {
        innerCreateXMLDocument();
    }
    return p->setContent(text, errorMsg, errorLine, errorColumn);
}

bool GLDXMLDocument::setContent(const QByteArray &text, GString *errorMsg, int *errorLine, int *errorColumn)
{
    if (NULL == p)
    {
        innerCreateXMLDocument();
    }
    return p->setContent(text, errorMsg, errorLine, errorColumn);
}

GLDXMLNode GLDXMLDocument::firstChild() const
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->firstChild(), false);
}

void GLDXMLDocument::save(GStream *stream, int indent)
{
    if (NULL == p)
        return;
    return p->save(stream, indent);
}

GString GLDXMLDocument::toString(int indent) const
{
    if (NULL == p)
        return GString();
    return p->toString(indent);
}

GLDXMLNode GLDXMLDocument::elementById(const GString &elementId)
{
    return GLDXMLNode();
    G_UNUSED(elementId);
}

GLDXMLNodeAttribute GLDXMLDocument::createAttribute(const GString &name)
{
    if (NULL == p)
        innerCreateXMLDocument();
    return GLDXMLNodeAttribute(p->createAttribute(name), false);
}

GLDXMLNode GLDXMLDocument::firstChildElement(const GString &tagName) const
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->firstChildElement(tagName), false);
}

GLDXMLNode GLDXMLDocument::createTextNode(const GString &data)
{
    if (NULL == p)
        innerCreateXMLDocument();
    return GLDXMLNode(p->createTextNode(data), false);
}

GLDXMLNodeList GLDXMLDocument::childNodes() const
{
    if (NULL == p)
        return GLDXMLNodeList();
    return GLDXMLNodeList(p->childNodes(), false);
}

void GLDXMLDocument::clear()
{
    if (p != NULL)
    {
        freeAndNilIntf(p);
    }
}

GLDXMLNode GLDXMLDocument::toElement() const
{
    if (NULL == p)
        return NULL;
    return GLDXMLNode(p->toElement(), false);
}

bool GLDXMLDocument::isNull() const
{
    return p == NULL;
}

void GLDXMLDocument::innerCreateXMLDocument()
{
    CGLDXMLDocument *xmlDocument = new CGLDXMLDocument;
    xmlDocument->setAutoIndent(true);
    xmlDocument->setIncludeHeader(true);
    p = xmlDocument;
    p->AddRef();
}

/* GLDXMLNode */
GLDXMLNode GLDXMLNode::addChild(const GString &strName)
{
    return GLDXMLNode(p->addChild(strName), false);
}

GLDXMLNode GLDXMLNode::cloneNode(bool deep)
{
    return GLDXMLNode(p->cloneNode(deep), false);
}

bool GLDXMLNode::hasAttribute(const GString &strName) const
{
    return p->hasAttribute(strName);
}

GLDXMLNode GLDXMLNode::nextSibling()
{
    return GLDXMLNode(p->nextSibling(), false);
}

GLDXMLNode GLDXMLNode::prevSibling()
{
    return GLDXMLNode(p->prevSibling(), false);
}

bool GLDXMLNode::asBoolean()
{
    return p->asBoolean();
}

double GLDXMLNode::asFloat()
{
    return p->asFloat();
}

gint64 GLDXMLNode::asInt64()
{
    return p->asInt64();
}

int GLDXMLNode::asInteger()
{
    return p->asInteger();
}

GString GLDXMLNode::asString()
{
    return p->asString();
}

GVariant GLDXMLNode::asVariant()
{
    return p->asVariant();
}

GLDXMLNodeAttributes GLDXMLNode::attributes() const
{
    if (NULL == p)
    {
        return GLDXMLNodeAttributes();
    }
    return GLDXMLNodeAttributes(p->attributes(), false);
}

GLDXMLNodeList GLDXMLNode::childNodes() const
{
    if (NULL == p)
    {
        return GLDXMLNodeList();
    }
    return GLDXMLNodeList(p->childNodes(), false);
}

int GLDXMLNode::level()
{
    return p->level();
}

GString GLDXMLNode::name()
{
    return p->name();
}

EnXMLNodeType GLDXMLNode::nodeType()
{
    return p->nodeType();
}

GLDXMLNode GLDXMLNode::parent() const
{
    return GLDXMLNode(p->parent(), false);
}

GString GLDXMLNode::text() const
{
    return p->asString();
}

GString GLDXMLNode::xml()
{
    return p->xml();
}

void GLDXMLNode::setAsBoolean(bool value)
{
    p->setAsBoolean(value);
}

void GLDXMLNode::setAsFloat(float value)
{
    p->setAsFloat(value);
}

void GLDXMLNode::setAsInt64(gint64 value)
{
    p->setAsInt64(value);
}

void GLDXMLNode::setAsInteger(int value)
{
    p->setAsInteger(value);
}

void GLDXMLNode::setAsString(const GString &value)
{
    p->setAsString(value);
}

void GLDXMLNode::setAsVariant(const GVariant value)
{
    p->setAsVariant(value);
}

void GLDXMLNode::setName(const GString &value)
{
    p->setName(value);
}

void GLDXMLNode::setNodeType(const EnXMLNodeType value)
{
    p->setNodeType(value);
}

void GLDXMLNode::setParent(const GLDXMLNode &parent)
{
    p->setParent(parent);
}

void GLDXMLNode::setText(const GString &value)
{
    p->setText(value);
}

void GLDXMLNode::setXML(const GString &value)
{
    p->setXML(value);
}

void GLDXMLNode::loadFromStream(GStream *pStream)
{
    p->loadFromStream(pStream);
}

void GLDXMLNode::saveToStream(GStream *pStream)
{
    p->saveToStream(pStream);
}

GLDXMLNode GLDXMLNode::firstChildElement(const GString &tagName) const
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->firstChildElement(tagName), false);
}

GString GLDXMLNode::nodeName() const
{
    if (NULL == p)
        return GString();
    return p->nodeName();
}

GLDXMLNode GLDXMLNode::nextSiblingElement(const GString &taName) const
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->nextSiblingElement(taName), false);
}

GLDXMLDocument GLDXMLNode::ownerDocument() const
{
    if (NULL == p)
        return GLDXMLDocument();
    return GLDXMLDocument(p->ownerDocument(), false);
}

GLDXMLNode GLDXMLNode::appendChild(const GLDXMLNode &newChild)
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->appendChild(newChild), false);
}

GString GLDXMLNode::attribute(const GString &name, const GString &defValue) const
{
    if (NULL == p)
        return GString();
    return p->attribute(name, defValue);
}

void GLDXMLNode::setAttribute(const GString &name, const GString &value)
{
    return p->setAttribute(name, value);
}

void GLDXMLNode::setAttribute(const GString &name, int value)
{
    return p->setAttribute(name, value);
}

GLDXMLNode GLDXMLNode::insertBefore(const GLDXMLNode &newChild, const GLDXMLNode &refChild)
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->insertBefore(newChild, refChild), false);
}

GLDXMLNode GLDXMLNode::removeChild(const GLDXMLNode &oldChild)
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->removeChild(oldChild), false);
}

bool GLDXMLNode::hasChildNodes() const
{
    if (NULL == p)
        return false;
    return p->hasChildNodes();
}

GLDXMLNode GLDXMLNode::parentNode() const
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->parentNode(), false);
}

void GLDXMLNode::setNodeValue(const GString &value)
{
    if (NULL == p)
        return;
    p->setNodeValue(value);
}

GLDXMLNodeAttribute GLDXMLNode::attributeNode(const GString &name)
{
    if (NULL == p)
        return GLDXMLNodeAttribute();
    return GLDXMLNodeAttribute(p->attributeNode(name), false);
}

void GLDXMLNode::clear()
{
    if (p != NULL)
    {
        freeAndNilIntf(p);
    }
}

void GLDXMLNode::setTagName(const GString &name)
{
    if (p != NULL)
    {
        p->setTagName(name);
    }
}

GString GLDXMLNode::tagName() const
{
    if (NULL == p)
        return GString();
    return p->tagName();
}

GLDXMLNode GLDXMLNode::toElement() const
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->toElement(), false);
}

GLDXMLNodeList GLDXMLNode::elementsByTagName(const GString &tagname) const
{
    if (NULL == p)
    {
        return GLDXMLNodeList();
    }
    return GLDXMLNodeList(p->elementsByTagName(tagname), false);
}

GLDXMLNode GLDXMLNode::firstChild() const
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->firstChild(), false);
}

GString GLDXMLNode::nodeValue() const
{
    if (NULL == p)
        return GString();
    return p->nodeValue();
}

GLDXMLNode GLDXMLNode::lastChild() const
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->lastChild(), false);
}

GLDXMLNode GLDXMLNode::toText() const
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->toText(), false);
}

GString GLDXMLNode::data() const
{
    if (NULL == p)
        return GString();
    return p->data();
}

void GLDXMLNode::removeAttribute(const GString &name)
{
    if (NULL == p)
        return;
    p->removeAttribute(name);
}

GLDXMLNodeAttribute GLDXMLNode::setAttributeNode(GLDXMLNodeAttribute &newAttr)
{
    if (NULL == p)
        return GLDXMLNodeAttribute();
    return GLDXMLNodeAttribute(p->setAttributeNode(newAttr), false);
}

bool GLDXMLNode::isNull() const
{
    return p == NULL;
}

bool GLDXMLNode::isElement() const
{
    if (NULL == p)
        return false;
    return p->isElement();
}

GLDXMLNode GLDXMLNode::replaceChild(const GLDXMLNode &newChild, const GLDXMLNode &oldChild)
{
    if (NULL == p || NULL == newChild.p || NULL == oldChild.p)
        return GLDXMLNode();
    return GLDXMLNode(p->replaceChild(newChild, oldChild), false);
}

GLDXMLNode GLDXMLNode::insertAfter(const GLDXMLNode &newChild, const GLDXMLNode &refChild)
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->insertAfter(newChild, refChild), false);
}

GLDXMLNode GLDXMLNode::namedItem(const GString &name) const
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->namedItem(name), false);
}

/* GLDXMLNodeList */
int GLDXMLNodeList::add(const GLDXMLNode &node)
{
    return p->add(node);
}

int GLDXMLNodeList::deleteNode(const GString &name)
{
    return p->deleteNode(name);
}

void GLDXMLNodeList::deleteNode(int index)
{
    p->deleteNode(index);
}

int GLDXMLNodeList::indexOf(const GLDXMLNode &node)
{
    return p->indexOf(node);
}

int GLDXMLNodeList::indexOf(const GString &name)
{
    return p->indexOf(name);
}

int GLDXMLNodeList::remove(const GLDXMLNode &node)
{
    return p->remove(node);
}

void GLDXMLNodeList::clear()
{
    p->clear();
}

void GLDXMLNodeList::exChange(int cureIndex, int newIndex)
{
    p->exChange(cureIndex, newIndex);
}

void GLDXMLNodeList::insert(int index, const GLDXMLNode &node)
{
    p->insert(index, node);
}

void GLDXMLNodeList::move(int cureIndex, int newIndex)
{
    p->move(cureIndex, newIndex);
}

GLDXMLNode GLDXMLNodeList::findNode(const GString &name)
{
    return GLDXMLNode(p->findNode(name), false);
}

int GLDXMLNodeList::count() const
{
    if (NULL == p)
    {
        return 0;
    }
    return p->count();
}

GLDXMLNode GLDXMLNodeList::node(int index)
{
    return GLDXMLNode(p->node(index), false);
}

GLDXMLNode GLDXMLNodeList::at(int index) const
{
    return GLDXMLNode(p->node(index), false);
}

GLDXMLNode GLDXMLNodeList::item(int index)
{
    if (NULL == p)
        return GLDXMLNode();
    return GLDXMLNode(p->item(index), false);
}

GLDXMLNode GLDXMLNodeList::nodeByName(const GString &name)
{
    return GLDXMLNode(p->nodeByName(name), false);
}

/*GLDXMLNodeAttribute*/
bool GLDXMLNodeAttribute::asBoolean()
{
    return p->asBoolean();
}

double GLDXMLNodeAttribute::asFloat()
{
    return p->asFloat();
}

gint64 GLDXMLNodeAttribute::asInt64()
{
    return p->asInt64();
}

int GLDXMLNodeAttribute::asInteger()
{
    return p->asInteger();
}

GString GLDXMLNodeAttribute::asString()
{
    return p->asString();
}

GVariant GLDXMLNodeAttribute::asVariant()
{
    return p->asVariant();
}

int GLDXMLNodeAttribute::hashData()
{
    return p->hashData();
}

GString GLDXMLNodeAttribute::name()
{
    return p->name();
}

GString GLDXMLNodeAttribute::text()
{
    return p->text();
}

GString GLDXMLNodeAttribute::xml()
{
    return p->xml();
}

void GLDXMLNodeAttribute::setAsBoolean(bool value)
{
    p->setAsBoolean(value);
}

void GLDXMLNodeAttribute::setAsFloat(double value)
{
    p->setAsFloat(value);
}

void GLDXMLNodeAttribute::setAsInt64(gint64 value)
{
    p->setAsInt64(value);
}

void GLDXMLNodeAttribute::setAsInteger(int value)
{
    p->setAsInteger(value);
}

void GLDXMLNodeAttribute::setAsString(const GString &value)
{
    p->setAsString(value);
}

void GLDXMLNodeAttribute::setAsVariant(const GVariant value)
{
    p->setAsVariant(value);
}

void GLDXMLNodeAttribute::setName(const GString &strName)
{
    p->setName(strName);
}

void GLDXMLNodeAttribute::setText(const GString &strText)
{
    p->setText(strText);
}

void GLDXMLNodeAttribute::setXml(const GString &strXML)
{
    p->setXml(strXML);
}

void GLDXMLNodeAttribute::setNodeValue(const GString &value)
{
    if (NULL == p)
        return;
    p->setText(value);
}

GString GLDXMLNodeAttribute::nodeValue() const
{
    if (NULL == p)
        return GString();
   return p->nodeValue();
}

GLDXMLNodeAttribute GLDXMLNodeAttribute::toAttr() const
{
    if (NULL == p)
        return GLDXMLNodeAttribute();
    return GLDXMLNodeAttribute(p->toAttr(), false);
}

GString GLDXMLNodeAttribute::value() const
{
    if (NULL == p)
        return GString();
    return p->value();
}

GString GLDXMLNodeAttribute::nodeName() const
{
    if (NULL == p)
        return GString();
    return p->nodeName();
}

/*GLDXMLNodeAttributes*/
void GLDXMLNodeAttributes::clear()
{
    p->clear();
}

void GLDXMLNodeAttributes::remove(int nIndex)
{
    p->remove(nIndex);
}

void GLDXMLNodeAttributes::setXml(const GString &value)
{
    p->setXml(value);
}

int GLDXMLNodeAttributes::count()
{
    if (NULL == p)
    {
        return 0;
    }
    return p->count();
}

int GLDXMLNodeAttributes::indexOf(const GString &strName)
{
    return p->indexOf(strName);
}

GString GLDXMLNodeAttributes::xml()
{
    return p->xml();
}

GLDXMLNodeAttribute GLDXMLNodeAttributes::add(const GString &strName)
{
    return GLDXMLNodeAttribute(p->add(strName), false);
}

GLDXMLNodeAttribute GLDXMLNodeAttributes::item(int nIndex)
{
    return GLDXMLNodeAttribute(p->item(nIndex), false);
}

GLDXMLNodeAttribute GLDXMLNodeAttributes::itemsByName(const GString &strName)
{
    return GLDXMLNodeAttribute(p->itemsByName(strName), false);
}

GLDXMLNodeAttribute GLDXMLNodeAttributes::namedItem(const GString &name) const
{
    if (NULL == p)
        return GLDXMLNodeAttribute();
    return GLDXMLNodeAttribute(p->namedItem(name), false);
}

bool GLDXMLNodeAttributes::contains(const GString &name)
{
    if (NULL == p)
        return false;
    return p->contains(name);
}
