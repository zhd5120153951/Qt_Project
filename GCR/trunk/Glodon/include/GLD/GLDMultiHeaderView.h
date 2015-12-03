/*!
 *@file glodonmultiheaderview.h
 *@brief {树型表头}
 *通过解析Model中的HeaderData，用‘|’分割父子关系
 *@author Gaosy
 *@date 2012.9.7
 *@remarks {remarks}
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDMULTIHEADERVIEW_H
#define GLDMULTIHEADERVIEW_H

#include "GLDHeaderView.h"
#include "GLDObjectList.h"

class GlodonMultiHeaderView;
class GlodonMultiHeaderViewPrivate;
class GHeaderSpanCollection;

/*!   
 *@enum: HeaderSpanRelation
 *@brief {两个HeaderSpan之间的位置关系，用于初始化表头}
 *@author Gaosy 
 *@date 2012.9.10
 */
enum HeaderSpanRelation
{
    Contain,
    Overlapping,
    AdjacentUp,
    AdjacentDown,
    AdjacentLeft,
    AdjacentRight,
    AdjacentIrrelevant,//相邻,但是高度或者宽度不一样
    Irrelevant
};

/*!   
 *@class : HeaderSpan
 *@brief {表头合并块儿，记录当前表头块显示的内容、是否选中及表头块的大小}
 *@author Gaosy 
 *@date 2012.9.10
 */
class GLDTABLEVIEWSHARED_EXPORT GLDHeaderSpan
{
public:
    GLDHeaderSpan()
        : m_top(-1), m_left(-1), m_bottom(-1), m_right(-1), m_section(-1), m_selected(false) {}

    GLDHeaderSpan(int row, int column, int rowCount, int columnCount, QString text, int section)
        : m_top(row), m_left(column), m_bottom(row + rowCount - 1),
          m_right(column + columnCount - 1), m_text(QString(text)),
          m_section(section), m_selected(false) {}

    inline int top() const { return m_top; }
    inline int left() const { return m_left; }
    inline int bottom() const { return m_bottom; }
    inline int right() const { return m_right; }
    inline int height() const { return m_bottom - m_top + 1; }
    inline int width() const { return m_right - m_left + 1; }

    inline void setTop(int top) { m_top = top; }
    inline void setLeft(int left) { m_left = left; }
    inline void setBottom(int bottom) { m_bottom = bottom; }
    inline void setRight(int right) { m_right = right; }

    inline QString text() const { return m_text; }
    inline void setText(QString text) { m_text = text; }
    inline int section() const { return m_section; }
    inline void setSection(int section) { m_section = section; }
    inline bool selected() const { return m_selected; }
    inline void setSelected(bool selected) { m_selected = selected; }

    /*!
     *判断该表头合并块与另一个合并块之间的位置关系
     *@param[in]  span  另一个表头合并块
     *@return HeaderSpanRelation
     */
    HeaderSpanRelation spanRelation(GLDHeaderSpan span);

private:
    int m_top;
    int m_left;
    int m_bottom;
    int m_right;
    QString m_text;
    int m_section;
    bool m_selected;
};

/*!   
 *@class: GHeaderSpanCollection
 *@brief {一个表头中所有表头合并块的集合}
 *@author Gaosy 
 *@date 2012.9.10
 */
class GLDTABLEVIEWSHARED_EXPORT GHeaderSpanCollection
{
public:
    GHeaderSpanCollection() {}

    ~GHeaderSpanCollection()
    {
    }

    /*!
     *添加一个合并块，主要在初始化合并表头的时候调用
     *@param[in]  span  
     *@return 无
     */
    void addSpan(GLDHeaderSpan *span);

    /*!
     *通过行列号获取该位置的合并块在集合中的下标
     *@param[in]  rowIndex  逻辑行号（model中的行号）
     *@param[in]  colIndex  逻辑列号（model中的列号）
     *@return 无
     */
    int spanAt(int rowIndex, int colIndex) const;

public:
    //所有表头合并块的集合
    GObjectList<GLDHeaderSpan *> spans;

private:
    void parseSpanInternal(int index, bool isInRecursion, int i);
    bool parseSpanRelationRecursively(GLDHeaderSpan *span, bool isInRecursion, int index);
};

/*!   
 *@class: GlodonMultiHeaderView
 *@brief {树型表头，通过解析Model中的HeaderData，用‘|’分割父子关系}
 *@author Gaosy 
 *@date 2012.9.10
 */
class GLDTABLEVIEWSHARED_EXPORT GlodonMultiHeaderView : public GlodonHeaderView
{
    Q_OBJECT

public:
    GlodonMultiHeaderView(Qt::Orientation orientation, QWidget *parent = 0);
    virtual ~GlodonMultiHeaderView();

    /*!
     *重新设置一列（行）的宽度（高度）
     *@param[in]  logicalIndex  逻辑列号（行号）
     *@param[in]  size          新的列宽（行高）
     *@return 无
     */
    void resizeSection(int logicalIndex, int size, bool update = true, bool isManual = false);

    /*!
     *为表头提供数据来源，每一个合并块中显示的内容通过model中的headerData()获取，进行拆分
     *根据‘|’划分父子关系，形成树形结构表头
     *                                          |_____a_____|
     *例如:第一列为‘a|b’，第二列为‘a|c’，解析后效果为  |  b  |  c  |
     *  
     *@param[in]  model  提供数据的model
     *@return 无
     */
    void setModel(QAbstractItemModel *model);

    /*!
     *获取当前表头额外行（列）数，即总行（列）数减1
     *@return int
     */
    int extraCount() const;

    /*!
     *获取水平（竖直）表头的高度（宽度）
     *@return int
     */
    int drawWidth() const;

