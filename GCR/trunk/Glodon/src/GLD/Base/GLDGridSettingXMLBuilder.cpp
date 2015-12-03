#include "GLDGridSettingXMLBuilder.h"
#include "GLDFile.h"
#include "GLDGlobal.h"
#include "GLDException.h"
#include "GLDStreamUtils.h"
#include "GLDXMLUtils.h"
#include "GLDStrUtils.h"
#include "GLDStrings.h"
#include "GLDTextStream.h"

//const char *SInvalidXMLNode		 = QT_TRANSLATE_NOOP("GLD", "Invalid configuration file");// "配置文件非法");
//const char *SNeedXMLNode		 = QT_TRANSLATE_NOOP("GLD", "XML Node field cannot be null");// "XML节点不能为空");

GString g_OldVersion;
int g_DefHeaderRowHeight;
int g_DefTotalRowHeight;

static GRgb g_clSilver = qRgb(125,125,125);
static GRgb g_clWindow = qRgb(255,255,255);
static GRgb g_clBlack  = qRgb(0,0,0);

GLDCellSuitType StrToCellSuitType(const GString &type)
{
    bool ok;
    int nValue;
    if (type.length() == 0)
        return gcstFixed;

    nValue = type.toInt(&ok);
    if (ok)
        return (GLDCellSuitType)nValue;

    for (int i=0; i<=3; i++)
    {
        if (sameText(c_GLDCellSuitTypeStrings[i], type))
            return (GLDCellSuitType)i;
    }

    return gcstFixed;
}

GString CellSuitTypeToStr(GLDCellSuitType type)
{
    return c_GLDCellSuitTypeStrings[(int)type];
}

GLDGridRuleType StrToGridRuleType(const GString &type)
{
    bool ok;
    int nValue;
    if (type.length() == 0)
        return grtFont;

    nValue = type.toInt(&ok);
    if (ok)
        return (GLDGridRuleType)nValue;

    for (int i=0; i<=19; i++)
    {
        if (sameText(c_GLDGridRuleTypeStrings[i], type))
            return (GLDGridRuleType)i;
    }

    return grtFont;
}

GString GridRuleTypeToStr(GLDGridRuleType type)
{
    return c_GLDGridRuleTypeStrings[(int)type];
}

/* GLDRecordGridSettingXMLReader */
GLDRecordGridSettingXMLReader::GLDRecordGridSettingXMLReader()
{

}

GLDRecordGridSettingXMLReader::~GLDRecordGridSettingXMLReader()
{

}

/* GLDGridSettingXMLBaseReader */
GLDGridSettingXMLBaseReader::~GLDGridSettingXMLBaseReader()
{
}

void GLDGridSettingXMLBaseReader::readGridRule(const GXMLNode &node, GLDGridRule *rule)
{
    unsigned char ucFontStyles;
//    if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.3"))
//    {
//        rule->setRuleName(readStrFromXML(node, "a:RuleName"));
//        rule->setEnable(readBoolFromXML(node, "a:Enable"));
//    }
//    else
    {
        rule->setRuleName(readStrFromXMLAttr(node, "Name"));
        rule->setEnable(readBoolFromXMLAttr(node, "Enable"));
    }
    rule->setRecordCondition(readStrFromXML(node, "a:RecordCondition"));
    rule->setFieldCondition(readStrFromXML(node, "a:FieldCondition"));
    switch (rule->ruleType())
    {
    case grtClearCell:
    {
        dynamic_cast<GLDClearCellRule *>(rule)->setAllowClear(readBoolFromXML(node, "a:AllowClear", true));
    }
        break;
    case grtFont:
    {
        dynamic_cast<GLDFontRule *>(rule)->setFontName(readStrFromXML(node, "a:FontName"));
        dynamic_cast<GLDFontRule *>(rule)->setFontSize(readIntFromXML(node, "a:FontSize"));
        dynamic_cast<GLDFontRule *>(rule)->setFontColor((GRgb)readIntFromXML(node, "a:FontColor"));
        ucFontStyles = readIntFromXML(node, "a:FontStyles");
        dynamic_cast<GLDFontRule *>(rule)->setFontStyles((FontStyles)ucFontStyles);
    }
        break;
    case grtBackColor:
        dynamic_cast<GLDBackColorRule *>(rule)->setBackColor(readIntFromXML(node, "a:BackColor"));
        break;
    case grtEditStyle:
        dynamic_cast<GLDEditStyleRule *>(rule)->setEditStyle((GLDEditStyle)readIntFromXML(node, "a:EditStyle"));
        break;
    case grtBoundLine:
    {
        dynamic_cast<GLDBoundLineRule *>(rule)->setRightLine(readIntFromXML(node, "a:RightLine"));
        dynamic_cast<GLDBoundLineRule *>(rule)->setBottomLine(readIntFromXML(node, "a:BottomLine"));
        dynamic_cast<GLDBoundLineRule *>(rule)->setDiagonal(readIntFromXML(node, "a:Diagonal"));
        dynamic_cast<GLDBoundLineRule *>(rule)->setAntiDiagonal(readIntFromXML(node, "a:AntiDiagonal"));
    }
        break;
    case grtAlignment:
    {
        dynamic_cast<GLDAlignRule *>(rule)->setHorzAlignment((GLDHorzAlignment) readIntFromXML(node,
                                                                                               "a:HorzAlignment"));
        dynamic_cast<GLDAlignRule *>(rule)->setVertAlignment((GLDVertAlignment) readIntFromXML(node,
                                                                                               "a:VertAlignment"));
    }
        break;
    case grtMargin:
    {
        dynamic_cast<GLDMarginRule *>(rule)->setLeftMargin(readIntFromXML(node, "a:LeftMargin"));
        dynamic_cast<GLDMarginRule *>(rule)->setRightMargin(readIntFromXML(node, "a:RightMargin"));
        dynamic_cast<GLDMarginRule *>(rule)->setTopMargin(readIntFromXML(node, "a:TopMargin"));
        dynamic_cast<GLDMarginRule *>(rule)->setBottomMargin(readIntFromXML(node, "a:BottomMargin"));
    }
        break;
    case grtReadonly:
        dynamic_cast<GLDReadonlyRule *>(rule)->setReadonly(readBoolFromXML(node, "a:Readonly"));
        break;
    case grtCanFocus:
        dynamic_cast<GLDCanFocusRule *>(rule)->setCanFocus(readBoolFromXML(node, "a:CanFocus"));
        break;
    case grtHandleSymbol:
        dynamic_cast<GLDHandleSymbolRule *>(rule)->setHandleSymbol(readBoolFromXML(node, "a:HandleSymbol"));
        break;
    case grtImage:
    {
        dynamic_cast<GLDImageRule *>(rule)->setImageIndex(readIntFromXML(node, "a:ImageIndex"));
        dynamic_cast<GLDImageRule *>(rule)->setImageLayout((GLDCellImageLayout)readIntFromXML(node, "a:ImageLayout"));
    }
        break;
    case grtComment:
    {
        dynamic_cast<GLDCommentRule *>(rule)->setShowComment(readBoolFromXML(node, "a:ShowComment"));
        dynamic_cast<GLDCommentRule *>(rule)->setComment(readStrFromXML(node, "a:Comment"));
    }
        break;
    case grtVisible:
        dynamic_cast<GLDVisibleRule *>(rule)->setInVisibleText(readStrFromXML(node, "a:InVisibleText"));
        break;
    case grtMerge:
    {
        dynamic_cast<GLDMergeRule *>(rule)->setMergeExpr(readStrFromXML(node, "a:MergeExpr"));
    }
        break;
    case grtRejectDelete:
    {
        dynamic_cast<GLDRejectDeleteRule *>(rule)->setIsTree(readIntFromXML(node, "a:IsTree"));
    }
        break;
    case grtRejectInsert:
    {
        dynamic_cast<GLDRejectInsertRule *>(rule)->setIsTree(readIntFromXML(node, "a:IsTree"));
        dynamic_cast<GLDRejectInsertRule *>(rule)->setIsBefore(readIntFromXML(node, "a:IsBefore"));
        dynamic_cast<GLDRejectInsertRule *>(rule)->setTableNames(readStrFromXML(node, "a:TableNames"));
    }
        break;
    case grtRejectInsertChild:
    {
        dynamic_cast<GLDRejectInsertChildRule *>(rule)->setIsTree(readIntFromXML(node, "a:IsTree"));
        dynamic_cast<GLDRejectInsertChildRule *>(rule)->setIsBefore(readIntFromXML(node, "a:IsBefore"));
        dynamic_cast<GLDRejectInsertChildRule *>(rule)->setTableNames(readStrFromXML(node, "a:TableNames"));
        dynamic_cast<GLDRejectInsertChildRule *>(rule)->setLevel(readIntFromXML(node, "a:Level"));
    }
        break;
    case grtRejectMove:
    {
        dynamic_cast<GLDRejectMoveRule *>(rule)->setIsTree(readIntFromXML(node, "a:IsTree"));
        dynamic_cast<GLDRejectMoveRule *>(rule)->setIsMoveUp(readIntFromXML(node, "a:IsMoveUp"));
        dynamic_cast<GLDRejectMoveRule *>(rule)->setIncludeChild(readIntFromXML(node, "a:IncludeChild"));
    }
        break;
    case grtRejectLevel:
    {
        dynamic_cast<GLDRejectLevelRule *>(rule)->setIsTree(readIntFromXML(node, "a:IsTree"));
        dynamic_cast<GLDRejectLevelRule *>(rule)->setIsLevelUp(readIntFromXML(node, "a:IsLevelUp"));
        dynamic_cast<GLDRejectLevelRule *>(rule)->setIsFixPos(readIntFromXML(node, "a:IsFixPos"));
    }
        break;
    case grtEditForm:
        dynamic_cast<GLDEditFormRule *>(rule)->setEnable(readBoolFromXML(node, "a:Enable"));
        break;
    }
}

void GLDGridSettingXMLBaseReader::readCellFormat(const GXMLNode &node, GLDCellFormat *cellFormat,
                                                 GLDHorzAlignment defHorzAlignment)
{
    cellFormat->setFontName(readStrFromXMLAttr(node, "FontName", CDefFontName));
    cellFormat->setFontSize(readIntFromXMLAttr(node, "FontSize", DefFontSize));
    cellFormat->setForeColor(readIntFromXMLAttr(node, "ForeColor", DefForeColor));
    cellFormat->setBackColor(readIntFromXMLAttr(node, "BackColor", DefBackColor));
    cellFormat->setFontStyle((FontStyles)readIntFromXMLAttr(node, "FontStyle", 0));
    cellFormat->setLeftLineWidth(readIntFromXMLAttr(node, "LeftLineWidth", DefLeftLineWidth));
    cellFormat->setTopLineWidth(readIntFromXMLAttr(node, "TopLineWidth", DefTopLineWidth));
    cellFormat->setRightLineWidth(readIntFromXMLAttr(node, "RightLineWidth", DefRightLineWidth));
    cellFormat->setBottomLineWidth(readIntFromXMLAttr(node, "BottomLineWidth", DefBottomLineWidth));
    cellFormat->setDiagonalWidth(readIntFromXMLAttr(node, "DiagonalWidth", DefDiagonalWidth));
    cellFormat->setAntiDiagonalWidth(readIntFromXMLAttr(node, "AntiDiagonalWidth", DefAntiDiagonalWidth));
    cellFormat->setHorzAlignment((GLDHorzAlignment) readIntFromXMLAttr(node, "HorzAlignment", (int)DefHorzAlignment));
    cellFormat->setVertAlignment((GLDVertAlignment) readIntFromXMLAttr(node, "VertAlignment", (int)DefVertAlignment));
    cellFormat->setFormatStr(readStrFromXMLAttr(node, "FormatStr", CDefFormatStr));
    cellFormat->setNullIsZero(readBoolFromXMLAttr(node, "NullIsZero", DefNullIsZero));
    cellFormat->setLeftMargin(readIntFromXMLAttr(node, "LeftMargin", DefHorzMargin));
    cellFormat->setRightMargin(readIntFromXMLAttr(node, "RightMargin", DefHorzMargin));
    cellFormat->setTopMargin(readIntFromXMLAttr(node, "TopMargin", DefVertMargin));
    cellFormat->setBottomMargin(readIntFromXMLAttr(node, "BottomMargin", DefVertMargin));
    cellFormat->setImageIndex(readIntFromXMLAttr(node, "ImageIndex", DefImageIndex));
    cellFormat->setImageLayout((GLDCellImageLayout)readIntFromXMLAttr(node, "ImageLayout", (int)CDefImageLayout));
    Q_UNUSED(defHorzAlignment);
}

