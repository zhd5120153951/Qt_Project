#include "GLDGridSetting.h"
#include "GLDStreamUtils.h"
#include "GLDStrings.h"
#include "GLDStrUtils.h"
#include "GLDFileInfo.h"
#include "GLDGridSettingXMLBuilder.h"
#include "GLDException.h"

const char *SNeedDesignState             = QT_TRANSLATE_NOOP("GLD", "This Operation Can Only Be Used in Design Status");// "该操作只能在设计状态下使用");
//const char *SNeedRunState              = QT_TRANSLATE_NOOP("GLD", "This Operation Can Only Be Used in Running Status");// "该操作只能在运行状态下使用");
//const char *SInvalidFieldName          = QT_TRANSLATE_NOOP("GLD", "Invalid Edit Field Name[%1] of Table[%2]");// "表[%s]非法编辑字段名[%s]");
const char *SInvalidSaveFieldName        = QT_TRANSLATE_NOOP("GLD", "Table[%s] has a valide written field name[%s]");// "表[%s]非法写入字段名[%s]");
//const char *SInvalidFieldNameMark      = QT_TRANSLATE_NOOP("GLD", "Table [%1] has illegal field name [%2]");// "表[%s]非法字段名称[%s]，\"&\"后必须为枚举字段、表达式字段、时间日期字段");
const char *SInvalidHeaderRowExpr        = QT_TRANSLATE_NOOP("GLD", "Table [%s] has a invalid header row expression [%s]");// "表[%s]非法带区标题行表达式[%s]");
//const char *SNotExistVisibleCol        = QT_TRANSLATE_NOOP("GLD", "No Non Fixed Column to Display");// "没有可显示的非固定列");

const char *CDefInvalidValueLabel        = QT_TRANSLATE_NOOP("GLD", "! invalid value");// "!非法值");
const GString CDefFontName               = TRANS_STRING("宋体");

const char *CDefFormatStr                = "";// "");
const char *CDefComment                  = "";// "");
const char *CDelimiter                   = "*##*";// "*##*");

const GLDDataType c_gdtString    = 12;
const char *g_GLDGridSettingVersion = "2.2.5";
const char *g_EditText = "@EditText";
const char *g_TitleCaptionDelimiter = "||";

const GString c_GLDCellSuitTypeStrings[4] =
{
    "cstFixed", "cstSuitRowHeight", "cstSuitColWidth", "cstFitColWidth"
};
const GString c_GLDGridRuleTypeStrings[20] =
{
    "grtFont", "grtBackColor", "grtEditStyle", "grtBoundLine", "grtAlignment",
    "grtMargin", "grtReadonly", "grtCanFocus", "grtHandleSymbol", "grtImage",
    "grtComment", "grtVisible", "grtMerge", "grtRejectDelete", "grtRejectInsert",
    "grtRejectInsertChild", "grtRejectMove", "grtRejectLevel", "grtEditForm", "grtClearCell"
};

GString trimExpression(const GString &s)
{
    GString result = trim(s);

    if (result != "" && result[0].unicode() == '=')
    {
        result = copy(s, 1);
    }

    return result;
}

bool textIsLabel(const GString &s)
{
    return (s == "") || (s[0].unicode() != '=');
}

/* GLDCustomGridSetting */

GLDCustomGridSetting::GLDCustomGridSetting() : m_active(false), m_loaded(false),
    m_extProps(NULL), m_fixedCols(CDefFixedCols), m_fixedEditCols(CDefFixedEditCols),
    m_fixedEditRows(CDefFixedEditRows), m_leftMargin(CDefHorzMargin), m_rightMargin(CDefHorzMargin),
    m_topMargin(CDefVertMargin), m_bottomMargin(CDefVertMargin), m_gridLineWidth(CDefGridLineWidth),
    m_gridLineColor(CDefGridLineColor), m_gridColor(CDefGridColor), m_boundLineColor(CDefBoundLineColor),
    m_invalidValueLabel(getGLDi18nStr(CDefInvalidValueLabel)), m_showAsTree(false), m_editing(true), m_vertLine(true),
    m_horzLine(true), m_rowSizing(false), m_colSizing(true), m_colMoving(true), m_allowSelectRow(true),
    m_allowSelectCol(true), m_allowSelectAll(true), m_multiSelection(true), m_alwaysShowEditor(false),
    m_rangeSelect(true), m_autoThemeAdapt(true), m_returnKeyAsTab(false), m_showFixedRow(true), m_cellFilling(false),
    m_fixedCellEvent(false), m_allow3DStyle(true), m_allowCopyPaste(true), m_borderStyle(true), m_hideEditOnExit(false),
    m_useBlendColor(true), m_cellFillEditField(false), m_designState(true), m_persistentStyle(gpsFromFile),
    m_persistentValue(""), m_description(""), m_remark(""), m_identity(""),
    m_afterActiveEventList(new GLDGridSettingNotifyEventList()), m_tag(0)
{

}

GLDCustomGridSetting::~GLDCustomGridSetting()
{
    freeAndNil(m_afterActiveEventList)
}

bool GLDCustomGridSetting::active() const
{
    return m_active;
}

GLDPersistentStyle GLDCustomGridSetting::persistentStyle() const
{
    return m_persistentStyle;
}

GString GLDCustomGridSetting::persistentValue() const
{
    return m_persistentValue;
}

bool GLDCustomGridSetting::designState() const
{
    return m_designState;
}

unsigned short GLDCustomGridSetting::fixedEditCols() const
{
    return m_fixedEditCols;
}

void GLDCustomGridSetting::setFixedEditCols(unsigned short value)
{
    m_fixedEditCols = value;
}

void GLDCustomGridSetting::setActive(const bool value)
{
    if (m_active == value)
    {
        return;
    }

    //todo yaok ComponentState 如何实现
    //if (ComponentState.contains(csReading))
    //  m_active = value;
    //else
    if (m_persistentValue != "")
    {
        if (value)
        {
            loadGridSetting();
        }
        else
        {
            doUnload();
        }
    }
    else
    {
        if (value)
        {
            m_active = true;
            m_loaded = true;
            doAfterActive();
        }
        else
        {
            doUnload();
        }
    }
}

void GLDCustomGridSetting::setPersistentStyle(const GLDPersistentStyle value)
{
    if (m_persistentStyle == value)
    {
        return;
    }

    //todo yaok ComponentState 如何实现
    // if (ComponentState.contains(csReading))
    //   m_persistentStyle = Value;
    // else
    {
        if (m_persistentValue != "") /*&& (ComponentState.contains(csDesigning)))*/
        {
            if (m_persistentStyle == gpsFromDFM)
            {
                designing_DfmToFile();
            }
            else
            {
                designing_FileToDfm();
            }
        }
        else
        {
            m_persistentValue = "";
        }

        m_persistentStyle = value;
        reBuild();
    }
}

void GLDCustomGridSetting::setPersistentValue(const GString &value)
{
    if (m_persistentValue != value)
    {
        //todo yaok
        //  if (ComponentState.contains(csReading))
        //   m_persistentValue = Value;
        // else
        {
            m_persistentValue = value;
            reBuild();
        }
    }
}

void GLDCustomGridSetting::reLoad()
{
    if (m_active)
    {
        doBeforeDeactive();
    }

    loadGridSetting();
}

void GLDCustomGridSetting::reBuild()
{
    if (!m_active || !m_loaded)
    {
        return;
    }

    //todo yaok Component 处理
    // operationNotify(opBeginRebuild);
    try
    {
        m_loaded = false;
        reLoad();
        //  operationNotify(opEndRebuild);
    }
    catch (...)
    {
        //   todo yaok
        //  operationNotify(opEndRebuild);
    }
}

void GLDCustomGridSetting::reBuildPersistentValue()
{
    GXMLDocument oDoc;
    doSaveToXML(oDoc);
    m_persistentValue = oDoc.toString();
}

void GLDCustomGridSetting::assignData(GLDCustomGridSetting &source)
{
    GMemoryStream oStream;
    source.saveToStream(&oStream);
    oStream.seek(0);
    loadFromStream(&oStream);
}

void GLDCustomGridSetting::loadFromStream(GStream *stream)
{
    readStrFromStream(stream, m_description);
    readStrFromStream(stream, m_identity);
    readMemoFromStream(stream, m_remark);
    readBoolFromStream(stream, m_showAsTree);
    readBoolFromStream(stream, m_editing);
    readBoolFromStream(stream, m_vertLine);
    readBoolFromStream(stream, m_horzLine);
    readBoolFromStream(stream, m_rowSizing);
    readBoolFromStream(stream, m_colSizing);
    readBoolFromStream(stream, m_colMoving);
    readBoolFromStream(stream, m_alwaysShowEditor);
    readBoolFromStream(stream, m_rangeSelect);
    readBoolFromStream(stream, m_allowSelectRow);
    readBoolFromStream(stream, m_allowSelectCol);
    readBoolFromStream(stream, m_allowSelectAll);
    readBoolFromStream(stream, m_multiSelection);
    readBoolFromStream(stream, m_autoThemeAdapt);
    readBoolFromStream(stream, m_returnKeyAsTab);
    readBoolFromStream(stream, m_showFixedRow);
    readBoolFromStream(stream, m_cellFilling);
    readBoolFromStream(stream, m_fixedCellEvent);
    readBoolFromStream(stream, m_allow3DStyle);
    readBoolFromStream(stream, m_allowCopyPaste);
    readBoolFromStream(stream, m_hideEditOnExit);
    readBoolFromStream(stream, m_borderStyle);
    readBoolFromStream(stream, m_useBlendColor);
    readBoolFromStream(stream, m_cellFillEditField);
    readWordFromStream(stream, m_fixedCols);
    readWordFromStream(stream, m_fixedEditRows);
    readWordFromStream(stream, m_fixedEditCols);
    readByteFromStream(stream, m_leftMargin);
    readByteFromStream(stream, m_rightMargin);
    readByteFromStream(stream, m_topMargin);
    readByteFromStream(stream, m_bottomMargin);
    readByteFromStream(stream, m_gridLineWidth);
    readColorFromStream(stream, m_gridLineColor);
    readColorFromStream(stream, m_gridColor);
    readColorFromStream(stream, m_boundLineColor);
    readStrFromStream(stream, m_invalidValueLabel);
    readBoolFromStream(stream, m_designState);

    m_extProps->loadFromStream(stream);
}

void GLDCustomGridSetting::saveToStream(GStream *stream)
{
    writeStrToStream(stream, m_description);
    writeStrToStream(stream, m_identity);
    writeMemoToStream(stream, m_remark);
    writeBoolToStream(stream, m_showAsTree);
    writeBoolToStream(stream, m_editing);
    writeBoolToStream(stream, m_vertLine);
    writeBoolToStream(stream, m_horzLine);
    writeBoolToStream(stream, m_rowSizing);
    writeBoolToStream(stream, m_colSizing);
    writeBoolToStream(stream, m_colMoving);
    writeBoolToStream(stream, m_alwaysShowEditor);
    writeBoolToStream(stream, m_rangeSelect);
    writeBoolToStream(stream, m_allowSelectRow);
    writeBoolToStream(stream, m_allowSelectCol);
    writeBoolToStream(stream, m_allowSelectAll);
    writeBoolToStream(stream, m_multiSelection);
    writeBoolToStream(stream, m_autoThemeAdapt);
    writeBoolToStream(stream, m_returnKeyAsTab);
    writeBoolToStream(stream, m_showFixedRow);
    writeBoolToStream(stream, m_cellFilling);
    writeBoolToStream(stream, m_fixedCellEvent);
    writeBoolToStream(stream, m_allow3DStyle);
    writeBoolToStream(stream, m_allowCopyPaste);
    writeBoolToStream(stream, m_hideEditOnExit);
    writeBoolToStream(stream, m_borderStyle);
    writeBoolToStream(stream, m_useBlendColor);
    writeBoolToStream(stream, m_cellFillEditField);
    writeWordToStream(stream, m_fixedCols);
    writeWordToStream(stream, m_fixedEditRows);
    writeWordToStream(stream, m_fixedEditCols);
    writeByteToStream(stream, m_leftMargin);
    writeByteToStream(stream, m_rightMargin);
    writeByteToStream(stream, m_topMargin);
    writeByteToStream(stream, m_bottomMargin);
    writeByteToStream(stream, m_gridLineWidth);
    writeColorToStream(stream, m_gridLineColor);
    writeColorToStream(stream, m_gridColor);
    writeColorToStream(stream, m_boundLineColor);
    writeStrToStream(stream, m_invalidValueLabel);
    writeBoolToStream(stream, m_designState);
    m_extProps->saveToStream(stream);
}

GString GLDCustomGridSetting::shortIdentity()
{
    GFileInfo fileInfo(fileName());

    return fileInfo.completeBaseName();
}

GString GLDCustomGridSetting::fullIdentity()
{
    return shortIdentity();
}

void GLDCustomGridSetting::setDesignState(bool value)
{
    if (m_designState == value)
    {
        return;
    }

    if (value)
    {
        clearCompileInfo();
    }
    else
    {
        compile();
    }

    m_designState = value;
}

GString GLDCustomGridSetting::version() const
{
    return g_GLDGridSettingVersion;
}

GString GLDCustomGridSetting:: description() const
{
    return m_description;
}

void GLDCustomGridSetting::setDescription(GString value)
{
    m_description = value;
}

GString GLDCustomGridSetting::remark() const
{
    return m_remark;
}

void GLDCustomGridSetting::setRemark(GString value)
{
    m_remark = value;
}

GString GLDCustomGridSetting::identity() const
{
    return m_identity;
}

void GLDCustomGridSetting::setIdentity(GString value)
{
    m_identity = value;
}

unsigned char GLDCustomGridSetting::leftMargin() const
{
    return m_leftMargin;
}

void GLDCustomGridSetting::setLeftMargin(unsigned char value)
{
    m_leftMargin = value;
}
unsigned char GLDCustomGridSetting::rightMargin() const
{
    return m_rightMargin;
}

void GLDCustomGridSetting::setRightMargin(unsigned char value)
{
    m_rightMargin = value;
}
unsigned char GLDCustomGridSetting::topMargin() const
{
    return m_topMargin;
}

void GLDCustomGridSetting::setTopMargin(unsigned char value)
{
    m_topMargin = value;
}

unsigned char GLDCustomGridSetting::bottomMargin() const
{
    return m_bottomMargin;
}

void GLDCustomGridSetting::setBottomMargin(unsigned char value)
{
    m_bottomMargin = value;
}

unsigned char GLDCustomGridSetting::gridLineWidth() const
{
    return m_gridLineWidth;
}

void GLDCustomGridSetting::setGridLineWidth(unsigned char value)
{
    m_gridLineWidth = value;
}

GRgb GLDCustomGridSetting::gridLineColor() const
{
    return m_gridLineColor;
}

void GLDCustomGridSetting::setGridLineColor(GRgb value)
{
    m_gridLineColor = value;
}

GRgb GLDCustomGridSetting::gridColor() const
{
    return m_gridColor;
}

void GLDCustomGridSetting::setGridColor(GRgb value)
{
    m_gridColor = value;
}

GRgb GLDCustomGridSetting::boundLineColor() const
{
    return m_boundLineColor;
}

void GLDCustomGridSetting::setBoundLineColor(GRgb value)
{
    m_boundLineColor = value;
}
unsigned short GLDCustomGridSetting::fixedCols() const
{
    return m_fixedCols;
}

void GLDCustomGridSetting::setFixedCols(unsigned short value)
{
    m_fixedCols = value;
}

unsigned short GLDCustomGridSetting::fixedEditRows() const
{
    return m_fixedEditRows;
}

void GLDCustomGridSetting::setFixedEditRows(unsigned short value)
{
    m_fixedEditRows = value;
}

bool GLDCustomGridSetting::showAsTree() const
{
    return m_showAsTree;
}

void GLDCustomGridSetting::setShowAsTree(bool value)
{
    m_showAsTree = value;
}

bool GLDCustomGridSetting::editing() const
{
    return m_editing;
}

void GLDCustomGridSetting::setEditing(bool value)
{
    m_editing = value;
}

bool GLDCustomGridSetting::vertLine() const
{
    return m_vertLine;
}

void GLDCustomGridSetting::setVertLine(bool value)
{
    m_vertLine = value;
}

bool GLDCustomGridSetting::horzLine() const
{
    return m_horzLine;
}

void GLDCustomGridSetting::setHorzLine(bool value)
{
    m_horzLine = value;
}

bool GLDCustomGridSetting::rowSizing() const
{
    return m_rowSizing;
}

void GLDCustomGridSetting::setRowSizing(bool value)
{
    m_rowSizing = value;
}

bool GLDCustomGridSetting::colSizing() const
{
    return m_colSizing;
}

void GLDCustomGridSetting::setColSizing(bool value)
{
    m_colSizing = value;
}
bool GLDCustomGridSetting::colMoving() const
{
    return m_colMoving;
}

void GLDCustomGridSetting::setColMoving(bool value)
{
    m_colMoving = value;
}

bool GLDCustomGridSetting::allowSelectRow() const
{
    return m_allowSelectRow;
}

void GLDCustomGridSetting::setAllowSelectRow(bool value)
{
    m_allowSelectRow = value;
}

bool GLDCustomGridSetting::allowSelectCol() const
{
    return m_allowSelectCol;
}

void GLDCustomGridSetting::setAllowSelectCol(bool value)
{
    m_allowSelectCol = value;
}

bool GLDCustomGridSetting::allowSelectAll() const
{
    return m_allowSelectAll;
}

void GLDCustomGridSetting::setAllowSelectAll(bool value)
{
    m_allowSelectAll = value;
}

bool GLDCustomGridSetting::multiSelection() const
{
    return m_multiSelection;
}

void GLDCustomGridSetting::setMultiSelection(bool value)
{
    m_multiSelection = value;
}

GString GLDCustomGridSetting::invalidValueLabel() const
{
    return m_invalidValueLabel;
}

void GLDCustomGridSetting::setInvalidValueLabel(GString value)
{
    m_invalidValueLabel = value;
}

bool GLDCustomGridSetting::alwaysShowEditor() const
{
    return m_alwaysShowEditor;
}

void GLDCustomGridSetting::setAlwaysShowEditor(bool value)
{
    m_alwaysShowEditor = value;
}

bool GLDCustomGridSetting::rangeSelect() const
{
    return m_rangeSelect;
}

void GLDCustomGridSetting::setRangeSelect(bool value)
{
    m_rangeSelect = value;
}

bool GLDCustomGridSetting::autoThemeAdapt() const
{
    return m_autoThemeAdapt;
}

void GLDCustomGridSetting::setAutoThemeAdapt(bool value)
{
    m_autoThemeAdapt = value;
}

bool GLDCustomGridSetting::returnKeyAsTab() const
{
    return m_returnKeyAsTab;
}

void GLDCustomGridSetting::setReturnKeyAsTab(bool value)
{
    m_returnKeyAsTab = value;
}

bool GLDCustomGridSetting::showFixedRow() const
{
    return m_showFixedRow;
}

void GLDCustomGridSetting::setShowFixedRow(bool value)
{
    m_showFixedRow = value;
}

bool GLDCustomGridSetting::cellFilling() const
{
    return m_cellFilling;
}

void GLDCustomGridSetting::setCellFilling(bool value)
{
    m_cellFilling = value;
}

bool GLDCustomGridSetting::fixedCellEvent() const
{
    return m_fixedCellEvent;
}

void GLDCustomGridSetting::setFixedCellEvent(bool value)
{
    m_fixedCellEvent = value;
}

bool GLDCustomGridSetting::allow3DStyle() const
{
    return m_allow3DStyle;
}

void GLDCustomGridSetting::setAllow3DStyle(bool value)
{
    m_allow3DStyle = value;
}

bool GLDCustomGridSetting::allowCopyPaste() const
{
    return m_allowCopyPaste;
}
void GLDCustomGridSetting::setAllowCopyPaste(bool value)
{
    m_allowCopyPaste = value;
}

bool GLDCustomGridSetting::borderStyle() const
{
    return m_borderStyle;
}

void GLDCustomGridSetting::setBorderStyle(bool value)
{
    m_borderStyle = value;
}

bool GLDCustomGridSetting::hideEditOnExit() const
{
    return m_hideEditOnExit;
}

void GLDCustomGridSetting::setHideEditOnExit(bool value)
{
    m_hideEditOnExit = value;
}

bool GLDCustomGridSetting::useBlendColor() const
{
    return m_useBlendColor;
}

void GLDCustomGridSetting::setUseBlendColor(bool value)
{
    m_useBlendColor = value;
}

bool GLDCustomGridSetting::cellFillEditField() const
{
    return m_cellFillEditField;
}

void GLDCustomGridSetting::setCellFillEditField(bool value)
{
    m_cellFillEditField = value;
}

CGLDExtPropDefs *GLDCustomGridSetting::extPropDefs() const
{
    return m_extProps;
}

PtrInt GLDCustomGridSetting::tag() const
{
    return m_tag;
}

void GLDCustomGridSetting::setTag(PtrInt tag)
{
    m_tag = tag;
}

GLDGridSettingNotifyEventList *GLDCustomGridSetting::afterActiveEventList()
{
    return m_afterActiveEventList;
}

GString GLDCustomGridSetting::fileName()
{
    //assert(m_persistentStyle == psFromFile);
    return m_persistentValue;
}

void GLDCustomGridSetting::doAfterActive()
{
    assert(m_active);
    assert(m_loaded);

    if (!m_afterActiveEventList)
    {
        return;
    }

    for (int i = 0; i < m_afterActiveEventList->count(); i++)
    {
        m_afterActiveEventList->at(i)->doEvent(this);
    }
}

void GLDCustomGridSetting::doBeforeDeactive()
{
    //todo yaok
    /*
    assert(m_active);
    beforeDeactiveEventList_->doEvent(this);
    */
}

void GLDCustomGridSetting::loaded()
{
    if (m_loaded)
    {
        return;
    }

    if (m_active)
    {
        loadGridSetting();
    }
}

void GLDCustomGridSetting::doLoad()
{
    GXMLDocument xmlDoc;
    clearData();

    if (m_persistentStyle == gpsFromDFM)
    {
        if (m_persistentValue != "")
        {
            doLoadFromXML(xmlDoc);
        }
    }
    else
    {
        doLoadFromFile(m_persistentValue);
    }

    m_active = true;
    m_loaded = true;
}

void GLDCustomGridSetting::doUnload()
{
    doBeforeDeactive();
    clearData();
    m_active = false;
    m_loaded = false;
}

void GLDCustomGridSetting::refreshRule(GLDGridRule *rule)
{
    rule->enable();
    /* todo yaok
    int i;
    for (i = 0; i <= clients_.count() - 1; i++)
      {
        assert(dynamic_cast< GSPCustomGrid *>(((TObject *) clients_[i])));
       ((GSPCustomGridHack *) clients_[iI])->refreshRule(rule);
      }
     */
}

