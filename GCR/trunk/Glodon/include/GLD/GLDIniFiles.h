#ifndef GLDINIFILES_H
#define GLDINIFILES_H

#include "GLDString.h"
#include "GLDDateTime.h"
#include "GLDStringObjectList.h"

class GLDCustomIniFilePrivate;
class GLDMemIniFilePrivate;
class GLDCOMMONSHARED_EXPORT GLDCustomIniFile
{
public:
    explicit GLDCustomIniFile(const GString &fileName);
    virtual ~GLDCustomIniFile();
public:
    bool sectionExists(const GString &section);
    virtual GString readString(const GString &section, const GString &ident, const GString &defVal) = 0;
    virtual void writeString(const GString &section, const GString &ident, const GString &value) = 0;

    virtual long readInteger(const GString &section, const GString &ident, long defVal);
    virtual void writeInteger(const GString &section, const GString &ident, long value);

    virtual bool readBool(const GString &section, const GString &ident, bool defVal);
    virtual void writeBool(const GString &section, const GString &ident, bool value);

    virtual int readBinaryStream(const GString &section, const GString &name, GStream *value);
    virtual void writeBinaryStream(const GString &section, const GString &name, GStream *value);

    virtual GDate readDate(const GString &section, const GString &name, GDate &defVal);
    virtual void writeDate(const GString &section, const GString &name, GDate &value);

    virtual GDateTime readDateTime(const GString &section, const GString &name, GDateTime &defVal);
    virtual void writeDateTime(const GString &section, const GString &name, GDateTime &value);

    virtual double readFloat(const GString &section, const GString &name, double defVal);
    virtual void writeFloat(const GString &section, const GString &name, double value);

    virtual GTime readTime(const GString &section, const GString &name, GTime &defVal);
    virtual void writeTime(const GString &section, const GString &name, GTime &value);

    virtual void readSection(const GString &section, GStringObjectList *strings) = 0;
    virtual void readSections(GStringObjectList *strings) = 0;
    virtual void readSections(const GString &section, GStringObjectList *strings);
    virtual void readSectionValues(const GString &section, GStringObjectList *strings) = 0;
    virtual void eraseSection(const GString &section) = 0;
    virtual void deleteKey(const GString &section, const GString &ident) = 0;
    virtual void updateFile() = 0;
    virtual bool valueExists(const GString &section, const GString &ident);
    void setFileName(const GString &fileName);
    GString fileName();

protected:
    GLDCustomIniFile(const GString &fileName, GLDCustomIniFilePrivate &d);
    GLDCustomIniFilePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GLDCustomIniFile);
};

class GLDCOMMONSHARED_EXPORT GLDIniFile : public GLDCustomIniFile
{
public:
    explicit GLDIniFile(const GString &fileName);
    ~GLDIniFile();
public:
    GString readString(const GString &section, const GString &ident, const GString &defVal);
    void writeString(const GString &section, const GString &ident, const GString &value);
    void readSection(const GString &section, GStringObjectList *strings);
    void readSections(GStringObjectList *strings);
    void readSectionValues(const GString &section, GStringObjectList *strings);
    void eraseSection(const GString &section);
    void deleteKey(const GString &section, const GString &ident);
    void updateFile();
private:
    int strCopy(const GString &source, char *dest, int size = -1);
    int getPrivateProfileString(const GString &strSectionName, const GString &strKeyName, GString strDefault,
                                char *pReturnedValue, int size, const GString &strFileName);
    bool writePrivateProfileString(const GString &strSectionName, const GString &strKeyName,
                                   const GString &strValue, const GString &strFileName);
};

/*!
 *@ GLDMemIniFile
 *@brief TMemIniFile - loads an entire INI file into memory and allows all
    operations to be performed on the memory image.  The image can then
    be written out to the disk file
 *@author duanb
 *@date 2013.08.23
*/

class GLDCOMMONSHARED_EXPORT GLDMemIniFile : public GLDCustomIniFile
{
public:
    explicit GLDMemIniFile(const GString &fileName);
    ~GLDMemIniFile();
public:
    void clear();
    void deleteKey(const GString &section, const GString &ident);
    void eraseSection(const GString &section);
    void strings(GStringObjectList *list);
    void readSection(const GString &section, GStringObjectList *strings);
    void readSections(GStringObjectList *strings);
    void readSectionValues(const GString &section, GStringObjectList *strings);
    GString readString(const GString &section, const GString &ident, const GString &Default);
    void rename(const GString &fileName, bool reload);
    void setStrings(GStringObjectList *list);
    void updateFile();
    void writeString(const GString &section, const GString &ident, const GString &value);
    bool allowValueHasSpace();
    void setAllowValueHasSpace(bool allowValueHasSpace);

private:
    GStringObjectList* addSection(const GString &section);
    bool caseSensitive();
    void setCaseSensitive(bool value);
    void loadValues();
private:
    Q_DECLARE_PRIVATE(GLDMemIniFile)
};

#endif // GLDINIFILES_H