void GLDGridSettingXMLBaseReader::readExtPropDefs(const GXMLNode &node, CGLDExtPropDefs *extPropDefs)
{
    CGLDExtPropDef *propDef;
    extPropDefs->clear();
    if (node.isNull())
        return;
    //    if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.3"))
    //      {
    //        for (i = 0; i != node.childNodes().count() - 1; i++)
    //          {
    //            iNode = node.childNodes(i);
    //            if (!sameText(iNode.name(), "o:ExtPropDef"))
    //              Continue;
    //            /*with extPropDefs.insert(-1) do*/
    //            {
    //              setCode(readStrFromXML(iNode, "a:Code"));
    //              setValue(readStrFromXML(iNode, "a:Value"));
    //            }
    //        }
    //    }
    //  else
    GXMLNode iNode= node.firstChildElement();
    while (!iNode.isNull())
    {
        if (!sameText(iNode.nodeName(), GLatin1String("o:ExtPropDef")))
            continue;
        propDef = extPropDefs->insertObj(-1);
        propDef->setCode(readStrFromXMLAttr(iNode, "Code"));
        propDef->setValue(readStrFromXMLAttr(iNode, "Value"));
        iNode = iNode.nextSiblingElement();

    }
}

/* GLDGridSettingXMLReader */

void GLDGridSettingXMLReader::read(const GString &fileName, GLDGridSetting *gridSetting)
{
    GXMLDocument iXMLDoc = loadXMLDocument(fileName);
    if (iXMLDoc.isNull())
        return;
    read(iXMLDoc, gridSetting);
}

