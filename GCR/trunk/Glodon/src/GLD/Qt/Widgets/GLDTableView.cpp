#include <QAbstractItemModel>
#include <QAbstractItemView>

#include <private/qabstractitemmodel_p.h>

#include "GLDTableView.h"
#include "GLDTableView_p.h"
#include "GLDTreeDrawInfo.h"
#include "GLDTreeModel.h"
#include "GLDGroupHeaderView.h"
#include "GLDGroupModel.h"
#include "GLDGlobal.h"
#include "GLDFileUtils.h"

bool GlodonTableViewPrivate::drawTree(QPainter *painter, QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_Q(GlodonTableView);

    if (m_treeDecorationStyle == NoDecoration)
        return false;

    if (q->visualIndex(index).column() != m_treeColumn)
        return false;

    const GTreeViewItem &oViewItem = m_drawInfo->m_viewItems.at(index.row());

    if (oViewItem.parentIndex == -1 && oViewItem.hasChildren == 0)
        return false;

    if (m_treeDecorationStyle == OSStyle)
    {
        drawOSStyle(painter, option, index, oViewItem);
    }
    else
    {
        drawNormalStyle(painter, option, index, oViewItem);
    }

    return true;
}

bool GlodonTableViewPrivate::expandOrCollapseItemAtPos(const QPoint &pos)
{
    Q_Q(GlodonTableView);

    // we want to handle mousePress in EditingState (persistent editors)
    if ((m_state != GlodonAbstractItemView::NoState
            && m_state != GlodonAbstractItemView::EditingState)
            || !viewport->rect().contains(pos))
    {
        return true;
    }

    int nItem = itemDecorationAt(pos);

    if ((nItem != -1) && 0 != m_drawInfo->m_viewItems.at(nItem).hasChildren)
    {
        if (m_drawInfo->m_viewItems.at(nItem).expanded)
        {
            collapse(nItem, true);
        }
        else
        {
            expand(nItem, true);
        }

        if (!isAnimating())
        {
            q->updateGeometries();
            viewport->update();
        }

        return true;
    }

    return false;
}

int GlodonTableViewPrivate::itemDecorationAt(const QPoint &pos)
{
    int nx = pos.x();
    int ncolumn = m_horizontalHeader->visualIndexAt(nx);

    if (ncolumn != m_treeColumn)
    {
        return -1;
    }

    int ny = pos.y();
    int nrow = m_verticalHeader->logicalIndexAt(ny);

    QRect returning = itemDecorationRect(nrow);

    if (!returning.contains(pos))
    {
        return -1;
    }

    return nrow;
}

QRect GlodonTableViewPrivate::itemDecorationRect(int row)
{
    Q_Q(GlodonTableView);

    if (row == -1)
    {
        return QRect();
    }

    int nitemIndentation = CON_INDENT * (m_drawInfo->m_viewItems[row].treeLevel + 1);

    int nposition = m_horizontalHeader->sectionVisualPosition(m_treeColumn);

    QRect rect;
    rect = QRect(nposition + nitemIndentation - CON_INDENT + 2, q->rowVisualPosition(row),
                 CON_INDENT + 2, q->rowHeight(row) + 2);
    return rect;
}

void GlodonTableViewPrivate::expand(int item, bool emitSignal)
{
    doExpand(item, emitSignal);
    viewport->update();
    m_state = GLDTableView::NoState;
}

void GlodonTableViewPrivate::collapse(int item, bool emitSignal)
{
    doCollapse(item, emitSignal);
    viewport->update();
    m_state = GLDTableView::NoState;
}

void GlodonTableViewPrivate::doExpand(int item, bool emitSignal)
{
    Q_Q(GlodonTableView);

    // 不是树形结构，不需要展开
    // 展开的点不合法，不需要展开
    // 已经展开，不需要展开
    if (!m_isTree || item <= -1 || m_drawInfo->m_viewItems.at(item).expanded || m_drawInfo->m_viewItems.count() <= item )
    {
        return;
    }

    q->setState(GlodonAbstractItemView::ExpandingState);
    m_drawInfo->expandItem(item);
    q->afterExpandedChanged(item, true, emitSignal);
    // TODO wangdd-a: 展开折叠后，没必要刷整个界面，刷后面的格子就可以了
    q->refreshDisplayColRow();
    //chensf : 展开后，需要重新计算批注框的显示位置
    q->resetCommentPosition();
}

void GlodonTableViewPrivate::doCollapse(int item, bool emitSignal)
{
    Q_Q(GlodonTableView);

    if (!m_isTree || item <= -1 || m_drawInfo->m_viewItems.count() <= item || 0 == m_drawInfo->m_viewItems.at(item).expanded)
    {
        return;
    }

    q->setState(GlodonAbstractItemView::CollapsingState);
    m_drawInfo->collapseItem(item);
    q->afterExpandedChanged(item, false, emitSignal);
    q->refreshDisplayColRow();
    //chensf : 收起时，隐藏相应节点的批注框，同时重新计算批注框的显示位置
    hideCommentWhenCollapse();
    q->resetCommentPosition();
}

