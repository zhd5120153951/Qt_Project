/*!
 *@brief {网格编辑控件}
 *
 *@date 2015.1.6
 *@remarks {}
 *Copyright (c) 1998-2015 Glodon Corporation
 */

#pragma once

#include <QScrollBar>
#include <QLinkedList>

#include "GLDHeaderView.h"
#include "GLDAbstractItemView.h"
#include "GLDTableCornerButton.h"
#include "GLDIntList.h"
#include "GLDList.h"
#include "GLDCustomCommentFrame.h"

class GlodonScrollBar;

struct GlodonTableViewPainterInfo;

const int c_MinColWidth            = 10;     // 列双击自动调整宽度时最小宽度
const int c_Indent                 = 10;     // 标识层次关系的树线之间的距离
const int c_HalfWidth              = 5;      // 内部有加减号的正方形的一半宽度
const int c_CheckBoxWidth          = 13;     // 复选框宽度

/*!
 *@class: GSpanCollection
 *@brief {用于处理所有合并格相关功能}
 *@author Gaosy
 *@date 2012.9.7
 */
class GLDTABLEVIEWSHARED_EXPORT GSpanCollection
{
public:
    struct GSpan
    {
        int m_top;
        int m_left;
        int m_bottom;
        int m_right;
        bool will_be_deleted;
        GSpan()
            : m_top(-1), m_left(-1), m_bottom(-1), m_right(-1), will_be_deleted(false) { }

        GSpan(int row, int column, int rowCount, int columnCount)
            : m_top(row), m_left(column), m_bottom(row + rowCount - 1), m_right(column + columnCount - 1),
              will_be_deleted(false) { }

        inline int top() const
        {
            return m_top;
        }
        inline int left() const
        {
            return m_left;
        }
        inline int bottom() const
        {
            return m_bottom;
        }
        inline int right() const
        {
            return m_right;
        }
        inline int height() const
        {
            return m_bottom - m_top + 1;
        }
        inline int width() const
        {
            return m_right - m_left + 1;
        }
    };

    ~GSpanCollection()
    {
        clear();
    }

    void addSpan(GSpan *span);
    void updateSpan(GSpan *span, int old_height);
    GSpan *spanAt(int x, int y) const;
    void clear();
    QList<GSpan *> spansInRect(int x, int y, int w, int h) const;

    void updateInsertedRows(int start, int end);
    void updateInsertedColumns(int start, int end);
    void updateRemovedRows(int start, int end);
    void updateRemovedColumns(int start, int end);

    typedef QLinkedList<GSpan *> SpanList;
    SpanList spans;

private:
    typedef QMap<int, GSpan *> SubIndex;
    typedef QMap<int, SubIndex> Index;
    Index m_nIndex;

    bool cleanSpanSubIndex(SubIndex &subindex, int end, bool update = false);
};

Q_DECLARE_TYPEINFO(GSpanCollection::GSpan, Q_MOVABLE_TYPE);

/**
 * @brief
 *      top<<0, bottom<<8, left<<16, right<<24
 */
union GBorderLineWidthHelper
{
    enum
    {
        top,
        bottom,
        left,
        right,
        count
    };

    int widths;
    unsigned char lineWiths[count];
};

enum TreeDecorationStyle
{
    NoDecoration,
    NormalStyle,
    NormalNoLineStyle,
    OSStyle
};

enum Margin
{
    LeftMargin,
    RightMargin,
    TopMargin,
    BottomMargin
};

enum UpdateHeaderViewType
{
    uhVertical,
    uhHorizontal,
    uhBoth
};

enum RangeFillingStyle
{
    rfsVertical,
    rfsHorizontal,
    rfsBoth
};

enum SelectBorderWidth
{
    TwoPixel = 2,
    ThreePixel = 3
};

/*!
 *@class: GlodonTableView
 *@brief {网格编辑控件，提供了数据编辑、显示等功能，支持树形结构显示}
 *@author Gaosy
 *@date 2012.9.7
 */
class GLDTABLEVIEWSHARED_EXPORT GLDTableView : public GlodonAbstractItemView
{
    Q_OBJECT

    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid)
    Q_PROPERTY(Qt::PenStyle gridStyle READ gridStyle WRITE setGridStyle)
    Q_PROPERTY(bool sortingEnabled READ isSortingEnabled WRITE setSortingEnabled)
    Q_PROPERTY(bool wordWrap READ wordWrap WRITE setWordWrap)
    Q_PROPERTY(bool cornerButtonEnabled READ isCornerButtonEnabled WRITE setCornerButtonEnabled)
    Q_PROPERTY(int fixedRowCount READ fixedRowCount WRITE setFixedRowCount)
    Q_PROPERTY(int fixedColCount READ fixedColCount WRITE setFixedColCount)

