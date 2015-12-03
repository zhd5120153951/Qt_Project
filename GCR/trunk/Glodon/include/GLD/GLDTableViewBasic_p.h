#pragma once

#include "GLDAbstractItemView_p.h"
#include "GLDTableViewBasic.h"

class QPushButton;

enum RangeFillHandlePositon
{
    LeftBottom,
    RightBottom,
    RightTop
};

/*!
 *@GLDTableViewPrivate
 *@brief { GLDTableView 对应的“私有类”，方法不对外公布}
 *@author Gaosy
 *@date 2012.9.7
 */
class GLDTABLEVIEWSHARED_EXPORT GLDTableViewPrivate : public GlodonAbstractItemViewPrivate
{
    Q_DECLARE_PUBLIC(GLDTableView)
public:
    GLDTableViewPrivate() :
        m_allowCopy(false),
        m_allowPaste(false),
        m_allowRangeFilling(false),
        m_allowSelectCol(true),
        m_allowSelectRow(true),
        m_allowToResizeCellByDragGridLine(false),
        m_alwaysShowRowSelectedColor(false),
        m_bRangeMoving(false),
        m_cellFillEditField(true),
        m_bDrawTopAndLeftBorderLine(false),
        m_geometryRecursionBlock(false),
        m_bShowHorizontalGridLine(true),
        m_bIsInAdjustFitCols(false),
        m_inCommitData(false),
        m_bIsCustomStyle(false),
        m_isShowResizeInfoFrame(true),
        m_legalData(true),
        m_showEllipsisButton(false),
        m_showGrid(true),
        m_sortingEnabled(false),
        m_bShowVerticalGridLine(true),
        m_columnResizeTimerID(0),
        m_columnSectionAnchor(-1),
        m_currResizeWidth(0),
        m_nCurrentResizeCol(-1),
        m_nFixedColCount(0),
        m_nFixedRowCount(0),
        m_nFixedColWidth(0),
        m_nFixedRowHeight(0),
        m_gridLineWidth(1),
        m_nResizeCellStartPosition(0),
        m_nResizeCellEndPosition(0),
        m_rowResizeTimerID(0),
        m_rowSectionAnchor(-1),
        m_infoFrame(NULL),
        m_pResizeInfoLineFrame(NULL),
        m_bAllRowsResizeToContents(false),
        m_bCalcAllColumns(false),
        m_bAllColumnsResizeToContents(false),
        m_bCalcAllRows(false),
        m_horizontalHeader(NULL),
        m_verticalHeader(NULL),
        m_ellipsisButton(NULL),
        m_cornerWidget(NULL),
        m_visualCursor(QPoint()),
        m_gridStyle(Qt::SolidLine),
        m_borderLineColor(QColor(192,192,192)),
        m_gridLineColor(QColor(192,192,192)),
        m_oFrameLineColor(QColor(192,192,192)),
        m_selectBoundLineColor(QColor("#39a9d1")),
        m_selectBoundLineWidth(2),
        m_oNoFocusSelectedBoundLineColor(QColor(Qt::darkGray)),
        m_rangeFillingStyle(rfsVertical),
        m_bDrawBoundLine(false),
        m_prevState(GlodonAbstractItemView::FirstFocus),
        m_bEnterJump(true),
        m_bEnterJumpPro(false),
        m_editStyleDrawType(GlodonAbstractItemView::SdtNone),
        m_pCommentFrame(NULL),
        m_bCloseEditorOnFocusOut(false),
        m_bUseBlendColor(false),
        m_oSelectedCellBackgroundColor("#f9f9f9"),
        m_bUseComboBoxCompleter(true),
        m_bIsInReset(false),
        m_rangeFillHandlePositon(RightBottom)
    {
        m_bWrapItemText = true;
#ifndef QT_NO_DRAGANDDROP
        m_bDragDropOverwrite = true;
#endif
    }

    ~GLDTableViewPrivate();

    void init();

    void trimHiddenSelections(QItemSelectionRange *range) const;

    int sectionSpanEndLogical(const GlodonHeaderView *header, int logical, int span) const;
    int sectionSpanSize(const GlodonHeaderView *header, int logical, int span) const;
    bool spanContainsSection(const GlodonHeaderView *header, int logical, int spanLogical, int span) const;