void GlodonTableViewPrivate::drawBranch(
        QPainter *painter, const QRect &rect, const QModelIndex &index, const GTreeViewItem &viewItem)
{
    Q_Q(GlodonTableView);
    QStyleOptionViewItem opt = q->viewOptions();
    QStyle::State extraFlags = QStyle::State_None;

    if (q->isEnabled())
    {
        extraFlags |= QStyle::State_Enabled;
    }

    if (q->window()->isActiveWindow())
    {
        extraFlags |= QStyle::State_Active;
    }

    QPoint oldBO = painter->brushOrigin();

    if (q->verticalScrollMode() == GlodonAbstractItemView::ScrollPerPixel)
    {
        painter->setBrushOrigin(QPoint(0, q->verticalOffset()));
    }

    if (m_selectionModel->isSelected(index))
    {
        extraFlags |= QStyle::State_Selected;
    }

    opt.rect = rect;
    const bool c_bExpanded = (0 != viewItem.expanded);
    const bool c_bChildren = (0 != viewItem.hasChildren);
    const bool c_bMoreSiblings = (0 != viewItem.hasMoreSiblings);

    opt.state = QStyle::State_Item | extraFlags
                | (c_bMoreSiblings ? QStyle::State_Sibling : QStyle::State_None)
                | (c_bChildren ? QStyle::State_Children : QStyle::State_None)
                | (c_bExpanded ? QStyle::State_Open : QStyle::State_None);

    q->style()->drawPrimitive(QStyle::PE_IndicatorBranch, &opt, painter, q);
    painter->setBrushOrigin(oldBO);
}

void GlodonTableViewPrivate::drawOSStyle(
        QPainter *painter, QStyleOptionViewItem &option, const QModelIndex &index, const GTreeViewItem &viewItem)
{
    Q_Q(GlodonTableView);
    int nXPos = option.rect.left();
    int nWidth = option.rect.width();
    int nHeight = option.rect.height();
    int nYPos = option.rect.y();

    q->style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &option, painter, q);

    int nDecorationWidth = CON_INDENT * (viewItem.treeLevel + 1) + c_nTreeDecorationLeftMargin;
    QRect oBeforeText(nXPos, nYPos, nDecorationWidth, nHeight);
    option.rect = oBeforeText;

    //处理树形结构显示的时候，格子实际内容之前部分的选中效果
    GlodonDefaultItemDelegate *pItemDelegate = q->itemDelegate(index);

    pItemDelegate->drawBackground(painter, option, index);

    option.rect.setRect(
                nDecorationWidth + nXPos, nYPos, nWidth - nDecorationWidth, nHeight);

    pItemDelegate->paint(painter, option, index);

    QRect oBranch(nXPos + CON_INDENT * viewItem.treeLevel + c_nTreeDecorationLeftMargin, nYPos, CON_INDENT, nHeight);
    drawBranch(painter, oBranch, index, viewItem);
}

void GlodonTableViewPrivate::drawNormalStyleLines(
        const GTreeViewItem &viewItem, int ngridWidth, QPoint levelCenter, const QModelIndex &index,
        QRect rect, QPainter *painter)
{
    /*
     *     -- A
     *      |
     *      |
     *     (1)
     *      |
     *      |-(2)- A1
     *      |   |
     *     (3)  |---- A11
     *      |
     *      |--(4)-- A2
     */
    if (viewItem.parentIndex != -1)
    {

        // 父节点到自己的垂直方向连线，如图所示(1)
        painter->drawLine(levelCenter.x() - CON_INDENT, rect.top(),
                          levelCenter.x() - CON_INDENT, levelCenter.y());

        // 连接到自己的水平方向连线，如图所示(2)
        painter->drawLine(levelCenter.x() - CON_INDENT, levelCenter.y(),
                          levelCenter.x() - CON_HALF_DECORATION_WIDTH - 1, levelCenter.y());

        // 如果有兄弟节点，绘制垂直方向，到兄弟节点的连线，如图所示(3)，是(1)的延长线
        if (1 == viewItem.hasMoreSiblings)
        {
            painter->drawLine(levelCenter.x() - CON_INDENT, rect.bottom() + ngridWidth,
                              levelCenter.x() - CON_INDENT, levelCenter.y());
        }
    }

    if (0 == viewItem.hasChildren)
    {
        // 没有孩子，有父亲，绘制叶子节点水平方向连线，如图所示(4), 是(2)的延长线
        if (viewItem.parentIndex != -1)
        {
            painter->drawLine(levelCenter.x() - CON_HALF_DECORATION_WIDTH, levelCenter.y(),
                              levelCenter.x() + CON_HALF_DECORATION_WIDTH + 1, levelCenter.y());
        }
    }
    else
    {
        if (0 != viewItem.expanded)
        {
            painter->drawLine(levelCenter.x(), levelCenter.y() + CON_HALF_DECORATION_WIDTH + 1,
                              levelCenter.x(), rect.bottom() + ngridWidth);
        }
    }

    int nlevel = viewItem.treeLevel;
    int nrowIndex = index.row();

    while (nlevel > 0)
    {
        nrowIndex = m_drawInfo->m_viewItems[nrowIndex].parentIndex;

        if (nrowIndex == -1)
        {
            nlevel = -1;
        }
        else
        {
            nlevel = m_drawInfo->m_viewItems[nrowIndex].treeLevel;

            if (nlevel > 0 && 1 == m_drawInfo->m_viewItems[nrowIndex].hasMoreSiblings)
            {
                painter->drawLine(rect.left() + CON_INDENT * nlevel - CON_HALF_DECORATION_WIDTH + 4,
                                  rect.top() - 1,
                                  rect.left() + CON_INDENT * nlevel - CON_HALF_DECORATION_WIDTH + 4,
                                  rect.bottom());
            }
        }
    }
}