public:
    explicit GLDTableView(QWidget *parent = 0);
    ~GLDTableView();

    /**
     * @brief 设置tableView的Model
     * @param model
     */
    void setModel(QAbstractItemModel *model);

    /*功能选项相关*/

    /**
     * @brief 是否处于编辑状态
     * @return
     */
    bool isEditing() const;
    /**
     * @brief 判断给定的行，列对应的格子是否能进入编辑状态(与ReadOnly不同)
     * @param row
     * @param column
     * @return
     */
    bool cellCanGetEdit(int row, int column);

    /**
     * @brief 是否显示过滤行
     * @return
     */
    virtual bool isDisplayFilter();

    /**
     * @brief 是否是合计行位于表脚
     * @return
     */
    virtual bool totalRowAtFooter() const;

    /**
     * @brief 设置是否允许排序
     * @param enable
     */
    void setSortingEnabled(bool enable);
    bool isSortingEnabled() const;

    /**
     * @brief
     *      设置自动行高
     *
     * @details
     *      表格会根据列宽和控件默认大小来自动计算给定的行高
     *
     * @param accordingCols 自动计算行高参考的列宽的列
     */
    void setSuitRowHeights(const GIntList &accordingCols);
    GIntList suitRowHeights();

    /**
     * @brief
     *      设置自动列宽
     *
     * @details
     *      表格会根据列中的内容和控件默认大小自动计算给定的列宽
     *
     * @param suitColWidths 自动计算列宽的列
     */
    void setSuitColWidths(const GIntList &suitColWidthCols);
    GIntList suitColWidths();

    /**
     * @brief
     *      设置合适列宽
     *
     * @details
     *      表格会根据其它列的情况自动计算给定的列宽
     *
     * @param fitColWidths 固定列宽的列
     */
    void setFitColWidths(const GIntList &fitColWidthsCols);
    GIntList fitColWidths();

    /**
     * @brief 设置是否允许复制
     * @param allowCopy
     */
    void setAllowCopy(bool allowCopy);
    bool allowCopy() const;

    /**
     * @brief 设置是否允许粘贴
     * @param allowPaste
     */
    void setAllowPaste(bool allowPaste);
    bool allowPaste() const;

    /**
     * @brief 设置左上角Button是否可用
     * @param enable
     */
    void setCornerButtonEnabled(bool enable);
    bool isCornerButtonEnabled() const;

    /**
     * @brief 设置新的控件（如果是可以设置模型的则会自动绑定）
     * @param widget
     */
    void setCornerWidget(QWidget *widget);
    QWidget *cornerWidget() const;

    /**
     * @brief 设置是否允许选择填充（与行选互斥）
     * @param enable
     */
    void setAllowRangeFilling(bool enable);
    bool allowRangeFilling() const;

    /**
     * @brief 选择填充的样式(只允许垂直，水平，或者都可以)
     * @param style
     */
    void setRangeFillingStyle(RangeFillingStyle style);
    RangeFillingStyle rangeFillingStyle();

    /**
     * @brief 设置是否允许行选(点击垂直表头，选中一整行)
     * @return
     */
    bool allowSelectRow() const;
    void setAllowSelectRow(bool value);

    /**
     * @brief 设置是否允许列选(点击水平表头，选中一整列)
     * @return
     */
    bool allowSelectCol() const;
    void setAllowSelectCol(bool value);

    /**
     * @brief 设置选中填充时，是否使用EditRole
     * @return
     */
    bool cellFillEditField() const;
    void setCellFillEditField(bool value);

    /*!
     *设置固定可编辑列数
     *@param[in]  fixedColCount  固定可编辑列数
     *@return 无
     */
    virtual void setFixedColCount(int fixedColCount);
    int fixedColCount() const;

    /*!
     *设置固定可编辑行数
     *@param[in]  fixedRowCount  固定可编辑行数
     *@return 无
     */
    void setFixedRowCount(int fixedRowCount);
    int fixedRowCount() const;

    /**
     * @brief 设置是否延迟刷新
     * @param delay
     */
    void setResizeDelay(bool delay);

    /**
     * @brief 设置是否允许框选拖拽
     * @param value
     */
    void setAllowRangeMoving(bool value);
    bool allowRangeMoving() const;

    /*!
     *判断给定逻辑行是否隐藏，true表示隐藏，false表示显示
     *@param[in]  row  逻辑行号
     *@return bool
     */
    bool isRowHidden(int row) const;

    /*!
     *设置给定逻辑行的隐藏状态
     *@param[in]  row   逻辑行号
     *@param[in]  hide  true表示隐藏，false表示显示
     *@return 无
     */
    void setRowHidden(int row, bool hide);

    /*!
     *判断给定逻辑列是否隐藏，true表示隐藏，false表示显示
     *@param[in]  column  逻辑列号
     *@return bool
     */
    bool isColumnHidden(int column) const;
    /*!
     *设置给定逻辑列的隐藏状态
     *@param[in]  column   逻辑列号
     *@param[in]  hide     true表示隐藏，false表示显示
     *@return 无
     */
    void setColumnHidden(int column, bool hide);

    /*!
     *滚动到给定的index处
     *@param[in]  index
     *@param[in]  hint   滚动方式
     *@return 无
     */
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);

    /*!
     *根据给定的逻辑行列号，返回该格子所处合并格包含的行数
     *@param[in]  row     逻辑行号
     *@param[in]  column  逻辑列号
     *@return int
     */
    int rowSpan(int row, int column) const;

    /*!
     *根据给定的逻辑行列号，返回该格子所处合并格包含的列数
     *@param[in]  row     逻辑行号
     *@param[in]  column  逻辑列号
     *@return int
     */
    int columnSpan(int row, int column) const;

    /*!
     * \brief 根据绝对行列号，返回合并格最左上角格子的QModelIndex（仅限可见区域）
     * \param row
     * \param column
     * \return
     */
    QModelIndex spanAt(int row, int column);

    /**
     * @brief 根据给定的列排序
     * @param column
     * @param order
     */
    void sortByColumn(int column, Qt::SortOrder order);

    /**
     * @brief 框选拖拽时，把原位置的内容清除，填充到新的位置
     * @param src
     * @param dest
     */
    void doRangeMoving(QRect src, QRect dest);

    /**
     * @author duanb
     * @param bCanCopyText 通知用户改选择区域是否可以进行复制
     * @return GString
     * 复制选中内容到剪切板
     */
    GString copyTextFromSelections(bool* bCanCopyText = 0);
    bool pasteFromClipboard();

    /**
     * @brief 把区域转换成一个QModelIndex的List集合
     * @param rect 需要转换的区域
     * @return
     */
    QList<QModelIndex> rangeToList(QRect rect);

    /**
     * @brief 把焦点设置到当前正在处于编辑状态的控件上
     */
    void resetEditorFocus();
    /**
     * @brief 水平表头
     * @return
     */
    GlodonHeaderView *horizontalHeader() const;
    virtual void setHorizontalHeader(GlodonHeaderView *header);

    /**
     * @brief 垂直表头
     * @return
     */
    GlodonHeaderView *verticalHeader() const;
    void setVerticalHeader(GlodonHeaderView *header);

    /**
     * @brief 设置在tableView的右下角显示一个带省略号的button
     * @param value
     */
    void setShowEllipsisButton(bool value);
    bool isShowEllipsisButton();

    /**
     * @brief 设置非编辑状态时，显示编辑方式
     * @return
     */
    GlodonAbstractItemView::EditStyleDrawType editStyleDrawType() const;
    void setEditStyleDrawType(GlodonAbstractItemView::EditStyleDrawType drawType);

    /*外观相关选项*/
    /**
     * @brief 设置是否显示格线
     * @param show
     */
    void setShowGrid(bool show);
    bool showGrid() const;

    /*!
     * \brief 在横向表头或则纵向表头隐藏时，是否绘制边框线
     * \param value
     */
    void setDrawTopAndLeftBorderLine(bool value);
    bool drawTopAndLeftBorderLine();

    /**
     * @brief 设置格线样式(实线，虚线等)
     * @return
     */
    void setGridStyle(Qt::PenStyle style);
    Qt::PenStyle gridStyle() const;

    void setWordWrap(bool on);
    bool wordWrap() const;

    /**
     * @brief 设置选中单元格对应的表头是否高亮显示
     * @return
     */
    void setHighlightSections(bool value);
    bool highlightSections() const;

    /**
     * @brief 设置格线宽度
     * @param value
     */
    void setGridLineWidth(int value);
    int gridLineWidth() const;

    /**
     * @brief 设置格线颜色
     * @param value
     */
    virtual void setGridLineColor(QColor value);
    QColor gridLineColor() const;

    /*!
     * \brief 在表头隐藏，且滚动模式为Item时，绘制的整个表格上边框或者左边框线的颜色
     * \param value
     */
    void setFrameLineColor(QColor value);
    QColor frameLineColor() const;

    /*!
     *设置选择框的颜色
     * \param value
     */
    void setSelectedBoundLineColor(QColor value);
    QColor selectedBoundLineColor() const;

    /*!
     *设置选择框的线宽(默认为2)
     * \param value
     */
    void setSelectedBoundLineWidth(SelectBorderWidth lineWidth);
    int selectedBoundLineWidth() const;

    /*!
     *设置失去焦点，选择框的颜色
     * \param value
     */
    void setNoFocusSelectedBoundLineColor(QColor value);
    QColor NoFocusSelectedBoundLineColor() const;

    /**
     * @brief 设置是否有垂直方向格线
     * @param value
     */
    void setShowVerticalGridLine(bool value);
    bool showVerticalGridLine() const;

    /**
     * @brief 是否有水平方向的格线
     * @return
     */
    void setShowHorizontalGridLine(bool value);
    bool showHorizontalGridLine() const;

    /**
     * @brief 设置表格背景色
     * @param value
     */
    void setGridColor(QColor value);

    /**
     * @brief 设置显示当前选中格子的背景色
     * @param showBackColor
     */
    void setShowCurCellBackgroundColor(bool showBackColor) { m_bShowCurCellBackgroundColor = showBackColor; }
    bool showCurCellBackgroundColor() { return m_bShowCurCellBackgroundColor; }

    /**
     * @brief 设置是否是自定义风格
     * @return
     */
    bool isCustomStyle() const;
    void setIsCustomStyle(bool value);

    /**
     * @brief 根据提供的缩放英子，缩放TableView
     * @param factor
     */
    void zoomTableView(double factor);

    /**
     * @brief 设置焦点格所在的行的背景色都变成被选择时的颜色
     * @param isShow
     */
    void setAlwaysShowRowSelectedBgColor(bool isShow);
    bool alwaysShowRowSelectedBgColor();

    /*!
     *是否允许通过单元格格线改变行高列宽,默认值为false
     *@return 无
     */
    void setAllowResizeCellByDragGridLine(bool canResize);
    bool allowResizeCellByDragGridLine();

    /**
     * @brief 设置回车跳格，如果只开启enterJump时，跳格的执行流程为
     *        1、如果没有进入编辑方式，按Enter—》进入编辑方式-》按Enter-》退出编辑方式并且焦点移动到下一格子-》按Enter
     *           -》进入编辑方式
     *        2、如果已经处于编辑方式，按Enter-》退出编辑方式并且焦点移动到下一个格子
     * @return
     */
    bool enterJump() const;
    void setEnterJump(bool value);

    /**
     * @brief 设置简化版回车跳格，开启enterJump，同时开启enterJumpPro时，跳格的执行流程为
     *        1、如果没有进入编辑方式，按Enter-》进入编辑方式-》按Enter-》退出编辑方式，焦点移动到下一个格子，并且使
     *           下一格子进入编辑方式-》按Enter-》退出编辑方式，焦点移动到下一个格子，并且使下一格子进入编辑方式
     *        2、如果已经处于编辑方式，按Enter-》退出编辑方式，焦点移动到下一个格子，并且使下一格子进入编辑方式
     * @return
     */
    bool enterJumpPro() const;
    void setEnterJumpPro(bool value);

    /**
     * @brief 设置是否使用混合背景色（当前选中格子的背景色为格子本身的颜色的混色，默认为蓝色）
     * @return
     */
    void setUseBlendColor(bool value);
    bool useBlendColor() const;

    /**
     * @brief 设置选择区域的背景颜色
     * @return
     */
    void setSelectedCellBackgroundColor(QColor value);
    QColor selectedCellBackgroundColor() const;

    /*编辑相关*/
    /*!
     *给定的index格子进入编辑状态
     *@param[in]  index
     *@return 无
     */
    void edit(const QModelIndex &index);

    /**
     * @brief 强制退出编辑状态，与按Esc效果一致
     */
    void forceCloseEditor();

    /**
     * @brief 设置是否显示调整列宽，行高时的信息提示框，默认值为true
     * @param value
     */
    void setDisplayResizeInfoFrame(bool value);

    /*!
     * \brief 插入批注
     * \param index
     * \param value
     */
    GCustomCommentFrame *addComment(const QModelIndex &index, QString value);

    /*!
     * \brief 编辑批注
     * \param index
     */
    GCustomCommentFrame *editComment(const QModelIndex &index);

    /*!
     * \brief 显示/隐藏批注
     * \param index
     * \param isShow
     */
    GCustomCommentFrame *showOrHideCommentPersistent(const QModelIndex &index, bool isShow = true);

    /*!
     * \brief 该备注框是否是显示
     * \param index
     * \return
     */
    bool isShowComment(const QModelIndex &index);

    /*!
     * \brief 删除批注
     * \param index
     */
    void deleteComment(const QModelIndex &index);

    /*!
     * \brief 显示或隐藏所有批注
     * \param isShow
     */
    void showOrHideAllCommentPersistent(bool isShow = true);

    /*!
     * \brief 返回当前index的批注框对象
     * \param index
     */
    const GCustomCommentFrame *findComment(const QModelIndex &index) const;

    /**
     * @brief 设置所有标注框是否刷新。该接口是为了避免标注框的连续多次绘制，导致tableview reset时行高失效。
     * 将多次绘制汇聚到最后一次，加快程序效率，也避免了tableview属性失效问题
     * @param enable 参数为真，所有标注框设置为刷新，并且所有标注框立即刷新一次
     */
    void setCommentsUpdatesEnabled(bool enable);

    /**
     * @brief 是否失去焦点退出编辑状态
     * @return
     */
    bool closeEditorOnFocusOut();
    /**
     * @brief 设置是否失去焦点退出编辑状态
     * @param value
     * @param ignoreActiveWindowFocusReason  是否忽略由其他程序引起的失去焦点
     */
    void setCloseEditorOnFocusOut(bool value, bool ignoreActiveWindowFocusReason = false);

    /*!
     * \brief 设置在创建comboBox编辑方式时，是否使用自动提示功能，默认为提示
     * \return
     */
    bool useComboBoxCompleter() const;
    void setUseComboBoxCompleter(bool value);

    /*刷新相关*/

    /*!
     *重置整个表体，重新绘制
     *@return 无
     *@see 参见GlodonAbstractItemView::doItemsLayout()
     */
    void doItemsLayout();

    /**
     * @brief 重新计算自动行高列宽，合适列宽
     */
    virtual void refreshDisplayColRow();
    /**
     * @brief 刷新一个格子(在开启填充或者选择移动功能的时候，黑框不在刷新范围之内)
     * @param logicIndex只在有行列移动时，logicalIndex和visualIndex不同
     */
    void updateModelIndex(const QModelIndex &logicIndex);

    virtual void update(const QModelIndex &logicalIndex);

    /**
     * @brief 刷新一列
     * @param col
     */
    void updateCol(int col);

    /**
     * @brief 刷新一行
     * @param row
     */
    void updateRow(int row);

    /**
     * @brief 刷新整个tableView
     */
    void updateAll();

    /**
     * 更新类型
     */
    void updateHeaderView(const UpdateHeaderViewType updateType);

    /*QModelIndex相关*/
    /*!
     * TODO: delete
     *@return 无
     */
    virtual QModelIndex treeIndex(const QModelIndex &index) const;

    /*!
     *TODO: delete
     *@return 无
     */
    virtual QModelIndex dataIndex(const QModelIndex &index) const;
    /*!
     *TODO: delete
     *@param[in]  pos  坐标点
     *@return QModelIndex
     */
    QModelIndex indexAt(const QPoint &pos) const;

    /**
     * @brief 显示Index转换为逻辑Index
     * @param visualIndex
     * @return
     */
    QModelIndex logicalIndex(QModelIndex visualIndex);
    /**
     * @brief 逻辑Index转换为显示Index
     * @param logicalIndex
     * @return
     */
    QModelIndex visualIndex(const QModelIndex &logicalIndex);

    /*!
     *返回所有被选中的QModelIndex
     *@return QModelIndexList
     */
    QModelIndexList selectedIndexes() const;

    /**
     * @brief 传入一个有行号列号组成的rect，返回一个dataIndex集合
     * @param rect
     * @return
     */
    QModelIndexList indexesFromRect(QRect rect, int &rowCount, int &columnCount);

    /*计算位置相关*/
    /*!
     *根据给定的逻辑行号，返回其绘制位置
     *@param[in]  row  逻辑行号
     *@return int
     */
    int rowViewportPosition(int row) const;

    /**
     * @brief 根据给定的显示行号，返回其绘制位置（减去了滚动条偏移）
     * @param visualRow 显示行号
     * @return
     */
    int rowVisualPosition(int visualRow) const;

    /*!
     *根据给定y坐标，返回该位置的逻辑行号
     *@param[in]  y  y方向坐标
     *@return int
     */
    int rowAt(int y) const;
    /**
     * @brief 根据给定的y坐标，返回该位置的显示行号
     * @param y
     * @return
     */
    int visualRowAt(int y) const;

    /*!
     *设置行高
     *@param[in]  row     逻辑行号
     *@param[in]  height  新行高
     *@param[in]  isManual  是否自定义，影响是否写入Model
     *@return 无
     */
    void setRowHeight(int row, int height, bool isManual = true);

    /*!
     *根据给定逻辑行号，返回行高
     *@param[in]  row  逻辑行号
     *@return int
     */
    int rowHeight(int row) const;

    /**
     * @brief 根据给定显示列号，返回行高
     * @param visualRow 显示列号
     * @return
     */
    int visualRowHeight(int visualRow) const;

    /*!
     *根据给定的逻辑列号，返回其绘制位置
     *@param[in]  column  逻辑列号
     *@return int
     */
    int columnViewportPosition(int column) const;

    /**
     * @brief 根据给定的显示列号，返回其绘制位置（减去了滚动条偏移）
     * @param visualColumn 显示列号
     * @return
     */
    int columnVisualPosition(int visualColumn) const;

    /*!
     *根据给定x坐标，返回该位置的逻辑列号
     *@param[in]  x  x方向坐标
     *@return int
     */
    int columnAt(int x) const;

    /**
     * @brief 根据给定x坐标，返回该位置的显示列号
     * @param x
     * @return
     */
    int visualColumnAt(int x) const;

    /*!
     *设置列宽
     *@param[in]  column  逻辑列号
     *@param[in]  width   新列宽
     *@return 无
     */
    void setColumnWidth(int column, int width);

    /*!
     *根据给定逻辑列号，返回列宽
     *@param[in]  column  逻辑列号
     *@return int
     */
    int columnWidth(int column) const;

    /**
     * @brief 根据给定显示列号，返回列宽
     * @param visualColumn 显示列号
     * @return
     */
    int visualColumnWidth(int visualColumn) const;

    /*!
     *返回给定logicIndex的绘制位置
     *@param[in]  index
     *@return QRect
     */
    QRect visualRect(const QModelIndex &index) const;

    /**
     * @brief 根据显示行列组成的Rect计算出绘制的位置
     * @param rowColNo
     * @return
     */
    QRect visualRectForRowColNo(const QRect &rowColNo);

    /*!
     *设置QItemselectionModel，用于记录当前已选择区域
     *@param[in]  selectionModel
     *@return 无
     *@see 参见GlodonAbstractItemView::setSelectionModel(QItemSelectionModel *selectionModel)
     */
    void setSelectionModel(QItemSelectionModel *selectionModel);

    /*!
     *打印功能，尚未实现
     *@return 无
     */
    void print();

    /*!
     * \brief 设置格线是否分段绘制，关闭这功能，能提高绘制效率
     * \param drawBoundLine
     */
    void setDrawBoundLine(bool drawBoundLine);
    bool drawBoundLine();