void GLDCustomGridSetting::designing_DfmToFile()
{
    /* todo yaok 这个函数在CPP中还有用吗？
    IXMLDocument iDoc;
    assert(m_persistentValue != "");
    assert(ComponentState.contains(csDesigning));
    assert(m_persistentStyle == psFromDFM);
    if (confirmDlg("是否将DFM中的内容保存到文件？") == IDYES)
    {
      with TSaveDialog.create(nil) do
      try
      {
        setDefaultExt(getSaveDialogDefaultExt());
        setFilter(getSaveDialogFilter());
        if (!execute())
          m_persistentValue = "";
        else
          {
            iDoc = createXMLDocument(true, true);
            iDoc.XML = m_persistentValue;
            iDoc.saveToFile(Files[0]);
            m_persistentValue = Files[0];
          }
      }
      __finally
      {
        Free;
      }
    }
    else
    {
    m_persistentValue = "";
    }
     */
}

void GLDCustomGridSetting::designing_FileToDfm()
{
    /* todo yaok 这个函数在CPP中还有用吗？
    IXMLDocument iDoc;
    assert(m_persistentValue != "");
    assert(ComponentState.contains(csDesigning));
    assert(m_persistentStyle == psFromFile);
    if (fileExists(FileName) &&(confirmDlg("是否将文件内容装载到DFM？") == IDYES))
    {
      iDoc = createXMLDocument(true, true);
      iDoc.loadFromFile(m_persistentValue);
      m_persistentValue = iDoc.XML;
    }
    else
    {
      m_persistentValue = "";
    }
     */
}

/* GLDGridSetting */

GLDGridSetting::GLDGridSetting() : GLDCustomGridSetting(), m_allowSort(false),
    m_sortFieldName(""), m_ascImageIndex(-1), m_descImageIndex(-1), m_totalRowAtFooter(true),
    m_colSettings(NULL), m_titleRows(NULL), m_filterRows(NULL), m_tableSettings(NULL),
    m_controlRef(0)
{
    m_extProps = new CGLDExtPropDefs;
    m_extProps->AddRef();
}

GLDGridSetting::~GLDGridSetting()
{
    setActive(false);
    //todo yaok TGSPDBComponent 方法
    //clearClientRefs();
    freeAndNil(m_colSettings);
    freeAndNil(m_titleRows);
    freeAndNil(m_filterRows);
    freeAndNil(m_tableSettings);
    m_extProps->Release();
}

void GLDGridSetting::initialize()
{
    m_colSettings = new GLDColSettings(this);
    m_titleRows = new GLDTitleRows(this);
    m_titleRows->insert(-1);
    m_filterRows = new GLDFilterRows(this);
    m_tableSettings = new GLDTableSettings(this);
}

GLDGridSetting *GLDGridSetting::createGLDGridSetting()
{
    GLDGridSetting *result = new GLDGridSetting();
    result->initialize();
    return result;
}

void GLDGridSetting::clearData()
{
//    m_colSettings->clearCompileInfo();
//    m_titleRows->clearCompileInfo();
//    m_filterRows->clearCompileInfo();
//    m_tableSettings->clearCompileInfo();
//    m_extProps->clearCompileInfo();

    m_titleRows->clear();
    m_filterRows->clear();
    m_colSettings->clear();
    m_tableSettings->clear();
}

void GLDGridSetting::saveToFile(const GString &fileName)
{
    GLDGridSettingXMLWriter writer;
    writer.write(fileName, this);
}

void GLDGridSetting::loadFromStream(GStream *stream)
{
    // 先清ColSetting,否则会把FixedCols清掉
    if (m_colSettings != NULL)
    {
        m_colSettings->clear();
    }

    GLDCustomGridSetting::loadFromStream(stream);

    readBoolFromStream(stream, m_allowSort);
    readStrFromStream(stream, m_sortFieldName);
    readIntFromStream(stream, m_ascImageIndex);
    readIntFromStream(stream, m_descImageIndex);
    readBoolFromStream(stream, m_totalRowAtFooter);
    m_colSettings->loadFromStream(stream);
    m_titleRows->loadFromStream(stream);
    m_filterRows->loadFromStream(stream);
    m_tableSettings->loadFromStream(stream);
}

void GLDGridSetting::saveToStream(GStream *stream)
{
    GLDCustomGridSetting::saveToStream(stream);
    writeBoolToStream(stream, m_allowSort);
    writeStrToStream(stream, m_sortFieldName);
    writeIntToStream(stream, m_ascImageIndex);
    writeIntToStream(stream, m_descImageIndex);
    writeBoolToStream(stream, m_totalRowAtFooter);
    m_colSettings->saveToStream(stream);
    m_titleRows->saveToStream(stream);
    m_filterRows->saveToStream(stream);
    m_tableSettings->saveToStream(stream);
}

void GLDGridSetting::compileExpandRow()
{
    if (m_tableSettings->count() == 0)
    {
        return;
    }

    for (int i = 0; i <= m_tableSettings->items(0)->headerRowCount() - 1; i++)
    {
        for (int j = 0; j <= m_tableSettings->items(0)->headerRows(i)->count() - 1; j++)
        {
            m_tableSettings ->items(0)->headerRows(i)->items(j)->compile();
        }
    }

    for (int i = 0; i <= m_tableSettings->items(0)->totalRowCount() - 1; i++)
    {
        for (int j = 0; j <= m_tableSettings ->items(0)->totalRows(i)->count() - 1; j++)
        {
            m_tableSettings->items(0)->totalRows(i)->items(j)->compile();
        }
    }
}

bool GLDGridSetting::allowSort() const
{
    return m_allowSort;
}

void GLDGridSetting::setAllowSort(bool value)
{
    m_allowSort = value;
}

GString GLDGridSetting::sortFieldNames() const
{
    return m_sortFieldName;
}

void GLDGridSetting::setSortFieldNames(GString value)
{
    m_sortFieldName = value;
}

int GLDGridSetting::ascImageIndex() const
{
    return m_ascImageIndex;
}

void GLDGridSetting::setAscImageIndex(int value)
{
    m_ascImageIndex = value;
}

int GLDGridSetting::descImageIndex() const
{
    return m_descImageIndex;
}

void GLDGridSetting::setDescImageIndex(int value)
{
    m_descImageIndex = value;
}

bool GLDGridSetting::totalRowAtFooter() const
{
    return m_totalRowAtFooter;
}

void GLDGridSetting::setTotalRowAtFooter(bool value)
{
    m_totalRowAtFooter = value;
}

GLDColSettings *GLDGridSetting::colSettings() const
{
    return m_colSettings;
}

GLDTitleRows *GLDGridSetting::titleRows() const
{
    return m_titleRows;
}

GLDFilterRows *GLDGridSetting::filterRows() const
{
    return m_filterRows;
}

GLDTableSettings *GLDGridSetting::tableSettings() const
{
    return m_tableSettings;
}

void GLDGridSetting::setTableSettings(GLDTableSettings *pTableSettings)
{
    m_tableSettings = pTableSettings;
}

ULONG GLDGridSetting::AddControlRef()
{
    return m_controlRef++;
}

ULONG GLDGridSetting::ReleaseControlRef()
{
    m_controlRef--;

    if (m_controlRef == 0)
    {
        delete this;
        return ULONG(0);
    }
    else
    {
        return m_controlRef;
    }
}

void GLDGridSetting::compile()
{
    m_colSettings->compile();
    m_titleRows->compile();
    m_filterRows->compile();
    m_tableSettings->compile();
    m_extProps->compile();
}

void GLDGridSetting::clearCompileInfo()
{
    m_colSettings->clearCompileInfo();
    m_titleRows->clearCompileInfo();
    m_filterRows->clearCompileInfo();
    m_tableSettings->clearCompileInfo();

    m_extProps->clearCompileInfo();
}

void GLDGridSetting::loadGridSetting()
{
    doLoad();
    doAfterActive();
}

void GLDGridSetting::doLoadFromXML(GXMLDocument &doc)
{
    GLDGridSettingXMLReader oBuilder;
    oBuilder.read(doc, this);
}

void GLDGridSetting::doLoadFromFile(const GString &fileName)
{
    GLDGridSettingXMLReader oBuilder;
    oBuilder.read(fileName, this);
}

void GLDGridSetting::doSaveToXML(GXMLDocument &doc)
{
    GLDGridSettingXMLWriter oWriter;
    oWriter.write(doc, this);
}

GString GLDGridSetting::saveDialogDefaultExt()
{
    return "GSF";
}

GString GLDGridSetting::saveDialogFilter()
{
    return "GridSetting files(*.GSF)|*.GSF";
}

/* GLDColSettings */

GLDColSettings::GLDColSettings(GLDGridSetting *owner)
{
    m_owner = owner;
}