QPen GlodonTableViewPrivate::normalStylePen(QStyleOptionViewItem &option)
{
    Q_Q(GlodonTableView);

    const int c_gridHint = q->style()->styleHint(QStyle::SH_Table_GridLineColor, &option, q);
    const QColor c_gridColor = static_cast<QRgb>(c_gridHint);
    return QPen(c_gridColor, 0, m_gridStyle);
}

QRect GlodonTableViewPrivate::beforeTextRect(const QRect &cellRect, int indentation)
{
    int nBeforeTextWidth =
            c_nTreeDecorationLeftMargin + indentation +
            CON_HALF_DECORATION_WIDTH * 2 + c_nTreeDecorationRightMargin;

    return QRect(cellRect.left(), cellRect.top(),
                 qMin(nBeforeTextWidth, cellRect.width()), cellRect.height());
}

void GlodonTableViewPrivate::drawNormalStyle(
        QPainter *painter, QStyleOptionViewItem &option, const QModelIndex &index, const GTreeViewItem &viewItem)
{
    Q_Q(GlodonTableView);

    q->style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &option, painter, q);

    QRect oCellRect = option.rect;
    int nIndentation = CON_INDENT * viewItem.treeLevel;

    //处理树形结构显示的时候，格子实际内容之前部分的选中效果
    GlodonDefaultItemDelegate *pItemDelegate = q->itemDelegate(index);

    pItemDelegate->drawBackground(painter, option, index);

    QStyle::State oldState = option.state;
    option.state &= ~QStyle::State_Selected;

    painter->save();
    painter->setPen(normalStylePen(option));

    QRect oBeforeTextRect = beforeTextRect(oCellRect, nIndentation);
    painter->setClipRect(oBeforeTextRect);

    QPoint oDecorationCenter = decorationCenter(oCellRect, nIndentation);

    // 绘制树形结构样式
    if (1 == viewItem.hasChildren)
    {
        drawNormalStyleTreeDecoration(painter, decorationRect(oDecorationCenter), 0 != viewItem.expanded, option);
    }

    int nGridWidth = m_showGrid ? 0 : 1;
    if (m_treeDecorationStyle == NormalStyle)
    {
        drawNormalStyleLines(viewItem, nGridWidth, oDecorationCenter, index, oCellRect, painter);
    }

    painter->restore();

    option.rect = afterDecorationRect(oDecorationCenter, oCellRect, oBeforeTextRect);
    option.state = oldState;

    // TODO liurx 此处应该统一计算树形结构偏移，不应该计算两次，再调整差异
    //保持与minTextHeight处一致
    {
        const int c_offset = q->treeCellDisplayHorizontalOffset(index.row(), m_treeColumn, false);
        QStyle *sty = q->style();
        const int c_textMargin = sty->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, q_func()) - 1;// + 1;
        const int c_autoSuitRowHeightWidth = m_horizontalHeader->sectionSize(index.column())
                + c_textMargin - c_offset - c_rightSelectedDashLineWidth;//此处同minTextHeight

        if (option.rect.width() < c_autoSuitRowHeightWidth)
        {
            option.rect.setWidth(c_autoSuitRowHeightWidth);
            const int c_displayRectOffset = (option.rect.right() - option.rect.right());

            if (c_displayRectOffset < 0)
            {
                option.rect.adjust(c_displayRectOffset, 0, c_displayRectOffset, 0);
            }
        }
    }
    pItemDelegate->paint(painter, option, index);
}

void GlodonTableViewPrivate::drawNormalStyleTreeDecoration(
        QPainter *painter, const QRect &rect, bool expanded,
        const QStyleOptionViewItem &option)
{
    QStyleOptionViewItem opt = option;
    opt.rect = rect;
    opt.state = opt.state & ~QStyle::State_HasFocus;
    opt.state |= QStyle::State_Off;

    painter->save();
    QPen decorationPen = painter->pen();

#ifndef __APPLE__
    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &opt, painter, widget);
#else
    decorationPen.setColor(QColor(125, 125, 125));
    painter->setPen(decorationPen);
    painter->fillRect(rect, QColor(255, 255, 255));
    painter->drawRect(rect);
