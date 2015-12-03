#ifndef GLDXMLUTILS_H
#define GLDXMLUTILS_H

#include "GLDXML_Global.h"
#include "GLDString.h"
#include "GLDVariantList.h"
#include "GLDXMLTypes.h"
#include "GLDXMLCore.h"

//namespace GlodonXMLUtils;
// 该单元所提供的方法，GXMLNode目前通过typedef定义，实际上位Qt的xml解析方法
// GLDXMLNode为与delphi一致的解析方式，以此类推方法名和类名

GLDXMLSHARED_EXPORT GXMLNode firstChildNode(const GXMLNode &node, const QString &tagName = QString());
GLDXMLSHARED_EXPORT GXMLNode findChildNode(const GXMLNode &node, const GString &nodeName);
GLDXMLSHARED_EXPORT GXMLNode addChild(GXMLNode &node, const GString &nodeName);

GLDXMLSHARED_EXPORT GString readStrFromXML(const GXMLNode &node, const GString &nodeName, const GString &defVal = "");
GLDXMLSHARED_EXPORT bool readBoolFromXML(const GXMLNode &node, const GString &nodeName, bool defVal = false);
GLDXMLSHARED_EXPORT int readIntFromXML(const GXMLNode &node, const GString &nodeName, int defVal = 0);
GLDXMLSHARED_EXPORT long long readInt64FromXML(const GXMLNode &node, const GString &nodeName, long long defVal = 0);
GLDXMLSHARED_EXPORT guint64 readUInt64FromXML(const GXMLNode &node, const GString &nodeName, guint64 defVal = 0);
GLDXMLSHARED_EXPORT double readFloatFromXML(const GXMLNode &node, const GString &nodeName, double defVal = 0);

GLDXMLSHARED_EXPORT GString readStrFromXMLAttr(const GXMLNode &node, const GString &attrName, const GString &defVal = "");
GLDXMLSHARED_EXPORT bool readBoolFromXMLAttr(const GXMLNode &node, const GString &attrName, bool defVal = false);
GLDXMLSHARED_EXPORT int readIntFromXMLAttr(const GXMLNode &node, const GString &attrName, int defVal = 0);
GLDXMLSHARED_EXPORT gint64 readInt64FromXMLAttr(const GXMLNode &node, const GString &attrName, gint64 defVal = 0);
GLDXMLSHARED_EXPORT guint64 readUInt64FromXMLAttr(const GXMLNode &node, const GString &attrName, guint64 defVal = 0);
GLDXMLSHARED_EXPORT double readFloatFromXMLAttr(const GXMLNode &node, const GString &attrName, double defVal = 0);

GLDXMLSHARED_EXPORT void writeStrToXML(GXMLNode &node, const GString &nodeName, const GString &value, const GString &defVal = "");
GLDXMLSHARED_EXPORT void writeBoolToXML(GXMLNode &node, const GString &nodeName, bool value, bool defVal = false);
GLDXMLSHARED_EXPORT void writeIntToXML(GXMLNode &node, const GString &nodeName, int value, int defVal = 0);
GLDXMLSHARED_EXPORT void writeInt64ToXML(GXMLNode &node, const GString &attrNodeName, long long value, long long defVal = 0);
GLDXMLSHARED_EXPORT void writeUInt64ToXML(GXMLNode &node, const GString &attrNodeName, guint64 value, guint64 defVal = 0);
GLDXMLSHARED_EXPORT void writeFloatToXML(GXMLNode &node, const GString &nodeName, double value, double defVal = 0);

GLDXMLSHARED_EXPORT void writeStrToXMLAttr(GXMLNode &node, const GString &attrName, const GString &value, const GString &defVal = "");
GLDXMLSHARED_EXPORT void writeBoolToXMLAttr(GXMLNode &node, const GString &attrName, bool value, bool defVal = false);
GLDXMLSHARED_EXPORT void writeIntToXMLAttr(GXMLNode &node, const GString &attrName, int value, int defVal = 0);
GLDXMLSHARED_EXPORT void writeUIntToXMLAttr(GXMLNode &node, const GString &attrName, uint value, uint defVal = 0);
GLDXMLSHARED_EXPORT void writeFloatToXMLAttr(GXMLNode &node, const GString &attrName, double value, double defVal = 0);

GLDXMLSHARED_EXPORT GLDXMLNode firstChildNode(const GLDXMLNode &node, const QString &tagName = QString());
GLDXMLSHARED_EXPORT GLDXMLNode findChildNode(const GLDXMLNode &node, const GString &nodeName);
GLDXMLSHARED_EXPORT GLDXMLNode addChild(GLDXMLNode &node, const GString &nodeName);