    /*!
     *刷新表头
     *@return 无
     @see 参见GlodonHeaderView::updateGeometries()
     */
    void updateGeometries();

    /*!
     *GlodonMultiHeaderView的绘制方法
     *@param[in]  e  
     *@return 无
     */
    void paintEvent(QPaintEvent *e);

    /*!
     *初始化表头，根据固定编辑行（列）与表头的对应关系，检查合法性
     *若出现某一个父合并块的部分子合并块为固定行（列），而部分不是
     *则返回false
     *@return bool
     */
    void initTitles();

    /*!
     *根据给定的坐标点，返回相应的合并块在其集合中的下标
     *@param[in]  point  坐标位置
     *@return int
     */
    int headerSpanAt(QPoint point);

    /*!
     *根据给定的合并块下标，返回其虚拟位置（实际绘制的位置）
     *@param[in]  index  合并块在其集合中的下标
     *@return HeaderSpan
     */
    GLDHeaderSpan visualSpan(int index);

    /*!
     *设置固定编辑行（列）数，会进行合法性判断
     *若出现某一个父合并块的部分子合并块为固定行（列）
     *而一部分不是，则发出Assert
     *@param[in]  fixedCount
     *@return 无
     */
    void setFixedCount(int fixedCount);

    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

    /*!
     *根据给定的合并块下标，返回其坐标位置
     *@param[in]  index  合并块在其集合中的下标
     *@return QRect
     */
    QRect headerSpanRect(int index);

    /*!
     *根据给定的合并块下标，刷新该合并块
     *@param[in]  index  合并块在其集合中的下标
     *@return 无
     */
    void updateHeaderSpan(int index);

    /*!
     *选择（反选）合并块，并递归处理父合并块和子合并块的选中状态
     *@param[in]  index        合并块在其集合中的下标
     *@param[in]  select       true表示选中，false表示不选中
     *@param[in]  checkParent  是否修改父合并块的选中状态  
     *@param[in]  checkChild   是否修改子合并块的选中状态
     *@return 无
     */
    void selectHeaderSpan(int index, bool select, bool checkParent = true, bool checkChild = true);

    /*!
     *根据给定的选择区域，返回该选择区域的坐标
     *@param[in]  selection  选择区域
     *@return QRegion
     */
    QRegion visualRegionForSelection(const QItemSelection &selection) const;

    /*!
     *根据选中的区域和反选的区域更新表头相关合并块的选中状态
     *@param[in]  selected    选中的区域
     *@param[in]  deselected  反选的区域  
     *@return 无
     */
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    /*!
     *返回表头行数
     *@return int
     */
    int rowCount() const;

    /*!
     *返回表头列数
     *@return int
     */
    int colCount() const;
    /**
     * @brief 重新计算当前显示行的多行表头
     * @return
     */
    int resetSpan();
    /**
     * @brief 刷新一行（列）（包括同步dataSource中的数据到headerData中）
     * @param index
     * @return
     */
    void updateSpan(int index);

    /**
     * @brief 根据行或列号，把相应的span选中
     * @param index tableView的行或列
     * @return
     */
    void setSpansSelected(int index, bool selected);

    /*!
     * \brief 用来让外部设置横向表头高度，顺序为从上往下
     * \param minHs高度的集合
     */
    void setMinimumHeights(QVector<int> &minHs);

public Q_SLOTS:
    /*!
     *设置滚动条偏移
     *@param[in]  offset  
     *@return 无
     *@see 参见GlodonHeaderView::setOffset(int offset)
     */
    void setOffset(int offset);

public:
    //为了达到水平表头多行（竖直表头多列）的效果，内部创建多行GlodonHeaderView
    //并利用其setModel、resizeSection方法实现与表体的同步
    QVector<GlodonHeaderView *> headerList;

    //所有合并块的集合
    GHeaderSpanCollection *headerData;

    //当前选中的合并块在集合中的下标
    int pressedHeaderSpan;

    //用于绘制拖拽动画影像
    QLabel *headerSpanIndicator;

    //拖拽动画影像与鼠标当前位置的偏移
    int headerSpanIndicatorOffset;

    //水平（竖直）表头拖拽动画影像的y方向（x方向）位置
    int headerSpanIndicatorDrawPos;

    //被拖拽的合并块在集合中的下标
    int moveStartSpan;

Q_SIGNALS:
    void headerSpanPressed(int left, int top, int right, int bottom);

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &old);

protected Q_SLOTS:
    void updateSection(int logicalIndex);

private:
    /*!
     *初始化拖拽动画影像
     *@param[in]  position  拖拽动画影像的起始位置
     *@return 无
     */
    void setupHeaderSpanIndicator(int position);

    /*!
     *更新拖拽动画影像
     *@param[in]  position  拖拽动画影像要移动到的位置
     *@return 无
     */
    void updateHeaderSpanIndicator(int position);

    void setExtraCount(int count);

    /*!
     *取得最后的列宽或行高
     *@param[in]  无
     *@return 最后的列宽或行高
     */
    int lastDrawWidth();
    void paintSpanHeader(QPainter *painter, const QRect &rect, GLDHeaderSpan *span) const;
    bool checkLogicalTitles(int fixedCount);
    bool inTopHeaderRange(int topHeaderIndex, int index);
    bool checkCanMove(int targetLogicalIndex);
    void deselectedHeaderSpans();
    int parseSpan(int nFirstVisual, int nLastVisual, int nMaxCount);
    int maxCount();
    Q_DECLARE_PRIVATE(GlodonMultiHeaderView)

    /*!
     * \brief 恢复重新创建前设置好的高度
     */
    void recoverMinimumHeights();

private:
    // 用来存储设置好的高度
    QVector<int> m_minimumHeights;
};

#endif // GLDMULTIHEADERVIEW_H