#endif

    decorationPen.setStyle(Qt::SolidLine);
    decorationPen.setColor(Qt::black);
    painter->setPen(decorationPen);
    QPoint center = rect.center();
    painter->drawLine(center.x() - 3, center.y(), center.x() + 3, center.y());

    if (!expanded)
    {
        painter->drawLine(center.x(), center.y() - 3, center.x(), center.y() + 3);
    }

    painter->restore();
}

void GlodonTableViewPrivate::doSelect(
        const QModelIndex &tl, const QModelIndex &br, QItemSelectionModel::SelectionFlags command, bool isRowSelect)
{
    Q_Q(GlodonTableView);

    QItemSelection selection(tl, br);

    m_oTopLeftSelectIndex = tl;
    m_oBottomRightSelectIndex = br;

    if (m_addChildInSelection && m_isTree)
    {
        q->setChildSelection(tl, br, selection, isRowSelect);
    }

    m_selectionModel->select(selection, command);
}

void GlodonTableViewPrivate::hideCommentWhenCollapse()
{
    QMap<QPersistentModelIndex,GCustomCommentFrame*>::iterator iter = m_oCommentFrames.begin();
    while (iter != m_oCommentFrames.end())
    {
        if (m_drawInfo->m_viewItems[iter.key().row()].hidden)
        {
            iter.value()->hide();
        }
        iter++;
    }
}

void GlodonTableViewPrivate::itemDelegatePaint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QStyleOptionViewItem opt = option;

    if (m_isTree)
    {
        if (GlodonTableViewPrivate::drawTree(painter, opt, index))
            return;
    }

    GLDTableViewPrivate::itemDelegatePaint(painter, opt, index);
}

void GlodonTableView::groupChanged(QVector<int> newGroup)
{
    // 将树形交给现存的最靠前的列
    Q_D(GlodonTableView);

    int nTreeCol = 0;

    for (int nCol = 0; nCol < d->m_horizontalHeader->count(); ++nCol)
    {
        for (int nGroup = 0; nGroup < newGroup.size(); ++nGroup)
        {
            if (nCol == newGroup.at(nGroup) && nCol <= nTreeCol)
            {
                nTreeCol++;
                break;
            }
        }
    }

    d->m_treeColumn = nTreeCol;
}

void GlodonTableView::hideRow(int row)
{
    Q_D(GlodonTableView);

    if (d->m_isTree || d->m_isGroupMode)
    {
        return;
    }

    GLDTableView::hideRow(row);
}

void GlodonTableView::setRootIndex(const QModelIndex &index)
{
    Q_D(GlodonTableView);

    if (index == d->m_root)
    {
        viewport()->update();

        return;
    }

    d->m_verticalHeader->setRootIndex(index);
    d->m_horizontalHeader->setRootIndex(index);

    GlodonAbstractItemView::setRootIndex(index);
}

QModelIndex GlodonTableView::treeIndex(const QModelIndex &index) const
{
    Q_D(const GlodonTableView);

    if (d->m_isGroupMode)
    {
        return QModelIndex();
    }

    if (d->m_isTree)
    {
        return static_cast<GlodonTreeModel *>(d->m_model)->treeIndex(index);
    }

    return index;
}

QModelIndex GlodonTableView::dataIndex(const QModelIndex &index) const
{
    Q_D(const GlodonTableView);

    if (d->m_isGroupMode)
    {
        QModelIndex treeIndex = static_cast<GlodonTreeModel *>(d->m_model)->dataIndex(index);
        QAbstractItemModel *itemModel = const_cast<QAbstractItemModel *>(treeIndex.model());

        return static_cast<GlodonGroupModel *>(itemModel)->dataIndex(treeIndex);
    }

    if (d->m_isTree)
    {
        if (index.model() == d->m_model)
        {
            QModelIndex treeIndex = static_cast<GlodonTreeModel *>(d->m_model)->dataIndex(index);
            const GlodonGroupModel *groupModel = dynamic_cast<const GlodonGroupModel *>(treeIndex.model());

            if (groupModel)
            {
                return groupModel->dataIndex(treeIndex);
            }
            else
            {
                return treeIndex;
            }
        }
    }

    return index;
}

QModelIndex GlodonTableView::currentDataIndex() const
{
    Q_D(const GlodonTableView);

    if (d->m_isTree)
    {
        const QModelIndex c_index = currentIndex();
        GlodonTreeModel *treeModel = static_cast<GlodonTreeModel *>(d->m_model);
        return treeModel->dataIndex(c_index);
    }

    return QModelIndex();
}

void GlodonTableView::setIsTree(bool value)
{
    Q_D(GlodonTableView);

    if ((d->m_isTree == value) || (d->m_isGroupMode))
    {
        return;
    }

    doSetIsTree(value);

    if (value)
    {
        if (!d->m_dataModel)
        {
            doSetModel(d->m_model);
        }
    }
    else
    {
        doSetModel(d->m_dataModel);
        setDataModel(0);
    }

    d->viewport->update();
}

