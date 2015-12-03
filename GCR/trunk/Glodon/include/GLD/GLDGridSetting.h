#ifndef GLDGRIDSETTING_H
#define GLDGRIDSETTING_H

#include "GLDSystem.h"
#include "GLDString.h"
#include "GLDIntList.h"
#include "GLDXMLTypes.h"
#include "GLDUITypes.h"
#include "GLDObject.h"
#include "GLDExtPropDefs.h"

typedef unsigned char FontStyles;

extern GLDTABLEVIEWSHARED_EXPORT const char *g_GLDGridSettingVersion; // "2.2.5";
extern GLDTABLEVIEWSHARED_EXPORT const char *g_EditText; // "@EditText";
extern GLDTABLEVIEWSHARED_EXPORT const char *g_TitleCaptionDelimiter; // "||";
DEF_RESOURCESTRING3(CDefInvalidValueLabel, "!非法值"); // 缺省非法值显示标签
extern GLDTABLEVIEWSHARED_EXPORT const GString CDefFontName;
DEF_RESOURCESTRING3(CDefFormatStr, "");
DEF_RESOURCESTRING3(CDefComment, "");
DEF_RESOURCESTRING3(CDelimiter, "*##*");
extern GLDTABLEVIEWSHARED_EXPORT const GString c_GLDCellSuitTypeStrings[4];
extern GLDTABLEVIEWSHARED_EXPORT const GString c_GLDGridRuleTypeStrings[20];

const int CDefDisplayWidth      = 80;           // 缺省列宽
const int CDefTitleRowHeight    = 20;           // 缺省标题行高
const int CDefFilterRowHeight   = 20;           // 缺省过滤行高
const int CDefRowHeight         = 18;           // 缺省行高
const int CDefFixedCols         = 1;            // 缺省固定列
const int CDefFixedEditRows     = 0;            // 缺省可编辑固定行数
const int CDefFixedEditCols     = 0;            // 缺省可编辑固定列数
const int CDefCellHorzMargin    = 2;            // 缺省水平间隙
const int CDefCellVertMargin    = 2;            // 缺省垂直间隙
const int CDefGridLineWidth     = 1;            // 缺省格线宽
const GRgb CDefGridLineColor    = qRgb(125,125,125);     // 缺省格线颜色
const GRgb CDefGridColor        = qRgb(255,255,255);     // 缺省表格颜色
const GRgb CDefBoundLineColor   = qRgb(0,0,0);      // 缺省框线颜色

const int DefFontSize           = 9;
const GRgb DefForeColor         = qRgb(0,0,0);
const GRgb DefBackColor         = qRgb(255,255,255);
//todo yaok
//DefFontStyle          = [];

const int DefLeftLineWidth      = 0;
const int DefTopLineWidth       = 0;
const int DefRightLineWidth     = 0;
const int DefBottomLineWidth    = 0;
const int DefDiagonalWidth      = 0;
const int DefAntiDiagonalWidth  = 0;

const bool DefNullIsZero        = false;
const int CDefHorzMargin        = 2;
const int CDefVertMargin        = 2;
const bool CDefHandleSymbol     = false;
const int CDefImageIndex        = -1;

const int DefHorzMargin         = 2;
const int DefVertMargin         = 2;
const int DefImageIndex         = -1;

const bool CDefShowComment      = false;

const wchar_t CExpressionMark = '&';      // 标记字段值为表达式前缀
const wchar_t CLookupMark     = '#';      // 标记查询字段前缀

enum GLDPersistentStyle
{
    gpsFromDFM = 0,
    gpsFromFile = 1
};

enum GLDTotalStyle
{
    gtsByTree = 0,
    gtsByList = 1
};

// 单元格自动折行类型
enum GLDCellSuitType
{
    gcstFixed,
    gcstSuitRowHeight,
    gcstSuitColWidth,
    gcstFitColWidth
};

// 水平对齐方式
enum GLDHorzAlignment
{
    ghaAuto,
    ghaLeftJustify,
    ghaRightJustify,
    ghaCenter
};

// 垂直对齐方式
enum GLDVertAlignment
{
    gvaAuto,
    gvaTopJustify,
    gvaBottomJustify,
    gvaCenter
};

// 显示规则类型
enum GLDGridRuleType
{
    grtFont = 0,
    grtBackColor = 1,
    grtEditStyle = 2,
    grtBoundLine = 3,
    grtAlignment = 4,
    grtMargin = 5,
    grtReadonly = 6,
    grtCanFocus = 7,
    grtHandleSymbol = 8,
    grtImage = 9,
    grtComment = 10,
    grtVisible = 11,
    grtMerge = 12,
    grtRejectDelete = 13,
    grtRejectInsert = 14,
    grtRejectInsertChild = 15,
    grtRejectMove = 16,
    grtRejectLevel = 17,
    grtEditForm = 18,
    grtClearCell = 19
};

const int CGridRuleTypeCount = 20;

const GLDEditStyle CDefEditStyle   = gesAuto;

// 视图编辑器属性缺省值
const int CDefFontSize         = 9;
const GRgb CDefFontColor       = qRgb(0, 0, 0);
const FontStyles CDefFontStyles= 0;
const GRgb CDefBackColor       = qRgb(125,125,125);
const int CDefRightLine        = 0;
const int CDefBottomLine       = 0;
const int CDefDiagonal         = 0;
const int CDefAntiDiagonal     = 0;

enum GLDCellImageLayout
{
    gclImageLeft = 0,
    gclImageRight = 1,
    gclImageTop = 2,
    gclImageBottom = 3
};

const GLDHorzAlignment DefTitleHorzAlignment = ghaCenter;

typedef unsigned char GLDBaseDataType;
typedef unsigned char GLDDataType;

struct GLDSortState
{
    inline GLDSortState() : sortIndex(-1), asc(true), dataType(0), col(-1) {}
    int sortIndex;
    bool asc;
    GLDBaseDataType dataType;
    int col;
};

const GLDHorzAlignment   CDefHorzAlignment    = ghaAuto;
const GLDVertAlignment   CDefVertAlignment    = gvaCenter;
const GLDVertAlignment   DefVertAlignment     = gvaAuto;
const GLDHorzAlignment   DefHorzAlignment     = ghaAuto;
const GLDCellImageLayout CDefImageLayout      = gclImageLeft;

const bool CDefReadonly          = true;
const bool CDefCanFocus          = true;
const bool CDefVisible           = true;

class GLDGridRule;
class GLDGridRules;
class CGLDExtPropDefs;
class GLDFontRule;
class GLDEditStyleRule;
class GLDBackColorRule;
class GLDBoundLineRule;
class GLDAlignRule;
class GLDMarginRule;
class GLDReadonlyRule;
class GLDCanFocusRule;
class GLDHandleSymbolRule;
class GLDImageRule;
class GLDVisibleRule;
class GLDCommentRule;
class GLDMergeRule;
class GLDRejectDeleteRule;
class GLDRejectRule;
class GLDRejectDeleteRule;
class GLDRejectInsertRule;
class GLDRejectInsertChildRule;
class GLDRejectMoveRule;
class GLDRejectLevelRule;
class GLDEditFormRule;
class GLDClearCellRule;

class GLDCustomGridSetting;
class GLDGridSetting;
class GLDTableSettings;
class GLDTableSetting;
class GLDColSettings;
class GLDColSetting;
class GLDFilterRows;
class GLDFilterRow;
class GLDTitleRows;
class GLDTitleRow;
class GLDHeaderRowFieldSettings;
class GLDHeaderRowFieldSetting;
class GLDExpandRowFieldSettings;
class GLDExpandRowFieldSetting;
class GLDTotalRowFieldSettings;
class GLDTotalRowFieldSetting;
class GLDCellFormats;
class GLDCellFormat;
class GLDFieldSettings;
class GLDFieldSetting;
class GLDCustomFieldSetting;
class GLDFilterCell;
class GLDTitleCell;

class GLDRecordGridSetting;
class GLDRecordSettings;
class GLDRecordSetting;
class GLDRecordFieldSettings;
class GLDRecordFieldSetting;
class GLDRecordGridRules;

typedef GLDVector<GLDColSetting *> GLDColSettingVector;

