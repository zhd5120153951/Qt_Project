/*!
 *@file glodonheaderview.h
 *@brief {表头}
 *
 *@author Gaosy
 *@date 2012.9.7
 *@remarks {}
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDHEADERVIEW_H
#define GLDHEADERVIEW_H

#include <QBitArray>
#include <QApplication>
#include <QLabel>
#include "GLDAbstractItemView.h"

class GLDHeaderSpan;
class GLDTableView;
class GlodonTableView;
class GlodonHeaderViewPrivate;

/*!
 *@class: GlodonHeaderView
 *@brief {默认表头，仅支持单行，表头显示内容从model中的headerData中获取}
 *@author Gaosy
 *@date 2012.9.10
 */
class GLDTABLEVIEWSHARED_EXPORT GlodonHeaderView : public GlodonAbstractItemView
{
    Q_OBJECT

    Q_PROPERTY(bool showSortIndicator READ isSortIndicatorShown WRITE setSortIndicatorShown)
    Q_PROPERTY(bool highlightSections READ highlightSections WRITE setHighlightSections)
    Q_PROPERTY(bool cascadingSectionResizes READ cascadingSectionResizes WRITE setCascadingSectionResizes)
    Q_PROPERTY(int defaultSectionSize READ defaultSectionSize WRITE setDefaultSectionSize)
    Q_PROPERTY(int minimumSectionSize READ minimumSectionSize WRITE setMinimumSectionSize)
    Q_PROPERTY(Qt::Alignment defaultAlignment READ defaultAlignment WRITE setDefaultAlignment)
    Q_ENUMS(ResizeMode)

public:
    /*!
     *@enum: ResizeModel
     *@brief {resize方式}
     *@author Gaosy
     *@date 2012.9.11
     */
    enum ResizeMode
    {
        Interactive,
        Fixed
    };

    enum VisibleState
    {
        LUnvisibleRVisible = -1,
        LVisibleRUnvisible = 1,
        BothVisible = 2,
        BothUnvisible = 0
    };

    enum ResizeState
    {
        Press,
        Move,
        Finish
    };

public:
    /*!
     *GlodonHeaderView构造方法
     *@param[in]  orientation  表头方向，Qt::Horizontal表示水平；Qt::Vertical表示竖直
     *@param[in]  parent       父widget
     *@return 无
     */
    explicit GlodonHeaderView(Qt::Orientation orientation, QWidget *parent = 0);
    virtual ~GlodonHeaderView();

    /*!
     *为表头提供数据来源
     *@param[in]  model
     *@return 无
     *@see 参见GlodonAbstractItemView::setModel(QAbstractItemModel *model)
     */
    void setModel(QAbstractItemModel *model);

    /*!
     *返回表头的方向
     *@return Qt::Orientation
     */
    Qt::Orientation orientation() const;

    /*!
     *返回表头的偏移
     *@return int
     */
    virtual int offset() const;

    /*!
     *返回表头的长度
     *@return int
     */
    virtual int length() const;

    /*!
     *返回水平（竖直）表头的高度（宽度）
     *@return int
     */
    virtual int drawWidth() const;

    /*!
     *返回水平（竖直）表头的列（行）数
     *@return int
     */
    int sectionCount() const;

    /*!
     *返回根据sizeHint计算的表头大小
     *@return QSize
     */
    QSize sizeHint() const;

    /**
     * @brief Qt5重新实现
     * @param v
     */
    void setVisible(bool v);

    /*!
     *根据sizeHint计算给定行（列）表头的宽度（高度）
     *@param[in]  logicalIndex  逻辑列（行）号
     *@return int
     */
    int sectionSizeHint(int logicalIndex) const;

    /*!
     *根据给定的坐标位置，返回虚拟列（行）号，即绘制时的列（行）号
     *@param[in]  position  水平（竖直）表头的x轴（y轴）坐标
     *@return int
     */
    int visualIndexAt(int position) const;

    /*!
     *根据给定的坐标位置，返回逻辑列（行）号，即model中的列（行）号
     *@param[in]  position  水平（竖直）表头的x轴（y轴）坐标
     *@return int
     */
    int logicalIndexAt(int position) const;