public Q_SLOTS:
    /*!
     *选择一行
     *@param[in]  row  逻辑行号
     *@return 无
     */
    void selectRow(int row);

    /*!
     *选择一列
     *@param[in]  column  逻辑列号
     *@return 无
     */
    void selectColumn(int column);

    /*!
     *隐藏一行
     *@param[in]  row  逻辑行号
     *@return 无
     */
    virtual void hideRow(int row);

    /*!
     *隐藏一列
     *@param[in]  column  逻辑列号
     *@return 无
     */
    void hideColumn(int column);

    /*!
     *显示一行
     *@param[in]  row  逻辑行号
     *@return 无
     */
    void showRow(int row);

    /*!
     *显示一列
     *@param[in]  column  逻辑列号
     *@return 无
     */
    void showColumn(int column);

    /*!
     *将给定逻辑行resize到适合内容的高度
     *@param[in]  row  逻辑行号
     *@return 无
     */
    void resizeRowToContents(int row);

    /*!
     * \brief 根据格子内容计算所有行的行高，并应用
     */
    void resizeRowsToContents();

    /*!
     *将给定逻辑行resize到适合内容的高度
     *@param[in]  column  逻辑列号
     *@return 无
     */
    void resizeColumnToContents(int column);

    /*!
     * \brief 根据格子内容计算所有列的列宽，并应用
     */
    void resizeColumnsToContents();

    /*!
     *将所有的行高调整到合适高度
     *@param[in]  calcAllColumns  是否依照所有列进行计算
     *@return 无
     */
    void setSuitRowHeightForAll(bool value, bool calcAllColumns = false);

    /*!
     *将所有的列宽调整到合适宽度
     *@param[in]  calcAllRows  是否依照所有行进行计算
     *@return 无
     */
    void setSuitColWidthForAll(bool value, bool calcAllRows = false);

    void sortByColumn(int column);

    /*!
     *选择连续的逻辑列
     *@param[in]  start  逻辑列号
     *@param[in]  end    逻辑列号
     *@return 无
     */
    void selectColumns(int start, int end);

    /*!
     *选择连续的逻辑行
     *@param[in]  start  逻辑行号
     *@param[in]  end    逻辑行号
     *@return 无
     */
    void selectRows(int start, int end);
    void setCurrentIndex(const QModelIndex &dataIndex);
    /**
     * @brief 通过行号，列号，设置焦点格，改行号，列号为绝对行号列号，没有父子结构
     * @param row
     * @param column
     */
    void setCurrentIndex(int row, int column);
    void onScrolled(int offset);