class GLDTABLEVIEWSHARED_EXPORT GLDDBGridSettingNotifyEvent
{
public:
    virtual void doEvent(GLDCustomGridSetting *sender) = 0;
};

typedef GLDVector<GLDDBGridSettingNotifyEvent *> GLDGridSettingNotifyEventList;
class GLDTABLEVIEWSHARED_EXPORT GLDCustomGridSetting
{
public:
    GLDCustomGridSetting();
    virtual ~GLDCustomGridSetting();

    bool active() const;
    void setActive(const bool value);
    GLDPersistentStyle persistentStyle() const;
    void setPersistentStyle(const GLDPersistentStyle value);
    GString persistentValue() const;
    void setPersistentValue(const GString &value);

public:
    void reLoad();
    void reBuild();
    virtual void reBuildPersistentValue();
    void assignData(GLDCustomGridSetting &source);
    virtual void clearData() = 0;

    virtual void saveToFile(const GString &fileName) = 0;
    virtual void loadFromStream(GStream *stream);
    virtual void saveToStream(GStream *stream);
    virtual GString shortIdentity();
    virtual GString fullIdentity();

    void refreshRule(GLDGridRule *rule);

    bool designState() const;
    void setDesignState(bool value);
    GString version() const;
    GString description() const;
    void setDescription(GString value);
    GString remark() const;
    void setRemark(GString value);
    GString identity() const;
    void setIdentity(GString value);
    unsigned char leftMargin() const;
    void setLeftMargin(unsigned char value);
    unsigned char rightMargin() const;
    void setRightMargin(unsigned char value);
    unsigned char topMargin() const;
    void setTopMargin(unsigned char value);
    unsigned char bottomMargin() const;
    void setBottomMargin(unsigned char value);
    unsigned char gridLineWidth() const;
    void setGridLineWidth(unsigned char value);
    GRgb gridLineColor() const;
    void setGridLineColor(GRgb value);
    GRgb gridColor() const;
    void setGridColor(GRgb value);
    GRgb boundLineColor() const;
    void setBoundLineColor(GRgb value);
    unsigned short fixedCols() const;
    void setFixedCols(unsigned short value);
    unsigned short fixedEditCols() const;
    void setFixedEditCols(unsigned short value);
    unsigned short fixedEditRows() const;
    void setFixedEditRows(unsigned short value);
    bool showAsTree() const;
    void setShowAsTree(bool value);
    bool editing() const;
    void setEditing(bool value);
    bool vertLine() const;
    void setVertLine(bool value);
    bool horzLine() const;
    void setHorzLine(bool value);
    bool rowSizing() const;
    void setRowSizing(bool value);
    bool colSizing() const;
    void setColSizing(bool value);
    bool colMoving() const;
    void setColMoving(bool value);
    bool allowSelectRow() const;
    void setAllowSelectRow(bool value);
    bool allowSelectCol() const;
    void setAllowSelectCol(bool value);
    bool allowSelectAll() const;
    void setAllowSelectAll(bool value);
    bool multiSelection() const;
    void setMultiSelection(bool value);
    GString invalidValueLabel() const;
    void setInvalidValueLabel(GString value);
    bool alwaysShowEditor() const;
    void setAlwaysShowEditor(bool value);
    bool rangeSelect() const;
    void setRangeSelect(bool value);
    bool autoThemeAdapt() const;
    void setAutoThemeAdapt(bool value);
    bool returnKeyAsTab() const;
    void setReturnKeyAsTab(bool value);
    bool showFixedRow() const;
    void setShowFixedRow(bool value);
    bool cellFilling() const;
    void setCellFilling(bool value);
    bool fixedCellEvent() const ;
    void setFixedCellEvent(bool value);
    bool allow3DStyle() const;
    void setAllow3DStyle(bool value);
    bool allowCopyPaste() const;
    void setAllowCopyPaste(bool value);
    bool borderStyle() const;
    void setBorderStyle(bool value);
    bool hideEditOnExit() const;
    void setHideEditOnExit(bool value);
    bool useBlendColor() const;
    void setUseBlendColor(bool value);
    bool cellFillEditField() const;
    void setCellFillEditField(bool value);
    CGLDExtPropDefs *extPropDefs() const;
    PtrInt tag() const;
    void setTag(PtrInt tag);
    GLDGridSettingNotifyEventList *afterActiveEventList();
    //GSPGridSettingNotifyEventList * beforeDeactiveEventList();//todo event

protected:
    virtual void compile() = 0;
    virtual void clearCompileInfo() = 0;
    virtual GString saveDialogDefaultExt() = 0;
    virtual GString saveDialogFilter() = 0;
    virtual void loadGridSetting() = 0;
    virtual GString fileName();
    virtual void doAfterActive();
    virtual void doBeforeDeactive();
    virtual void doLoadFromXML(GXMLDocument &doc) = 0;
    virtual void doLoadFromFile(const GString &fileName) = 0;
    virtual void doSaveToXML(GXMLDocument &doc) = 0;
    virtual void loaded();
    virtual void doLoad();
    virtual void doUnload();

protected:
    bool m_active;
    bool m_loaded;

    CGLDExtPropDefs *m_extProps;
    //派生有操作
    unsigned short m_fixedCols;         //全局选项  固定列数                    1
    unsigned short m_fixedEditCols;     //全局选项  可编辑固定列数               3
    unsigned short m_fixedEditRows;     //全局选项  可编辑固定行数               2
    unsigned char m_leftMargin;         //全局选项  内容与格线水平空间？          4
    unsigned char m_rightMargin;        //全局选项  内容与格线垂直空间？          5
    unsigned char m_topMargin;          //全局选项
    unsigned char m_bottomMargin;
    unsigned char m_gridLineWidth;
    GRgb m_gridLineColor;
    GRgb m_gridColor;
    GRgb m_boundLineColor;
    GString m_invalidValueLabel;
    bool m_showAsTree;
    bool m_editing;
    bool m_vertLine;
    bool m_horzLine;
    bool m_rowSizing;
    bool m_colSizing;
    bool m_colMoving;
    bool m_allowSelectRow;
    bool m_allowSelectCol;
    bool m_allowSelectAll;
    bool m_multiSelection;

    bool m_alwaysShowEditor;
    bool m_rangeSelect;
    bool m_autoThemeAdapt;
    bool m_returnKeyAsTab;
    bool m_showFixedRow;
    bool m_cellFilling;
    bool m_fixedCellEvent;
    bool m_allow3DStyle;
    bool m_allowCopyPaste;
    bool m_borderStyle;
    bool m_hideEditOnExit;
    bool m_useBlendColor;
    bool m_cellFillEditField;
    bool m_designState;
    GLDPersistentStyle m_persistentStyle;

private:
    void designing_DfmToFile();
    void designing_FileToDfm();
protected:

    GString m_persistentValue;  //持久化对象名称（也就是gsf文件名）
    GString m_description;      //xml头部信息中标签Description里的内容
    GString m_remark;           //同上remark中的内容
    GString m_identity;         //怀疑对应于基本信息中的名称(Delphi版的未保存该值)

    GLDGridSettingNotifyEventList *m_afterActiveEventList;
    //GSPGridSettingNotifyEventList *beforeDeactiveEventList_;//todo event
    PtrInt m_tag;
};

class GLDTABLEVIEWSHARED_EXPORT GLDGridSetting: public GLDCustomGridSetting
{
public:
    GLDGridSetting();
    virtual ~GLDGridSetting();

    virtual void initialize();
    static GLDGridSetting *createGLDGridSetting();

    /*此函数供组件编辑器使用 */
    void saveToFile(const GString &fileName);
    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    void clearData();
    virtual void compileExpandRow();

    bool allowSort() const;
    void setAllowSort(bool value);
    GString sortFieldNames() const;
    void setSortFieldNames(GString value);
    int ascImageIndex() const;
    void setAscImageIndex(int value);
    int descImageIndex() const;
    void setDescImageIndex(int value);
    bool totalRowAtFooter() const;
    void setTotalRowAtFooter(bool value);

    GLDColSettings *colSettings() const;
    GLDTitleRows *titleRows() const;
    GLDFilterRows *filterRows() const;
    GLDTableSettings *tableSettings() const;

