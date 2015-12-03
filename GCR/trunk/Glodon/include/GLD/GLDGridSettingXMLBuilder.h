#ifndef GLDGRIDSETTINGXMLBUILDER_H
#define GLDGRIDSETTINGXMLBUILDER_H

#include "GLDSystem.h"
#include "GLDString.h"
#include "GLDXMLTypes.h"
#include "GLDGridSetting.h"

enum GLDGridOption
{
    ggoShowAsTree,
    ggoEditing,
    ggoVertLine,
    ggoHorzLine,
    ggoRowSizing,
    ggoColSizing,
    ggoColMoving,
    ggoAlwaysShowEditor,
    ggoRangeSelect,
    ggoAllowSelectRow,
    ggoAllowSelectCol,
    ggoAllowSelectAll,
    ggoMultiSelection,
    ggoAutoThemeAdapt,
    ggoReturnKeyAsTab,
    ggoShowFixedRow,
    ggoCellFilling,
    ggoFixedCellEvent,
    ggo3DStyle,
    ggoHideEditOnExit,
    ggoBorderStyle,
    ggoUseBlendColor,
    ggoCellFillEditField
};

extern GLDTABLEVIEWSHARED_EXPORT GString g_OldVersion;
class GLDTABLEVIEWSHARED_EXPORT GLDGridSettingXMLBaseReader
{
public:
    inline GLDGridSettingXMLBaseReader() {}
    virtual ~GLDGridSettingXMLBaseReader();
protected:
    void readGridRule(const GXMLNode &node, GLDGridRule *rule);
    void readCellFormat(const GXMLNode &node, GLDCellFormat *cellFormat, GLDHorzAlignment
                        defHorzAlignment = DefHorzAlignment );
    void readExtPropDefs(const GXMLNode &node, CGLDExtPropDefs *extPropDefs);
};

//GSF文件读取类
class GLDTABLEVIEWSHARED_EXPORT GLDGridSettingXMLReader: public GLDGridSettingXMLBaseReader
{
public:
    inline GLDGridSettingXMLReader() {}
    inline ~GLDGridSettingXMLReader() {}
public:
    void read(const GString &fileName, GLDGridSetting *gridSetting);
    void read(GXMLDocument &xmlDoc, GLDGridSetting *gridSetting);
    void read(const GXMLNode &node, GLDGridSetting *gridSetting);
private:
    void readColSettings(const GXMLNode &node, GLDColSettings *colSettings);
    void readColSetting(const GXMLNode &node, GLDColSetting *colSetting);
    void readTitleRows(const GXMLNode &node, GLDTitleRows *titleRows);
    void readTitleRow(const GXMLNode &node, GLDTitleRow *titleRow);
    void readTitleCells(const GXMLNode &node, GLDTitleRow *titleRow);
    void readTitleCell(const GXMLNode &node, GLDTitleCell *titleCell);
    void readFilterRows(const GXMLNode &node, GLDFilterRows *filterRows);
    void readFilterRow(const GXMLNode &node, GLDFilterRow *filterRow);
    void readFilterCells(const GXMLNode &node, GLDFilterRow *filterRow);
    void readFilterCell(const GXMLNode &node, GLDFilterCell *filterCell);
    void readTableSettings(const GXMLNode &node, GLDTableSettings *tableSettings);
    void readTableSetting(const GXMLNode &node, GLDTableSetting *tableSetting);
    void readFieldSettings(const GXMLNode &node, GLDFieldSettings *fieldSettings);
    void readFieldSetting(const GXMLNode &node, GLDFieldSetting *fieldSetting);
    void readGridRules(const GXMLNode &node, GLDGridRules *gridRules);
    void readHeaderRowFieldSettings(const GXMLNode &node, GLDHeaderRowFieldSettings *headerRowFieldSettings);
    void readHeaderRowFieldSetting(const GXMLNode &node, GLDHeaderRowFieldSetting *headerRowFieldSetting);
    void readTotalRowFieldSettings(const GXMLNode &node, GLDTotalRowFieldSettings *totalRowFieldSettings);
    void readTotalRowFieldSetting(const GXMLNode &node, GLDTotalRowFieldSetting *totalRowFieldSetting);
};