protected slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void onHeaderDataChanged(Qt::Orientation orientation, int, int);

    void closeEditor(QWidget *editor, bool &canCloseEditor, GlodonDefaultItemDelegate::EndEditHint hint);
    void commitData(QWidget *editor, bool &canCloseEditor);

    void canRowMoved(int from, int to, bool &canMove);
    void rowMoved(int row, int oldIndex, int newIndex);
    void canColumnMoved(int from, int to, bool &canMove);
    void columnMoved(int column, int oldIndex, int newIndex);
    void rowResized(int row, int oldHeight, int newHeight, bool isManual);
    /**
     * @brief ColumnHeaderView调整大小时，会调用该槽，当用了filterTableView,footTableView,GSPTableView
     *        时，会先调用子类的调整大小方法，然后调用该方法，这样就避免了在自动列宽，合适列宽出现filterTableView
     *        大小调整不对的情况
     * @param column
     * @param oldWidth
     * @param newWidth
     */
    virtual void columnResized(int column, int oldWidth, int newWidth, bool isManual = false);
    void rowCountChanged(int oldCount, int newCount);
    void columnCountChanged(int oldCount, int newCount);

    /**
     * @brief 调整行（列）大小时，计算当前行（列）调整的大小，并且画提示框（黑线在paint方法中）
     * @param mousePostion 鼠标的位置
     * @param direction  headerview的方向
     * @param state 当前动作的状态(mousePress, mouseMove, mouseRelease)
     */
    virtual void showSectionResizingInfoFrame(const QPoint &mousePosition, Qt::Orientation direction, GlodonHeaderView::ResizeState state);