    void setTableSettings(GLDTableSettings *pTableSettings);
public:
    ULONG AddControlRef();
    ULONG ReleaseControlRef();
protected:
    void compile();
    void clearCompileInfo();
    void loadGridSetting();
    void doLoadFromXML(GXMLDocument &doc);
    void doLoadFromFile(const GString &fileName);
    void doSaveToXML(GXMLDocument &doc);
    GString saveDialogDefaultExt();
    GString saveDialogFilter();
protected:
    bool m_allowSort;
    GString m_sortFieldName;
    int m_ascImageIndex;
    int m_descImageIndex;
    bool m_totalRowAtFooter;
    GLDColSettings *m_colSettings;
    GLDTitleRows *m_titleRows;
    GLDFilterRows *m_filterRows;
    GLDTableSettings *m_tableSettings;
    ULONG m_controlRef;
};

// 表设置容器类
class GLDTABLEVIEWSHARED_EXPORT GLDTableSettings
{
public:
    GLDTableSettings(GLDGridSetting *owner);
    virtual ~GLDTableSettings();

    GLDGridSetting *owner() const;
    bool hasHeaderOrTotalRow() const;

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    void clone(GLDTableSetting *protoType, int insertIndex, int count);
    void clearClonedTableSetting();
    virtual GLDTableSetting *insert(int index = -1);
    void Delete(int index);
    void move(int fromIndex, int toIndex);
    void clear();
    int count() const;
    virtual void setCount(const int value);
    int indexOf(GLDTableSetting *tableSetting) const;
    GLDTableSetting *items(int index);
    GLDTableSetting *operator[](int index);
    GLDTableSetting *createTableSetting(GLDTableSettings *owner);
    virtual GLDTableSetting *doCreateTableSetting(GLDTableSettings *owner);

    void compile();
    void clearCompileInfo();
protected:
    GLDGridSetting *m_owner;
    bool m_hasHeaderOrTotalRow;
    GObjectList<GLDTableSetting *>* m_list;
};

// 列设置容器类
class GLDTABLEVIEWSHARED_EXPORT GLDColSettings
{
public:
    GLDColSettings(GLDGridSetting *owner);
    virtual ~GLDColSettings();

    GLDGridSetting *owner() const;

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    virtual void clearCompileInfo();
    void clone(GLDColSetting *protoType, int insertIndex, int count);
    void clearClonedColSetting();
    GLDColSetting *insert(int index = -1);
    void Delete(int index);
    void move(int fromIndex, int toIndex);
    void clear();
    int indexOf(GLDColSetting *colSetting) const;
    GLDColSetting *findColSetting(const GString &identity);
    int visibleColCount();
    int fixedColCount();
    GIntList suitRowHeightCols(); // todo
    GIntList suitColWidthCols();
    GIntList fitColWidthCols();
    int count() const;
    void setCount(int value);
    GLDColSetting *items(int index) const;
    GLDColSetting *operator[](int index);
    GObjectList<GLDColSettingVector *> &visibleCondItems();
    virtual void compile();
    virtual GLDColSetting *createColSetting(GLDColSettings *owner);
    virtual GLDFieldSetting *createFieldSetting(GLDFieldSettings *owner);
    virtual GLDHeaderRowFieldSetting *createHeaderRowFieldSetting(GLDHeaderRowFieldSettings *owner);
    virtual GLDTotalRowFieldSetting *createTotalRowFieldSetting(GLDExpandRowFieldSettings *owner);
    virtual GLDTitleCell *createTitleCell(GLDTitleRow *owner);
    virtual GLDFilterCell *createFilterCell(GLDFilterRow *owner);
protected:
    virtual int findParserIndex(const GString &expr);
protected:
    GLDGridSetting *m_owner;
    GLDColSettingVector m_list;
    GObjectList<GLDColSettingVector *> m_visibleCondItems;
};

// 列设置类
class GLDTABLEVIEWSHARED_EXPORT GLDColSetting
{
public:
    GLDColSetting(GLDColSettings *owner);
    virtual ~GLDColSetting();

    void clearCompileInfo();
    void compile();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    virtual void refreshVisibleState(int col);
    GString fullIdentity();

    int displayWidth() const;
    void setDisplayWidth(int value);
    bool enabled() const;
    void setEnabled(bool value);
    int calcedWidth() const;
    void setCalcedWidth(int value);
    int zoomWidth() const;
    void setZoomWidth(int value);
    int originWidth() const;
    void setOriginWidth(int value);
    int originIndex() const;
    void setOriginIndex(int value);
    GString visibleCondition() const;
    void setVisibleCondition(const GString &value);
    bool visible() const;
    void setVisible(bool value);
    GLDCellSuitType cellSuitType() const;
    void setCellSuitType(GLDCellSuitType value);
    int index();
    bool isFixedCol();
    GString identity() const;
    void setIdentity(GString value);
    GString extData() const;
    void setExtData(GString value);
    CGLDExtPropDefs *extPropDefs() const;
    bool cloneFlag() const;
    void setCloneFlag(bool value);
    bool crossExtFlag() const;
    void setCrossExtFlag(bool value);
    PtrInt tag() const;
    void setTag(PtrInt value);
    GLDGridSetting *gridSetting() const;
    GLDColSettings *owner();
protected:
    GLDColSettings *m_owner;
    int m_displayWidth;
    int m_calcedWidth;
    int m_zoomWidth;
    int m_originWidth;              // 为了外部保存配置用
    int m_originIndex;              // 为了外部保存配置用
    bool m_visible;
    bool m_enabled;                  // Enable=False等价于Visible=False
    bool m_cloneFlag;
    bool m_crossExtFlag;          // 交叉表动态扩展标志
    GLDCellSuitType m_cellSuitType;
    GString m_visibleCondition;
    GString m_identity;
    GString m_extData;
    CGLDExtPropDefs *m_extProps;
    PtrInt m_tag;
};

// 表设置类
class GLDTABLEVIEWSHARED_EXPORT GLDTableSetting
{
public:
    GLDTableSetting(GLDTableSettings *owner);
    virtual ~GLDTableSetting();

    GLDTableSettings *owner() const;

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    virtual GLDTableSetting *clone();
    void deleteHeaderRow(int index);
    void deleteTotalRow(int index);
    void clearTotalRows();
    void clearHeaderRows();
    GLDHeaderRowFieldSettings *insertHeaderRow(int index = -1);
    GLDTotalRowFieldSettings *insertTotalRow(int index = -1);
    int index() const;
    int defRowHeight() const;
    void setDefRowHeight(int value);

    GLDFieldSettings *fieldSettings() const;
    CGLDExtPropDefs *extPropDefs() const;
    bool hasHeaderRow();
    bool hasTotalRow();
    GString refDatabaseName() const;
    void setRefDatabaseName(const GString &value);
    GString refTableName() const;
    void setRefTableName(const GString &value);
    GString refFullName();
    GLDGridRules *rules() const;
    PtrInt tag() const;
    void setTag(const PtrInt value);
    int headerRowCount() const;
    int totalRowCount() const;
    GLDHeaderRowFieldSettings *headerRows(int index);
    GLDTotalRowFieldSettings *totalRows(int index);

    bool customDataSource() const;
    void setCustomDataSource(const bool &value);
    GString masterViewName() const;
    void setMasterViewName(const GString &value);
    GString selfFieldName() const;
    void setSelfFieldName(const GString &value);
    GString masterFieldName() const;
    void setMasterFieldName(const GString &value);
    GString primaryKeyFieldName() const;
    void setPrimaryKeyFieldName(const GString &value);
    GString parentKeyFieldName() const;
    void setParentKeyFieldName(const GString &value);
    bool isTree() const;
    void setIsTree(bool value);
    bool isTreeWithMasterView() const;
    void setIsTreeWithMasterView(bool value);
    GLDTotalStyle totalStyle() const;
    void setTotalStyle(GLDTotalStyle value);
    bool needExpandTotalExpr() const;
    void setNeedExpandTotalExpr(const bool value);
    bool cloneFlag() const;
    void setCloneFlag(bool value);