    int logicalIndexAt(int x, int y) const
    {
        return orientation() == Qt::Horizontal ? logicalIndexAt(x) : logicalIndexAt(y);
    }

    int logicalIndexAt(const QPoint &pos) const
    {
        return logicalIndexAt(pos.x(), pos.y());
    }

    /*!
     *返回给定列（行）的宽度（高度）
     *@param[in]  logicalIndex  逻辑列（行）号
     *@return int
     */
    int sectionSize(int logicalIndex) const;
    /**
     * @brief 返回给定列（行）的宽度（高度）
     * @param visualIndex 显示列（行）号
     * @return
     */
    int visualSectionSize(int visualIndex)const;

    /*!
     *返回给定列（行）的绝对起始位置
     *@param[in]  logicalIndex  逻辑列（行）号
     *@return int
     */
    int sectionPosition(int logicalIndex) const;

    /*!
     *返回给定列（行）的相对起始位置，即绘制起始位置
     *@param[in]  logicalIndex  逻辑列（行）号
     *@return int
     */
    int sectionViewportPosition(int logicalIndex) const;

    /**
     * @brief 返回给定列（行）的相对起始位置，即绘制起始位置
     * @param visualIndex 显示列（行）号
     * @return
     */
    int sectionVisualPosition(int visualIndex) const;

    /*!
     *将from列（行）移动到to列（行）位置
     *@param[in]  from  起始列的虚拟列（行）号
     *@param[in]  to    目标的虚拟列（行）号
     *@return 无
     */
    void moveSection(int from, int to);

    /*!
     *交换两列（行）的位置
     *@param[in]  first   要交换的虚拟列（行）号
     *@param[in]  second  另一个要交换的虚拟列（行）号
     *@return 无
     */
    void swapSections(int first, int second);

    /*!
     *调整给定列（行）的列宽（行高）
     *@param[in]  logicalIndex  逻辑列（行）号
     *@param[in]  size          新的列宽（行高）
     *@return 无
     */
    virtual void resizeSection(int logicalIndex, int size, bool update = true, bool isManual = false);

    /**
     * @brief 设置headerView的SectionSpan的创建方式，默认为false
     * @param value
     */
    void setSectionSpanSeparate(bool value);
    bool isSectionSpanSeparate();

    /*!
     *判断给定列（行）是否被隐藏(特指collapsed)
     *@param[in]  logicalIndex  逻辑列（行）号
     *@return bool
     */
    bool isSectionHidden(int logicalIndex) const;

    /*!
     *判断给定列（行）是否可见（即区分处于以滚动条为代表的视窗内还是视窗外）
     *@param[in]  logicalIndex  逻辑列（行）号
     *@return int -1:左(上)端不可见，右(下)端可见。0：不可见。1:左(上)端可见，右(下)端不可见。2:双端可见。3:预留定义。详见VisibleState型枚举值
     */
    inline bool isSectionVisible(const int &logicalIndex) const
    {
        return visibleState(logicalIndex) != BothUnvisible;
    }

    VisibleState visibleState(int logicalIndex) const;

    /*!
     *隐藏（显示）给定列（行）
     *@param[in]  logicalIndex  逻辑列（行）号
     *@param[in]  hide          true表示隐藏，false表示显示
     *@return bool
     */
    void setSectionHidden(int logicalIndex, bool hide);

    /*!
     * \brief 批量显示或隐藏section，只进行一次recalcSectionStartPos
     * \param logicalIndexs
     * \param hide
     */
    void batchSetSectionHidden(QSet<int> logicalIndexs, bool hide);

    /*!
     *返回隐藏的列（行）数
     *@return int
     */
    int hiddenSectionCount() const;

    /*!
     *隐藏列（行）
     *@param[in]  logicalIndex  逻辑列（行）号
     *@return 无
     */
    void hideSection(int logicalIndex)
    {
        setSectionHidden(logicalIndex, true);
    }

    /*!
     *显示列（行）
     *@param[in]  logicalIndex  逻辑列（行）号
     *@return 无
     */
    void showSection(int logicalIndex)
    {
        setSectionHidden(logicalIndex, false);
    }

    /*!
     *返回水平（竖直）表头的列（行）数
     *@return int
     */
    virtual int count() const;