bool GlodonTableView::isTree() const
{
    Q_D(const GlodonTableView);

    return d->m_isTree;
}

void GlodonTableView::setIsGroupMode(bool groupModeEnable)
{
    Q_D(GlodonTableView);

    if ((groupModeEnable == d->m_isGroupMode) || isDisplayFilter() || totalRowAtFooter())
    {
        return;
    }

    d->m_isGroupMode = groupModeEnable;

    if (groupModeEnable)
    {
        //切换model时如果处于编辑状态，强制退出
        if (d->m_oEditorIndex.isValid())
        {
            forceCloseEditor();
        }

        GlodonGroupHeaderView *cxHeader = new GlodonGroupHeaderView(this);
        setHorizontalHeader(cxHeader);
        doSetModel(d->m_model);

        if (!cxHeader->isVisible())
        {
            cxHeader->show();
        }
    }
    else
    {
        GlodonHeaderView *header = new GlodonHeaderView(Qt::Horizontal, this);
        setHorizontalHeader(header);
        doSetModel(d->m_dataModel);

        if (!header->isVisible())
        {
            header->show();
        }
    }
}

bool GlodonTableView::isGroupMode() const
{
    Q_D(const GlodonTableView);
    return d->m_isGroupMode;
}

void GlodonTableView::setChildSelection(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                        QItemSelection &selection, bool isRowSelect)
{
    Q_D(GlodonTableView);

    if (!isRowSelect)
    {
        return;
    }

    if (!selection.isEmpty())
    {
        selection.clear();
    }

    QModelIndex tl;
    QModelIndex br;
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row)
    {
        int nbottomRow = d->m_drawInfo->lastChildRow(row);
        tl = d->m_model->index(row, topLeft.column(), d->m_root);
        br = d->m_model->index(nbottomRow, bottomRight.column(), d->m_root);

        if (!selection.contains(tl) && !selection.contains(br))
        {
            selection.append(QItemSelectionRange(tl, br));
        }
    }

    d->m_oTopLeftSelectIndex = topLeft;
    d->m_oBottomRightSelectIndex = br;
}

void GlodonTableView::setTreeDecorationStyle(TreeDecorationStyle style)
{
    Q_D(GlodonTableView);

    if (d->m_treeDecorationStyle != style)
    {
        d->m_treeDecorationStyle = style;
        d->viewport->update();
    }
}

TreeDecorationStyle GlodonTableView::treeDecorationStyle() const
{
    Q_D(const GlodonTableView);
    return d->m_treeDecorationStyle;
}

void GlodonTableView::setTreeDrawInfo(GlodonTreeDrawInfo *tableViewDrawInfo)
{
    Q_D(GlodonTableView);

    if (d->m_drawInfo == tableViewDrawInfo)
    {
        return;
    }
    else if (!d->m_drawInfo)
    {
        d->m_drawInfo = tableViewDrawInfo;
        d->m_drawInfo->m_header = d->m_verticalHeader;
        d->m_drawInfo->setModel(d->m_model);
    }
}

GlodonTreeDrawInfo *GlodonTableView::treeDrawInfo()
{
    Q_D(GlodonTableView);
    return d->m_drawInfo;
}

bool GlodonTableView::isAddChildToSelection()
{
    Q_D(GlodonTableView);
    return d->m_addChildInSelection;
}

void GlodonTableView::setAddChildToSelection(bool value)
{
    Q_D(GlodonTableView);
    d->m_addChildInSelection = value;
}

void GlodonTableView::setTreeColumn(int column)
{
    Q_D(GlodonTableView);
    d->m_treeColumn = column;
}

int GlodonTableView::treeColumn()
{
    Q_D(GlodonTableView);
    return d->m_treeColumn;
}

void GlodonTableView::expand(int row, bool emitSignal)
{
    Q_D(GlodonTableView);
    d->expand(row, emitSignal);
}

void GlodonTableView::collapse(int row, bool emitSignal)
{
    Q_D(GlodonTableView);
    d->collapse(row, emitSignal);
}

bool GlodonTableView::isRowExpanded(int row)
{
    Q_D(GlodonTableView);
    Q_ASSERT(row >= 0);
    return 0 != d->m_drawInfo->m_viewItems[row].expanded;
}

void GlodonTableView::expandAll()
{
    Q_D(GlodonTableView);

    if (d->m_isTree)
    {
        setState(GlodonAbstractItemView::ExpandingState);

        try
        {
            QSet<int> oExpandItems;
            for (int row = 0; row < d->m_drawInfo->m_viewItems.count(); row++)
            {
                if (d->m_drawInfo->m_viewItems.at(row).expanded)
                {
                    continue;
                }
                d->m_drawInfo->doExpandItem(row, oExpandItems);
                afterExpandedChanged(row, true, true);
            }
            d->m_verticalHeader->batchSetSectionHidden(oExpandItems, false);
        }
        catch (...)
        {
            d->m_state = GLDTableView::NoState;
            throw;
        }

        d->m_state = GLDTableView::NoState;
        refreshDisplayColRow();

        d->viewport->update();
        QSize oViewportSize = d->viewport->size();
        updateHorizontalScrollBar(oViewportSize);
        updateVerticalScrollBar(oViewportSize);
    }
}

