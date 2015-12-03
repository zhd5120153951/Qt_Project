#ifndef GLDJSONUTILS_H
#define GLDJSONUTILS_H

#include "GLDString.h"
#include "GLDJSONTypes.h"
#include "GLDSystem.h"
#include <QTextStream>
#include "GLDCommon_Global.h"

GLDCOMMONSHARED_EXPORT GString readStrFromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, const GString &defVal = "");
GLDCOMMONSHARED_EXPORT bool readBoolFromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, bool defVal = false);
GLDCOMMONSHARED_EXPORT int readIntFromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, int defVal = 0);
GLDCOMMONSHARED_EXPORT gint64 readInt64FromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, long long defVal = 0);
GLDCOMMONSHARED_EXPORT guint64 readUInt64FromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, guint64 defVal = 0);
GLDCOMMONSHARED_EXPORT double readFloatFromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, double defVal = 0);

GLDCOMMONSHARED_EXPORT void writeStrToJSON(GJsonObject &jsonObject, const GString &jsonObjectKey, const GString &value, const GString &defVal = "");
GLDCOMMONSHARED_EXPORT void writeBoolToJSON(GJsonObject &jsonObject, const GString &jsonObjectKey, bool value, bool defVal = false);
GLDCOMMONSHARED_EXPORT void writeIntToJSON(GJsonObject &jsonObject, const GString &jsonObjectKey, int value, int defVal = 0);
GLDCOMMONSHARED_EXPORT void writeInt64ToJSON(GJsonObject &jsonObject, const GString &attrjsonObjectKey, long long value, long long defVal = 0);
GLDCOMMONSHARED_EXPORT void writeUInt64ToJSON(GJsonObject &jsonObject, const GString &attrjsonObjectKey, guint64 value, guint64 defVal = 0);
GLDCOMMONSHARED_EXPORT void writeFloatToJSON(GJsonObject &jsonObject, const GString &jsonObjectKey, double value, double defVal = 0);

GLDCOMMONSHARED_EXPORT GJsonDocument loadJSONDocument(const GString &fileName);
GLDCOMMONSHARED_EXPORT GJsonDocument loadJSONDocument(QTextStream &stream);
GLDCOMMONSHARED_EXPORT void saveJSONDocument(GJsonDocument &doc, const GString &fileName);
GLDCOMMONSHARED_EXPORT void saveJSONDocument(GJsonDocument &doc, QTextStream &stream);

#endif // GLDJSONUTILS_H