GLDColSettings::~GLDColSettings()
{
    clear();
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 清除列设置数据
-----------------------------------------------------------------------------*/
void GLDColSettings::clear()
{
    for (int i = count() - 1; i >= 0; i--)
    {
        Delete(i);
    }
}

int GLDColSettings::indexOf(GLDColSetting *colSetting) const
{
    return m_list.indexOf(colSetting);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：删除指定下标列
  参数：index -- 列下标
-----------------------------------------------------------------------------*/
void GLDColSettings::Delete(int index)
{
    assert((index >= 0) && (index < count()));
    bool bIsFixedCol = items(index)->isFixedCol();
    // 删除列设置对象
    GLDColSetting *oColSetting = items(index);
    m_list.Delete(index);
    freeAndNil(oColSetting);

    // 删除字段设置列
    for (int i = 0; i <= m_owner->tableSettings()->count() - 1; i++)
    {
        m_owner->tableSettings()->items(i)->fieldSettings()->Delete(index);
    }

    // 删除带区标题行字段设置
    for (int i = 0; i <= m_owner->tableSettings()->count() - 1; i++)
    {
        for (int j = 0; j <= m_owner->tableSettings()->items(i)->headerRowCount() - 1; j++)
        {
            m_owner->tableSettings()->items(i)->headerRows(j)->Delete(index);
        }
    }

    // 删除合计行字段设置
    for (int i = 0; i <= m_owner->tableSettings()->count() - 1; i++)
    {
        for (int j = 0; j <= m_owner->tableSettings()->items(i)->totalRowCount() - 1; j++)
        {
            m_owner->tableSettings()->items(i)->totalRows(j)->Delete(index);
        }
    }

    // 删除标题行单元格
    for (int i = 0; i <= m_owner->titleRows()->count() - 1; i++)
    {
        m_owner->titleRows()->items(i)->Delete(index);
    }

    // 删除过滤行单元格
    for (int i = 0; i <= m_owner->filterRows()->count() - 1; i++)
    {
        m_owner->filterRows()->items(i)->Delete(index);
    }

    if (bIsFixedCol)
    {
        m_owner->setFixedCols(m_owner->fixedCols() - 1);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：插入列
  参数：index -- 列位置
  返回：插入的列设置对象
-----------------------------------------------------------------------------*/
GLDColSetting *GLDColSettings::insert(int index)
{
    // 插入列设置对象
    GLDColSetting *result = createColSetting(this);

    if (index == -1)
    {
        index = count();
    }

    m_list.insert(index, result);

    // 插入字段设置
    for (int i = 0; i != m_owner->tableSettings()->count(); i++)
    {
        GLDFieldSettings *oFieldSettings = m_owner->tableSettings()->items(i)->fieldSettings();
        oFieldSettings->list().insert(index, createFieldSetting(oFieldSettings));

        for (int j = 0; j <= m_owner->tableSettings()->items(i)->headerRowCount() - 1; j++)
        {
            GLDHeaderRowFieldSettings *oHeaderRowFieldSettings = m_owner->tableSettings()->items(i)->headerRows(j);
            oHeaderRowFieldSettings->list().insert(index, createHeaderRowFieldSetting(oHeaderRowFieldSettings));
        }

        for (int j = 0; j <= m_owner->tableSettings()->items(i)->totalRowCount() - 1; j++)
        {
            GLDTotalRowFieldSettings *oTotalRowFieldSettings = m_owner->tableSettings()->items(i)->totalRows(j);
            oTotalRowFieldSettings->list().insert(index, createTotalRowFieldSetting(oTotalRowFieldSettings));
        }
    }

    // 插入标题行单元格
    for (int i = 0; i <= m_owner->titleRows()->count() - 1; i++)
    {
        m_owner->titleRows()->items(i)->list().insert(index, createTitleCell(m_owner->titleRows()->items(i)));
    }

    // 插入过滤行单元格
    for (int i = 0; i <= m_owner->filterRows()->count() - 1; i++)
    {
        m_owner->filterRows()->items(i)->list().insert(index, createFilterCell(m_owner->filterRows()->items(i)));
    }

    return result;
}

GLDGridSetting *GLDColSettings::owner() const
{
    return m_owner;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDColSettings::loadFromStream(GStream *stream)
{
    int nInts(0);
    readIntFromStream(stream, nInts);
    clear();

    for (int i = 0; i <= nInts - 1; i++)
    {
        insert()->loadFromStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：移动列
  参数：fromIndex -- 源下标
        toIndex -- 目标下标
-----------------------------------------------------------------------------*/
void GLDColSettings::move(int fromIndex, int toIndex)
{
    //assert((fromIndex >= 0) &&(fromIndex < count()));
    //assert((toIndex >= 0) &&(toIndex < count()));
    if (fromIndex == toIndex)
    {
        return;
    }

    // 移动列设置对象
    m_list.move(fromIndex, toIndex);

    // 移动字段设置对象
    for (int i = 0; i <= m_owner->tableSettings()->count() - 1; i++)
    {
        m_owner->tableSettings()->items(i)->fieldSettings()->list().move(fromIndex, toIndex);
    }

    // 移动带区标题行字段设置对象
    for (int i = 0; i <= m_owner->tableSettings()->count() - 1; i++)
    {
        for (int j = 0; j <= m_owner->tableSettings()->items(i)->headerRowCount() - 1; j++)
        {
            m_owner->tableSettings()->items(i)->headerRows(j)->list().move(fromIndex, toIndex);
        }
    }

    // 移动合计行字段设置对象
    for (int i = 0; i <= m_owner->tableSettings()->count() - 1; i++)
    {
        for (int j = 0; j <= m_owner->tableSettings()->items(i)->totalRowCount() - 1; j++)
        {
            m_owner->tableSettings()->items(i)->totalRows(j)->list().move(fromIndex, toIndex);
        }
    }

    // 移动标题单元格
    for (int i = 0; i <= m_owner->titleRows()->count() - 1; i++)
    {
        m_owner->titleRows()-> items(i)->list().move(fromIndex, toIndex);
    }

    // 移动过滤单元格
    for (int i = 0; i <= m_owner->filterRows()->count() - 1; i++)
    {
        m_owner->filterRows()->items(i)->list().move(fromIndex, toIndex);
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDColSettings::saveToStream(GStream *stream)
{
    writeIntToStream(stream, count());

    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->saveToStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：设置总列数
  参数：value -- 总列数
-----------------------------------------------------------------------------*/
void GLDColSettings::setCount(int value)
{
    int nSize = (int)m_list.size();

    if (value == nSize)
    {
        return;
    }

    if (value > nSize)
    {
        for (int i = nSize; i <= value - 1; i++)
        {
            m_list.push_back(createColSetting(this));
        }
    }

    for (int i = 0; i <= m_owner->tableSettings()->count() - 1; i++)
    {
        m_owner->tableSettings()->items(i)->fieldSettings()->setCount(value);
    }

    for (int i = 0; i <= m_owner->titleRows()->count() - 1; i++)
    {
        m_owner->titleRows()->items(i)->setCellCount(value);
    }

    for (int i = 0; i <= m_owner->filterRows()->count() - 1; i++)
    {
        m_owner->filterRows()->items(i)->setCellCount(value);
    }
}

GLDColSetting *GLDColSettings::items(int index) const
{
    return m_list[index];
}

GLDColSetting *GLDColSettings::operator[](int index)
{
    return m_list[index];
}

GObjectList<GLDColSettingVector *> &GLDColSettings::visibleCondItems()
{
    return m_visibleCondItems;
}

int GLDColSettings::findParserIndex(const GString &expr)
{
    Q_UNUSED(expr);
    return -1;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回可视列数
  返回：可视列数
-----------------------------------------------------------------------------*/
int GLDColSettings::visibleColCount()
{
    int result = 0;

    for (int i = 0; i <= count() - 1; i++)
    {
        if (items(i)->visible())
        {
            ++result;
        }
    }

    return result;
}

GLDColSetting *GLDColSettings::findColSetting(const GString &identity)
{
    for (int i = 0; i <= count() - 1; i++)
    {
        if (items(i)->fullIdentity().toLower() == identity.toLower())
        {
            return items(i);
        }
    }

    return NULL;
}

int GLDColSettings::fixedColCount()
{
    int result = 0;

    for (int i = 0; i <= count() - 1; i++)
    {
        if (items(i)->isFixedCol())
        {
            result++;
        }
    }

    return result;
}

GIntList GLDColSettings::suitRowHeightCols()
{
    GIntList oResult;
    int nFixedColCount = fixedColCount();

    for (int i = 0; i <= count() - 1; ++i)
    {
        if (items(i)->cellSuitType() == gcstSuitRowHeight)
        {
            oResult.push_back(i - nFixedColCount);
        }
    }

    return oResult;
}

GIntList GLDColSettings::suitColWidthCols()
{
    GIntList oResult;
    int nFixedColCount = fixedColCount();

    for (int i = 0; i <= count() - 1; i++)
    {
        if (items(i)->cellSuitType() == gcstSuitColWidth)
        {
            oResult.push_back(i - nFixedColCount);
        }
    }

    return oResult;
}

GIntList GLDColSettings::fitColWidthCols()
{
    GIntList oResult;
    int nFixedColCount = fixedColCount();

    for (int i = 0; i <= count() - 1; i++)
    {
        if (items(i)->cellSuitType() == gcstFitColWidth)
        {
            oResult.push_back(i - nFixedColCount);
        }
    }

    return oResult;
}

int GLDColSettings::count() const
{
    return (int)m_list.size();
}

void GLDColSettings::compile()
{
    int nIndex(0);
    GLDColSettingVector *oList = NULL;
    // 必须存在显示行
    bool bHasVisibleCol = false;

    for (int i = 0; i <= count() - 1; i++)
    {
        if (!items(i)->isFixedCol() && items(i)->visible())
        {
            bHasVisibleCol = true;
            break;
        }
    }

    if (!bHasVisibleCol)
    {
        throw GLDColSettingException(getGSPi18nStr(g_NotExistVisibleCol));
    }

    // 编译每列
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->compile();
    }

    // 编译列显示条件表达式
    // 把列显示表达式放在容器层处理，主要目的是为了优化（多个列的显示条件一样）
    for (int i = 0; i <= count() - 1; i++)
    {
        GString sExpr = trimExpression(items(i)->visibleCondition());

        if (sExpr != "")
        {
            nIndex = findParserIndex(sExpr);

            if (nIndex == -1)
            {
                //todo yaok event
                // oParser->setOnRefreshValue(doRefreshValue());
                oList = new GLDColSettingVector();
                oList->push_back(items(i));
                m_visibleCondItems.push_back(oList);
            }
            else
            {
                oList = m_visibleCondItems[nIndex];
                oList->push_back(items(i));
            }
        }
    }
}

GLDColSetting *GLDColSettings::createColSetting(GLDColSettings *owner)
{
    return new GLDColSetting(owner);
}

GLDFieldSetting *GLDColSettings::createFieldSetting(GLDFieldSettings *owner)
{
    return new GLDFieldSetting(owner);
}

GLDHeaderRowFieldSetting *GLDColSettings::createHeaderRowFieldSetting(GLDHeaderRowFieldSettings *owner)
{
    return new GLDHeaderRowFieldSetting(owner);
}

GLDTotalRowFieldSetting *GLDColSettings::createTotalRowFieldSetting(GLDExpandRowFieldSettings *owner)
{
    return new GLDTotalRowFieldSetting(owner);
}

GLDTitleCell *GLDColSettings::createTitleCell(GLDTitleRow *owner)
{
    return new GLDTitleCell(owner);
}

GLDFilterCell *GLDColSettings::createFilterCell(GLDFilterRow *owner)
{
    return new GLDFilterCell(owner);
}

void GLDColSettings::clearClonedColSetting()
{
    for (int i = count() - 1; i >= 0; i--)
    {
        if (items(i)->cloneFlag())
        {
            Delete(i);
        }
    }
}

void GLDColSettings::clearCompileInfo()
{
    m_visibleCondItems.clear();

    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->clearCompileInfo();
    }
}

void GLDColSettings::clone(GLDColSetting *protoType, int insertIndex, int count)
{
    GMemoryStream oStream;
    int i, j, k, nSourceIndex, nTargetIndex;

    //assert((insertIndex >= 0) &&(insertIndex <= count()));
    //assert(protoType != NULL);
    //assert(count() > 0);
    for (k = 0; k <= count - 1; k++)
    {
        GLDColSetting *oColSetting = insert(insertIndex);
        oColSetting->setCloneFlag(true);
        nSourceIndex = protoType->index();
        nTargetIndex = oColSetting->index();

        // 克隆列配置
        oStream.seek(0);
        protoType->saveToStream(&oStream);
        oStream.seek(0);
        oColSetting->loadFromStream(&oStream);

        // 克隆标题行
        for (i = 0; i <= m_owner->titleRows()->count() - 1; i++)
        {
            oStream.seek(0);
            m_owner->titleRows()->items(i)->cells(nSourceIndex)->saveToStream(&oStream);
            oStream.seek(0);
            m_owner->titleRows()->items(i)->cells(nTargetIndex)->loadFromStream(&oStream);
        }

        // 克隆过滤行
        for (i = 0; i <= m_owner->filterRows()->count() - 1; i++)
        {
            oStream.seek(0);
            m_owner->filterRows()->items(i)->cells(nSourceIndex)->saveToStream(&oStream);
            oStream.seek(0);
            m_owner->filterRows()->items(i)->cells(nTargetIndex)->loadFromStream(&oStream);
        }

        // 克隆表
        for (i = 0; i <= m_owner->tableSettings()->count() - 1; i++)
        {
            oStream.seek(0);
            m_owner->tableSettings()->items(i)->fieldSettings()->items(nSourceIndex)->saveToStream(&oStream);
            oStream.seek(0);
            m_owner->tableSettings()->items(i)->fieldSettings()->items(nTargetIndex)->loadFromStream(&oStream);

            for (j = 0; j <= m_owner->tableSettings()->items(i)->headerRowCount() - 1; j++)
            {
                oStream.seek(0);
                m_owner->tableSettings()->items(i)->headerRows(j)->items(nSourceIndex)->saveToStream(&oStream);
                oStream.seek(0);
                m_owner->tableSettings()->items(i)->headerRows(j)->items(nTargetIndex)->loadFromStream(&oStream);
            }

            for (j = 0; j <= m_owner->tableSettings()->items(i)->totalRowCount() - 1; j++)
            {
                oStream.seek(0);
                m_owner->tableSettings()->items(i)->totalRows(j)->items(nSourceIndex)->saveToStream(&oStream);
                oStream.seek(0);
                m_owner->tableSettings()->items(i)->totalRows(j)->items(nTargetIndex)->loadFromStream(&oStream);
            }
        }
    }
}

/* GLDColSetting */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDColSetting::GLDColSetting(GLDColSettings *owner)
    : m_owner(owner),
      m_displayWidth(CDefDisplayWidth),
      m_calcedWidth(CDefDisplayWidth),
      m_zoomWidth(0),
      m_originWidth(0),
      m_originIndex(0),
      m_visible(true),
      m_enabled(true),
      m_cloneFlag(false),
      m_crossExtFlag(false),
      m_cellSuitType(gcstFixed),
      m_extProps(new CGLDExtPropDefs()),
      m_tag(0)

{
    m_extProps->AddRef();
}

GLDColSetting::~GLDColSetting()
{
    m_extProps->Release();
}

void GLDColSetting::setDisplayWidth(int value)
{
    if (value < 0)
    {
        m_displayWidth = 0;              // 自动列宽
    }
    else
    {
        m_displayWidth = value;
    }

    m_calcedWidth = m_displayWidth;
}

void GLDColSetting::setVisibleCondition(const GString &value)
{
    if (m_owner->owner()->designState())
    {
        m_visibleCondition = trim(value);
    }
    else
    {
        gldError(getGLDi18nStr(g_NeedRunState));
    }
}

int GLDColSetting::index()
{
    return m_owner->indexOf(this);
}

bool GLDColSetting::isFixedCol()
{
    int nIndex;
    nIndex = m_owner->indexOf(this);
    return nIndex < m_owner->owner()->fixedCols();
}

void GLDColSetting::refreshVisibleState(int col)
{
    if (-1 == col)
    {
        return;
    }
}

void GLDColSetting::compile()
{
}

void GLDColSetting::clearCompileInfo()
{
}

GString GLDColSetting::fullIdentity()
{
    GString result = m_identity;

    if ((result == "") && (m_owner->owner()->titleRows()->count() > 0))
    {
        result = m_owner->owner()->titleRows()->items(0)->cells(index())->caption();

        for (int i = 1; i <= m_owner->owner()->titleRows()->count() - 1; i++)
        {
            if (!sameText(m_owner->owner()->titleRows()->items(i)->cells(index())->caption(),
                          m_owner->owner()->titleRows()->items(i - 1)->cells(index())->caption()))
            {
                result = result + "|" + m_owner->owner()->titleRows()->items(i)->cells(index())->caption();
            }
        }
    }

    return result;
}

int GLDColSetting::displayWidth() const
{
    return m_displayWidth;
}

bool GLDColSetting::enabled() const
{
    return m_enabled;
}

void GLDColSetting::setEnabled(bool value)
{
    m_enabled = value;
}

int GLDColSetting::calcedWidth() const
{
    return m_calcedWidth;
}

void GLDColSetting::setCalcedWidth(int value)
{
    m_calcedWidth = value;
}

int GLDColSetting::zoomWidth() const
{
    return m_zoomWidth;
}

void GLDColSetting::setZoomWidth(int value)
{
    m_zoomWidth = value;
}

int GLDColSetting::originWidth() const
{
    return m_originWidth;
}

void GLDColSetting::setOriginWidth(int value)
{
    m_originWidth = value;
}

int GLDColSetting::originIndex() const
{
    return m_originIndex;
}

void GLDColSetting::setOriginIndex(int value)
{
    m_originIndex = value;
}

GString GLDColSetting::visibleCondition() const
{
    return m_visibleCondition;
}

bool GLDColSetting::visible() const
{
    return m_visible && m_enabled;
}

void GLDColSetting::setVisible(bool value)
{
    m_visible = value;
}

GLDCellSuitType GLDColSetting::cellSuitType() const
{
    return m_cellSuitType;
}

void GLDColSetting::setCellSuitType(GLDCellSuitType value)
{
    m_cellSuitType = value;
}

GString GLDColSetting::identity() const
{
    return m_identity;
}

void GLDColSetting::setIdentity(GString value)
{
    m_identity = value;
}

GString GLDColSetting::extData() const
{
    return m_extData;
}

void GLDColSetting::setExtData(GString value)
{
    m_extData = value;
}

CGLDExtPropDefs *GLDColSetting::extPropDefs() const
{
    return m_extProps;
}

bool GLDColSetting::cloneFlag() const
{
    return m_cloneFlag;
}

void GLDColSetting::setCloneFlag(bool value)
{
    m_cloneFlag = value;
}

bool GLDColSetting::crossExtFlag() const
{
    return m_crossExtFlag;
}

void GLDColSetting::setCrossExtFlag(bool value)
{
    m_crossExtFlag = value;
}

PtrInt GLDColSetting::tag() const
{
    return m_tag;
}

void GLDColSetting::setTag(PtrInt value)
{
    m_tag = value;
}

GLDGridSetting *GLDColSetting::gridSetting() const
{
    return m_owner->owner();
}

GLDColSettings *GLDColSetting::owner()
{
    return m_owner;
}

void GLDColSetting::loadFromStream(GStream *stream)
{
    int nCellSuitType;
    readIntFromStream(stream, m_displayWidth);
    readBoolFromStream(stream, m_visible);
    readStrFromStream(stream, m_visibleCondition);
    readIntFromStream(stream, nCellSuitType);
    m_cellSuitType = ((GLDCellSuitType) nCellSuitType);
    readStrFromStream(stream, m_identity);
    readIntFromStream(stream, m_calcedWidth);
    readStrFromStream(stream, m_extData);
    //todo yaok
    //GSPExtPropDefsStreamBuilder::read(stream, m_extProps);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDColSetting::saveToStream(GStream *stream)
{
    writeIntToStream(stream, m_displayWidth);
    writeBoolToStream(stream, m_visible);
    writeStrToStream(stream, m_visibleCondition);
    writeIntToStream(stream, (int)m_cellSuitType);
    writeStrToStream(stream, m_identity);
    writeIntToStream(stream, m_calcedWidth);
    writeStrToStream(stream, m_extData);
    //todo yaok
    //GSPExtPropDefsStreamBuilder::write(stream, m_extProps);
}

/* GLDTableSetting */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数
-----------------------------------------------------------------------------*/
void GLDTableSetting::initialize()
{
    m_rules = new GLDGridRules(this);
    m_fieldSettings = new GLDFieldSettings(this);
    m_fieldSettings->setCount(m_owner->owner()->colSettings()->count());
    m_extProps = new CGLDExtPropDefs();
    m_extProps->AddRef();
}

GLDTableSetting *GLDTableSetting::createTableSetting(GLDTableSettings *owner)
{
    GLDTableSetting *result = new GLDTableSetting(owner);
    result->initialize();
    return result;
}

GLDHeaderRowFieldSettings *GLDTableSetting::createHeaderRowFieldSettings(GLDTableSetting *owner)
{
    return new GLDHeaderRowFieldSettings(owner);
}

GLDTotalRowFieldSettings *GLDTableSetting::createTotalRowFieldSettings(GLDTableSetting *owner)
{
    return new GLDTotalRowFieldSettings(owner);
}

GLDTableSetting::GLDTableSetting(GLDTableSettings *owner)
{
    m_owner = owner;
    m_defRowHeight = CDefRowHeight;
    m_isTree = true;
    m_isTreeWithMasterView = true;
    m_cloneFlag = false;
    m_customDataSource = false;
    m_needExpandTotalExpr = false;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数
-----------------------------------------------------------------------------*/
GLDTableSetting::~GLDTableSetting()
{
    freeAndNil(m_rules);
    freeAndNil(m_fieldSettings);
    clearTotalRows();
    clearHeaderRows();
    m_extProps->Release();
}

GLDTableSettings *GLDTableSetting::owner() const
{
    return m_owner;
}

void GLDTableSetting::clearTotalRows()
{
    for (int i = totalRowCount() - 1; i >= 0; i--)
    {
        deleteTotalRow(i);
    }
}

void GLDTableSetting::clearHeaderRows()
{
    for (int i = headerRowCount() - 1; i >= 0; i--)
    {
        deleteHeaderRow(i);
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2006-01-05
  功能: 返回RefDatabaseName
-----------------------------------------------------------------------------*/
GString GLDTableSetting::refFullName()
{
    GString result;

    if (m_refDatabaseName != "")
    {
        result = m_refDatabaseName + "." + m_refTableName;
    }
    else
    {
        result = m_refTableName;
    }

    return result;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2006-01-19
  功能: 根据下标取合计行设置
  参数: index -- 下标
  返回: 合计行设置
-----------------------------------------------------------------------------*/
GLDTotalRowFieldSettings *GLDTableSetting::totalRows(int index)
{
    //assert((index >= 0) &&(index < m_totalRows.count()()));
    return m_totalRows[index];
}

void GLDTableSetting::deleteHeaderRow(int index)
{
    assert((index >= 0) && (index < m_headerRows.count()));
    m_headerRows.removeAt(index);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2006-01-19
  功能: 删除合计行
  参数: index -- 合计行下标
-----------------------------------------------------------------------------*/
void GLDTableSetting::deleteTotalRow(int index)
{
    assert((index >= 0) && (index < m_totalRows.count()));
    m_totalRows.removeAt(index);
}

GLDHeaderRowFieldSettings *GLDTableSetting::insertHeaderRow(int index)
{
    GLDHeaderRowFieldSettings *result;
    result = createHeaderRowFieldSettings(this);
    result->setCount(m_owner->owner()->colSettings()->count());

    if (index == -1)
    {
        m_headerRows.push_back(result);
    }
    else
    {
        m_headerRows.insert(index, result);
    }

    return result;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2006-01-19
  功能: 插入合计行
  参数: index -- 插入的下标
  返回: 合计行对象
-----------------------------------------------------------------------------*/
GLDTotalRowFieldSettings *GLDTableSetting::insertTotalRow(int index)
{
    GLDTotalRowFieldSettings *result = createTotalRowFieldSettings(this);
    result->setCount(m_owner->owner()->colSettings()->count());

    if (index == -1)
    {
        m_totalRows.push_back(result);
    }
    else
    {
        m_totalRows.insert(index, result);
    }

    return result;
}

int GLDTableSetting::index() const
{
    return m_owner->indexOf(const_cast<GLDTableSetting *>(this));
}

int GLDTableSetting::defRowHeight() const
{
    return m_defRowHeight;
}

void GLDTableSetting::setDefRowHeight(int value)
{
    m_defRowHeight = value;
}

GLDFieldSettings *GLDTableSetting::fieldSettings() const
{
    return m_fieldSettings;
}

CGLDExtPropDefs *GLDTableSetting::extPropDefs() const
{
    return m_extProps;
}

bool GLDTableSetting::hasHeaderRow()
{
    return !m_headerRows.isEmpty();
}

bool GLDTableSetting::hasTotalRow()
{
    return !m_totalRows.isEmpty();
}

GString GLDTableSetting::refDatabaseName() const
{
    return m_refDatabaseName;
}

void GLDTableSetting::setRefDatabaseName(const GString &value)
{
    m_refDatabaseName = value;
}

GString GLDTableSetting::refTableName() const
{
    return m_refTableName;
}

GLDGridRules *GLDTableSetting::rules() const
{
    return m_rules;
}

PtrInt GLDTableSetting::tag() const
{
    return m_tag;
}

void GLDTableSetting::setTag(const PtrInt value)
{
    m_tag = value;
}

int GLDTableSetting::headerRowCount() const
{
    return (int)m_headerRows.size();
}

int GLDTableSetting::totalRowCount() const
{
    return (int)m_totalRows.size();
}

GLDHeaderRowFieldSettings *GLDTableSetting::headerRows(int index)
{
    return m_headerRows[index];
}

bool GLDTableSetting::customDataSource() const
{
    return m_customDataSource;
}

void GLDTableSetting::setCustomDataSource(const bool &value)
{
    m_customDataSource = value;
}

GString GLDTableSetting::masterViewName() const
{
    return m_masterViewName;
}

void GLDTableSetting::setMasterViewName(const GString &value)
{
    m_masterViewName = value;
}

GString GLDTableSetting::selfFieldName() const
{
    return m_selfFieldName;
}

void GLDTableSetting::setSelfFieldName(const GString &value)
{
    m_selfFieldName = value;
}

GString GLDTableSetting::masterFieldName() const
{
    return m_masterFieldName;
}

void GLDTableSetting::setMasterFieldName(const GString &value)
{
    m_masterFieldName = value;
}

GString GLDTableSetting::primaryKeyFieldName() const
{
    return m_primaryKeyFieldName;
}

void GLDTableSetting::setPrimaryKeyFieldName(const GString &value)
{
    m_primaryKeyFieldName = value;
}

GString GLDTableSetting::parentKeyFieldName() const
{
    return m_parentKeyFieldName;
}

void GLDTableSetting::setParentKeyFieldName(const GString &value)
{
    m_parentKeyFieldName = value;
}

bool GLDTableSetting::isTree() const
{
    return m_isTree;
}

void GLDTableSetting::setIsTree(bool value)
{
    m_isTree = value;
}

bool GLDTableSetting::isTreeWithMasterView() const
{
    return m_isTreeWithMasterView;
}

void GLDTableSetting::setIsTreeWithMasterView(bool value)
{
    m_isTreeWithMasterView = value;
}

GLDTotalStyle GLDTableSetting::totalStyle() const
{
    return m_totalStyle;
}

void GLDTableSetting::setTotalStyle(GLDTotalStyle value)
{
    m_totalStyle = value;
}

bool GLDTableSetting::needExpandTotalExpr() const
{
    return m_needExpandTotalExpr;
}

void GLDTableSetting::setNeedExpandTotalExpr(const bool value)
{
    m_needExpandTotalExpr = value;
}

bool GLDTableSetting::cloneFlag() const
{
    return m_cloneFlag;
}

void GLDTableSetting::setCloneFlag(bool value)
{
    m_cloneFlag = value;
}
/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：编译，编译字段设置及规则
-----------------------------------------------------------------------------*/
void GLDTableSetting::compile()
{
    // assert(m_owner->owner()->DesignState);
    m_needExpandTotalExpr = false;

    for (int i = 0; i <= m_fieldSettings->count() - 1; i++)
    {
        //if not m_owner.m_owner.colSettings(i).crossExtFlag() then
        m_fieldSettings->items(i)->compile();
    }

    for (int i = 0; i <= headerRowCount() - 1; i++)
    {
        for (int j = 0; j <= headerRows(i)->count() - 1; j++)
        {
            headerRows(i)->items(j)->compile();
        }
    }

    for (int i = 0; i <= totalRowCount() - 1; i++)
    {
        for (int j = 0; j <= totalRows(i)->count() - 1; j++)
        {
            totalRows(i)->items(j)->compile();
        }
    }

    for (int i = 0; i <= m_rules->count() - 1; i++)
    {
        m_rules->items(i)->compile();
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDTableSetting::loadFromStream(GStream *stream)
{
    int nInts(0);
    int nCount(0);
    readStrFromStream(stream, m_refTableName);
    readStrFromStream(stream, m_refDatabaseName);
    readIntFromStream(stream, m_defRowHeight);
    readBoolFromStream(stream, m_customDataSource);
    readStrFromStream(stream, m_masterViewName);
    readStrFromStream(stream, m_selfFieldName);
    readStrFromStream(stream, m_masterFieldName);
    readStrFromStream(stream, m_primaryKeyFieldName);
    readStrFromStream(stream, m_parentKeyFieldName);
    readBoolFromStream(stream, m_isTree);
    readBoolFromStream(stream, m_isTreeWithMasterView);
    readIntFromStream(stream, nInts);
    m_totalStyle = ((GLDTotalStyle) nInts);
    m_rules->loadFromStream(stream);
    m_fieldSettings->loadFromStream(stream);
    readIntFromStream(stream, nCount);

    for (int i = 0; i <= nCount - 1; i++)
    {
        insertHeaderRow(-1)->loadFromStream(stream);
    }

    readIntFromStream(stream, nCount);

    for (int i = 0; i <= nCount - 1; i++)
    {
        insertTotalRow(-1)->loadFromStream(stream);
    }

    //todo yaok
    // GSPExtPropDefsStreamBuilder::read(stream, m_extProps);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDTableSetting::saveToStream(GStream *stream)
{
    writeStrToStream(stream, m_refTableName);
    writeStrToStream(stream, m_refDatabaseName);
    writeIntToStream(stream, m_defRowHeight);
    writeBoolToStream(stream, m_customDataSource);
    writeStrToStream(stream, m_masterViewName);
    writeStrToStream(stream, m_selfFieldName);
    writeStrToStream(stream, m_masterFieldName);
    writeStrToStream(stream, m_primaryKeyFieldName);
    writeStrToStream(stream, m_parentKeyFieldName);
    writeBoolToStream(stream, m_isTree);
    writeBoolToStream(stream, m_isTreeWithMasterView);
    writeIntToStream(stream, (int)m_totalStyle);
    m_rules->saveToStream(stream);
    m_fieldSettings->saveToStream(stream);
    writeIntToStream(stream, (int)m_headerRows.size());

    for (int i = 0; i != (int)m_headerRows.size(); i++)
    {
        headerRows(i)->saveToStream(stream);
    }

    writeIntToStream(stream, (int)m_totalRows.size());

    for (int i = 0; i != (int)m_totalRows.size(); i++)
    {
        totalRows(i)->saveToStream(stream);
    }

    //todo yaok
    //GSPExtPropDefsStreamBuilder::write(stream, m_extProps);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：设置参考表名
  参数：参考表名
-----------------------------------------------------------------------------*/
void GLDTableSetting::setRefTableName(const GString &value)
{
    if (!m_owner->owner()->designState())
    {
        gldError("SNeedDesignState");
    }

    m_refTableName = value;
}

void GLDTableSetting::clearCompileInfo()
{
    for (int i = 0; i <= m_fieldSettings->count() - 1; i++)
    {
        m_fieldSettings->items(i)->clearCompileInfo();
    }

    for (int i = 0; i <= headerRowCount() - 1; i++)
    {
        for (int j = 0; j <= headerRows(i)->count() - 1; j++)
        {
            headerRows(i)->items(j)->clearCompileInfo();
        }
    }

    for (int i = 0; i <= totalRowCount() - 1; i++)
    {
        for (int j = 0; j <= totalRows(i)->count() - 1; j++)
        {
            totalRows(i)->items(j)->clearCompileInfo();
        }
    }

    for (int i = 0; i <= m_rules->count() - 1; i++)
    {
        m_rules->items(i)->clearCompileInfo();
    }
}

GLDTableSetting *GLDTableSetting::clone()
{
    GLDTableSetting *result = createTableSetting(m_owner);
    result->initialize();
    GMemoryStream oStream;
    saveToStream(&oStream);
    oStream.seek(0);
    result->loadFromStream(&oStream);
    result->setCloneFlag(true);
    return result;
}

/* GLDFilterRows */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，创建对象列表
  参数：owner -- 所属容器
-----------------------------------------------------------------------------*/
GLDFilterRows::GLDFilterRows(GLDGridSetting *owner)
{
    m_owner = owner;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数，释放对象列表
-----------------------------------------------------------------------------*/
GLDFilterRows::~GLDFilterRows()
{
    clear();
}

GLDGridSetting *GLDFilterRows::owner() const
{
    return m_owner;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 清除标题行
-----------------------------------------------------------------------------*/
void GLDFilterRows::clear()
{
    m_list.clear();
}

int GLDFilterRows::count() const
{
    return (int)m_list.size();
}

int GLDFilterRows::indexOf(GLDFilterRow *value) const
{
    return m_list.indexOf(value);
}

GLDFilterRow *GLDFilterRows::items(int index)
{
    return m_list[index];
}

GLDFilterRow *GLDFilterRows::operator[](int index)
{
    return items(index);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：删除指定下标标题行
  参数：index -- 下标
-----------------------------------------------------------------------------*/
void GLDFilterRows::Delete(int index)
{
    assert((index >= 0) && (index <= count() - 1));
    m_list.removeAt(index);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：插入标题行
  参数：index -- 插入位置
  返回：插入的标题行对象
-----------------------------------------------------------------------------*/
GLDFilterRow *GLDFilterRows::insert(int index)
{
    GLDFilterRow *result = createFilterRow(this);

    if (count() == 0)
    {
        m_list.push_back(result);
    }
    else
    {
        if (-1 == index)
        {
            if (m_list.isEmpty())
            {
                m_list.push_back(result);
            }
            else
            {
                m_list.insert(m_list.count() - 1, result);
            }
        }
        else
        {
            m_list.insert(index, result);
        }
    }

    return result;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDFilterRows::loadFromStream(GStream *stream)
{
    int nInts(0);
    readIntFromStream(stream, nInts);
    clear();

    for (int i = 0; i <= nInts - 1; i++)
    {
        insert()->loadFromStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDFilterRows::saveToStream(GStream *stream)
{
    writeIntToStream(stream, count());

    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->saveToStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：设置标题行数
  参数：value -- 标题行数
-----------------------------------------------------------------------------*/
void GLDFilterRows::setCount(const int value)
{
    int nSize = (int)m_list.size();

    if (value == nSize)
    {
        return;
    }

    if (value > nSize)
    {
        for (int i = nSize; i <= value - 1; i++)
        {
            m_list.push_back(createFilterRow(this));
        }
    }
}

void GLDFilterRows::clearCompileInfo()
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->clearCompileInfo();
    }
}

GLDFilterRow *GLDFilterRows::createFilterRow(GLDFilterRows *owner)
{
    return new GLDFilterRow(owner);
}

void GLDFilterRows::compile()
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->compile();
    }
}

/* GLDFilterRow */

void GLDFilterRow::clearCompileInfo()
{
    for (int i = 0; i <= cellCount() - 1; i++)
    {
        cells(i)->clearCompileInfo();
    }
}

void GLDFilterRow::compile()
{
    for (int i = 0; i <= cellCount() - 1; i++)
    {
        cells(i)->compile();
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，创建单元格列表
-----------------------------------------------------------------------------*/
GLDFilterRow::GLDFilterRow(GLDFilterRows *owner)
{
    m_owner = owner;
    setCellCount(m_owner->owner()->colSettings()->count());
    m_rowHeight = CDefFilterRowHeight;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数，释放单元格列表
-----------------------------------------------------------------------------*/
GLDFilterRow::~GLDFilterRow()
{
    clear();
}

GLDFilterRows *GLDFilterRow::owner() const
{
    return m_owner;
}

int GLDFilterRow::indexOf(GLDFilterCell *value)
{
    return m_list.indexOf(value);
}

int GLDFilterRow::index() const
{
    return m_owner->indexOf(const_cast<GLDFilterRow *>(this));
}

int GLDFilterRow::rowHeight() const
{
    return m_rowHeight;
}

void GLDFilterRow::setRowHeight(const int &value)
{
    m_rowHeight = value;
}

int GLDFilterRow::cellCount() const
{
    return (int)m_list.size();
}

GLDFilterCell *GLDFilterRow::cells(int index)
{
    return m_list[index];
}

GLDFilterCell *GLDFilterRow::operator[](int index)
{
    return cells(index);
}

GObjectList<GLDFilterCell *> &GLDFilterRow::list()
{
    return m_list;
}

GLDFilterCell *GLDFilterRow::cellByCaption(const GString &caption)
{
    for (int i = 0; i <= cellCount() - 1; i++)
    {
        if (cells(i)->caption().toLower() == caption.toLower())
        {
            return cells(i);
        }
    }

    return NULL;
}

GLDFilterCell *GLDFilterRow::creatrFilterCell(GLDFilterRow *owner)
{
    return new GLDFilterCell(owner);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDFilterRow::loadFromStream(GStream *stream)
{
    readIntFromStream(stream, m_rowHeight);

    for (int i = 0; i <= cellCount() - 1; i++)
    {
        cells(i)->loadFromStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDFilterRow::saveToStream(GStream *stream)
{
    writeIntToStream(stream, m_rowHeight);

    for (int i = 0; i <= cellCount() - 1; i++)
    {
        cells(i)->saveToStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：设置单元格数
  参数：value -- 单元格数
-----------------------------------------------------------------------------*/
void GLDFilterRow::setCellCount(int value)
{
    int nSize = (int)m_list.size();

    if (value == nSize)
    {
        return;
    }

    if (value > nSize)
    {
        for (int i = nSize; i <= value - 1; i++)
        {
            m_list.push_back(creatrFilterCell(this));
        }
    }
}

void GLDFilterRow::Delete(int index)
{
    m_list.removeAt(index);
}

void GLDFilterRow::clear()
{
    m_list.clear();
}

/* GLDTitleRows */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，创建对象列表
  参数：owner -- 所属容器
-----------------------------------------------------------------------------*/
GLDTitleRows::GLDTitleRows(GLDGridSetting *owner)
{
    m_owner = owner;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数，释放对象列表
-----------------------------------------------------------------------------*/
GLDTitleRows::~GLDTitleRows()
{
    clear();
}

GLDGridSetting *GLDTitleRows::owner() const
{
    return m_owner;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 清除标题行
-----------------------------------------------------------------------------*/
void GLDTitleRows::clear()
{
    m_list.clear();
}

int GLDTitleRows::count() const
{
    return (int)m_list.size();
}

int GLDTitleRows::indexOf(GLDTitleRow * value) const
{
    return m_list.indexOf(value);
}

GLDTitleRow *GLDTitleRows::operator[](int index)
{
    return items(index);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：删除指定下标标题行
  参数：index -- 下标
-----------------------------------------------------------------------------*/
void GLDTitleRows::Delete(int index)
{
    assert((index >= 0) && (index < count()));
    m_list.removeAt(index);
}

GString GLDTitleRows::fullTitle(int index) const
{
    GString result;

    //assert(count() > 0);
    if (count() == 0)
    {
        result = "";
    }
    else
    {
        result = items(0)->cells(index)->caption();
        QString sFirstMerge = result;

        for (int i = 1; i <= count() - 1; i++)
        {
//            int nTest = items(i)->cells(index)->mergeID();
            if (items(i)->cells(index)->mergeID() != 0 && items(i)->cells(index)->mergeID() ==  items(i - 1)->cells(index)->mergeID())
            {
                result = result + "|" + sFirstMerge;
            }
            else
            {
                result = result + "|" + items(i)->cells(index)->caption();
                sFirstMerge = items(i)->cells(index)->caption();
            }
        }
    }

    return result;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回指定下标标题行对象
  参数：index -- 下标
  返回：index对应标题行对象
-----------------------------------------------------------------------------*/
GLDTitleRow *GLDTitleRows::items(int index) const
{
    assert((index >= 0) && (index < count()));
    return m_list[index];
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：插入标题行
  参数：index -- 插入位置
  返回：插入的标题行对象
-----------------------------------------------------------------------------*/
GLDTitleRow *GLDTitleRows::insert(int index)
{
    GLDTitleRow *result = createTitleRow(this);

    if ((count() == 0) || (index == -1))
    {
        m_list.push_back(result);
    }
    else
    {
        m_list.insert(index, result);
    }

    return result;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDTitleRows::loadFromStream(GStream *stream)
{
    int nInts(0);
    readIntFromStream(stream, nInts);
    clear();

    for (int i = 0; i <= nInts - 1; i++)
    {
        insert()->loadFromStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDTitleRows::saveToStream(GStream *stream)
{
    writeIntToStream(stream, count());

    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->saveToStream(stream);
    }
}

void GLDTitleRows::setCount(const int value)
{
    int nSize = (int)m_list.size();

    if (value == nSize)
    {
        return;
    }

    //    m_list.setCount(value);
    if (value > nSize)
    {
        for (int i = nSize; i <= value - 1; i++)
        {
            m_list.push_back(createTitleRow(this));
        }
    }
}

void GLDTitleRows::clearCompileInfo()
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->clearCompileInfo();
    }
}

GLDTitleRow *GLDTitleRows::doCreateTitleRow(GLDTitleRows *owner)
{
    return new GLDTitleRow(owner);
}

GLDTitleRow *GLDTitleRows::createTitleRow(GLDTitleRows *owner)
{
    GLDTitleRow *result = doCreateTitleRow(owner);
    result->setCellCount(owner->owner()->colSettings()->count());
    return result;
}

void GLDTitleRows::compile()
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->compile();
    }
}

/* GLDTitleRow */

void GLDTitleRow::clearCompileInfo()
{
    for (int i = 0; i <= cellCount() - 1; i++)
    {
        cells(i)->clearCompileInfo();
    }
}

void GLDTitleRow::compile()
{
    for (int i = 0; i <= cellCount() - 1; i++)
    {
        cells(i)->compile();
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，创建单元格列表
-----------------------------------------------------------------------------*/
GLDTitleRow::GLDTitleRow(GLDTitleRows *owner)
{
    m_owner = owner;
    m_rowHeight = CDefTitleRowHeight;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数，释放单元格列表
-----------------------------------------------------------------------------*/
GLDTitleRow::~GLDTitleRow()
{
    clear();
}

void GLDTitleRow::clear()
{
    m_list.clear();
}

GLDTitleCell *GLDTitleRow::cellByCaption(const GString &caption)
{
    GLDTitleCell *result = NULL;

    for (int i = 0; i <= cellCount() - 1; i++)
    {
        if (cells(i)->caption().toLower() == caption.toLower())
        {
            result = cells(i);
            break;
        }
    }

    return result;
}

int GLDTitleRow::index()
{
    return m_owner->indexOf(this);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDTitleRow::loadFromStream(GStream *stream)
{
    readIntFromStream(stream, m_rowHeight);

    for (int i = 0; i <= cellCount() - 1; i++)
    {
        cells(i)->loadFromStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流 对 象
-----------------------------------------------------------------------------*/
void GLDTitleRow::saveToStream(GStream *stream)
{
    writeIntToStream(stream, m_rowHeight);

    for (int i = 0; i <= cellCount() - 1; i++)
    {
        cells(i)->saveToStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：设置单元格数
  参数：value -- 单元格数
-----------------------------------------------------------------------------*/
void GLDTitleRow::setCellCount(int value)
{
    int nSize = (int)m_list.size();

    if (value == nSize)
    {
        return;
    }

    if (value > nSize)
    {
        for (int i = nSize; i <= value - 1; i++)
        {
            m_list.push_back(createTitleCell(this));
        }
    }
}

void GLDTitleRow::Delete(int index)
{
    m_list.removeAt(index);
}

GLDGridSetting *GLDTitleRow::gridSetting()
{
    return m_owner->owner();
}

GLDTitleCell *GLDTitleRow::createTitleCell(GLDTitleRow *owner)
{
    return new GLDTitleCell(owner);
}

int GLDTitleRow::rowHeight() const
{
    return m_rowHeight;
}

void GLDTitleRow::setRowHeight(int value)
{
    m_rowHeight = value;
}

int GLDTitleRow::cellCount() const
{
    return (int)m_list.size();
}

GLDTitleCell *GLDTitleRow::cells(const int index) const
{
    return m_list[index];
}

GObjectList<GLDTitleCell*> &GLDTitleRow::list()
{
    return m_list;
}

/*GSPHeaderRowFieldSettings */

GLDExpandRowFieldSetting *GLDHeaderRowFieldSettings::createExpandRowFieldSetting()
{
    return new GLDHeaderRowFieldSetting(this);
}

GLDHeaderRowFieldSetting *GLDHeaderRowFieldSettings::items(int index)
{
    // assert((index >= 0) &&(index < count()));
    return dynamic_cast<GLDHeaderRowFieldSetting *>(m_list[index]);
}

/* GLDHeaderRowFieldSetting */

void GLDHeaderRowFieldSetting::compile()
{
    try
    {
        GLDExpandRowFieldSetting::compile();
    }
    catch (...)
    {
        //todo yaok
        //throw new eGSPHeaderRowExprException(format(GSPGridSetting_SInvalidHeaderRowExpr,
        //refFullName(), m_value), tableIndex(), Index);
    }
}

/* GLDExpandRowFieldSettings */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，创建对象列表
-----------------------------------------------------------------------------*/
GLDExpandRowFieldSettings::GLDExpandRowFieldSettings(GLDTableSetting *owner)
{
    m_owner = owner;
    m_visible = true;
    m_rowHeight = CDefRowHeight;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数，释放对象列表
-----------------------------------------------------------------------------*/
GLDExpandRowFieldSettings::~GLDExpandRowFieldSettings()
{
    clear();
}

GLDExpandRowFieldSetting *GLDExpandRowFieldSettings::createExpandRowFieldSetting()
{
    return NULL;
    //  assert(false);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDExpandRowFieldSettings::loadFromStream(GStream *stream)
{
    readIntFromStream(stream, m_rowHeight);

    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->loadFromStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDExpandRowFieldSettings::saveToStream(GStream *stream)
{
    writeIntToStream(stream, m_rowHeight);

    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->saveToStream(stream);
    }
}

int GLDExpandRowFieldSettings::count() const
{
    return (int)m_list.size();
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：设置字段设置对象数
  参数：value -- 字段设置对象数
-----------------------------------------------------------------------------*/
void GLDExpandRowFieldSettings::setCount(int value)
{
    int nSize = (int)m_list.size();

    if (value == nSize)
    {
        return;
    }

    if (value > nSize)
    {
        for (int i = nSize; i <= value - 1; i++)
        {
            m_list.push_back(createExpandRowFieldSetting());
        }
    }
}

bool GLDExpandRowFieldSettings::visible() const
{
    return m_visible;
}

void GLDExpandRowFieldSettings::setVisible(bool value)
{
    m_visible = value;
}

int GLDExpandRowFieldSettings::rowHeight() const
{
    return m_rowHeight;
}

void GLDExpandRowFieldSettings::setRowHeight(int value)
{
    m_rowHeight = value;
}

GLDTableSetting *GLDExpandRowFieldSettings::owner() const
{
    return m_owner;
}

GLDExpandRowFieldSetting *GLDExpandRowFieldSettings::items(int index)
{
    return m_list[index];
}

GObjectList<GLDExpandRowFieldSetting*> &GLDExpandRowFieldSettings::list()
{
    return m_list;
}

GLDExpandRowFieldSetting *GLDExpandRowFieldSettings::operator[](int index)
{
    return items(index);
}

GStringList GLDExpandRowFieldSettings::toValueArray()
{
    GStringList result;

    for (int i = 0; i <= count() - 1; i++)
    {
        result.append(items(i)->value());
    }

    return result;
}

void GLDExpandRowFieldSettings::Delete(int index)
{
    m_list.removeAt(index);
}

void GLDExpandRowFieldSettings::clear()
{
    m_list.clear();
}

/* GLDExpandRowFieldSetting */

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-15
  功能: 构造函数
-----------------------------------------------------------------------------*/
GLDExpandRowFieldSetting::GLDExpandRowFieldSetting(GLDExpandRowFieldSettings *owner)
{
    m_isSimpleStr = false;
    m_mergeID = 0;
    m_owner = owner;
    m_cellFormat = new GLDCellFormat;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-15
  功能: 析构函数
-----------------------------------------------------------------------------*/
GLDExpandRowFieldSetting::~GLDExpandRowFieldSetting()
{
    freeAndNil(m_cellFormat);
}

void GLDExpandRowFieldSetting::clearCompileInfo()
{
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-16
  功能: 编译合计行
-----------------------------------------------------------------------------*/
void GLDExpandRowFieldSetting::compile()
{
    GString sExpr;

    if (textIsLabel(m_value))
    {
        m_resultDataType = c_gdtString;
        m_isSimpleStr = true;
    }
    else
    {
        sExpr = copy(m_value, 1);
        m_isSimpleStr = false;
    }
}

GString GLDExpandRowFieldSetting::varCodes(int index)
{
    Q_UNUSED(index);
    return "";
}

int GLDExpandRowFieldSetting::varCount()
{
    return -1;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-15
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDExpandRowFieldSetting::loadFromStream(GStream *stream)
{
    readIntFromStream(stream, m_mergeID);
    readStrFromStream(stream, m_value);
    m_cellFormat->loadFromStream(stream);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-15
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDExpandRowFieldSetting::saveToStream(GStream *stream)
{
    writeIntToStream(stream, m_mergeID);
    writeStrToStream(stream, m_value);
    m_cellFormat->saveToStream(stream);
}

bool GLDExpandRowFieldSetting::isSimpleStr() const
{
    return m_isSimpleStr;
}

int GLDExpandRowFieldSetting::mergeID() const
{
    return m_mergeID;
}

void GLDExpandRowFieldSetting::setMergeID(int value)
{
    m_mergeID = value;
}

GLDDataType GLDExpandRowFieldSetting::resultDataType() const
{
    return m_resultDataType;
}

GString GLDExpandRowFieldSetting::value() const
{
    return m_value;
}

void GLDExpandRowFieldSetting::setValue(GString value)
{
    m_value = value;
}

int GLDExpandRowFieldSetting::index()
{
    return m_owner->list().indexOf(this);
}

GLDCellFormat *GLDExpandRowFieldSetting::cellFormat() const
{
    return m_cellFormat;
}

void GLDExpandRowFieldSetting::setCellFormat(GLDCellFormat *value)
{
    m_cellFormat = value;
}

bool GLDExpandRowFieldSetting::isFixedCol()
{
    return m_owner->owner()->owner()->owner()->colSettings()->items(index())->isFixedCol();
}

GString GLDExpandRowFieldSetting::refFullName()
{
    return m_owner->owner()->refFullName();
}

void GLDExpandRowFieldSetting::setVarCodes(int index, const GString &value)
{
    // todo
    //parser_->setVarCodes(index, value);
    Q_UNUSED(index);
    Q_UNUSED(value);
}

int GLDExpandRowFieldSetting::tableIndex()
{
    return m_owner->owner()->index();
}

/* GLDTotalRowFieldSettings */

GLDExpandRowFieldSetting *GLDTotalRowFieldSettings::createExpandRowFieldSetting()
{
    return new GLDTotalRowFieldSetting(this);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回指定下标的字段设置对象
  参数：index -- 下标
  返回：index对应的字段设置对象
-----------------------------------------------------------------------------*/
GLDTotalRowFieldSetting *GLDTotalRowFieldSettings::items(int index)
{
    return dynamic_cast<GLDTotalRowFieldSetting *>(m_list[index]);
}

/*GSPTotalRowFieldSetting */

void GLDTotalRowFieldSetting::compile()
{
    try
    {
        GLDExpandRowFieldSetting::compile();
    }
    catch (...)
    {
        //todo yaok
        //throw new eGSPTotalRowExprException(format(GSPGridSetting_SInvalidTotalRowExpr,
        //refFullName(), m_value), tableIndex(), Index);
    }
}

/* GLDCellFormat */

GLDCellFormat::GLDCellFormat()
{
    m_fontName = CDefFontName;
    m_fontSize = DefFontSize;
    m_foreColor = DefForeColor;
    m_fontStyle = 0;
    m_backColor = DefBackColor;
    m_leftLineWidth = DefLeftLineWidth;
    m_topLineWidth = DefTopLineWidth;
    m_rightLineWidth = DefRightLineWidth;
    m_bottomLineWidth = DefBottomLineWidth;
    m_diagonalWidth = DefDiagonalWidth;
    m_antiDiagonalWidth = DefAntiDiagonalWidth;
    m_horzAlignment = (int)DefHorzAlignment;
    m_vertAlignment = (int)DefVertAlignment;
    m_formatStr = CDefFormatStr;
    m_nullIsZero = DefNullIsZero;
    m_leftMargin = DefHorzMargin;
    m_rightMargin = DefHorzMargin;
    m_topMargin = DefVertMargin;
    m_bottomMargin = DefVertMargin;
    m_imageIndex = DefImageIndex;
    m_imageLayout = CDefImageLayout;
}

bool GLDCellFormat::isEqual(GLDCellFormat *cellFormat)
{

    //  assert(cellFormat != NULL);
    return (cellFormat->m_fontName.toLower() == m_fontName.toLower())
           && (cellFormat->m_fontSize == m_fontSize)
           && (cellFormat->m_foreColor == m_foreColor)
           && (cellFormat->m_backColor == m_backColor)
           && (cellFormat->m_fontStyle == m_fontStyle)
           && (cellFormat->m_leftLineWidth == m_leftLineWidth)
           && (cellFormat->m_rightLineWidth == m_rightLineWidth)
           && (cellFormat->m_topLineWidth == m_topLineWidth)
           && (cellFormat->m_bottomLineWidth == m_bottomLineWidth)
           && (cellFormat->m_diagonalWidth == m_diagonalWidth)
           && (cellFormat->m_antiDiagonalWidth == m_antiDiagonalWidth)
           && (cellFormat->m_horzAlignment == m_horzAlignment)
           && (cellFormat->m_leftMargin == m_leftMargin)
           && (cellFormat->m_rightMargin == m_rightMargin)
           && (cellFormat->m_topMargin == m_topMargin)
           && (cellFormat->m_bottomMargin == m_bottomMargin)
           && (cellFormat->m_imageIndex == m_imageIndex)
           && (cellFormat->m_imageLayout == m_imageLayout);
}

GRgb GLDCellFormat::foreColor() const
{
    return m_foreColor;
}

void GLDCellFormat::setForeColor(const GRgb value)
{
    m_foreColor = value;
}

GString GLDCellFormat::fontName() const
{
    return m_fontName;
}

void GLDCellFormat::setFontName(GString value)
{
    m_fontName = value;
}

int GLDCellFormat::fontSize() const
{
    return m_fontSize;
}

void GLDCellFormat::setFontSize(int value)
{
    m_fontSize = value;
}
FontStyles GLDCellFormat::fontStyle() const
{
    return m_fontStyle;
}

void GLDCellFormat::setFontStyle(FontStyles value)
{
    m_fontStyle = value;
}

GRgb GLDCellFormat::backColor() const
{
    return m_backColor;
}

void GLDCellFormat::setBackColor(const GRgb value)
{
    m_backColor = value;
}

GLDHorzAlignment GLDCellFormat::horzAlignment() const
{
    return (GLDHorzAlignment) m_horzAlignment;
}

GLDVertAlignment GLDCellFormat::vertAlignment() const
{
    return (GLDVertAlignment) m_vertAlignment;
}

void GLDCellFormat::setVertAlignment(const GLDVertAlignment value)
{
    m_vertAlignment = (int)value;
}

unsigned char GLDCellFormat::leftLineWidth() const
{
    return m_leftLineWidth;
}

void GLDCellFormat::setLeftLineWidth(const unsigned char value)
{
    m_leftLineWidth = value;
}

unsigned char GLDCellFormat::topLineWidth() const
{
    return m_topLineWidth;
}

void GLDCellFormat::setTopLineWidth(unsigned char value)
{
    m_topLineWidth = value;
}

unsigned char GLDCellFormat::rightLineWidth() const
{
    return m_rightLineWidth;
}

void GLDCellFormat::setRightLineWidth(unsigned char value)
{
    m_rightLineWidth = value;
}

unsigned char GLDCellFormat::bottomLineWidth() const
{
    return m_bottomLineWidth;
}

void GLDCellFormat::setBottomLineWidth(unsigned char value)
{
    m_bottomLineWidth = value;
}

unsigned char GLDCellFormat::diagonalWidth() const
{
    return m_diagonalWidth;
}

void GLDCellFormat::setDiagonalWidth(unsigned char value)
{
    m_diagonalWidth = value;
}

unsigned char GLDCellFormat::antiDiagonalWidth() const
{
    return m_antiDiagonalWidth;
}

void GLDCellFormat::setAntiDiagonalWidth(unsigned char value)
{
    m_antiDiagonalWidth = value;
}

GString GLDCellFormat::formatStr() const
{
    return m_formatStr;
}

void GLDCellFormat::setFormatStr(const GString &value)
{
    m_formatStr = trim(value);
}

bool GLDCellFormat::nullIsZero() const
{
    return m_nullIsZero;
}

void GLDCellFormat::setNullIsZero(const bool value)
{
    m_nullIsZero = value;
}

unsigned char GLDCellFormat::leftMargin() const
{
    return m_leftMargin;
}

void GLDCellFormat::setLeftMargin(unsigned char value)
{
    m_leftMargin = value;
}

unsigned char GLDCellFormat::rightMargin() const
{
    return m_rightMargin;
}

void GLDCellFormat::setRightMargin(unsigned char value)
{
    m_rightMargin = value;
}

unsigned char GLDCellFormat::topMargin() const
{
    return m_topMargin;
}

void GLDCellFormat::setTopMargin(unsigned char value)
{
    m_topMargin = value;
}

unsigned char GLDCellFormat::bottomMargin() const
{
    return m_bottomMargin;
}

void GLDCellFormat::setBottomMargin(unsigned char value)
{
    m_bottomMargin = value;
}

int GLDCellFormat::imageIndex() const
{
    return m_imageIndex;
}

void GLDCellFormat::setImageIndex(int value)
{
    m_imageIndex = value;
}

GLDCellImageLayout GLDCellFormat::imageLayout()
{
    return m_imageLayout;
}

void GLDCellFormat::setImageLayout(GLDCellImageLayout value)
{
    m_imageLayout = value;
}

void GLDCellFormat::setHorzAlignment(const GLDHorzAlignment value)
{
    m_horzAlignment = (int)value;
}
void GLDCellFormat::loadFromStream(GStream *stream)
{
    unsigned char ucFontStyle;
    int nImageLayout;
    readStrFromStream(stream, m_fontName);
    readIntFromStream(stream, m_fontSize);
    readIntFromStream(stream, m_foreColor);
    readIntFromStream(stream, m_backColor);
    readByteFromStream(stream, m_leftLineWidth);
    readByteFromStream(stream, m_topLineWidth);
    readByteFromStream(stream, m_rightLineWidth);
    readByteFromStream(stream, m_bottomLineWidth);
    readByteFromStream(stream, m_diagonalWidth);
    readByteFromStream(stream, m_antiDiagonalWidth);
    readIntFromStream(stream, m_horzAlignment);
    readIntFromStream(stream, m_vertAlignment);
    readStrFromStream(stream, m_formatStr);
    readBoolFromStream(stream, m_nullIsZero);
    readByteFromStream(stream, ucFontStyle);
    m_fontStyle = (FontStyles)ucFontStyle;
    readByteFromStream(stream, m_leftMargin);
    readByteFromStream(stream, m_rightMargin);
    readByteFromStream(stream, m_topMargin);
    readByteFromStream(stream, m_bottomMargin);
    readIntFromStream(stream, m_imageIndex);
    readIntFromStream(stream, nImageLayout);
    m_imageLayout = (GLDCellImageLayout)nImageLayout;
}

void GLDCellFormat::saveToStream(GStream *stream)
{
    writeStrToStream(stream, m_fontName);
    writeIntToStream(stream, m_fontSize);
    writeIntToStream(stream, m_foreColor);
    writeIntToStream(stream, m_backColor);
    writeByteToStream(stream, m_leftLineWidth);
    writeByteToStream(stream, m_topLineWidth);
    writeByteToStream(stream, m_rightLineWidth);
    writeByteToStream(stream, m_bottomLineWidth);
    writeByteToStream(stream, m_diagonalWidth);
    writeByteToStream(stream, m_antiDiagonalWidth);
    writeIntToStream(stream, m_horzAlignment);
    writeIntToStream(stream, m_vertAlignment);
    writeStrToStream(stream, m_formatStr);
    writeBoolToStream(stream, m_nullIsZero);
    writeByteToStream(stream, (unsigned char)m_fontStyle);
    writeByteToStream(stream, m_leftMargin);
    writeByteToStream(stream, m_rightMargin);
    writeByteToStream(stream, m_topMargin);
    writeByteToStream(stream, m_bottomMargin);
    writeIntToStream(stream, m_imageIndex);
    writeIntToStream(stream, (int)m_imageLayout);
}

/* GLDCellFormats */

GLDCellFormats::GLDCellFormats()
{
    m_list = new GObjectList<GLDCellFormat *>;
}

GLDCellFormats::~GLDCellFormats()
{
    freeAndNil(m_list);
}

int GLDCellFormats::count() const
{
    return (int)m_list->size();
}

GLDCellFormat *GLDCellFormats::formats(const int index) const
{
    return m_list->at(index);
}

/* GLDTableSettings */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，创建对象列表
-----------------------------------------------------------------------------*/
GLDTableSettings::GLDTableSettings(GLDGridSetting *owner)
{
    m_owner = owner;
    m_list = new GObjectList<GLDTableSetting *>;
    m_hasHeaderOrTotalRow = false;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数，释放对象列表
-----------------------------------------------------------------------------*/
GLDTableSettings::~GLDTableSettings()
{
    freeAndNil(m_list);
}

/*-----------------------------------------------------------------------------
   创建: Liuxd 2005-12-12
   功能: 清除表设置数据
 -----------------------------------------------------------------------------*/
void GLDTableSettings::clear()
{
    m_list->clear();
}

int GLDTableSettings::count() const
{
    return (int)m_list->size();
}

int GLDTableSettings::indexOf(GLDTableSetting *tableSetting) const
{
    return m_list->indexOf(tableSetting);
}

GLDTableSetting *GLDTableSettings::operator[](int index)
{
    return items(index) ;
}

/*-----------------------------------------------------------------------------
    创建：Tu Jianhua 2005.11.10
    功能：编译
  -----------------------------------------------------------------------------*/
void GLDTableSettings::compile()
{
    m_hasHeaderOrTotalRow = false;

    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->compile();
        m_hasHeaderOrTotalRow = m_hasHeaderOrTotalRow || items(i)->hasHeaderRow()
                                || items(i)->hasTotalRow();
    }
}

/*-----------------------------------------------------------------------------
    创建：Tu Jianhua 2005.11.10
    功能：删除指定下标的表设置对象
    参数：index -- 下标
-----------------------------------------------------------------------------*/
void GLDTableSettings::Delete(int index)
{
    assert((index >= 0) && (index < count()));
    m_list->removeAt(index);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回指定下标的表设置对象
  参数：index -- 下标
  返回：index对应的表设置对象
-----------------------------------------------------------------------------*/
GLDTableSetting *GLDTableSettings::items(int index)
{
    assert((index >= 0) && (index < count()));
    return (*m_list)[index];
}

GLDTableSetting *GLDTableSettings::doCreateTableSetting(GLDTableSettings *owner)
{
    return new GLDTableSetting(owner);
}

GLDTableSetting *GLDTableSettings::createTableSetting(GLDTableSettings *owner)
{
    GLDTableSetting *result = doCreateTableSetting(owner);
    result->initialize();
    return result;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：插入表设置对象
  参数：index -- 插入下标
  返回：插入的表设置对象
-----------------------------------------------------------------------------*/
GLDTableSetting *GLDTableSettings::insert(int index)
{
    GLDTableSetting *result = createTableSetting(this);

    if (index == -1)
    {
        m_list->push_back(result);
    }
    else
    {
        m_list->insert(index, result);
    }

    return result;
}

GLDGridSetting *GLDTableSettings::owner() const
{
    return m_owner;
}

bool GLDTableSettings::hasHeaderOrTotalRow() const
{
    return m_hasHeaderOrTotalRow;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDTableSettings::loadFromStream(GStream *stream)
{
    int nints(0);
    readIntFromStream(stream, nints);
    clear();

    for (int i = 0; i <= nints - 1; i++)
    {
        insert()->loadFromStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：移动表设置对象
  参数：fromIndex -- 源下标
        toIndex -- 目标下标
-----------------------------------------------------------------------------*/
void GLDTableSettings::move(int fromIndex, int toIndex)
{
    assert((fromIndex >= 0) && (fromIndex < count()));
    assert((toIndex >= 0) && (toIndex < count()));

    if (fromIndex == toIndex)
    {
        return;
    }
    else
    {
        m_list->move(fromIndex, toIndex);
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDTableSettings::saveToStream(GStream *stream)
{
    writeIntToStream(stream, count());

    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->saveToStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：设置表设置对象数
  参数：value -- 表设置对象数
-----------------------------------------------------------------------------*/
void GLDTableSettings::setCount(const int value)
{
    int nSize = m_list->count();

    if (value == nSize)
    {
        return;
    }

    if (value > nSize)
    {
        for (int i = nSize; i <= value - 1; i++)
        {
            m_list->push_back(createTableSetting(this));
        }
    }
}

void GLDTableSettings::clearClonedTableSetting()
{
    for (int i = count() - 1; i >= 0; i--)
    {
        if (items(i)->cloneFlag())
        {
            Delete(i);
        }
    }
}

void GLDTableSettings::clearCompileInfo()
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->clearCompileInfo();
    }
}

void GLDTableSettings::clone(GLDTableSetting *protoType, int insertIndex, int count)
{
    assert((insertIndex >= 0) && (insertIndex < count));
    assert(protoType != NULL);
    assert(count > 0);

    if (insertIndex == -1)
    {
        for (int i = 0; i <= count - 1; i++)
        {
            m_list->push_back(protoType->clone());
        }
    }
    else
    {
        for (int i = 0; i <= count - 1; i++)
        {
            m_list->insert(insertIndex, protoType->clone());
        }
    }
}

/* GLDFieldSettings */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，创建对象列表
-----------------------------------------------------------------------------*/
GLDFieldSettings::GLDFieldSettings(GLDTableSetting *owner)
{
    m_owner = owner;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数，释放对象列表
-----------------------------------------------------------------------------*/
GLDFieldSettings::~GLDFieldSettings()
{
    clear();
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDFieldSettings::loadFromStream(GStream *stream)
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->loadFromStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDFieldSettings::saveToStream(GStream *stream)
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->saveToStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：设置字段设置对象数
  参数：value -- 字段设置对象数
-----------------------------------------------------------------------------*/
void GLDFieldSettings::setCount(int value)
{
    int nSize = (int)m_list.size();

    if (value == nSize)
    {
        return;
    }

    if (value > nSize)
    {
        for (int i = nSize; i <= value - 1; i++)
        {
            m_list.push_back(createFieldSetting(this));
        }
    }
}

void GLDFieldSettings::Delete(int index)
{
    m_list.removeAt(index);
}

void GLDFieldSettings::clear()
{
    m_list.clear();
}

GLDFieldSetting *GLDFieldSettings::createFieldSetting(GLDFieldSettings *owner)
{
    return new GLDFieldSetting(owner);
}

GLDFieldSetting *GLDFieldSettings::items(int index)
{
    return m_list[index];
}

int GLDFieldSettings::count() const
{
    return (int)m_list.size();
}

GLDTableSetting *GLDFieldSettings::owner() const
{
    return m_owner;
}

GObjectList<GLDFieldSetting *> &GLDFieldSettings::list()
{
    return m_list;
}

GLDFieldSetting *GLDFieldSettings::operator[](int index)
{
    return items(index);
}

/* GLDCustomFieldSetting */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数
  参数：owner -- 所属容器
-----------------------------------------------------------------------------*/
GLDCustomFieldSetting::GLDCustomFieldSetting()
    : m_mergeID(0),
      m_extProps(new CGLDExtPropDefs),
      m_cellFormat(new GLDCellFormat),
      m_displayExprIsFieldName(false),
      m_editTextIsDisplayText(true)
{
    m_extProps->AddRef();
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数，释放解析器对象
-----------------------------------------------------------------------------*/
GLDCustomFieldSetting::~GLDCustomFieldSetting()
{
    freeAndNil(m_cellFormat);
    m_extProps->Release();
    m_extProps = NULL;
}

GString GLDCustomFieldSetting::buildDisplayParserDisplayExpr()
{
    if ((m_displayExpr == "") && (m_editFieldName != ""))
    {
        return "=" + m_editFieldName;
    }
    else
    {
        return m_displayExpr;
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：建立显示表达式解析器
-----------------------------------------------------------------------------*/
void GLDCustomFieldSetting::buildDisplayParser()
{
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：建立写入规则解析器
-----------------------------------------------------------------------------*/
void GLDCustomFieldSetting::buildSaveRuleParser()
{
}

void GLDCustomFieldSetting::clearCompileInfo()
{
    m_extProps->clearCompileInfo();
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：编译，设置编辑字段、写入字段下标，建立显示表达式解析器和写入规则解析器
-----------------------------------------------------------------------------*/
void GLDCustomFieldSetting::compile()
{
    GString sEditFieldName;
    GString sSaveFieldName;

    // 编译编辑字段名称
    if (m_editFieldName != "")
    {
        if (m_editFieldName[0] == CExpressionMark)
        {
            sEditFieldName = copy(m_editFieldName, 1);

            if (!isExprEnumDateTimeField(sEditFieldName))
            {
                gldFieldNameError(format(getGLDi18nStr(g_InvalidFieldNameMark), refFullName(), m_editFieldName), tableIndex(), index());
            }
        }
        else if (m_editFieldName[0] == CLookupMark)
        {
            sEditFieldName = copy(m_editFieldName, 1);
        }
        else
        {
            sEditFieldName = m_editFieldName;
        }

        if (!isValidFieldName(sEditFieldName))
        {
            gldFieldNameError(format(getGLDi18nStr(g_InvalidFieldName), GVariantList() << refFullName() << m_editFieldName),
                              tableIndex(), index());
        }
    }

    // 编译保存字段名
    if (m_saveFieldName == "")
    {
        sSaveFieldName = m_editFieldName;
    }
    else
    {
        sSaveFieldName = m_saveFieldName;
    }

    if (sSaveFieldName != "")
    {
        if (sSaveFieldName[0] == CExpressionMark)
        {
            {
                sSaveFieldName = copy(sSaveFieldName, 1);
            }

            if (!isExprEnumDateTimeField(sSaveFieldName))
            {
                gldSaveFieldNameError(format(getGLDi18nStr(g_InvalidFieldNameMark), refFullName(), m_saveFieldName), tableIndex(),
                                      index());
            }
        }
        else if (sSaveFieldName[0] == CLookupMark)
        {
            sSaveFieldName = copy(sSaveFieldName, 1);
        }

        if (!isValidFieldName(sSaveFieldName))
        {
            gldSaveFieldNameError(format(getGLDi18nStr(SInvalidSaveFieldName), refFullName(), m_saveFieldName), tableIndex(), index());
        }
    }

    // 构造表达式解析器
    buildDisplayParser();
    buildSaveRuleParser();
    m_extProps->compile();
}

/*-----------------------------------------------------------------------------
  作者：Liuxd 2006-03-30
  功能：显示表达式是否等于编辑字段
  返回：相等标志
-----------------------------------------------------------------------------*/
bool GLDCustomFieldSetting::displayExprIsFieldName() const
{
    return (m_displayExpr == "") || m_displayExprIsFieldName;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDCustomFieldSetting::loadFromStream(GStream *stream)
{
    readIntFromStream(stream, m_mergeID);
    readStrFromStream(stream, m_editFieldName);
    readMemoFromStream(stream, m_displayExpr);
    readStrFromStream(stream, m_saveFieldName);
    readMemoFromStream(stream, m_saveRule);
    readMemoFromStream(stream, m_nullValDisplayStr);
    readMemoFromStream(stream, m_extData);
    readBoolFromStream(stream, m_editTextIsDisplayText);
    readStrFromStream(stream, m_editorName);
    readMemoFromStream(stream, m_cellIdentity);
    m_cellFormat->loadFromStream(stream);
    //todo yaok
    // GSPExtPropDefsStreamBuilder::read(stream, m_extProps);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDCustomFieldSetting::saveToStream(GStream *stream)
{
    writeIntToStream(stream, m_mergeID);
    writeStrToStream(stream, m_editFieldName);
    writeMemoToStream(stream, m_displayExpr);
    writeStrToStream(stream, m_saveFieldName);
    writeMemoToStream(stream, m_saveRule);
    writeMemoToStream(stream, m_nullValDisplayStr);
    writeMemoToStream(stream, m_extData);
    writeBoolToStream(stream, m_editTextIsDisplayText);
    writeStrToStream(stream, m_editorName);
    writeMemoToStream(stream, m_cellIdentity);
    m_cellFormat->saveToStream(stream);
    //todo yaok
    // GSPExtPropDefsStreamBuilder::write(stream, m_extProps);
}

void GLDCustomFieldSetting::setIsLabel(bool value)
{
    Q_UNUSED(value)
}

GString GLDCustomFieldSetting::cellIdentity() const
{
    return m_cellIdentity;
}

bool GLDCustomFieldSetting::isLabel() const
{
    return false;
}

GString GLDCustomFieldSetting::varCodes(int index) const
{
    Q_UNUSED(index);
    return "";
}

int GLDCustomFieldSetting::varCount() const
{
    return -1;
}

GString GLDCustomFieldSetting::displayFieldName() const
{
    // assert(displayExprIsFieldName());
    if ("" == m_displayExpr)
    {
        return m_editFieldName;
    }
    else
    {
        return trim(copy(m_displayExpr, 1));
    }
}

int GLDCustomFieldSetting::mergeID() const
{
    return m_mergeID;
}

void GLDCustomFieldSetting::setMergeID(int value)
{
    m_mergeID = value;
}

GString GLDCustomFieldSetting::editFieldName() const
{
    return m_editFieldName;
}

void GLDCustomFieldSetting::setEditFieldName(const GString &value)
{
    m_editFieldName = value.trimmed();
}

GString GLDCustomFieldSetting::displayExpr() const
{
    return m_displayExpr;
}

void GLDCustomFieldSetting::setDisplayExpr(const GString &value)
{
    m_displayExpr = value.trimmed();
}

GString GLDCustomFieldSetting::saveFieldName() const
{
    return m_saveFieldName;
}

void GLDCustomFieldSetting::setSaveFieldName(const GString &value)
{
    m_saveFieldName = value.trimmed();
}

GString GLDCustomFieldSetting::saveRule() const
{
    return m_saveRule;
}

void GLDCustomFieldSetting::setSaveRule(const GString &value)
{
    m_saveRule = value.trimmed();
}

GString GLDCustomFieldSetting::nullValDisplayStr() const
{
    return m_nullValDisplayStr;
}

void GLDCustomFieldSetting::setNullValDisplayStr(GString value)
{
    m_nullValDisplayStr = value;
}

void GLDCustomFieldSetting::setCellIdentity(const GString &value)
{
    m_cellIdentity = value.trimmed();
}

GString GLDCustomFieldSetting::editorName() const
{
    return m_editorName;
}

void GLDCustomFieldSetting::setEditorName(const GString &value)
{
    m_editorName = value.trimmed();
}

GString GLDCustomFieldSetting::extData() const
{
    return m_extData;
}

void GLDCustomFieldSetting::setExtData(GString value)
{
    m_extData = value;
}

CGLDExtPropDefs *GLDCustomFieldSetting::extPropDefs() const
{
   return m_extProps;
}

GLDCellFormat *GLDCustomFieldSetting::cellFormat() const
{
    return m_cellFormat;
}

void GLDCustomFieldSetting::setCellFormat(GLDCellFormat * value)
{
    m_cellFormat = value;
}

bool GLDCustomFieldSetting::editTextIsDisplayText() const
{
    return m_editTextIsDisplayText;
}

void GLDCustomFieldSetting::setEditTextIsDisplayText(bool value)
{
    m_editTextIsDisplayText = value;
}

bool GLDCustomFieldSetting::isExprEnumDateTimeField(const GString &fieldName)
{
    Q_UNUSED(fieldName);
    return false;
}

bool GLDCustomFieldSetting::isValidFieldName(const GString &fieldName)
{
    Q_UNUSED(fieldName);
    return false;
}

/* GLDFieldSetting */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数
  参数：owner -- 所属容器
-----------------------------------------------------------------------------*/
GLDFieldSetting::GLDFieldSetting(GLDFieldSettings *owner) : GLDCustomFieldSetting(),
    m_owner(owner)
{
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数，释放解析器对象
-----------------------------------------------------------------------------*/
GLDFieldSetting::~GLDFieldSetting()
{
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回设置状态标志
  返回：设计状态标志
-----------------------------------------------------------------------------*/
bool GLDFieldSetting::designState()
{
    return m_owner->owner()->owner()->owner()->designState();
}

GString GLDFieldSetting::cellIdentity() const
{
    GString result;

    if (m_cellIdentity != "")
    {
        result = m_cellIdentity;
    }
    else
    {
        result = m_owner->owner()->owner()->owner()->titleRows()->fullTitle(index());
    }

    return result;
}

GLDCustomGridSetting *GLDFieldSetting::gridSetting()
{
    return m_owner->owner()->owner()->owner();
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回本对象在容器中的下标
  返回：本对象在容器中的下标
-----------------------------------------------------------------------------*/
int GLDFieldSetting::index() const
{
    return m_owner->list().indexOf(const_cast<GLDFieldSetting *>(this));
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回所属表设置下标
  返回：所属表设置下标
-----------------------------------------------------------------------------*/
int GLDFieldSetting::tableIndex()
{
    return m_owner->owner()->index();
}

GString GLDFieldSetting::title()
{
    int nCol(0);
    int nMergeID(0);
    GString result = "";
    int nIndex = this->index();
    GLDTitleRows *oTitleRows = m_owner->owner()->owner()->owner()->titleRows();
    int nRow = oTitleRows->count() - 2;

    while (nRow >= 0)
    {
        nCol = nIndex;
        nMergeID = oTitleRows->items(nRow)->cells(nCol)->mergeID();

        if (nMergeID != 0)
        {
            while (nCol > 0)
            {
                if (oTitleRows->items(nRow)->cells(nCol - 1)->mergeID() != nMergeID)
                {
                    break;
                }

                nCol--;
            }

            while (nRow > 0)
            {
                if (oTitleRows->items(nRow - 1)->cells(nCol)->mergeID() != nMergeID)
                {
                    break;
                }

                nRow--;
            }
        }

        GString strCaption = oTitleRows->items(nRow)->cells(nCol)->caption();

        if (strCaption != "")
        {
            if (result == "")
            {
                result = strCaption;
            }
            else
            {
                result = GString("%1|%2").arg(strCaption, result);
            }
        }

        nRow--;
    }

    return result;
}

bool GLDFieldSetting::isDefaultCellIdentity()
{
    return (m_cellIdentity == "")
           || sameText(m_cellIdentity, m_owner->owner()->owner()->owner()->titleRows()->fullTitle(index()));
}

GString GLDFieldSetting::refFullName()
{
    return m_owner->owner()->refFullName();
}

bool GLDFieldSetting::isFixedCol()
{
    return m_owner->owner()->owner()->owner()->colSettings()->items(index())->isFixedCol();
}

GLDFieldSettings *GLDFieldSetting::owner() const
{
    return m_owner;
}

GLDTableSetting *GLDFieldSetting::tableSetting() const
{
    return m_owner->owner();
}

GLDSortState GLDFieldSetting::sortState() const
{
    return m_sortState;
}

void GLDFieldSetting::setsortState(GLDSortState value)
{
    m_sortState = value;
}

GLDDataType GLDFieldSetting::displayExprDataType()
{
    unsigned char result = 0;

    if (m_owner->owner()->owner()->owner()->designState())
    {
        gldError(getGLDi18nStr(g_NeedRunState));
    }

    return result;
}

/* GLDTitleCell */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDTitleCell::GLDTitleCell(GLDTitleRow *owner)
{
    assert(owner != NULL);
    m_owner = owner;
    m_cellFormat = new GLDCellFormat;
    m_cellFormat->setHorzAlignment(DefTitleHorzAlignment);
    m_isLabel = false;
    m_mergeID = 0;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
GLDTitleCell::~GLDTitleCell()
{
    freeAndNil(m_cellFormat)
}

int GLDTitleCell::index() const
{
    return m_owner->list().indexOf(const_cast<GLDTitleCell *>(this));
}

int GLDTitleCell::colIndex() const
{
    return m_owner->list().indexOf(const_cast<GLDTitleCell *>(this));
}

int GLDTitleCell::rowIndex() const
{
    return m_owner->index();
}

void GLDTitleCell::clearCompileInfo()
{
}

void GLDTitleCell::compile()
{
    m_isLabel = textIsLabel(m_caption);
}

GLDGridSetting *GLDTitleCell::gridSetting()
{
    return m_owner->gridSetting();
}

bool GLDTitleCell::isLabel() const
{
    return m_isLabel;
}

GString GLDTitleCell::caption() const
{
    return m_caption;
}

void GLDTitleCell::setCaption(const GString &value)
{
    m_caption = value;
}

int GLDTitleCell::mergeID() const
{
    return m_mergeID;
}

void GLDTitleCell::setMergeID(const int &value)
{
    m_mergeID = value;
}

GLDCellFormat *GLDTitleCell::cellFormat() const
{
    return m_cellFormat;
}

void GLDTitleCell::setCellFormat(GLDCellFormat *value)
{
    m_cellFormat = value;
}

GLDTitleRow *GLDTitleCell::owner()
{
    return m_owner;
}

void GLDTitleCell::loadFromStream(GStream *stream)
{
    readStrFromStream(stream, m_caption);
    readIntFromStream(stream, m_mergeID);
    m_cellFormat->loadFromStream(stream);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDTitleCell::saveToStream(GStream *stream)
{
    writeStrToStream(stream, m_caption);
    writeIntToStream(stream, m_mergeID);
    m_cellFormat->saveToStream(stream);
}

/* GLDFilterCell */

void GLDFilterCell::clearCompileInfo()
{
}

void GLDFilterCell::compile()
{
    m_isLabel = textIsLabel(m_caption);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDFilterCell::GLDFilterCell(GLDFilterRow *owner)
{
    m_owner = owner;
    m_cellFormat = new GLDCellFormat();
    m_isLabel = false;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
GLDFilterCell::~GLDFilterCell()
{
    freeAndNil(m_cellFormat)
}

GLDGridSetting *GLDFilterCell::gridSetting()
{
    return m_owner->owner()->owner();
}

bool GLDFilterCell::isFixedCol()
{
    return m_owner->owner()->owner()->colSettings()->items(index())->isFixedCol();
}

void GLDFilterCell::loadFromStream(GStream *stream)
{
    readStrFromStream(stream, m_caption);
    readStrFromStream(stream, m_editorName);
    readIntFromStream(stream, m_mergeID);
    m_cellFormat->loadFromStream(stream);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDFilterCell::saveToStream(GStream *stream)
{
    writeStrToStream(stream, m_caption);
    writeStrToStream(stream, m_editorName);
    writeIntToStream(stream, m_mergeID);
    m_cellFormat->saveToStream(stream);
}

int GLDFilterCell::index()
{
    return m_owner->indexOf(const_cast<GLDFilterCell *>(this));
}

int GLDFilterCell::rowIndex()
{
    return m_owner->index();
}

int GLDFilterCell::colIndex()
{
    return m_owner->indexOf(const_cast<GLDFilterCell *>(this));
}

bool GLDFilterCell::isLabel() const
{
    return m_isLabel;
}

GString GLDFilterCell::caption() const
{
    return m_caption;
}

void GLDFilterCell::setCaption(const GString &value)
{
    m_caption = value;
}

GString GLDFilterCell::editorName() const
{
    return m_editorName;
}

void GLDFilterCell::setEditorName(const GString &value)
{
    m_editorName = trim(value);
}

int GLDFilterCell::mergeID() const
{
    return m_mergeID;
}

void GLDFilterCell::setMergeID(const int &value)
{
    m_mergeID = value;
}

GLDCellFormat *GLDFilterCell::cellFormat() const
{
    return m_cellFormat;
}

void GLDFilterCell::setCellFormat(GLDCellFormat *value)
{
    m_cellFormat = value;
}

/* GLDGridRules */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，创建对象列表
  参数：owner -- 表设置对象
-----------------------------------------------------------------------------*/
GLDGridRules::GLDGridRules(GLDTableSetting *owner)
{
    m_owner = owner;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数
-----------------------------------------------------------------------------*/
GLDGridRules::~GLDGridRules()
{
}

GLDCustomGridSetting *GLDGridRules::gridSetting()
{
    return m_owner->owner()->owner();
}

bool GLDGridRules::designState()
{
    return m_owner->owner()->owner()->designState();
}

int GLDGridRules::tableIndex()
{
    return m_owner->index();
}

GString GLDGridRules::refFullName()
{
    return m_owner->refFullName();
}

/* GLDCustomGridRules */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，创建对象列表
  参数：owner -- 表设置对象
-----------------------------------------------------------------------------*/
GLDCustomGridRules::GLDCustomGridRules()
{
    clearRuleArray();
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数
-----------------------------------------------------------------------------*/
GLDCustomGridRules::~GLDCustomGridRules()
{
    clearRuleArray();
    clear();
}

void GLDCustomGridRules::assign(GLDCustomGridRules *source)
{
    GMemoryStream oStream;
    source->saveToStream(&oStream);
    oStream.seek(0);
    loadFromStream(&oStream);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：创建指定类型规则数组
  参数：gridRuleType -- 规则类型
-----------------------------------------------------------------------------*/
void GLDCustomGridRules::buildRuleArray(GLDGridRuleType ruleType)
{
    m_ruleArray[ruleType].clear();

    for (int i = 0; i != (int)m_list.size(); ++i)
    {
        if (m_list[i]->enable() && (m_list[i]->ruleType() == ruleType))
        {
            m_ruleArray[ruleType].push_back(m_list[i]);
        }
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：清空规则类型数组
-----------------------------------------------------------------------------*/
void GLDCustomGridRules::clear()
{
    clearRuleArray();
    m_list.clear();
}

void GLDCustomGridRules::clearRuleArray()
{
    for (int i = 0; i < CGridRuleTypeCount; ++i)
    {
        m_ruleArray[i].clear();
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：删除指定下标规则
  参数：index -- 下标
-----------------------------------------------------------------------------*/
void GLDCustomGridRules::Delete(int index)
{
    assert((index >= 0) && (index < count()));
    GLDGridRuleType eRuleType = items(index)->ruleType();
    m_list.removeAt(index);
    buildRuleArray(eRuleType);
}

bool GLDCustomGridRules::hasRule(GLDGridRuleType ruleType)
{
    return !m_ruleArray[(int)ruleType].empty();
}

GLDGridRule *GLDCustomGridRules::findRule(GLDGridRuleType ruleType, const GString &ruleName)
{
    for (int i = 0; i != m_ruleArray[(int)ruleType].count(); ++i)
    {
        if (sameText(m_ruleArray[(int)ruleType].at(i)->ruleName(), ruleName))
        {
            return m_ruleArray[(int)ruleType].at(i);
        }
    }

    return NULL;
}

int GLDCustomGridRules::count() const
{
    return (int)m_list.size();
}

int GLDCustomGridRules::indexOf(GLDGridRule* value) const
{
    return m_list.indexOf(value);
}

GLDGridRule *GLDCustomGridRules::items(int index) const
{
    return m_list[index];
}

GLDFontRule *GLDCustomGridRules::createFontRule(GLDCustomGridRules *owner)
{
    return new GLDFontRule(owner);
}

GLDBackColorRule *GLDCustomGridRules::createBackColorRule(GLDCustomGridRules *owner)
{
    return new GLDBackColorRule(owner);
}

GLDEditStyleRule *GLDCustomGridRules::createEditStyleRule(GLDCustomGridRules *owner)
{
    return new GLDEditStyleRule(owner);
}

GLDBoundLineRule *GLDCustomGridRules::createBoundLineRule(GLDCustomGridRules *owner)
{
    return new GLDBoundLineRule(owner);
}

GLDAlignRule *GLDCustomGridRules::createAlignRule(GLDCustomGridRules *owner)
{
    return new GLDAlignRule(owner);
}

GLDMarginRule *GLDCustomGridRules::createMarginRule(GLDCustomGridRules *owner)
{
    return new GLDMarginRule(owner);
}

GLDReadonlyRule *GLDCustomGridRules::createReadonlyRule(GLDCustomGridRules *owner)
{
    return new GLDReadonlyRule(owner);
}

GLDCanFocusRule *GLDCustomGridRules::createCanFocusRule(GLDCustomGridRules *owner)
{
    return new GLDCanFocusRule(owner);
}

GLDHandleSymbolRule *GLDCustomGridRules::createHandleSymbolRule(GLDCustomGridRules *owner)
{
    return new GLDHandleSymbolRule(owner);
}

GLDImageRule *GLDCustomGridRules::createImageRule(GLDCustomGridRules *owner)
{
    return new GLDImageRule(owner);
}

GLDCommentRule *GLDCustomGridRules::createCommentRule(GLDCustomGridRules *owner)
{
    return new GLDCommentRule(owner);
}

GLDVisibleRule *GLDCustomGridRules::createVisibleRule(GLDCustomGridRules *owner)
{
    return new GLDVisibleRule(owner);
}

GLDMergeRule *GLDCustomGridRules::createMergeRule(GLDCustomGridRules *owner)
{
    return new GLDMergeRule(owner);
}

GLDRejectDeleteRule *GLDCustomGridRules::createRejectDeleteRule(GLDCustomGridRules *owner)
{
    return new GLDRejectDeleteRule(owner);
}

GLDRejectInsertRule *GLDCustomGridRules::createRejectInsertRule(GLDCustomGridRules *owner)
{
    return new GLDRejectInsertRule(owner);
}

GLDRejectInsertChildRule *GLDCustomGridRules::createRejectInsertChileRule(GLDCustomGridRules *owner)
{
    return new GLDRejectInsertChildRule(owner);
}

GLDRejectMoveRule *GLDCustomGridRules::createRejectMoveRule(GLDCustomGridRules *owner)
{
    return new GLDRejectMoveRule(owner);
}

GLDRejectLevelRule *GLDCustomGridRules::createRejectLevelRule(GLDCustomGridRules *owner)
{
    return new GLDRejectLevelRule(owner);
}

GLDEditFormRule *GLDCustomGridRules::creataeEditFormRule(GLDCustomGridRules *owner)
{
    return new GLDEditFormRule(owner);
}

GLDClearCellRule *GLDCustomGridRules::createClearCellRule(GLDCustomGridRules *owner)
{
    return new GLDClearCellRule(owner);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：插入规则
  参数：index -- 插入下标
        ruleType -- 插入的规则类型
  返回：插入的规则对象
-----------------------------------------------------------------------------*/
GLDGridRule *GLDCustomGridRules::insert(int index, GLDGridRuleType ruleType)
{
    GLDGridRule *result;

    switch (ruleType)
    {
        case grtFont:
            result = createFontRule(this);
            break;

        case grtBackColor:
            result = createBackColorRule(this);
            break;

        case grtEditStyle:
            result = createEditStyleRule(this);
            break;

        case grtBoundLine:
            result = createBoundLineRule(this);
            break;

        case grtAlignment:
            result = createAlignRule(this);
            break;

        case grtMargin:
            result = createMarginRule(this);
            break;

        case grtReadonly:
            result = createReadonlyRule(this);
            break;

        case grtCanFocus:
            result = createCanFocusRule(this);
            break;

        case grtHandleSymbol:
            result = createHandleSymbolRule(this);
            break;

        case grtImage:
            result = createImageRule(this);
            break;

        case grtComment:
            result = createCommentRule(this);
            break;

        case grtVisible:
            result = createVisibleRule(this);
            break;

        case grtMerge:
            result = createMergeRule(this);
            break;

        case grtRejectDelete:
            result = createRejectDeleteRule(this);
            break;

        case grtRejectInsert:
            result = createRejectInsertRule(this);
            break;

        case grtRejectInsertChild:
            result = createRejectInsertChileRule(this);
            break;

        case grtRejectMove:
            result = createRejectMoveRule(this);
            break;

        case grtRejectLevel:
            result = createRejectLevelRule(this);
            break;

        case grtEditForm:
            result = creataeEditFormRule(this);
            break;

        case grtClearCell:
            result = createClearCellRule(this);
            break;

        default:
            result = NULL;
            return result;
    }

    if (index == -1)
    {
        m_list.push_back(result);
    }
    else
    {
        m_list.insert(index, result);
    }

    buildRuleArray(ruleType);

    return result;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDCustomGridRules::loadFromStream(GStream *stream)
{
    int nints(0);
    unsigned char ucvalue;
    clear();
    readIntFromStream(stream, nints);

    for (int i = 0; i <= nints - 1; i++)
    {
        readByteFromStream(stream, ucvalue);
        insert(-1, (GLDGridRuleType)ucvalue)->loadFromStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：移动规则
  参数：fromIndex -- 源下标
        toIndex -- 目标下标
-----------------------------------------------------------------------------*/
void GLDCustomGridRules::move(int fromIndex, int toIndex)
{
    assert((fromIndex >= 0) && (fromIndex < count()));
    assert((toIndex >= 0) && (toIndex < count()));

    if (fromIndex == toIndex)
    {
        return;
    }

    GLDGridRuleType eRuleType = items(fromIndex)->ruleType();
    m_list.move(fromIndex, toIndex);
    buildRuleArray(eRuleType);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDCustomGridRules::saveToStream(GStream *stream)
{
    writeIntToStream(stream, count());

    for (int i = 0; i <= count() - 1; i++)
    {
        writeByteToStream(stream, byte(items(i)->ruleType()));
        items(i)->saveToStream(stream);
    }
}

bool GLDCustomGridRules::designState()
{
    return false;
}

int GLDCustomGridRules::tableIndex()
{
    return -1;
}

/* GLDFontRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDFontRule::GLDFontRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_fontName = CDefFontName;
    m_fontSize = CDefFontSize;
    m_fontStyles = CDefFontStyles;
    m_fontColor = CDefFontColor;
}

GLDFontRule::~GLDFontRule()
{
}

bool GLDFontRule::autoRefresh() const
{
    return true;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDFontRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);

    byte fontStyles;
    int nFontColor;
    readStrFromStream(stream, m_fontName);
    readIntFromStream(stream, m_fontSize);
    readByteFromStream(stream, fontStyles);
    readIntFromStream(stream, nFontColor);

    m_fontStyles = (FontStyles)fontStyles;
    nFontColor = (GRgb)nFontColor;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDFontRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeStrToStream(stream, m_fontName);
    writeIntToStream(stream, m_fontSize);
    writeByteToStream(stream, (byte)m_fontStyles);
    writeIntToStream(stream, (int)m_fontColor);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDFontRule::ruleType()
{
    return grtFont;
}

GRgb GLDFontRule::fontColor() const
{
    return m_fontColor;
}

void GLDFontRule::setFontColor(GRgb value)
{
    m_fontColor = value;
}

GString GLDFontRule::fontName() const
{
    return m_fontName;
}

void GLDFontRule::setFontName(const GString &value)
{
    m_fontName = value;
}

int GLDFontRule::fontSize() const
{
    return m_fontSize;
}

void GLDFontRule::setFontSize(int value)
{
    m_fontSize = value;
}

FontStyles GLDFontRule::fontStyles() const
{
    return m_fontStyles;
}

void GLDFontRule::setFontStyles(FontStyles value)
{
    m_fontStyles = value;
}

/* GLDGridRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数
-----------------------------------------------------------------------------*/
GLDGridRule::GLDGridRule(GLDCustomGridRules *owner)
{
    m_owner = owner;
    m_enable = true;

    if (owner)
    {
        m_fixedCols = owner->gridSetting()->fixedCols();
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：析构函数
-----------------------------------------------------------------------------*/
GLDGridRule::~GLDGridRule()
{
}

bool GLDGridRule::autoRefresh() const
{
    return false;
}

void GLDGridRule::clearCompileInfo()
{
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：编译，构造记录条件解析器
-----------------------------------------------------------------------------*/
void GLDGridRule::compile()
{
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回设计状态标志
  返回：设计状态标志
-----------------------------------------------------------------------------*/
bool GLDGridRule::designState()
{
    return m_owner->designState();
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则下标
  返回：当前规则在容器中的下标
-----------------------------------------------------------------------------*/
int GLDGridRule::index()
{
    return m_owner->indexOf(this);
}

GString GLDGridRule::fieldCondition() const
{
    return m_fieldCondition;
}

void GLDGridRule::setFieldCondition(const GString &value)
{
    m_fieldCondition = trim(value);
}

GString GLDGridRule::recordCondition() const
{
    return m_recordCondition;
}

void GLDGridRule::setRecordCondition(const GString &value)
{
    m_recordCondition = trim(value);
}

GString GLDGridRule::ruleName() const
{
    return m_ruleName;
}

void GLDGridRule::setRuleName(const GString &value)
{
    m_ruleName = value;
}

int GLDGridRule::varCount()
{
    return -1;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDGridRule::loadFromStream(GStream *stream)
{
    readStrFromStream(stream, m_ruleName);
    readBoolFromStream(stream, m_enable);
    readMemoFromStream(stream, m_recordCondition);
    readMemoFromStream(stream, m_fieldCondition);
}

GString GLDGridRule::refFullName()
{
    return m_owner->refFullName();
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDGridRule::saveToStream(GStream *stream)
{
    writeStrToStream(stream, m_ruleName);
    writeBoolToStream(stream, m_enable);
    writeMemoToStream(stream, m_recordCondition);
    writeMemoToStream(stream, m_fieldCondition);
}

bool GLDGridRule::enable() const
{
    return m_enable;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.12.06
  功能：设置Enable标志
  参数：value -- 规则Enable标志
-----------------------------------------------------------------------------*/
void GLDGridRule::setEnable(const bool value)
{
    if (m_enable == value)
    {
        return;
    }

    m_enable = value;
    m_owner->buildRuleArray(ruleType());
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回表设置下标
  返回：表设置下标
-----------------------------------------------------------------------------*/
int GLDGridRule::tableIndex()
{
    return m_owner->tableIndex();
}

/* GLDBackColorRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDBackColorRule::GLDBackColorRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_backColor = CDefBackColor;
}

GLDBackColorRule::~GLDBackColorRule()
{
}

bool GLDBackColorRule::autoRefresh() const
{
    return true;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDBackColorRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    int nvalue;
    readIntFromStream(stream, nvalue);
    m_backColor = (GRgb)nvalue;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDBackColorRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeIntToStream(stream, (int)m_backColor);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDBackColorRule::ruleType()
{
    return grtBackColor;
}

GRgb GLDBackColorRule::backColor() const
{
    return m_backColor;
}

void GLDBackColorRule::setBackColor(GRgb value)
{
    m_backColor = value;
}

/* GLDEditStyleRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDEditStyleRule::GLDEditStyleRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_editStyle = CDefEditStyle;
}

GLDEditStyleRule::~GLDEditStyleRule()
{
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDEditStyleRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    byte value;
    readByteFromStream(stream, value);
    m_editStyle = (GLDEditStyle)value;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDEditStyleRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeByteToStream(stream, (byte)m_editStyle);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDEditStyleRule::ruleType()
{
    return grtEditStyle;
}

GLDEditStyle GLDEditStyleRule::editStyle() const
{
    return m_editStyle;
}

void GLDEditStyleRule::setEditStyle(GLDEditStyle value)
{
    m_editStyle = value;
}

/* GLDBoundLineRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDBoundLineRule::GLDBoundLineRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_rightLine = CDefRightLine;
    m_bottomLine = CDefBottomLine;
    m_diagonal = CDefDiagonal;
    m_antiDiagonal = CDefAntiDiagonal;
}

GLDBoundLineRule::~GLDBoundLineRule()
{
}

bool GLDBoundLineRule::autoRefresh() const
{
    return true;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDBoundLineRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    readByteFromStream(stream, m_rightLine);
    readByteFromStream(stream, m_bottomLine);
    readByteFromStream(stream, m_diagonal);
    readByteFromStream(stream, m_antiDiagonal);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDBoundLineRule::ruleType()
{
    return grtBoundLine;
}

byte GLDBoundLineRule::antiDiagonal() const
{
    return m_antiDiagonal;
}

void GLDBoundLineRule::setAntiDiagonal(const byte value)
{
    m_antiDiagonal = value;
}

byte GLDBoundLineRule::bottomLine() const
{
    return m_bottomLine;
}

void GLDBoundLineRule::setBottomLine(const byte value)
{
    m_bottomLine = value;
}

byte GLDBoundLineRule::diagonal() const
{
    return m_diagonal;
}

void GLDBoundLineRule::setDiagonal(const byte value)
{
    m_diagonal = value;
}

byte GLDBoundLineRule::rightLine() const
{
    return m_rightLine;
}

void GLDBoundLineRule::setRightLine(const byte value)
{
    m_rightLine = value;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDBoundLineRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeByteToStream(stream, m_rightLine);
    writeByteToStream(stream, m_bottomLine);
    writeByteToStream(stream, m_diagonal);
    writeByteToStream(stream, m_antiDiagonal);
}

/* GLDAlignRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDAlignRule::GLDAlignRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_horzAlignment = CDefHorzAlignment;
    m_vertAlignment = CDefVertAlignment;
}

GLDAlignRule::~GLDAlignRule()
{
}

bool GLDAlignRule::autoRefresh() const
{
    return true;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDAlignRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    byte hvalue;
    byte vvalue;
    readByteFromStream(stream, hvalue);
    readByteFromStream(stream, vvalue);

    m_horzAlignment = (GLDHorzAlignment)hvalue;
    m_vertAlignment = (GLDVertAlignment)(vvalue);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDAlignRule::ruleType()
{
    return grtAlignment;
}

GLDHorzAlignment GLDAlignRule::horzAlignment() const
{
    return m_horzAlignment;
}

void GLDAlignRule::setHorzAlignment(const GLDHorzAlignment value)
{
    m_horzAlignment = value;
}

GLDVertAlignment GLDAlignRule::vertAlignment() const
{
    return m_vertAlignment;
}

void GLDAlignRule::setVertAlignment(const GLDVertAlignment value)
{
    m_vertAlignment = value;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDAlignRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeByteToStream(stream, (byte)m_horzAlignment);
    writeByteToStream(stream, (byte)m_vertAlignment);
}

/* GLDMarginRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDMarginRule::GLDMarginRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_leftMargin = CDefHorzMargin;
    m_rightMargin = CDefHorzMargin;
    m_topMargin = CDefVertMargin;
    m_bottomMargin = CDefVertMargin;
}

GLDMarginRule::~GLDMarginRule()
{
}

bool GLDMarginRule::autoRefresh() const
{
    return true;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDMarginRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);

    readByteFromStream(stream, m_leftMargin);
    readByteFromStream(stream, m_rightMargin);
    readByteFromStream(stream, m_topMargin);
    readByteFromStream(stream, m_bottomMargin);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDMarginRule::ruleType()
{
    return grtMargin;
}

byte GLDMarginRule::leftMargin() const
{
    return m_leftMargin;
}

void GLDMarginRule::setLeftMargin(const byte value)
{
    m_leftMargin = value;
}

byte GLDMarginRule::rightMargin() const
{
    return m_rightMargin;
}

void GLDMarginRule::setRightMargin(const byte value)
{
    m_rightMargin = value;
}

byte GLDMarginRule::topMargin() const
{
    return m_topMargin;
}

void GLDMarginRule::setTopMargin(const byte value)
{
    m_topMargin = value;
}

byte GLDMarginRule::bottomMargin() const
{
    return m_bottomMargin;
}

void GLDMarginRule::setBottomMargin(const byte value)
{
    m_bottomMargin = value;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDMarginRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeByteToStream(stream, byte(m_leftMargin));
    writeByteToStream(stream, byte(m_rightMargin));
    writeByteToStream(stream, byte(m_topMargin));
    writeByteToStream(stream, byte(m_bottomMargin));
}

/* GLDReadonlyRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDReadonlyRule::GLDReadonlyRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_readonly = CDefReadonly;
}

GLDReadonlyRule::~GLDReadonlyRule()
{
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDReadonlyRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    readBoolFromStream(stream, m_readonly);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDReadonlyRule::ruleType()
{
    return grtReadonly;
}

bool GLDReadonlyRule::readonly() const
{
    return m_readonly;
}

void GLDReadonlyRule::setReadonly(bool value)
{
    m_readonly = value;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDReadonlyRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeBoolToStream(stream, m_readonly);
}

/* GLDHandleSymbolRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDHandleSymbolRule::GLDHandleSymbolRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_handleSymbol = CDefHandleSymbol;
}

GLDHandleSymbolRule::~GLDHandleSymbolRule()
{
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDHandleSymbolRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    readBoolFromStream(stream, m_handleSymbol);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDHandleSymbolRule::ruleType()
{
    return grtHandleSymbol;
}

bool GLDHandleSymbolRule::handleSymbol() const
{
    return m_handleSymbol;
}
void GLDHandleSymbolRule::setHandleSymbol(bool value)
{
    m_handleSymbol = value;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDHandleSymbolRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeBoolToStream(stream, m_handleSymbol);
}

/* GLDImageRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDImageRule::GLDImageRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_imageIndex = CDefImageIndex;
    m_imageLayout = CDefImageLayout;
}

GLDImageRule::~GLDImageRule()
{
}

bool GLDImageRule::autoRefresh() const
{
    return true;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDImageRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    readIntFromStream(stream, m_imageIndex);
    byte imageLayout;
    readByteFromStream(stream, imageLayout);
    m_imageLayout = (GLDCellImageLayout)imageLayout;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDImageRule::ruleType()
{
    return grtImage;
}

int GLDImageRule::imageIndex() const
{
    return m_imageIndex;
}

void GLDImageRule::setImageIndex(int value)
{
    m_imageIndex = value;
}

GLDCellImageLayout GLDImageRule::imageLayout() const
{
    return m_imageLayout;
}

void GLDImageRule::setImageLayout(GLDCellImageLayout value)
{
    m_imageLayout = value;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDImageRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeIntToStream(stream, m_imageIndex);
    writeByteToStream(stream, (byte)m_imageLayout);
}

/* GLDCommentRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDCommentRule::GLDCommentRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_showComment = CDefShowComment;
    m_comment = CDefComment;
}

GLDCommentRule::~GLDCommentRule()
{
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDCommentRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    readBoolFromStream(stream, m_showComment);
    readStrFromStream(stream, m_comment);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDCommentRule::ruleType()
{
    return grtComment;
}

GString GLDCommentRule::comment() const
{
    return m_comment;
}

void GLDCommentRule::setComment(const GString &value)
{
    m_comment = value;
}

bool GLDCommentRule::showComment() const
{
    return m_showComment;
}

void GLDCommentRule::setShowComment(bool value)
{
    m_showComment = value;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDCommentRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeBoolToStream(stream, m_showComment);
    writeStrToStream(stream, m_comment);
}

/* GLDVisibleRule */

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDVisibleRule::GLDVisibleRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
}

GLDVisibleRule::~GLDVisibleRule()
{
}

bool GLDVisibleRule::autoRefresh() const
{
    return true;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDVisibleRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    readStrFromStream(stream, m_inVisibleText);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDVisibleRule::ruleType()
{
    return grtVisible;
}

GString GLDVisibleRule::inVisibleText() const
{
    return m_inVisibleText;
}

void GLDVisibleRule::setInVisibleText(GString value)
{
    m_inVisibleText = value;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDVisibleRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeStrToStream(stream, m_inVisibleText);
}

/* GLDMergeRule */

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDMergeRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    readStrFromStream(stream, m_mergeExpr);
}

bool GLDMergeRule::autoRefresh() const
{
    return true;
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDMergeRule::ruleType()
{
    return grtMerge;
}

GString GLDMergeRule::mergeExpr() const
{
    return m_mergeExpr;
}

void GLDMergeRule::setMergeExpr(const GString &value)
{
    m_mergeExpr = trim(value);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDMergeRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeStrToStream(stream, m_mergeExpr);
}

/* GLDRejectDeleteRule */

GLDGridRuleType GLDRejectDeleteRule::ruleType()
{
    return grtRejectDelete;
}

/* GLDRejectRule */

void GLDRejectRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    readIntFromStream(stream, m_isTree);
}

void GLDRejectRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeIntToStream(stream, m_isTree);
}

int GLDRejectRule::isTree() const
{
    return m_isTree;
}

void GLDRejectRule::setIsTree(int value)
{
    m_isTree = value;
}
/* GLDRejectInsertRule */

int GLDRejectInsertRule::isBefore() const
{
    return m_isBefore;
}

void GLDRejectInsertRule::setIsBefore(const int &value)
{
    m_isBefore = value;
}

GString GLDRejectInsertRule::tableNames() const
{
    return m_tableNames;
}

void GLDRejectInsertRule::setTableNames(const GString &value)
{
    m_tableNames = value;
}

void GLDRejectInsertRule::loadFromStream(GStream *stream)
{
    GLDRejectRule::loadFromStream(stream);
    readStrFromStream(stream, m_tableNames);
    readIntFromStream(stream, m_isBefore);
}

void GLDRejectInsertRule::saveToStream(GStream *stream)
{
    GLDRejectRule::saveToStream(stream);
    writeStrToStream(stream, m_tableNames);
    writeIntToStream(stream, m_isBefore);
}

GLDGridRuleType GLDRejectInsertRule::ruleType()
{
    return grtRejectInsert;
}

/* GLDRejectInsertChildRule */

int GLDRejectInsertChildRule::level() const
{
    return m_level;
}

void GLDRejectInsertChildRule::setLevel(int value)
{
    m_level = value;
}

GLDRejectInsertChildRule::~GLDRejectInsertChildRule()
{
}

void GLDRejectInsertChildRule::loadFromStream(GStream *stream)
{
    GLDRejectInsertRule::loadFromStream(stream);
    readIntFromStream(stream, m_level);
}

GLDGridRuleType GLDRejectInsertChildRule::ruleType()
{
    return grtRejectInsertChild;
}

void GLDRejectInsertChildRule::saveToStream(GStream *stream)
{
    GLDRejectInsertRule::saveToStream(stream);
    writeIntToStream(stream, m_level);
}

/* GLDRejectMoveRule */

int GLDRejectMoveRule::isMoveUp() const
{
    return m_isMoveUp;
}

void GLDRejectMoveRule::setIsMoveUp(int value)
{
    m_isMoveUp = value;
}

int GLDRejectMoveRule::includeChild() const
{
    return m_includeChild;
}

void GLDRejectMoveRule::setIncludeChild(int value)
{
    m_includeChild = value;
}

void GLDRejectMoveRule::saveToStream(GStream *stream)
{
    GLDRejectRule::saveToStream(stream);
    writeIntToStream(stream, m_isMoveUp);
    writeIntToStream(stream, m_includeChild);
}

void GLDRejectMoveRule::loadFromStream(GStream *stream)
{
    GLDRejectRule::loadFromStream(stream);
    readIntFromStream(stream, m_isMoveUp);
    readIntFromStream(stream, m_includeChild);
}

GLDGridRuleType GLDRejectMoveRule::ruleType()
{
    return grtRejectMove;
}

/* GLDRejectLevelRule */

int GLDRejectLevelRule::isLevelUp() const
{
    return m_isLevelUp;
}

void GLDRejectLevelRule::setIsLevelUp(int value)
{
    m_isLevelUp = value;
}

int GLDRejectLevelRule::isFixPos() const
{
    return m_isFixPos;
}

void GLDRejectLevelRule::setIsFixPos(int value)
{
    m_isFixPos = value;
}

void GLDRejectLevelRule::loadFromStream(GStream *stream)
{
    GLDRejectRule::loadFromStream(stream);
    readIntFromStream(stream, m_isLevelUp);
    readIntFromStream(stream, m_isFixPos);
}

GLDGridRuleType GLDRejectLevelRule::ruleType()
{
    return grtRejectLevel;
}

void GLDRejectLevelRule::saveToStream(GStream *stream)
{
    GLDRejectRule::saveToStream(stream);
    writeIntToStream(stream, m_isLevelUp);
    writeIntToStream(stream, m_isFixPos);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：构造函数，设置缺省值
-----------------------------------------------------------------------------*/
GLDEditFormRule::GLDEditFormRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_enable = true;
}

GLDEditFormRule::~GLDEditFormRule()
{
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDEditFormRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    readBoolFromStream(stream, m_enable);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.10
  功能：返回规则类型
  返回：规则类型
-----------------------------------------------------------------------------*/
GLDGridRuleType GLDEditFormRule::ruleType()
{
    return grtEditForm;
}

bool GLDEditFormRule::enable() const
{
    return m_enable;
}

void GLDEditFormRule::setEnable(bool value)
{
    m_enable = value;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDEditFormRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeBoolToStream(stream, m_enable);
}

/* GLDClearCellRule */

GLDClearCellRule::GLDClearCellRule(GLDCustomGridRules *owner)
    : GLDGridRule(owner)
{
    m_allowClear = true;
}

GLDClearCellRule::~GLDClearCellRule()
{
}

bool GLDClearCellRule::allowClear() const
{
    return m_allowClear;
}

void GLDClearCellRule::setAllowClear(bool value)
{
    m_allowClear = value;
}

void GLDClearCellRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    readBoolFromStream(stream, m_allowClear);
}

GLDGridRuleType GLDClearCellRule::ruleType()
{
    return grtClearCell;
}

void GLDClearCellRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeBoolToStream(stream, m_allowClear);
}

/* GLDRecordGridSetting */

void GLDRecordGridSetting::doLoadFromXML(GXMLDocument &doc)
{
    GLDRecordGridSettingXMLReader oRead;
    oRead.read(doc, this);
}

void GLDRecordGridSetting::doSaveToXML(GXMLDocument &doc)
{
    GLDRecordGridSettingXMLWriter oWriter;
    oWriter.write(doc, this);
}

void GLDRecordGridSetting::doLoadFromFile(const GString &fileName)
{
    //assert(m_persistentStyle == psFromFile);
    GLDRecordGridSettingXMLReader oRead;
    oRead.read(fileName, this);
}

GString GLDRecordGridSetting::intAryToStr(GIntList &ary)
{
    GString result = "";

    for (int i = 0; i != ary.size(); i++)
    {
        if (0 == i)
        {
            result = GString::number(ary[i], 10);
        }
        else
        {
            result = result + ";" + GString::number(ary[i], 10);
        }
    }

    return result;

}

void GLDRecordGridSetting::setLength(GIntList &ary, int count)
{
    ary.clear();

    for (int i = 0; i != count; i++)
    {
        ary.push_back(0);
    }
}

void GLDRecordGridSetting::setLength(GStringList &ary, int count)
{
    ary.clear();

    for (int i = 0; i != count; i++)
    {
        ary.push_back("");
    }
}

GString GLDRecordGridSetting::colWidthsToStr()
{
//    return intAryToStr(m_originColWidths);//Delphi中取原始数据？
    return intAryToStr(m_colWidths);
}

void GLDRecordGridSetting::compile()
{
    m_recordSettings->compile();
    m_fieldSettings->compile();
    m_fieldCols.clear();

    for (int i = 0; i != m_colStyles.size(); i++)
    {
        if (m_colStyles[i] != 0)
        {
            m_fieldCols.push_back(i);
        }
    }

    setLength(m_colWidths, m_colCount);
    setLength(m_colZoomWidths, m_colCount);

    for (int i = 0; i != m_colCount; i++)
    {
        m_colWidths[i] = m_originColWidths[i];
    }
}

void GLDRecordGridSetting::initialize()
{
    m_recordSettings = new GLDRecordSettings(this);
    m_fieldSettings = new GLDRecordFieldSettings(this);
    m_extProps = new CGLDExtPropDefs();
    m_extProps->AddRef();

    for (int i = 0; i <= m_rowCount * m_colCount - 1; i++)
    {
        m_fieldSettings->insert(-1);
    }
}

GLDRecordGridSetting::GLDRecordGridSetting() : m_fixedRows(1), m_ignoreInvalidCell(true),
    m_allowLabelSelection(false), m_rowCount(5), m_colCount(4)
{
    setLength(m_rowHeights, m_rowCount);
    setLength(m_colWidths, m_colCount);
    setLength(m_colZoomWidths, m_colCount);
    setLength(m_originColWidths, m_colCount);
    setLength(m_minColWidths, m_colCount);
    setLength(m_colStyles, m_colCount);
    setLength(m_colIdentitys, m_colCount);

    for (int i = 0; i < m_rowCount; i++)
    {
        m_rowHeights[i] = CDefRowHeight;
    }

    for (int i = 0; i < m_colCount; i++)
    {
        m_originColWidths[i] = CDefDisplayWidth;
        m_colWidths[i] = CDefDisplayWidth;
        m_minColWidths[i] = -1;
    }

    m_recordSettings = NULL;
    m_fieldSettings = NULL;
}

GLDRecordGridSetting::~GLDRecordGridSetting()
{
    setActive(false);
    freeAndNil(m_recordSettings);
    freeAndNil(m_fieldSettings);
    m_extProps->Release();
}

GLDRecordGridSetting *GLDRecordGridSetting::createGLDRecordGridSetting()
{
    GLDRecordGridSetting *result = new GLDRecordGridSetting();
    result->initialize();
    return result;
}

void GLDRecordGridSetting::deleteCol(int index)
{
    assert((index >= 0) && (index < m_colCount));

    for (int i = m_rowCount - 1; i >= 0; i--)
    {
        m_fieldSettings->Delete(i * m_colCount + index);
    }

    m_colCount--;

    for (int i = index; i <= m_colCount - 1; i++)
    {
        m_colWidths[i] = m_colWidths[i + 1];
        m_originColWidths[i] = m_originColWidths[i + 1];
        m_minColWidths[i] = m_minColWidths[i + 1];
    }

    m_colWidths.removeAt(m_colCount);
    m_colZoomWidths.removeAt(m_colCount);
    m_originColWidths.removeAt(m_colCount);
    m_minColWidths.removeAt(m_colCount);
    m_colStyles.removeAt(m_colCount);
    m_colIdentitys.removeAt(m_colCount);

    if (index < m_fixedCols)
    {
        m_fixedCols--;
    }
}

void GLDRecordGridSetting::deleteRow(int index)
{
    //assert((index >= 0) &&(index < m_rowCount));
    for (int i = 0; i <= m_colCount - 1; i++)
    {
        m_fieldSettings->Delete(m_colCount * index);
    }

    m_rowCount--;

    for (int i = index; i <= m_rowCount - 1; i++)
    {
        m_rowHeights[i] = m_rowHeights[i + 1];
    }

    m_rowHeights.removeAt(m_rowCount);

    if (index < m_fixedRows)
    {
        m_fixedRows--;
    }
}
GLDRecordSettings *GLDRecordGridSetting::recordSettings()
{
    return m_recordSettings;
}

GLDRecordFieldSettings *GLDRecordGridSetting::fieldSettings() const
{
    return m_fieldSettings;
}

GLDRecordFieldSetting *GLDRecordGridSetting::cells(int col, int row)
{
    assert((col >= 0) && (col < m_colCount));
    assert((row >= 0) && (row < m_rowCount));
    return m_fieldSettings->items(row * m_colCount + col);
}

GString GLDRecordGridSetting::saveDialogDefaultExt()
{
    return "RGF";
}

GString GLDRecordGridSetting::saveDialogFilter()
{
    return "RecordGridSetting files(*.RGF)|*.RGF";
}

void GLDRecordGridSetting::insertCol(int index)
{
    //assert((index >= - 1) &&(index <= m_colCount));
    if (index == -1)
    {
        index = m_colCount;
    }

    for (int i = m_rowCount - 1; i >= 0; i--)
    {
        m_fieldSettings->insert(i * m_colCount + index);
    }

    m_colCount++;
    m_colWidths.append(0);
    m_colZoomWidths.append(0);
    m_originColWidths.append(0);
    m_minColWidths.append(0);
    m_colStyles.append(0);
    m_colIdentitys.append(0);

    for (int i = m_colCount - 2; i >= 0; i--)
    {
        if (i >= index)
        {
            m_colWidths[i + 1] = m_colWidths[i];
            m_originColWidths[i + 1] = m_originColWidths[i];
            m_minColWidths[i + 1] = m_minColWidths[i];
        }
    }

    m_colWidths[index] = CDefDisplayWidth;
    m_originColWidths[index] = CDefDisplayWidth;
    m_minColWidths[index] = -1;
}

void GLDRecordGridSetting::insertRow(int index)
{
    if (index == -1)
    {
        index = m_rowCount;
    }

    for (int i = 0; i <= m_colCount - 1; i++)
    {
        m_fieldSettings->insert(m_colCount * index);
    }

    m_rowCount++;
    m_rowHeights.append(0);

    for (int i = m_rowCount - 2; i >= 0; i--)
    {
        if (i >= index)
        {
            m_rowHeights[i + 1] = m_rowHeights[i];
        }
    }

    m_rowHeights[index] = CDefRowHeight;
}

void GLDRecordGridSetting::loadColWidthsFromStr(const GString &s)
{
    GStringList strings = s.split(";");
    int nIndex = 0;

    for (QStringList::Iterator it = strings.begin(); it != strings.end(); ++it)
    {
        GString value = *it;
        m_originColWidths[nIndex] = value.toInt();
        m_colWidths[nIndex] = value.toInt();
        nIndex++;
    }
}

void GLDRecordGridSetting::loadFromStream(GStream *stream)
{
    GLDCustomGridSetting::loadFromStream(stream);

    setRowCount(readIntFromStream(stream));
    setColCount(readIntFromStream(stream));
    m_fixedRows = readWordFromStream(stream);
    m_ignoreInvalidCell = readBoolFromStream(stream);
    m_allowLabelSelection = readBoolFromStream(stream);
    loadRowHeightsFromStr(readStrFromStream(stream));
    loadColWidthsFromStr(readStrFromStream(stream));
    loadMinColWidthsFromStr(readStrFromStream(stream));
    loadColStylesFromStr(readStrFromStream(stream));
    loadColIdentitysFromStr(readStrFromStream(stream));
    m_recordSettings->loadFromStream(stream);
    m_fieldSettings->loadFromStream(stream);
}

void GLDRecordGridSetting::loadGridSetting()
{
    doLoad();
    doAfterActive();
}

void GLDRecordGridSetting::loadMinColWidthsFromStr(const GString &s)
{
    GStringList strings = s.split(";");

    //   assert(count() == m_colCount);
    int nIndex = 0;

    for (GStringList::Iterator it = strings.begin(); it != strings.end(); ++it)
    {
        GString value = *it;
        m_minColWidths[nIndex] = value.toInt();
        nIndex++;
    }
}

void GLDRecordGridSetting::loadRowHeightsFromStr(const GString &s)
{
    GStringList strings = s.split(";");
    //   assert(count() == m_colCount);
    int nIndex = 0;
    int nCount = strings.count() ;

    if (nCount > m_rowCount)
    {
        setRowCount(nCount);
    }

    for (QStringList::Iterator it = strings.begin(); it != strings.end(); ++it)
    {
        GString strValue = *it;
        m_rowHeights[nIndex] = strValue.toInt();
        nIndex++;
    }
}

GString GLDRecordGridSetting::minColWidthsToStr()
{
    return intAryToStr(m_minColWidths);
}

void GLDRecordGridSetting::moveCol(int fromIndex, int toIndex)
{
    assert((fromIndex >= 0) && (fromIndex < m_rowCount));
    assert((toIndex >= 0) && (toIndex < m_rowCount));
    assert(fromIndex != toIndex);

    for (int i = 0; i <= m_rowCount - 1; i++)
    {
        m_fieldSettings->list().move(i * m_colCount + fromIndex, i * m_colCount + toIndex);
    }

    int nColWidth = m_colWidths[fromIndex];
    m_colWidths[fromIndex] = m_colWidths[toIndex];
    m_colWidths[toIndex] = nColWidth;
    nColWidth = m_originColWidths[fromIndex];
    m_originColWidths[fromIndex] = m_originColWidths[toIndex];
    m_originColWidths[toIndex] = nColWidth;
    nColWidth = m_minColWidths[fromIndex];
    m_minColWidths[fromIndex] = m_minColWidths[toIndex];
    m_minColWidths[toIndex] = nColWidth;
}

void GLDRecordGridSetting::moveRow(int fromIndex, int toIndex)
{
    //assert((fromIndex >= 0) &&(fromIndex < m_rowCount));
    //assert((toIndex >= 0) &&(toIndex < m_rowCount));
    //assert(fromIndex != toIndex);
    for (int i = 0; i <= m_colCount - 1; i++)
    {
        m_fieldSettings->list().move(fromIndex * m_colCount + i, toIndex * m_colCount + i);
    }

    int nRowHeight = m_rowHeights[fromIndex];
    m_rowHeights[fromIndex] = m_rowHeights[toIndex];
    m_rowHeights[toIndex] = nRowHeight;
}

GString GLDRecordGridSetting::rowHeightsToStr()
{
    return intAryToStr(m_rowHeights);
}

void GLDRecordGridSetting::saveToFile(const GString &fileName)
{
    GLDRecordGridSettingXMLWriter oWriter;
    oWriter.write(fileName, this);
}

void GLDRecordGridSetting::saveToStream(GStream *stream)
{
    GLDCustomGridSetting::saveToStream(stream);
    writeIntToStream(stream, rowCount());
    writeIntToStream(stream, colCount());
    writeWordToStream(stream, m_fixedRows);
    writeBoolToStream(stream, m_ignoreInvalidCell);
    writeBoolToStream(stream, m_allowLabelSelection);
    writeStrToStream(stream, rowHeightsToStr());
    writeStrToStream(stream, colWidthsToStr());
    writeStrToStream(stream, minColWidthsToStr());
    writeStrToStream(stream, colStylesToStr());
    writeStrToStream(stream, colIdentitysToStr());
    m_recordSettings->saveToStream(stream);
    m_fieldSettings->saveToStream(stream);
}

void GLDRecordGridSetting::setColCount(const int value)
{
    int nOffset(0);

    //assert(value >= 0);
    if (m_colCount == value)
    {
        return;
    }

    if (m_colCount < value)
    {
        nOffset = value - m_colCount;

        for (int i = 0; i <= nOffset - 1; i++)
        {
            insertCol(-1);
        }
    }
    else
    {
        nOffset = m_colCount - value;

        for (int i = 0; i <= nOffset - 1; i++)
        {
            deleteCol(m_colCount - 1);
        }
    }
}

void GLDRecordGridSetting::setOriginColWidths(int index, const int value)
{
    assert((index >= 0) && (index < m_colCount));
    m_originColWidths[index] = value;
    m_colWidths[index] = value;
}

void GLDRecordGridSetting::setMinColWidths(int index, const int value)
{
    assert((index >= 0) && (index < m_colCount));
    m_minColWidths[index] = value;
}

void GLDRecordGridSetting::setRowCount(const int value)
{
    int nOffset(0);
    assert(value >= 0);

    if (m_rowCount == value)
    {
        return;
    }

    if (m_rowCount < value)
    {
        nOffset = value - m_rowCount;

        for (int i = 0; i <= nOffset - 1; i++)
        {
            insertRow(-1);
        }
    }
    else
    {
        nOffset = m_rowCount - value;

        for (int i = 0; i <= nOffset - 1; i++)
        {
            deleteRow(m_rowCount - 1);
        }
    }
}

void GLDRecordGridSetting::setRowHeights(int index, const int value)
{
    assert((index >= 0) && (index < m_rowCount));

    if (value < 0)
    {
        return;
    }

    m_rowHeights[index] = value;
}

GLDRecordFieldSetting *GLDRecordGridSetting::fieldsetting(int row, int col)
{
    assert((col >= 0) && (col < m_colCount));
    assert((row >= 0) && (row < m_rowCount));
    return fieldSettings()->items(row * m_colCount + col);
}

void GLDRecordGridSetting::clearCompileInfo()
{
    m_recordSettings->clearCompileInfo();
    m_fieldSettings->clearCompileInfo();
}

void GLDRecordGridSetting::clearData()
{
    m_rowCount = 0;
    m_colCount = 0;
    m_fixedRows = 0;
    m_fixedCols = 0;
    m_fixedEditRows = 0;
    m_fixedEditCols = 0;
    m_rowHeights.clear();
    m_colWidths.clear();
    m_originColWidths.clear();
    m_minColWidths.clear();
    m_colStyles.clear();
    m_colIdentitys.clear();
    m_recordSettings->clear();
    m_fieldSettings->clear();
}

GLDCellSuitType GLDRecordGridSetting::colStyles(int index)
{
    assert((index >= 0) && (index < m_colCount));
    return (GLDCellSuitType) m_colStyles[index];
}

void GLDRecordGridSetting::setColStyles(int index, const GLDCellSuitType value)
{
    assert((index >= 0) && (index < m_colCount));
    m_colStyles[index] = (int)value;
}

void GLDRecordGridSetting::setColWidths(int index, const int value)
{
    assert((index >= 0) && (index < m_colCount));
    m_colWidths[index] = value;
}

void GLDRecordGridSetting::setColZoomWidths(int index, const int value)
{
    assert((index >= 0) && (index < m_colCount));
    m_colZoomWidths[index] = value;
}

GString GLDRecordGridSetting::colStylesToStr()
{
    return intAryToStr(m_colStyles);
}

void GLDRecordGridSetting::loadColStylesFromStr(const GString &s)
{
    GStringList strings = s.split(";");
    //   assert(count() == m_colCount);
    int nCounter = 0;

    for (GStringList::Iterator it = strings.begin(); it != strings.end(); ++it)
    {
        GString strValue = *it;
        m_colStyles[nCounter] = strValue.toInt();
        nCounter++;
    }
}

GString GLDRecordGridSetting::colIdentitysToStr()
{
    GString result = m_colIdentitys[0];

    for (int i = 1; i <= m_colCount - 1; i++)
    {
        result = result + CDelimiter + m_colIdentitys[i];
    }

    return result;
}

void GLDRecordGridSetting::loadColIdentitysFromStr(const GString &s)
{
    GStringList strings = s.split(CDelimiter);
    int nIndex = 0;

    for (GStringList::Iterator it = strings.begin(); it != strings.end(); ++it)
    {
        m_colIdentitys[nIndex] = *it;
        nIndex++;
    }
}

void GLDRecordGridSetting::setColIdentitys(int index, const GString &value)
{
    assert((index >= 0) && (index < m_colCount));
    m_colIdentitys[index] = value;
}

GIntList GLDRecordGridSetting::suitColWidthCols()
{
    GIntList result;

    for (int i = 0; i != m_colCount; i++)
    {
        if (m_colStyles[i] == gcstSuitColWidth)
        {
            result.push_back(i);
        }
    }

    return result;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2006-01-05
  功能: 返回自动折行的列的下标列表
-----------------------------------------------------------------------------*/
GIntList GLDRecordGridSetting::suitRowHeightCols()
{
    GIntList result;

    for (int i = 0; i != m_colCount; i++)
    {
        if (m_colStyles[i] == gcstSuitRowHeight)
        {
            result.push_back(i);
        }
    }

    return result;
}

GIntList GLDRecordGridSetting::fitColWidthCols()
{
    GIntList result;

    int nFixedColCount = fixedCols();
    for (int i = 0; i != m_colCount; i++)
    {
        if (m_colStyles[i] == gcstFitColWidth)
        {
            result.push_back(i - nFixedColCount);
        }
    }

    return result;
}

int GLDRecordGridSetting::rowCount() const
{   
    return m_rowCount;
}

int GLDRecordGridSetting::colCount() const
{
    return m_colCount;
}

int GLDRecordGridSetting::rowHeights(const int index) const
{
    return m_rowHeights[index];
}

int GLDRecordGridSetting::colWidths(int index) const
{
    return m_colWidths[index];
}

int GLDRecordGridSetting::colZoomWidths(int index) const
{
    return m_colZoomWidths[index];
}

int GLDRecordGridSetting::originColWidths(int index) const
{
    return m_originColWidths[index];
}
int GLDRecordGridSetting::minColWidths(int index) const
{
    return m_minColWidths[index];
}

GString GLDRecordGridSetting::colIdentitys(int index) const
{
    return m_colIdentitys[index];
}

unsigned short GLDRecordGridSetting::fixedRows() const
{
    return m_fixedRows;
}

void GLDRecordGridSetting::setFixedRows(unsigned short value)
{
    m_fixedRows = value;
}

bool GLDRecordGridSetting::ignoreInvalidCell() const
{
    return m_ignoreInvalidCell;
}

void GLDRecordGridSetting::setIgnoreInvalidCell(bool value)
{
    m_ignoreInvalidCell = value;
}

bool GLDRecordGridSetting::allowLabelSelection() const
{
    return m_allowLabelSelection;
}

void GLDRecordGridSetting::setAllowLabelSelection(bool value)
{
    m_allowLabelSelection = value;
}

/* GLDRecordSettings */

GLDRecordSettings::GLDRecordSettings(GLDRecordGridSetting *owner)
{
    m_owner = owner;
    insert(-1);
}

GLDRecordSettings::~GLDRecordSettings()
{
    clear();
}

void GLDRecordSettings::clear()
{
    m_list.clear();
}

int GLDRecordSettings::count() const
{
    return (int)m_list.size();
}

GLDRecordSetting *GLDRecordSettings::items(int index) const
{
    return m_list[index];
}

GLDRecordSetting *GLDRecordSettings::operator[](int index)
{
    return items(index);
}

GLDRecordGridSetting *GLDRecordSettings::owner()
{
    return m_owner;
}

void GLDRecordSettings::clearCompileInfo()
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->clearCompileInfo();
    }
}

void GLDRecordSettings::compile()
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->compile();
    }
}

void GLDRecordSettings::Delete(int index)
{
    assert((index >= 0) && (index < count()));
    m_list.removeAt(index);
}

GLDRecordSetting *GLDRecordSettings::insert(int index)
{
    GLDRecordSetting *result = createRecordSetting(this);

    if (index == -1)
    {
        m_list.push_back(result);
    }
    else
    {
        m_list.insert(index, result);
    }

    return result;
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 从流对象中装载数据
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDRecordSettings::loadFromStream(GStream *stream)
{
    int nInts(0);
    readIntFromStream(stream, nInts);
    clear();

    for (int i = 0; i <= nInts - 1; i++)
    {
        insert()->loadFromStream(stream);
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-12
  功能: 保存数据到流对象中
  参数: stream -- 流对象
-----------------------------------------------------------------------------*/
void GLDRecordSettings::saveToStream(GStream *stream)
{
    writeIntToStream(stream, count());

    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->saveToStream(stream);
    }
}

void GLDRecordSettings::move(int fromIndex, int toIndex)
{
    assert((fromIndex >= 0) && (fromIndex < count()));
    assert((toIndex >= 0) && (toIndex < count()));

    if (fromIndex == toIndex)
    {
        return;
    }

    m_list.move(fromIndex, toIndex);
}

void GLDRecordSettings::setCount(const int value)
{
    int nSize = (int)m_list.size();

    if (value == nSize)
    {
        return;
    }

    if (value > nSize)
    {
        for (int i = nSize; i <= value - 1; i++)
        {
            m_list.push_back(createRecordSetting(this));
        }
    }
}

GLDRecordSetting *GLDRecordSettings::doCreateRecordSetting(GLDRecordSettings *owner)
{
    return new GLDRecordSetting(owner);
}

GLDRecordSetting *GLDRecordSettings::createRecordSetting(GLDRecordSettings *owner)
{
    GLDRecordSetting *result = doCreateRecordSetting(owner);
    result->initialize();
    return result;
}

/* GLDRecordSetting */

void GLDRecordSetting::initialize()
{
    m_rules = createGridRules(this);
}

GLDRecordSetting::GLDRecordSetting(GLDRecordSettings *owner)
{
    assert(owner != NULL);
    m_owner = owner;
}

GLDRecordSetting::~GLDRecordSetting()
{
    freeAndNil(m_rules)
}

void GLDRecordSetting::clearCompileInfo()
{
    for (int i = 0; i <= m_rules->count() - 1; i++)
    {
        m_rules->items(i)->clearCompileInfo();
    }
}

void GLDRecordSetting::compile()
{
    for (int i = 0; i <= m_rules->count() - 1; i++)
    {
        m_rules->items(i)->compile();
    }
}

GString GLDRecordSetting::refFullName()
{
    if (m_refDatabaseName != "")
    {
        return m_refDatabaseName + "." + m_refTableName;
    }
    else
    {
        return m_refTableName;
    }
}

GString GLDRecordSetting::refDatabaseName() const
{
    return m_refDatabaseName;
}

void GLDRecordSetting::setRefDatabaseName(const GString &value)
{
    m_refDatabaseName = value;
}

GString GLDRecordSetting::refTableName() const
{
    return m_refTableName;
}

void GLDRecordSetting::setRefTableName(const GString &value)
{
    m_refTableName = value;
}

GLDRecordGridRules *GLDRecordSetting::rules()
{
    return m_rules;
}

GLDRecordSettings *GLDRecordSetting::owner()
{
    return m_owner;
}

GLDRecordGridRules *GLDRecordSetting::createGridRules(GLDRecordSetting *owner)
{
    return new GLDRecordGridRules(owner);
}

void GLDRecordSetting::loadFromStream(GStream *stream)
{
    readStrFromStream(stream, m_refDatabaseName);
    readStrFromStream(stream, m_refTableName);
    m_rules->loadFromStream(stream);
}

void GLDRecordSetting::saveToStream(GStream *stream)
{
    writeStrToStream(stream, m_refDatabaseName);
    writeStrToStream(stream, m_refTableName);
    m_rules->saveToStream(stream);
}

/* GLDRecordFieldSettings */

GLDRecordFieldSettings::GLDRecordFieldSettings(GLDRecordGridSetting *owner)
{
    m_owner = owner;
}

GLDRecordFieldSettings::~GLDRecordFieldSettings()
{
    clear();
}

void GLDRecordFieldSettings::Delete(int index)
{
    assert((index >= 0) && (index < m_list.count()));
    m_list.removeAt(index);
}

void GLDRecordFieldSettings::clear()
{
    m_list.clear();
}

GLDRecordFieldSetting *GLDRecordFieldSettings::items(int index) const
{
    return m_list[index];
}

GLDRecordFieldSetting *GLDRecordFieldSettings::operator[](int index)
{
    return items(index);
}

int GLDRecordFieldSettings::indexOf(GLDRecordFieldSetting *value) const
{
    return m_list.indexOf(value);
}

GObjectList<GLDRecordFieldSetting *> &GLDRecordFieldSettings::list()
{
    return m_list;
}

int GLDRecordFieldSettings::count()
{
    return (int)m_list.size();
}

GLDRecordGridSetting *GLDRecordFieldSettings::owner() const
{
    return m_owner;
}

GLDRecordFieldSetting *GLDRecordFieldSettings::createFieldSetting(GLDRecordFieldSettings *owner)
{
    return new GLDRecordFieldSetting(owner);
}

void GLDRecordFieldSettings::clearCompileInfo()
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->clearCompileInfo();
    }
}

void GLDRecordFieldSettings::compile()
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->compile();
    }
}

GLDRecordFieldSetting *GLDRecordFieldSettings::insert(int index)
{
//    assert((index >= - 1) && (index <= m_list.count()()));
    GLDRecordFieldSetting *result = createFieldSetting(this);

    if (index == -1)
    {
        m_list.push_back(result);
    }
    else
    {
        m_list.insert(index, result);
    }

    return result;
}

void GLDRecordFieldSettings::loadFromStream(GStream *stream)
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->loadFromStream(stream);
    }
}

void GLDRecordFieldSettings::saveToStream(GStream *stream)
{
    for (int i = 0; i <= count() - 1; i++)
    {
        items(i)->saveToStream(stream);
    }
}

/* GLDRecordFieldSetting */

GLDRecordFieldSetting::GLDRecordFieldSetting(GLDRecordFieldSettings *owner)
{
    m_owner = owner;
    m_visible = true;
    m_isLabel = false;
    m_isPasswordChar = false;
}

GLDRecordFieldSetting::~GLDRecordFieldSetting()
{
}

void GLDRecordFieldSetting::assign(GLDRecordFieldSetting *fieldSetting)
{
    assert(fieldSetting != NULL);
    GMemoryStream oStream;
    fieldSetting->saveToStream(&oStream);
    oStream.seek(0);
    loadFromStream(&oStream);
}

bool GLDRecordFieldSetting::isLabel() const
{
    return m_isLabel;
}

void GLDRecordFieldSetting::setIsLabel(bool value)
{
    m_isLabel = value;
}

bool GLDRecordFieldSetting::visible() const
{
    return m_visible;
}

void GLDRecordFieldSetting::setVisible(const bool &value)
{
    m_visible = value;
}

GString GLDRecordFieldSetting::visibleCondition() const
{
    return m_visibleCondition;
}

void GLDRecordFieldSetting::setVisibleCondition(const GString &value)
{
    m_visibleCondition = trim(value);
}

int GLDRecordFieldSetting::dataSourceID() const
{
    return m_dataSourceID;
}

void GLDRecordFieldSetting::setDataSourceID(const int value)
{
    m_dataSourceID = value;
}

bool GLDRecordFieldSetting::isPasswordChar() const
{
    return m_isPasswordChar;
}

void GLDRecordFieldSetting::setIsPasswordChar(bool value)
{
    m_isPasswordChar = value;
}

GLDRecordFieldSettings *GLDRecordFieldSetting::owner()
{
    return m_owner;
}

void GLDRecordFieldSetting::clearCompileInfo()
{
    GLDCustomFieldSetting::clearCompileInfo();
    m_isLabel = false;
    m_visible = true;
}

void GLDRecordFieldSetting::compile()
{
    GString strExpr;

    try
    {
        m_isLabel = false;
        GLDCustomFieldSetting::compile();
        strExpr = trimExpression(m_visibleCondition);

        if (strExpr != "")
        {
            doNothingMacro();//
        }
    }
    catch (...)
    {
        if (!m_owner->owner()->ignoreInvalidCell())
        {
            throw;
        }
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2006-09-13
  功能：Index 相当于 ColNo
-----------------------------------------------------------------------------*/
int GLDRecordFieldSetting::colNo()
{
    int nIndex = recordGridSetting()->fieldSettings()->indexOf(this);
    return nIndex % recordGridSetting()->colCount();
}

GLDDataType GLDRecordFieldSetting::displayExprDataType()
{
    if (m_owner->owner()->designState())
    {
        gldError("GSPGridSetting_SNeedRunState");
    }

    return 0;
}

GLDCustomGridSetting *GLDRecordFieldSetting::gridSetting()
{
    return m_owner->owner();
}

int GLDRecordFieldSetting::index() const
{
    return recordGridSetting()->fieldSettings()->indexOf(const_cast<GLDRecordFieldSetting *>(this));
}

bool GLDRecordFieldSetting::designState()
{
    return m_owner->owner()->designState();
}

GLDRecordSetting *GLDRecordFieldSetting::recordSetting()
{
    if (m_owner->owner()->recordSettings()->count() == 0)
    {
        return NULL;
    }
    else
    {
        return m_owner->owner()->recordSettings()->items(m_dataSourceID);
    }
}

int GLDRecordFieldSetting::rowNo()
{
    int nIndex = recordGridSetting()->fieldSettings()->indexOf(this);
    return nIndex / recordGridSetting()->colCount();
}

GLDRecordGridSetting *GLDRecordFieldSetting::recordGridSetting() const
{
    return m_owner->owner();
}

void GLDRecordFieldSetting::loadFromStream(GStream *stream)
{
    GLDCustomFieldSetting::loadFromStream(stream);
    readIntFromStream(stream, m_dataSourceID);
    readBoolFromStream(stream, m_isPasswordChar);
}

void GLDRecordFieldSetting::saveToStream(GStream *stream)
{
    GLDCustomFieldSetting::saveToStream(stream);
    writeIntToStream(stream, m_dataSourceID);
    writeBoolToStream(stream, m_isPasswordChar);
}

GString GLDRecordFieldSetting::refFullName()
{
    return m_owner->owner()->recordSettings()->items(m_dataSourceID)->refFullName();
}

int GLDRecordFieldSetting::tableIndex()
{
    int nIndex = recordGridSetting()->fieldSettings()->indexOf(this);
    return nIndex / recordGridSetting()->rowCount();
}

/* GLDRecordGridRules */

GLDRecordGridRules::GLDRecordGridRules(GLDRecordSetting *owner)
{
    m_owner = owner;
}

bool GLDRecordGridRules::designState()
{
    return m_owner->owner()->owner()->designState();
}

GLDRecordGridRules::~GLDRecordGridRules()
{
}

GLDCustomGridSetting *GLDRecordGridRules::gridSetting()
{
    return m_owner->owner()->owner();
}

int GLDRecordGridRules::tableIndex()
{
    return 0;
}

GString GLDRecordGridRules::refFullName()
{
    return m_owner->refFullName();
}

bool isNullTitleCell(GLDTitleCell *cell)
{
    return (cell->mergeID() == 0) && (cell->caption() == "")
           && isDefaultCellFormat(cell->cellFormat(), ghaCenter);
}

bool isDefaultCellFormat(GLDCellFormat *cellFormat, GLDHorzAlignment defHorzAlignment)
{
    return sameText(cellFormat->fontName(), CDefFontName)
           && sameText(cellFormat->formatStr(), CDefFormatStr)
           && (cellFormat->fontSize() == DefFontSize)
           && (cellFormat->foreColor() == DefForeColor)
           && (cellFormat->backColor() == DefBackColor)
           //todo yaok(cellFormat->fontStyle() == DefFontStyle) &&
           && (cellFormat->leftLineWidth() == DefLeftLineWidth)
           && (cellFormat->topLineWidth() == DefTopLineWidth)
           && (cellFormat->rightLineWidth() == DefRightLineWidth)
           && (cellFormat->bottomLineWidth() == DefBottomLineWidth)
           && (cellFormat->diagonalWidth() == DefDiagonalWidth)
           && (cellFormat->antiDiagonalWidth() == DefAntiDiagonalWidth)
           && (cellFormat->horzAlignment() == defHorzAlignment)
           && (cellFormat->vertAlignment() == DefVertAlignment)
           && (cellFormat->nullIsZero() == DefNullIsZero);
}

bool isNullFilterCell(GLDFilterCell *filterCell)
{
    return (filterCell->mergeID() == 0)
           && (filterCell->caption() == "")
           && (filterCell->editorName() == "")
           && isDefaultCellFormat(filterCell->cellFormat());
}

bool isNullFieldSetting(GLDFieldSetting *fieldSetting)
{
    return (fieldSetting->mergeID() == 0)
           && (fieldSetting->editFieldName() == "")
           && (fieldSetting->displayExpr() == "")
           && (fieldSetting->saveFieldName() == "")
           && (fieldSetting->saveRule() == "")
           && (fieldSetting->nullValDisplayStr() == "")
           && (fieldSetting->extData() == "")
           && (fieldSetting->editorName() == "")
           && !fieldSetting->editTextIsDisplayText()
           && fieldSetting->isDefaultCellIdentity()
           && isDefaultCellFormat(fieldSetting->cellFormat())
           && true;
    //todo yaok  (fieldSetting->ExtPropDefs.Count = 0);
}

bool isNullExpandFieldSetting(GLDExpandRowFieldSetting *fieldSetting)
{
    return (fieldSetting->mergeID() == 0)
           && (fieldSetting->value() == "")
           && isDefaultCellFormat(fieldSetting->cellFormat());
}

bool isNullFieldSetting(GLDRecordFieldSetting *fieldSetting)
{
    return (fieldSetting->dataSourceID() == 0)
           && (fieldSetting->mergeID() == 0)
           && (fieldSetting->editFieldName() == "")
           && (fieldSetting->displayExpr() == "")
           && (fieldSetting->saveFieldName() == "")
           && (fieldSetting->saveRule() == "")
           && (fieldSetting->nullValDisplayStr() == "")
           && (fieldSetting->visibleCondition() == "")
           && (fieldSetting->extData() == "")
           && (fieldSetting->cellIdentity() == "")
           && !fieldSetting->editTextIsDisplayText()
           && !fieldSetting->isPasswordChar()
           && isDefaultCellFormat(fieldSetting->cellFormat());
    //todo yaok(fieldSetting->ExtPropDefs.Count = 0);
}

/* GLDCanFocusRule */

GLDCanFocusRule::GLDCanFocusRule(GLDCustomGridRules *owner) : GLDGridRule(owner)
{
    m_canFocus = CDefCanFocus;
}

GLDCanFocusRule::~GLDCanFocusRule()
{
}

void GLDCanFocusRule::loadFromStream(GStream *stream)
{
    GLDGridRule::loadFromStream(stream);
    readBoolFromStream(stream, m_canFocus);
}

void GLDCanFocusRule::saveToStream(GStream *stream)
{
    GLDGridRule::saveToStream(stream);
    writeBoolToStream(stream, m_canFocus);
}

GLDGridRuleType GLDCanFocusRule::ruleType()
{
    return grtCanFocus;
}

bool GLDCanFocusRule::canFocus() const
{
    return m_canFocus;
}

void GLDCanFocusRule::setCanFocus(const bool value)
{
    m_canFocus = value;
}