Q_SIGNALS:
    void rangeFill(QRect src, QRect dest, bool &handled);
    void rangeFill(QModelIndexList src, QModelIndexList dest, int rowCount, int ColumnCount, bool &handled);

    void expanded(int index);
    void collapsed(int index);

    void onEllipsisButtonClick();

    //for分页表格
    void columnNewWidths(GIntList *newWidths);
    void enableFitColWidths(bool enable);

protected:
    GLDTableView(GLDTableViewPrivate &, QWidget *parent);

    virtual void doSetModel(QAbstractItemModel *model);
    virtual bool canRangeFill(const QRect &rect);
    bool doCanRangeFill(const QRect &rect);

    void scrollContentsBy(int dx, int dy);
    bool isEditorInFixedColAfterHScroll();
    bool isEditorInFixedRowAfterVScroll();

    QStyleOptionViewItem viewOptions() const;

    /*!
     *调整TableView中各个部件的布局
     *@return 无
     */
    void updateGeometries();

    void paintEvent(QPaintEvent *e);
    void timerEvent(QTimerEvent *event);

    virtual void onBoolCellPress(QMouseEvent *event);
    virtual void onMousePress(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

    bool viewportEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void leaveEvent(QEvent *event);
    void wheelEvent(QWheelEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void showEvent(QShowEvent *event);

    int horizontalOffset() const;
    int verticalOffset() const;
    QModelIndex moveCursor(GlodonAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers);

    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    /**
     * @brief 根据实际选择的区域设置selection(实际选择区域与逻辑选择区域在有行列移动时有区别)
     * @param visualTL 实际选择区域的topLeftModelIndex
     * @param visualBR 实际选择区域的bottomRightModelIndex
     * @param command
     */
    void setSelectionByIndex(
            const QModelIndex &visualTL, const QModelIndex &visualBR,
            QItemSelectionModel::SelectionFlags command);

    QRegion visualRegionForSelection(const QItemSelection &selection) const;
    QRegion visualRegionForSelectionWithSelectionBounding(const QItemSelection &selection) const;
    int sizeHintForRow(int row) const;
    int sizeHintForColumn(int column) const;

    int calcSuitHeight(const QList<int> &oNeedCalcHeightCols, int nLogicalRow);
    int calcSuitWidth(int nLogicalCol);

    void verticalScrollbarAction(int action);
    void horizontalScrollbarAction(int action);

    bool isIndexHidden(const QModelIndex &index) const;

    bool inBoolCell(const QPoint pos) const;

    void selectionChanged(const QItemSelection &selected,
                          const QItemSelection &deselected);
    void currentChanged(const QModelIndex &current,
                        const QModelIndex &previous);

    bool isLegalData();
    void setLegalData(bool value);

    bool event(QEvent *event);

    /*!
     * \brief 获取第一个可见行，在有固定行时，获取的是第一个非固定区域的可见行
     * \return
     */
    int firstVisualRow() const;
    int lastVisualRow() const;

    /*!
     * \brief 获取第一个可见列，在有固定列时，获取的是第一个非固定区域的可见列
     * \return
     */
    int firstVisualCol() const;
    int lastVisualCol() const;

    GIntList needCalcSuitRowHeightColsInViewport();

    /**
     * @brief 获取传入的Rect左上角的ModelIndex
     * @param rowColRect
     * @return
     */
    QModelIndex topLeftIndex(const QRect &rowColRect) const;
    /**
     * @brief 获取传入的Rect的右下角的ModelIndex
     * @param rowColRect
     * @return
     */
    QModelIndex bottomRightIndex(const QRect &rowColRect) const;

    void bindMergeCell();

    virtual void rebuildGridRows(const GLDList<QModelIndex> &arrRows);

    /*!
     * TODO: delete
     * \return 为了显示树形的格子的文字区将进行的水平方向的偏移(主要是树干的细线和branch图标占去的宽度)
     */
    virtual int treeCellDisplayHorizontalOffset(int row, int col, bool isOldMinTextHeightCalWay = true) const;
    void doMoveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
                       QItemSelectionModel::SelectionFlags command, MoveReason moveReason);

    virtual QAbstractItemModel* itemModel()
    {
        return model();
    }

    inline int currentGridLineWidth() const;
    inline QPen currentGridPen();

    bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event);
    void syncStateToDelegate(GlodonDefaultItemDelegate *delegate);

    void beforeReset();
    void doReset();
    void afterReset();

    /**
     * @brief 拖拉复制时，填充内容(src，和dest都是由ModelIndex组成，且都是界面显示的位置，重写该方法，实现自己的填充逻辑
     *                          时，需要将显示的位置改为逻辑位置)
     * @param src 原选择区域
     * @param dest 填充的目标区域
     */
    virtual void doRangeFill();

    /*!
     *@brief 整个表体的绘制
     *@param[in]  painter     画笔
     *@param[in]  drawRegion  绘制区域
     *@return 无
     */
    void paint(QPainter &painter, const QRegion &drawRegion);