    /*!
     *返回给定逻辑列（行）的虚拟列（行）号，即返回其绘制时的列（行）号
     *@param[in]  logicalIndex  逻辑列（行）号
     *@return int
     */
    int visualIndex(int logicalIndex) const;

    /*!
     *返回给定虚拟列（行）的逻辑列（行）号，即返回其在model中对应的列（行）号
     *@param[in]  visualIndex  虚拟列（行）号
     *@return int
     */
    int logicalIndex(int visualIndex) const;

    void setMovable(bool movable);
    bool isMovable() const;

    void setClickable(bool clickable);
    bool isClickable() const;

    void setHighlightSections(bool highlight);
    bool highlightSections() const;

    /*!
     *设置固定格背景色
     *@param[in]  index  列（行）号
     *@param[in]  value  填充的背景色
     */
    void setFixedCellColor(const QColor &value, int index = -1);
    void inserFixedCellColor(const QColor &value, int index);
    void setFixedCellEvent(bool value);
    bool isFixedcellEvent() const;

    void setGridLineWidth(int value);
    void setGridLineColor(QColor value);
    void setResizeMode(ResizeMode mode);
    void setResizeMode(int logicalIndex, ResizeMode mode);
    ResizeMode resizeMode() const;
    ResizeMode resizeMode(int logicalIndex) const;

    void setSortIndicatorShown(bool show);
    bool isSortIndicatorShown() const;

    void setSortIndicator(int logicalIndex, Qt::SortOrder order);
    int sortIndicatorSection() const;
    void setSortIndicatorSection(int logicalIndex);
    Qt::SortOrder sortIndicatorOrder() const;
    void setSortIndicatorOrder(Qt::SortOrder order);

    bool cascadingSectionResizes() const;
    void setCascadingSectionResizes(bool enable);

    int defaultSectionSize() const;
    void setDefaultSectionSize(int size);

    int minimumSectionSize() const;
    void setMinimumSectionSize(int size);

    bool isTree() const;

    /*!
     *设置是否显示树形结构，仅对竖直表头有意义
     *由于会显示（隐藏）子节点，所以会更新行数
     *同时，会导致之前设置的行高，无效
     *@param[in]  showTree
     */
    void setIsTree(bool showTree);

    bool autoSectionData() const;

    /*!
     *设置是否自动填充列（行）号
     *@param[in]  autoData
     */
    void setAutoSectionData(bool autoData);

    int fixedCount() const;

    /*!
     *设置固定编辑列（行）数
     *@param[in]  fixedCount
     */
    virtual void setFixedCount(int fixedCount);

    Qt::Alignment defaultAlignment() const;
    void setDefaultAlignment(Qt::Alignment alignment);

    /*!
     *重置表头
     *@return 无
     */
    void doItemsLayout();

    /*!
     *返回是否有列（行）移动过
     *@return bool
     */
    bool sectionsMoved() const;

    /*!
     * @brief 清除列移动的信息
     */
    void clearSectionMove();

    /*!
     *返回是否有列（行）隐藏
     *@return bool
     */
    bool sectionsHidden() const;

    /*!
     *返回指定坐标的sectionHandle
     *@return int
     */
    int getSectionHandleAt(int position);

    QModelIndex indexAt(const QPoint &p) const;
    bool isIndexHidden(const QModelIndex &index) const;
    bool isAllSectionHidden() const;

    void paintEvent(QPaintEvent *e);
    bool viewportEvent(QEvent *e);
    virtual void updateGeometries();
    void currentChanged(const QModelIndex &current, const QModelIndex &old);
    /**
     * @brief 调整行（列），高（框）时，返回鼠标的位置
     * @return
     */
    QPoint mousePosition();
    void setMousePosition(QPoint pos);

    void setResizeDelay(bool delay);

#ifndef QT_NO_DATASTREAM
    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);