    /*!
     *绘制所有合并格，并将它们所在区域标记为已绘制
     *@param[in]  area               整个TableView的绘制区域
     *@param[in]  painter
     *@param[in]  option
     *@param[in]  drawn              记录已绘制区域的数组
     *@param[in]  firstVisualRow     第一个可见行的虚拟行号
     *@param[in]  lastVisualRow      最后一个可见行的虚拟行号
     *@param[in]  firstVisualColumn  第一个可见列的虚拟列号
     *@param[in]  lastVisualColumn   最后一个可见列的虚拟列号
     *@return 无
     */
    virtual void drawAndClipSpans(const QRegion &area, QPainter *painter,
                          const QStyleOptionViewItem &option, QBitArray *drawn,
                          int firstVisualRow, int lastVisualRow, int firstVisualColumn, int lastVisualColumn);

    /*!
     *绘制格子
     *@param[in]  painter
     *@param[in]  option
     *@param[in]  index    所绘制格子对应的model中的QModelIndex
     *@return 无
     */
    virtual void drawCell(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index);

    void setSpan(int row, int column, int rowSpan, int columnSpan);
    GSpanCollection::GSpan span(int row, int column) const;
    QRect visualSpanRect(const GSpanCollection::GSpan &span) const;
    QRect visualSpanRectIncludeBoundingLineWidth(const GSpanCollection::GSpan &span) const;

    QRect visualRectIncludeBoundingLineWidth(const QModelIndex &index) const;

    void spanTopRowLeftColumn(const GSpanCollection::GSpan &span, int &row, int &column) const;

    QRect viewportScrollArea(Qt::Orientation orientation);

    void _q_selectRow(int row);
    void _q_selectColumn(int column);
    void _q_selectRows(int left, int top, int right, int bottom);
    void _q_selectColumns(int left, int top, int right, int bottom);

    void selectRow(int row, bool anchor);
    void selectColumn(int column, bool anchor);
    bool selectColumns(int start, int end, bool anchor);
    bool selectRows(int start, int end, bool anchor);

    void _q_updateSpanInsertedRows(const QModelIndex &parent, int start, int end);
    void _q_updateSpanInsertedColumns(const QModelIndex &parent, int start, int end);
    void _q_updateSpanRemovedRows(const QModelIndex &parent, int start, int end);
    void _q_updateSpanRemovedColumns(const QModelIndex &parent, int start, int end);

