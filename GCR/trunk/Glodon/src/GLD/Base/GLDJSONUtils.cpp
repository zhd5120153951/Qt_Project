#include "GLDJSONUtils.h"
#include "GLDStrUtils.h"
#include "GLDMathUtils.h"

#include "GLDFile.h"
#include "GLDTextStream.h"

void saveJSONDocument(GJsonDocument &doc, const GString &fileName)
{
    GFileStream file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }
    QTextStream oStream(&file);
    oStream << doc.toJson();
    file.close();
}


void saveJSONDocument(GJsonDocument &doc, QTextStream &stream)
{
    stream << doc.toJson();
}

GString readStrFromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, const GString &defVal)
{
    if (jsonObject.contains(jsonObjectKey))
    {
        return jsonObject.find(jsonObjectKey).value().toString(defVal);
    }
    else
    {
        return defVal;
    }
}

bool readBoolFromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, bool defVal)
{
    if (jsonObject.contains(jsonObjectKey))
        return jsonObject.find(jsonObjectKey).value().toBool(defVal);
    else
        return defVal;
}

int readIntFromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, int defVal)
{
    if (jsonObject.contains(jsonObjectKey))
        return jsonObject.find(jsonObjectKey).value().toDouble(defVal);
    else
        return defVal;
}

gint64 readInt64FromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, long long defVal)
{
    // 在 GJsonValue 中我们使用 string 来存储 int64;
    if (jsonObject.contains(jsonObjectKey))
    {
        GString sDefValue = int64ToStr(defVal);
        GString sResult = jsonObject.find(jsonObjectKey).value().toString(sDefValue);
        return strToInt64Def(sResult, defVal);
    }
    else
        return defVal;
}

guint64 readUInt64FromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, guint64 defVal)
{
    // 在 GJsonValue 中我们使用 string 来存储 int64;
    if (jsonObject.contains(jsonObjectKey))
    {
        GString sDefValue = uint64ToStr(defVal);
        GString sResult = jsonObject.find(jsonObjectKey).value().toString(sDefValue);
        return strToUInt64Def(sResult, defVal);
    }
    else
        return defVal;
}

double readFloatFromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, double defVal)
{
    if (jsonObject.contains(jsonObjectKey))
    {
        return jsonObject.find(jsonObjectKey).value().toDouble(defVal);
    }
    else
        return defVal;
}

void writeStrToJSON(GJsonObject &jsonObject, const GString &jsonObjectKey, const GString &value, const GString &defVal)
{
    if (value == defVal)
        return;
    jsonObject.insert(jsonObjectKey, value);
}

void writeBoolToJSON(GJsonObject &jsonObject, const GString &jsonObjectKey, bool value, bool defVal)
{
    if (value == defVal)
        return;
    jsonObject.insert(jsonObjectKey, value);
}

void writeIntToJSON(GJsonObject &jsonObject, const GString &jsonObjectKey, int value, int defVal)
{
    if (value == defVal)
        return;
    jsonObject.insert(jsonObjectKey, value);
}

void writeInt64ToJSON(GJsonObject &jsonObject, const GString &jsonObjectKey, long long value, long long defVal)
{
    if (value == defVal)
        return;

    writeStrToJSON(jsonObject, jsonObjectKey, int64ToStr(value));
}

void writeUInt64ToJSON(GJsonObject &jsonObject, const GString &jsonObjectKey, guint64 value, guint64 defVal)
{
    if (value == defVal)
        return;

    writeStrToJSON(jsonObject, jsonObjectKey, uint64ToStr(value));
}

void writeFloatToJSON(GJsonObject &jsonObject, const GString &jsonObjectKey, double value, double defVal)
{
    if (sameValue(value, defVal))
        return;

    jsonObject.insert(jsonObjectKey, value);
}

GJsonDocument loadJSONDocument(const GString &fileName)
{
    GFile oJsonFile(fileName);
    if (!oJsonFile.exists())
    {
        return GJsonDocument();
    }
    if (!oJsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return GJsonDocument();
    }
    QTextStream oStream(&oJsonFile);
    return loadJSONDocument(oStream);
}

GJsonDocument loadJSONDocument(QTextStream &stream)
{
    GString sJson = stream.readAll();
    if (sJson.isEmpty())
    {
        return GJsonDocument();
    }
    return GJsonDocument::fromJson(sJson.toUtf8());
}


GString readEnCodeStrFromJSON(const GJsonObject &jsonObject, const GString &jsonObjectKey, const GString &defVal)
{
    if (jsonObject.contains(jsonObjectKey))
    {
        GString sValue = jsonObject.find(jsonObjectKey).value().toString();
        if (!sValue.isEmpty())
        {
            QByteArray bValue = sValue.toLatin1();
            bValue = QByteArray::fromBase64(bValue);
            return QString::fromLocal8Bit(bValue);
        }
        else
        return defVal;
    }
    else
    {
        return defVal;
    }
}


void writeEnCodeStrToJSON(GJsonObject &jsonObject, const GString &jsonObjectKey, const GString &value, const GString &defVal)
{
    if (value == defVal)
        return;
    GByteArray bValue = value.toLocal8Bit();
    jsonObject.insert(jsonObjectKey, GString(bValue.toBase64()));
}