void GlodonTableView::collapseAll()
{
    Q_D(GlodonTableView);

    if (d->m_isTree)
    {
        setState(GlodonAbstractItemView::CollapsingState);

        try
        {
            QSet<int> oCollapseRows;
            for (int row = 0; row < d->m_drawInfo->m_viewItems.count(); row++)
            {
                if (!d->m_drawInfo->m_viewItems.at(row).expanded)
                {
                    continue;
                }
                d->m_drawInfo->doCollapseItem(row, oCollapseRows);
                afterExpandedChanged(row, false, true);
            }
            d->m_verticalHeader->batchSetSectionHidden(oCollapseRows, true);
        }
        catch (...)
        {
            d->m_state = GLDTableView::NoState;
            throw;
        }

        d->m_state = GLDTableView::NoState;
        refreshDisplayColRow();

        d->viewport->update();
        QSize oViewportSize = d->viewport->size();
        updateHorizontalScrollBar(oViewportSize);
        updateVerticalScrollBar(oViewportSize);
    }
}

void GlodonTableView::reBuildTree()
{
    Q_D(GlodonTableView);

    d->m_drawInfo->init(false);
    d->viewport->update();
    d->m_horizontalHeader->update();
    d->m_verticalHeader->update();
}

void GlodonTableView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                  const QVector<int> &roles)
{
    Q_D(GlodonTableView);

    if (roles.contains(gidrRowExpandedRole))
    {
        if ((-1 == topLeft.row()) || (-1 == bottomRight.row()))
        {
            return;
        }

        for (int i = topLeft.row(); i <= bottomRight.row(); i++)
        {
            QModelIndex oIndex = model()->index(i, 0);
            QVariant varExpanded = model()->data(oIndex, gidrRowExpandedRole);

            if (qvariant_cast<bool>(varExpanded))
            {
                d->doExpand(i, true);
            }
            else
            {
                d->doCollapse(i, true);
            }
        }

        d->viewport->update();

        setState(NoState);
    }
    else
    {
        GLDTableView::dataChanged(topLeft, bottomRight, roles);
    }
}

GlodonTableView::GlodonTableView(QWidget *parent) :
    GLDTableView(*new GlodonTableViewPrivate, parent)
{
    this->setStyleSheet(loadQssFile(c_sTableViewQssFile));
}

GlodonTableView::GlodonTableView(GlodonTableViewPrivate &dd, QWidget *parent) :
    GLDTableView(dd, parent)
{

}

GlodonTableView::~GlodonTableView()
{

}

void GlodonTableView::doSetIsTree(bool value)
{
    Q_D(GlodonTableView);

    //切换model时如果处于编辑状态， 强制退出
    if (d->m_oEditorIndex.isValid())
    {
        forceCloseEditor();
    }

    d->m_isTree = value;
    d->m_verticalHeader->setIsTree(value);
    d->m_horizontalHeader->setIsTree(value);

    if (value)
    {
        if (!d->m_drawInfo)
        {
            GlodonTreeDrawInfo *treeDrawInfo = new GlodonTreeDrawInfo(this);
            treeDrawInfo->setTreeColumn(d->m_treeColumn);
            setTreeDrawInfo(treeDrawInfo);
        }
    }
    else
    {
        d->m_drawInfo = NULL;
    }
}