    void drawTriangles(QRect rect, QPainter *painter);

public: // Inline
    inline bool isHidden(int row, int col) const
    {
        return m_verticalHeader->isSectionHidden(row)
               || m_horizontalHeader->isSectionHidden(col);
    }
    inline int visualRow(int logicalRow) const
    {
        return m_verticalHeader->visualIndex(logicalRow);
    }
    inline int visualColumn(int logicalCol) const
    {
        return m_horizontalHeader->visualIndex(logicalCol);
    }
    inline int logicalRow(int visualRow) const
    {
        return m_verticalHeader->logicalIndex(visualRow);
    }
    inline int logicalColumn(int visualCol) const
    {
        return m_horizontalHeader->logicalIndex(visualCol);
    }
    inline int accessibleTable2Index(const QModelIndex &index) const
    {
        return (index.row() + (m_horizontalHeader ? 1 : 0)) * (index.model()->columnCount() + (m_verticalHeader ? 1 : 0))
               + index.column() + (m_verticalHeader ? 1 : 0) + 1;
    }
    inline int rowSpan(int row, int column) const
    {
        return span(row, column).height();
    }
    inline int columnSpan(int row, int column) const
    {
        return span(row, column).width();
    }
    inline bool hasSpans() const
    {
        return !m_spans.spans.isEmpty();
    }
    inline int rowSpanHeight(int row, int span) const
    {
        return sectionSpanSize(m_verticalHeader, row, span);
    }
    inline int columnSpanWidth(int column, int span) const
    {
        return sectionSpanSize(m_horizontalHeader, column, span);
    }
    inline int rowSpanEndLogical(int row, int span) const
    {
        return sectionSpanEndLogical(m_verticalHeader, row, span);
    }
    inline int columnSpanEndLogical(int column, int span) const
    {
        return sectionSpanEndLogical(m_horizontalHeader, column, span);
    }
    inline bool isRowHidden(int row) const
    {
        return m_verticalHeader->isSectionHidden(row);
    }
    inline bool isColumnHidden(int column) const
    {
        return m_horizontalHeader->isSectionHidden(column);
    }
    inline bool isCellEnabled(int row, int column) const
    {
        return isIndexEnabled(m_model->index(row, column, m_root));
    }
    inline bool isCellEnabledAndSelectabled(int row, int column) const
    {
        return (isIndexEnabled(m_model->index(row, column, m_root)) && isIndexSelectable(m_model->index(row, column, m_root)));
    }
    inline bool isVisualRowHiddenOrDisabled(int row, int column) const
    {
        int nRow = logicalRow(row);
        int nCol = logicalColumn(column);

        return isRowHidden(nRow) || !isCellEnabled(nRow, nCol);
    }
    inline bool isVisualColumnHiddenOrDisabled(int row, int column) const
    {
        int nRow = logicalRow(row);
        int nCol = logicalColumn(column);

        return isColumnHidden(nCol) || !isCellEnabled(nRow, nCol);
    }
    inline bool isVisibleColumn(int column) const
    {
        int nCol = logicalColumn(column);

        return (hbar != NULL) ? m_horizontalHeader->isSectionVisible(nCol) : true;
    }
    inline bool isVisibleRow(int row)const
    {
        int nRow = logicalRow(row);

        return (vbar != NULL) ? m_verticalHeader->isSectionVisible(nRow) : true;
    }

public: // bool
    bool m_allowCopy;                   //是否允许复制
    bool m_allowPaste;                  //是否允许粘贴
    bool m_allowRangeFilling;           //是否允许下拉填充
    bool m_allowSelectCol;              //是否允许列选
    bool m_allowSelectRow;              //是否允许行选
    bool m_allowToResizeCellByDragGridLine;               //是否允许改变大小
    bool m_alwaysShowRowSelectedColor;  //是否总是显示当前焦点格所在行选择背景色
    bool m_bRangeMoving;                //是否允许框选可拖拽
    bool m_cellFillEditField;           //是否允许填充编辑字段
    bool m_bDrawTopAndLeftBorderLine;               //是否绘制无表头时候的上/左框线
    bool m_geometryRecursionBlock;
    bool m_bShowHorizontalGridLine;                    //是否显示水平格线
    bool m_bIsInAdjustFitCols;             //是否处于计算合适列宽标示符，防止设置超过一列的合适列宽后，出现循环调用，栈溢出
    bool m_inCommitData;                //当onCommitEditor有弹窗并且按Tabel键或Enter键时，在QItemDelegate的EventFilter中,会出现发送两次commit和closeEdit信号现象（按键一次，失去焦点一次），用该变量处理
    bool m_bIsCustomStyle;               //是否是自定义风格，用于GSF选项：格线颜色、宽度、固定格背景色、表格背景色
    bool m_isShowResizeInfoFrame;       //是否显示调整行高列宽的信息窗
    bool m_legalData;                   //是否是合法数据：details: 当commitData失败时，为false，不允许closeEditor
    bool m_showEllipsisButton;          //是否显示带三点按钮在tableView的右下角
    bool m_showGrid;                    //是否显示格线
    bool m_sortingEnabled;              //是否支持排序
    bool m_bShowVerticalGridLine;                    //是否显示垂直格线

public: // int
    int m_columnResizeTimerID;
    int m_columnSectionAnchor;
    int m_currResizeWidth;
    int m_nCurrentResizeCol;
    int m_nFixedColCount;            //固定列数
    int m_nFixedRowCount;            //固定行数
    mutable int m_nFixedColWidth;           // 存储可编辑固定列宽度，包括固定列之间的线宽和最后一根线的线宽
    mutable int m_nFixedRowHeight;          // 存储可编辑固定列高度，同上
    int m_gridLineWidth;            //格线宽度
    int m_nResizeCellStartPosition;
    int m_nResizeCellEndPosition;
    int m_rowResizeTimerID;
    int m_rowSectionAnchor;

public: // pointer
    GInfoFrame *m_infoFrame;
    QFrame *m_pResizeInfoLineFrame;

    GIntList m_oFitColWidthCols;
    GIntList m_oSuitColWidthCols;  // 存放计算自动列宽的时候，需要参考的列号
    GIntList m_oSuitRowHeightAccordingCols; // 存放计算自动行高的时候，需要参考的列号

    bool m_bAllRowsResizeToContents;
    bool m_bCalcAllColumns;

    bool m_bAllColumnsResizeToContents;
    bool m_bCalcAllRows;

    GlodonHeaderView *m_horizontalHeader;
    GlodonHeaderView *m_verticalHeader;

    QPushButton *m_ellipsisButton;

    QWidget *m_cornerWidget;

public: // type
    GSpanCollection m_spans;