    virtual void compile();
    void clearCompileInfo();
    virtual void initialize();
    virtual GLDTableSetting *createTableSetting(GLDTableSettings *owner);
    virtual GLDHeaderRowFieldSettings *createHeaderRowFieldSettings(GLDTableSetting *owner);
    virtual GLDTotalRowFieldSettings *createTotalRowFieldSettings(GLDTableSetting *owner);
protected:
    GLDTableSettings *m_owner;
    int m_defRowHeight;
    GObjectList<GLDHeaderRowFieldSettings *> m_headerRows;
    GObjectList<GLDTotalRowFieldSettings *> m_totalRows;
    GLDFieldSettings *m_fieldSettings;
    GLDGridRules *m_rules;
    GString m_refDatabaseName;
    GString m_refTableName;
    PtrInt m_tag;
    bool m_cloneFlag;

    // 带区设置
    CGLDExtPropDefs *m_extProps;
    bool m_customDataSource;
    GString m_masterViewName;
    GString m_selfFieldName;
    GString m_masterFieldName;
    GString m_primaryKeyFieldName;
    GString m_parentKeyFieldName;
    bool m_isTree;
    bool m_isTreeWithMasterView;
    GLDTotalStyle m_totalStyle;
    bool m_needExpandTotalExpr;
};

// 过滤行容器
class GLDTABLEVIEWSHARED_EXPORT GLDFilterRows
{
public:
    GLDFilterRows(GLDGridSetting *owner);
    virtual ~GLDFilterRows();

    GLDGridSetting *owner() const;

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GLDFilterRow *insert(int index = -1);
    void Delete(int index);
    void clear();
    int count() const;
    void setCount(const int value);
    int indexOf(GLDFilterRow *value) const;
    GLDFilterRow *items(int index);
    GLDFilterRow *operator[](int index);
    void compile();
    void clearCompileInfo();
    virtual GLDFilterRow *createFilterRow(GLDFilterRows *owner);
protected:
    GLDGridSetting *m_owner;
    GObjectList<GLDFilterRow *> m_list;
};

// 过滤行
class GLDTABLEVIEWSHARED_EXPORT GLDFilterRow
{
public:
    GLDFilterRow(GLDFilterRows *owner);
    virtual ~GLDFilterRow();

    GLDFilterRows *owner() const;
    int indexOf(GLDFilterCell *value);

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    int index() const;
    int rowHeight() const;
    void setRowHeight(const int &value);
    int cellCount() const;
    GLDFilterCell *cells(int index);
    void setCellCount(int value);
    void Delete(int index);
    void compile();
    void clear();
    void clearCompileInfo();
    GLDFilterCell *cellByCaption(const GString &caption);
    GLDFilterCell *operator[](int index);
    GObjectList<GLDFilterCell *> &list();
    virtual GLDFilterCell *creatrFilterCell(GLDFilterRow *owner);
protected:
    GLDFilterRows *m_owner;
    GObjectList<GLDFilterCell *> m_list;
    int m_rowHeight;
};

// 过滤单元格
class GLDTABLEVIEWSHARED_EXPORT GLDFilterCell
{
public:
    GLDFilterCell(GLDFilterRow *owner);
    virtual ~GLDFilterCell();
    virtual void compile();
    virtual void clearCompileInfo();
    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    int index();
    int rowIndex();
    int colIndex();
    bool isLabel() const;
    GString caption() const;
    void setCaption(const GString &value);
    GString editorName() const;
    void setEditorName(const GString &value);
    int mergeID() const;
    void setMergeID(const int &value);
    bool isFixedCol();
    GLDCellFormat *cellFormat() const;
    void setCellFormat(GLDCellFormat *value);
    GLDGridSetting *gridSetting();
protected:
    GLDFilterRow *m_owner;
    GString m_caption;
    GString m_editorName;
    int m_mergeID;
    bool m_isLabel;
    GLDCellFormat *m_cellFormat;
};

// 标题行容器
class GLDTABLEVIEWSHARED_EXPORT GLDTitleRows
{
public:
    GLDTitleRows(GLDGridSetting *owner);
    virtual ~GLDTitleRows();

    GLDGridSetting *owner() const;

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GString fullTitle(int index) const;
    GLDTitleRow *insert(int index = -1);
    void Delete(int index);
    void clear();
    int count() const;
    void setCount(const int value);
    GLDTitleRow *items(int index) const;
    int indexOf(GLDTitleRow * value) const;
    GLDTitleRow *operator[](int index);

    void compile();
    void clearCompileInfo();
    GLDTitleRow *createTitleRow(GLDTitleRows *owner);
    virtual GLDTitleRow *doCreateTitleRow(GLDTitleRows *owner);
protected:
    GLDGridSetting *m_owner;
    GObjectList<GLDTitleRow *> m_list;
};

// 标题单元格
class GLDTABLEVIEWSHARED_EXPORT GLDTitleCell
{
public:
    GLDTitleCell(GLDTitleRow *owner);
    virtual ~GLDTitleCell();
    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    virtual void compile();
    virtual void clearCompileInfo();

    int index() const;
    int colIndex() const;
    int rowIndex() const;
    GLDGridSetting *gridSetting();

    bool isLabel() const;
    GString caption() const;
    void setCaption(const GString &value);
    int mergeID() const;
    void setMergeID(const int &value);
    GLDCellFormat *cellFormat() const;
    void setCellFormat(GLDCellFormat *value);
    GLDTitleRow *owner();
protected:
    GLDTitleRow *m_owner;
    GString m_caption;
    int m_mergeID;
    bool m_isLabel;
    GLDCellFormat *m_cellFormat;
};

// 标题行
class GLDTABLEVIEWSHARED_EXPORT GLDTitleRow
{
public:
    GLDTitleRow(GLDTitleRows *owner);
    virtual ~GLDTitleRow();
    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    int index();

    void setCellCount(int value);
    void compile();
    void clear();
    void clearCompileInfo();
    GLDTitleCell *cellByCaption(const GString &caption);
    void Delete(int index);
    GLDGridSetting *gridSetting();

    virtual GLDTitleCell *createTitleCell(GLDTitleRow *owner);

    int rowHeight() const;
    void setRowHeight(int value);
    int cellCount() const;
    GLDTitleCell *cells(const int index) const;
    GObjectList<GLDTitleCell*> &list();
protected:
    GLDTitleRows *m_owner;
    GObjectList<GLDTitleCell*> m_list;
    int m_rowHeight;
};

// 扩展行字段设置容器类
class GLDTABLEVIEWSHARED_EXPORT GLDExpandRowFieldSettings
{
public:
    GLDExpandRowFieldSettings(GLDTableSetting *owner);
    virtual ~GLDExpandRowFieldSettings();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GStringList toValueArray();
    void Delete(int index);
    void clear();

    int count() const;
    void setCount(int value);

    bool visible() const;
    void setVisible(bool value);
    int rowHeight() const;
    void setRowHeight(int value);
    GLDTableSetting *owner() const;
    GLDExpandRowFieldSetting *items(int index);
    GObjectList<GLDExpandRowFieldSetting*> &list();
    GLDExpandRowFieldSetting *operator[](int index);
protected:
    virtual GLDExpandRowFieldSetting *createExpandRowFieldSetting();
protected:
    GObjectList<GLDExpandRowFieldSetting*> m_list;
    GLDTableSetting *m_owner;
    bool m_visible;
    int m_rowHeight;
};

// 带区标题行字段设置容器类
class GLDTABLEVIEWSHARED_EXPORT GLDHeaderRowFieldSettings: public GLDExpandRowFieldSettings
{
public:
    inline GLDHeaderRowFieldSettings(GLDTableSetting *owner): GLDExpandRowFieldSettings(owner) {}

    GLDHeaderRowFieldSetting *items(int index);
    inline GLDHeaderRowFieldSetting *operator[](int index) { return items(index); }
protected:
    virtual GLDExpandRowFieldSetting *createExpandRowFieldSetting();
};

// 扩展行字段设置类
class GLDTABLEVIEWSHARED_EXPORT GLDExpandRowFieldSetting
{
public:
    GLDExpandRowFieldSetting(GLDExpandRowFieldSettings *owner);
    virtual ~GLDExpandRowFieldSetting();

    virtual void compile();
    virtual void clearCompileInfo();
    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    bool isSimpleStr() const;
    int mergeID() const;
    void setMergeID(int value);
    GLDDataType resultDataType() const;
    GString value() const;
    void setValue(GString value);
    int index();
    GLDCellFormat *cellFormat() const;
    void setCellFormat(GLDCellFormat *value);
    bool isFixedCol();