void GLDGridSettingXMLReader::read(GXMLDocument &xmlDoc, GLDGridSetting *gridSetting)
{
    GXMLNode root = xmlDoc.documentElement();
    if (root.isNull())
        gldError(getGSPi18nStr(g_NeedXMLNode));
    if (sameText(root.nodeName(), GLatin1String("c:GridSettings")))
    {
        if (root.childNodes().count() == 0)
            gldError(getGSPi18nStr(g_InvalidXMLNode));
        else
            read(firstChildNode(root), gridSetting);
    }
    else
        read(root, gridSetting);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取Grid属性设置对象
  参数：node -- XML节点
        gridSetting -- Grid属性设置对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::read(const GXMLNode &node, GLDGridSetting *gridSetting)
{
    if (node.isNull())
        gldError(getGSPi18nStr(g_NeedXMLNode));

    GXMLNode oNode = node;
    // 兼容之前的版本
    if (sameText(oNode.nodeName(), GLatin1String("c:GridSettings")))
    {
        if (!oNode.hasChildNodes())
            return;
        oNode = oNode.firstChildElement();
        if (oNode.isNull())
            gldError(getGSPi18nStr(g_NeedXMLNode));
    }
    if (!sameText(oNode.nodeName(), GLatin1String("o:GridSetting")))
        gldError(getGSPi18nStr(g_InvalidXMLNode));

    // 版本信息TGridSettings已经读取
    g_OldVersion = oNode.attribute("Version");
    gridSetting->setDesignState(true);
    gridSetting->setDescription(readStrFromXMLAttr(oNode, "Description"));
    gridSetting->setIdentity(readStrFromXMLAttr(oNode, "Identity"));
    gridSetting->setRemark(readStrFromXMLAttr(oNode, "Remark"));

    // 版本转换用
    if (oNode.hasAttribute("Name"))
        gridSetting->setDescription(readStrFromXMLAttr(oNode,"Name"));

    // 先清ColSetting,否则会把FixedCols清掉
    gridSetting->colSettings()->clear();
    gridSetting->setFixedCols(readIntFromXML(oNode, "a:FixedCols"));
    gridSetting->setFixedEditRows(readIntFromXML(oNode, "a:FixedEditRows"));
    gridSetting->setFixedEditCols(readIntFromXML(oNode, "a:FixedEditCols"));
    /* todo yaok  if (evaluateVersion(GOldVersion) < evaluateVersion("2.1.2"))
      {
        gridSetting->setLeftMargin(readIntFromXML(node, "a:CellHorzMargin"));
        gridSetting->setRightMargin(readIntFromXML(node, "a:CellHorzMargin"));
        gridSetting->setTopMargin(readIntFromXML(node, "a:CellVertMargin"));
        gridSetting->setBottomMargin(readIntFromXML(node, "a:CellVertMargin"));
      }
    else */
    {
        gridSetting->setLeftMargin(readIntFromXML(oNode, "a:LeftMargin"));
        gridSetting->setRightMargin(readIntFromXML(oNode, "a:RightMargin"));
        gridSetting->setTopMargin(readIntFromXML(oNode, "a:TopMargin"));
        gridSetting->setBottomMargin(readIntFromXML(oNode, "a:BottomMargin"));
    }
    gridSetting->setInvalidValueLabel(readStrFromXML(oNode, "a:InvalidValueLabel"));
    gridSetting->setGridLineWidth(readIntFromXML(oNode, "a:GridLineWidth", 0));
    gridSetting->setGridLineColor(readIntFromXML(oNode, "a:GridLineColor", g_clSilver));
    gridSetting->setGridColor(readIntFromXML(oNode, "a:GridColor", g_clWindow));
    gridSetting->setBoundLineColor(readIntFromXML(oNode, "a:BoundLineColor", g_clBlack));
    gridSetting->setAllowSort(readBoolFromXML(oNode, "a:AllowSort"));
    gridSetting->setAllowCopyPaste(readBoolFromXML(oNode, "a:AllowCopyPaste", true));
    gridSetting->setSortFieldNames(readStrFromXML(oNode, "a:SortFieldNames"));
    gridSetting->setAscImageIndex(readIntFromXML(oNode, "a:AscImageIndex", -1));
    gridSetting->setDescImageIndex(readIntFromXML(oNode, "a:DescImageIndex", -1));
    GXMLNode iNode = findChildNode(oNode, "a:Options");
    //todo yaok
    //    if (iNode != NULL)
    //        readOldVertionOptions(iNode);
    //    else
    {
        gridSetting->setShowAsTree(readBoolFromXML(oNode, "a:ShowAsTree"));
        gridSetting->setEditing(readBoolFromXML(oNode, "a:Editing", true));
        gridSetting->setVertLine(readBoolFromXML(oNode, "a:VertLine", true));
        gridSetting->setHorzLine(readBoolFromXML(oNode, "a:HorzLine", true));
        gridSetting->setRowSizing(readBoolFromXML(oNode, "a:RowSizing"));
        gridSetting->setColSizing(readBoolFromXML(oNode, "a:ColSizing", true));
        gridSetting->setColMoving(readBoolFromXML(oNode, "a:ColMoving", true));
        gridSetting->setAlwaysShowEditor(readBoolFromXML(oNode, "a:AlwaysShowEditor"));
        gridSetting->setRangeSelect(readBoolFromXML(oNode, "a:RangeSelect", true));
        gridSetting->setAllowSelectRow(readBoolFromXML(oNode, "a:AllowSelectRow", true));
        gridSetting->setAllowSelectCol(readBoolFromXML(oNode, "a:AllowSelectCol", true));
        gridSetting->setAllowSelectAll(readBoolFromXML(oNode, "a:AllowSelectAll", true));
        gridSetting->setMultiSelection(readBoolFromXML(oNode, "a:MultiSelection", true));
        gridSetting->setAutoThemeAdapt(readBoolFromXML(oNode, "a:AutoThemeAdapt", true));
        gridSetting->setReturnKeyAsTab(readBoolFromXML(oNode, "a:ReturnKeyAsTab"));
        gridSetting->setShowFixedRow(readBoolFromXML(oNode, "a:ShowFixedRow", true));
        gridSetting->setCellFilling(readBoolFromXML(oNode, "a:CellFilling"));
        gridSetting->setFixedCellEvent(readBoolFromXML(oNode, "a:FixedCellEvent"));
        gridSetting->setAllow3DStyle(readBoolFromXML(oNode, "a:Allow3DStyle", true));
        gridSetting->setHideEditOnExit(readBoolFromXML(oNode, "a:HideEditOnExit"));
        gridSetting->setBorderStyle(readBoolFromXML(oNode, "a:BorderStyle", true));
        gridSetting->setUseBlendColor(readBoolFromXML(oNode, "a:UseBlendColor", true));
        gridSetting->setCellFillEditField(readBoolFromXML(oNode, "a:CellFillEditField"));
        gridSetting->setTotalRowAtFooter(readBoolFromXML(oNode, "a:TotalRowAtFooter", true));
    }
    iNode = findChildNode(oNode,"c:ExtPropDefs");
    if (!iNode.isNull())
        readExtPropDefs(iNode, gridSetting->extPropDefs());
    readColSettings(findChildNode(oNode, "c:ColSettings"), gridSetting->colSettings());
    readTitleRows(findChildNode(oNode, "c:TitleRows"), gridSetting->titleRows());
    readFilterRows(findChildNode(oNode, "c:FilterRows"), gridSetting->filterRows());
    readTableSettings(findChildNode(oNode, "c:TableSettings"), gridSetting->tableSettings());
    //todo yaok
    //    int i;
    //    int nFixedColWidth;
    //  if (evaluateVersion(GOldVersion) <= evaluateVersion("1.0.3"))
    //      {
    //        nFixedColWidth = readIntFromXML(node, "a:FixedColWidth");
    //        if (nFixedColWidth == 0)
    //          nFixedColWidth = 20;
    //        /*with gridSetting.colSettings().insert(0) do*/
    //        {
    //          gridSetting->ColSettings->insert(0).setDisplayWidth(nFixedColWidth);
    //          gridSetting->ColSettings->insert(0).setVisible(true);
    //          gridSetting->ColSettings->insert(0).setCellSuitType(cstSuitColWidth);
    //        }
    //      for (i = 0; i != gridSetting->TableSettings->Count - 1; i++)
    //        gridSetting->TableSettings->Items[i]->FieldSettings->Items[0].setDisplayExpr(CDefFixedColDisplayExpr);
    //    }

}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取列设置容器对象
  参数：node -- XML节点
        colSettings -- 列设置容器对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readColSettings(const GXMLNode &node, GLDColSettings *colSettings)
{
    colSettings->clear();
    if (node.isNull())
        return;
    GXMLNode child= node.firstChildElement();
    while (!child.isNull())
    {
        if (sameText(child.nodeName(), GLatin1String("o:ColSetting")))
        {
            readColSetting(child, colSettings->insert(-1));
        }
        child = child.nextSiblingElement();

    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取列设置对象
  参数：node -- XML节点
        colSetting -- 列设置对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readColSetting(const GXMLNode &node, GLDColSetting *colSetting)
{
    //todo yaok
    //    bool bVisible;
    //  if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.3"))
    //    {
    //      colSetting->setDisplayWidth(readIntFromXML(node, "a:DisplayWidth"));
    //      colSetting->setVisibleCondition(readStrFromXML(node, "a:VisibleCondition"));
    //      if (evaluateVersion(GOldVersion) < evaluateVersion("1.0.11"))
    //        colSetting->setVisibleCondition(readStrFromXML(node, "a:Expr"));
    //      if (evaluateVersion(GOldVersion) < evaluateVersion("1.0.8"))
    //        {
    //          bVisible = GXMLDocument(node, "a:Visible");
    //          if (!bVisible)
    //            colSetting->setVisibleCondition("=False");
    //        }
    //      colSetting->CellSuitType = ((GLDCellSuitType) readIntFromXML(node, "a:AutoWrapType"));
    //      colSetting->setIdentity(readStrFromXML(node, "a:Identity"));
    //      if (evaluateVersion(GOldVersion) < evaluateVersion("1.0.19"))
    //        {
    //          if (GXMLDocument(node, "a:IsFixedCol"))
    //            colSetting->GridSetting->setFixedCols(colSetting->GridSetting->FixedCols + 1);
    //        }
    //      colSetting->setExtData(readStrFromXML(node, "a:ExtData"));
    //    }
    //  else
    {
        colSetting->setDisplayWidth(readIntFromXMLAttr(node, "DisplayWidth"));
        colSetting->setCellSuitType(StrToCellSuitType(readStrFromXMLAttr(node, "AutoWrapType", "cstFixed")));
        colSetting->setIdentity(readStrFromXMLAttr(node, "Identity"));
        colSetting->setExtData(readStrFromXMLAttr(node, "ExtData"));
        colSetting->setVisibleCondition(readStrFromXMLAttr(node, "VisibleCondition"));
        GXMLNode iNode = findChildNode(node, "c:ExtPropDefs");
        if (!iNode.isNull())
        {
            readExtPropDefs(iNode, colSetting->extPropDefs());
        }
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取标题行容器对象
  参数：node -- XML节点
        titleRows -- 标题行容器对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readTitleRows(const GXMLNode &node, GLDTitleRows *titleRows)
{
    titleRows->clear();
    if (!node.isNull())
    {
        GXMLNode child= node.firstChildElement();
        while (!child.isNull())
        {
            if (sameText(child.nodeName(), GLatin1String("o:TitleRow")))
            {
                readTitleRow(child, titleRows->insert(-1));
            }
            child = child.nextSiblingElement();
        }
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取标题行对象
  参数：node -- XML节点
        titleRow -- 标题行对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readTitleRow(const GXMLNode &node, GLDTitleRow *titleRow)
{
    if (!node.isNull())
    {
        titleRow->setRowHeight(readIntFromXML(node, "a:RowHeight"));
        GXMLNode iNode = findChildNode(node, "c:TitleCells");
        if (!iNode.isNull())
            readTitleCells(iNode, titleRow);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取标题单元格对象列表
  参数：node -- XML节点
        titleCell -- 标题单元格对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readTitleCells(const GXMLNode &node, GLDTitleRow *titleRow)
{
    int nIndex;
    //todo yaok
    //  if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.2"))
    //    {
    //      for (i = 0; i != node.childNodes().count() - 1; i++)
    //        if (sameText(node.childNodes(i).name(), "o:TitleCell"))
    //          readTitleCell(node.childNodes(i), titleRow->Cells[i]);
    //    }
    //  else
    //    {
    if (!node.hasChildNodes())
        return;
    GXMLNode child= node.firstChildElement();
    while (!child.isNull())
    {
        if (sameText(child.nodeName(), GLatin1String("o:TitleCell")))
        {
            nIndex = readIntFromXMLAttr(child, "Index");
            readTitleCell(child, titleRow ->cells(nIndex));
        }
        child = child.nextSiblingElement();
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取标题单元格对象
  参数：node -- XML节点
        titleCell -- 标题单元格对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readTitleCell(const GXMLNode &node, GLDTitleCell *titleCell)
{
    //todo yaok
    //  if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.3"))
    //    {
    //      titleCell->setCaption(readStrFromXML(node, "a:Caption"));
    //      titleCell->setMergeID(readIntFromXML(node, "a:MergeID"));
    //    }
    //  else
    {
        titleCell->setCaption(readStrFromXMLAttr(node, "Caption"));
        titleCell->setMergeID(readIntFromXMLAttr(node, "MergeID"));
    }
    GXMLNode iNode = findChildNode(node, "o:CellFormat");
    if (!iNode.isNull())
        readCellFormat(iNode, titleCell->cellFormat(), ghaCenter);
    else
        titleCell->cellFormat()->setHorzAlignment(ghaCenter);
}

void GLDGridSettingXMLReader::readFilterRows(const GXMLNode &node, GLDFilterRows *filterRows)
{
    filterRows->clear();
    if (!node.isNull())
    {
        GXMLNode child= node.firstChildElement();
        while (!child.isNull())
        {
            if (sameText(child.text(), GLatin1String("o:FilterRow")))
            {
                readFilterRow(child, filterRows->insert(-1));
            }
            child = child.nextSiblingElement();
        }
    }
}

void GLDGridSettingXMLReader::readFilterRow(const GXMLNode &node, GLDFilterRow *filterRow)
{
    if (!node.isNull())
    {
        filterRow->setRowHeight(readIntFromXML(node, "a:RowHeight"));
        GXMLNode iNode = findChildNode(node, "c:FilterCells");
        if (!iNode.isNull())
            readFilterCells(iNode, filterRow);
    }
}

void GLDGridSettingXMLReader::readFilterCells(const GXMLNode &node, GLDFilterRow *filterRow)
{
    int nIndex;
    GXMLNode child= node.firstChildElement();
    while (!child.isNull())
    {
        if (sameText(child.nodeName(), GLatin1String("o:FilterCell")))
        {
            nIndex = readIntFromXMLAttr(child, "Index");
            readFilterCell(child, filterRow ->cells(nIndex));
        }
        child = child.nextSiblingElement();
    }
}

void GLDGridSettingXMLReader::readFilterCell(const GXMLNode &node, GLDFilterCell *filterCell)
{
    filterCell->setCaption(readStrFromXMLAttr(node, "Caption"));
    filterCell->setEditorName(readStrFromXMLAttr(node, "EditorName"));
    filterCell->setMergeID(readIntFromXMLAttr(node, "MergeID"));
    GXMLNode iNode = findChildNode( node, "o:CellFormat");
    if (!iNode.isNull())
    {
        if (filterCell->isFixedCol())
            readCellFormat(iNode, filterCell->cellFormat(), ghaCenter);
        else
            readCellFormat(iNode, filterCell->cellFormat());
    }
    else
        if (filterCell->isFixedCol())
            filterCell->cellFormat()->setHorzAlignment(ghaCenter);

}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取表设置容器对象
  参数：node -- XML节点
        tableSettings -- 表设置容器对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readTableSettings(const GXMLNode &node, GLDTableSettings *tableSettings)
{
    tableSettings->clear();
    if (!node.isNull())
    {
        GXMLNode child = node.firstChildElement();
        while (!child.isNull())
        {
            if (sameText(child.nodeName(), GLatin1String("o:TableSetting")))
            {
                readTableSetting(child, tableSettings->insert(-1));
            }
            child = child.nextSiblingElement();
        }

    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取表设置对象
  参数：node -- XML节点
        tableSetting -- 表设置对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readTableSetting(const GXMLNode &node, GLDTableSetting *tableSetting)
{
    GXMLNode child;
    //  if (evaluateVersion(GOldVersion) < evaluateVersion("1.0.20"))
    //    {
    //      tableSetting->setCustomDataSource(false);
    //      tableSetting->setMasterViewName("");
    //      tableSetting->setSelfFieldName("");
    //      tableSetting->setMasterFieldName("");
    //      tableSetting->setPrimaryKeyFieldName("");
    //      tableSetting->setParentKeyFieldName("");
    //    }
    //  else
    {
        tableSetting->setCustomDataSource(readBoolFromXML(node, "a:CustomDataSource"));
        tableSetting->setMasterViewName(readStrFromXML(node, "a:MasterViewName"));
        tableSetting->setSelfFieldName(readStrFromXML(node, "a:SelfFieldName"));
        tableSetting->setMasterFieldName(readStrFromXML(node, "a:MasterFieldName"));
        tableSetting->setPrimaryKeyFieldName(readStrFromXML(node, "a:PrimaryKeyFieldName"));
        tableSetting->setParentKeyFieldName(readStrFromXML(node, "a:ParentKeyFieldName"));
    }
    tableSetting->setIsTree(readBoolFromXML(node, "a:IsTree"));
    tableSetting->setIsTreeWithMasterView(readBoolFromXML(node, "a:IsTreeWithMasterView"));
    tableSetting->setTotalStyle((GLDTotalStyle) readIntFromXML(node, "a:TotalStyle"));
    tableSetting->setRefTableName(readStrFromXML(node, "a:RefTableName"));
    tableSetting->setRefDatabaseName(readStrFromXML(node, "a:RefDatabaseName"));
    tableSetting->setDefRowHeight(readIntFromXML(node, "a:DefRowHeight"));
    g_DefHeaderRowHeight = readIntFromXML(node, "a:DefHeaderRowHeight");
    if (g_DefHeaderRowHeight == 0)
        g_DefHeaderRowHeight = CDefRowHeight;
    g_DefTotalRowHeight = readIntFromXML(node, "a:DefTotalRowHeight");
    if (g_DefTotalRowHeight == 0)
        g_DefTotalRowHeight = CDefRowHeight;
    GXMLNode iNode = findChildNode(node, "c:FieldSettings");
    if (!iNode.isNull())
        readFieldSettings(iNode, tableSetting->fieldSettings());
    iNode = findChildNode( node,"c:Rules");
    if (!iNode.isNull())
        readGridRules(iNode, tableSetting->rules());
    iNode = findChildNode(node, "c:HeaderRows");
    if (!iNode.isNull())
    {
        //      if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.2"))
        //        {
        //          for (i = 0; i != iNode.childNodes().count() - 1; i++)
        //            {
        //              if (!sameText(iNode.childNodes(i).name(), "c:HeaderRowFieldSettings"))
        //                Continue;
        //              readHeaderRowFieldSettings(iNode.childNodes(i), tableSetting->insertHeaderRow(- 1));
        //            }
        //        }
        //      else
        {
            child = iNode.firstChildElement();
            while (!child.isNull())
            {
                if (sameText(child.nodeName(), GLatin1String("o:HeaderRow")))
                {
                    readHeaderRowFieldSettings(child, tableSetting->insertHeaderRow(-1));
                }
                child = child.nextSiblingElement();
            }
        }
    }
    iNode = findChildNode(node, "c:TotalRows");
    if (!iNode.isNull())
    {
        //      if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.2"))
        //        {
        //          for (i = 0; i != iNode.childNodes().count() - 1; i++)
        //            {
        //              if (!sameText(iNode.childNodes(i).name(), "c:TotalRowFieldSettings"))
        //                Continue;
        //              readTotalRowFieldSettings(iNode.childNodes(i), tableSetting->insertTotalRow(- 1));
        //            }
        //        }
        //      else
        {

            child = iNode.firstChildElement();
            while (!child.isNull())
            {
                if (sameText(child.nodeName(), GLatin1String("o:TotalRow")))
                {
                    readTotalRowFieldSettings(child, tableSetting->insertTotalRow(-1));
                }
                child = child.nextSiblingElement();
            }
        }
    }
    iNode = findChildNode(node, "c:ExtPropDefs");
    if (!iNode.isNull())
        readExtPropDefs(iNode, tableSetting->extPropDefs());

}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取字段设置容器对象
  参数：node -- XML节点
        fieldSetting -- 字段设置容器对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readFieldSettings(const GXMLNode &node, GLDFieldSettings *fieldSettings)
{
    int nIndex;
    if (!node.isNull())
    {
        //      if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.2"))
        //        {
        //          for (i = 0; i != node.childNodes().count() - 1; i++)
        //            if (sameText(node.childNodes(i).name(), "o:FieldSetting"))
        //              readFieldSetting(node.childNodes(i), fieldSettings->Items[i]);
        //        }
        //      else
        {
            if (!node.hasChildNodes())
                return;
            GXMLNode child = node.firstChildElement();
            while (!child.isNull())
            {
                if (sameText(child.nodeName(), GLatin1String("o:FieldSetting")))
                {
                    nIndex = readIntFromXMLAttr(child, "Index");
                    readFieldSetting(child, fieldSettings->items(nIndex));
                }
                child = child.nextSiblingElement();
            }
        }
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取字段设置对象
  参数：node -- XML节点
        fieldSetting -- 字段设置对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readFieldSetting(const GXMLNode &node, GLDFieldSetting *fieldSetting)
{
    //  if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.3"))
    //    {
    //      fieldSetting->setMergeID(readIntFromXML(node, "a:MergeID"));
    //      fieldSetting->setEditFieldName(readStrFromXML(node, "a:EditFieldName"));
    //      fieldSetting->setDisplayExpr(readStrFromXML(node, "a:DisplayExpr"));
    //      fieldSetting->setSaveFieldName(readStrFromXML(node, "a:SaveFieldName"));
    //      fieldSetting->setSaveRule(readStrFromXML(node, "a:SaveRule"));
    //      fieldSetting->setNullValDisplayStr(readStrFromXML(node, "a:NullValDisplayStr"));
    //      fieldSetting->setExtData(readStrFromXML(node, "a:ExtData"));
    //      fieldSetting->setEditTextIsDisplayText(readBoolFromXML(node, "a:EditTextIsDisplayText", true));
    //      fieldSetting->setCellIdentity(readStrFromXML(node, "a:CellIdentity"));
    //    }
    //  else
    {
        fieldSetting->setMergeID(readIntFromXMLAttr(node, "MergeID"));
        fieldSetting->setEditFieldName(readStrFromXMLAttr(node, "EditFieldName"));
        fieldSetting->setDisplayExpr(readStrFromXMLAttr(node, "DisplayExpr"));
        fieldSetting->setSaveFieldName(readStrFromXMLAttr(node, "SaveFieldName"));
        fieldSetting->setSaveRule(readStrFromXMLAttr(node, "SaveRule"));
        fieldSetting->setNullValDisplayStr(readStrFromXMLAttr(node, "NullValDisplayStr"));
        fieldSetting->setExtData(readStrFromXMLAttr(node, "ExtData"));
        fieldSetting->setEditTextIsDisplayText(readBoolFromXMLAttr(node, "EditTextIsDisplayText", true));
        fieldSetting->setEditorName(readStrFromXMLAttr(node, "EditorName"));
        fieldSetting->setCellIdentity(readStrFromXMLAttr(node, "CellIdentity"));
    }
    GXMLNode iNode = findChildNode( node, "o:CellFormat");
    if (!iNode.isNull())
    {
        if (fieldSetting->isFixedCol())
            readCellFormat(iNode, fieldSetting->cellFormat(), ghaCenter);
        else
            readCellFormat(iNode, fieldSetting->cellFormat());
    }
    else
        if (fieldSetting->isFixedCol())
            fieldSetting->cellFormat()->setHorzAlignment(ghaCenter);
    //  if (evaluateVersion(GOldVersion) < evaluateVersion("1.0.7"))
    //    {
    //      if (fieldSetting->DisplayExpr != "")
    //        fieldSetting->setDisplayExpr("=" + fieldSetting->DisplayExpr);
    //    }
    iNode = findChildNode( node, "c:ExtPropDefs");
    if (!iNode.isNull())
        readExtPropDefs(iNode, fieldSetting->extPropDefs());
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取显示规则容器对象
  参数：node -- XML节点
        gridRules -- 显示规则容器对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readGridRules(const GXMLNode &node, GLDGridRules *gridRules)
{
    GLDGridRuleType eRuleType;
    gridRules->clear();
    if (node.isNull())
        return;

    GXMLNode child;
    child = node.firstChildElement();
    while (!child.isNull())
    {

        if (sameText(child.nodeName(), GLatin1String("o:GridRule")))
        {
            eRuleType = StrToGridRuleType(readStrFromXMLAttr(child, "RuleType"));
            readGridRule(child, gridRules->insert(-1, eRuleType));
        }
        child = child.nextSiblingElement();
    }
}

void GLDGridSettingXMLReader::readHeaderRowFieldSettings(const GXMLNode &node,
                                                         GLDHeaderRowFieldSettings *headerRowFieldSettings)
{
    if (!node.isNull())
    {
        headerRowFieldSettings->setRowHeight(readIntFromXML(node, "a:RowHeight"));
        //      if (evaluateVersion(GOldVersion) < evaluateVersion("1.0.18"))
        //          headerRowFieldSettings->setRowHeight(GDefHeaderRowHeight);
        //N = 0;
        //      if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.2"))
        //        {
        //          for (i = 0; i != node.childNodes().count() - 1; i++)
        //            if (sameText(node.childNodes(i).name(), "o:HeaderRowFieldSetting"))
        //              {
        //                readHeaderRowFieldSetting(node.childNodes(i), headerRowFieldSettings->Items[N]);
        //                N ++;
        //              }
        //        }
        //      else
        {
            GXMLNode iNode = findChildNode( node, "c:FieldSettings");
            if (!iNode.isNull())
            {
                GXMLNode child = iNode.firstChildElement();
                while (!child.isNull())
                {
                    if (sameText(child.nodeName(), GLatin1String("o:FieldSetting")))
                    {
                        int nIndex = readIntFromXMLAttr(child, "Index");
                        readHeaderRowFieldSetting(child, headerRowFieldSettings->items(nIndex));
                    }
                    child = child.nextSiblingElement();
                }
            }
        }
    }
}

void GLDGridSettingXMLReader::readHeaderRowFieldSetting(const GXMLNode &node,
                                                        GLDHeaderRowFieldSetting *headerRowFieldSetting)
{
    //  if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.3"))
    //    {
    //      headerRowFieldSetting->setMergeID(readIntFromXML(node, "a:MergeID"));
    //      headerRowFieldSetting->setValue(readStrFromXML(node, "a:Value"));
    //    }
    //  else
    {
        headerRowFieldSetting->setMergeID(readIntFromXMLAttr(node, "MergeID"));
        headerRowFieldSetting->setValue(readStrFromXMLAttr(node, "Value"));
    }
    GXMLNode iNode = findChildNode(node, "o:CellFormat");
    if (!iNode.isNull())
        readCellFormat(iNode, headerRowFieldSetting->cellFormat(), ghaCenter);
    else
    {
        if (headerRowFieldSetting->isFixedCol())
        {
            headerRowFieldSetting->cellFormat()->setHorzAlignment(ghaCenter);
        }
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-16
  功能: 从XML文档中读取合计行设置容器
  参数: node: GXMLNode &-- XML节点
        totalRowFieldSettings -- 合计行设置容器
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readTotalRowFieldSettings(const GXMLNode &node,
                                                        GLDTotalRowFieldSettings *totalRowFieldSettings)
{
    int nIndex;
    GXMLNode child;
    if (!node.isNull())
    {
        totalRowFieldSettings->setRowHeight(readIntFromXML(node, "a:RowHeight"));
        //      if (evaluateVersion(GOldVersion) < evaluateVersion("1.0.18"))
        //        totalRowFieldSettings->setRowHeight(GDefTotalRowHeight);
        //      N = 0;
        //      if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.2"))
        //        {
        //          for (i = 0; i != node.childNodes().count() - 1; i++)
        //            if (sameText(node.childNodes(i).name(), "o:TotalRowFieldSetting"))
        //              {
        //                readTotalRowFieldSetting(node.childNodes(i), totalRowFieldSettings->Items[N]);
        //                N ++;
        //              }
        //        }
        //      else
        {
            GXMLNode iNode = findChildNode(node, "c:FieldSettings");
            if (!iNode.isNull())
            {
                child = iNode.firstChildElement();
                while (!child.isNull())
                {
                    if (sameText(child.nodeName(), GLatin1String("o:FieldSetting")))
                    {
                        nIndex = readIntFromXMLAttr(child, "Index");
                        readTotalRowFieldSetting(child, totalRowFieldSettings->items(nIndex));
                    }
                    child = child.nextSiblingElement();
                }
            }
        }
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-16
  功能: 从XML文档中读取合计行设置对象
  参数: node: GXMLNode &-- XML节点
        totalRowFieldSetting -- 合计行设置对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLReader::readTotalRowFieldSetting(const GXMLNode &node,
                                                       GLDTotalRowFieldSetting *totalRowFieldSetting)
{
    //  if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.3"))
    //  {
    //      totalRowFieldSetting->setMergeID(readIntFromXML(node, "a:MergeID"));
    //      totalRowFieldSetting->setValue(readStrFromXML(node, "a:Value"));
    //  }
    //  else
    {
        totalRowFieldSetting->setMergeID(readIntFromXMLAttr(node, "MergeID"));
        totalRowFieldSetting->setValue(readStrFromXMLAttr(node, "Value"));
    }
    GXMLNode iNode = findChildNode(node, "o:CellFormat");
    if (!iNode.isNull())
    {
        if (totalRowFieldSetting->isFixedCol())
            readCellFormat(iNode, totalRowFieldSetting->cellFormat(), ghaCenter);
        else
            readCellFormat(iNode, totalRowFieldSetting->cellFormat());
    }
    else
        if (totalRowFieldSetting->isFixedCol())
            totalRowFieldSetting->cellFormat()->setHorzAlignment(ghaCenter);
}

void GLDRecordGridSettingXMLReader::read(const GString &fileName, GLDRecordGridSetting *gridSetting)
{
    GXMLDocument iXMLDoc = loadXMLDocument(fileName);
    if (iXMLDoc.isNull())
        return;
    read(iXMLDoc, gridSetting);
}

void GLDRecordGridSettingXMLReader::read(const GXMLDocument &xmlDoc, GLDRecordGridSetting *gridSetting)
{
    GXMLNode root = xmlDoc.documentElement();
    if (root.isNull())
        gldError(getGSPi18nStr(g_NeedXMLNode));
    if (sameText(root.nodeName(), GLatin1String("c:RecordGridSettings")))
    {
        if (root.childNodes().count() == 0)
            gldError(getGSPi18nStr(g_InvalidXMLNode));
        else
            read(root.firstChildElement(), gridSetting);
    }
    else
        read(root, gridSetting);
}

void GLDRecordGridSettingXMLReader::read(const GXMLNode &node, GLDRecordGridSetting *gridSetting)
{
    if (!sameText(node.nodeName(), GLatin1String("o:RecordGridSetting")))
        gldError(getGSPi18nStr(g_InvalidXMLNode));
    gridSetting->setDesignState(true);
    if (node.hasAttribute("Version"))
        g_OldVersion = readStrFromXMLAttr(node, "Version");
    // 版本转换用
    if (node.hasAttribute("Name"))
        gridSetting->setDescription(readStrFromXMLAttr(node, "Name"));
    if (node.hasAttribute("Description"))
        gridSetting->setDescription(readStrFromXMLAttr(node, "Description"));
    if (node.hasAttribute("Identity"))
        gridSetting->setIdentity(readStrFromXMLAttr(node, "Identity"));
    if (node.hasAttribute("Remark"))
        gridSetting->setRemark(readStrFromXMLAttr(node, "Remark"));
    gridSetting->setRowCount(0);
    gridSetting->setColCount(0);
    gridSetting->fieldSettings()->clear();
    gridSetting->setInvalidValueLabel(readStrFromXML(node, "a:InvalidValueLabel"));
    gridSetting->setRowCount(readIntFromXML(node, "a:RowCount"));
    gridSetting->setColCount(readIntFromXML(node, "a:ColCount"));
    gridSetting->setFixedRows(readIntFromXML(node, "a:FixedRows"));
    gridSetting->setFixedCols(readIntFromXML(node, "a:FixedCols"));
    gridSetting->setFixedEditRows(readIntFromXML(node, "a:FixedEditRows"));
    gridSetting->setFixedEditCols(readIntFromXML(node, "a:FixedEditCols"));
    //    if (evaluateVersion(GOldVersion) < evaluateVersion("2.1.2"))
    //    {
    //        gridSetting->setLeftMargin(readIntFromXML(node, "a:CellHorzMargin"));
    //        gridSetting->setRightMargin(readIntFromXML(node, "a:CellHorzMargin"));
    //        gridSetting->setTopMargin(readIntFromXML(node, "a:CellVertMargin"));
    //        gridSetting->setBottomMargin(readIntFromXML(node, "a:CellVertMargin"));
    //    }
    //    else
    {
        gridSetting->setLeftMargin(readIntFromXML(node, "a:LeftMargin"));
        gridSetting->setRightMargin(readIntFromXML(node, "a:RightMargin"));
        gridSetting->setTopMargin(readIntFromXML(node, "a:TopMargin"));
        gridSetting->setBottomMargin(readIntFromXML(node, "a:BottomMargin"));
    }
    gridSetting->setIgnoreInvalidCell(readBoolFromXML(node, "a:IgnoreInvalidCell"));
    gridSetting->setGridLineWidth(readIntFromXML(node, "a:GridLineWidth", 1));
    gridSetting->setGridLineColor(readIntFromXML(node, "a:GridLineColor", g_clSilver));
    gridSetting->setGridColor(readIntFromXML(node, "a:GridColor", g_clWindow));
    gridSetting->setBoundLineColor(readIntFromXML(node, "a:BoundLineColor", g_clBlack));
    gridSetting->setAllowLabelSelection(readBoolFromXML(node, "a:AllowLabelSelection"));
    gridSetting->setAllowCopyPaste(readBoolFromXML(node, "a:AllowCopyPaste", true));
    gridSetting->loadRowHeightsFromStr(readStrFromXML(node, "a:RowHeights"));
    gridSetting->loadColWidthsFromStr(readStrFromXML(node, "a:ColWidths"));
    gridSetting->loadMinColWidthsFromStr(readStrFromXML(node, "a:MinColWidths"));
    //    if (evaluateVersion(GOldVersion) >= evaluateVersion("1.0.9"))
    gridSetting->loadColStylesFromStr(readStrFromXML(node, "a:ColStyles"));
    //    if (evaluateVersion(GOldVersion) >= evaluateVersion("1.0.10"))
    gridSetting->loadColIdentitysFromStr(readStrFromXML(node, "a:ColIdentitys"));
    //    int i;
    //    if (evaluateVersion(GOldVersion) < evaluateVersion("1.0.14"))
    //    {
    //        for (i = 0; i != gridSetting->ColCount - 1; i++)
    //            if (gridSetting->ColStyles[i] == cstSuitRowHeight)
    //                gridSetting->setColStyles(i, cstSuitColWidth);
    //    }
    //    if (evaluateVersion(GOldVersion) < evaluateVersion("2.1.1"))
    //    {
    //        for (i = 0; i != gridSetting->ColCount - 1; i++)
    //            if (gridSetting->ColStyles[i] == cstSuitColWidth)
    //                gridSetting->setColStyles(i, cstFitColWidth);
    //    }
    //    iNode = node. node.firstChildElement("a:Options");
    //    if (!iNode.isNull())
    //        readOldVertionOptions(iNode);
    //    else
    {
        gridSetting->setShowAsTree(readBoolFromXML(node, "a:ShowAsTree"));
        gridSetting->setEditing(readBoolFromXML(node, "a:Editing", true));
        gridSetting->setVertLine(readBoolFromXML(node, "a:VertLine", true));
        gridSetting->setHorzLine(readBoolFromXML(node, "a:HorzLine", true));
        gridSetting->setRowSizing(readBoolFromXML(node, "a:RowSizing"));
        gridSetting->setColSizing(readBoolFromXML(node, "a:ColSizing", true));
        gridSetting->setColMoving(readBoolFromXML(node, "a:ColMoving", true));
        gridSetting->setAlwaysShowEditor(readBoolFromXML(node, "a:AlwaysShowEditor"));
        gridSetting->setRangeSelect(readBoolFromXML(node, "a:RangeSelect", true));
        gridSetting->setAllowSelectRow(readBoolFromXML(node, "a:AllowSelectRow", true));
        gridSetting->setAllowSelectCol(readBoolFromXML(node, "a:AllowSelectCol", true));
        gridSetting->setAllowSelectAll(readBoolFromXML(node, "a:AllowSelectAll", true));
        gridSetting->setMultiSelection(readBoolFromXML(node, "a:MultiSelection", true));
        gridSetting->setAutoThemeAdapt(readBoolFromXML(node, "a:AutoThemeAdapt", true));
        gridSetting->setReturnKeyAsTab(readBoolFromXML(node, "a:ReturnKeyAsTab"));
        gridSetting->setShowFixedRow(readBoolFromXML(node, "a:ShowFixedRow", true));
        gridSetting->setCellFilling(readBoolFromXML(node, "a:CellFilling"));
        gridSetting->setFixedCellEvent(readBoolFromXML(node, "a:FixedCellEvent"));
        gridSetting->setAllow3DStyle(readBoolFromXML(node, "a:Allow3DStyle", true));
        gridSetting->setHideEditOnExit(readBoolFromXML(node, "a:HideEditOnExit"));
        gridSetting->setBorderStyle(readBoolFromXML(node, "a:BorderStyle", true));
        gridSetting->setUseBlendColor(readBoolFromXML(node, "a:UseBlendColor", true));
        gridSetting->setCellFillEditField(readBoolFromXML(node, "a:CellFillEditField"));
    }
    GXMLNode iNode = node.firstChildElement("c:ExtPropDefs"); ;//node.firstChildElement("c:ExtPropDefs");
    if (!iNode.isNull())
        readExtPropDefs(iNode, gridSetting->extPropDefs());
    iNode = node.firstChildElement("c:RecordSettings");
    if (!iNode.isNull())
        readRecordSettings(iNode, gridSetting->recordSettings());
    iNode = node.firstChildElement("c:FieldSettings");
    if (!iNode.isNull())
        readFieldSettings(iNode, gridSetting->fieldSettings());
    //    if (evaluateVersion(GOldVersion) < evaluateVersion("1.0.16"))
    //    {
    //        gridSetting->RecordSettings->clear();
    //        /*with gridSetting.recordSettings().insert(-1) do*/
    //        {
    //            gridSetting->RecordSettings->insert(- 1).setRefDatabaseName(
    //                  readStrFromXML(node, "a:RefDatabaseName"));
    //            gridSetting->RecordSettings->insert(- 1).setRefTableName(readStrFromXML(node, "a:RefTableName"));
    //            iNode = node.firstChildElement("c:Rules");
    //            if (!iNode.isNull())
    //                readGridRules(iNode, gridSetting->RecordSettings->insert(- 1).rules());
    //        }
    //    }
}

void GLDRecordGridSettingXMLReader::readRecordSettings(const GXMLNode &node, GLDRecordSettings *recordSettings)
{
    recordSettings->clear();
    GXMLNode child = node.firstChildElement("o:RecordSetting");
    while (!child.isNull())
    {
        readRecordSetting(child, recordSettings->insert(-1));
        child = child.nextSiblingElement("o:RecordSetting");
    }
}

void GLDRecordGridSettingXMLReader::readRecordSetting(const GXMLNode &node, GLDRecordSetting *recordSetting)
{
    recordSetting->setRefDatabaseName(readStrFromXML(node, "a:RefDatabaseName"));
    recordSetting->setRefTableName(readStrFromXML(node, "a:RefTableName"));
    GXMLNode iNode = node.firstChildElement("c:Rules");
    if (!iNode.isNull())
        readGridRules(iNode, recordSetting->rules());
}

void GLDRecordGridSettingXMLReader::readGridRules(const GXMLNode &node, GLDRecordGridRules *gridRules)
{
    GLDGridRuleType eRuleType;
    gridRules->clear();
    if (node.isNull())
        return;

    GXMLNode iNode = node.firstChildElement("o:GridRule");
    while (!iNode.isNull())
    {
        eRuleType = StrToGridRuleType(readStrFromXMLAttr(iNode, "RuleType"));
        readGridRule(iNode, gridRules->insert(-1, eRuleType));
        iNode = iNode.nextSiblingElement("o:GridRule");
    }

}

void GLDRecordGridSettingXMLReader::readFieldSettings(const GXMLNode &node, GLDRecordFieldSettings *fieldSettings)
{
    int nIndex;
    //  if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.2"))
    //  {
    //      for (i = 0; i != node.childNodes().count() - 1; i++)
    //          if (sameText(node.childNodes(i).name(), "o:FieldSetting"))
    //              readFieldSetting(node.childNodes(i), fieldSettings->Items[i]);
    //  }
    //  else
    {
        GXMLNode child = node.firstChildElement("o:FieldSetting");
        while (!child.isNull())
        {
            nIndex = readIntFromXMLAttr(child, "Index");
            readFieldSetting(child, fieldSettings->items(nIndex));
            child = child.nextSiblingElement("o:FieldSetting");
        }
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：从XML文档中读取显示规则对象
  参数：node -- XML节点
        rule -- 显示规则对象
-----------------------------------------------------------------------------*/
void GLDRecordGridSettingXMLReader::readFieldSetting(const GXMLNode &node, GLDRecordFieldSetting *fieldSetting)
{
    //  void result;

    //  if (evaluateVersion(GOldVersion) < evaluateVersion("2.2.3"))
    //    {
    //      fieldSetting->setDataSourceID(readIntFromXML(node, "a:DataSourceID"));
    //      fieldSetting->setMergeID(readIntFromXML(node, "a:MergeID"));
    //      fieldSetting->setEditFieldName(readStrFromXML(node, "a:EditFieldName"));
    //      fieldSetting->setDisplayExpr(readStrFromXML(node, "a:DisplayExpr"));
    //      fieldSetting->setSaveFieldName(readStrFromXML(node, "a:SaveFieldName"));
    //      fieldSetting->setSaveRule(readStrFromXML(node, "a:SaveRule"));
    //      fieldSetting->setNullValDisplayStr(readStrFromXML(node, "a:NullValDisplayStr"));
    //      fieldSetting->setExtData(readStrFromXML(node, "a:ExtData"));
    //      fieldSetting->setEditTextIsDisplayText(readBoolFromXML(node, "a:EditTextIsDisplayText", true));
    //      fieldSetting->setIsPasswordChar(readBoolFromXML(node, "a:IsPasswordChar"));
    //      fieldSetting->setCellIdentity(readStrFromXML(node, "a:CellIdentity"));
    //      fieldSetting->setVisibleCondition(readStrFromXML(node, "a:VisibleCondition"));
    //    }
    //  else
    {
        fieldSetting->setDataSourceID(readIntFromXMLAttr(node, "DataSourceID"));
        fieldSetting->setMergeID(readIntFromXMLAttr(node, "MergeID"));
        fieldSetting->setEditFieldName(readStrFromXMLAttr(node, "EditFieldName"));
        fieldSetting->setDisplayExpr(readStrFromXMLAttr(node, "DisplayExpr"));
        fieldSetting->setSaveFieldName(readStrFromXMLAttr(node, "SaveFieldName"));
        fieldSetting->setSaveRule(readStrFromXMLAttr(node, "SaveRule"));
        fieldSetting->setNullValDisplayStr(readStrFromXMLAttr(node, "NullValDisplayStr"));
        fieldSetting->setExtData(readStrFromXMLAttr(node, "ExtData"));
        fieldSetting->setEditTextIsDisplayText(readBoolFromXMLAttr(node, "EditTextIsDisplayText", true));
        fieldSetting->setIsPasswordChar(readBoolFromXMLAttr(node, "IsPasswordChar"));
        fieldSetting->setEditorName(readStrFromXMLAttr(node, "EditorName"));
        fieldSetting->setCellIdentity(readStrFromXMLAttr(node, "CellIdentity"));
        fieldSetting->setVisibleCondition(readStrFromXMLAttr(node, "VisibleCondition"));
    }
    GXMLNode iNode = node.firstChildElement("o:CellFormat");
    if (!iNode.isNull())
        readCellFormat(iNode, fieldSetting->cellFormat());
    //  if (evaluateVersion(GOldVersion) < evaluateVersion("1.0.7"))
    //    {
    //      if (fieldSetting->DisplayExpr != "")
    //        fieldSetting->setDisplayExpr("=" + fieldSetting->DisplayExpr);
    //    }
    iNode = node.firstChildElement("c:ExtPropDefs");
    if (!iNode.isNull())
        readExtPropDefs(iNode, fieldSetting->extPropDefs());

}

//GLDGridSettingXMLBaseWriter

void GLDGridSettingXMLBaseWriter::writeCellFormat(GXMLNode &node, GLDCellFormat *cellFormat,
                                                  GLDHorzAlignment defHorzAlignment)
{
    //  assert(cellFormat != NULL);
    writeStrToXMLAttr(node, "FontName",cellFormat->fontName(), CDefFontName);
    writeIntToXMLAttr(node, "FontSize",cellFormat->fontSize(), DefFontSize);
    writeIntToXMLAttr(node, "ForeColor",cellFormat->foreColor(), DefForeColor);
    writeIntToXMLAttr(node, "BackColor",cellFormat->backColor(), DefBackColor);
    writeIntToXMLAttr(node, "FontStyle", (byte)cellFormat->fontStyle(), 0);
    writeIntToXMLAttr(node, "LeftLineWidth",cellFormat->leftLineWidth(), DefLeftLineWidth);
    writeIntToXMLAttr(node, "TopLineWidth",cellFormat->topLineWidth(), DefTopLineWidth);
    writeIntToXMLAttr(node, "RightLineWidth",cellFormat->rightLineWidth(), DefRightLineWidth);
    writeIntToXMLAttr(node, "BottomLineWidth",cellFormat->bottomLineWidth(), DefBottomLineWidth);
    writeIntToXMLAttr(node, "DiagonalWidth",cellFormat->diagonalWidth(), DefDiagonalWidth);
    writeIntToXMLAttr(node, "AntiDiagonalWidth",cellFormat->antiDiagonalWidth(), DefAntiDiagonalWidth);
    writeIntToXMLAttr(node, "HorzAlignment", (int)cellFormat->horzAlignment(), (int)defHorzAlignment);
    writeIntToXMLAttr(node, "VertAlignment", (int)cellFormat->vertAlignment(), (int)DefVertAlignment);
    writeStrToXMLAttr(node, "FormatStr",cellFormat->formatStr(), CDefFormatStr);
    writeBoolToXMLAttr(node, "NullIsZero",cellFormat->nullIsZero(), DefNullIsZero);
    writeIntToXMLAttr(node, "LeftMargin",cellFormat->leftMargin(), DefHorzMargin);
    writeIntToXMLAttr(node, "RightMargin",cellFormat->rightMargin(), DefHorzMargin);
    writeIntToXMLAttr(node, "TopMargin",cellFormat->topMargin(), DefVertMargin);
    writeIntToXMLAttr(node, "BottomMargin",cellFormat->bottomMargin(), DefVertMargin);
    writeIntToXMLAttr(node, "ImageIndex",cellFormat->imageIndex(), DefImageIndex);
    writeIntToXMLAttr(node, "ImageLayout", (int)cellFormat->imageLayout(), (int)CDefImageLayout);

}

void GLDGridSettingXMLBaseWriter::writeGridRule(GXMLNode &node, GLDGridRule *rule)
{
    writeStrToXMLAttr(node, "Name", rule->ruleName());
    writeBoolToXMLAttr(node, "Enable", rule->enable());
    writeStrToXML(node, "a:RecordCondition", rule->recordCondition());
    writeStrToXML(node, "a:FieldCondition", rule->fieldCondition());
    switch (rule->ruleType())
    {
    case grtClearCell:
        writeBoolToXML(node, "a:AllowClear", dynamic_cast<GLDClearCellRule *>(rule)->allowClear(), true);
        break;
    case grtFont:
    {
        writeStrToXML(node, "a:FontName", dynamic_cast<GLDFontRule *>(rule)->fontName());
        writeIntToXML(node, "a:FontSize", dynamic_cast<GLDFontRule *>(rule)->fontSize());
        writeIntToXML(node, "a:FontColor", dynamic_cast<GLDFontRule *>(rule)->fontColor());
        writeIntToXML(node, "a:FontStyles", byte(dynamic_cast<GLDFontRule *>(rule)->fontStyles()));
    }
        break;
    case grtBackColor:
        writeIntToXML(node, "a:BackColor", dynamic_cast<GLDBackColorRule *>(rule)->backColor());
        break;
    case grtEditStyle:
        writeIntToXML(node, "a:EditStyle", (int)(dynamic_cast<GLDEditStyleRule *>(rule)->editStyle()));
        break;
    case grtBoundLine:
    {
        writeIntToXML(node, "a:RightLine", dynamic_cast<GLDBoundLineRule *>(rule)->rightLine());
        writeIntToXML(node, "a:BottomLine", dynamic_cast<GLDBoundLineRule *>(rule)->bottomLine());
        writeIntToXML(node, "a:Diagonal", dynamic_cast<GLDBoundLineRule *>(rule)->diagonal());
        writeIntToXML(node, "a:AntiDiagonal", dynamic_cast<GLDBoundLineRule *>(rule)->antiDiagonal());
    }
        break;
    case grtAlignment:
    {
        writeIntToXML(node, "a:HorzAlignment", (int)(dynamic_cast<GLDAlignRule *>(rule)->horzAlignment()));
        writeIntToXML(node, "a:VertAlignment", (int)(dynamic_cast<GLDAlignRule *>(rule)->vertAlignment()));
    }
        break;
    case grtMargin:
    {
        writeIntToXML(node, "a:LeftMargin", dynamic_cast<GLDMarginRule *>(rule)->leftMargin());
        writeIntToXML(node, "a:RightMargin", dynamic_cast<GLDMarginRule *>(rule)->rightMargin());
        writeIntToXML(node, "a:TopMargin", dynamic_cast<GLDMarginRule *>(rule)->topMargin());
        writeIntToXML(node, "a:BottomMargin", dynamic_cast<GLDMarginRule *>(rule)->bottomMargin());
    }
        break;
    case grtReadonly:
        writeBoolToXML(node, "a:Readonly", dynamic_cast<GLDReadonlyRule *>(rule)->readonly());
        break;
    case grtCanFocus:
        writeBoolToXML(node, "a:CanFocus", dynamic_cast<GLDCanFocusRule *>(rule)->canFocus());
        break;
    case grtHandleSymbol:
        writeBoolToXML(node, "a:HandleSymbol", dynamic_cast<GLDHandleSymbolRule *>(rule)->handleSymbol());
        break;
    case grtImage:
    {
        writeIntToXML(node, "a:ImageIndex", dynamic_cast<GLDImageRule *>(rule)->imageIndex());
        writeIntToXML(node, "a:ImageLayout", (int)(dynamic_cast<GLDImageRule *>(rule)->imageLayout()));
    }
        break;
    case grtComment:
    {
        writeBoolToXML(node, "a:ShowComment", dynamic_cast<GLDCommentRule *>(rule)->showComment());
        writeStrToXML(node, "a:Comment", dynamic_cast<GLDCommentRule *>(rule)->comment());
    }
        break;
    case grtVisible:
        writeStrToXML(node, "a:InVisibleText", dynamic_cast<GLDVisibleRule *>(rule)->inVisibleText());
        break;
    case grtMerge:
        writeStrToXML(node, "a:MergeExpr", dynamic_cast<GLDMergeRule *>(rule)->mergeExpr());
        break;
    case grtRejectDelete:
        writeIntToXML(node, "a:IsTree", dynamic_cast<GLDRejectDeleteRule *>(rule)->isTree());
        break;
    case grtRejectInsert:
    {
        writeIntToXML(node, "a:IsTree", dynamic_cast<GLDRejectInsertRule *>(rule)->isTree());
        writeIntToXML(node, "a:IsBefore", dynamic_cast<GLDRejectInsertRule *>(rule)->isBefore());
        writeStrToXML(node, "a:TableNames", dynamic_cast<GLDRejectInsertRule *>(rule)->tableNames());
    }
        break;
    case grtRejectInsertChild:
    {
        writeIntToXML(node, "a:IsTree", dynamic_cast<GLDRejectInsertChildRule *>(rule)->isTree());
        writeIntToXML(node, "a:IsBefore", dynamic_cast<GLDRejectInsertChildRule *>(rule)->isBefore());
        writeStrToXML(node, "a:TableNames", dynamic_cast<GLDRejectInsertChildRule *>(rule)->tableNames());
        writeIntToXML(node, "a:Level", dynamic_cast<GLDRejectInsertChildRule *>(rule)->level());
    }
        break;
    case grtRejectMove:
    {
        writeIntToXML(node, "a:IsTree", dynamic_cast<GLDRejectMoveRule *>(rule)->isTree());
        writeIntToXML(node, "a:IsMoveUp", dynamic_cast<GLDRejectMoveRule *>(rule)->isMoveUp());
        writeIntToXML(node, "a:IncludeChild", dynamic_cast<GLDRejectMoveRule *>(rule)->includeChild());
    }
        break;
    case grtRejectLevel:
    {
        writeIntToXML(node, "a:IsTree", dynamic_cast<GLDRejectLevelRule *>(rule)->isTree());
        writeIntToXML(node, "a:IsLevelUp", dynamic_cast<GLDRejectLevelRule *>(rule)->isLevelUp());
        writeIntToXML(node, "a:IsFixPos", dynamic_cast<GLDRejectLevelRule *>(rule)->isFixPos());

    }
        break;
    case grtEditForm:
        writeBoolToXML(node, "a:Enable", dynamic_cast<GLDEditFormRule *>(rule)->enable());
        break;
    }
}

void GLDGridSettingXMLBaseWriter::writeExtPropDefs(GXMLNode &node, CGLDExtPropDefs *extPropDefs)
{
    GXMLNode iNode;
    for (int i = 0; i <= extPropDefs->count() - 1; i++)
    {
        iNode = addChild(node, "o:ExtPropDef");
        writeStrToXMLAttr(iNode, "Code", extPropDefs->itemObjs(i)->code());
        writeStrToXMLAttr(iNode, "Value", extPropDefs->itemObjs(i)->value());
    }
}

/*GLDGridSettingXMLWriter*/
void GLDGridSettingXMLWriter::write(const GString &fileName, GLDGridSetting *gridSetting)
{
    GFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    GXMLDocument iXMLDoc = createXMLDocument();
    write(iXMLDoc, gridSetting);
#ifdef GLD_XML
    iXMLDoc.saveToStream(&file);
#else
    GTextStream out(&file);
    iXMLDoc.save(out, 4);
#endif
    file.close();
}

void GLDGridSettingXMLWriter::write(GXMLDocument &xmlDoc, GLDGridSetting *gridSetting)
{
    if (xmlDoc.documentElement().isNull())
    {
        GXMLNode root = xmlDoc.createElement("o:GridSetting");
        xmlDoc.appendChild(root);
    }
    GXMLNode oNode = xmlDoc.documentElement();
    write(oNode, gridSetting);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存Grid属性设置对象为XML文档
  参数：node -- XML节点
        gridSetting -- Grid属性设置对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::write(GXMLNode &node, GLDGridSetting *gridSetting)
{
    if (node.isNull())
        return;

    writeStrToXMLAttr(node, "xmlns:a", "attribute");
    writeStrToXMLAttr(node, "xmlns:c", "collection");
    writeStrToXMLAttr(node, "xmlns:o", "object");
    writeStrToXMLAttr(node, "Version", gridSetting->version());

    writeStrToXMLAttr(node, "Description", gridSetting->description());
    writeStrToXMLAttr(node, "Identity", gridSetting->identity());
    writeStrToXMLAttr(node, "Remark",gridSetting->remark());
    writeIntToXML(node, "a:FixedCols",gridSetting->fixedCols());
    writeIntToXML(node, "a:FixedEditRows",gridSetting->fixedEditRows());
    writeIntToXML(node, "a:FixedEditCols",gridSetting->fixedEditCols());
    writeIntToXML(node, "a:LeftMargin",gridSetting->leftMargin());
    writeIntToXML(node, "a:RightMargin",gridSetting->rightMargin());
    writeIntToXML(node, "a:TopMargin",gridSetting->topMargin());
    writeIntToXML(node, "a:BottomMargin",gridSetting->bottomMargin());
    writeStrToXML(node, "a:InvalidValueLabel",gridSetting->invalidValueLabel());
    writeIntToXML(node, "a:GridLineWidth",gridSetting->gridLineWidth());
    writeIntToXML(node, "a:GridLineColor",gridSetting->gridLineColor());
    writeIntToXML(node, "a:GridColor",gridSetting->gridColor());
    writeIntToXML(node, "a:BoundLineColor",gridSetting->boundLineColor());
    writeBoolToXML(node, "a:AllowSort",gridSetting->allowSort());
    writeBoolToXML(node, "a:AllowCopyPaste",gridSetting->allowCopyPaste(), true);
    writeStrToXML(node, "a:SortFieldNames",gridSetting->sortFieldNames());
    writeIntToXML(node, "a:AscImageIndex",gridSetting->ascImageIndex(), -1);
    writeIntToXML(node, "a:DescImageIndex",gridSetting->descImageIndex(), -1);
    writeBoolToXML(node, "a:ShowAsTree",gridSetting->showAsTree());
    writeBoolToXML(node, "a:Editing",gridSetting->editing(), true);
    writeBoolToXML(node, "a:VertLine",gridSetting->vertLine(), true);
    writeBoolToXML(node, "a:HorzLine",gridSetting->horzLine(), true);
    writeBoolToXML(node, "a:RowSizing",gridSetting->rowSizing());
    writeBoolToXML(node, "a:ColSizing",gridSetting->colSizing(), true);
    writeBoolToXML(node, "a:ColMoving",gridSetting->colMoving(), true);
    writeBoolToXML(node, "a:AlwaysShowEditor",gridSetting->alwaysShowEditor());
    writeBoolToXML(node, "a:RangeSelect",gridSetting->rangeSelect(), true);
    writeBoolToXML(node, "a:AllowSelectRow",gridSetting->allowSelectRow(), true);
    writeBoolToXML(node, "a:AllowSelectCol",gridSetting->allowSelectCol(), true);
    writeBoolToXML(node, "a:AllowSelectAll",gridSetting->allowSelectAll(), true);
    writeBoolToXML(node, "a:MultiSelection",gridSetting->multiSelection(), true);
    writeBoolToXML(node, "a:AutoThemeAdapt",gridSetting->autoThemeAdapt(), true);
    writeBoolToXML(node, "a:ReturnKeyAsTab",gridSetting->returnKeyAsTab());
    writeBoolToXML(node, "a:ShowFixedRow",gridSetting->showFixedRow(), true);
    writeBoolToXML(node, "a:CellFilling",gridSetting->cellFilling());
    writeBoolToXML(node, "a:FixedCellEvent",gridSetting->fixedCellEvent());
    writeBoolToXML(node, "a:Allow3DStyle",gridSetting->allow3DStyle(), true);
    writeBoolToXML(node, "a:HideEditOnExit",gridSetting->hideEditOnExit());
    writeBoolToXML(node, "a:BorderStyle",gridSetting->borderStyle(), true);
    writeBoolToXML(node, "a:UseBlendColor",gridSetting->useBlendColor(), true);
    writeBoolToXML(node, "a:CellFillEditField",gridSetting->cellFillEditField());
    writeBoolToXML(node, "a:TotalRowAtFooter",gridSetting->totalRowAtFooter(), true);

    if (gridSetting->extPropDefs()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:ExtPropDefs");
        writeExtPropDefs(oNode, gridSetting->extPropDefs());
    }
    if (gridSetting->colSettings()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:ColSettings");
        writeColSettings(oNode, gridSetting->colSettings());
    }
    if (gridSetting->titleRows()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:TitleRows");
        writeTitleRows(oNode, gridSetting->titleRows());
    }
    if (gridSetting->filterRows()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:FilterRows");
        writeFilterRows(oNode, gridSetting->filterRows());
    }
    if (gridSetting->tableSettings()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:TableSettings");
        writeTableSettings(oNode, gridSetting->tableSettings());
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存列设置容器对象为XML文档
  参数：node -- XML节点
        colSettings -- 列设置容器对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeColSettings(GXMLNode &node, GLDColSettings *colSettings)
{
    for (int i = 0; i <= colSettings->count() - 1; i++)
    {
        GXMLNode oNode = addChild(node, "o:ColSetting");
        writeColSetting(oNode, colSettings->items(i));
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存列设置对象为XML文档
  参数：node -- XML节点
        colSetting -- 列设置对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeColSetting(GXMLNode &node, GLDColSetting *colSetting)
{
    writeIntToXMLAttr(node, "DisplayWidth", colSetting->displayWidth());
    writeStrToXMLAttr(node, "AutoWrapType", CellSuitTypeToStr(colSetting->cellSuitType()), "cstFixed");
    writeStrToXMLAttr(node, "Identity", colSetting->identity());
    writeStrToXMLAttr(node, "ExtData", colSetting->extData());
    writeStrToXMLAttr(node, "VisibleCondition", colSetting->visibleCondition());
    if (colSetting->extPropDefs()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:ExtPropDefs");
        writeExtPropDefs(oNode, colSetting->extPropDefs());
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存标题行容器对象为XML文档
  参数：node -- XML节点
        titleRows -- 标题行容器对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeTitleRows(GXMLNode &node, GLDTitleRows *titleRows)
{
    for (int i = 0; i <= titleRows->count() - 1; i++)
    {
        GXMLNode oNode = addChild(node, "o:TitleRow");
        writeTitleRow(oNode, titleRows->items(i));
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存标题行对象为XML文档
  参数：node -- XML节点
        titleRow -- 标题行对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeTitleRow(GXMLNode &node, GLDTitleRow *titleRow)
{
    writeIntToXML(node, "a:RowHeight", titleRow->rowHeight());
    GXMLNode iNode = addChild(node, "c:TitleCells");
    writeTitleCells(iNode, titleRow);
    if (!iNode.hasChildNodes())
        node.removeChild(iNode);
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存标题行单元格容器对象为XML文档
  参数：node -- XML节点
        titleRow -- 标题行对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeTitleCells(GXMLNode &node, GLDTitleRow *titleRow)
{
    for (int i = 0; i <= titleRow->cellCount() - 1; i++)
    {
        if (isNullTitleCell(titleRow->cells(i)))
            continue;
        GXMLNode oNode = addChild(node, "o:TitleCell");
        writeTitleCell(oNode, titleRow->cells(i));
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存标题单元格对象为XML文档
  参数：node -- XML节点
        titleCell -- 标题单元格对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeTitleCell(GXMLNode &node, GLDTitleCell *titleCell)
{
    writeIntToXMLAttr(node, "Index", titleCell->index(), -1);
    writeStrToXMLAttr(node, "Caption", titleCell->caption());
    writeIntToXMLAttr(node, "MergeID", titleCell->mergeID());
    if (!isDefaultCellFormat(titleCell->cellFormat(), ghaCenter))
    {
        GXMLNode oNode = addChild(node, "o:CellFormat");
        writeCellFormat(oNode, titleCell->cellFormat(), ghaCenter);
    }
}

void GLDGridSettingXMLWriter::writeFilterRows(GXMLNode &node, GLDFilterRows *filterRows)
{
    for (int i = 0; i <= filterRows->count() - 1; i++)
    {
        GXMLNode oNode = addChild(node, "o:FilterRow");
        writeFilterRow(oNode, filterRows->items(i));
    }
}

void GLDGridSettingXMLWriter::writeFilterRow(GXMLNode &node, GLDFilterRow *filterRow)
{
    writeIntToXML(node, "a:RowHeight", filterRow->rowHeight());
    GXMLNode iNode = addChild(node, "c:FilterCells");
    writeFilterCells(iNode, filterRow);
    if (iNode.hasChildNodes())
        node.removeChild(iNode);
}

void GLDGridSettingXMLWriter::writeFilterCells(GXMLNode &node, GLDFilterRow *filterRow)
{
    for (int i = 0; i <= filterRow->cellCount() - 1; i++)
    {
        if (isNullFilterCell(filterRow->cells(i)))
            continue;
        GXMLNode oNode = addChild(node, "o:FilterCell");
        writeFilterCell(oNode, filterRow->cells(i));
    }
}

void GLDGridSettingXMLWriter::writeFilterCell(GXMLNode &node, GLDFilterCell *filterCell)
{

    GLDHorzAlignment eDefHorzAlignment;
    writeIntToXMLAttr(node, "Index", filterCell->index(), -1);
    writeStrToXMLAttr(node, "Caption", filterCell->caption());
    writeStrToXMLAttr(node, "EditorName", filterCell->editorName());
    writeIntToXMLAttr(node, "MergeID", filterCell->mergeID());
    if (filterCell->isFixedCol())
        eDefHorzAlignment = ghaCenter;
    else
        eDefHorzAlignment = ghaAuto;
    if (!isDefaultCellFormat(filterCell->cellFormat(), eDefHorzAlignment))
    {
        GXMLNode oNode = addChild(node, "o:CellFormat");
        writeCellFormat(oNode, filterCell->cellFormat(), eDefHorzAlignment);
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存表设置容器对象为XML文档
  参数：node -- XML节点
        tableSettings -- 表设置容器对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeTableSettings(GXMLNode &node, GLDTableSettings *tableSettings)
{
    for (int i = 0; i <= tableSettings->count() - 1; i++)
    {
        GXMLNode oNode = addChild(node, "o:TableSetting");
        writeTableSetting(oNode, tableSettings->items(i));
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存表设置对象为XML文档
  参数：node -- XML节点
        tableSetting -- 表设置对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeTableSetting(GXMLNode &node, GLDTableSetting *tableSetting)
{
    writeBoolToXML(node, "a:CustomDataSource",tableSetting->customDataSource());
    writeStrToXML(node, "a:MasterViewName",tableSetting->masterViewName());
    writeStrToXML(node, "a:SelfFieldName",tableSetting->selfFieldName());
    writeStrToXML(node, "a:MasterFieldName",tableSetting->masterFieldName());
    writeStrToXML(node, "a:PrimaryKeyFieldName",tableSetting->primaryKeyFieldName());
    writeStrToXML(node, "a:ParentKeyFieldName",tableSetting->parentKeyFieldName());
    writeBoolToXML(node, "a:IsTree",tableSetting->isTree());
    writeBoolToXML(node, "a:IsTreeWithMasterView",tableSetting->isTreeWithMasterView());
    writeIntToXML(node, "a:TotalStyle", (int)tableSetting->totalStyle(), (int)gtsByTree);
    writeStrToXML(node, "a:RefTableName",tableSetting->refTableName());
    writeStrToXML(node, "a:RefDatabaseName",tableSetting->refDatabaseName());
    writeIntToXML(node, "a:DefRowHeight",tableSetting->defRowHeight());
    GXMLNode iNode = addChild(node, "c:FieldSettings");
    writeFieldSettings(iNode, tableSetting->fieldSettings());
    if (!iNode.hasChildNodes())
    {
        node.removeChild(iNode);
    }

    if (tableSetting->rules()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:Rules");
        writeGridRules(oNode, tableSetting->rules());
    }
    if (tableSetting->headerRowCount() != 0)
    {
        iNode = addChild(node, "c:HeaderRows");
        for (int i = 0; i <= tableSetting->headerRowCount() - 1; i++)
        {
            GXMLNode oNode = addChild(iNode, "o:HeaderRow");
            writeHeaderRowFieldSettings(oNode, tableSetting->headerRows(i));
        }
    }
    if (tableSetting->totalRowCount() != 0)
    {
        iNode = addChild(node, "c:TotalRows");
        for (int i = 0; i <= tableSetting->totalRowCount() - 1; i++)
        {
            GXMLNode oNode = addChild(iNode, "o:TotalRow");
            writeTotalRowFieldSettings(oNode, tableSetting->totalRows(i));
        }
    }

    if (tableSetting->extPropDefs()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:ExtPropDefs");
        writeExtPropDefs(oNode, tableSetting->extPropDefs());
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存字段设置容器对象为XML文档
  参数：node -- XML节点
        fieldSettings -- 字段设置容器对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeFieldSettings(GXMLNode &node, GLDFieldSettings *fieldSettings)
{
    for (int i = 0; i <= fieldSettings->count() - 1; i++)
    {
        if (isNullFieldSetting(fieldSettings->items(i)))
            continue;
        GXMLNode oNode = addChild(node, "o:FieldSetting");
        writeFieldSetting(oNode, fieldSettings->items(i));
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存字段设置对象为XML文档
  参数：node -- XML节点
        fieldSetting -- 字段设置对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeFieldSetting(GXMLNode &node, GLDFieldSetting *fieldSetting)
{
    GLDHorzAlignment eDefHorzAlignment;
    writeIntToXMLAttr(node, "Index", fieldSetting->index(), -1);
    writeIntToXMLAttr(node, "MergeID", fieldSetting->mergeID());
    writeStrToXMLAttr(node, "EditFieldName", fieldSetting->editFieldName());
    writeStrToXMLAttr(node, "DisplayExpr", fieldSetting->displayExpr());
    writeStrToXMLAttr(node, "SaveFieldName",fieldSetting->saveFieldName());
    writeStrToXMLAttr(node, "SaveRule",fieldSetting->saveRule());
    writeStrToXMLAttr(node, "NullValDisplayStr",fieldSetting->nullValDisplayStr());
    writeStrToXMLAttr(node, "ExtData",fieldSetting->extData());
    writeBoolToXMLAttr(node, "EditTextIsDisplayText",fieldSetting->editTextIsDisplayText(), true);
    writeStrToXMLAttr(node, "EditorName",fieldSetting->editorName());
    if (!fieldSetting->isDefaultCellIdentity())
        writeStrToXMLAttr(node, "CellIdentity", fieldSetting->cellIdentity());
    if (fieldSetting->isFixedCol())
        eDefHorzAlignment = ghaCenter;
    else
        eDefHorzAlignment = ghaAuto;
    if (!isDefaultCellFormat(fieldSetting->cellFormat(), eDefHorzAlignment))
    {
        GXMLNode oNode = addChild(node, "o:CellFormat");
        writeCellFormat(oNode, fieldSetting->cellFormat(), eDefHorzAlignment);
    }
    if (fieldSetting->extPropDefs()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:ExtPropDefs");
        writeExtPropDefs(oNode, fieldSetting->extPropDefs());
    }

}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存呈现规则容器对象为XML文档
  参数：node -- XML节点
        gridRules -- 呈现规则容器对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeGridRules(GXMLNode &node, GLDGridRules *gridRules)
{
    GXMLNode iNode;
    for (int i = 0; i <= gridRules->count() - 1; i++)
    {
        iNode = addChild(node, "o:GridRule");
        writeStrToXMLAttr(iNode, "RuleType", GridRuleTypeToStr(gridRules->items(i)->ruleType()));
        writeGridRule(iNode, gridRules->items(i));
    }
}

void GLDGridSettingXMLWriter::writeHeaderRowFieldSettings(GXMLNode &node,
                                                          GLDHeaderRowFieldSettings *headerRowFieldSettings)
{
    GXMLNode iNode;
    writeIntToXML(node, "a:RowHeight", headerRowFieldSettings->rowHeight());
    iNode = addChild(node, "c:FieldSettings");
    for (int i = 0; i <= headerRowFieldSettings->count() - 1; i++)
    {
        if (isNullExpandFieldSetting(headerRowFieldSettings->items(i)))
            continue;
        GXMLNode oNode = addChild(iNode, "o:FieldSetting");
        writeHeaderRowFieldSetting(oNode, headerRowFieldSettings->items(i));
    }
    if (!iNode.hasChildNodes())
        node.removeChild(iNode);
}

void GLDGridSettingXMLWriter::writeHeaderRowFieldSetting(GXMLNode &node,
                                                         GLDHeaderRowFieldSetting *headerRowFieldSetting)
{
    writeIntToXMLAttr(node, "Index", headerRowFieldSetting->index(), -1);
    writeIntToXMLAttr(node, "MergeID", headerRowFieldSetting->mergeID());
    writeStrToXMLAttr(node, "Value", headerRowFieldSetting->value());
    if (!isDefaultCellFormat(headerRowFieldSetting->cellFormat(), ghaCenter))
    {
        GXMLNode oNode = addChild(node, "o:CellFormat");
        writeCellFormat(oNode, headerRowFieldSetting->cellFormat(), ghaCenter);
    }
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-16
  功能: 保存合计行设置容器为XML文档
  参数: node: GXMLNode &-- XML节点
        totalRowFieldSettings -- 合计行设置容器
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeTotalRowFieldSettings(GXMLNode &node,
                                                         GLDTotalRowFieldSettings *totalRowFieldSettings)
{
    GXMLNode iNode;
    writeIntToXML(node, "a:RowHeight", totalRowFieldSettings->rowHeight());
    iNode = addChild(node, "c:FieldSettings");
    for (int i = 0; i <= totalRowFieldSettings->count() - 1; i++)
    {
        if (isNullExpandFieldSetting(totalRowFieldSettings->items(i)))
            continue;
        GXMLNode oNode = addChild(iNode, "o:FieldSetting");
        writeTotalRowFieldSetting(oNode, totalRowFieldSettings->items(i));
    }
    if (!iNode.hasChildNodes())
        node.removeChild(iNode);
}

/*-----------------------------------------------------------------------------
  创建: Liuxd 2005-12-16
  功能: 保存合计行设置对象为XML文档
  参数: node: GXMLNode &-- XML节点
        totalRowFieldSettings -- 合计行设置对象
-----------------------------------------------------------------------------*/
void GLDGridSettingXMLWriter::writeTotalRowFieldSetting(GXMLNode &node, GLDTotalRowFieldSetting *totalRowFieldSetting)
{
    GLDHorzAlignment eDefHorzAlignment;
    writeIntToXMLAttr(node, "Index",totalRowFieldSetting->index(), -1);
    writeIntToXMLAttr(node, "MergeID",totalRowFieldSetting->mergeID());
    writeStrToXMLAttr(node, "Value",totalRowFieldSetting->value());
    if (totalRowFieldSetting->isFixedCol())
        eDefHorzAlignment = ghaCenter;
    else
        eDefHorzAlignment = ghaAuto;
    if (!isDefaultCellFormat(totalRowFieldSetting->cellFormat(), eDefHorzAlignment))
    {
        GXMLNode oNode = addChild(node, "o:CellFormat");
        writeCellFormat(oNode, totalRowFieldSetting->cellFormat(), eDefHorzAlignment);
    }
}

void GLDRecordGridSettingXMLWriter::write(const GString &fileName, GLDRecordGridSetting *gridSetting, bool encryptData)
{
    GFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    GXMLDocument iXMLDoc = createXMLDocument();
    write(iXMLDoc, gridSetting);
#ifdef GLD_XML
    iXMLDoc.saveToStream(&file);
#else
    GTextStream out(&file);
    iXMLDoc.save(out, 4);
#endif
    file.close();
    Q_UNUSED(encryptData);
}

void GLDRecordGridSettingXMLWriter::write(GXMLDocument &xmlDoc, GLDRecordGridSetting *gridSetting)
{
    if (xmlDoc.documentElement().isNull())
    {
        GXMLNode root = xmlDoc.createElement("o:RecordGridSetting");
        xmlDoc.appendChild(root);
    }
    GXMLNode oNode = xmlDoc.documentElement();
    write(oNode, gridSetting);
}

void GLDRecordGridSettingXMLWriter::write(GXMLNode &node, GLDRecordGridSetting *gridSetting)
{
    if (node.isNull())
        return;

    writeStrToXMLAttr(node, "xmlns:a", "attribute");
    writeStrToXMLAttr(node, "xmlns:c", "collection");
    writeStrToXMLAttr(node, "xmlns:o", "object");
    writeStrToXMLAttr(node, "Version", gridSetting->version());
    writeStrToXMLAttr(node, "Description",gridSetting->description());
    writeStrToXMLAttr(node, "Identity",gridSetting->identity());
    writeStrToXMLAttr(node, "Remark",gridSetting->remark());
    writeIntToXML(node, "a:RowCount",gridSetting->rowCount());
    writeIntToXML(node, "a:ColCount",gridSetting->colCount());
    writeStrToXML(node, "a:RowHeights",gridSetting->rowHeightsToStr());
    writeStrToXML(node, "a:ColWidths",gridSetting->colWidthsToStr());
    writeStrToXML(node, "a:MinColWidths",gridSetting->minColWidthsToStr());
    writeStrToXML(node, "a:ColStyles",gridSetting->colStylesToStr());
    writeStrToXML(node, "a:ColIdentitys",gridSetting->colIdentitysToStr());
    writeStrToXML(node, "a:InvalidValueLabel",gridSetting->invalidValueLabel());
    writeIntToXML(node, "a:GridLineWidth",gridSetting->gridLineWidth());
    writeIntToXML(node, "a:GridLineColor",gridSetting->gridLineColor());
    writeIntToXML(node, "a:GridColor",gridSetting->gridColor());
    writeIntToXML(node, "a:BoundLineColor",gridSetting->boundLineColor(), g_clBlack);
    writeBoolToXML(node, "a:ShowAsTree",gridSetting->showAsTree());
    writeBoolToXML(node, "a:Editing",gridSetting->editing(), true);
    writeBoolToXML(node, "a:VertLine",gridSetting->vertLine(), true);
    writeBoolToXML(node, "a:HorzLine",gridSetting->horzLine(), true);
    writeBoolToXML(node, "a:RowSizing",gridSetting->rowSizing());
    writeBoolToXML(node, "a:ColSizing",gridSetting->colSizing(), true);
    writeBoolToXML(node, "a:ColMoving",gridSetting->colMoving(), true);
    writeBoolToXML(node, "a:AlwaysShowEditor",gridSetting->alwaysShowEditor());
    writeBoolToXML(node, "a:RangeSelect",gridSetting->rangeSelect(), true);
    writeBoolToXML(node, "a:AllowSelectRow",gridSetting->allowSelectRow(), true);
    writeBoolToXML(node, "a:AllowSelectCol",gridSetting->allowSelectCol(), true);
    writeBoolToXML(node, "a:AllowSelectAll",gridSetting->allowSelectAll(), true);
    writeBoolToXML(node, "a:MultiSelection",gridSetting->multiSelection(), true);
    writeBoolToXML(node, "a:AutoThemeAdapt",gridSetting->autoThemeAdapt(), true);
    writeBoolToXML(node, "a:ReturnKeyAsTab",gridSetting->returnKeyAsTab());
    writeBoolToXML(node, "a:ShowFixedRow",gridSetting->showFixedRow(), true);
    writeBoolToXML(node, "a:CellFilling",gridSetting->cellFilling());
    writeBoolToXML(node, "a:FixedCellEvent",gridSetting->fixedCellEvent());
    writeBoolToXML(node, "a:Allow3DStyle",gridSetting->allow3DStyle(), true);
    writeBoolToXML(node, "a:AllowCopyPaste",gridSetting->allowCopyPaste(), true);
    writeBoolToXML(node, "a:HideEditOnExit",gridSetting->hideEditOnExit());
    writeBoolToXML(node, "a:BorderStyle",gridSetting->borderStyle(), true);
    writeBoolToXML(node, "a:UseBlendColor",gridSetting->useBlendColor(), true);
    writeBoolToXML(node, "a:CellFillEditField",gridSetting->cellFillEditField());
    writeIntToXML(node, "a:FixedRows",gridSetting->fixedRows());
    writeIntToXML(node, "a:FixedCols",gridSetting->fixedCols());
    writeIntToXML(node, "a:FixedEditRows",gridSetting->fixedEditRows());
    writeIntToXML(node, "a:FixedEditCols",gridSetting->fixedEditCols());
    writeIntToXML(node, "a:LeftMargin",gridSetting->leftMargin());
    writeIntToXML(node, "a:RightMargin",gridSetting->rightMargin());
    writeIntToXML(node, "a:TopMargin",gridSetting->topMargin());
    writeIntToXML(node, "a:BottomMargin",gridSetting->bottomMargin());
    writeBoolToXML(node, "a:IgnoreInvalidCell",gridSetting->ignoreInvalidCell());
    writeBoolToXML(node, "a:AllowLabelSelection",gridSetting->allowLabelSelection());

    if (gridSetting->extPropDefs()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:ExtPropDefs");
        writeExtPropDefs(oNode, gridSetting->extPropDefs());
    }
    GXMLNode oNode = addChild(node, "c:RecordSettings");
    writeRecordSettings(oNode, gridSetting->recordSettings());
    GXMLNode iNode = addChild(node, "c:FieldSettings");
    writeFieldSettings(iNode, gridSetting->fieldSettings());
    if (!iNode.hasChildNodes())
        node.removeChild(iNode);

}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存呈现规则容器对象为XML文档
  参数：node -- XML节点
        gridRules -- 呈现规则容器对象
-----------------------------------------------------------------------------*/
void GLDRecordGridSettingXMLWriter::writeGridRules(GXMLNode &node, GLDRecordGridRules *gridRules)
{
    GXMLNode iNode;
    for (int i = 0; i <= gridRules->count() - 1; i++)
    {
        iNode = addChild(node, "o:GridRule");
        writeStrToXMLAttr(iNode, "RuleType", GridRuleTypeToStr(gridRules->items(i)->ruleType()));
        writeGridRule(iNode, gridRules->items(i));
    }
}

/*-----------------------------------------------------------------------------
  创建：Tu Jianhua 2005.11.11
  功能：保存呈现规则对象为XML文档
  参数：node -- XML节点
        rule -- 呈现规则对象
-----------------------------------------------------------------------------*/
void GLDRecordGridSettingXMLWriter::writeFieldSetting(GXMLNode &node, GLDRecordFieldSetting *fieldSetting)
{
    writeIntToXMLAttr(node, "Index",fieldSetting->index(), -1);
    writeIntToXMLAttr(node, "DataSourceID",fieldSetting->dataSourceID());
    writeIntToXMLAttr(node, "MergeID",fieldSetting->mergeID());
    writeStrToXMLAttr(node, "EditFieldName",fieldSetting->editFieldName());
    writeStrToXMLAttr(node, "DisplayExpr",fieldSetting->displayExpr());
    writeStrToXMLAttr(node, "SaveFieldName",fieldSetting->saveFieldName());
    writeStrToXMLAttr(node, "SaveRule",fieldSetting->saveRule());
    writeStrToXMLAttr(node, "NullValDisplayStr",fieldSetting->nullValDisplayStr());
    writeStrToXMLAttr(node, "ExtData",fieldSetting->extData());
    writeBoolToXMLAttr(node, "EditTextIsDisplayText",fieldSetting->editTextIsDisplayText(), true);
    writeBoolToXMLAttr(node, "IsPasswordChar",fieldSetting->isPasswordChar());
    writeStrToXMLAttr(node, "EditorName",fieldSetting->editorName());
    writeStrToXMLAttr(node, "CellIdentity",fieldSetting->cellIdentity());
    writeStrToXMLAttr(node, "VisibleCondition",fieldSetting->visibleCondition());

    if (!isDefaultCellFormat(fieldSetting->cellFormat()))
    {
        GXMLNode oNode = addChild(node, "o:CellFormat");
        writeCellFormat(oNode, fieldSetting->cellFormat());
    }
    if (fieldSetting->extPropDefs()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:ExtPropDefs");
        writeExtPropDefs(oNode, fieldSetting->extPropDefs());
    }
}

void GLDRecordGridSettingXMLWriter::writeFieldSettings(GXMLNode &node, GLDRecordFieldSettings *fieldSettings)
{
    for (int i = 0; i <= fieldSettings->count() - 1; i++)
    {
        if (isNullFieldSetting(fieldSettings->items(i)))
            continue;
        GXMLNode oNode = addChild(node, "o:FieldSetting");
        writeFieldSetting(oNode, fieldSettings->items(i));
    }
}

void GLDRecordGridSettingXMLWriter::writeRecordSetting(GXMLNode &node, GLDRecordSetting *recordSetting)
{
    writeStrToXML(node, "a:RefDatabaseName",recordSetting->refDatabaseName());
    writeStrToXML(node, "a:RefTableName",recordSetting->refTableName());

    if (recordSetting->rules()->count() > 0)
    {
        GXMLNode oNode = addChild(node, "c:Rules");
        writeGridRules(oNode, recordSetting->rules());
    }
}

void GLDRecordGridSettingXMLWriter::writeRecordSettings(GXMLNode &node, GLDRecordSettings *recordSettings)
{
    for (int i = 0; i <= recordSettings->count() - 1; i++)
    {
        GXMLNode oNode = addChild(node, "o:RecordSetting");
        writeRecordSetting(oNode, recordSettings->items(i));
    }
}

//#include <QXmlStreamReader>

////XMLStreamReader测试，实际工作别使用此类
//class GLDRecordGridSettingXMLStreamReader: public QXmlStreamReader
//{
//private:
//    void readGridSetting(GLDRecordGridSetting *gridSetting);
//    void readRecordSettings(GLDRecordSettings *recordSettings);
//    void readRecordSetting(GLDRecordSetting *recordSetting);
//    void readRules(GLDRecordGridRules *rules);
//    void readGridRule(GLDGridRule *rule);
//    void readFieldSettings(GLDRecordGridSetting *gridSetting);
//public:
//    void read(const GString fileName, GLDRecordGridSetting *gridSetting);
//public:
//    inline GLDRecordGridSettingXMLStreamReader() {}
//    inline ~GLDRecordGridSettingXMLStreamReader() {}
//};

///* GLDRecordGridSettingXMLStreamReader */

//void GLDRecordGridSettingXMLStreamReader::read(const GString fileName, GLDRecordGridSetting *gridSetting)
//{
//    GFile file(fileName);
//    if (!file.open(QIODevice::ReadOnly))
//        return;

//    setDevice(&file);
//    while (!atEnd())
//    {
//        readNext();
//        if (isStartElement())
//        {
//            if (sameText(name().toString(), "o:RecordGridSetting"))
//                readGridSetting(gridSetting);
//            else if (sameText(name().toString(), "a:RowCount"))
//                gridSetting->setRowCount(readElementText().toInt());
//            else if (sameText(name().toString(), "c:RecordSettings"))
//                readRecordSettings(gridSetting->recordSettings());
//            else if (sameText(name().toString(), "c:FieldSettings"))
//                readFieldSettings(gridSetting);

//        }
//        else if (isEndElement())
//        {
//            //
//        }

//    }

//    file.close();
//}

//void GLDRecordGridSettingXMLStreamReader::readGridSetting(GLDRecordGridSetting *gridSetting)
//{
//    if (attributes().hasAttribute("Description"))
//        gridSetting->setDescription(attributes().value("Description").toString());


//}

//void GLDRecordGridSettingXMLStreamReader::readRecordSettings(GLDRecordSettings *recordSettings)
//{
//    while (!atEnd())
//    {
//        readNext();
//        if (isStartElement() && sameText(name().toString(), "c:RecordSetting"))
//            readRecordSetting(recordSettings->insert(-1));
//        else if (isEndElement() && sameText(name().toString(), "c:RecordSettings"))
//            return;
//    }
//}

//void GLDRecordGridSettingXMLStreamReader::readRecordSetting(GLDRecordSetting *recordSetting)
//{
//    while (!atEnd())
//    {
//        readNext();
//        if (isStartElement() )
//        {
//            if (sameText(name().toString(), "a:RefDatabaseName"))
//                recordSetting->setRefDatabaseName(readElementText());
//            else if  (sameText(name().toString(), "a:RefTableName"))
//                recordSetting->setRefTableName(readElementText());
//            else if  (sameText(name().toString(), "c:Rules"))
//                readRules(recordSetting->rules());
//        }
//        if (isEndElement() && sameText(name().toString(), "c:RecordSetting"))
//            return;
//        readNext();
//    }
//}

//void GLDRecordGridSettingXMLStreamReader::readRules(GLDRecordGridRules *rules)
//{
//    GLDGridRuleType eRuleType;
//    while (!atEnd())
//    {
//        readNext();
//        if (isStartElement() && sameText(name().toString(), "o:GridRule"))
//        {
//            eRuleType = StrToGridRuleType(attributes().value("RuleType").toString());
//            readGridRule(rules->insert(- 1, eRuleType));
//        }
//        if (isEndElement() && sameText(name().toString(), "c:Rules"))
//            return;
//    }
//}

//void GLDRecordGridSettingXMLStreamReader::readGridRule(GLDGridRule *rule)
//{
//    if (attributes().hasAttribute("Name"))
//        rule->setRuleName(attributes().value("name").toString());
//    if (attributes().hasAttribute("Enable"))
//        rule->setEnable(sameText(attributes().value("Enable").toString(), "true"));

//    while (!atEnd())
//    {
//        readNext();
//        if (isStartElement())
//        {
//            if (sameText(name().toString(), "a:FieldCondition"))
//                rule->setFieldCondition(readElementText());
//            else if (sameText(name().toString(), "a:RecordCondition"))
//                rule->setRecordCondition(readElementText());
//            else if (sameText(name().toString(), "a:AllowClear"))
//                dynamic_cast<GLDClearCellRule *>(rule)->setAllowClear(sameText(readElementText(), "true"));
//            else if (sameText(name().toString(), "a:FontName"))
//                dynamic_cast<GLDFontRule *>(rule)->setFontName(readElementText());
//        }
//        if (isEndElement() && sameText(name().toString(), "o:GridRule"))
//            return;
//    }
//}

//void GLDRecordGridSettingXMLStreamReader::readFieldSettings(GLDRecordGridSetting *gridSetting)
//{
//    G_UNUSED(gridSetting);
//}