    QMap<QPersistentModelIndex, GCustomCommentFrame *> m_oCommentFrames;
    QPoint m_visualCursor;      // (Row,column) cell coordinates to track through span navigation.

    Qt::PenStyle m_gridStyle;

    QColor m_borderLineColor;    //框线颜色
    QColor m_gridLineColor;     //格线颜色
    QColor m_oFrameLineColor;   // 在表头隐藏，且滚动模式为Item时，绘制的整个表格上边框或者左边框线的颜色

    QColor m_selectBoundLineColor;  //选择的的单元格，绘制的框线颜色
    int m_selectBoundLineWidth;     //选择的的单元格，绘制的框线宽度
    QColor m_oNoFocusSelectedBoundLineColor;                          // 失去焦点时，选择的的单元格，绘制的框线颜色

    QList<int> m_columnsToUpdate;
    QList<int> m_rowsToUpdate;

    RangeFillingStyle m_rangeFillingStyle;

    QHash<QModelIndex, QVariant> m_borderLines; // 用于缓存绘制时的边框线值
    bool m_bDrawBoundLine; // 格线分段绘制
    QRect m_oRangeFillHandleRect; // 缓存选择填充时，右下角矩形的大小

    QModelIndex m_oTopLeftSelectIndex; // 如果有span或者行，列移动之后，selectionModel里面存储的ItemRange为多个，所以用此缓存仅选择一个区域时的TopLeft
    QModelIndex m_oBottomRightSelectIndex; // 缓存仅选择一个区域时的BottomRight
    GlodonAbstractItemView::EnterMovingState m_prevState;
    bool m_bEnterJump; //回车跳格
    bool m_bEnterJumpPro; //开启回车跳格后，再开启这个，减少设置焦点的一步（处于编辑状态时，如果下一个能编辑，则直接进入下一个的编辑状态）
    GlodonAbstractItemView::EditStyleDrawType m_editStyleDrawType;

    GCommentFrame *m_pCommentFrame;
    bool m_bCloseEditorOnFocusOut;
    bool m_bUseBlendColor; //使用混合色
    QColor m_oSelectedCellBackgroundColor; //选择区域的背景颜色
    bool m_bUseComboBoxCompleter;//设置是否使用comboBox自动提示功能

    bool m_bIsInReset; // 防止批注框在reset时，会一致移动，添加，后续改批注框或者GSPTableView的reset恢复rowHeight流程时再考虑去掉

    RangeFillHandlePositon m_rangeFillHandlePositon;

    void mergeOriginColsWidthAndAdjustColsWidth(GIntList &colWidths, const GIntList &originColsWidth, const GIntList &adjustColsWidth);
protected:
    virtual void doSelect(const QModelIndex& tl, const QModelIndex& br, QItemSelectionModel::SelectionFlags command, bool isRowSelect);
    virtual void itemDelegatePaint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    // duanb cuidc 注释小三角
    void drawTriangles(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index);

    /**
     * @brief 根据传入的样式和当前上下文来配置新的绘制样式
     * @param option 当前样式
     * @param index
     * @param opt  新样式
     */
    void adjustStyleOption(QStyleOptionViewItem &opt, const QModelIndex &index);

    bool nothingToDraw();
    bool currentCellDrawn(GlodonTableViewPainterInfo &info, int nVisualCol, QBitArray &drawn, int nColumnCount, int nVisualRow);
    bool isCoverByFixedRow(int row, int rowYEndPos, bool addGridLineWidth);
    bool isCoverByFixedColumn(int column, int columnXEndPos, bool addGridLineWidth);
    void setOptionFeaturesAlternateProperty(bool bAlternateBase, bool balternate, QStyleOptionViewItem &option);
    QRect cellPainterRect(QRect srcRect, int visualRow, int visualCol);

    void calculateFixedRegion();
    void calculateFixedColumnWidth() const;
    void calculateFixedRowHeight() const;

    void setOptionSelectedState(QStyleOptionViewItem& opt, const QModelIndex &index);
    void setOptionMouseOverState(const QModelIndex &index, QStyleOptionViewItem& opt);
    void setOptionEnabledState(QStyleOptionViewItem& opt, const QModelIndex &index);
    void setOptionHasFocusState(const QModelIndex &index, QStyleOptionViewItem& opt);

