#ifndef GLDTABLEVIEW_P_H
#define GLDTABLEVIEW_P_H

#include "GLDAbstractItemView_p.h"
#include "GLDTableView.h"
#include "GLDTableViewBasic_p.h"

struct GTreeViewItem;

#define CON_INDENT 10
#define CON_HALF_DECORATION_WIDTH 6
#define CON_DECORATION_WIDTH (CON_HALF_DECORATION_WIDTH*2)

const int c_nTreeDecorationLeftMargin = 2;
const int c_nTreeDecorationRightMargin = 3;

const int c_rightSelectedDashLineWidth = 1;

class GLDTABLEVIEWSHARED_EXPORT GlodonTableViewPrivate : public GLDTableViewPrivate
{
    Q_DECLARE_PUBLIC(GlodonTableView)

public:
    GlodonTableViewPrivate() :
              m_isTree(false),
              m_isGroupMode(false),
              m_addChildInSelection(true),
              m_treeColumn(0),
              m_drawInfo(NULL),
              m_treeDecorationStyle(NormalStyle)
    {

    }

    virtual bool drawTree(QPainter *painter, QStyleOptionViewItem &option, const QModelIndex &index);

    virtual bool expandOrCollapseItemAtPos(const QPoint &pos);

    int itemDecorationAt(const QPoint &pos);

    virtual QRect itemDecorationRect(int row);

    void expand(int item, bool emitSignal);

    void collapse(int item, bool emitSignal);

    virtual void doExpand(int item, bool emitSignal);

    virtual void doCollapse(int item, bool emitSignal);

    void drawOSStyle(QPainter *painter, QStyleOptionViewItem &option, const QModelIndex &index, const GTreeViewItem &viewItem);
    void drawBranch(QPainter *painter, const QRect &rect, const QModelIndex &index, const GTreeViewItem &viewItem);

    void drawNormalStyle(
            QPainter *painter, QStyleOptionViewItem &option, const QModelIndex &index,
                         const GTreeViewItem &viewItem);
    void drawNormalStyleTreeDecoration(
            QPainter *painter, const QRect &rect, bool expanded, const QStyleOptionViewItem &option);
    void drawNormalStyleLines(
            const GTreeViewItem &viewItem, int ngridWidth, QPoint levelCenter,
            const QModelIndex &index, QRect rect, QPainter *painter);

    QPen normalStylePen(QStyleOptionViewItem &option);
    QRect beforeTextRect(const QRect &cellRect, int indentation);

    inline QRect afterDecorationRect(const QPoint &decorationCenter, const QRect &cellRect, const QRect &beforeTextRect)
    {
        return QRect(decorationCenter.x() + CON_HALF_DECORATION_WIDTH + 2, cellRect.top(),
                                cellRect.width() - beforeTextRect.width() + 1, cellRect.height());
    }

    inline QPoint decorationCenter(const QRect &cellRect, int indentation)
    {
        return QPoint(cellRect.left() + indentation + CON_HALF_DECORATION_WIDTH + c_nTreeDecorationLeftMargin,
                      (cellRect.top() + cellRect.bottom()) / 2);
    }

    inline QRect decorationRect(const QPoint &decorationRect)
    {
        return QRect(decorationRect.x() - CON_HALF_DECORATION_WIDTH,
                     decorationRect.y() - CON_HALF_DECORATION_WIDTH,
                     2 * CON_HALF_DECORATION_WIDTH + 1,
                     2 * CON_HALF_DECORATION_WIDTH + 1);
    }

protected:
    virtual void itemDelegatePaint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index);
    virtual void doSelect(const QModelIndex& tl, const QModelIndex& br, QItemSelectionModel::SelectionFlags command, bool isRowSelect);
    /**
     * @brief 当树形节点收起时，隐藏对应的批注框
     */
    void hideCommentWhenCollapse();
public:
    bool m_isTree;
    bool m_isGroupMode;
    bool m_addChildInSelection;     //树形结构时，是否把选中且折叠的子加入到selection中
    int m_treeColumn;               //显示树形结构的列

    GlodonTreeDrawInfo *m_drawInfo;
    TreeDecorationStyle m_treeDecorationStyle;
};

#endif // GLDTABLEVIEW_P_H