protected:
    virtual void afterCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
    virtual void afterExpandedChanged(int index, bool expand, bool emitSignal);
    virtual void afterManualChangedRowHeight(int index, int rowHeight);
    virtual void afterManualChangedColWidth(int index, int colWidth);

protected:
    QRect m_oRangeSrc;
    QRect m_oRangeDest;
    QRect m_tempRangeRect;//用来临时存储选择移动时计算出来的Rect
    QRect m_preTempRangeRect;//用来临时存储选择移动时，之前的Rect
    int m_nRangeCurRow;
    int m_nRangeCurCol;
    QRect m_dirtyArea;
    bool m_bShowCurCellBackgroundColor; //是否显示选中区域中当前单元格的背景色

protected:
    void drawCellBorderLine(
            QPainter &painter, const QRect &cellRect, const QRegion &spans,
            int gridLineWidth, int borderLineWidth, const QVariant &borderLineColor, Qt::Orientation direction);

    void drawCellBorderLine(
            QPainter &painter, const QLine& line, bool needDraw);

    virtual void drawCellBorderLine(
            QPainter &painter, const QRect &cellRect, const QRegion &spans,
            const QModelIndex &index);

    /**
     * @brief 重置EllipsisButton的位置
     */
    virtual void resetEllipsisButtonLocation();

    /**
     * @brief
     *      如果水平和垂直表头不可见，则需要在表格的上边和左边绘制框线.

       @details
            我们只需要在滚动相关的函数被调用时更新这两条线即可

     * @param painter
     */
    virtual void drawTopAndLeftBorderLine(QPainter &painter);

    void drawEachRow(QPainter &painter, QRegion &spans, const QRect &params);
    void drawEachColumn(QPainter &painter, QRegion &spans, const QRect &params);

    /**
     * @brief 设置bool值到model中
     * @param currIndex
     */
    void setBoolEditValue(QModelIndex &currIndex);

protected:
    // 重构updateGeometries
    void updateVerticalHeaderGeometry(int nVerticalHeaderWidth);
    void updateHorizontalHeaderGeometry(int nHorizontalHeaderHeight);
    void updateConrnerWidgetGeometry(int nVerticalHeaderWidth, int nHorizontalHeaderHeight);
    void updateHorizontalScrollBar(const QSize &oViewportSize);
    void updateVerticalScrollBar(const QSize &oViewportSize);
    int colCountBackwardsInViewPort(const QSize &oViewportSize);
    int rowCountBackwardsInViewPort(const QSize &oViewportSize);

private:
    int lastVisualRowInDirtyArea(const GlodonHeaderView *verticalHeader) const;
    void adjustCurrentDirtyArea(uint verticalBottom, uint horizontalRight);
    void adjustColumnByLayoutDirection(int &firstVisualColumn, int &lastVisualColumn);
    void drawGridCellFromRowToColumn(QPainter &painter, GlodonTableViewPainterInfo &info, QBitArray &drawn, QStyleOptionViewItem &option);
    void calcTopRowAndAlternateBase(int bottom, int &top, bool &balternateBase);