#endif

    /*!
     *重置表头
     *@return 无
     *@see 参见GlodonAbstractItemView::reset();
     */
    void reset();

    /*!
     *为分组模式重置表头
     *@return 无
     *@see 参见GlodonAbstractItemView::reset();
     */
    inline void resetForGroup()
    {
        initializeSections();
    }

    void resetAfterTreeBuild(const QVector<int> &showRows);

    /**
     * @brief 重置表头，即使sectionCount没有变化
     */
    void resetSectionItems();

    /**
     * @brief 根据缩放因子，缩放headerView的大小
     */
    void zoomInSection(double factor);

    void setInZoomingSection(bool zooming);
    bool inZoomingSection() const;

    /**
     * @brief 设置是表头移动的数据是否写入到model中
     * @brief 暂时需要外部截获canSectionMove信号进行处理
     */
    void setLogicalSectionMovable(bool isManual);
    bool isLogicalSectionMovable() const;

    /**
     * @brief 设置是否显示边框
     * @return
     */
    bool drawBorder() const;
    void setDrawBorder(bool value);

    /**
     * @brief 设置是否允许双击事件
     * @return
     */
    bool allowDoubleClicked() const;
    void setAllowDoubleClicked(bool value);

    /**
     * @brief 选中时，字体是否加粗显示
     * @return
     */
    bool boldWithSelected();
    void setBoldWithSelected(bool value);

    /**
    * @brief 字体是否带有白色阴影
    * @return
    */
    bool isFontShadow() const;
    void setFontShadow(bool fontShadow);

    QColor fontShadowColor() const;
    void setFontShadowColor(const QColor &clr);

    /**
     * @brief isHeaderLinearGradient 判断表头是否带有渐变色
     * @return
     */
    bool isHeaderLinearGradient() const;
    void setHeaderLinearGradient(bool value);

    /**
     * @brief 设置排序列的箭头是否在文字的上方
     * @param bIsSortArrowOnTextUp
     */
    void setSortArrowOnTextTop(bool bIsSortArrowOnTextTop);

    void setLogicalIndexImage(QMultiHash<int, QString> logicalIndexHash, int spanTop);

    /*!
     * \brief 批量reSizeSection过程中只会重新设置SectionItem中的size，最后只调用一次recalcSectionStartPos
     * \param logicalIndexSizes key为sectionIndex，value为size
     */
    void batchResizeSection(const QMap<int, int> &logicalIndexSizes);

public Q_SLOTS:
    virtual void setOffset(int offset);
    void setOffsetToSectionPosition(int visualSectionNumber);
    void setOffsetToLastSection();
    void headerDataChanged(Qt::Orientation orientation, int logicalFirst, int logicalLast);
    void setDrawWidth(int drawWidth);

Q_SIGNALS:
    void sectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
    void sectionResized(int logicalIndex, int oldSize, int newSize, bool isManual = false);
    void sectionPressed(int logicalIndex);
    void sectionClicked(int logicalIndex);
    void sectionEntered(int logicalIndex);
    void sectionDoubleClicked(int logicalIndex);
    void sectionCountChanged(int oldCount, int newCount);
    void sectionHandleDoubleClicked(int logicalIndex);
    void sectionAutoResize(int logicalIndex, GlodonHeaderView::ResizeMode mode);

    void geometriesChanged();
    void sortIndicatorChanged(int logicalIndex, Qt::SortOrder order);
    void scrolled(int offset);
    void drawWidthChanged(int newWidth);
    void canSectionMove(int from, int to, bool &canMove);
    /**
     * @brief 调整行（列）大小时，发型号，通知tableview画黑线
     * @param mousePosition  鼠标的位置
     * @param direction  headview的方向
     * @param state  当前处于什么状态（mousepress,mousemove, mouserelease）
     */
    void sectionResizing(QPoint &mousePosition, Qt::Orientation direction, GlodonHeaderView::ResizeState state);

protected Q_SLOTS:
    void updateSection(int logicalIndex);
    void sectionsInserted(const QModelIndex &parent, int logicalFirst, int logicalLast);
    void sectionsAboutToBeRemoved(const QModelIndex &parent, int logicalFirst, int logicalLast);