    virtual int varCount();
    virtual GString varCodes(int index);
    virtual void setVarCodes(int index, const GString &value);
private:
    int tableIndex();
    GString refFullName();
protected:
    bool m_isSimpleStr;
    int m_mergeID;
    GLDExpandRowFieldSettings *m_owner;
    GLDDataType m_resultDataType;
    GString m_value;
    GLDCellFormat *m_cellFormat;
};

// 合计字段设置类
class GLDTABLEVIEWSHARED_EXPORT GLDHeaderRowFieldSetting: public virtual GLDExpandRowFieldSetting
{
public:
    inline GLDHeaderRowFieldSetting(GLDExpandRowFieldSettings *owner): GLDExpandRowFieldSetting(owner) {}
    virtual void compile();
};

// 合计字段设置容器类
class GLDTABLEVIEWSHARED_EXPORT GLDTotalRowFieldSettings: public GLDExpandRowFieldSettings
{
public:
    inline GLDTotalRowFieldSettings(GLDTableSetting *owner) : GLDExpandRowFieldSettings(owner){}
    GLDTotalRowFieldSetting *items(int index);
    inline GLDTotalRowFieldSetting *operator[](int index) { return items(index); }
protected:
    virtual GLDExpandRowFieldSetting *createExpandRowFieldSetting();
};

// 合计字段设置类
class GLDTABLEVIEWSHARED_EXPORT GLDTotalRowFieldSetting: virtual public GLDExpandRowFieldSetting
{
public:
    inline GLDTotalRowFieldSetting(GLDExpandRowFieldSettings *owner) : GLDExpandRowFieldSetting(owner) {}
    virtual void compile();
};

class GLDTABLEVIEWSHARED_EXPORT GLDCellFormat
{
public:
    GLDCellFormat();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    bool isEqual(GLDCellFormat *cellFormat);

    GRgb foreColor() const;
    void setForeColor(const GRgb value);
    GString fontName() const;
    void setFontName(GString value);
    int fontSize() const;
    void setFontSize(int value);
    FontStyles fontStyle() const;
    void setFontStyle(FontStyles value);
    GRgb backColor() const;
    void setBackColor(const GRgb value);
    GLDHorzAlignment horzAlignment() const;
    void setHorzAlignment(const GLDHorzAlignment value);
    GLDVertAlignment vertAlignment() const;
    void setVertAlignment(const GLDVertAlignment value);
    unsigned char leftLineWidth() const;
    void setLeftLineWidth(const unsigned char value);
    unsigned char topLineWidth() const;
    void setTopLineWidth(unsigned char value);
    unsigned char rightLineWidth() const;
    void setRightLineWidth(unsigned char value);
    unsigned char bottomLineWidth() const;
    void setBottomLineWidth(unsigned char value);
    unsigned char diagonalWidth() const;
    void setDiagonalWidth(unsigned char value);
    unsigned char antiDiagonalWidth() const;
    void setAntiDiagonalWidth(unsigned char value);
    GString formatStr() const;
    void setFormatStr(const GString &value);
    bool nullIsZero() const;
    void setNullIsZero(const bool value);
    unsigned char leftMargin() const;
    void setLeftMargin(unsigned char value);
    unsigned char rightMargin() const;
    void setRightMargin(unsigned char value);
    unsigned char topMargin() const;
    void setTopMargin(unsigned char value);
    unsigned char bottomMargin() const;
    void setBottomMargin(unsigned char value);
    int imageIndex() const;
    void setImageIndex(int value);
    GLDCellImageLayout imageLayout();
    void setImageLayout(GLDCellImageLayout value);

protected:
    GString m_fontName;
    int m_fontSize;
    int m_foreColor;
    FontStyles m_fontStyle;
    int m_backColor;
    unsigned char m_leftLineWidth;
    unsigned char m_topLineWidth;
    unsigned char m_bottomLineWidth;
    unsigned char m_rightLineWidth;
    unsigned char m_diagonalWidth;
    unsigned char m_antiDiagonalWidth;
    int m_horzAlignment;
    int m_vertAlignment;
    GString m_formatStr;
    bool m_nullIsZero;
    unsigned char m_leftMargin;
    unsigned char m_rightMargin;
    unsigned char m_topMargin;
    unsigned char m_bottomMargin;
    GLDCellImageLayout m_imageLayout;
    int m_imageIndex;
};

class GLDTABLEVIEWSHARED_EXPORT GLDCellFormats
{
public:
    GLDCellFormats();
    virtual ~GLDCellFormats();
    int count() const;
    GLDCellFormat *formats(const int index) const;
    GLDCellFormat *operator[](int index) { return formats(index); }
protected:
    GObjectList<GLDCellFormat *> *m_list;
};

// 字段设置容器类
class GLDTABLEVIEWSHARED_EXPORT GLDFieldSettings
{
public:
    GLDFieldSettings(GLDTableSetting *owner);
    virtual ~GLDFieldSettings();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    void setCount(int value);
    void Delete(int index);
    void clear();

    virtual GLDFieldSetting *createFieldSetting(GLDFieldSettings *owner);

    GLDFieldSetting *items(int index);
    int count() const;
    GLDTableSetting *owner() const;
    GObjectList<GLDFieldSetting *> &list();
    GLDFieldSetting *operator[](int index);
protected:
    GLDTableSetting *m_owner;
    GObjectList<GLDFieldSetting *> m_list;
};

// 字段设置类
class GLDTABLEVIEWSHARED_EXPORT GLDCustomFieldSetting
{
public:
    GLDCustomFieldSetting();
    virtual ~GLDCustomFieldSetting();

    virtual void compile();
    virtual void clearCompileInfo();
    virtual int index() const = 0;

    virtual void loadFromStream(GStream *stream);
    virtual void saveToStream(GStream *stream);
    bool displayExprIsFieldName() const;

    GString displayFieldName() const;

    int mergeID() const;
    void setMergeID(int value);
    GString editFieldName() const;
    void setEditFieldName(const GString &value);
    GString displayExpr() const;
    void setDisplayExpr(const GString &value);
    GString saveFieldName() const;
    void setSaveFieldName(const GString &value);
    GString saveRule() const;
    void setSaveRule(const GString &value);
    GString nullValDisplayStr() const;
    void setNullValDisplayStr(GString value);
    virtual GString cellIdentity() const;
    void setCellIdentity(const GString &value);
    GString editorName() const;
    void setEditorName(const GString &value);
    GString extData() const;
    void setExtData(GString value);
    CGLDExtPropDefs *extPropDefs() const;
    GLDCellFormat *cellFormat() const;
    void setCellFormat(GLDCellFormat * value);
    bool editTextIsDisplayText() const;
    void setEditTextIsDisplayText(bool value);

    virtual unsigned char displayExprDataType() = 0;
    virtual bool isLabel() const;
    virtual int varCount() const;
    virtual GString varCodes(int index) const;
protected:
    virtual bool isValidFieldName(const GString &fieldName);
    virtual bool isExprEnumDateTimeField(const GString &fieldName);
    virtual void buildDisplayParser();
    virtual void buildSaveRuleParser();
    GString buildDisplayParserDisplayExpr();

    virtual void setIsLabel(bool value);

    virtual bool designState() = 0;

    virtual int tableIndex() = 0;
    virtual GString refFullName() = 0;
    virtual GLDCustomGridSetting *gridSetting() = 0;
protected:
    int m_mergeID;
    GString m_editFieldName;
    GString m_displayExpr;
    GString m_saveFieldName;
    GString m_saveRule;
    GString m_nullValDisplayStr;
    GString m_extData;
    CGLDExtPropDefs *m_extProps;
    GString m_editorName;          // 二级窗体编辑器名
    GString m_cellIdentity;
    GLDCellFormat *m_cellFormat;
    bool m_displayExprIsFieldName;
    bool m_editTextIsDisplayText;
};

// 字段设置类
class GLDTABLEVIEWSHARED_EXPORT GLDFieldSetting: virtual public GLDCustomFieldSetting
{
public:
    GLDFieldSetting(GLDFieldSettings *owner);
    virtual ~GLDFieldSetting();