void GlodonTableView::doSetModel(QAbstractItemModel *model)
{
    Q_D(GlodonTableView);

    if (d->m_isGroupMode)
    {
        if (!dynamic_cast<GlodonGroupModel *>(model))
        {
            GlodonGroupModel *cxModel;

            if (d->m_isTree)
            {
                GlodonTreeModel *treeModel = dynamic_cast<GlodonTreeModel *>(model);
                cxModel = new GlodonGroupModel(treeModel->model, this);
                d->m_dataModel = treeModel->model;
                treeModel->model = cxModel;
                d->m_drawInfo->setModel(cxModel);
                model = treeModel;
            }
            else
            {
                cxModel = new GlodonGroupModel(model, this);
                d->m_model = cxModel;
                doSetIsTree(true);
                GlodonTreeModel *treeModel = new GlodonTreeModel(cxModel, this);
                treeModel->drawInfo = d->m_drawInfo;
                d->m_dataModel = model;
                model = treeModel;
            }

            connect(dynamic_cast<GlodonGroupHeaderView *>(d->m_horizontalHeader), SIGNAL(groupChanged(QVector<int>)),
                    cxModel, SLOT(groupChanged(QVector<int>)));
            connect(dynamic_cast<GlodonGroupHeaderView *>(d->m_horizontalHeader), SIGNAL(groupChanged(QVector<int>)),
                    this, SLOT(groupChanged(QVector<int>)));
            connect(cxModel, SIGNAL(modelRebuild()), this, SLOT(reBuildTree()));

        }
    }
    else if (d->m_isTree)
    {
        if (d->m_drawInfo->model != model)
        {
            d->m_drawInfo->setModel(model);
        }

        d->m_dataModel = model;
        GlodonTreeModel *treeModel = new GlodonTreeModel(model, this);
        treeModel->drawInfo = d->m_drawInfo;
        model = treeModel;
    }

    if (model == d->m_model)
    {
        return;
    }

    //let's disconnect from the old model
    if (d->m_model && d->m_model != QAbstractItemModelPrivate::staticEmptyModel())
    {
        disconnect(d->m_model, SIGNAL(rowsInserted(QModelIndex, int, int)),
                   this, SLOT(_q_updateSpanInsertedRows(QModelIndex, int, int)));
        disconnect(d->m_model, SIGNAL(columnsInserted(QModelIndex, int, int)),
                   this, SLOT(_q_updateSpanInsertedColumns(QModelIndex, int, int)));
        disconnect(d->m_model, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                   this, SLOT(_q_updateSpanRemovedRows(QModelIndex, int, int)));
        disconnect(d->m_model, SIGNAL(columnsRemoved(QModelIndex, int, int)),
                   this, SLOT(_q_updateSpanRemovedColumns(QModelIndex, int, int)));
        disconnect(d->m_model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
                   this, SLOT(onHeaderDataChanged(Qt::Orientation, int, int)));

        disconnect(d->m_verticalHeader, SIGNAL(scrolled(int)),
                   this, SLOT(onScrolled(int)));
        disconnect(d->m_horizontalHeader, SIGNAL(scrolled(int)),
                   this, SLOT(onScrolled(int)));
        disconnect(d->m_verticalHeader, &GlodonHeaderView::sectionResizing,
                   this, &GlodonTableView::showSectionResizingInfoFrame);
        disconnect(d->m_horizontalHeader, &GlodonHeaderView::sectionResizing,
                   this, &GlodonTableView::showSectionResizingInfoFrame);
    }

    if (model) //and connect to the new one
    {
        connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)),
                this, SLOT(_q_updateSpanInsertedRows(QModelIndex, int, int)));
        connect(model, SIGNAL(columnsInserted(QModelIndex, int, int)),
                this, SLOT(_q_updateSpanInsertedColumns(QModelIndex, int, int)));
        connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                this, SLOT(_q_updateSpanRemovedRows(QModelIndex, int, int)));
        connect(model, SIGNAL(columnsRemoved(QModelIndex, int, int)),
                this, SLOT(_q_updateSpanRemovedColumns(QModelIndex, int, int)));
        connect(model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
                   this, SLOT(onHeaderDataChanged(Qt::Orientation, int, int)));

        connect(d->m_verticalHeader, SIGNAL(scrolled(int)),
                this, SLOT(onScrolled(int)));
        connect(d->m_horizontalHeader, SIGNAL(scrolled(int)),
                this, SLOT(onScrolled(int)));
        connect(d->m_verticalHeader, &GlodonHeaderView::sectionResizing,
                this, &GlodonTableView::showSectionResizingInfoFrame);
        connect(d->m_horizontalHeader, &GlodonHeaderView::sectionResizing,
                this, &GlodonTableView::showSectionResizingInfoFrame);
    }

    d->m_verticalHeader->setModel(model);
    d->m_horizontalHeader->setModel(model);

    GTableCornerWidget *cornerWidget = dynamic_cast<GTableCornerWidget *>(d->m_cornerWidget);

    if (cornerWidget)
    {
        cornerWidget->setModel(model);
    }

    GlodonAbstractItemView *gView = dynamic_cast<GlodonAbstractItemView *>(d->m_cornerWidget);

    if (gView)
    {
        gView->setModel(model);
    }

    QAbstractItemView *qView = dynamic_cast<QAbstractItemView *>(d->m_cornerWidget);

    if (qView)
    {
        qView->setModel(model);
    }

    GlodonAbstractItemView::setModel(model);
    GLDTableView::doSetModel(model);

    updateGeometries();
}

int GlodonTableView::treeCellDisplayHorizontalOffset(int row, int col, bool isOldMinTextHeightCalWay) const
{
    int nOffset = 0;

    if (isTree() && col == d_func()->m_treeColumn)
    {
        if (isOldMinTextHeightCalWay)
        {
            GlodonTreeModel *treeModel = dynamic_cast<GlodonTreeModel *>(model());
            nOffset = (treeModel->drawInfo->m_viewItems.at(row).treeLevel + 1) * c_Indent + 2 * c_HalfWidth;
        }
        else
        {
            const GTreeViewItem &viewItem = d_func()->m_drawInfo->m_viewItems[row];
            const int c_levelCenterOffset = CON_INDENT * viewItem.treeLevel + CON_HALF_DECORATION_WIDTH + 2;
            nOffset = qMin((c_levelCenterOffset + CON_HALF_DECORATION_WIDTH + 3),
                           d_func()->m_horizontalHeader->sectionSize(col));
        }
    }

    return nOffset;
}