private:
    /**
     * @brief 绘制各种格子边框
     * @param painter
     * @param params
     * @param spans
     * @param option
     */
    void drawCellBorderLines(QPainter &painter, const QRect &params, QRegion &spans, QStyleOptionViewItem &option);

    /**
     * @brief 按行绘制边框线
     * @param painter
     * @param params
     * @param spans
     * @param option
     */
    void drawBorderLinesByRow(QPainter &painter, const QRect &params, QRegion &spans, QStyleOptionViewItem &option);

    /**
     * @brief 按列绘制边框线
     * @param painter
     * @param params
     * @param spans
     * @param option
     */
    void drawBorderLinesByCol(QPainter &painter, const QRect &params, QRegion &spans, QStyleOptionViewItem &option, int row);

    /**
     * @brief 根据当前状态绘制选择框
     * @param painter
     * @param offset
     */
    void drawSelectionBorderLines(QPainter &painter, const QPoint &offset);

    /**
     * @brief 绘制正常状态（非选择填充，非选择移动）下，选择区域的边框线
     * @param painter
     */
    void drawSelectionBorderLines(QPainter &painter);

    void drawGridLines(QPainter &painter, const QRect &params, QRegion &spans);

    //用于当个单元格右下角的单元格的index
    QModelIndex calculationRightBottomIndex(QModelIndex drawIndex);

    int firstVisableColumn();
    int firstVisableRow();

    void adjustCommonBorder(const QLine &line, QList<QLine> &rectVisablelBorders);
    void adjustCommonBorder(const QLine &lineTopOfCell, QList<QLine> &rectVisablelBorders,
                            const QLine& adjustedLineTopOfCell);
    void adjustCommonBorder(const QRect &rect, QList<QLine> &rectVisablelBorders);

    void appendHideCellBorder(const QRect &rect, QList<QLine> &rectHideBorder);

    /*!
     *@brief 画填充虚线框
     *@param[in]  painter
     *@param[in]  offset  偏移点
     *@return 无
     */
    void drawRangeFillingState(QPainter &painter, const QPoint &offset);

    /**
     * @brief 框选可拖拽时，画拖拽时灰色实线框
     * @param painter
     * @param offset
     */
    void drawRangeMovingState(QPainter &painter);

    QRect initRangeFillingDest();
    /**
     * @brief 判断Point是否处于可以允许移动的范围
     * @param p
     * @return
     */
    bool inRangeMovingRegion(const QPoint &p);

    /**
     * @brief 初始化选择拖拽时，目标位rangeDest的值
     * @return 返回值为false则说明存在合并，且操作不合法，不再执行本次操作的后续动作
     */
    bool initRangeMovingDest();

    /**
     * @brief 获取当前选中区域的矩形大小
     * @return  ModelIndex组合成的Rect
     */
    QRect visualRectForSelection();
    void initRangeAction();
    /**
     * @brief 计算选择移动时的区域
     * @return
     */
    QRect calcRangeMovingRegion();
    /**
     * @brief 选择填充，选择移动时，设置CurrentRow和CurrentColumn
     * @param pos
     */
    void setCurRangeRowAndCol(QPoint pos);
    //void doRangeFill();
    void addNewRow(int newRowCount);
    /**
     * @brief 调整行（列）大小时，设置提示框中的信息
     * @param mousePosition
     * @return
     */
    QString resizeInfoText(QPoint mousePosition, Qt::Orientation);
    /**
     * @brief 根据headerview方向，设置调整行（列）大小时，初始高度（宽度）
     */
    void setResizeStartPosition(Qt::Orientation);

    /**
     * @brief 计算自动列宽
     * @param col
     */
    void setGridDisplayColWidths();

    void refreshDisplayRows(GLDList<QModelIndex> &arrRows);
    void refreshDisplayRows();

    /**
     * @brief 计算自动行高
     * @param cols 需要计算的自动行高的列
     */
    void setGridDisplayRowHeights();

    int getSuitRowHeight(int row);

    /**
     * @brief 计算合适列宽
     * @param col
     */
    void adjustColWidths(int currentResizeCol);

    /**
     * @brief 通过枚举类型Margin,获取top,left,right,bottom的margin
     * @param modelIndex
     * @param margin
     * @return
     */
    int cellMargin(const QModelIndex &modelIndex, Margin margin) const;

    GCustomCommentFrame *findOrCreateCommentFrame(const QModelIndex &index, bool createCommentIfNotFind = true);

    /**
     * @brief 获取右边或者底边线宽
     * @param boundLine
     * @param isRightBoundLine 是否是右边
     * @param compareWithGridLine 是否跟格线比较
     * @return
     */
    int borderLineWidth(const QVariant &boundLine, bool isRightBoundLine) const;

    int borderLineSubtractGridLineWidth(const QVariant &boundLine, bool isRightBoundLine) const;

    /*!
    *@brief 滚动到给定的index处
    *@param[in]  index
    *@param[in]  hint   滚动方式
    *@param[in]  cellHeight 单元格的高度
    *@return 无
    */
    void scrollVerticalTo(const QModelIndex &index, ScrollHint &hint, int nCellHeight);

    /*!
    *@brief 纵向滚动条，按格子滚动，滚动到指定的Index处
    *@param[in]  index
    *@param[in]  hint   滚动方式
    *@param[in]  cellHeight 单元格的宽度
    *@return 无
    */
    void scrollVerticalPerItemTo(const QModelIndex &index, ScrollHint &hint, int nCellHeight);

    /*!
    *@brief 纵向滚动条，按像素滚动，滚动到指定的Index处
    *@param[in]  index
    *@param[in]  hint   滚动方式
    *@param[in]  cellHeight 单元格的宽度
    *@return 无
    */
    void scrollVerticalPerPixelTo(const QModelIndex &index, ScrollHint &hint, int nCellWidth);

    /*!
    *@brief 滚动到给定的index处
    *@param[in]  index
    *@param[in]  hint   滚动方式
    *@param[in]  cellWidth 单元格的宽度
    *@return 无
    */
    void scrollHorizontalTo(const QModelIndex &index, ScrollHint &hint, int nCellWidth);

    /*!
    *@brief 横向滚动条，按格子滚动，滚动到指定的Index处
    *@param[in]  index
    *@param[in]  hint   滚动方式
    *@param[in]  cellWidth 单元格的宽度
    *@return 无
    */
    void scrollHorizontalPerItemTo(const QModelIndex &index, ScrollHint &hint, int nCellWidth);

    /*!
    *@brief 横向滚动条，按像素滚动，滚动到指定的Index处
    *@param[in]  index
    *@param[in]  hint   滚动方式
    *@param[in]  cellWidth 单元格的宽度
    *@return 无
    */
    void scrollHorizontalPerPixelTo(const QModelIndex &index, ScrollHint &hint, int nCellWidth);

    /**
     * @brief 判断选中区域能否进行复制
     * @param[in]  indexList
     */
    bool checkSelectionCopyEnable(const QModelIndexList& indexList) const;

    /**
     * @brief 从一个有序的QModelIndexList中获取所有单元格的内容
     * @param indexList
     * @return
     */
    GString getContentsFromOrderedIndexList(const QModelIndexList & indexList);

    /**
     * @brief 根据剪贴板中内容来填充单元格
     * @param[in]   indexList       被填充的单元格QModelIndex
     * @param[in]   sClipBoardText  剪贴板中内容
     * @param[out]  nPasteRowCount  填充行数
     * @param[out]  nPasteColCount  填充列数
     */
    void fillCellsOnClipBoardText(const QModelIndexList &indexList, GString &sClipBoardText,
                                  int & nPasteRowCount, int & nPasteColCount);

    /**
    * @brief 设置选中区域
    * @param[in]   indexList       被填充的单元格QModelIndex
    * @param[in]   nPasteRowCount  粘贴函数
    * @param[in]   nPasteColCount  粘贴列数
    */
    void setNewSelection(const QModelIndexList &indexList, int & nPasteRowCount, int & nPasteColCount);

    /**
     * @brief 给单元格设值,设值过程中需要考虑单元格的只读性问题,该函数主要用于pasteFromClipboard()函数中
     * @param index    QModelIndex
     * @param variant  需要设定的值
     */
    void setCellData(const QModelIndex &index, const QVariant &variant);

    /*!
     * \brief 返回给定logicIndex的绘制位置，其中包含边框线
     * \param index
     * \return
     */
    QRect visualRectIncludeBoundingLineWidth(const QModelIndex &index) const;

    // MoveCursor重构
    int getBottomVisibleRow();
    int getRightVisibleCol();
    int getLastVisualRow(int nBottom);
    int getLastVisualCol(int nRight);

    QModelIndex leftTopVisualIndex(int nRight, int nBottom);

    void updateVisualCursor();
    void adjustRightToLeftCursorAction(CursorAction cursorAction);

    void dealWithMoveUp(int &nLastVisualRow, int &nLastVisualCol);
    void dealWithMoveDown(int &nLastVisualRow, int &nLastVisualCol, int nBottom);

    QModelIndex getLeftPageCursorIndex();
    void dealWithMoveLeft(int &nLastVisualRow, int &nLastVisualCol, int nRight, int nBottom, CursorAction cursorAction);

    QModelIndex getRightPageCursorIndex(int nRight);
    void dealWithMoveRight(int &nLastVisualRow, int &nLastVisualCol, int nRight, int nBottom, CursorAction cursorAction);

    void dealWithMoveHome(int &nLastVisualRow, int &nLastVisualCol, int nRight, int nBottom, Qt::KeyboardModifiers modifiers);
    void dealWithMoveEnd(int &nLastVisualRow, int &nLastVisualCol, int nRight, int nBottom, Qt::KeyboardModifiers modifiers);

    QModelIndex getUpPageCursorIndex();
    QModelIndex getDownPageCursorIndex(int nBottom);

    QModelIndex getCursorIndex(int nLastVisualRow, int nLastVisualCol);

    bool copyOrPastOperation(QKeyEvent *event);
    bool openEditOrScrollContent(QKeyEvent *event);
    void dealWithKeyEnterPress(QKeyEvent *event);

    void clickEditorButtonAccordingToEditStyleDrawType(QModelIndex index, QMouseEvent *event);
    void processEnterJumpAfterCloseEdit(GlodonDefaultItemDelegate::EndEditHint hint);
    void resetEnterJumpPreState();

    /**
     *@brief 是否需要显示编辑方式的画法
     *@param[in]  testIndex  被测试modelIndex
     *@return true:需要 false:不需要
     */
    bool shouldDoEditorDraw(const QModelIndex &testIndex) const;

    // ScrollTo重构
    int firstVisualIndexAfterHorizontalScroll(const QModelIndex &index, ScrollHint &hint, int nCellWidth);
    int getHorizontalHiddenSectionCountBeforeIndex(int nHorizontalIndex);

    bool isPositionAtTopForRowExchange(int nVerticalPosition, int nVerticalOffset);
    int firstVisualIndexAfterVerticalScroll(const QModelIndex &index, ScrollHint &hint, int nCellHeight);
    int getVerticalHiddenSectionCountBeforeIndex(int nVerticalIndex);

    /*!
    *设置合并格
    *@param[in]  row         合并格左上角逻辑行号
    *@param[in]  column      合并格左上角逻辑列号
    *@param[in]  rowSpan     合并格包含的行数，即该合并格的纵向包含的格子数
    *@param[in]  columnSpan  合并格包含的列数，即该合并格的横向包含的格子数
    *@return 无
    */
    void setSpan(int row, int column, int rowSpan, int columnSpan);

    /*!
     *清除所有合并
     *@return 无
     */
    void clearSpans();

    /**
     * @brief 此函数主要修复在开启失去焦点退出编辑状态时,在与onCommitEditor信号相连接的槽中
     *        若将canCloseEditor设置为false,滚动条会自动滚动的问题
     */
    void processScrollBarAfterCanNotCloseEditor();