    bool designState();
    int index() const;
    int tableIndex();
    GString refFullName();
    GLDCustomGridSetting *gridSetting();
    GLDDataType displayExprDataType();
    GString cellIdentity() const;

    bool isDefaultCellIdentity();
    GString title();
    bool isFixedCol();

    GLDFieldSettings *owner() const;
    GLDTableSetting *tableSetting() const;
    GLDSortState sortState() const;
    void setsortState(GLDSortState value);
protected:
    GLDFieldSettings *m_owner;
    GLDSortState m_sortState;
};

class GLDTABLEVIEWSHARED_EXPORT GLDCustomGridRules
{
public:
    GLDCustomGridRules();
    virtual ~GLDCustomGridRules();

    void buildRuleArray(GLDGridRuleType ruleType);
    void clearRuleArray();

    virtual GLDCustomGridSetting *gridSetting() = 0;
    virtual GString refFullName() = 0;

    virtual bool designState();
    virtual int tableIndex();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    void assign(GLDCustomGridRules *source);
    GLDGridRule *insert(int index, GLDGridRuleType ruleType);
    void Delete(int index);
    void move(int fromIndex, int toIndex);
    void clear();

    bool hasRule(GLDGridRuleType ruleType);
    GLDGridRule *findRule(GLDGridRuleType ruleType, const GString &ruleName);

    int count() const;
    int indexOf(GLDGridRule* value) const;
    GLDGridRule *items(int index) const;
    GLDGridRule *operator[](int index) const { return items(index); }

    virtual GLDFontRule *createFontRule(GLDCustomGridRules *owner);
    virtual GLDBackColorRule *createBackColorRule(GLDCustomGridRules *owner);
    virtual GLDEditStyleRule *createEditStyleRule(GLDCustomGridRules *owner);
    virtual GLDBoundLineRule *createBoundLineRule(GLDCustomGridRules *owner);
    virtual GLDAlignRule *createAlignRule(GLDCustomGridRules *owner);
    virtual GLDMarginRule *createMarginRule(GLDCustomGridRules *owner);
    virtual GLDReadonlyRule *createReadonlyRule(GLDCustomGridRules *owner);
    virtual GLDCanFocusRule *createCanFocusRule(GLDCustomGridRules *owner);
    virtual GLDHandleSymbolRule *createHandleSymbolRule(GLDCustomGridRules *owner);
    virtual GLDImageRule *createImageRule(GLDCustomGridRules *owner);
    virtual GLDCommentRule *createCommentRule(GLDCustomGridRules *owner);
    virtual GLDVisibleRule *createVisibleRule(GLDCustomGridRules *owner);
    virtual GLDMergeRule *createMergeRule(GLDCustomGridRules *owner);
    virtual GLDRejectDeleteRule *createRejectDeleteRule(GLDCustomGridRules *owner);
    virtual GLDRejectInsertRule *createRejectInsertRule(GLDCustomGridRules *owner);
    virtual GLDRejectInsertChildRule *createRejectInsertChileRule(GLDCustomGridRules *owner);
    virtual GLDRejectMoveRule *createRejectMoveRule(GLDCustomGridRules *owner);
    virtual GLDRejectLevelRule *createRejectLevelRule(GLDCustomGridRules *owner);
    virtual GLDEditFormRule *creataeEditFormRule(GLDCustomGridRules *owner);
    virtual GLDClearCellRule *createClearCellRule(GLDCustomGridRules *owner);
protected:
    GObjectList<GLDGridRule*> m_list;
    GLDVector<GLDGridRule*> m_ruleArray[CGridRuleTypeCount];
};

// 显示规则基类
class GLDTABLEVIEWSHARED_EXPORT GLDGridRule
{
public:
    GLDGridRule(GLDCustomGridRules *owner);
    virtual ~GLDGridRule();

    virtual void loadFromStream(GStream *stream);
    virtual void saveToStream(GStream *stream);

    virtual bool autoRefresh() const;
    virtual void compile();
    virtual void clearCompileInfo();

    virtual int varCount();
    int index();
    virtual GLDGridRuleType ruleType() = 0;

    bool enable() const;
    void setEnable(const bool value);
    GString fieldCondition() const;
    void setFieldCondition(const GString &value);
    GString recordCondition() const;
    void setRecordCondition(const GString &value);
    GString ruleName() const;
    void setRuleName(const GString &value);
private:
    bool designState();
    int tableIndex();
    GString refFullName();
protected:
    GLDCustomGridRules *m_owner;
    bool m_enable;
    GString m_fieldCondition;
    GString m_recordCondition;
    GString m_ruleName;
    int m_fixedCols;
};

// 显示规则容器类
class GLDTABLEVIEWSHARED_EXPORT GLDGridRules: virtual public GLDCustomGridRules
{
public:
    GLDGridRules(GLDTableSetting *owner);
    virtual ~GLDGridRules();
    GLDCustomGridSetting *gridSetting();
    GString refFullName();
protected:
    bool designState();
    int tableIndex();
protected:
    GLDTableSetting *m_owner;
};

// 字体规则类
class GLDTABLEVIEWSHARED_EXPORT GLDFontRule: virtual public GLDGridRule
{
public:
    GLDFontRule(GLDCustomGridRules *owner);
    virtual ~GLDFontRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    bool autoRefresh() const;
    GLDGridRuleType ruleType();

    GRgb fontColor() const;
    void setFontColor(GRgb value);
    GString fontName() const;
    void setFontName(const GString &value);
    int fontSize() const;
    void setFontSize(int value);
    FontStyles fontStyles() const;
    void setFontStyles(FontStyles value);
protected:
    GRgb m_fontColor;
    GString m_fontName;
    int m_fontSize;
    FontStyles m_fontStyles;
};

// 背景色规则类
class GLDTABLEVIEWSHARED_EXPORT GLDBackColorRule: virtual public GLDGridRule
{
public:
    GLDBackColorRule(GLDCustomGridRules *owner);
    virtual ~GLDBackColorRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    bool autoRefresh() const;
    GLDGridRuleType ruleType();

    GRgb backColor() const;
    void setBackColor(GRgb value);

protected:
    GRgb m_backColor;
};

// 编辑方式规则类
class GLDTABLEVIEWSHARED_EXPORT GLDEditStyleRule: virtual public GLDGridRule
{
public:
    GLDEditStyleRule(GLDCustomGridRules *owner);
    virtual ~GLDEditStyleRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GLDGridRuleType ruleType();

    GLDEditStyle editStyle() const;
    void setEditStyle(GLDEditStyle value);

protected:
    GLDEditStyle m_editStyle;
};

// 边框线规则类
class GLDTABLEVIEWSHARED_EXPORT GLDBoundLineRule: virtual public GLDGridRule
{
public:
    GLDBoundLineRule(GLDCustomGridRules *owner);
    virtual ~GLDBoundLineRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    bool autoRefresh() const;
    GLDGridRuleType ruleType();

    byte antiDiagonal() const;
    void setAntiDiagonal(const byte value);
    byte bottomLine() const;
    void setBottomLine(const byte value);
    byte diagonal() const;
    void setDiagonal(const byte value);
    byte rightLine() const;
    void setRightLine(const byte value);
protected:
    byte m_antiDiagonal;
    byte m_bottomLine;
    byte m_diagonal;
    byte m_rightLine;
};

// 对齐方式规则类
class GLDTABLEVIEWSHARED_EXPORT GLDAlignRule: virtual public GLDGridRule
{
public:
    GLDAlignRule(GLDCustomGridRules *owner);
    virtual ~GLDAlignRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    bool autoRefresh() const;
    GLDGridRuleType ruleType();

    GLDHorzAlignment horzAlignment() const;
    void setHorzAlignment(const GLDHorzAlignment value);
    GLDVertAlignment vertAlignment() const;
    void setVertAlignment(const GLDVertAlignment value);

protected:
    GLDHorzAlignment m_horzAlignment;
    GLDVertAlignment m_vertAlignment;
};

// 边界规则类
class GLDTABLEVIEWSHARED_EXPORT GLDMarginRule: virtual public GLDGridRule
{
public:
    GLDMarginRule(GLDCustomGridRules *owner);
    virtual ~GLDMarginRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    bool autoRefresh() const;
    GLDGridRuleType ruleType();