//RGF文件读取类
class GLDTABLEVIEWSHARED_EXPORT GLDRecordGridSettingXMLReader: public GLDGridSettingXMLBaseReader
{
public:
    GLDRecordGridSettingXMLReader();
    ~GLDRecordGridSettingXMLReader();
public:
    void read(const GString &fileName, GLDRecordGridSetting *gridSetting);
    void read(const GXMLDocument &xmlDoc, GLDRecordGridSetting *gridSetting);
    void read(const GXMLNode &node, GLDRecordGridSetting *gridSetting);
private:
    void readGridRules(const GXMLNode &node, GLDRecordGridRules *gridRules);
    void readFieldSetting(const GXMLNode &node, GLDRecordFieldSetting *fieldSetting);
    void readFieldSettings(const GXMLNode &node, GLDRecordFieldSettings *fieldSettings);
    void readRecordSetting(const GXMLNode &node, GLDRecordSetting *recordSetting);
    void readRecordSettings(const GXMLNode &node, GLDRecordSettings *recordSettings);
};

class GLDTABLEVIEWSHARED_EXPORT GLDGridSettingXMLBaseWriter
{
public:
    inline GLDGridSettingXMLBaseWriter() {}
    inline ~GLDGridSettingXMLBaseWriter() {}
protected:

    void writeCellFormat(GXMLNode &node, GLDCellFormat *cellFormat, GLDHorzAlignment defHorzAlignment = DefHorzAlignment);
    void writeGridRule(GXMLNode &node, GLDGridRule *rule);
    void writeExtPropDefs(GXMLNode &node, CGLDExtPropDefs *extPropDefs);
};

//GSF写入类
class GLDTABLEVIEWSHARED_EXPORT GLDGridSettingXMLWriter: public GLDGridSettingXMLBaseWriter
{
private:
    void writeColSettings(GXMLNode &node, GLDColSettings *colSettings);
    void writeColSetting(GXMLNode &node, GLDColSetting *colSetting);
    void writeTitleRows(GXMLNode &node, GLDTitleRows *titleRows);
    void writeTitleRow(GXMLNode &node, GLDTitleRow *titleRow);
    void writeTitleCells(GXMLNode &node, GLDTitleRow *titleRow);
    void writeTitleCell(GXMLNode &node, GLDTitleCell *titleCell);
    void writeFilterRows(GXMLNode &node, GLDFilterRows *filterRows);
    void writeFilterRow(GXMLNode &node, GLDFilterRow *filterRow);
    void writeFilterCells(GXMLNode &node, GLDFilterRow *filterRow);
    void writeFilterCell(GXMLNode &node, GLDFilterCell *filterCell);
    void writeTableSettings(GXMLNode &node, GLDTableSettings *tableSettings);
    void writeTableSetting(GXMLNode &node, GLDTableSetting *tableSetting);
    void writeFieldSettings(GXMLNode &node, GLDFieldSettings *fieldSettings);
    void writeFieldSetting(GXMLNode &node, GLDFieldSetting *fieldSetting);
    void writeGridRules(GXMLNode &node, GLDGridRules *gridRules);
    void writeHeaderRowFieldSettings(GXMLNode &node, GLDHeaderRowFieldSettings *headerRowFieldSettings);
    void writeHeaderRowFieldSetting(GXMLNode &node, GLDHeaderRowFieldSetting *headerRowFieldSetting);
    void writeTotalRowFieldSettings(GXMLNode &node, GLDTotalRowFieldSettings *totalRowFieldSettings);
    void writeTotalRowFieldSetting(GXMLNode &node, GLDTotalRowFieldSetting *totalRowFieldSetting);
public:
    void write(const GString &fileName, GLDGridSetting *gridSetting);
    void write(GXMLDocument &xmlDoc, GLDGridSetting *gridSetting);
    void write(GXMLNode &node, GLDGridSetting *gridSetting);
public:
    inline GLDGridSettingXMLWriter(){}
    inline ~GLDGridSettingXMLWriter(){}
};

//RGF写入类
class GLDTABLEVIEWSHARED_EXPORT GLDRecordGridSettingXMLWriter: public GLDGridSettingXMLBaseWriter
{
private:
    void writeGridRules(GXMLNode &node, GLDRecordGridRules *gridRules);
    void writeFieldSetting(GXMLNode &node, GLDRecordFieldSetting *fieldSetting);
    void writeFieldSettings(GXMLNode &node, GLDRecordFieldSettings *fieldSettings);
    void writeRecordSetting(GXMLNode &node, GLDRecordSetting *recordSetting);
    void writeRecordSettings(GXMLNode &node, GLDRecordSettings *recordSettings);
public:
    void write(const GString &fileName, GLDRecordGridSetting *gridSetting, bool encryptData = false);
    void write(GXMLDocument &xmlDoc, GLDRecordGridSetting *gridSetting);
    void write(GXMLNode &node, GLDRecordGridSetting *gridSetting);
public:
    inline GLDRecordGridSettingXMLWriter() {}
    inline ~GLDRecordGridSettingXMLWriter() {}
};

#endif // GLDGRIDSETTINGXMLBUILDER_H