void GlodonTableView::onBoolCellPress(QMouseEvent *event)
{
//    Q_D(GlodonTableView);

    QModelIndex currIndex = indexAt(event->pos());
    QRect rect = visualRect(currIndex);
    QRect boolRect(rect.center().rx() - 6,
                   rect.center().ry() - 6,
                   13, 13);

//    if (d->m_isTree && currIndex.column() == d->m_treeColumn)
//    {
//        //如果是第一级且没有子的情况下，应与非树形结构时区域一致
//        const GTreeViewItem &viewItem = d->m_drawInfo->m_viewItems.at(currIndex.row());

//        if (!((viewItem.parentIndex == -1) && viewItem.hasChildren != 1))
//        {
//            QRect textRect(rect.left() + CON_INDENT * (viewItem.treeLevel + 1) + 2, rect.top(),
//                           rect.width() - CON_INDENT * (viewItem.treeLevel + 1) - 2, rect.height());
//            boolRect = QRect(textRect.center().rx() - 6, textRect.center().ry() - 6, 13, 13);
//        }
//    }

    if ((event->button() & Qt::LeftButton) && boolRect.contains(event->pos()))
    {
        setBoolEditValue(currIndex);
    }
}

void GlodonTableView::onMousePress(QMouseEvent *event)
{
    Q_D(GlodonTableView);

    if (d->m_isTree)
    {

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

        if (event->type() == QEvent::MouseButtonPress)
#else
        if (style()->styleHint(QStyle::SH_Q3ListViewExpand_SelectMouseType, 0, this) == QEvent::MouseButtonPress)
#endif
        {
            bool bhandled = d->expandOrCollapseItemAtPos(event->pos());
            if (bhandled)
            {
                return;
            }
        }

        QModelIndex currIndex = indexAt(event->pos());
        int nChildCount = -1;
        if (currIndex.isValid())
        {
            nChildCount = d->m_drawInfo->m_viewItems[currIndex.row()].hasChildren;
        }

        if (-1 != nChildCount && d->itemDecorationAt(event->pos()) != -1)
        {
            return;
        }
    }

    GLDTableView::onMousePress(event);
}

bool GlodonTableView::event(QEvent *event)
{
    GLDEvent *gldEvent = dynamic_cast<GLDEvent *>(event);

    if (gldEvent == NULL)
    {
        return GLDTableView::event(event);
    }

    bool result = GlodonAbstractItemView::event(event);

    switch (gldEvent->type())
    {
        case GM_EXPAND:
        {
            doGMExpand(gldEvent);
            break;
        }

        case GM_QUERYEXPAND:
        {
            doGMQueryExpand(gldEvent);
            break;
        }

        case GM_COLLAPSE:
        {
            doGMCollapse(gldEvent);
            break;
        }

        case GM_QUERYCOLLAPSE:
        {
            doGMQueryCollapse(gldEvent);
            break;
        }

        case GM_EXPANDALL:
        {
            doGMExpandAll(gldEvent);
            break;
        }

        case GM_QUERYEXPANDALL:
        {
            doGMQueryExpandAll(gldEvent);
            break;
        }

        case GM_COLLAPSEALL:
        {
            doGMCollapseAll(gldEvent);
            break;
        }

        case GM_QUERYCOLLAPSEALL:
        {
            doGMQueryCollapseAll(gldEvent);
            break;
        }

        default:
        {
            // nothing
            break;
        }
    }

    return result;
}

QAbstractItemModel *GlodonTableView::itemModel()
{
    Q_D(GlodonTableView);

    QAbstractItemModel *dm = GLDTableView::itemModel();

    if (d->m_isTree)
    {
        dm = dataModel();
    }

    return dm;
}

void GlodonTableView::doGMExpand(GLDEvent *event)
{
    expand(currentIndex().row(), true);
    G_UNUSED(event);
}

void GlodonTableView::doGMQueryExpand(GLDEvent *event)
{
    Q_D(GlodonTableView);
    event->setResult(d->m_model->rowCount(currentIndex()) > 0);
    G_UNUSED(event);
}

void GlodonTableView::doGMCollapse(GLDEvent *event)
{
    collapse(currentIndex().row(), true);
    G_UNUSED(event);
}

void GlodonTableView::doGMQueryCollapse(GLDEvent *event)
{
    Q_D(GlodonTableView);
    event->setResult(d->m_model->rowCount(currentIndex()) > 0);
    G_UNUSED(event);
}

void GlodonTableView::doGMExpandAll(GLDEvent *event)
{
    expandAll();
    G_UNUSED(event);
}

void GlodonTableView::doGMQueryExpandAll(GLDEvent *event)
{
    event->setResult(isTree());
    G_UNUSED(event);
}

void GlodonTableView::doGMCollapseAll(GLDEvent *event)
{
    collapseAll();
    G_UNUSED(event);
}

void GlodonTableView::doGMQueryCollapseAll(GLDEvent *event)
{
    event->setResult(isTree());
    G_UNUSED(event);
}