    byte leftMargin() const;
    void setLeftMargin(const byte value);
    byte rightMargin() const;
    void setRightMargin(const byte value);
    byte topMargin() const;
    void setTopMargin(const byte value);
    byte bottomMargin() const;
    void setBottomMargin(const byte value);
protected:
    byte m_leftMargin;
    byte m_rightMargin;
    byte m_topMargin;
    byte m_bottomMargin;
};

// 只读规则类
class GLDTABLEVIEWSHARED_EXPORT GLDReadonlyRule: virtual public GLDGridRule
{
public:
    GLDReadonlyRule(GLDCustomGridRules *owner);
    virtual ~GLDReadonlyRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    GLDGridRuleType ruleType();

    bool readonly() const;
    void setReadonly(bool value);

protected:
    bool m_readonly;
};

// 只读规则类
class GLDTABLEVIEWSHARED_EXPORT GLDCanFocusRule: virtual public GLDGridRule
{
public:
    GLDCanFocusRule(GLDCustomGridRules *owner);
    virtual ~GLDCanFocusRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GLDGridRuleType ruleType();

    bool canFocus() const;
    void setCanFocus(const bool value);
protected:
    bool m_canFocus;
};

// 处理符号规则类
class GLDTABLEVIEWSHARED_EXPORT GLDHandleSymbolRule: virtual public GLDGridRule
{
public:
    GLDHandleSymbolRule(GLDCustomGridRules *owner);
    virtual ~GLDHandleSymbolRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GLDGridRuleType ruleType();
    bool handleSymbol() const;
    void setHandleSymbol(bool value);
protected:
    bool m_handleSymbol;
};

// 图标规则类
class GLDTABLEVIEWSHARED_EXPORT GLDImageRule: virtual public GLDGridRule
{
public:
    GLDImageRule(GLDCustomGridRules *owner);
    virtual ~GLDImageRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    bool autoRefresh() const;
    GLDGridRuleType ruleType();

    int imageIndex() const;
    void setImageIndex(int value);
    GLDCellImageLayout imageLayout() const;
    void setImageLayout(GLDCellImageLayout value);
protected:
    int m_imageIndex;
    GLDCellImageLayout m_imageLayout;
};

// 注释规则类
class GLDTABLEVIEWSHARED_EXPORT GLDCommentRule: virtual public GLDGridRule
{
public:
    GLDCommentRule(GLDCustomGridRules *owner);
    virtual ~GLDCommentRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GLDGridRuleType ruleType();

    GString comment() const;
    void setComment(const GString &value);
    bool showComment() const;
    void setShowComment(bool value);
protected:
    GString m_comment;
    bool m_showComment;
};

// 显示规则类
class GLDTABLEVIEWSHARED_EXPORT GLDVisibleRule: virtual public GLDGridRule
{
public:
    GLDVisibleRule(GLDCustomGridRules *owner);
    virtual ~GLDVisibleRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    bool autoRefresh() const;
    GLDGridRuleType ruleType();

    GString inVisibleText() const;
    void setInVisibleText(GString value);

protected:
    GString m_inVisibleText;
};

// 合并单元格规则类
class GLDTABLEVIEWSHARED_EXPORT GLDMergeRule: virtual public GLDGridRule
{
public:
    inline GLDMergeRule(GLDCustomGridRules *owner) : GLDGridRule(owner) {}

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    bool autoRefresh() const;
    GLDGridRuleType ruleType();

    GString mergeExpr() const;
    void setMergeExpr(const GString &value);
protected:
    GString m_mergeExpr;
};

// 禁止规则基类
class GLDTABLEVIEWSHARED_EXPORT GLDRejectRule: virtual public GLDGridRule
{
public:
    inline GLDRejectRule(GLDCustomGridRules *owner) : GLDGridRule(owner) {}

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    int isTree() const;
    void setIsTree(int value);
protected:
    int m_isTree;   // 1: IsTree; -1: IsList;  0:全部
};

// 禁止删除规则类
class GLDTABLEVIEWSHARED_EXPORT GLDRejectDeleteRule: virtual public GLDRejectRule
{
public:
    inline GLDRejectDeleteRule(GLDCustomGridRules *owner) : GLDGridRule(owner), GLDRejectRule(owner) {}
    GLDGridRuleType ruleType();
};

// 禁止插入规则类
class GLDTABLEVIEWSHARED_EXPORT GLDRejectInsertRule: virtual public GLDRejectRule
{
public:
    inline GLDRejectInsertRule(GLDCustomGridRules *owner) : GLDGridRule(owner), GLDRejectRule(owner) {}

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GLDGridRuleType ruleType();

    int isBefore() const;
    void setIsBefore(const int &value);
    GString tableNames() const;
    void setTableNames(const GString &value);
protected:
    GString m_tableNames;
    int m_isBefore;       // 1: Before; -1: After;  0:全部
};

//禁止插入孩子规则
class GLDTABLEVIEWSHARED_EXPORT GLDRejectInsertChildRule: virtual public GLDRejectInsertRule
{
public:
    inline GLDRejectInsertChildRule(GLDCustomGridRules *owner) :
        GLDGridRule(owner), GLDRejectRule(owner), GLDRejectInsertRule(owner), m_level(-1) {}
    virtual ~GLDRejectInsertChildRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GLDGridRuleType ruleType();

    int level() const;
    void setLevel(int value);
protected:
    int m_level;
};

// 禁止上下移规则类
class GLDTABLEVIEWSHARED_EXPORT GLDRejectMoveRule: virtual public GLDRejectRule
{
public:
    inline GLDRejectMoveRule(GLDCustomGridRules *owner) : GLDGridRule(owner), GLDRejectRule(owner) {}

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GLDGridRuleType ruleType();

    int isMoveUp() const;
    void setIsMoveUp(int value);
    int includeChild() const;
    void setIncludeChild(int value);
protected:
    int m_isMoveUp;     // 1: 上移; -1: 下移; 0：全部
    int m_includeChild; // 1: IncludeChild; -1: not IncludeChild;  0:全部
};

// 禁止升降级规则类
class GLDTABLEVIEWSHARED_EXPORT GLDRejectLevelRule: virtual public GLDRejectRule
{
public:
    inline GLDRejectLevelRule(GLDCustomGridRules *owner) : GLDGridRule(owner), GLDRejectRule(owner) {}

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GLDGridRuleType ruleType();

    int isLevelUp() const;
    void setIsLevelUp(int value);
    int isFixPos() const;
    void setIsFixPos(int value);
protected:
    int m_isLevelUp;    // 1: 升级; -1: 降级; 0：全部
    int m_isFixPos;       // 1: 保持位置; -1: 不保持位置;  0:全部
};

// 通用编辑窗体规则类
class GLDTABLEVIEWSHARED_EXPORT GLDEditFormRule: virtual public GLDGridRule
{
public:
    GLDEditFormRule(GLDCustomGridRules *owner);
    virtual ~GLDEditFormRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    GLDGridRuleType ruleType();

    bool enable() const;
    void setEnable(bool value);
protected:
    bool m_enable;
};

// 清空格子内容规则
class GLDTABLEVIEWSHARED_EXPORT GLDClearCellRule: virtual public GLDGridRule
{
public:
    GLDClearCellRule(GLDCustomGridRules *owner);
    virtual ~GLDClearCellRule();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GLDGridRuleType ruleType();

    bool allowClear() const;
    void setAllowClear(bool value);
protected:
    bool m_allowClear;
};

class GLDTABLEVIEWSHARED_EXPORT GLDRecordGridSetting: public GLDCustomGridSetting
{
public:
    GLDRecordGridSetting();
    virtual ~GLDRecordGridSetting();

    static GLDRecordGridSetting *createGLDRecordGridSetting();
    /*此函数供组件编辑器使用 */
    void saveToFile(const GString &fileName);
    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    void clearData();