    QItemSelection getItemSelectionAccordingToSelectState();
    void adjustSelectionBoundingRectAccordingToFixedColumn(const QRegion &bordingRegion, QRect &srcRect);
    void adjustSelectionBoundingRectAccordingToFixedRow(const QRegion &bordingRegion, QRect &srcRect);
    void drawNormalSelectionBoundingLine(QPainter &painter, QRect rect);
    void drawBorderWithFillHandle(QPainter &painter, QRect rect);

    QList<GSpanCollection::GSpan *> getVisibleSpansAccordingToIsSectionMove(int firstVisualRow, int firstVisualColumn, int lastVisualRow, int lastVisualColumn);
    void adjustOptionAlternateFeature(QStyleOptionViewItem &opt, bool bAlternateBase);

    bool isRangeFillingHandleSelected(const QPoint &p);
    bool isInMutiSelect() const;
    inline bool isEnterJump() { return m_bEnterJump && !m_bEnterJumpPro; }
    inline bool isEnterJumpPro() { return m_bEnterJump && m_bEnterJumpPro; }

    void drawComment(const QPersistentModelIndex &index);
    void hideCommentFrame();

    void setGridLineWidthToHorizontalHeader(int gridLineWidth);
    void setGridLineWidthToVertialHeader(int gridLineWidth);

    // 重构自动行高
    int heightHintForIndex(const QModelIndex &index, int nHint, QStyleOptionViewItem &option) const;
    int calcSizeHintForRow(int row, const QList<int> &oNeedCalcSizeHintCols) const;
    int widthHintForIndex(const QModelIndex &index, int hint, const QStyleOptionViewItem &option) const;
    int widthHintForIndex(int visualRow, int column, int hint, const QStyleOptionViewItem &option) const;

    int sizeHintForColumn(int column, bool calcAllRows) const;

    void calcFixedColSuitWidth(bool &shouldResetCommentPosition);
    void calcNormalColSuitWidth(bool &shouldResetCommentPosition);

    void calcFixedRowSuitHeight(const QList<int> &oNeedCalcHeightCols, bool &shouldResetCommentPosition);
    void calcNormalRowSuitHeight(const QList<int> &oNeedCalcHeightCols, bool &shouldResetCommentPosition);

    int calcSuitWidth(int nCol, bool calcAllRows = false);
    int calcSuitHeight(const QList<int> &oNeedCalcHeightCols, int nLogicalRow);

    void adjustRectWidthByTreeColumn(const QModelIndex &index, QRect &rect) const;
    void adjustRectWidthBySpan(const QModelIndex &index, QRect &rect) const;

    QColor getPenColor();
    bool isNearHorizontalGridLine(QMouseEvent *event);
    bool isNearVerticalGridLine(QMouseEvent *event);

    // 重构合适列宽
    void doAdjustColWidths(int currentResizeCol);
    void adjustFitColWidths(GIntList &originColWidths);
    void applyAdjustFitColsWidth(const GIntList &originColsWidth, const GIntList &adjustColsWidth);
    GIntList getAdjustedColWidths(const GIntList &originColWidths, int currentResizeColWidth);

    /*!
     * \brief 如果当前调整的列宽的列是合适列宽的列，就调整origineCols中的值
     * \param adjustedColWidths
     * \param currentResizeColIndexInFitCols
     * \param currentResizeCol
     * \param currentResizeColWidth
     * \param originColWidths
     */
    void adjustOriginColWidthWhenResizeColIsInFitCols(const GIntList &adjustedColWidths, int currentResizeColIndexInFitCols,
            int currentResizeColWidth, GIntList &originColWidths);
    void calcOldAdjustColsTotalWidth(const GIntList &oAdjustedColsWidth,
            int &nOldAdjustColsTotalWidth, int &nCurrentResizeColIndexInFitCols);
    void calcAdjustColsWidth(const int nAvailableAdjustWidth, const int nOldAdjustColsTotalWidth, GIntList &oAdjustedColsWidth,
            int &nNewAdjustColsTotalWidth);

    int getTotalColWidth(const GIntList &originColsWidth, int nColWidth);
    GIntList getOriginColWidthsFromView();

    int getMinTotalColWidth(const GIntList &originColWidths, const int nOriginTotalColWidth);
    int getDefaultColWidth(int col);

    bool isVisibleIndex(const QModelIndex &visualIndex);
    int defaultRowHeight(const QModelIndex &logicalIndex);

    bool isInTableView(QMouseEvent *event);
    QPoint calcResizeInfoLineFramePosition(Qt::Orientation direction);

    void fillSuitColWidthCols();
    void fillSuitRowHeightCols();
};