private:
    // 重构ScrollContentsBy
    void scrollHorizontalContentsBy(int dx);
    void scrollVerticalContentsBy(int dy);
    void updateFirstLine(int dx, int dy);

    bool commitDataAndCloseEditorInIngnoreFocusOut();

    // 重构MousePressEvent
    void dealWithSpecialMousePressOperations(const QModelIndex &index, QMouseEvent *event);
    void dealWithResizeCellWhenPressGridLine(QMouseEvent *event);
    bool shouldDoRangeFill(QMouseEvent *event);
    bool shouldDoRangeMove(QMouseEvent *event);
    bool shoudDoResizeCellByDragGridLine(QMouseEvent *event);

    // 重构MouseMoveEvent
    bool shouldSetRangeFillMouseShape(const QPoint &pos);
    void setRangeFillMouseShape(bool bHasCursor);
    void getRangeFillingCurRowAndCol(int nOldRow, int nOldColumn, const QPoint &pos);
    bool shouldSetRangeMoveMouseShape(const QPoint &pos);
    void setRangeMoveMouseShape(bool bHasCursor);
    void updateTempRangeRect();
    bool shouldSetResizeCellByDragGridLineMouseShape(QMouseEvent *event);
    void setResizeCellByDragGridLineMouseShape(QMouseEvent *event);
    void dealWithResizeCellWhenDragGridLine(QMouseEvent *event);
    void recoverCursor();
    void dealWithCurCommet(const QPoint &pos);
    void dealWithRangeFillingOnMouseMove(bool bHasCursor, const QPoint &pos);
    void dealWithRangeMovingOnMouseMove(const QPoint &pos);
    bool isInRangeFilling();
    bool isInRangeMoving();
    bool isInResizingCellByDragGridLine(QMouseEvent *event);
    void updateRangeFillingRect(int nOldRow, int nOldColumn);

    // 重构mouseReleaseEvent
    void completeRangeFilling();
    void completeRangeMoving();
    void completeResizeCellByDragGridLine(QMouseEvent *event);
    void initRangeValuesAndUpdateViewport();

    // 重构doRangeFill
    bool doRangeFillHandled();
    void doRangeFillToTop();
    void doRangeFillToBottom();
    void doRangeFillToLeft();
    void doRangeFillToRight();
    void setSrcIndexDataToDestIndex(const QModelIndex &oSrcLogicalIndex, QModelIndex &oDestLogicalIndex);

    // 重构SectionResizing
    void initSectionResizingInfoFrame(const QPoint &mousePostion, Qt::Orientation direction);
    void updateSectionResizingInfoFrameText(Qt::Orientation direction);

    // 重构bindMergeCell
    void adjustVisualRect(int &nFirstVisualRow, int &nFirstVisualCol, int &nLastVisualRow, int &nLastVisualCol);
    void adjustVisualRow(int &nFirstVisualRow, int &nLastVisualRow) const;
    void adjustVisualCol(int &nFirstVisualCol, int &nLastVisualCol) const;

    void calcAndSetSpan(const int nFirstVisualRow, const int nLastVisualRow, const int nFirstVisualCol, const int nLastVisualCol);
    void initMergeCellState(const int nFirstVisualRow, const int nLastVisualRow, const int nFirstVisualCol, const int nLastVisualCol,
            QVector<QBitArray> &oCellMergeState);
    int findLastSpanRow(
            const int nCurMergeID, const int nRow, const int nLastVisualRow, const int nCurLogicalCol);
    int findLastSpanCol(const int nCurMergeID, const int nCol, const int nLastVisualCol, const int nCurLogicalRow);
    void setMergeCellState(int nRow, int nSpanRow, int nCol, int nSpanCol, QVector<QBitArray> &oCellMergeState);

    void initResizeInfoFrames(Qt::Orientation direction);

private Q_SLOTS:
    void _q_selectRow(int row);
    void _q_selectColumn(int column);
    void _q_selectRows(int left, int top, int right, int bottom);
    void _q_selectColumns(int left, int top, int right, int bottom);
    void _q_updateSpanInsertedRows(const QModelIndex &parent, int start, int end);
    void _q_updateSpanInsertedColumns(const QModelIndex &parent, int start, int end);
    void _q_updateSpanRemovedRows(const QModelIndex &parent, int start, int end);
    void _q_updateSpanRemovedColumns(const QModelIndex &parent, int start, int end);

    void writeComment(const QString &value);
    void onDrawComment(const QModelIndex &index, bool &canShow);
protected Q_SLOTS:
    void resetCommentPosition(bool isMove = false, int dx = 0, int dy = 0);
private:
    Q_DECLARE_PRIVATE(GLDTableView)
    Q_DISABLE_COPY(GLDTableView)

private:
    friend class GlodonTableViewToExcel;
    friend class GlodonDefaultItemDelegate;
};

class GlodonScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    explicit GlodonScrollBar(QWidget *parent=0);
    explicit GlodonScrollBar(Qt::Orientation orientation, QWidget *parent=0);

protected:
    void enterEvent(QEvent *event);

private:
    QWidget *m_pParent;
};