    void loadRowHeightsFromStr(const GString &s);
    void loadColWidthsFromStr(const GString &s);
    void loadMinColWidthsFromStr(const GString &s);
    void loadColStylesFromStr(const GString &s);
    void loadColIdentitysFromStr(const GString &s);
    GString rowHeightsToStr();
    GString colWidthsToStr();
    GString minColWidthsToStr();
    GString colStylesToStr();
    GString colIdentitysToStr();
    void setColWidths(int index, const int value);
    void setOriginColWidths(int index, const int value);
    void setRowHeights(int index, const int value);
    GLDRecordFieldSetting *fieldsetting(int row, int col);
    void setColStyles(int index, const GLDCellSuitType value);
    void setColIdentitys(int index, const GString &value);
    void setColZoomWidths(int index, const int value);
    void setMinColWidths(int index, const int value);
    void insertRow(int index);
    void insertCol(int index);
    void deleteRow(int index);
    void deleteCol(int index);
    void moveRow(int fromIndex, int toIndex);
    void moveCol(int fromIndex, int toIndex);
    GIntList suitColWidthCols();
    GIntList suitRowHeightCols();
    GIntList fitColWidthCols();
    GLDRecordSettings *recordSettings();
    GLDRecordFieldSettings *fieldSettings() const;
    GLDRecordFieldSetting *cells(int col, int row);

    int rowCount() const;
    void setRowCount(const int value);
    int colCount() const;
    void setColCount(const int value);
    int rowHeights(const int index) const;
    int colWidths(int index) const;
    int colZoomWidths(int index) const;
    GLDCellSuitType colStyles(int index);
    int originColWidths(int index) const;
    int minColWidths(int index) const;
    GString colIdentitys(int index) const;
    unsigned short fixedRows() const;
    void setFixedRows(unsigned short value);
    bool ignoreInvalidCell() const;
    void setIgnoreInvalidCell(bool value);
    bool allowLabelSelection() const;
    void setAllowLabelSelection(bool value);
    virtual void initialize();
protected:
    void compile();
    void clearCompileInfo();
    void loadGridSetting();
    void doLoadFromXML(GXMLDocument &doc);
    void doLoadFromFile(const GString &fileName);
    void doSaveToXML(GXMLDocument &doc);
    GString saveDialogDefaultExt();
    GString saveDialogFilter();
private:
    GString intAryToStr(GIntList &ary);
    void setLength(GIntList &ary, int count);
    void setLength(GStringList &ary, int count);
protected:
    unsigned short m_fixedRows;
    bool m_ignoreInvalidCell;
    bool m_allowLabelSelection;
    int m_rowCount;
    int m_colCount;
    GIntList m_rowHeights;
    GIntList m_colWidths;
    GIntList m_colZoomWidths;
    GIntList m_colStyles;   // 0:固定尺寸； 1:自动行高； 2、自动列宽
    GStringList m_colIdentitys;
    GLDRecordSettings *m_recordSettings;
    GLDRecordFieldSettings *m_fieldSettings;
    /*临时信息，不需要存储 */
    GIntList m_fieldCols;
    GIntList m_originColWidths;
    GIntList m_minColWidths;
};

// 记录设置容器类
class GLDTABLEVIEWSHARED_EXPORT GLDRecordSettings
{
public:
    GLDRecordSettings(GLDRecordGridSetting *owner);
    virtual ~GLDRecordSettings();

    void compile();
    void clearCompileInfo();
    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GLDRecordSetting *insert(int index = -1);
    void Delete(int index);
    void move(int fromIndex, int toIndex);
    void clear();
    int count() const ;
    void setCount(const int value);
    GLDRecordSetting *items(int index) const;
    GLDRecordSetting *operator[](int index);
    GLDRecordGridSetting *owner();
    GLDRecordSetting *createRecordSetting(GLDRecordSettings *owner);
    virtual GLDRecordSetting *doCreateRecordSetting(GLDRecordSettings *owner);
protected:
    GObjectList<GLDRecordSetting *> m_list;
    GLDRecordGridSetting *m_owner;
};

// 记录设置规则
class GLDTABLEVIEWSHARED_EXPORT GLDRecordSetting
{
public:
    GLDRecordSetting(GLDRecordSettings *owner);
    virtual ~GLDRecordSetting();

    void compile();
    void clearCompileInfo();
    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
    GString refFullName();

    GString refDatabaseName() const;
    void setRefDatabaseName(const GString &value);
    GString refTableName() const;
    void setRefTableName(const GString &value);
    GLDRecordGridRules *rules();
    GLDRecordSettings *owner();
    virtual GLDRecordGridRules *createGridRules(GLDRecordSetting *owner);
    void initialize();
protected:
    GLDRecordSettings *m_owner;
    GLDRecordGridRules *m_rules;
    GString m_refDatabaseName;
    GString m_refTableName;
};

class GLDTABLEVIEWSHARED_EXPORT GLDRecordFieldSettings
{
public:
    GLDRecordFieldSettings(GLDRecordGridSetting *owner);
    virtual ~GLDRecordFieldSettings();

    void compile();
    void clearCompileInfo();
    void saveToStream(GStream *stream);
    void loadFromStream(GStream *stream);
    GLDRecordFieldSetting *insert(int index);
    void Delete(int index);
    void clear();
    GLDRecordFieldSetting *items(int index) const;
    GLDRecordFieldSetting *operator[](int index);
    int indexOf(GLDRecordFieldSetting *value) const;
    GObjectList<GLDRecordFieldSetting *> &list();
    int count();
    GLDRecordGridSetting *owner() const;
    virtual GLDRecordFieldSetting *createFieldSetting(GLDRecordFieldSettings *owner);
protected:
    GLDRecordGridSetting *m_owner;
    GObjectList<GLDRecordFieldSetting *> m_list;
};

class GLDTABLEVIEWSHARED_EXPORT GLDRecordFieldSetting: virtual public GLDCustomFieldSetting
{
public:
    GLDRecordFieldSetting(GLDRecordFieldSettings *owner);
    virtual ~GLDRecordFieldSetting();

    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    void compile();
    void clearCompileInfo();
    GLDDataType displayExprDataType();
    bool designState();
    int index() const;
    int tableIndex();
    GString refFullName();
    GLDCustomGridSetting *gridSetting();

    int colNo();
    int rowNo();
    GLDRecordSetting *recordSetting();
    GLDRecordGridSetting *recordGridSetting() const;
    void assign(GLDRecordFieldSetting *fieldSetting);

    bool isLabel() const;
    void setIsLabel(bool value);
    bool visible() const;
    void setVisible(const bool &value);
    GString visibleCondition() const;
    void setVisibleCondition(const GString &value);
    int dataSourceID() const;
    void setDataSourceID(const int value);
    bool isPasswordChar() const;
    void setIsPasswordChar(bool value);
    GLDRecordFieldSettings *owner();
protected:
    GLDRecordFieldSettings *m_owner;
    bool m_isLabel;
    int m_dataSourceID;
    bool m_isPasswordChar;
    bool m_visible;
    GString m_visibleCondition;
};

class GLDTABLEVIEWSHARED_EXPORT GLDRecordGridRules: virtual public GLDCustomGridRules
{
public:
    GLDRecordGridRules(GLDRecordSetting *owner);
    virtual ~GLDRecordGridRules();
    GLDCustomGridSetting *gridSetting();
    GString refFullName();
protected:
    bool designState();
    int tableIndex();
protected:
    GLDRecordSetting *m_owner;
};

GString GLDTABLEVIEWSHARED_EXPORT trimExpression(const GString &s);
bool GLDTABLEVIEWSHARED_EXPORT textIsLabel(const GString &s);
bool GLDTABLEVIEWSHARED_EXPORT isNullTitleCell(GLDTitleCell *cell);
bool GLDTABLEVIEWSHARED_EXPORT isDefaultCellFormat(GLDCellFormat *cellFormat, GLDHorzAlignment defHorzAlignment =
        DefHorzAlignment);
bool GLDTABLEVIEWSHARED_EXPORT isNullFilterCell(GLDFilterCell *filterCell);
bool GLDTABLEVIEWSHARED_EXPORT isNullFieldSetting(GLDFieldSetting *fieldSetting);
bool GLDTABLEVIEWSHARED_EXPORT isNullFieldSetting(GLDRecordFieldSetting *fieldSetting);
bool GLDTABLEVIEWSHARED_EXPORT isNullExpandFieldSetting(GLDExpandRowFieldSetting *fieldSetting);

#endif // GLDGRIDSETTING_H