GLDXMLSHARED_EXPORT GString readStrFromXML(const GLDXMLNode &node, const GString &nodeName, const GString &defVal = "");
GLDXMLSHARED_EXPORT bool readBoolFromXML(const GLDXMLNode &node, const GString &nodeName, bool defVal = false);
GLDXMLSHARED_EXPORT int readIntFromXML(const GLDXMLNode &node, const GString &nodeName, int defVal = 0);
GLDXMLSHARED_EXPORT long long readInt64FromXML(const GLDXMLNode &node, const GString &nodeName, long long defVal = 0);
GLDXMLSHARED_EXPORT guint64 readUInt64FromXML(const GLDXMLNode &node, const GString &nodeName, guint64 defVal = 0);
GLDXMLSHARED_EXPORT double readFloatFromXML(const GLDXMLNode &node, const GString &nodeName, double defVal = 0);

GLDXMLSHARED_EXPORT GString readStrFromXMLAttr(const GLDXMLNode &node, const GString &attrName, const GString &defVal = "");
GLDXMLSHARED_EXPORT bool readBoolFromXMLAttr(const GLDXMLNode &node, const GString &attrName, bool defVal = false);
GLDXMLSHARED_EXPORT int readIntFromXMLAttr(const GLDXMLNode &node, const GString &attrName, int defVal = 0);
GLDXMLSHARED_EXPORT gint64 readInt64FromXMLAttr(const GLDXMLNode &node, const GString &attrName, gint64 defVal = 0);
GLDXMLSHARED_EXPORT guint64 readUInt64FromXMLAttr(const GLDXMLNode &node, const GString &attrName, guint64 defVal = 0);
GLDXMLSHARED_EXPORT double readFloatFromXMLAttr(const GLDXMLNode &node, const GString &attrName, double defVal = 0);

GLDXMLSHARED_EXPORT void writeStrToXML(GLDXMLNode &node, const GString &nodeName, const GString &value, const GString &defVal = "");
GLDXMLSHARED_EXPORT void writeBoolToXML(GLDXMLNode &node, const GString &nodeName, bool value, bool defVal = false);
GLDXMLSHARED_EXPORT void writeIntToXML(GLDXMLNode &node, const GString &nodeName, int value, int defVal = 0);
GLDXMLSHARED_EXPORT void writeInt64ToXML(GLDXMLNode &node, const GString &attrNodeName, long long value, long long defVal = 0);
GLDXMLSHARED_EXPORT void writeUInt64ToXML(GLDXMLNode &node, const GString &attrNodeName, guint64 value, guint64 defVal = 0);
GLDXMLSHARED_EXPORT void writeFloatToXML(GLDXMLNode &node, const GString &nodeName, double value, double defVal = 0);

GLDXMLSHARED_EXPORT void writeStrToXMLAttr(GLDXMLNode &node, const GString &attrName, const GString &value, const GString &defVal = "");
GLDXMLSHARED_EXPORT void writeBoolToXMLAttr(GLDXMLNode &node, const GString &attrName, bool value, bool defVal = false);
GLDXMLSHARED_EXPORT void writeIntToXMLAttr(GLDXMLNode &node, const GString &attrName, int value, int defVal = 0);
GLDXMLSHARED_EXPORT void writeUIntToXMLAttr(GLDXMLNode &node, const GString &attrName, uint value, uint defVal = 0);
GLDXMLSHARED_EXPORT void writeFloatToXMLAttr(GLDXMLNode &node, const GString &attrName, double value, double defVal = 0);

GLDXMLSHARED_EXPORT GXMLDocument createXMLDocument(bool aIncludeHeader = true, bool autoIndent = true);
GLDXMLSHARED_EXPORT GXMLDocument loadXMLDocument(const GString &fileName);
GLDXMLSHARED_EXPORT GXMLDocument loadXMLDocument(GStream *stream);
GLDXMLSHARED_EXPORT void saveXMLDocument(GXMLDocument &doc, const GString &fileName);
GLDXMLSHARED_EXPORT void saveXMLDocument(GXMLDocument &doc, GStream *stream);

GLDXMLSHARED_EXPORT bool isXMLStream(GStream *stream);

GLDXMLSHARED_EXPORT void clearNodes(GXMLNode &node);

GLDXMLSHARED_EXPORT int posXMLString(GString strSub,GString strValue,int nStart = 0);
GLDXMLSHARED_EXPORT void xmlDocError(const GString &msg);
GLDXMLSHARED_EXPORT void xmlDocError(const GString &msg, const GVariantList &variantList);

GLDXMLSHARED_EXPORT GLDXMLDocument createGLDXMLDocument(bool aIncludeHeader = true, bool autoIndent = true);
GLDXMLSHARED_EXPORT GLDXMLDocument loadGLDXMLDocument(const GString &fileName);
GLDXMLSHARED_EXPORT GLDXMLDocument loadGLDXMLDocument(GStream *stream);
GLDXMLSHARED_EXPORT void saveGLDXMLDocument(GLDXMLDocument &doc, const GString &fileName);
GLDXMLSHARED_EXPORT void saveGLDXMLDocument(GLDXMLDocument &doc, GStream *stream);

#endif // GLDXMLUTILS_H