protected:
    GlodonHeaderView(GlodonHeaderViewPrivate &dd, Qt::Orientation orientation, QWidget *parent = 0);

    void updateTargetSectionRecord(int pos);
    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags);

    void scrollTo(const QModelIndex &index, ScrollHint hint);
    void scrollContentsBy(int dx, int dy);

    void rowsInserted(const QModelIndex &parent, int start, int end);

    /**
     * @brief 绘制排序列的表头小三角
     * @param painter
     * @param opt
     * @param widget
     */
    void paintSectionSortArrow(QPainter *painter, QStyleOptionHeader &opt, const QWidget *widget) const;

    /*!
     * @brief    基于Qt绘制函数绘制表头section部分
     * @param    [i]painter
     * @param    [i]opt
     * @param    [i]widget
     * @return   void
     */
    void paintSectionOnQt(QPainter *painter, QStyleOptionHeader &opt, const QWidget *widget, bool bHighLight) const;
    void paintHighLightSection(QPainter *painter, QStyleOptionHeader &opt, const QRect &rect) const;
    void paintEmptySection(QPainter *painter, const QRect &rect);

    void mouseReleaseEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);

    inline bool isInValidLogicalIndex(const int &logicalIndex) const
    {
        return logicalIndex < 0 || logicalIndex >= count();
    }

    void initStyleOption(QStyleOptionHeader *option) const;
    void initializeSections(int start, int end);
    void initializeSections();
    void initialize();

    void fillFixedCell(QPainter *painter, QStyleOptionHeader &opt, QRect rect, int logicalIndex) const;
    void drawCellBorder(QPainter *painter, QStyleOptionHeader &opt) const;

    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    virtual void paintSection(int beginToHideIndex) const;

    virtual QSize sectionSizeFromContents(int logicalIndex) const;

    QRegion visualRegionForSelection(const QItemSelection &selection) const;

    QRect visualRect(const QModelIndex &index) const;
    QRect textRect(const QRect &rect) const;

    QModelIndex moveCursor(CursorAction, Qt::KeyboardModifiers);

    int verticalOffset() const;
    int horizontalOffset() const;

    bool event(QEvent *e);

    void drawImageInDesignatedLogicalIndex(QPainter *painter, QStyleOptionHeader &supopt) const;

private Q_SLOTS:
    void _q_sectionsRemoved(const QModelIndex &parent, int logicalFirst, int logicalLast);
    void _q_layoutAboutToBeChanged();

private:
    bool m_isSortArrowOnTextTop;

private:
    friend class GLDTableView;
    friend class GLDTableViewPrivate;
    Q_DECLARE_PRIVATE(GlodonHeaderView)
    Q_DISABLE_COPY(GlodonHeaderView)
};

struct GlodonCellAttr
{
public:
    Qt::Orientation orientation;
    QColor background;
};

class GLDTABLEVIEWSHARED_EXPORT GlodonHHeaderView : public GlodonHeaderView
{
    Q_OBJECT
public:
    explicit GlodonHHeaderView(QWidget *parent = 0);
    ~GlodonHHeaderView();

};

class GLDTABLEVIEWSHARED_EXPORT GlodonVHeaderView : public GlodonHeaderView
{
    Q_OBJECT
public:
    explicit GlodonVHeaderView(QWidget *parent = 0);
    ~GlodonVHeaderView();

};

class GLDTABLEVIEWSHARED_EXPORT GlodonHHeaderViewGraphicsEffect : public QGraphicsEffect
{
public:
    GlodonHHeaderViewGraphicsEffect(GlodonHeaderView *header)
        : QGraphicsEffect(header)
        , m_header(header)
        , m_clrStart("#A5AAB3")
        , m_clrMid("#A5AAB3")
        , m_clrEnd("#A5AAB3")
        , m_shadowHeight(3)
    {
        m_clrMid.setAlpha(40);
        m_clrEnd.setAlpha(20);
    }
    virtual void draw(QPainter *painter);

    virtual QRectF boundingRectFor(const QRectF &rect) const;

    void setStartColor(const QColor &color)
    {
        m_clrStart = color;
    }
    void setMidColor(const QColor &color)
    {
        m_clrMid = color;
    }
    void setEndColor(const QColor &color)
    {
        m_clrEnd = color;
    }

    void setShadowHeight(int height)
    {
        m_shadowHeight = height;
    }

private:
    GlodonHeaderView *m_header;
    QColor m_clrStart;
    QColor m_clrMid;
    QColor m_clrEnd;
    int    m_shadowHeight;
};

#endif // GLDHEADERVIEW_H
