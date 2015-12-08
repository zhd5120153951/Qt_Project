#include <QStylePainter>
#include <QDesktopServices>
#include <QPushButton>
#include <QDebug>
#include <QClipboard>
#include <QScrollBar>
#include <QAbstractItemView>

#include "GLDTableViewBasic.h"
#include "GLDTableViewBasic_p.h"
#include "GLDHeaderView_p.h"
#include "GLDMultiHeaderView.h"
#include "GLDTreeModel.h"
#include "GLDGroupHeaderView.h"
#include "GLDAbstractItemModel.h"
#include "GLDGlobal.h"
#include "GLDStrUtils.h"
#include "GLDScrollStyle.h"
#include "GLDEllipsis.h"

const int c_rightSelectedDashLineWidth = 1;
const int c_nCommentTriangleRightLength = 5; // 批注直角三角形，直角边长
const int c_nResizeHandlerOffset = 4;
const int c_nEmptyRectWidth = 5;
const int c_nFillHandleWidth = 4;

struct GlodonTableViewPainterInfo
{
    GlodonTableViewPainterInfo(
            int top, int bottom, int left, int right, int fVisualRow,
            int fVisualColumn, int lVisualColumn, bool alternateBase)
        : m_nDirtyAreaTop(top), m_nDirtyAreaBottom(bottom), m_nDirtyAreaLeft(left),
          m_nDirtyAreaRight(right), m_nFirstVisualRow(fVisualRow), m_nFirstVisualColumn(fVisualColumn),
          m_nLastVisualColumn(lVisualColumn), m_bAlternateBase(alternateBase)
    {

    }
    int m_nDirtyAreaTop;
    int m_nDirtyAreaBottom;
    int m_nDirtyAreaLeft;
    int m_nDirtyAreaRight;
    int m_nFirstVisualRow;
    int m_nFirstVisualColumn;
    int m_nLastVisualColumn;
    bool m_bAlternateBase;
};

void GSpanCollection::addSpan(GSpanCollection::GSpan *span)
{
    spans.append(span);
    Index::iterator it_y = m_nIndex.lowerBound(-span->top());

    if (it_y == m_nIndex.end() || it_y.key() != -span->top())
    {
        //there is no spans that starts with the row in the index, so create a sublist for it.
        SubIndex sub_index;

        if (it_y != m_nIndex.end())
        {
            //the previouslist is the list of spans that sarts _before_ the row of the span.
            // and which may intersect this row.
            const SubIndex c_previousList = it_y.value();
            foreach(GSpan * s, c_previousList)
            {
                //If a subspans intersect the row, we need to split it into subspans
                if (s->bottom() >= span->top())
                {
                    sub_index.insert(-s->left(), s);
                }
            }
        }

        it_y = m_nIndex.insert(-span->top(), sub_index);
        //we will insert span to *it_y in the later loop
    }

    //insert the span as supspan in all the lists that intesects the span
    while (-it_y.key() <= span->bottom())
    {
        (*it_y).insert(-span->left(), span);

        if (it_y == m_nIndex.begin())
        {
            break;
        }

        --it_y;
    }
}

void GSpanCollection::updateSpan(GSpanCollection::GSpan *span, int old_height)
{
    if (old_height < span->height())
    {
        //add the span as subspan in all the lists that intersect the new covered columns
        Index::iterator it_y = m_nIndex.lowerBound(-(span->top() + old_height - 1));
        Q_ASSERT(it_y != m_nIndex.end()); //it_y must exist since the span is in the list

        while (-it_y.key() <= span->bottom())
        {
            (*it_y).insert(-span->left(), span);

            if (it_y == m_nIndex.begin())
            {
                break;
            }

            --it_y;
        }
    }
    else if (old_height > span->height())
    {
        //remove the span from all the subspans lists that intersect the columns not covered anymore
        Index::iterator it_y = m_nIndex.lowerBound(-qMax(span->bottom(), span->top())); //qMax useful if height is 0
        Q_ASSERT(it_y != m_nIndex.end()); //it_y must exist since the span is in the list

        while (-it_y.key() <= span->top() + old_height - 1)
        {
            if (-it_y.key() > span->bottom())
            {
                int nremoved = (*it_y).remove(-span->left());
                Q_ASSERT(nremoved == 1);
                Q_UNUSED(nremoved);

                if (it_y->isEmpty())
                {
                    it_y = m_nIndex.erase(it_y);
                }
            }

            if (it_y == m_nIndex.begin())
            {
                break;
            }

            --it_y;
        }
    }

    if (span->width() == 0 && span->height() == 0)
    {
        spans.removeOne(span);
        delete span;
    }
}

/** \internal
 * \return a spans that spans over cell x,y  (column,row)  or 0 if there is none.
 */
GSpanCollection::GSpan *GSpanCollection::spanAt(int x, int y) const
{
    Index::const_iterator it_y = m_nIndex.lowerBound(-y);

    if (it_y == m_nIndex.end())
    {
        return 0;
    }

    SubIndex::const_iterator it_x = (*it_y).lowerBound(-x);

    if (it_x == (*it_y).end())
    {
        return 0;
    }

    GSpan *span = *it_x;

    if (span->right() >= x && span->bottom() >= y)
    {
        return span;
    }

    return 0;
}

/** \internal
* remove and deletes all spans inside the collection
*/
void GSpanCollection::clear()
{
    m_nIndex.clear();
    qDeleteAll(spans);
    spans.clear();
}

/** \internal
 * return a list to all the spans that spans over cells in the given rectangle
 */
QList<GSpanCollection::GSpan *> GSpanCollection::spansInRect(int x, int y, int w, int h) const
{
    QSet<GSpan *> list;
    Index::const_iterator it_y = m_nIndex.lowerBound(-y);

    if (it_y == m_nIndex.end())
    {
        --it_y;
    }

    while (-it_y.key() <= y + h)
    {
        SubIndex::const_iterator it_x = (*it_y).lowerBound(-x);

        if (it_x == (*it_y).end())
        {
            --it_x;
        }

        while (-it_x.key() <= x + w)
        {
            GSpan *pSpan = *it_x;

            if (pSpan->bottom() >= y && pSpan->right() >= x)
            {
                list << pSpan;
            }

            if (it_x == (*it_y).begin())
            {
                break;
            }

            --it_x;
        }

        if (it_y == m_nIndex.begin())
        {
            break;
        }

        --it_y;
    }

    return list.toList();
}

#undef DEBUG_SPAN_UPDATE

#ifdef DEBUG_SPAN_UPDATE
QDebug operator<<(QDebug str, const GSpanCollection::GSpan &span)
{
    str << "(" << span.top() << "," << span.left() << "," << span.bottom() << "," << span.right() << ")";
    return str;
}
#endif

/** \internal
* Updates the span collection after row insertion.
*/
void GSpanCollection::updateInsertedRows(int start, int end)
{
#ifdef DEBUG_SPAN_UPDATE
    qDebug() << Q_FUNC_INFO;
    qDebug() << start << end;
    qDebug() << index;
#endif

    if (spans.isEmpty())
    {
        return;
    }

    int ndelta = end - start + 1;
#ifdef DEBUG_SPAN_UPDATE
    qDebug("Before");
#endif

    for (SpanList::iterator it = spans.begin(); it != spans.end(); ++it)
    {
        GSpan *span = *it;
#ifdef DEBUG_SPAN_UPDATE
        qDebug() << span << *span;
#endif

        if (span->m_bottom < start)
        {
            continue;
        }

        if (span->m_top >= start)
        {
            span->m_top += ndelta;
        }

        span->m_bottom += ndelta;
    }

#ifdef DEBUG_SPAN_UPDATE
    qDebug("After");
    foreach(GSpanCollection::GSpan * span, spans)
    qDebug() << span << *span;
#endif

    for (Index::iterator it_y = m_nIndex.begin(); it_y != m_nIndex.end();)
    {
        int ny = -it_y.key();

        if (ny < start)
        {
            ++it_y;
            continue;
        }

        m_nIndex.insert(-ny - ndelta, it_y.value());
        it_y = m_nIndex.erase(it_y);
    }

#ifdef DEBUG_SPAN_UPDATE
    qDebug() << index;
#endif
}

/** \internal
* Updates the span collection after column insertion.
*/
void GSpanCollection::updateInsertedColumns(int start, int end)
{
#ifdef DEBUG_SPAN_UPDATE
    qDebug() << Q_FUNC_INFO;
    qDebug() << start << end;
    qDebug() << index;
#endif

    if (spans.isEmpty())
    {
        return;
    }

    int ndelta = end - start + 1;
#ifdef DEBUG_SPAN_UPDATE
    qDebug("Before");
#endif

    for (SpanList::iterator it = spans.begin(); it != spans.end(); ++it)
    {
        GSpan *span = *it;
#ifdef DEBUG_SPAN_UPDATE
        qDebug() << span << *span;
#endif

        if (span->m_right < start)
        {
            continue;
        }

        if (span->m_left >= start)
        {
            span->m_left += ndelta;
        }

        span->m_right += ndelta;
    }

#ifdef DEBUG_SPAN_UPDATE
    qDebug("After");
    foreach(GSpanCollection::GSpan * span, spans)
    qDebug() << span << *span;
#endif

    for (Index::iterator it_y = m_nIndex.begin(); it_y != m_nIndex.end(); ++it_y)
    {
        SubIndex &subindex = it_y.value();

        for (SubIndex::iterator it = subindex.begin(); it != subindex.end();)
        {
            int nx = -it.key();

            if (nx < start)
            {
                ++it;
                continue;
            }

            subindex.insert(-nx - ndelta, it.value());
            it = subindex.erase(it);
        }
    }

#ifdef DEBUG_SPAN_UPDATE
    qDebug() << index;
#endif
}

/** \internal
* Cleans a subindex from to be deleted spans. The update argument is used
* to move the spans inside the subindex, in case their anchor changed.
* \return true if no span in this subindex starts at y, and should thus be deleted.
*/
bool GSpanCollection::cleanSpanSubIndex(GSpanCollection::SubIndex &subindex, int y, bool update)
{
    if (subindex.isEmpty())
    {
        return true;
    }

    bool bshould_be_deleted = true;
    SubIndex::iterator it = subindex.end();

    do
    {
        --it;
        int nx = -it.key();
        GSpan *span = it.value();

        if (span->will_be_deleted)
        {
            it = subindex.erase(it);
            continue;
        }

        if (update && span->m_left != nx)
        {
            subindex.insert(-span->m_left, span);
            it = subindex.erase(it);
        }

        if (bshould_be_deleted && span->m_top == y)
        {
            bshould_be_deleted = false;
        }
    }
    while (it != subindex.begin());

    return bshould_be_deleted;
}

/** \internal
* Updates the span collection after row removal.
*/
void GSpanCollection::updateRemovedRows(int start, int end)
{
#ifdef DEBUG_SPAN_UPDATE
    qDebug() << Q_FUNC_INFO;
    qDebug() << start << end;
    qDebug() << index;
#endif

    if (spans.isEmpty())
    {
        return;
    }

    SpanList spansToBeDeleted;
    int ndelta = end - start + 1;
#ifdef DEBUG_SPAN_UPDATE
    qDebug("Before");
#endif

    for (SpanList::iterator it = spans.begin(); it != spans.end();)
    {
        GSpan *span = *it;
#ifdef DEBUG_SPAN_UPDATE
        qDebug() << span << *span;
#endif

        if (span->m_bottom < start)
        {
            ++it;
            continue;
        }

        if (span->m_top < start)
        {
            if (span->m_bottom <= end)
            {
                span->m_bottom = start - 1;
            }
            else
            {
                span->m_bottom -= ndelta;
            }
        }
        else
        {
            if (span->m_bottom > end)
            {
                if (span->m_top <= end)
                {
                    span->m_top = start;
                }
                else
                {
                    span->m_top -= ndelta;
                }

                span->m_bottom -= ndelta;
            }
            else
            {
                span->will_be_deleted = true;
            }
        }

        if (span->m_top == span->m_bottom && span->m_left == span->m_right)
        {
            span->will_be_deleted = true;
        }

        if (span->will_be_deleted)
        {
            spansToBeDeleted.append(span);
            it = spans.erase(it);
        }
        else
        {
            ++it;
        }
    }

#ifdef DEBUG_SPAN_UPDATE
    qDebug("After");
    foreach(GSpanCollection::GSpan * span, spans)
    qDebug() << span << *span;
#endif

    if (spans.isEmpty())
    {
        qDeleteAll(spansToBeDeleted);
        m_nIndex.clear();
        return;
    }

    Index::iterator it_y = m_nIndex.end();

    do
    {
        --it_y;
        int ny = -it_y.key();
        SubIndex &subindex = it_y.value();

        if (ny < start)
        {
            if (cleanSpanSubIndex(subindex, ny))
            {
                it_y = m_nIndex.erase(it_y);
            }
        }
        else if (ny >= start && ny <= end)
        {
            bool bspan_at_start = false;
            SubIndex spansToBeMoved;

            for (SubIndex::iterator it = subindex.begin(); it != subindex.end(); ++it)
            {
                GSpan *span = it.value();

                if (span->will_be_deleted)
                {
                    continue;
                }

                if (!bspan_at_start && span->m_top == start)
                {
                    bspan_at_start = true;
                }

                spansToBeMoved.insert(it.key(), span);
            }

            if (ny == start && bspan_at_start)
            {
                subindex.clear();
            }
            else
            {
                it_y = m_nIndex.erase(it_y);
            }

            if (bspan_at_start)
            {
                Index::iterator it_start;

                if (ny == start)
                {
                    it_start = it_y;
                }
                else
                {
                    it_start = m_nIndex.find(-start);

                    if (it_start == m_nIndex.end())
                    {
                        it_start = m_nIndex.insert(-start, SubIndex());
                    }
                }

                SubIndex &start_subindex = it_start.value();

                for (SubIndex::iterator it = spansToBeMoved.begin(); it != spansToBeMoved.end(); ++it)
                {
                    start_subindex.insert(it.key(), it.value());
                }
            }
        }
        else
        {
            if (ny == end + 1)
            {
                Index::iterator it_top = m_nIndex.find(-ny + ndelta);

                if (it_top == m_nIndex.end())
                {
                    it_top = m_nIndex.insert(-ny + ndelta, SubIndex());
                }

                for (SubIndex::iterator it = subindex.begin(); it != subindex.end();)
                {
                    GSpan *span = it.value();

                    if (!span->will_be_deleted)
                    {
                        it_top.value().insert(it.key(), span);
                    }

                    ++it;
                }
            }
            else
            {
                m_nIndex.insert(-ny + ndelta, subindex);
            }

            it_y = m_nIndex.erase(it_y);
        }
    }
    while (it_y != m_nIndex.begin());

#ifdef DEBUG_SPAN_UPDATE
    qDebug() << index;
    qDebug("Deleted");
    foreach(GSpanCollection::GSpan * span, spansToBeDeleted)
    qDebug() << span << *span;
#endif
    qDeleteAll(spansToBeDeleted);
}

/** \internal
* Updates the span collection after column removal.
*/
void GSpanCollection::updateRemovedColumns(int start, int end)
{
#ifdef DEBUG_SPAN_UPDATE
    qDebug() << Q_FUNC_INFO;
    qDebug() << start << end;
    qDebug() << index;
#endif

    if (spans.isEmpty())
    {
        return;
    }

    SpanList toBeDeleted;
    int ndelta = end - start + 1;
#ifdef DEBUG_SPAN_UPDATE
    qDebug("Before");
#endif

    for (SpanList::iterator it = spans.begin(); it != spans.end();)
    {
        GSpan *span = *it;
#ifdef DEBUG_SPAN_UPDATE
        qDebug() << span << *span;
#endif

        if (span->m_right < start)
        {
            ++it;
            continue;
        }

        if (span->m_left < start)
        {
            if (span->m_right <= end)
            {
                span->m_right = start - 1;
            }
            else
            {
                span->m_right -= ndelta;
            }
        }
        else
        {
            if (span->m_right > end)
            {
                if (span->m_left <= end)
                {
                    span->m_left = start;
                }
                else
                {
                    span->m_left -= ndelta;
                }

                span->m_right -= ndelta;
            }
            else
            {
                span->will_be_deleted = true;
            }
        }

        if (span->m_top == span->m_bottom && span->m_left == span->m_right)
        {
            span->will_be_deleted = true;
        }

        if (span->will_be_deleted)
        {
            toBeDeleted.append(span);
            it = spans.erase(it);
        }
        else
        {
            ++it;
        }
    }

#ifdef DEBUG_SPAN_UPDATE
    qDebug("After");
    foreach(GSpanCollection::GSpan * span, spans)
    qDebug() << span << *span;
#endif

    if (spans.isEmpty())
    {
        qDeleteAll(toBeDeleted);
        m_nIndex.clear();
        return;
    }

    for (Index::iterator it_y = m_nIndex.begin(); it_y != m_nIndex.end();)
    {
        int ny = -it_y.key();

        if (cleanSpanSubIndex(it_y.value(), ny, true))
        {
            it_y = m_nIndex.erase(it_y);
        }
        else
        {
            ++it_y;
        }
    }

#ifdef DEBUG_SPAN_UPDATE
    qDebug() << index;
    qDebug("Deleted");
    foreach(GSpanCollection::GSpan * span, toBeDeleted)
    qDebug() << span << *span;
#endif
    qDeleteAll(toBeDeleted);
}

#ifdef QT_BUILD_INTERNAL
/*!
  \internal
  Checks whether the span index structure is self-consistent, and consistent with the spans list.
*/
/*
bool GSpanCollection::checkConsistency() const
{
    for (Index::const_iterator it_y = index.begin(); it_y != index.end(); ++it_y)
    {
        int y = -it_y.key();
        const SubIndex &subIndex = it_y.value();
        for (SubIndex::const_iterator it = subIndex.begin(); it != subIndex.end(); ++it)
        {
            int x = -it.key();
            Span *span = it.value();
            if (!spans.contains(span) || span->left() != x
                    || y < span->top() || y > span->bottom())
            {
                return false;
            }
        }
    }

    foreach (const Span *span, spans)
    {
        if (span->width() < 1 || span->height() < 1
                || (span->width() == 1 && span->height() == 1))
        {
            return false;
        }
        for (int y = span->top(); y <= span->bottom(); ++y)
        {
            Index::const_iterator it_y = index.find(-y);
            if (it_y == index.end())
            {
                if (y == span->top())
                {
                    return false;
                }
                else
                {
                    continue;
                }
            }
            const SubIndex &subIndex = it_y.value();
            SubIndex::const_iterator it = subIndex.find(-span->left());
            if (it == subIndex.end() || it.value() != span)
            {
                return false;
            }
        }
    }
    return true;
}
*/
#endif

GLDTableViewPrivate::~GLDTableViewPrivate()
{
    m_oCommentFrames.clear();
}

void GLDTableViewPrivate::init()
{
    Q_Q(GLDTableView);

    q->setEditTriggers(m_editTriggers | GlodonAbstractItemView::AnyKeyPressed | GlodonAbstractItemView::SelectedClicked);

    GlodonHeaderView *vertical = new GlodonVHeaderView(q);
    vertical->setClickable(true);
    vertical->setHighlightSections(true);
    q->setVerticalHeader(vertical);

    GlodonHeaderView *horizontal = new GlodonHHeaderView(q);
    horizontal->setClickable(true);
    horizontal->setHighlightSections(true);
    q->setHorizontalHeader(horizontal);

    m_bTabKeyNavigation = true;

    m_cornerWidget = new GTableCornerbutton(q);
    m_cornerWidget->setFocusPolicy(Qt::NoFocus);
    QObject::connect(m_cornerWidget, SIGNAL(clicked()), q, SLOT(selectAll()));

    // override GlodonAbstractItemView default QStyledItemDelegate
    q->setItemDelegate(new GlodonDefaultItemDelegate(q));

    GlodonScrollBar *pHScrollBar = new GlodonScrollBar(Qt::Horizontal, q);
    GlodonScrollBar *pVScrollBar = new GlodonScrollBar(Qt::Vertical, q);

    q->setHorizontalScrollBar(pHScrollBar);
    q->setVerticalScrollBar(pVScrollBar);

    QObject::connect(pVScrollBar, SIGNAL(actionTriggered(int)),
                     q, SLOT(verticalScrollbarAction(int)));
    QObject::connect(pHScrollBar, SIGNAL(actionTriggered(int)),
                     q, SLOT(horizontalScrollbarAction(int)));
    QObject::connect(pVScrollBar, SIGNAL(valueChanged(int)),
                     q, SLOT(verticalScrollbarValueChanged(int)));
    QObject::connect(pHScrollBar, SIGNAL(valueChanged(int)),
                     q, SLOT(horizontalScrollbarValueChanged(int)));

    q->setProperty("TableView", "GLDTableView");
    GLDScrollStyle *scrollStyle = new GLDScrollStyle(q, false);
    scrollStyle->setIsShowArrow(false);
    q->horizontalScrollBar()->setStyle(scrollStyle);
    q->verticalScrollBar()->setStyle(scrollStyle);
}

/*!
  \internal
  Trims away indices that are hidden in the treeview due to hidden horizontal or vertical sections.
*/
void GLDTableViewPrivate::trimHiddenSelections(QItemSelectionRange *range) const
{
    Q_ASSERT(range && range->isValid());

    int ntop = range->top();
    int nleft = range->left();
    int nbottom = range->bottom();
    int nright = range->right();

    while (nbottom >= ntop && m_verticalHeader->isSectionHidden(nbottom))
    {
        --nbottom;
    }

    while (nright >= nleft && m_horizontalHeader->isSectionHidden(nright))
    {
        --nright;
    }

    if (ntop > nbottom || nleft > nright) // everything is hidden
    {
        *range = QItemSelectionRange();
        return;
    }

    while (m_verticalHeader->isSectionHidden(ntop) && ntop <= nbottom)
    {
        ++ntop;
    }

    while (m_horizontalHeader->isSectionHidden(nleft) && nleft <= nright)
    {
        ++nleft;
    }

    if (ntop > nbottom || nleft > nright) // everything is hidden
    {
        *range = QItemSelectionRange();
        return;
    }

    QModelIndex bottomRight = m_model->index(nbottom, nright, range->parent());
    QModelIndex topLeft = m_model->index(ntop, nleft, range->parent());
    *range = QItemSelectionRange(topLeft, bottomRight);
}

/*!
  \internal
  Sets the span for the cell at (\a row, \a column).
*/
void GLDTableViewPrivate::setSpan(int row, int column, int rowSpan, int columnSpan)
{
    if (row < 0 || column < 0 || rowSpan <= 0 || columnSpan <= 0)
    {
        qWarning() << "GLDTableView::setSpan: invalid span given: (" << row << ',' << column <<
                   ',' << rowSpan << ',' << columnSpan << ')';
        return;
    }

    GSpanCollection::GSpan *sp = m_spans.spanAt(column, row);

    if (sp)
    {
        if (sp->top() != row || sp->left() != column)
        {
//            qWarning() << "GLDTableView::setSpan: span cannot overlap";
            return;
        }

        if (rowSpan == 1 && columnSpan == 1)
        {
            rowSpan = columnSpan = 0;
        }

        const int c_old_height = sp->height();
        sp->m_bottom = row + rowSpan - 1;
        sp->m_right = column + columnSpan - 1;
        m_spans.updateSpan(sp, c_old_height);
        return;
    }
    else if (rowSpan == 1 && columnSpan == 1)
    {
        qWarning() << "GLDTableView::setSpan: single cell span won't be added";
        return;
    }

    sp = new GSpanCollection::GSpan(row, column, rowSpan, columnSpan);
    m_spans.addSpan(sp);
}

/*!
  \internal
  Gets the span information for the cell at (\a row, \a column).
*/
GSpanCollection::GSpan GLDTableViewPrivate::span(int row, int column) const
{
    GSpanCollection::GSpan *sp = m_spans.spanAt(column, row);

    if (sp)
    {
        return *sp;
    }

    return GSpanCollection::GSpan(row, column, 1, 1);
}

/*!
  \internal
  Returns the logical index of the last section that's part of the span.
*/
int GLDTableViewPrivate::sectionSpanEndLogical(const GlodonHeaderView *header, int logical, int span) const
{
    int nVisual = header->visualIndex(logical);

    for (int i = 1; i < span;)
    {
        if (++nVisual >= header->count())
        {
            break;
        }

        logical = header->logicalIndex(nVisual);
        ++i;
    }

    return logical;
}

/*!
  \internal
  Returns the size of the span starting at logical index \a logical
  and spanning \a span sections.
*/
int GLDTableViewPrivate::sectionSpanSize(const GlodonHeaderView *header, int logical, int span) const
{
    int nendLogical = sectionSpanEndLogical(header, logical, span);
    return header->sectionPosition(nendLogical)
           - header->sectionPosition(logical)
           + header->sectionSize(nendLogical);
}

/*!
  \internal
  Returns true if the section at logical index \a logical is part of the span
  starting at logical index \a spanLogical and spanning \a span sections;
  otherwise, returns false.
*/
bool GLDTableViewPrivate::spanContainsSection(const GlodonHeaderView *header, int logical,
        int spanLogical, int span) const
{
    if (logical == spanLogical)
    {
        return true; // it's the start of the span
    }

    int nVisual = header->visualIndex(spanLogical);

    for (int i = 1; i < span;)
    {
        if (++nVisual >= header->count())
        {
            break;
        }

        spanLogical = header->logicalIndex(nVisual);

        if (logical == spanLogical)
        {
            return true;
        }

        ++i;
    }

    return false;
}

/*!
  \internal
  Returns the visual rect for the given \a span.
*/
QRect GLDTableViewPrivate::visualSpanRect(const GSpanCollection::GSpan &span) const
{
    Q_Q(const GLDTableView);

    int nRow;
    int nColumn;
    spanTopRowLeftColumn(span, nRow, nColumn);

    QModelIndex oTopLeftIndex = m_model->index(nRow, nColumn, m_root);
    QVariant oBoundLine = oTopLeftIndex.data(gidrBoundLineRole);

    QRect oVisualSpanRect = visualSpanRectIncludeBoundingLineWidth(span);
    oVisualSpanRect.setWidth(oVisualSpanRect.width() - q->borderLineSubtractGridLineWidth(oBoundLine, true));
    oVisualSpanRect.setHeight(oVisualSpanRect.height() - q->borderLineSubtractGridLineWidth(oBoundLine, false));

    return oVisualSpanRect;
}

QRect GLDTableViewPrivate::visualSpanRectIncludeBoundingLineWidth(const GSpanCollection::GSpan &span) const
{
    int nRow;
    int nColumn;
    spanTopRowLeftColumn(span, nRow, nColumn);

    int nRowP = m_verticalHeader->sectionViewportPosition(nRow);
    int nRowH = rowSpanHeight(nRow, span.height());

    int nColP = m_horizontalHeader->sectionViewportPosition(nColumn);
    int nColW = columnSpanWidth(nColumn, span.width());

    return QRect(nColP, nRowP, nColW, nRowH);
}

QRect GLDTableViewPrivate::visualRectIncludeBoundingLineWidth(const QModelIndex &index) const
{
    Q_Q(const GLDTableView);

    int nRowP = q->rowViewportPosition(index.row());
    int nRowH = q->rowHeight(index.row());
    int nColP = q->columnViewportPosition(index.column());
    int nColW = q->columnWidth(index.column());

    return QRect(nColP, nRowP, nColW, nRowH);
}

void GLDTableViewPrivate::spanTopRowLeftColumn(const GSpanCollection::GSpan &span, int &row, int &column) const
{
    Q_Q(const GLDTableView);
    // vertical
    row = span.top();

    // horizontal
    column = span.left();

    if (q->isRightToLeft())
    {
        column = span.right();
    }
}

QRect GLDTableViewPrivate::viewportScrollArea(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal)
    {
        int nleft = 0;

//        for (int i = 0; i < m_fixedColCount; i++)
//        {
//            nleft += m_horizontalHeader->sectionSize(i);
//        }

        return QRect(nleft, 0, viewport->width() - nleft, viewport->height());
    }
    else
    {
        int ntop = 0;

//        for (int i = 0; i < m_fixedRowCount; i++)
//        {
//            ntop += m_verticalHeader->sectionSize(i);
//        }

        return QRect(0, ntop, viewport->width(), viewport->height() - ntop);
    }
}

QList<GSpanCollection::GSpan *> GLDTableViewPrivate::getVisibleSpansAccordingToIsSectionMove(
        int firstVisualRow, int firstVisualColumn, int lastVisualRow, int lastVisualColumn)
{
    QList<GSpanCollection::GSpan *> visibleSpans;
    bool bSectionMoved = m_verticalHeader->sectionsMoved() || m_horizontalHeader->sectionsMoved();

    if (!bSectionMoved)
    {
        visibleSpans = m_spans.spansInRect(logicalColumn(firstVisualColumn), logicalRow(firstVisualRow),
                                           lastVisualColumn - firstVisualColumn + 1, lastVisualRow - firstVisualRow + 1);
    }
    else
    {
        QSet<GSpanCollection::GSpan *> set;

        for (int i = firstVisualColumn; i <= lastVisualColumn; i++)
        {
            for (int j = firstVisualRow; j <= lastVisualRow; j++)
            {
                set.insert(m_spans.spanAt(i, j));
            }
        }

        set.remove(0);
        visibleSpans = set.toList();
    }

    return visibleSpans;
}

void GLDTableViewPrivate::adjustOptionAlternateFeature(QStyleOptionViewItem &opt, bool bAlternateBase)
{
    if (bAlternateBase)
    {
        opt.features |= QStyleOptionViewItem::Alternate;
    }
    else
    {
        opt.features &= ~QStyleOptionViewItem::Alternate;
    }
}

bool GLDTableViewPrivate::isRangeFillingHandleSelected(const QPoint &p)
{
    // 如果处于多选状态，则不允许拖拽复制
    if (isInMutiSelect() || m_selectionModel.isNull())
    {
        return false;
    }

    return m_oRangeFillHandleRect.contains(p);
}

bool GLDTableViewPrivate::isInMutiSelect() const
{
    return m_bIsInMultiSelect;
}

void GLDTableViewPrivate::drawComment(const QPersistentModelIndex &index)
{
    Q_Q(GLDTableView);

    // 显示注释内容
    if (q->state() == GlodonAbstractItemView::DragSelectingState)
        return;

    if (m_pCommentFrame == NULL)
    {
        m_pCommentFrame = new GCommentFrame(q);
    }

    if (index.isValid())
    {
        GString sComment = q->model()->data(index, gidrCommentRole).toString();

        if (sComment.isEmpty())
        {
            m_pCommentFrame->hide();
        }
        else
        {
            m_pCommentFrame->setCommentText(sComment);
            m_pCommentFrame->move(q->visualRect(index).topRight() + q->viewport()->pos());
            m_pCommentFrame->show();
        }
    }
    else
    {
        m_pCommentFrame->hide();
    }
}

void GLDTableViewPrivate::hideCommentFrame()
{
    if (m_pCommentFrame != NULL)
    {
        m_pCommentFrame->hide();
    }
}

void GLDTableViewPrivate::setGridLineWidthToHorizontalHeader(int gridLineWidth)
{
    if (m_horizontalHeader == NULL)
        return;

    m_horizontalHeader->setGridLineWidth(gridLineWidth);
}

void GLDTableViewPrivate::setGridLineWidthToVertialHeader(int gridLineWidth)
{
    if (m_verticalHeader == NULL)
        return;

    m_verticalHeader->setGridLineWidth(gridLineWidth);
}

int GLDTableViewPrivate::heightHintForIndex(const QModelIndex &index, int nHint, QStyleOptionViewItem &option) const
{
    Q_Q(const GLDTableView);

    QWidget *editor = editorForIndex(index).widget.data();

    if (editor && m_persistent.contains(editor))
    {
        nHint = qMax(nHint, editor->sizeHint().height());
        int nMin = editor->minimumSize().height();
        int nMax = editor->maximumSize().height();
        nHint = qBound(nMin, nHint, nMax);
    }

    if (m_bWrapItemText) // for wrapping boundaries
    {
        option.rect.setY(q->rowViewportPosition(index.row()));

        int nHeight = q->rowHeight(index.row());
        // if the option.height == 0 then q->itemDelegate(index)->sizeHint(option, index) will be wrong.
        // The option.height == 0 is used to conclude that the text is not wrapped, and hence it will
        // (exactly like widthHintForIndex) return a QSize with a long width (that we don't use) -
        // and the height of the text if it was/is on one line.
        // What we want is a height hint for the current width (and we know that this section is not hidden)
        // Therefore we catch this special situation with:
        if (nHeight == 0)
            nHeight = 1;

        option.rect.setHeight(nHeight);
        option.rect.setX(q->columnViewportPosition(index.column()));
        option.rect.setWidth(q->columnWidth(index.column()));
    }

    adjustRectWidthByTreeColumn(index, option.rect);
    adjustRectWidthBySpan(index, option.rect);
    // 水平margin的处理放到了delegate中

    const int c_vertMargin = q->cellMargin(index, TopMargin);
    return qMax(nHint, q->itemDelegate(index)->sizeHint(option, index).height() + c_vertMargin * 2);
}

int GLDTableViewPrivate::calcSizeHintForRow(int row, const QList<int> &oNeedCalcSizeHintCols) const
{
    Q_Q(const GLDTableView);

    if (!q->model())
        return -1;

    q->ensurePolished();

    QStyleOptionViewItem option = viewOptionsV4();

    int nHint = 0;
    QModelIndex index;

    for (int i = 0; i < oNeedCalcSizeHintCols.count(); ++i)
    {
        int nLogicalColumn = m_horizontalHeader->logicalIndex(oNeedCalcSizeHintCols.at(i));

        if (m_horizontalHeader->isSectionHidden(nLogicalColumn))
        {
            continue;
        }

        index = m_model->index(row, nLogicalColumn, m_root);
        nHint = heightHintForIndex(index, nHint, option);
    }

    return nHint;
}

int GLDTableViewPrivate::widthHintForIndex(const QModelIndex &index, int hint, const QStyleOptionViewItem &option) const
{
    Q_Q(const GLDTableView);

    QWidget *editor = editorForIndex(index).widget.data();

    if (editor && m_persistent.contains(editor))
    {
        hint = qMax(hint, editor->sizeHint().width());
        int nMin = editor->minimumSize().width();
        int nMax = editor->maximumSize().width();
        hint = qBound(nMin, hint, nMax);
    }

    //如果是树形结构显示，则因加上树形结构偏移
    const int c_nTreeOffset = q->treeCellDisplayHorizontalOffset(index.row(), index.column());
    hint = qMax(hint, q->itemDelegate(index)->sizeHint(option, index).width() + c_nTreeOffset);
    return hint;
}

int GLDTableViewPrivate::widthHintForIndex(
        int visualRow, int column, int hint, const QStyleOptionViewItem &option) const
{
    int nLogicalRow = m_verticalHeader->logicalIndex(visualRow);

    if (m_verticalHeader->isSectionHidden(nLogicalRow))
    {
        return hint;
    }

    QModelIndex index = m_model->index(nLogicalRow, column, m_root);
    return widthHintForIndex(index, hint, option);
}

int GLDTableViewPrivate::sizeHintForColumn(int column, bool calcAllRows) const
{
    Q_Q(const GLDTableView);

    if (!q->model())
    {
        return -1;
    }

    q->ensurePolished();

    QStyleOptionViewItem option = viewOptionsV4();

    int nHint = 0;
    for (int row = 0; row < m_nFixedRowCount; ++row)
    {
        nHint = widthHintForIndex(row, column, nHint, option);
    }

    int nTop = q->firstVisualRow();
    int nBottom = q->lastVisualRow();

    if (calcAllRows)
    {
        nTop = 0;
        nBottom = m_verticalHeader->count() - 1;
    }
    else
    {
        q->adjustVisualRow(nTop, nBottom);
    }

    if (!q->isVisible() || nBottom == -1) // the table don't have enough rows to fill the viewport
    {
        nBottom = m_verticalHeader->count() - 1;
    }

    for (int row = nTop; row <= nBottom; ++row)
    {
        nHint = widthHintForIndex(row, column, nHint, option);
    }

    return nHint;
}

void GLDTableViewPrivate::calcFixedColSuitWidth(bool &shouldResetCommentPosition)
{
    QMap<int, int> oUpdateFixedColWidths;
    if (m_nFixedColCount > 0)
    {
        for (int nCol = 0; nCol < m_nFixedColCount; ++nCol)
        {
            if (isColumnHidden(nCol) || !m_oSuitColWidthCols.contains(nCol))
                continue;

            oUpdateFixedColWidths.insert(nCol, calcSuitWidth(nCol, m_bCalcAllRows));
            shouldResetCommentPosition = true;
        }

        if (oUpdateFixedColWidths.count() > 0)
        {
            m_horizontalHeader->batchResizeSection(oUpdateFixedColWidths);
        }
    }
}

void GLDTableViewPrivate::calcNormalColSuitWidth(bool &shouldResetCommentPosition)
{
    Q_Q(GLDTableView);

    int nCurVisualCol = q->firstVisualCol();

    // dousp 当前第一可见列小于0时，不进行操作
    if (nCurVisualCol < 0)
    {
        return;
    }

    int nColCount = m_horizontalHeader->count();

    int nViewportWidth = viewport->width();

    int nVisualColWidthTotal = 0;

    QMap<int, int> oUpdateColWidths;

    // TODO liurx 如果最后一列是合适列宽列，且倒数第二列的列宽超过viewport的宽度时，最后一列合适列宽不会计算。这是一个bug
    while ((nVisualColWidthTotal <= nViewportWidth) && (nCurVisualCol < nColCount))
    {
        int nCurLogicalCol = m_horizontalHeader->logicalIndex(nCurVisualCol);

        if (isColumnHidden(nCurLogicalCol))
        {
            nCurVisualCol++;
            continue;
        }

        int nCurOldColWidth = q->columnWidth(nCurLogicalCol);

        if (m_oSuitColWidthCols.indexOf(nCurLogicalCol) < 0)
        {
            nVisualColWidthTotal += nCurOldColWidth;
            nCurVisualCol++;
            continue;
        }

        const int nMaxWidth = calcSuitWidth(nCurLogicalCol, m_bCalcAllRows);
        if (nMaxWidth != nCurOldColWidth)
        {
            shouldResetCommentPosition = true;
            oUpdateColWidths.insert(nCurLogicalCol, nMaxWidth);
            m_columnsToUpdate.append(nCurLogicalCol);
        }

        nVisualColWidthTotal += nMaxWidth;
        nCurVisualCol++;
    }

    m_horizontalHeader->batchResizeSection(oUpdateColWidths);
}

void GLDTableViewPrivate::calcFixedRowSuitHeight(const QList<int> &oNeedCalcHeightCols, bool &shouldResetCommentPosition)
{
    Q_Q(GLDTableView);

    QMap<int, int> oUpdateRowHeights;
    for (int nRow = 0; nRow < m_nFixedRowCount; ++nRow)
    {
        int nCurLogicalRow = m_verticalHeader->logicalIndex(nRow);

        if (nCurLogicalRow < 0)
            return;

        if (q->isRowHidden(nCurLogicalRow))
            continue;

        const int nMaxRowHeight = calcSuitHeight(oNeedCalcHeightCols, nCurLogicalRow);

        if (nMaxRowHeight != q->rowHeight(nCurLogicalRow))
        {
            shouldResetCommentPosition = true;
            oUpdateRowHeights.insert(nCurLogicalRow, nMaxRowHeight);
            m_rowsToUpdate.append(nCurLogicalRow);
        }
    }
    m_verticalHeader->batchResizeSection(oUpdateRowHeights);
}

void GLDTableViewPrivate::calcNormalRowSuitHeight(const QList<int> &oNeedCalcHeightCols, bool &shouldResetCommentPosition)
{
    Q_Q(GLDTableView);

    int nTotalRowHeight = 0;
    int nCurVisualRow = q->firstVisualRow();

    int nRowCount = m_verticalHeader->count();
    QMap<int, int> oUpdateRowHeights;

    while ((nTotalRowHeight <= viewport->height()) && (nCurVisualRow < nRowCount))
    {
        int nCurLogicalRow = m_verticalHeader->logicalIndex(nCurVisualRow);

        if (nCurLogicalRow < 0)
            return;

        if (q->isRowHidden(nCurLogicalRow))
        {
            nCurVisualRow++;
            continue;
        }

        const int nMaxRowHeight = calcSuitHeight(oNeedCalcHeightCols, nCurLogicalRow);

        if (nMaxRowHeight != q->rowHeight(nCurLogicalRow))
        {
            shouldResetCommentPosition = true;
            oUpdateRowHeights.insert(nCurLogicalRow, nMaxRowHeight);
            m_rowsToUpdate.append(nCurLogicalRow);
        }

        nTotalRowHeight += nMaxRowHeight + q->currentGridLineWidth();
        nCurVisualRow++;
    }

    m_verticalHeader->batchResizeSection(oUpdateRowHeights);
}

int GLDTableViewPrivate::calcSuitWidth(int nCol, bool calcAllRows)
{
    Q_Q(GLDTableView);

    int nHorizontalHeaderSectionSizeHint = m_horizontalHeader->sectionSizeHint(nCol);
    QModelIndex oCurIndex = q->model()->index(0, nCol);

    const int nDefaultColWidth = q->dataIndex(oCurIndex).data(gidrColWidthRole).toInt();
    int nCalcColWidth = sizeHintForColumn(nCol, calcAllRows);
    nCalcColWidth = qMax(nHorizontalHeaderSectionSizeHint, nCalcColWidth);

    return qMax(nCalcColWidth, nDefaultColWidth);
}

int GLDTableViewPrivate::calcSuitHeight(const QList<int> &oNeedCalcHeightCols, int nLogicalRow)
{
    Q_Q(GLDTableView);

    QModelIndex oCurIndex = q->model()->index(nLogicalRow, 0);

    const int nCalcRowHeight = calcSizeHintForRow(nLogicalRow, oNeedCalcHeightCols);
    const int nDefaultRowHeight = defaultRowHeight(oCurIndex);

    return qMax(nDefaultRowHeight, nCalcRowHeight);
}

void GLDTableViewPrivate::adjustRectWidthByTreeColumn(const QModelIndex &index, QRect &rect) const
{
    Q_Q(const GLDTableView);
    rect.setWidth(rect.width() - q->treeCellDisplayHorizontalOffset(index.row(), index.column(), false));
}

void GLDTableViewPrivate::adjustRectWidthBySpan(const QModelIndex &index, QRect &rect) const
{
    Q_Q(const GLDTableView);
    int nColumnSpanCount = columnSpan(index.row(), index.column());

    int nSpanExtraWidth = 0;
    for (int i = 1; i < nColumnSpanCount; ++i)
    {
        nSpanExtraWidth += (q->columnWidth(index.column() + i) + q->gridLineWidth());
    }
    rect.setWidth(rect.width() + nSpanExtraWidth);
}

QColor GLDTableViewPrivate::getPenColor()
{
    Q_Q(GLDTableView);

    if (q->hasFocus())
    {
        return m_selectBoundLineColor;
    }
    else
    {
        if (q->isEditing())
        {
            return m_selectBoundLineColor;
        }
        else
        {
            return m_oNoFocusSelectedBoundLineColor;
        }
    }
}

bool GLDTableViewPrivate::isNearHorizontalGridLine(QMouseEvent *event)
{
    return m_horizontalHeader->getSectionHandleAt(event->x()) != -1;
}

bool GLDTableViewPrivate::isNearVerticalGridLine(QMouseEvent *event)
{
    return m_verticalHeader->getSectionHandleAt(event->y()) != -1;
}

void GLDTableViewPrivate::doAdjustColWidths(int currentResizeCol)
{
    GIntList oOriginColWidths = getOriginColWidthsFromView();

    // 如果没有列时，直接返回
    if (oOriginColWidths.size() == 0)
    {
        return;
    }

    m_nCurrentResizeCol = currentResizeCol;
    // 计算合适列宽
    adjustFitColWidths(oOriginColWidths);
}

void GLDTableViewPrivate::adjustFitColWidths(GIntList &originColWidths)
{
    int nCurrentResizeColWidth = 0;
    if (m_nCurrentResizeCol != -1)
    {
        nCurrentResizeColWidth = m_currResizeWidth;
    }

    // 获取设置了合适列宽列的宽度
    GIntList oAdjustedColsWidth = getAdjustedColWidths(originColWidths, nCurrentResizeColWidth);

    // 获取现有全部列的列宽.(如果当前正在调整某列宽度，就用调整后的宽度计算)
    const int nOriginTotalColWidth = getTotalColWidth(originColWidths, nCurrentResizeColWidth);
    // 比较是否需要计算合适列宽时，合适列宽列应该用默认列宽计算
    int nMinOriginTotalColWidth = getMinTotalColWidth(originColWidths, nOriginTotalColWidth);

    const int nViewportWidth = viewport->width();

    // 如果现在列的宽度已经大于viewport的宽度了，那就不需要计算了
    if (nMinOriginTotalColWidth >= nViewportWidth)
    {
        if (m_nCurrentResizeCol != -1)
        {
            originColWidths.replace(m_nCurrentResizeCol, nCurrentResizeColWidth);
        }

        applyAdjustFitColsWidth(originColWidths, oAdjustedColsWidth);
    }
    else
    {
        int nOldAdjustColsTotalWidth = 0;
        int nCurrentResizeColIndexInFitCols = -1;

        // 先计算合适列宽的列的总宽度（除当前正在调整列宽的列）
        calcOldAdjustColsTotalWidth(oAdjustedColsWidth, nOldAdjustColsTotalWidth, nCurrentResizeColIndexInFitCols);

        int nNewAdjustColsTotalWidth = 0;
        int nAvailableAdjustWidth = nViewportWidth - nOriginTotalColWidth;

        // 调整合适列宽每列的宽度，使屏幕充满
        calcAdjustColsWidth(nAvailableAdjustWidth, nOldAdjustColsTotalWidth, oAdjustedColsWidth, nNewAdjustColsTotalWidth);

        // 当前调整列宽的列为合适列宽的列
        if (nCurrentResizeColIndexInFitCols != -1)
        {
            // TODO liurx, 既然是合适列宽的列，宽度总是需要调整的，为什么还需要特殊处理并写回到originWidths
            adjustOriginColWidthWhenResizeColIsInFitCols(
                        oAdjustedColsWidth, nCurrentResizeColIndexInFitCols,
                        nCurrentResizeColWidth, originColWidths);
        }
        else if (m_nCurrentResizeCol != -1)
        {
            originColWidths.replace(m_nCurrentResizeCol, nCurrentResizeColWidth);
        }

        // 把计算的误差，加到最后一个合适列宽列上
        nAvailableAdjustWidth = nOldAdjustColsTotalWidth + nAvailableAdjustWidth - nNewAdjustColsTotalWidth;
        oAdjustedColsWidth.replace(oAdjustedColsWidth.size() - 1, oAdjustedColsWidth.last() + nAvailableAdjustWidth);

        applyAdjustFitColsWidth(originColWidths, oAdjustedColsWidth);
    }
}

void GLDTableViewPrivate::mergeOriginColsWidthAndAdjustColsWidth(
        GIntList &colWidths, const GIntList &originColsWidth, const GIntList &adjustColsWidth)
{
    for (int i = 0; i < originColsWidth.size(); ++i)
    {
        if (i != m_nCurrentResizeCol)
        {
            colWidths.append(originColsWidth.at(i));
        }
        else
        {
            colWidths.append(m_currResizeWidth);
        }
    }

    for (int i = 0; i < m_oFitColWidthCols.size(); ++i)
    {
        if (colWidths.at(m_oFitColWidthCols.at(i)) != 0)
        {
            colWidths.replace(m_oFitColWidthCols.at(i), adjustColsWidth.at(i));//自动列宽调整
        }
    }
}

void GLDTableViewPrivate::applyAdjustFitColsWidth(
        const GIntList &originColsWidth, const GIntList &adjustColsWidth)
{
    Q_ASSERT(adjustColsWidth.size() == m_oFitColWidthCols.size());

    GIntList colWidths;
    mergeOriginColsWidthAndAdjustColsWidth(colWidths, originColsWidth, adjustColsWidth);

    QMap<int, int> oUpdateColsWidth;
    for (int i = 0; i < originColsWidth.size(); ++i)
    {
        oUpdateColsWidth.insert(i, colWidths.at(i));
    }
    m_horizontalHeader->batchResizeSection(oUpdateColsWidth);
}

GIntList GLDTableViewPrivate::getAdjustedColWidths(
        const GIntList &originColWidths, int currentResizeColWidth)
{
    GIntList oAdjustedColWidths;

    for (int i = 0; i < m_oFitColWidthCols.size(); ++i)
    {
        if (m_oFitColWidthCols.at(i) != m_nCurrentResizeCol)
        {
            oAdjustedColWidths.append(originColWidths.at(m_oFitColWidthCols.at(i)));
        }
        else
        {
            oAdjustedColWidths.append(currentResizeColWidth);
        }
    }

    return oAdjustedColWidths;
}

void GLDTableViewPrivate::adjustOriginColWidthWhenResizeColIsInFitCols(
        const GIntList &adjustedColWidths, int currentResizeColIndexInFitCols, int currentResizeColWidth,
        GIntList &originColWidths)
{
    if ((m_oFitColWidthCols.size() == 1))
    {
        if (m_nCurrentResizeCol != -1)
        {
            originColWidths.replace(m_nCurrentResizeCol, currentResizeColWidth);
        }
    }
    else
    {
        int nIndex = 0;

        for (int i = 0; i < m_oFitColWidthCols.size(); ++i)
        {
            if (m_oFitColWidthCols.at(i) != m_nCurrentResizeCol)
            {
                nIndex = i;
                break;
            }
        }

        if (adjustedColWidths.at(nIndex) != 0)
        {
            currentResizeColWidth = (int)(originColWidths.at(m_oFitColWidthCols.at(nIndex))
                                           * adjustedColWidths.at(currentResizeColIndexInFitCols) / adjustedColWidths.at(nIndex));
        }

        originColWidths.replace(m_nCurrentResizeCol, currentResizeColWidth);
    }
}

void GLDTableViewPrivate::calcOldAdjustColsTotalWidth(
        const GIntList &oAdjustedColsWidth, int &nOldAdjustColsTotalWidth, int &nCurrentResizeColIndexInFitCols)
{
    Q_Q(GLDTableView);

    for (int i = 0; i < m_oFitColWidthCols.size(); ++i)
    {
        int nCurrentCol = m_oFitColWidthCols.at(i);
        if (nCurrentCol != m_nCurrentResizeCol)
        {
            nOldAdjustColsTotalWidth += oAdjustedColsWidth.at(i) + (isColumnHidden(nCurrentCol) ? 0 : q->currentGridLineWidth());
        }
        else
        {
            nCurrentResizeColIndexInFitCols = i;
        }
    }
}

void GLDTableViewPrivate::calcAdjustColsWidth(
        const int nAvailableAdjustWidth, const int nOldAdjustColsTotalWidth, GIntList &oAdjustedColsWidth,
        int &nNewAdjustColsTotalWidth)
{
    Q_Q(GLDTableView);

    double dAdjustFactor = 0;

    if (nOldAdjustColsTotalWidth != 0)
    {
        dAdjustFactor = (nOldAdjustColsTotalWidth + nAvailableAdjustWidth) * 1.0 / nOldAdjustColsTotalWidth;
    }
    else
    {
        dAdjustFactor = 0;
    }

    for (int i = 0; i < oAdjustedColsWidth.size(); ++i)
    {
        if (m_oFitColWidthCols.at(i) != m_nCurrentResizeCol)
        {
            oAdjustedColsWidth.replace(i, (int)(oAdjustedColsWidth.at(i) * dAdjustFactor));
            nNewAdjustColsTotalWidth += oAdjustedColsWidth.at(i) + (oAdjustedColsWidth.at(i) == 0 ? 0 : q->currentGridLineWidth());
        }
    }
}

int GLDTableViewPrivate::getTotalColWidth(const GIntList &originColsWidth, int nColWidth)
{
    Q_Q(GLDTableView);

    int nOriginTotalColWidth = 0;

    for (int i = 0; i < originColsWidth.size(); ++i)
    {
        nOriginTotalColWidth += originColsWidth.at(i) + (isColumnHidden(i) ? 0 : q->currentGridLineWidth());
    }

    if (m_nCurrentResizeCol != -1)
    {
        nOriginTotalColWidth = nOriginTotalColWidth - originColsWidth.at(m_nCurrentResizeCol) + nColWidth;
    }

    return nOriginTotalColWidth;
}

GIntList GLDTableViewPrivate::getOriginColWidthsFromView()
{
    GIntList oOriginColWidths;

    // TODO liurx delphi获取originColWidth时，如果是合适列宽列的话获取的是calcedWidth，否则获取的是displayWidth
    for (int i = 0; i < m_horizontalHeader->count(); ++i)
    {
        int nColWidth = m_horizontalHeader->sectionSize(i);
        oOriginColWidths.push_back(nColWidth);
    }

    return oOriginColWidths;
}

int GLDTableViewPrivate::getMinTotalColWidth(const GIntList &originColWidths, const int nOriginTotalColWidth)
{
    int nMinOriginTotalColWidth = nOriginTotalColWidth;
    for (int i = 0; i < originColWidths.count(); ++i)
    {
        if ((i != m_nCurrentResizeCol) && m_oFitColWidthCols.contains(i))
        {
            int nDefaultColWidth = getDefaultColWidth(i);
            // 当合适列宽列的宽度小于默认值时，取实际宽度即可，否则差值会被加到其他合适列宽列上
            // 导致其他合适列宽列的宽度大于默认值时，就会出现滚动条
            if (nDefaultColWidth < originColWidths.at(i))
            {
                nMinOriginTotalColWidth = nMinOriginTotalColWidth - originColWidths.at(i) + nDefaultColWidth;
            }
        }
    }
    return nMinOriginTotalColWidth;
}

int GLDTableViewPrivate::getDefaultColWidth(int col)
{
    Q_Q(GLDTableView);

    QModelIndex oIndex = q->model()->index(0, col);
    QVariant oColWidth = oIndex.data(gidrColWidthRole);

    int nColWidth = -1;
    if (oColWidth.isValid())
    {
        nColWidth = oColWidth.toInt();
    }
    else
    {
        nColWidth = m_horizontalHeader->defaultSectionSize();
    }

    return nColWidth;
}

bool GLDTableViewPrivate::isVisibleIndex(const QModelIndex &visualIndex)
{
    Q_Q(GLDTableView);

    bool bIsVisibleInHorizontal = (visualIndex.column() >= q->firstVisualCol()) && (visualIndex.column() <= q->lastVisualCol());
    if (m_nFixedColCount > 0)
    {
        bIsVisibleInHorizontal = bIsVisibleInHorizontal || (visualIndex.column() >= 0 && visualIndex.column() < m_nFixedColCount);
    }

    bool bIsVisibleInVertical = (visualIndex.row() >= q->firstVisualRow()) && (visualIndex.row() <= q->lastVisualRow());
    if (m_nFixedRowCount > 0)
    {
        bIsVisibleInVertical = bIsVisibleInVertical || (visualIndex.row() >= 0 && visualIndex.row() < m_nFixedRowCount);
    }

    return bIsVisibleInHorizontal && bIsVisibleInVertical;
}

int GLDTableViewPrivate::defaultRowHeight(const QModelIndex &logicalIndex)
{
    int nDefaultRowHeight = m_verticalHeader->defaultSectionSize();
    QVariant oDefaultRowHeight = logicalIndex.data(gidrRowHeightRole);
    if (oDefaultRowHeight.isValid())
    {
        nDefaultRowHeight = oDefaultRowHeight.toInt();
    }

    return nDefaultRowHeight;
}

bool GLDTableViewPrivate::isInTableView(QMouseEvent *event)
{
    QPoint oGlobalBottomRight = viewport->mapToGlobal(viewport->rect().bottomRight());
    QPoint oMousePoint = QCursor::pos();
    bool bInViewport = oGlobalBottomRight.x() > oMousePoint.x() && oGlobalBottomRight.y() > oMousePoint.y();

    return bInViewport;
}

QPoint GLDTableViewPrivate::calcResizeInfoLineFramePosition(Qt::Orientation direction)
{
    QPoint oDestPoint;
    if (direction == Qt::Horizontal)
    {
        oDestPoint = viewport->mapToGlobal(
                    QPoint(m_horizontalHeader->mousePosition().x(), 0));
    }
    else
    {
        oDestPoint = viewport->mapToGlobal(
                    QPoint(0, m_verticalHeader->mousePosition().y()));
    }

    return oDestPoint;
}

void GLDTableViewPrivate::fillSuitColWidthCols()
{
    Q_Q(GLDTableView);

    if (m_bAllColumnsResizeToContents)
    {
        m_oSuitColWidthCols.clear();
        for (int nCol = 0; nCol < q->model()->columnCount(); ++nCol)
        {
            m_oSuitColWidthCols << nCol;
        }
    }
}

void GLDTableViewPrivate::fillSuitRowHeightCols()
{
    Q_Q(GLDTableView);

    if (m_bAllRowsResizeToContents)
    {
        m_oSuitRowHeightAccordingCols.clear();
        for (int nCol = 0; nCol < q->model()->columnCount(); ++nCol)
        {
            m_oSuitRowHeightAccordingCols << nCol;
        }
    }
}

/*!
  \internal
  Draws the spanning cells within rect \a area, and clips them off as
  preparation for the main drawing loop.
  \a drawn is a QBitArray of visualRowCountxvisualCoulumnCount which say if particular cell has been drawn
*/
void GLDTableViewPrivate::drawAndClipSpans(const QRegion &area, QPainter *painter,
        const QStyleOptionViewItem &option, QBitArray *drawn,
        int firstVisualRow, int lastVisualRow, int firstVisualColumn,
        int lastVisualColumn)
{
    Q_Q(GLDTableView);

    bool bAlternateBase = false;
    QRegion excludeSpansRegion = viewport->rect();

    QList<GSpanCollection::GSpan *> visibleSpans = getVisibleSpansAccordingToIsSectionMove(
                firstVisualRow, firstVisualColumn, lastVisualRow, lastVisualColumn);

    foreach(GSpanCollection::GSpan * span, visibleSpans)
    {
        int row = span->top();
        int col = span->left();

        if (m_verticalHeader->isSectionHidden(row) || m_horizontalHeader->isSectionHidden(col))
        {
            continue;
        }

        QModelIndex index = m_model->index(row, col, m_root);

        if (!index.isValid())
        {
            continue;
        }

        QRect rect = visualSpanRect(*span);
        rect.translate(m_scrollDelayOffset);

        if (isCoverByFixedColumn(index.column(), rect.right(), false) || !area.intersects(rect))
        {
            continue;
        }

        if (isCoverByFixedRow(index.row(), rect.bottom(), false) || !area.intersects(rect))
        {
            continue;
        }

        QStyleOptionViewItem opt = option;
        opt.rect = rect;

        // 如果合并格被固定列覆盖了一部分，那应该只绘制未被覆盖的部分
        QRect oPainterRect = rect;
        if (rect.left() < m_nFixedColWidth && rect.right() > m_nFixedColWidth)
        {
            oPainterRect.setLeft(m_nFixedColWidth);
        }

        // 如果合并格被固定行覆盖了一部分，那应该只绘制未被覆盖的部分
        if (rect.top() < m_nFixedRowHeight && rect.bottom() > m_nFixedRowHeight)
        {
            oPainterRect.setTop(m_nFixedRowHeight);
        }

        bAlternateBase = m_bAlternatingColors && (1 == (span->top() & 1));
        adjustOptionAlternateFeature(opt, bAlternateBase);

        painter->setClipRect(oPainterRect);

        drawCell(painter, opt, index);

        //合并单元格的计算貌似在水平方向差了一个像素，安装着急发版，先这么处理下，后续需要找一下这一像素的差距的原因
        //bug GQI-40275
        if(m_horizontalHeader->isSectionHidden(span->right()))
            oPainterRect.adjust(0, 0, - q->gridLineWidth(), 0);
        if(m_verticalHeader->isSectionHidden(span->bottom()))
            oPainterRect.adjust(0, 0, 0, - q->gridLineWidth());

        excludeSpansRegion -= oPainterRect;

        for (int row = span->top(); row <= span->bottom(); ++row)
        {
            const int c_vr = visualRow(row);

            if (c_vr < firstVisualRow || c_vr > lastVisualRow)
            {
                continue;
            }

            for (int col = span->left(); col <= span->right(); ++col)
            {
                const int c_vc = visualColumn(col);

                if (c_vc < firstVisualColumn || c_vc > lastVisualColumn)
                {
                    continue;
                }

                drawn->setBit((c_vr - firstVisualRow) * (lastVisualColumn - firstVisualColumn + 1)
                              + c_vc - firstVisualColumn);
            }
        }
    }
    painter->setClipRegion(excludeSpansRegion);
}

void GLDTableViewPrivate::_q_updateSpanInsertedRows(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    m_spans.updateInsertedRows(start, end);
}

void GLDTableViewPrivate::_q_updateSpanInsertedColumns(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    m_spans.updateInsertedColumns(start, end);
}

void GLDTableViewPrivate::_q_updateSpanRemovedRows(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    m_spans.updateRemovedRows(start, end);
}

void GLDTableViewPrivate::_q_updateSpanRemovedColumns(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    m_spans.updateRemovedColumns(start, end);
}

void GLDTableViewPrivate::drawTriangles(QRect rect, QPainter *painter)
{
    // TODO liurx 绘制得有点瑕疵，和excel不一样
    QPolygon oPoints;
    oPoints.append(QPoint(rect.right() - c_nCommentTriangleRightLength, rect.top()));
    oPoints.append(QPoint(rect.right(), rect.top()));
    oPoints.append(QPoint(rect.right(), rect.top() + c_nCommentTriangleRightLength));

    painter->save();

    QColor color = Qt::red;

    QPen pen(color);
    painter->setPen(pen);

    QBrush brush(color);
    painter->setBrush(brush);

    painter->drawConvexPolygon(oPoints);

    painter->restore();
}

void GLDTableViewPrivate::drawTriangles(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index)
{
    GString str =  m_model->data(index, gidrCommentRole).toString();

    if (str.length() > 0)
    {
        drawTriangles(opt.rect, painter);
    }
}

void GLDTableViewPrivate::itemDelegatePaint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_Q(GLDTableView);

    q->style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &option, painter, q);
    q->itemDelegate(index)->paint(painter, option, index);
}

void GLDTableViewPrivate::setOptionSelectedState(QStyleOptionViewItem& opt, const QModelIndex &index)
{
    Q_Q(GLDTableView);

    if (m_selectionModel)
    {
        if(m_selectionModel->isSelected(index) || m_selectionModel->isRowSelected(index.row(), index.parent()))
        {
            opt.state |= QStyle::State_Selected;
        }
    }

    if (m_alwaysShowRowSelectedColor)
    {
        if (index.row() == q->currentIndex().row())
        {
            opt.state |= QStyle::State_Selected;
        }
    }
}

void GLDTableViewPrivate::setOptionEnabledState(QStyleOptionViewItem& opt, const QModelIndex &index)
{
    if (opt.state & QStyle::State_Enabled)
    {
        QPalette::ColorGroup cg;

        if ((m_model->flags(index) & Qt::ItemIsEnabled) == 0)
        {
            opt.state &= ~QStyle::State_Enabled;
            cg = QPalette::Disabled;
        }
        else
        {
            cg = QPalette::Normal;
        }

        opt.palette.setCurrentColorGroup(cg);
    }
}

void GLDTableViewPrivate::setOptionMouseOverState(const QModelIndex &index, QStyleOptionViewItem& opt)
{
    if (index == m_hover)
    {
        opt.state |= QStyle::State_MouseOver;
    }
}

void GLDTableViewPrivate::setOptionHasFocusState(const QModelIndex &index, QStyleOptionViewItem& opt)
{
    Q_Q(GLDTableView);

    if (index == q->currentIndex())
    {
        const bool c_focus = (q->hasFocus() || viewport->hasFocus()) && q->currentIndex().isValid();

        if (c_focus)
        {
            opt.state |= QStyle::State_HasFocus;
        }

        if (index.row() == m_oEditorIndex.row())
        {
            opt.state |= QStyle::State_HasFocus;
            opt.state |= QStyle::State_Active;
            opt.showDecorationSelected = true;
        }
    }
}

QItemSelection GLDTableViewPrivate::getItemSelectionAccordingToSelectState()
{
    Q_Q(GLDTableView);

    QItemSelection selection;

    // 如果选择区域有多个，那么就只绘制currentIndex的选择边框线
    if (isInMutiSelect())
    {
        selection = QItemSelection(q->currentIndex(), q->currentIndex());
    }
    else
    {
        selection = m_selectionModel.data()->selection();
    }

    return selection;
}

void GLDTableViewPrivate::adjustSelectionBoundingRectAccordingToFixedColumn(const QRegion &bordingRegion, QRect &srcRect)
{
    Q_Q(GLDTableView);

    // 如果选择的区域有一部分在固定列中，当滚动条滚动，使不在固定列中的选中区域，被固定列覆盖时，就应该只绘制在固定列部分的选中区域边框线
    if (m_oTopLeftSelectIndex.column() < m_nFixedColCount && m_oBottomRightSelectIndex.column() >= m_nFixedColCount)
    {
        // 非固定列区域被覆盖一部分
        if (srcRect.left() < m_nFixedColWidth && srcRect.right() > m_nFixedColWidth)
        {
            srcRect.setLeft(m_horizontalHeader->sectionVisualPosition(m_oTopLeftSelectIndex.column()));
        }
        // 非固定列区域完全被固定列区域覆盖时
        else if (srcRect.right() <= m_nFixedColWidth)
        {
            srcRect.setRight(m_nFixedColWidth - q->currentGridLineWidth());
            srcRect.setLeft(m_horizontalHeader->sectionVisualPosition(m_oTopLeftSelectIndex.column()));

            // bordingRegion为空，是因为上层的visualRegionForSelection方法计算出的Rect的right比left小，有固定列，把滚动条拉到最后时会出现
            // 导致转换出来的QRegion无效，此时，应该重新计算top和bottom，保证绘制正确
            if (bordingRegion.isEmpty())
            {
                int nTop = q->rowViewportPosition(m_oTopLeftSelectIndex.row());
                int nBottom = q->rowViewportPosition(m_oBottomRightSelectIndex.row()) + q->rowHeight(m_oBottomRightSelectIndex.row());
                srcRect.setTop(nTop);
                srcRect.setBottom(nBottom);
            }
        }
    }
    else if (m_oTopLeftSelectIndex.column() >= m_nFixedColCount)
    {
        if (srcRect.right() <= m_nFixedColWidth)
        {
            srcRect = QRect();
        }
        else if (srcRect.right() > m_nFixedColWidth && srcRect.left() < m_nFixedColWidth)
        {
            srcRect.setLeft(m_nFixedColWidth);
        }
    }
}

void GLDTableViewPrivate::adjustSelectionBoundingRectAccordingToFixedRow(const QRegion &bordingRegion, QRect &srcRect)
{
    Q_Q(GLDTableView);
    if (m_oTopLeftSelectIndex.row() < m_nFixedRowCount && m_oBottomRightSelectIndex.row() >= m_nFixedRowCount)
    {
        if (srcRect.top() < m_nFixedRowHeight && srcRect.bottom() > m_nFixedRowHeight)
        {
            srcRect.setTop(m_verticalHeader->sectionVisualPosition(m_oTopLeftSelectIndex.row()));
        }
        else if (srcRect.bottom() <= m_nFixedRowHeight)
        {
            srcRect.setBottom(m_nFixedRowHeight - q->currentGridLineWidth());
            srcRect.setTop(m_verticalHeader->sectionVisualPosition(m_oTopLeftSelectIndex.row()));

            if (bordingRegion.isEmpty())
            {
                int nLeft = q->columnViewportPosition(m_oTopLeftSelectIndex.column());
                int nRight = q->columnViewportPosition(m_oBottomRightSelectIndex.column()) + q->columnWidth(m_oBottomRightSelectIndex.column());

                srcRect.setLeft(nLeft);
                srcRect.setRight(nRight);
            }
        }
    }
    else if (m_oTopLeftSelectIndex.row() >= m_nFixedRowCount)
    {
        if (srcRect.bottom() <= m_nFixedRowHeight)
        {
            srcRect = QRect();
        }
        else if (srcRect.bottom() > m_nFixedRowHeight && srcRect.top() < m_nFixedRowHeight)
        {
            srcRect.setTop(m_nFixedRowHeight);
        }
    }
}

void GLDTableViewPrivate::drawNormalSelectionBoundingLine(QPainter &painter, QRect rect)
{
    painter.save();

    QPen oLinePen = painter.pen();
    oLinePen.setColor(getPenColor());
    oLinePen.setWidth(m_selectBoundLineWidth);
    painter.setPen(oLinePen);

    // 当m_selectBoundLineWidth为3像素时，绘制要求为，格线外占一像素，格线内占一像素
    if (m_selectBoundLineWidth == (int)ThreePixel)
    {
        rect.setRight(rect.right() - 1);
        rect.setBottom(rect.bottom() - 1);
    }

    painter.drawLine(rect.topLeft(), rect.topRight());
    painter.drawLine(rect.bottomLeft(), rect.bottomRight());

    painter.drawLine(rect.topLeft(), rect.bottomLeft());
    painter.drawLine(rect.topRight(), rect.bottomRight());

    painter.restore();
}

void GLDTableViewPrivate::drawBorderWithFillHandle(QPainter &painter, QRect rect)
{
    painter.save();

    QPen linePen = painter.pen();
    QColor oPenColor = getPenColor();
    linePen.setColor(oPenColor);
    linePen.setWidth(3);
    painter.setPen(linePen);

    // 绘制要求，格线外占一像素，格线内占一像素
    rect.setRight(rect.right() - 1);
    rect.setBottom(rect.bottom() - 1);

    painter.drawLine(rect.topLeft(), rect.topRight());
    painter.drawLine(rect.topLeft(), rect.bottomLeft());
    painter.drawLine(rect.bottomRight(), rect.topRight());
    painter.drawLine(rect.bottomRight(), rect.bottomLeft());

    QRect oEmptyRect;
    switch (m_rangeFillHandlePositon)
    {
    case RightBottom:
    {
        oEmptyRect = QRect(rect.right() - 2, rect.bottom() - 2, c_nEmptyRectWidth, c_nEmptyRectWidth);
        m_oRangeFillHandleRect = QRect(rect.right() - 1, rect.bottom() - 1,
                                       c_nFillHandleWidth, c_nFillHandleWidth);
        break;
    }
    case LeftBottom:
    {
        oEmptyRect = QRect(rect.left() + 1, rect.bottom() - 2, c_nEmptyRectWidth, c_nEmptyRectWidth);
        m_oRangeFillHandleRect = QRect(rect.left() + 1, rect.bottom() - 1,
                                       c_nFillHandleWidth, c_nFillHandleWidth);
        break;
    }
    case RightTop:
    {
        oEmptyRect = QRect(rect.right() - 2, rect.top() + 1, c_nEmptyRectWidth, c_nEmptyRectWidth);
        m_oRangeFillHandleRect = QRect(rect.right() - 1, rect.top() + 1,
                                       c_nFillHandleWidth, c_nFillHandleWidth);
        break;
    }
    default:
        break;
    }

    // 先绘制一个白色的矩形，把右下角多余的格线给去掉
    painter.fillRect(oEmptyRect, Qt::white);
    // 再填充一个有颜色的矩形
    painter.fillRect(m_oRangeFillHandleRect, oPenColor);

    painter.restore();
}

void GLDTableViewPrivate::adjustStyleOption(QStyleOptionViewItem& opt, const QModelIndex &index)
{
    setOptionSelectedState(opt, index);
    setOptionMouseOverState(index, opt);
    setOptionEnabledState(opt, index);
    setOptionHasFocusState(index, opt);
}

bool GLDTableViewPrivate::nothingToDraw()
{
    return m_horizontalHeader->count() == 0 || m_verticalHeader->count() == 0 || !m_itemDelegate;
}

bool GLDTableViewPrivate::currentCellDrawn(
        GlodonTableViewPainterInfo &info, int nVisualCol, QBitArray &drawn, int nColumnCount, int nVisualRow)
{
    int nCurrentBit = (nVisualRow - info.m_nFirstVisualRow) * nColumnCount + nVisualCol - info.m_nFirstVisualColumn;

    if (nCurrentBit < 0 || nCurrentBit >= drawn.size() || drawn.testBit(nCurrentBit))
    {
        return true;
    }

    drawn.setBit(nCurrentBit);
    return false;
}

bool GLDTableViewPrivate::isCoverByFixedRow(int row, int rowYEndPos, bool addGridLineWidth)
{
    Q_Q(GLDTableView);
    // 如果是绘制格线时，判断是否被固定列覆盖，则应该加载格线的宽度
    int nRowYEndPos = addGridLineWidth ? rowYEndPos + q->currentGridLineWidth() : rowYEndPos;
    if ((row > m_nFixedRowCount - 1) && (nRowYEndPos <= m_nFixedRowHeight))
    {
        return true;
    }

    return false;
}

bool GLDTableViewPrivate::isCoverByFixedColumn(int column, int columnXEndPos, bool addGridLineWidth)
{
    Q_Q(GLDTableView);
    // 如果是绘制格线时，判断是否被固定列覆盖，则应该加载格线的宽度
    int nColumnXEndPos = addGridLineWidth ? columnXEndPos + q->currentGridLineWidth() : columnXEndPos;
    if ((column > m_nFixedColCount - 1) && (nColumnXEndPos <= m_nFixedColWidth))
    {
        return true;
    }

    return false;
}

void GLDTableViewPrivate::setOptionFeaturesAlternateProperty(bool bAlternateBase, bool balternate, QStyleOptionViewItem &option)
{
    if (!balternate)
        return;

    if (bAlternateBase)
    {
        option.features |= QStyleOptionViewItem::Alternate;
    }
    else
    {
        option.features &= ~QStyleOptionViewItem::Alternate;
    }
}

QRect GLDTableViewPrivate::cellPainterRect(QRect srcRect, int visualRow, int visualCol)
{
    QRect oCellPainterRect(srcRect);

    if ((visualCol >= m_nFixedColCount) && (m_nFixedColWidth > 0))
    {
        if ((srcRect.left() < m_nFixedColWidth) && (srcRect.right() >= m_nFixedColWidth))
        {
            int nCoverByFixedColWidth = m_nFixedColWidth - srcRect.left();
            oCellPainterRect.adjust(nCoverByFixedColWidth, 0, 0, 0);
        }
    }

    if ((visualRow >= m_nFixedRowCount) && (m_nFixedRowHeight > 0))
    {
        if ((srcRect.top() < m_nFixedRowHeight) && (srcRect.bottom() >= m_nFixedRowHeight))
        {
            int nCoverByFixedRowHeight = m_nFixedRowHeight - srcRect.top();
            oCellPainterRect.adjust(0, nCoverByFixedRowHeight, 0, 0);
        }
    }
    return oCellPainterRect;
}

void GLDTableViewPrivate::calculateFixedRegion()
{
    calculateFixedColumnWidth();
    calculateFixedRowHeight();
}

void GLDTableViewPrivate::calculateFixedColumnWidth() const
{
    Q_Q(const GLDTableView);

    m_nFixedColWidth = 0;
    for (int nCol = 0; nCol < m_nFixedColCount; ++nCol)
    {
        if (!m_horizontalHeader->isSectionHidden(nCol))
        {
            m_nFixedColWidth += q->columnWidth(nCol) + q->currentGridLineWidth();
        }
    }
}

void GLDTableViewPrivate::calculateFixedRowHeight() const
{
    Q_Q(const GLDTableView);

    m_nFixedRowHeight = 0;
    for (int nRow = 0; nRow < m_nFixedRowCount; ++nRow)
    {
        if (!m_verticalHeader->isSectionHidden(nRow))
        {
            m_nFixedRowHeight += q->rowHeight(nRow) + q->currentGridLineWidth();
        }
    }
}

void GLDTableViewPrivate::drawCell(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
{    
    QStyleOptionViewItem opt = option;
    opt.index = index;

    adjustStyleOption(opt, index);

    itemDelegatePaint(painter, opt, index);
    drawTriangles(painter, opt, index);
}


GLDTableView::GLDTableView(QWidget *parent)
    : GlodonAbstractItemView(*new GLDTableViewPrivate, parent), m_bShowCurCellBackgroundColor(false)
{
    Q_D(GLDTableView);

    d->init();
}

GLDTableView::GLDTableView(GLDTableViewPrivate &dd, QWidget *parent)
    : GlodonAbstractItemView(dd, parent), m_bShowCurCellBackgroundColor(false)
{
    Q_D(GLDTableView);

    d->init();

    connect(this, SIGNAL(canShowComment(const QModelIndex &, bool &)),
            this, SLOT(onDrawComment(const QModelIndex &, bool &)));
}

void GLDTableView::doSetModel(QAbstractItemModel *)
{
    Q_D(GLDTableView);
    d->fillSuitColWidthCols();
    d->fillSuitRowHeightCols();
}

bool GLDTableView::canRangeFill(const QRect &)
{
    return true;
}

bool GLDTableView::doCanRangeFill(const QRect & rect)
{
    return allowRangeFilling() && !(d_func()->isInMutiSelect())
            && rect.isValid() && canRangeFill(rect);
}

GLDTableView::~GLDTableView()
{
}

void GLDTableView::setModel(QAbstractItemModel *model)
{
    Q_D(GLDTableView);

    //每次setModel时，把正处于编辑状态的格子强制关闭
    if (d->m_oEditorIndex.isValid())
    {
        forceCloseEditor();
    }

    doSetModel(model);
}

void GLDTableView::doItemsLayout()
{
    Q_D(GLDTableView);

    GlodonAbstractItemView::doItemsLayout();

    if (verticalScrollMode() == GlodonAbstractItemView::ScrollPerItem)
    {
        d->m_verticalHeader->setOffsetToSectionPosition(verticalScrollBar()->value());
    }
    else
    {
        d->m_verticalHeader->setOffset(verticalScrollBar()->value());
    }

    if (!d->m_verticalHeader->updatesEnabled())
    {
        d->m_verticalHeader->setUpdatesEnabled(true);
    }
}

void GLDTableView::paint(QPainter &painter, const QRegion &drawRegion)
{
    Q_D(GLDTableView);

    QStyleOptionViewItem oOption = d->viewOptionsV4();

    const GlodonHeaderView *pVerticalHeader = d->m_verticalHeader;
    const GlodonHeaderView *pHorizontalHeader = d->m_horizontalHeader;

    if (d->nothingToDraw())
        return;

    // 获取可见行，列
    int nFirstVisualRow = pVerticalHeader->visualIndexAt(0);
    int nLastVisualRow = lastVisualRow();

    int nFirstVisualColumn = pHorizontalHeader->visualIndexAt(0);
    int nLastVisualColumn = pHorizontalHeader->visualIndexAt(pHorizontalHeader->viewport()->width());
    adjustColumnByLayoutDirection(nFirstVisualColumn, nLastVisualColumn);

    // 计算固定行，固定列区域
    d->calculateFixedRegion();

    QBitArray drawn((nLastVisualRow - nFirstVisualRow + 1) * (nLastVisualColumn - nFirstVisualColumn + 1));
    QRegion toBeDrawn(d->viewport->rect());

    QPen gridPen = currentGridPen();

    // 绘制合并格（合并格只初始化了可见区域）
    const QRegion c_scrollDelayReigon = drawRegion.translated(d->m_scrollDelayOffset);
    if (d->hasSpans())
    {
        painter.save();

        painter.setPen(gridPen);
        d->drawAndClipSpans(c_scrollDelayReigon, &painter, oOption, &drawn,
                            nFirstVisualRow, nLastVisualRow,
                            nFirstVisualColumn, nLastVisualColumn);
        toBeDrawn = painter.clipRegion();

        painter.restore();
    }

    // 开始绘制除合并格之外的格子
    int nGridLineWidth = currentGridLineWidth();
    uint horizontalRight = pHorizontalHeader->length() - pHorizontalHeader->offset() - (isRightToLeft() ? 0 : nGridLineWidth);
    uint verticalBottom = pVerticalHeader->length() - pVerticalHeader->offset() - nGridLineWidth;

    const QVector<QRect> c_rects = c_scrollDelayReigon.rects();
    for (int i = 0; i < c_rects.size(); ++i)
    {
        m_dirtyArea = c_rects.at(i);
        adjustCurrentDirtyArea(verticalBottom, horizontalRight);

        // 获取当前Dirty区域的边界行列
        int nLeft = pHorizontalHeader->visualIndexAt(m_dirtyArea.left());
        int nRight = pHorizontalHeader->visualIndexAt(m_dirtyArea.right());
        adjustColumnByLayoutDirection(nLeft, nRight);

        int nBottom = lastVisualRowInDirtyArea(pVerticalHeader);

        int nTop = 0;
        bool alternateBase = false;
        calcTopRowAndAlternateBase(nBottom, nTop, alternateBase);

        if (nTop == -1 || nTop > nBottom)
        {
            continue;
        }

        GlodonTableViewPainterInfo oPainterParams(
                    nTop, nBottom, nLeft, nRight, nFirstVisualRow, nFirstVisualColumn, nLastVisualColumn, alternateBase);
        drawGridCellFromRowToColumn(painter, oPainterParams, drawn, oOption);

        if (d->m_showGrid)
        {
            QRect oCurrentDirtyAreaRect(QPoint(nLeft, nTop), QPoint(nRight, nBottom));
            drawCellBorderLines(painter, oCurrentDirtyAreaRect, toBeDrawn, oOption);
        }

        painter.save();
        drawSelectionBorderLines(painter, d->m_scrollDelayOffset);
        painter.restore();
    }

#ifndef QT_NO_DRAGANDDROP
    // Paint the dropIndicator
    d->paintDropIndicator(&painter);
#endif
}

int GLDTableView::currentGridLineWidth() const
{
    Q_D(const GLDTableView);

    if (d->m_gridStyle == Qt::NoPen)
        return 0;

    if (d->m_bIsCustomStyle)
    {
        return d->m_showGrid ? d->m_gridLineWidth : 0;
    }
    else
    {
        return d->m_showGrid ? 1 : 0;
    }
}

QPen GLDTableView::currentGridPen()
{
    Q_D(GLDTableView);

    if (d->m_bIsCustomStyle)
    {
        const int nGridLineWidth = currentGridLineWidth();
        return QPen(d->m_gridLineColor, nGridLineWidth, d->m_gridStyle);
    }

    const QStyleOptionViewItem option = d->viewOptionsV4();
    const int gridHint = style()->styleHint(QStyle::SH_Table_GridLineColor, &option, this);
    const QColor c_gridColor = static_cast<QRgb>(gridHint);
    return QPen(c_gridColor, 0, d->m_gridStyle);
}

bool GLDTableView::edit(const QModelIndex &index, GlodonAbstractItemView::EditTrigger trigger, QEvent *event)
{
    Q_D(GLDTableView);

    bool bEditResult = GlodonAbstractItemView::edit(index, trigger, event);

    if (bEditResult)
    {
        if (d->m_bEnterJump)
        {
            d->m_prevState = GlodonAbstractItemView::Editting;
        }
    }

    return bEditResult;
}

void GLDTableView::syncStateToDelegate(GlodonDefaultItemDelegate *pNewDelegate)
{
    Q_D(GLDTableView);

    GlodonAbstractItemView::syncStateToDelegate(pNewDelegate);

    pNewDelegate->setIsEnterJump(d->m_bEnterJump);
    pNewDelegate->setIsEnterJumpPro(d->m_bEnterJumpPro);
    pNewDelegate->setCloseEditorOnFocusOut(d->m_bCloseEditorOnFocusOut, d->m_ignoreActiveWindowFocusReason);
    pNewDelegate->setUseBlendColor(d->m_bUseBlendColor);
    pNewDelegate->setUseComboBoxCompleter(d->m_bUseComboBoxCompleter);
    pNewDelegate->setSelectedCellBackgroundColor(d->m_oSelectedCellBackgroundColor);
}

int GLDTableView::lastVisualRowInDirtyArea(const GlodonHeaderView *verticalHeader) const
{
    int nBottom = verticalHeader->visualIndexAt(m_dirtyArea.bottom());

    if (nBottom == -1)
    {
        nBottom = verticalHeader->count() - 1;
    }

    return nBottom;
}

void GLDTableView::calcTopRowAndAlternateBase(int bottom, int &top, bool &alternateBase)
{
    Q_D(GLDTableView);

    if (d->m_bAlternatingColors && d->m_verticalHeader->sectionsHidden())
    {
        uint verticalOffset = d->m_verticalHeader->offset();
        int row = d->m_verticalHeader->logicalIndex(top);

        for (int j = 0; ((uint)(j += d->m_verticalHeader->sectionSize(top) + d->m_gridLineWidth) <= verticalOffset) && (top < bottom); ++top)
        {
            row = d->m_verticalHeader->logicalIndex(top);

            if (d->m_bAlternatingColors && !d->m_verticalHeader->isSectionHidden(row))
            {
                alternateBase = !alternateBase;
            }
        }

        if (fixedRowCount() > 0)
        {
            top = 0;
        }

    }
    else
    {
        top = d->m_verticalHeader->visualIndexAt(m_dirtyArea.top());
        alternateBase = (1 == (top & 1)) && d->m_bAlternatingColors;
    }
}

void GLDTableView::adjustCurrentDirtyArea(uint verticalBottom, uint horizontalRight)
{
    if (int(verticalBottom) < m_dirtyArea.bottom())
    {
        m_dirtyArea.setBottom(int(verticalBottom));
    }

    if (int(horizontalRight) < m_dirtyArea.right())
    {
        m_dirtyArea.setRight(int(horizontalRight));
    }

    //修改GTJ-4250Bug
    if (int(horizontalRight) < m_dirtyArea.left())
    {
        m_dirtyArea.setLeft(int(horizontalRight));
    }
}

void GLDTableView::adjustColumnByLayoutDirection(int &firstVisualColumn, int &lastVisualColumn)
{
    if (isRightToLeft())
    {
        qSwap(firstVisualColumn, lastVisualColumn);
    }

    if (firstVisualColumn == -1)
    {
        firstVisualColumn = 0;
    }

    if (lastVisualColumn == -1)
    {
        lastVisualColumn = horizontalHeader()->count() - 1;
    }
}

void GLDTableView::drawGridCellFromRowToColumn(
        QPainter &painter, GlodonTableViewPainterInfo &info, QBitArray &drawn,
        QStyleOptionViewItem &option)
{
    Q_D(GLDTableView);

    const QPoint offset = d->m_scrollDelayOffset;
    const int c_nGridSize = currentGridLineWidth();
    bool bAlternateBase = info.m_bAlternateBase;

    d->m_borderLines.clear();

    bool balternate = d->m_bAlternatingColors;

    int nColumnCount = info.m_nLastVisualColumn - info.m_nFirstVisualColumn + 1;

    // 绘制行
    for (int nVisualRow = info.m_nDirtyAreaTop; nVisualRow <= info.m_nDirtyAreaBottom; ++nVisualRow)
    {
        int nLogicalRow = d->m_verticalHeader->logicalIndex(nVisualRow);

        if (d->m_verticalHeader->isSectionHidden(nLogicalRow))
        {
            continue;
        }

        int nRowY = rowViewportPosition(nLogicalRow) + offset.y();
        int nRowh = rowHeight(nLogicalRow);

        if (d->isCoverByFixedRow(nVisualRow, nRowY + nRowh, false))
        {
            continue;
        }

        // 绘制列
        for (int nVisualCol = info.m_nDirtyAreaLeft; nVisualCol <= info.m_nDirtyAreaRight; ++nVisualCol)
        {
            if (d->currentCellDrawn(info, nVisualCol, drawn, nColumnCount, nVisualRow))
            {
                continue;
            }

            int nLogicalCol = d->m_horizontalHeader->logicalIndex(nVisualCol);

            if (d->m_horizontalHeader->isSectionHidden(nLogicalCol))
            {
                continue;
            }

            int nColp = columnViewportPosition(nLogicalCol) + offset.x();
            int nColw = columnWidth(nLogicalCol);

            if (d->isCoverByFixedColumn(nVisualCol, nColp + nColw, false))
            {
                continue;
            }

            QModelIndex index = d->m_model->index(nLogicalRow, nLogicalCol, d->m_root);

            if (!index.isValid())
            {
                continue;
            }

            QVariant oBoundLine = index.data(gidrBoundLineRole);
            d->m_borderLines.insert(index, oBoundLine);

            option.rect = QRect(nColp + (d->m_showGrid && isRightToLeft() ? c_nGridSize : 0), nRowY,
                                nColw - borderLineSubtractGridLineWidth(oBoundLine, true),
                                nRowh - borderLineSubtractGridLineWidth(oBoundLine, false));

            // 根据固定行，固定列，获取绘制的区域，主要是被固定行或固定列覆盖了一部分的区域
            QRect oCellPainterRect = d->cellPainterRect(option.rect, nVisualRow, nVisualCol);

            d->setOptionFeaturesAlternateProperty(bAlternateBase, balternate, option);

            painter.save();
            painter.setClipRect(oCellPainterRect);

            // TODO liurx 按像素滚动时，树形结构区域绘制还有问题
            d->drawCell(&painter, option, index);
            painter.restore();
        }

        bAlternateBase = !bAlternateBase && balternate;
    }
}

void GLDTableView::drawCellBorderLines(QPainter &painter, const QRect &params, QRegion &spans,
        QStyleOptionViewItem &option)
{
    Q_D(GLDTableView);

    painter.save();
    painter.setPen(currentGridPen());

    if (d->m_bDrawBoundLine)
    {
        drawBorderLinesByRow(painter, params, spans, option);
    }
    else
    {
        drawGridLines(painter, params, spans);
    }

    if (d->m_bDrawTopAndLeftBorderLine)
    {
        drawTopAndLeftBorderLine(painter);
    }
    painter.restore();
}

void GLDTableView::drawBorderLinesByRow(
        QPainter &painter, const QRect &params, QRegion &spans,
        QStyleOptionViewItem &option)
{
    Q_D(GLDTableView);

    int nTop = params.top();
    int nBottom = params.bottom();

    for (int nVisualRowIndex = nTop; nVisualRowIndex <= nBottom; ++nVisualRowIndex)
    {
        int row = d->m_verticalHeader->logicalIndex(nVisualRowIndex);

        if (d->m_verticalHeader->isSectionHidden(row))
        {
            continue;
        }

        drawBorderLinesByCol(painter, params, spans, option, row);
    }
}

void GLDTableView::drawBorderLinesByCol(
        QPainter &painter, const QRect &params, QRegion &spans,
        QStyleOptionViewItem &option, int row)
{
    Q_D(GLDTableView);

    int nLeft = params.left();
    int nRight = params.right();

    for (int nVisualColumnIndex = nLeft; nVisualColumnIndex <= nRight; ++nVisualColumnIndex)
    {
        int nCol = d->m_horizontalHeader->logicalIndex(nVisualColumnIndex);

        if (d->m_horizontalHeader->isSectionHidden(nCol)
                || 0 == d->m_horizontalHeader->sectionSize(nCol))
        {
            continue;
        }

        QModelIndex index = d->m_model->index(row, nCol, d->m_root);
        option.index = index;

        QRect oCellRect = visualRectIncludeBoundingLineWidth(index);

        if (d->isCoverByFixedRow(row, oCellRect.bottom(), true))
        {
            continue;
        }

        if (d->isCoverByFixedColumn(nVisualColumnIndex, oCellRect.right(), true))
        {
            continue;
        }

        drawCellBorderLine(painter, oCellRect, spans, index);
    }
}

void GLDTableView::drawSelectionBorderLines(QPainter &painter, const QPoint &offset)
{
    Q_D(GLDTableView);

    drawSelectionBorderLines(painter);

    switch (d->m_state)
    {
        case RangeFillingState:
        {
            drawRangeFillingState(painter, offset);
            break;
        }
        case RangeMovingState:
        {
            drawRangeMovingState(painter);
            break;
        }
    }
}

void GLDTableView::drawCellBorderLine(
        QPainter &painter, const QRect &cellRect, const QRegion &spans,
        const QModelIndex &index)
{
    Q_D(GLDTableView);

    // 得到设置的格线宽度
    QVariant oBoundLine = d->m_borderLines.value(index);

    if (oBoundLine.isNull() || !oBoundLine.isValid())
    {
        oBoundLine = index.data(gidrBoundLineRole);
    }

    int nGridLineWidth = currentGridLineWidth();
    drawCellBorderLine(
                painter, cellRect, spans,
                nGridLineWidth, borderLineWidth(oBoundLine, false), index.data(gidrBottomBoundLineColor),
                Qt::Horizontal);

    drawCellBorderLine(
                painter, cellRect, spans,
                nGridLineWidth, borderLineWidth(oBoundLine, true), index.data(gidrRightBoundLineColor),
                Qt::Vertical);
}

void GLDTableView::drawCellBorderLine(
        QPainter &painter, const QRect &cellRect, const QRegion &spans, int gridLineWidth,
        int borderLineWidth, const QVariant &borderLineColor, Qt::Orientation direction)
{
    Q_D(GLDTableView);

    QPen borderLinePen = painter.pen();

    borderLinePen.setWidth(qMax(borderLineWidth, gridLineWidth));
    borderLinePen.setColor(borderLineColor.isValid() ? borderLineColor.value<QColor>() : d->m_gridLineColor);

    if (borderLinePen.width() == 0)
    {
        return;
    }

    painter.save();
    painter.setPen(borderLinePen);

    int rowY = cellRect.y();
    int rowH = cellRect.height();
    int colX = cellRect.x();
    int colW = cellRect.width();

    // 根据QPainter的pen宽度，调整绘制位置，格线大于边框线时，需要向外扩
    // 格线小于边框线时，需要向内扩
    if (gridLineWidth >= borderLineWidth)
    {
        // 由于QPainter在绘制的时候，根据像素不一样，距离中心线有不同的偏移
        // 经验数据，需要做除2处理，以保证格线绘制在一条线上，且占用的是格子内部
        // 其他类似操作同理
        rowY += gridLineWidth / 2;
        colX += gridLineWidth / 2;
    }
    else
    {
        int nDrawOffset = (borderLineWidth - gridLineWidth) / 2;

        rowY += nDrawOffset;
        colX += nDrawOffset;

        rowH -= 2 * nDrawOffset;
        colW -= 2 * nDrawOffset;
    }

    QRect viewportRect = d->viewport->rect();

    painter.setClipRect(viewportRect);
    painter.setClipRegion(spans, Qt::IntersectClip);

    // 画线
    if (direction == Qt::Horizontal)
    {
        QLine hLine(colX, rowY + rowH, colX + colW, rowY + rowH);
        drawCellBorderLine(painter, hLine, d->m_bShowHorizontalGridLine);
    }
    else
    {
        QLine vLine(colX + colW, rowY, colX + colW, rowY + rowH);
        drawCellBorderLine(painter, vLine, d->m_bShowVerticalGridLine);
    }

    painter.restore();
}

void GLDTableView::drawCellBorderLine(
        QPainter &painter, const QLine &line, bool needDraw)
{
    if (needDraw)
    {
        painter.drawLine(line);
    }
}

void GLDTableView::drawEachRow(QPainter &painter, QRegion &spans, const QRect &params)
{
    Q_D(GLDTableView);

    const GlodonHeaderView *verticalHeader = d->m_verticalHeader;
    // Paint each row
    painter.setClipRect(d->viewport->rect());
    painter.setClipRegion(spans, Qt::IntersectClip);

    int nTop = params.top();
    int nBottom = params.bottom();

    const QPoint offset = d->m_scrollDelayOffset;

    for (int nVisualIndex = nTop; nVisualIndex <= nBottom; ++nVisualIndex)
    {
        int nRow = verticalHeader->logicalIndex(nVisualIndex);

        if (verticalHeader->isSectionHidden(nRow))
        {
            continue;
        }

        int nRowY = rowViewportPosition(nRow) + offset.y();
        nRowY += rowHeight(nRow);

        int nDrawY = nRowY + currentGridLineWidth() / 2;

        if (d->isCoverByFixedRow(nRow, nRowY, true))
        {
            continue;
        }

        painter.drawLine(m_dirtyArea.left(), nDrawY, m_dirtyArea.right(), nDrawY);
    }
}

void GLDTableView::drawEachColumn(QPainter &painter, QRegion &spans, const QRect &params)
{
    Q_D(GLDTableView);

    const GlodonHeaderView *horizontalHeader = d->m_horizontalHeader;
    // Paint each column
    painter.setClipRect(d->viewport->rect());
    painter.setClipRegion(spans, Qt::IntersectClip);

    int nLeft = params.left();
    int nRight = params.right();

    const QPoint offset = d->m_scrollDelayOffset;

    for (int i = nLeft; i <= nRight; ++i)
    {
        int col = horizontalHeader->logicalIndex(i);

        if (horizontalHeader->isSectionHidden(col))
        {
            continue;
        }

        int nColX = columnViewportPosition(col) + offset.x();
        nColX += columnWidth(col);

        int nDrawX = nColX + currentGridLineWidth() / 2;

        if (d->isCoverByFixedColumn(col, nColX, true))
        {
            continue;
        }

        painter.drawLine(nDrawX, m_dirtyArea.top(), nDrawX, m_dirtyArea.bottom());
    }
}

void GLDTableView::drawTopAndLeftBorderLine(QPainter &painter)
{
    Q_D(GLDTableView);

    painter.save();

    QPen pen = painter.pen();
    pen.setColor(d->m_oFrameLineColor);
    painter.setPen(pen);

    // TODO liurx 绘制边框线时，是否需要把画笔的宽度除2，需要结合headerView统一考虑，是把线画全盖住内容？
    // 还是把线画不全，还是把整个表格向右偏移，不占用格子的内容
    if (horizontalHeader()->isHidden() && verticalScrollMode() == ScrollPerItem)
    {
        const QLine topFrameLine(m_dirtyArea.left(), painter.pen().width() / 2,
                                 m_dirtyArea.right(), painter.pen().width() / 2);
        painter.drawLine(topFrameLine);
    }

    // TODO liurx，在满足绘制整个表格边框线情况下，数据只有一行，并且把这一行隐藏，会出现黑点
    if (verticalHeader()->isHidden() && horizontalScrollMode() == ScrollPerItem)
    {
        const QLine leftFrameLine(painter.pen().width() / 2, m_dirtyArea.top(),
                                  painter.pen().width() / 2, m_dirtyArea.bottom());
        painter.drawLine(leftFrameLine);
    }

    painter.restore();
}

void GLDTableView::adjustCommonBorder(const QLine &line, QList<QLine> &rectVisablelBorders)
{
    if (!rectVisablelBorders.contains(line))
    {
        rectVisablelBorders.append(line);
    }
    else
    {
        rectVisablelBorders.removeOne(line);
    }
}

void GLDTableView::adjustCommonBorder(const QLine &lineTopOfCell, QList<QLine> &rectVisablelBorders,
                                      const QLine &adjustedLineTopOfCell)
{
    if (!rectVisablelBorders.contains(adjustedLineTopOfCell))
    {
        rectVisablelBorders.append(lineTopOfCell);
    }
    else
    {
        rectVisablelBorders.removeOne(lineTopOfCell);
        rectVisablelBorders.removeOne(adjustedLineTopOfCell);
    }
}

void GLDTableView::adjustCommonBorder(const QRect &rect, QList<QLine> &rectVisablelBorders)
{
    QLine lineLeftOfCell(rect.left(), rect.top(), rect.left(), rect.bottom());
    QPoint lineLeftStart(lineLeftOfCell.x1() + 1, lineLeftOfCell.y1());
    QPoint lineLeftEnd(lineLeftOfCell.x2() + 1, lineLeftOfCell.y2());
    QLine adjustedLineLeftOfCell(lineLeftStart, lineLeftEnd);
    adjustCommonBorder(lineLeftOfCell, rectVisablelBorders, adjustedLineLeftOfCell);

    QLine lineTopOfCell(rect.left(), rect.top(), rect.right(), rect.top());
    QPoint lineTopStart(lineTopOfCell.x1(), lineTopOfCell.y1() + 1);
    QPoint lineTopEnd(lineTopOfCell.x2(), lineTopOfCell.y2() + 1);
    QLine adjustedLineTopOfCell(lineTopStart, lineTopEnd);
    adjustCommonBorder(lineTopOfCell, rectVisablelBorders, adjustedLineTopOfCell);

    QLine lineRight(rect.right(), rect.top(), rect.right(), rect.bottom());
    adjustCommonBorder(lineRight, rectVisablelBorders);

    QLine lineBottom(rect.left(), rect.bottom(), rect.right(), rect.bottom());
    adjustCommonBorder(lineBottom, rectVisablelBorders);
}

QModelIndex GLDTableView::calculationRightBottomIndex(QModelIndex drawIndex)
{
    int rowCount = rowSpan(drawIndex.row(), drawIndex.column());
    int columnCount = columnSpan(drawIndex.row(), drawIndex.column());
    QModelIndex rightBottom;

    if (rowCount != 1 || columnCount != 1)
    {
        rightBottom = model()->index(drawIndex.row() + rowCount - 1,
                                     drawIndex.column() + columnCount - 1);
    }
    else
    {
        rightBottom = drawIndex;
    }

    return rightBottom;
}

void GLDTableView::drawSelectionBorderLines(QPainter &painter)
{
    Q_D(GLDTableView);

    QItemSelection selection = d->getItemSelectionAccordingToSelectState();

    if (selection.isEmpty())
    {
        return;
    }

    QRegion region = visualRegionForSelection(selection);

    QRect rect = region.boundingRect();

    d->adjustSelectionBoundingRectAccordingToFixedColumn(region, rect);
    if (rect.isEmpty())
        return;

    d->adjustSelectionBoundingRectAccordingToFixedRow(region, rect);
    if (rect.isEmpty())
        return;

    // 选择边框线需要向外扩一像素
    rect.adjust(-1, -1, 1, 1);

    if (doCanRangeFill(visualRectForSelection()))
    {
        d->drawBorderWithFillHandle(painter, rect);
    }
    else
    {
        d->drawNormalSelectionBoundingLine(painter, rect);
    }
}

void GLDTableView::drawGridLines(QPainter &painter, const QRect &params, QRegion &spans)
{
    Q_D(GLDTableView);

    int nGridLineWidth = currentGridLineWidth();
    if (nGridLineWidth <= 0)
        return;

    QPen oPen = painter.pen();
    oPen.setWidth(nGridLineWidth);

    painter.save();
    painter.setPen(oPen);

    if (d->m_bShowHorizontalGridLine)
    {
        drawEachRow(painter, spans, params);
    }
    if (d->m_bShowVerticalGridLine)
    {
        drawEachColumn(painter, spans, params);
    }

    painter.restore();
}

int GLDTableView::firstVisableColumn()
{
    Q_D(GLDTableView);

    int nFirstVisableColumn = 0;

    for (int i = 0; i < d->m_horizontalHeader->count(); i++)
    {
        if (!isColumnHidden(i) && columnWidth(i) != 0)
        {
            nFirstVisableColumn = i;
            break;
        }
    }

    return nFirstVisableColumn;
}

int GLDTableView::firstVisableRow()
{
    Q_D(GLDTableView);

    int nFirstVisableRow = 0;

    for (int i = 0; i < d->m_verticalHeader->count(); i++)
    {
        if (!isRowHidden(i) && rowHeight(i) != 0)
        {
            nFirstVisableRow = i;
            break;
        }
    }

    return nFirstVisableRow;
}

void GLDTableView::appendHideCellBorder(const QRect &rect, QList<QLine> &rectHideBorder)
{
    QLine lineLeft(rect.left(), rect.top(), rect.left(), rect.bottom());
    QLine lineTop(rect.left(), rect.top(), rect.right(), rect.top());
    QLine lineRight(rect.right(), rect.top(), rect.right(), rect.bottom());
    QLine lineBottom(rect.left(), rect.bottom(), rect.right(), rect.bottom());

    rectHideBorder.append(lineLeft);
    rectHideBorder.append(lineTop);
//    rectHideBorder.append(QLine(QPoint(lineLeft.x1() + 1, lineLeft.y1()), QPoint(lineLeft.x2() + 1, lineLeft.y2())));
//    rectHideBorder.append();
    rectHideBorder.append(lineRight);
    rectHideBorder.append(lineBottom);
}

void GLDTableView::drawRangeFillingState(QPainter &painter, const QPoint &offset)
{
    Q_D(GLDTableView);

    painter.save();
    painter.setClipRect(d->viewport->rect());

    QPen oLinePen = painter.pen();
    oLinePen.setColor(d->m_selectBoundLineColor);
    oLinePen.setWidth(3);
    oLinePen.setStyle(Qt::DotLine);
    painter.setPen(oLinePen);

    int nSrcLeft = columnVisualPosition(m_oRangeSrc.left()) + offset.x();
    int nSrcRight = columnVisualPosition(m_oRangeSrc.right()) + offset.x() + visualColumnWidth(m_oRangeSrc.right());
    int nSrcTop = rowVisualPosition(m_oRangeSrc.top()) + offset.y();
    int nSrcBottom = rowVisualPosition(m_oRangeSrc.bottom()) + offset.y() + visualRowHeight(m_oRangeSrc.bottom());

    // 向上拖拉
    if (m_nRangeCurRow < m_oRangeSrc.top())
    {
        int nCurRowLeft = nSrcLeft - oLinePen.width() / 2;
        int nCurRowTop = rowVisualPosition(m_nRangeCurRow) + offset.y();
        painter.drawLine(nCurRowLeft, nSrcTop, nCurRowLeft, nCurRowTop);
        painter.drawLine(nSrcRight, nSrcTop, nSrcRight, nCurRowTop);
        painter.drawLine(nCurRowLeft, nCurRowTop, nSrcRight, nCurRowTop);
    }
    // 向下拖拉
    else if (m_nRangeCurRow > m_oRangeSrc.bottom())
    {
        int nCurRowLeft = nSrcLeft - oLinePen.width() / 2;
        int nCurRowBottom = rowVisualPosition(m_nRangeCurRow) + offset.y() + visualRowHeight(m_nRangeCurRow);
        painter.drawLine(nCurRowLeft, nSrcBottom, nCurRowLeft, nCurRowBottom);
        painter.drawLine(nSrcRight, nSrcBottom, nSrcRight, nCurRowBottom);
        painter.drawLine(nCurRowLeft, nCurRowBottom, nSrcRight, nCurRowBottom);
    }
    // 向左拖拉
    else if (m_nRangeCurCol < m_oRangeSrc.left())
    {
        int nCurColTop = nSrcTop - oLinePen.width() / 2;
        int nCurColLeft = columnVisualPosition(m_nRangeCurCol) + offset.x();
        painter.drawLine(nSrcLeft, nCurColTop, nCurColLeft, nCurColTop);
        painter.drawLine(nSrcLeft, nSrcBottom, nCurColLeft, nSrcBottom);
        painter.drawLine(nCurColLeft, nSrcBottom, nCurColLeft, nCurColTop);
    }
    // 向右拖拉
    else if (m_nRangeCurCol > m_oRangeSrc.right())
    {
        int nCurColTop = nSrcTop - oLinePen.width() / 2;
        int nCurColRight = columnVisualPosition(m_nRangeCurCol) + offset.x() + visualColumnWidth(m_nRangeCurCol);
        painter.drawLine(nSrcRight, nCurColTop, nCurColRight, nCurColTop);
        painter.drawLine(nSrcRight, nSrcBottom, nCurColRight, nSrcBottom);
        painter.drawLine(nCurColRight, nSrcBottom, nCurColRight, nCurColTop);
    }

    painter.restore();
}

void GLDTableView::drawRangeMovingState(QPainter &painter)
{
    Q_D(GLDTableView);
    painter.save();
    painter.setClipRect(d->viewport->rect());

    QPen oLinePen = painter.pen();
    oLinePen.setColor(d->m_selectBoundLineColor);
    oLinePen.setWidth(3);
    painter.setPen(oLinePen);

    int nOffset = oLinePen.width() / 2;
    int nLeft = m_tempRangeRect.left() - nOffset;
    int nRight = m_tempRangeRect.right();
    int nTop = m_tempRangeRect.top() - nOffset;
    int nBottom = m_tempRangeRect.bottom();

    painter.drawLine(nLeft, nTop, nRight, nTop);
    painter.drawLine(nLeft, nBottom, nRight, nBottom);
    painter.drawLine(nLeft, nTop, nLeft, nBottom);
    painter.drawLine(nRight, nTop, nRight, nBottom);

    painter.restore();
}

void GLDTableView::print()
{
    //TODO
}

void GLDTableView::setDrawBoundLine(bool drawBoundLine)
{
    Q_D(GLDTableView);
    d->m_bDrawBoundLine = drawBoundLine;
}

bool GLDTableView::drawBoundLine()
{
    Q_D(GLDTableView);
    return d->m_bDrawBoundLine;
}

void GLDTableView::edit(const QModelIndex &index)
{
    Q_D(GLDTableView);

    if (!d->isIndexValid(index))
    {
        qWarning("edit: index was invalid");
    }

    // 优先处理行或列上的delegate,因为行或列上的delegate优先级高于ItemDelegate
    GlodonDefaultItemDelegate* pDelegate = d->delegateForIndex(index);

    pDelegate->setCurrTreeEditting(index);

    bool breadOnly = false;
    GEditStyle es = pDelegate->editStyle(dataIndex(index), breadOnly);

    if (es == esNone || es == esBool)
    {
        return;
    }

    //设置NoEditTrigger后不让进入编辑
    if (0 == (d->m_editTriggers & AllEditTriggers))
    {
        return;
    }

    if (!edit(index, AllEditTriggers, 0))
    {
        qWarning("edit: editing failed");
    }
}

void GLDTableView::setSelectionModel(QItemSelectionModel *selectionModel)
{
    Q_D(GLDTableView);
    Q_ASSERT(selectionModel);
    d->m_verticalHeader->setSelectionModel(selectionModel);
    d->m_horizontalHeader->setSelectionModel(selectionModel);
    GlodonAbstractItemView::setSelectionModel(selectionModel);
}

GlodonHeaderView *GLDTableView::horizontalHeader() const
{
    Q_D(const GLDTableView);
    return d->m_horizontalHeader;
}

GlodonHeaderView *GLDTableView::verticalHeader() const
{
    Q_D(const GLDTableView);
    return d->m_verticalHeader;
}

void GLDTableView::setHorizontalHeader(GlodonHeaderView *header)
{
    Q_D(GLDTableView);

    if (!header || header == d->m_horizontalHeader)
    {
        return;
    }

    if (d->m_horizontalHeader && d->m_horizontalHeader->parent() == this)
    {
        freeAndNil(d->m_horizontalHeader);
    }

    d->m_horizontalHeader = header;
    d->m_horizontalHeader->setParent(this);

    if (!d->m_horizontalHeader->model())
    {
        d->m_horizontalHeader->setModel(d->m_model);

        if (d->m_selectionModel)
        {
            d->m_horizontalHeader->setSelectionModel(d->m_selectionModel);
        }
    }

    d->m_horizontalHeader->setFixedCount(d->m_nFixedColCount);

    connect(d->m_horizontalHeader, SIGNAL(sectionResized(int, int, int, bool)),
            this, SLOT(columnResized(int, int, int, bool)));
    connect(d->m_horizontalHeader, SIGNAL(canSectionMove(int, int, bool &)),
            this, SLOT(canColumnMoved(int, int, bool &)));
    connect(d->m_horizontalHeader, SIGNAL(sectionMoved(int, int, int)),
            this, SLOT(columnMoved(int, int, int)));
    connect(d->m_horizontalHeader, SIGNAL(sectionCountChanged(int, int)),
            this, SLOT(columnCountChanged(int, int)));
    connect(d->m_horizontalHeader, SIGNAL(sectionPressed(int)), this, SLOT(selectColumn(int)));
    connect(d->m_horizontalHeader, SIGNAL(sectionEntered(int)), this, SLOT(_q_selectColumn(int)));
    connect(d->m_horizontalHeader, SIGNAL(sectionHandleDoubleClicked(int)),
            this, SLOT(resizeColumnToContents(int)));
    connect(d->m_horizontalHeader, SIGNAL(geometriesChanged()), this, SLOT(updateGeometries()));
    connect(d->m_verticalHeader, SIGNAL(scrolled(int)), this, SLOT(onScrolled(int)));
    connect(d->m_horizontalHeader, SIGNAL(scrolled(int)), this, SLOT(onScrolled(int)));

    connect(d->m_horizontalHeader, &GlodonHeaderView::sectionResizing,
            this, &GLDTableView::showSectionResizingInfoFrame);

    //update the sorting enabled states on the new header
    //强制执行setSortingEnabled
    bool bsortEnabled = d->m_sortingEnabled;
    d->m_sortingEnabled = !bsortEnabled;
    setSortingEnabled(bsortEnabled);

    if (GlodonMultiHeaderView *horizontalHeader = dynamic_cast<GlodonMultiHeaderView *>(d->m_horizontalHeader))
    {
        connect(horizontalHeader, SIGNAL(headerSpanPressed(int, int, int, int)),
                this, SLOT(_q_selectColumns(int, int, int, int)));
    }
    else if (GlodonGroupHeaderView *horizontalHeader = dynamic_cast<GlodonGroupHeaderView *>(d->m_horizontalHeader))
    {
        connect(horizontalHeader, SIGNAL(cgFrameChanged()), this, SLOT(updateGeometries()));
    }
}

void GLDTableView::setVerticalHeader(GlodonHeaderView *header)
{
    Q_D(GLDTableView);

    if (!header || header == d->m_verticalHeader)
    {
        return;
    }

    if (d->m_verticalHeader && d->m_verticalHeader->parent() == this)
    {
        delete d->m_verticalHeader;
    }

    d->m_verticalHeader = header;
    d->m_verticalHeader->setParent(this);

    if (!d->m_verticalHeader->model())
    {
        d->m_verticalHeader->setModel(d->m_model);

        if (d->m_selectionModel)
        {
            d->m_verticalHeader->setSelectionModel(d->m_selectionModel);
        }
    }

    d->m_verticalHeader->setFixedCount(d->m_nFixedRowCount);

    connect(d->m_verticalHeader, SIGNAL(sectionResized(int, int, int, bool)),
            this, SLOT(rowResized(int, int, int, bool)));
    connect(d->m_verticalHeader, SIGNAL(canSectionMove(int, int, bool &)),
            this, SLOT(canRowMoved(int, int, bool &)));
    connect(d->m_verticalHeader, SIGNAL(sectionMoved(int, int, int)),
            this, SLOT(rowMoved(int, int, int)));
    connect(d->m_verticalHeader, SIGNAL(sectionCountChanged(int, int)),
            this, SLOT(rowCountChanged(int, int)));
    connect(d->m_verticalHeader, SIGNAL(sectionPressed(int)), this, SLOT(selectRow(int)));
    connect(d->m_verticalHeader, SIGNAL(sectionEntered(int)), this, SLOT(_q_selectRow(int)));
    connect(d->m_verticalHeader, SIGNAL(sectionHandleDoubleClicked(int)),
            this, SLOT(resizeRowToContents(int)));
    connect(d->m_verticalHeader, SIGNAL(geometriesChanged()), this, SLOT(updateGeometries()));

    connect(d->m_verticalHeader, &GlodonHeaderView::sectionResizing,
            this, &GLDTableView::showSectionResizingInfoFrame);

    if (GlodonMultiHeaderView *verticalHeader = dynamic_cast<GlodonMultiHeaderView *>(d->m_verticalHeader))
    {
        connect(verticalHeader, SIGNAL(headerSpanPressed(int, int, int, int)), this, SLOT(_q_selectRows(int, int, int, int)));
    }
}

void GLDTableView::setShowEllipsisButton(bool value)
{
    Q_D(GLDTableView);
    d->m_showEllipsisButton = value;

    if (NULL == d->m_ellipsisButton)
    {
        QPixmap pixMap(":/icons/EllipsisButton_Normal");
        d->m_ellipsisButton = new QPushButton(this);
        d->m_ellipsisButton->setFixedSize(pixMap.size());
        d->m_ellipsisButton->setStyleSheet("QPushButton{ border-image: url(:/icons/EllipsisButton_Normal);} \
                                           QPushButton:hover { border-image: url(:/icons/EllipsisButton_Hover);} \
                                           QPushButton:pressed {border-image: url(:/icons/EllipsisButton_Click);}");
        d->m_ellipsisButton->setFixedSize(pixMap.size());
        connect(d->m_ellipsisButton, SIGNAL(clicked()), this, SIGNAL(onEllipsisButtonClick()));
    }

    if (value)
    {
        resetEllipsisButtonLocation();
        d->m_ellipsisButton->setVisible(true);
    }
    else
    {
        d->m_ellipsisButton->setVisible(false);
    }
}

bool GLDTableView::isShowEllipsisButton()
{
    Q_D(GLDTableView);
    return d->m_showEllipsisButton;
}


GlodonAbstractItemView::EditStyleDrawType GLDTableView::editStyleDrawType() const
{
    Q_D(const GLDTableView);
    return d->m_editStyleDrawType;
}

void GLDTableView::setEditStyleDrawType(GlodonAbstractItemView::EditStyleDrawType drawType)
{
    Q_D(GLDTableView);

    if (d->m_editStyleDrawType == drawType)
    {
        return;
    }

    d->m_editStyleDrawType = drawType;

    update(currentIndex());
}

void GLDTableView::scrollContentsBy(int dx, int dy)
{
    Q_D(GLDTableView);

    d->m_delayedAutoScroll.stop();

    scrollHorizontalContentsBy(dx);
    scrollVerticalContentsBy(dy);

    // TODO wangdd-a，这个方法不是很懂，注释掉没有看出来有啥问题。。。
    updateFirstLine(dx, dy);

    resetEllipsisButtonLocation();
//    resetCommentPosition(true, dx, dy);
}

bool GLDTableView::isEditorInFixedColAfterHScroll()
{
    Q_D(GLDTableView);

    if (!isEditing())
    {
        return false;
    }

    QModelIndex oCurIndex = currentIndex();
    const int nCurLogicalColumn = oCurIndex.column();
    const int nCurIndexHorizontalPos = columnViewportPosition(nCurLogicalColumn);
    const int nCurIndexColumnWidth = columnWidth(nCurLogicalColumn);
    const int nColumnXEndPos = nCurIndexHorizontalPos + nCurIndexColumnWidth;

    // 如果当前处于编辑状态，当editing的列被可编辑固定列覆盖掉时，应该把edit隐藏，并且不需要调用外层的d->scrollContentsBy(dx, 0);
    // 不在可编辑固定列上，且当前被遮挡
    if (d->isCoverByFixedColumn(nCurLogicalColumn, nColumnXEndPos, false))
    {
        if (d->m_currentEditor->isVisible())
        {
            d->m_currentEditor->hide();
        }
    }
    // 不在可编辑固定列上，且当前没有被遮挡
    else if (nColumnXEndPos > d->m_nFixedColWidth)
    {
        if (d->m_currentEditor->isHidden())
        {
            d->m_currentEditor->show();
        }        
    }
    // 在可编辑固定列上，永远不会被遮挡
    else
    {
        return true;
    }

    return false;
}

bool GLDTableView::isEditorInFixedRowAfterVScroll()
{
    Q_D(GLDTableView);

    if (!isEditing())
    {
        return false;
    }

    QModelIndex oCurIndex = currentIndex();
    const int nCurLogicalRow = oCurIndex.row();
    const int nCurIndexVerticalPos = rowViewportPosition(nCurLogicalRow);
    const int nCurIndexRowHeight = rowHeight(nCurLogicalRow);
    const int nRowYEndPos = nCurIndexVerticalPos + nCurIndexRowHeight;

    // 如果当前处于编辑状态，当editing的列被可编辑固定列覆盖掉时，应该把edit隐藏，并且不需要调用外层的d->scrollContentsBy(dx, 0);
    // 不在可编辑固定行上，且当前被遮挡
    if (d->isCoverByFixedRow(nCurLogicalRow, nRowYEndPos, false))
    {
        if (d->m_currentEditor->isVisible())
        {
            d->m_currentEditor->hide();
        }
    }
    // 不在可编辑固定行上，且当前没有被遮挡
    else if (nRowYEndPos > d->m_nFixedRowHeight)
    {
        if (d->m_currentEditor->isHidden())
        {
            d->m_currentEditor->show();
        }
    }
    // 在可编辑固定行上，永远不会被遮挡
    else
    {
        return true;
    }

    return false;
}

QStyleOptionViewItem GLDTableView::viewOptions() const
{
    QStyleOptionViewItem option = GlodonAbstractItemView::viewOptions();
    option.showDecorationSelected = true;
    return option;
}

void GLDTableView::paintEvent(QPaintEvent *event)
{
    Q_D(GLDTableView);
    QStylePainter painter(d->viewport);
    QRegion region = event->region();
    paint(painter, region);
}

QModelIndex GLDTableView::indexAt(const QPoint &pos) const
{
    Q_D(const GLDTableView);
    d->executePostedLayout();
    int nRow = rowAt(pos.y());
    int nColumn = columnAt(pos.x());

    if (nRow < 0 || nColumn < 0)
    {
        return QModelIndex();
    }

    if (d->hasSpans())
    {
        GSpanCollection::GSpan span = d->span(nRow, nColumn);
        nRow = span.top();
        nColumn = span.left();
    }
    return d->m_model->index(nRow, nColumn, d->m_root);
}

int GLDTableView::horizontalOffset() const
{
    Q_D(const GLDTableView);
    return d->m_horizontalHeader->offset();
}

int GLDTableView::verticalOffset() const
{
    Q_D(const GLDTableView);
    return d->m_verticalHeader->offset();
}

QModelIndex GLDTableView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_D(GLDTableView);

    // make sure that bottom is the bottommost *visible* row
    int nBottom = getBottomVisibleRow();
    int nRight = getRightVisibleCol();
    if (nBottom == -1 || nRight == -1)
    {
        return QModelIndex(); // model is empty
    }

    // 如果当前没有焦点Index，则找出左上角第一个可见格子，并返回index
    QModelIndex oCurIndex = currentIndex();
    if (!oCurIndex.isValid())
    {
        return leftTopVisualIndex(nRight, nBottom);
    }

    // Update visual cursor if current index has changed.
    updateVisualCursor();

    int nLastVisualRow = getLastVisualRow(nBottom);
    int nLastVisualCol = getLastVisualCol(nRight);

    adjustRightToLeftCursorAction(cursorAction);

    switch (cursorAction)
    {
    case MoveUp:
    {
        dealWithMoveUp(nLastVisualRow, nLastVisualCol);
        break;
    }
    case MoveDown:
    {
        dealWithMoveDown(nLastVisualRow, nLastVisualCol, nBottom);
        break;
    }
    case MovePrevious:
    case MoveLeft:
    {
        // Ctrl + Left 向左滚动一屏
        if (modifiers & Qt::CTRL && cursorAction == MoveLeft)
        {
            return getLeftPageCursorIndex();
        }

        dealWithMoveLeft(nLastVisualRow, nLastVisualCol, nRight, nBottom, cursorAction);
        break;
    }
    case MoveNext:
    case MoveRight:
    {
        // Ctrl + Right 向右滚动一屏
        if (modifiers & Qt::CTRL && cursorAction == MoveRight)
        {
            return getRightPageCursorIndex(nRight);
        }

        dealWithMoveRight(nLastVisualRow, nLastVisualCol, nRight, nBottom, cursorAction);
        break;
    }
    case MoveHome:
    {
        // Home 定位到当前行开头
        // Ctrl + Home 定位到左上角Index
        dealWithMoveHome(nLastVisualRow, nLastVisualCol, nRight, nBottom, modifiers);
        break;
    }
    case MoveEnd:
    {
        // End 定位到当前行结束
        // Ctrl + End 定位到右下角Index
        dealWithMoveEnd(nLastVisualRow, nLastVisualCol, nRight, nBottom, modifiers);
        break;
    }
    case MovePageUp:
    {
        return getUpPageCursorIndex();
    }
    case MovePageDown:
    {
        return getDownPageCursorIndex(nBottom);
    }
    }

    d->m_visualCursor = QPoint(nLastVisualCol, nLastVisualRow);
    return getCursorIndex(nLastVisualRow, nLastVisualCol);
}

void GLDTableView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    Q_D(GLDTableView);

    QModelIndex tl = indexAt(
                         QPoint(isRightToLeft() ? qMax(rect.left(), rect.right())
                                : qMin(rect.left(), rect.right()), qMin(rect.top(), rect.bottom())));
    QModelIndex br = indexAt(
                         QPoint(isRightToLeft() ? qMin(rect.left(), rect.right())
                                : qMax(rect.left(), rect.right()), qMax(rect.top(), rect.bottom())));
    QModelIndexList list = selectionModel()->selectedIndexes();

    if (!d->m_selectionModel || !tl.isValid() || !br.isValid() || !d->isIndexEnabled(tl) || !d->isIndexEnabled(br))
    {
        return;
    }

    if ((rect.left() < d->m_pressedPosition.x() && rect.right() > d->m_pressedPosition.x()) ||
        (rect.top() < d->m_pressedPosition.x() && rect.bottom() > d->m_pressedPosition.y()))
    {
        if (list.contains(tl))
        {
            d->m_oTopLeftSelectIndex = tl;
        }
    }
    else
    {
        d->m_oTopLeftSelectIndex = tl;
    }

    d->m_oBottomRightSelectIndex = br;

    d->m_rangeFillHandlePositon = RightBottom;

//    setSelectionByIndex(visualIndex(tl), visualIndex(br), command);
    setSelectionByIndex(visualIndex(d->m_oTopLeftSelectIndex), visualIndex(br), command);
}

void GLDTableView::setSelectionByIndex(const QModelIndex &tl, const QModelIndex &br,
                                       QItemSelectionModel::SelectionFlags command)
{
    Q_D(GLDTableView);

    if (!d->m_selectionModel || !tl.isValid() || !br.isValid() || !d->isIndexEnabled(tl) || !d->isIndexEnabled(br))
    {
        return;
    }

    bool bverticalMoved = verticalHeader()->sectionsMoved();
    bool bhorizontalMoved = horizontalHeader()->sectionsMoved();

    QItemSelection selection;

    int ntop = tl.row();
    int nleft = tl.column();
    int nbottom = br.row();
    int nright = br.column();

    if (d->hasSpans())
    {
        bool bexpanded(false);

        do
        {
            bexpanded = false;
            foreach(GSpanCollection::GSpan * it, d->m_spans.spans)
            {
                const GSpanCollection::GSpan &span = *it;
                int nt = d->visualRow(span.top());
                int nl = d->visualColumn(span.left());
                int nb = d->visualRow(d->rowSpanEndLogical(span.top(), span.height()));
                int nr = d->visualColumn(d->columnSpanEndLogical(span.left(), span.width()));

                if ((nt > nbottom) || (nl > nright) || (ntop > nb) || (nleft > nr))
                {
                    continue;    // no intersect
                }

                if (nt < ntop)
                {
                    ntop = nt;
                    bexpanded = true;
                }

                if (nl < nleft)
                {
                    nleft = nl;
                    bexpanded = true;
                }

                if (nb > nbottom)
                {
                    nbottom = nb;
                    bexpanded = true;
                }

                if (nr > nright)
                {
                    nright = nr;
                    bexpanded = true;
                }

                if (bexpanded)
                {
                    break;
                }
            }
        }
        while (bexpanded);

        for (int nhorizontal = nleft; nhorizontal <= nright; ++nhorizontal)
        {
            int col = d->logicalColumn(nhorizontal);

            for (int nvertical = ntop; nvertical <= nbottom; ++nvertical)
            {
                int row = d->logicalRow(nvertical);

                if (d->isRowHidden(row))
                {
                    continue;
                }

                QModelIndex index = d->m_model->index(row, col, d->m_root);
                selection.append(QItemSelectionRange(index));
            }
        }
    }
    else if (bverticalMoved && bhorizontalMoved)
    {
        for (int nhorizontal = nleft; nhorizontal <= nright; ++nhorizontal)
        {
            int col = d->logicalColumn(nhorizontal);

            for (int nvertical = ntop; nvertical <= nbottom; ++nvertical)
            {
                int row = d->logicalRow(nvertical);

                if (d->isRowHidden(row))
                {
                    continue;
                }

                QModelIndex index = d->m_model->index(row, col, d->m_root);
                selection.append(QItemSelectionRange(index));
            }
        }
    }
    else if (bhorizontalMoved)
    {
        for (int nVisual = nleft; nVisual <= nright; ++nVisual)
        {
            int col = d->logicalColumn(nVisual);

            if (d->isColumnHidden(col))
            {
                continue;
            }

            QModelIndex topLeft = d->m_model->index(tl.row(), col, d->m_root);
            QModelIndex bottomRight = d->m_model->index(br.row(), col, d->m_root);
            selection.append(QItemSelectionRange(topLeft, bottomRight));
        }
    }
    else if (bverticalMoved)
    {
        for (int nVisual = ntop; nVisual <= nbottom; ++nVisual)
        {
            int row = d->logicalRow(nVisual);

            if (d->isRowHidden(row))
            {
                continue;
            }

            QModelIndex topLeft = d->m_model->index(row, tl.column(), d->m_root);
            QModelIndex bottomRight = d->m_model->index(row, br.column(), d->m_root);
            selection.append(QItemSelectionRange(topLeft, bottomRight));
        }
    }
    else
    {
        // nothing moved
        QItemSelectionRange range(tl, br);

        if (!range.isEmpty())
        {
            selection.append(range);
        }
    }

    d->m_selectionModel->select(selection, command);
}

QRegion GLDTableView::visualRegionForSelection(const QItemSelection &selection) const
{
    Q_D(const GLDTableView);

    if (selection.isEmpty())
    {
        return QRegion();
    }

    QRegion selectionRegion;
    const QRect &viewportRect = d->viewport->rect();
    bool bVerticalMoved = verticalHeader()->sectionsMoved();
    bool bHorizontalMoved = horizontalHeader()->sectionsMoved();

    if ((bVerticalMoved && bHorizontalMoved) || (d->hasSpans() && (bVerticalMoved || bHorizontalMoved)))
    {
        for (int i = 0; i < selection.count(); ++i)
        {
            QItemSelectionRange range = selection.at(i);

            if (range.parent() != d->m_root || !range.isValid())
            {
                continue;
            }

            for (int row = range.top(); row <= range.bottom(); ++row)
            {
                for (int col = range.left(); col <= range.right(); ++col)
                {
                    const QRect &rangeRect = visualRect(d->m_model->index(row, col, d->m_root));

                    if (viewportRect.intersects(rangeRect))
                    {
                        selectionRegion += rangeRect;
                    }
                }
            }
        }
    }
    else if (bHorizontalMoved)
    {
        for (int i = 0; i < selection.count(); ++i)
        {
            QItemSelectionRange range = selection.at(i);

            if (range.parent() != d->m_root || !range.isValid())
            {
                continue;
            }

            int ntop = rowViewportPosition(range.top());
            int nbottom = rowViewportPosition(range.bottom()) + rowHeight(range.bottom());

            if (ntop > nbottom)
            {
                qSwap<int>(ntop, nbottom);
            }

            int nheight = nbottom - ntop;

            for (int col = range.left(); col <= range.right(); ++col)
            {
                const QRect rangeRect(columnViewportPosition(col), ntop, columnWidth(col), nheight);

                if (viewportRect.intersects(rangeRect))
                {
                    selectionRegion += rangeRect;
                }
            }
        }
    }
    else if (bVerticalMoved)
    {
        for (int i = 0; i < selection.count(); ++i)
        {
            QItemSelectionRange range = selection.at(i);

            if (range.parent() != d->m_root || !range.isValid())
            {
                continue;
            }

            int nLeft = columnViewportPosition(range.left());
            int nRight = columnViewportPosition(range.right()) + columnWidth(range.right());

            if (nLeft > nRight)
            {
                qSwap<int>(nLeft, nRight);
            }

            int nWidth = nRight - nLeft;

            for (int row = range.top(); row <= range.bottom(); ++row)
            {
                const QRect c_rangeRect(nLeft, rowViewportPosition(row), nWidth, rowHeight(row));

                if (viewportRect.intersects(c_rangeRect))
                {
                    selectionRegion += c_rangeRect;
                }
            }
        }
    }
    else // nothing moved
    {
        for (int i = 0; i < selection.count(); ++i)
        {
            QItemSelectionRange range = selection.at(i);

            if (range.parent() != d->m_root || !range.isValid())
            {
                continue;
            }

            d->trimHiddenSelections(&range);

            const int c_rtop = rowViewportPosition(range.top());
            const int c_rbottom = rowViewportPosition(range.bottom()) + rowHeight(range.bottom());
            int nrleft;
            int nrright;

            if (isLeftToRight())
            {
                nrleft = columnViewportPosition(range.left());
                nrright = columnViewportPosition(range.right()) + columnWidth(range.right());
            }
            else
            {
                nrleft = columnViewportPosition(range.right());
                nrright = columnViewportPosition(range.left()) + columnWidth(range.left());
            }

            const QRect c_rangeRect(QPoint(nrleft, c_rtop),
                                    QPoint(nrright, c_rbottom));

            if (viewportRect.intersects(c_rangeRect))
            {
                selectionRegion += c_rangeRect;
            }

            if (d->hasSpans())
            {
                foreach(GSpanCollection::GSpan * s,
                        d->m_spans.spansInRect(range.left(), range.top(), range.width(), range.height()))
                {
                    if (range.contains(s->top(), s->left(), range.parent()))
                    {
                        const QRect &visualSpanRect = d->visualSpanRect(*s);

                        if (viewportRect.intersects(visualSpanRect))
                        {
                            selectionRegion += visualSpanRect;
                        }
                    }
                }
            }
        }
    }

    return selectionRegion;
}

QRegion GLDTableView::visualRegionForSelectionWithSelectionBounding(const QItemSelection &selection) const
{
    QRegion oVisualRegion = visualRegionForSelection(selection);

    QRegion oContainSelectionBorderRegion;
    QVector<QRect> oVisualRects = oVisualRegion.rects();
    for (int i = 0; i < oVisualRects.count(); ++i)
    {
        oContainSelectionBorderRegion += oVisualRects[i].adjusted(-3, -3, 3, 3);
    }
    return oContainSelectionBorderRegion;
}


QModelIndexList GLDTableView::selectedIndexes() const
{
    Q_D(const GLDTableView);
    QModelIndexList viewSelected;
    QModelIndexList modelSelected;

    if (d->m_selectionModel)
    {
        modelSelected = d->m_selectionModel->selectedIndexes();
    }

    for (int i = 0; i < modelSelected.count(); ++i)
    {
        QModelIndex index = modelSelected.at(i);

        if (!isIndexHidden(index) && index.parent() == d->m_root)
        {
            viewSelected.append(index);
        }
    }

    return viewSelected;
}

QModelIndexList GLDTableView::indexesFromRect(QRect rect, int &rowCount, int &columnCount)
{
    Q_D(GLDTableView);

    rowCount = rect.bottom() - rect.top() + 1;
    columnCount = rect.right() - rect.left() + 1;

    QModelIndexList indexes;
    for (int r = rect.top(); r <= rect.bottom(); ++r)
    {
        for (int c = rect.left(); c <= rect.right(); ++c)
        {
            indexes.push_back(dataIndex(d->m_model->index(r, c)));
        }
    }

    return indexes;
}

void GLDTableView::rowCountChanged(int oldCount, int newCount)
{
    Q_D(GLDTableView);

    GlodonMultiHeaderView *pMultiVertHeader = dynamic_cast<GlodonMultiHeaderView *>(verticalHeader());
    if (NULL != pMultiVertHeader)
    {
        pMultiVertHeader->initTitles();
    }

    //when removing rows, we need to disable updates for the header until the geometries have been
    //updated and the offset has been adjusted, or we risk calling paintSection for all the sections
    if (newCount < oldCount)
    {
        d->m_verticalHeader->setUpdatesEnabled(false);
    }
    else if (newCount > oldCount && d->m_showEllipsisButton)
    {
        updateGeometries();
    }

    d->doDelayedItemsLayout();
}

void GLDTableView::columnCountChanged(int oldCount, int newCount)
{
    Q_D(GLDTableView);
    G_UNUSED(newCount);

    // 当列发生变化的时候，应该清除自动行高等缓存, oldCount为0时不应该清除，应该可能还在初始化
    // TODO liurx 列变化的时候，情况了自动行高，自动列宽，之后怎么处理？
    if (oldCount != 0)
    {
        d->m_oFitColWidthCols.clear();
        d->m_oSuitColWidthCols.clear();
        d->m_oSuitRowHeightAccordingCols.clear();
    }

    d->fillSuitColWidthCols();
    d->fillSuitRowHeightCols();

    GlodonMultiHeaderView *pMultiHoriHeader = dynamic_cast<GlodonMultiHeaderView *>(d->m_horizontalHeader);
    if (NULL != pMultiHoriHeader)
    {
        pMultiHoriHeader->initTitles();
    }

    updateGeometries();

    if (horizontalScrollMode() == GlodonAbstractItemView::ScrollPerItem)
    {
        d->m_horizontalHeader->setOffsetToSectionPosition(horizontalScrollBar()->value());
    }
    else
    {
        d->m_horizontalHeader->setOffset(horizontalScrollBar()->value());
    }

    d->viewport->update();
}

void GLDTableView::showSectionResizingInfoFrame(
        const QPoint &mousePostion, Qt::Orientation direction, GlodonHeaderView::ResizeState state)
{
    Q_D(GLDTableView);

    if (!d->m_isShowResizeInfoFrame)
    {
        return;
    }

    initResizeInfoFrames(direction);

    if (state == GlodonHeaderView::Press)
    {
        QPoint oDestPoint = d->calcResizeInfoLineFramePosition(direction);
        d->m_pResizeInfoLineFrame->move(oDestPoint);

        d->m_pResizeInfoLineFrame->show();

        initSectionResizingInfoFrame(mousePostion, direction);
        d->m_infoFrame->show();
    }
    else if (state == GlodonHeaderView::Move)
    {
        QPoint oDestPoint = d->calcResizeInfoLineFramePosition(direction);
        d->m_pResizeInfoLineFrame->move(oDestPoint);

        updateSectionResizingInfoFrameText(direction);
    }
    else
    {
        d->m_pResizeInfoLineFrame->hide();
        d->m_infoFrame->hide();
    }
}

void GLDTableView::updateGeometries()
{
    Q_D(GLDTableView);

    if (d->m_geometryRecursionBlock)
    {
        return;
    }
    d->m_geometryRecursionBlock = true;

    int nVerticalHeaderWidth = d->m_verticalHeader->drawWidth();
    int nHorizontalHeaderHeight = d->m_horizontalHeader->drawWidth();

    setViewportMargins(nVerticalHeaderWidth, nHorizontalHeaderHeight, 0, 0);

    // update headers
    updateVerticalHeaderGeometry(nVerticalHeaderWidth);
    updateHorizontalHeaderGeometry(nHorizontalHeaderHeight);

    // update cornerWidget
    updateConrnerWidgetGeometry(nVerticalHeaderWidth, nHorizontalHeaderHeight);

    // update scroll bars
    // ### move this block into the if
    QSize oViewportSize = d->viewport->size();
    QSize oMaxViewportSize = maximumViewportSize();
    uint nHorizontalHeaderLength = d->m_horizontalHeader->length();
    uint nVerticalHeaderLength = d->m_verticalHeader->length();

    if ((uint)oMaxViewportSize.width() >= nHorizontalHeaderLength
            && (uint)oMaxViewportSize.height() >= nVerticalHeaderLength)
    {
        oViewportSize = oMaxViewportSize;
    }

    updateHorizontalScrollBar(oViewportSize);
    updateVerticalScrollBar(oViewportSize);

    d->m_geometryRecursionBlock = false;
    GlodonAbstractItemView::updateGeometries();
}

int GLDTableView::sizeHintForRow(int row) const
{
    Q_D(const GLDTableView);

    int nLeft = firstVisualCol();
    int nRight = lastVisualCol();

    adjustVisualCol(nLeft, nRight);

    QSet<int> oNeedCalcSizeHintCols;

    for (int i = 0; i < d->m_nFixedColCount; ++i)
    {
        oNeedCalcSizeHintCols.insert(i);
    }

    for (int i = nLeft; i <= nRight; ++i)
    {
        oNeedCalcSizeHintCols.insert(i);
    }

    return d->calcSizeHintForRow(row, oNeedCalcSizeHintCols.toList());
}

int GLDTableView::sizeHintForColumn(int column) const
{
    Q_D(const GLDTableView);

    return d->sizeHintForColumn(column, false);
}

int GLDTableView::calcSuitHeight(const QList<int> &oNeedCalcHeightCols, int nLogicalRow)
{
    Q_D(GLDTableView);
    return d->calcSuitHeight(oNeedCalcHeightCols, nLogicalRow);
}

int GLDTableView::calcSuitWidth(int nLogicalCol)
{
    Q_D(GLDTableView);
    return d->calcSuitWidth(nLogicalCol, d->m_bCalcAllRows);
}

int GLDTableView::rowViewportPosition(int row) const
{
    Q_D(const GLDTableView);
    return d->m_verticalHeader->sectionViewportPosition(row);
}

int GLDTableView::rowVisualPosition(int visualRow) const
{
    Q_D(const GLDTableView);
    return d->m_verticalHeader->sectionVisualPosition(visualRow) - d->m_verticalHeader->offset();
}

int GLDTableView::rowAt(int y) const
{
    Q_D(const GLDTableView);
    return d->m_verticalHeader->logicalIndexAt(y);
}

int GLDTableView::visualRowAt(int y) const
{
    Q_D(const GLDTableView);
    return d->m_verticalHeader->visualIndexAt(y);
}

/*!
    \since 4.1

    Sets the height of the given \a row to be \a height.
*/
void GLDTableView::setRowHeight(int row, int height, bool isManual)
{
    Q_D(const GLDTableView);
    d->m_verticalHeader->resizeSection(row, height, true, isManual);
}

int GLDTableView::rowHeight(int row) const
{
    Q_D(const GLDTableView);
    return d->m_verticalHeader->sectionSize(row);
}

int GLDTableView::visualRowHeight(int visualRow) const
{
    Q_D(const GLDTableView);
    return d->m_verticalHeader->visualSectionSize(visualRow);
}

int GLDTableView::columnViewportPosition(int column) const
{
    Q_D(const GLDTableView);
    return d->m_horizontalHeader->sectionViewportPosition(column);
}

int GLDTableView::columnVisualPosition(int visualColumn) const
{
    Q_D(const GLDTableView);
    return d->m_horizontalHeader->sectionVisualPosition(visualColumn) - d->m_horizontalHeader->offset();
}

int GLDTableView::columnAt(int x) const
{
    Q_D(const GLDTableView);
    return d->m_horizontalHeader->logicalIndexAt(x);
}

int GLDTableView::visualColumnAt(int x) const
{
    Q_D(const GLDTableView);
    return d->m_horizontalHeader->visualIndexAt(x);
}

void GLDTableView::setColumnWidth(int column, int width)
{
    Q_D(const GLDTableView);
    d->m_horizontalHeader->resizeSection(column, width);
}

int GLDTableView::columnWidth(int column) const
{
    Q_D(const GLDTableView);
    return d->m_horizontalHeader->sectionSize(column);
}

int GLDTableView::visualColumnWidth(int visualColumn) const
{
    Q_D(const GLDTableView);
    return d->m_horizontalHeader->visualSectionSize(visualColumn);
}

bool GLDTableView::isRowHidden(int row) const
{
    Q_D(const GLDTableView);
    return d->m_verticalHeader->isSectionHidden(row);
}

void GLDTableView::setRowHidden(int row, bool hide)
{
    Q_D(GLDTableView);

    if (row < 0 || row >= d->m_verticalHeader->count())
    {
        return;
    }

    d->m_verticalHeader->setSectionHidden(row, hide);
}

bool GLDTableView::isColumnHidden(int column) const
{
    Q_D(const GLDTableView);
    return d->m_horizontalHeader->isSectionHidden(column);
}

void GLDTableView::setColumnHidden(int column, bool hide)
{
    Q_D(GLDTableView);

    if (column < 0 || column >= d->m_horizontalHeader->count())
    {
        return;
    }

    d->m_horizontalHeader->setSectionHidden(column, hide);
}

void GLDTableView::setSortingEnabled(bool enable)
{
    Q_D(GLDTableView);

    if (d->m_sortingEnabled != enable)
    {
        d->m_sortingEnabled = enable;
        horizontalHeader()->setSortIndicatorShown(enable);

        if (enable)
        {
            disconnect(d->m_horizontalHeader, SIGNAL(sectionEntered(int)),
                       this, SLOT(_q_selectColumn(int)));
            disconnect(horizontalHeader(), SIGNAL(sectionPressed(int)),
                       this, SLOT(selectColumn(int)));

            if (GlodonMultiHeaderView *horizontalHeader = dynamic_cast<GlodonMultiHeaderView *>(d->m_horizontalHeader))
                disconnect(horizontalHeader, SIGNAL(headerSpanPressed(int, int, int, int)),
                           this, SLOT(_q_selectColumns(int, int, int, int)));

            connect(horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
                    this, SLOT(sortByColumn(int)), Qt::UniqueConnection);

            sortByColumn(horizontalHeader()->sortIndicatorSection(),
                         horizontalHeader()->sortIndicatorOrder());
        }
        else
        {
            connect(d->m_horizontalHeader, SIGNAL(sectionEntered(int)),
                    this, SLOT(_q_selectColumn(int)), Qt::UniqueConnection);
            connect(horizontalHeader(), SIGNAL(sectionPressed(int)),
                    this, SLOT(selectColumn(int)), Qt::UniqueConnection);

            if (GlodonMultiHeaderView *horizontalHeader = dynamic_cast<GlodonMultiHeaderView *>(d->m_horizontalHeader))
                connect(horizontalHeader, SIGNAL(headerSpanPressed(int, int, int, int)),
                        this, SLOT(_q_selectColumns(int, int, int, int)));

            disconnect(horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
                       this, SLOT(sortByColumn(int)));
        }
    }
}

bool GLDTableView::isSortingEnabled() const
{
    Q_D(const GLDTableView);
    return d->m_sortingEnabled;
}

void GLDTableView::forceCloseEditor()
{
    Q_D(GLDTableView);
    bool bCanCloseEditor = true;
    closeEditor(d->delegateForIndex(d->m_oEditorIndex)->curEditor(),
                bCanCloseEditor, GlodonDefaultItemDelegate::RevertModelCache);
}

void GLDTableView::setDisplayResizeInfoFrame(bool value)
{
    Q_D(GLDTableView);
    d->m_isShowResizeInfoFrame = value;
}

GCustomCommentFrame *GLDTableView::addComment(const QModelIndex &index, QString value)
{
    Q_D(GLDTableView);
    GCustomCommentFrame *pCommentFrame = findOrCreateCommentFrame(index);
    d->m_model->setData(index, value, gidrCommentRole);
    pCommentFrame->setCommentText(value);
    pCommentFrame->show();
    return pCommentFrame;
}

GCustomCommentFrame *GLDTableView::editComment(const QModelIndex &index)
{
    Q_D(GLDTableView);

    if (GCustomCommentFrame *pCommentFrame = findOrCreateCommentFrame(index))
    {
        pCommentFrame->setCommentText(d->m_model->data(index, gidrCommentRole).toString());
        pCommentFrame->show();
        return pCommentFrame;
    }
    else
    {
        return NULL;
    }
}

bool GLDTableView::isShowComment(const QModelIndex &index)
{
    if (GCustomCommentFrame *pCommentFrame = findOrCreateCommentFrame(index, false))
    {
        return pCommentFrame->isVisible();
    }
    else
    {
        return false;
    }
}

GCustomCommentFrame *GLDTableView::showOrHideCommentPersistent(const QModelIndex &index, bool isShow)
{
    Q_D(GLDTableView);

    if (GCustomCommentFrame *pCommentFrame = findOrCreateCommentFrame(index))
    {
        if (isShow)
        {
            pCommentFrame->setCommentText(d->m_model->data(index, gidrCommentRole).toString());
            pCommentFrame->show(true);
        }
        else
        {
            pCommentFrame->hide();
        }

        return pCommentFrame;
    }
    else
    {
        return NULL;
    }
}

void GLDTableView::deleteComment(const QModelIndex &index)
{
    Q_D(GLDTableView);
    d->m_oCommentFrames.value(index)->deleteLater();
    d->m_oCommentFrames.remove(index);
}

void GLDTableView::showOrHideAllCommentPersistent(bool isShow)
{
    Q_D(GLDTableView);
    QMap<QPersistentModelIndex, GCustomCommentFrame *>::iterator it = d->m_oCommentFrames.begin();

    while (it != d->m_oCommentFrames.end())
    {
        if (isShow)
        {
            it.value()->show(true);
        }
        else
        {
            it.value()->hide();
        }

        it++;
    }
}

const GCustomCommentFrame *GLDTableView::findComment(const QModelIndex &index) const
{
    Q_D(const GLDTableView);

    return d->m_oCommentFrames.value(index);
}

void GLDTableView::setBoolEditValue(QModelIndex &currIndex)
{
    Q_D(GLDTableView);

    if (Qt::ItemIsEditable == (d->m_model->flags(currIndex) & Qt::ItemIsEditable))
    {
        bool breadOnly = false;

        GlodonDefaultItemDelegate *pDelegate = d->delegateForIndex(currIndex);
        pDelegate->setCurrTreeEditting(currIndex);

        GEditStyle editStyle = pDelegate->editStyle(dataIndex(currIndex), breadOnly);

        if (editStyle == esNone)
        {
            breadOnly = true;
        }

        if (!breadOnly)
        {
            bool bvalue = currIndex.data(Qt::DisplayRole).toBool();
            model()->setData(currIndex, !bvalue);
        }
    }
}

void GLDTableView::setAllowCopy(bool allowCopy)
{
    Q_D(GLDTableView);
    d->m_allowCopy = allowCopy;
}

bool GLDTableView::allowCopy() const
{
    Q_D(const GLDTableView);
    return d->m_allowCopy;
}

void GLDTableView::setAllowPaste(bool allowPaste)
{
    Q_D(GLDTableView);
    d->m_allowPaste = allowPaste;
}

bool GLDTableView::allowPaste() const
{
    Q_D(const GLDTableView);
    return d->m_allowPaste;
}

bool GLDTableView::isEditing() const
{
    Q_D(const GLDTableView);

    if (d->m_state == GlodonAbstractItemView::EditingState)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool GLDTableView::cellCanGetEdit(int row, int column)
{
    // 只要设置了NoEditTrigger，就是不能编辑的
    if (editTriggers() == NoEditTriggers)
    {
        return false;
    }

    QModelIndex index = dataIndex(model()->index(row, column));
    if (!index.isValid())
    {
        return false;
    }

    GlodonDefaultItemDelegate *pItemDelegate = itemDelegate();
    if (!pItemDelegate->editable(index))
    {
        return false;
    }

    bool bResdOnly = true;
    if (pItemDelegate->editStyle(index, bResdOnly) == esNone)
    {
        return false;
    }

    return true;
}

bool GLDTableView::isDisplayFilter()
{
    return false;
}

bool GLDTableView::totalRowAtFooter() const
{
    return false;
}

bool GLDTableView::showGrid() const
{
    Q_D(const GLDTableView);
    return d->m_showGrid;
}

void GLDTableView::setDrawTopAndLeftBorderLine(bool value)
{
    Q_D(GLDTableView);
    d->m_bDrawTopAndLeftBorderLine = value;
}

bool GLDTableView::drawTopAndLeftBorderLine()
{
    Q_D(const GLDTableView);
    return d->m_bDrawTopAndLeftBorderLine;
}

void GLDTableView::setShowGrid(bool show)
{
    Q_D(GLDTableView);

    if (d->m_showGrid == show)
        return;

    d->m_showGrid = show;

    d->setGridLineWidthToHorizontalHeader(show ? d->m_gridLineWidth : 0);
    d->setGridLineWidthToVertialHeader(show ? d->m_gridLineWidth : 0);

    d->viewport->update();
}

void GLDTableView::selectColumns(int start, int end)
{
    Q_D(GLDTableView);
    d->selectColumns(start, end, true);
}

void GLDTableView::selectRows(int start, int end)
{
    Q_D(GLDTableView);
    d->selectRows(start, end, true);
}

void GLDTableView::setCurrentIndex(const QModelIndex &dataIndex)
{
    QModelIndex index = treeIndex(dataIndex);
    Q_ASSERT(index.model() == model());
    GlodonAbstractItemView::setCurrentIndex(index);
}

void GLDTableView::setCurrentIndex(int row, int column)
{
    Q_D(GLDTableView);
    QModelIndex index = d->m_model->index(row, column);
    GlodonAbstractItemView::setCurrentIndex(index);
}

void GLDTableView::setSuitRowHeights(const GIntList &accordingCols)
{
    Q_D(GLDTableView);

    d->m_oSuitRowHeightAccordingCols = accordingCols;

    if (isVisible())
    {
        setGridDisplayRowHeights();
    }
}

GIntList GLDTableView::suitRowHeights()
{
    Q_D(GLDTableView);

    return d->m_oSuitRowHeightAccordingCols;
}

void GLDTableView::setSuitColWidths(const GIntList &suitColWidthCols)
{
    Q_D(GLDTableView);

    d->m_oSuitColWidthCols = suitColWidthCols;

    if (isVisible())
    {
        setGridDisplayColWidths();
    }
}

GIntList GLDTableView::suitColWidths()
{
    Q_D(GLDTableView);

    return d->m_oSuitColWidthCols;
}

void GLDTableView::setFitColWidths(const GIntList &fitColWidthsCols)
{
    Q_D(GLDTableView);

    d->m_oFitColWidthCols = fitColWidthsCols;

    if (isVisible())
    {
        adjustColWidths(-1);
    }
}

GIntList GLDTableView::fitColWidths()
{
    Q_D(GLDTableView);

    return d->m_oFitColWidthCols;
}

void GLDTableView::onScrolled(int offset)
{
    Q_UNUSED(offset);
    refreshDisplayColRow();
}

void GLDTableView::beforeReset()
{
    d_func()->m_bIsInReset = true;
    GlodonAbstractItemView::beforeReset();
}

void GLDTableView::doReset()
{
    Q_D(GLDTableView);
    d->m_legalData = true;
    d->m_prevState = GlodonAbstractItemView::FirstFocus;
    GlodonAbstractItemView::doReset();
}

void GLDTableView::afterReset()
{
    d_func()->m_bIsInReset = false;

    refreshDisplayColRow();
    GlodonAbstractItemView::afterReset();
}

void GLDTableView::canRowMoved(int from, int to, bool &canMove)
{
    Q_D(GLDTableView);

    for (int col = 0; col < horizontalHeader()->count(); ++col)
    {
        GSpanCollection::GSpan *fromSpan = d->m_spans.spanAt(col, from);
        GSpanCollection::GSpan *toSpan = d->m_spans.spanAt(col, to);

        if ((NULL != fromSpan) || (NULL != toSpan))
        {
            canMove = false;
        }
    }

    if (!canMove)
    {
        d->viewport->update();
    }
}

Qt::PenStyle GLDTableView::gridStyle() const
{
    Q_D(const GLDTableView);
    return d->m_gridStyle;
}

void GLDTableView::setGridStyle(Qt::PenStyle style)
{
    Q_D(GLDTableView);

    if (d->m_gridStyle == style)
        return;

    d->m_gridStyle = style;

    d->setGridLineWidthToHorizontalHeader((style == Qt::NoPen) ? 0 : d->m_gridLineWidth);
    d->setGridLineWidthToVertialHeader((style == Qt::NoPen) ? 0 : d->m_gridLineWidth);

    d->viewport->update();
}

void GLDTableView::setWordWrap(bool on)
{
    Q_D(GLDTableView);

    if (d->m_bWrapItemText == on)
    {
        return;
    }

    d->m_bWrapItemText = on;
    QMetaObject::invokeMethod(d->m_verticalHeader, "resizeSections");
    QMetaObject::invokeMethod(d->m_horizontalHeader, "resizeSections");
}

bool GLDTableView::wordWrap() const
{
    Q_D(const GLDTableView);
    return d->m_bWrapItemText;
}

void GLDTableView::setCornerButtonEnabled(bool enable)
{
    Q_D(GLDTableView);
    d->m_cornerWidget->setEnabled(enable);
}

bool GLDTableView::isCornerButtonEnabled() const
{
    Q_D(const GLDTableView);
    return d->m_cornerWidget->isEnabled();
}

QWidget *GLDTableView::cornerWidget() const
{
    Q_D(const GLDTableView);
    return d->m_cornerWidget;
}

void GLDTableView::setCornerWidget(QWidget *widget)
{
    Q_D(GLDTableView);

    QWidget *oldWidget = d->m_cornerWidget;

    if (oldWidget != widget)
    {
        if (oldWidget)
        {
            oldWidget->hide();
            oldWidget->deleteLater();
        }

        d->m_cornerWidget = widget;

        if (widget && widget->parentWidget() != this)
        {
            widget->setParent(this);
        }

        d->layoutChildren();

        if (widget)
        {
            widget->show();
        }
    }
    else
    {
        d->m_cornerWidget = widget;
        d->layoutChildren();
    }

    GTableCornerWidget *cornerWidget = dynamic_cast<GTableCornerWidget *>(d->m_cornerWidget);

    if (cornerWidget)
    {
        cornerWidget->setModel(d->m_model);
    }

    GlodonAbstractItemView *gView = dynamic_cast<GlodonAbstractItemView *>(d->m_cornerWidget);

    if (gView)
    {
        gView->setModel(d->m_model);
    }

    QAbstractItemView *qView = dynamic_cast<QAbstractItemView *>(d->m_cornerWidget);

    if (qView)
    {
        qView->setModel(d->m_model);
    }
}

void GLDTableView::setAllowRangeFilling(bool enable)
{
    Q_D(GLDTableView);

    if (d->m_allowRangeFilling != enable)
    {
        d->m_allowRangeFilling = enable;
        d->viewport->update();
    }
}

bool GLDTableView::allowRangeFilling() const
{
    Q_D(const GLDTableView);
    return d->m_allowRangeFilling;
}

void GLDTableView::setRangeFillingStyle(RangeFillingStyle style)
{
    Q_D(GLDTableView);
    d->m_rangeFillingStyle = style;
}

RangeFillingStyle GLDTableView::rangeFillingStyle()
{
    Q_D(GLDTableView);
    return d->m_rangeFillingStyle;
}

bool GLDTableView::highlightSections() const
{
    Q_D(const GLDTableView);
    return d->m_verticalHeader->highlightSections();
}

void GLDTableView::setHighlightSections(bool value)
{
    Q_D(GLDTableView);
    d->m_verticalHeader->setHighlightSections(value);
    d->m_horizontalHeader->setHighlightSections(value);
}

int GLDTableView::gridLineWidth() const
{
    Q_D(const GLDTableView);
    return d->m_gridLineWidth;
}

void GLDTableView::setGridLineWidth(int value)
{
    Q_D(GLDTableView);
    if (d->m_gridLineWidth == value)
        return;

    if (!d->m_bIsCustomStyle)
    {
        setIsCustomStyle(true);
    }

    d->m_gridLineWidth = value;

    if (d->m_showGrid)
    {
        d->setGridLineWidthToHorizontalHeader(value);
        d->setGridLineWidthToVertialHeader(value);
    }

    d->viewport->update();
}

QColor GLDTableView::gridLineColor() const
{
    Q_D(const GLDTableView);
    return d->m_gridLineColor;
}

void GLDTableView::setFrameLineColor(QColor value)
{
    Q_D(GLDTableView);
    d->m_oFrameLineColor = value;
}

QColor GLDTableView::frameLineColor() const
{
    Q_D(const GLDTableView);
    return d->m_oFrameLineColor;
}

void GLDTableView::setGridLineColor(QColor value)
{
    Q_D(GLDTableView);

    if (!d->m_bIsCustomStyle)
    {
        setIsCustomStyle(true);
    }

    d->m_gridLineColor = value;

    if (d->m_horizontalHeader)
    {
        d->m_horizontalHeader->setGridLineColor(value);
    }

    if (d->m_verticalHeader)
    {
        d->m_verticalHeader->setGridLineColor(value);
    }

    d->viewport->update();
}

bool GLDTableView::showVerticalGridLine() const
{
    Q_D(const GLDTableView);
    return d->m_bShowVerticalGridLine;
}

void GLDTableView::setShowVerticalGridLine(bool value)
{
    Q_D(GLDTableView);

    if (d->m_bShowVerticalGridLine != value)
    {
        d->m_bShowVerticalGridLine = value;

        if (value)
        {
            d->m_horizontalHeader->setGridLineWidth(d->m_gridLineWidth);
        }
        else
        {
            d->m_horizontalHeader->setGridLineWidth(0);
        }

        d->viewport->update();
    }
}

bool GLDTableView::showHorizontalGridLine() const
{
    Q_D(const GLDTableView);
    return d->m_bShowHorizontalGridLine;
}

void GLDTableView::setShowHorizontalGridLine(bool value)
{
    Q_D(GLDTableView);

    if (d->m_bShowHorizontalGridLine != value)
    {
        d->m_bShowHorizontalGridLine = value;

        if (value)
        {
            d->m_verticalHeader->setGridLineWidth(d->m_gridLineWidth);
        }
        else
        {
            d->m_verticalHeader->setGridLineWidth(0);
        }
    }

    d->viewport->update();
}

bool GLDTableView::allowSelectRow() const
{
    Q_D(const GLDTableView);
    return d->m_allowSelectRow;
}

void GLDTableView::setAllowSelectRow(bool value)
{
    Q_D(GLDTableView);
    d->m_allowSelectRow = value;
}

bool GLDTableView::allowSelectCol() const
{
    Q_D(const GLDTableView);
    return d->m_allowSelectCol;
}

void GLDTableView::setAllowSelectCol(bool value)
{
    Q_D(GLDTableView);
    d->m_allowSelectCol = value;
}

bool GLDTableView::cellFillEditField() const
{
    Q_D(const GLDTableView);
    return d->m_cellFillEditField;
}

void GLDTableView::setCellFillEditField(bool value)
{
    Q_D(GLDTableView);
    d->m_cellFillEditField = value;
}

void GLDTableView::setGridColor(QColor value)
{
    Q_D(GLDTableView);

    if (!d->m_bIsCustomStyle)
    {
        setIsCustomStyle(true);
    }

    d->m_horizontalHeader->setGridColor(value);
    d->m_verticalHeader->setGridColor(value);

    GlodonAbstractItemView::setGridColor(value);
}

void GLDTableView::setFixedColCount(int fixedColCount)
{
    Q_D(GLDTableView);
    d->m_nFixedColCount = fixedColCount;
    d->m_horizontalHeader->setFixedCount(fixedColCount);

    d->m_horizontalHeader->viewport()->update();
    d->viewport->update();
}

int GLDTableView::fixedColCount() const
{
    Q_D(const GLDTableView);
    return d->m_nFixedColCount;
}

void GLDTableView::setFixedRowCount(int fixedRowCount)
{
    Q_D(GLDTableView);
    d->m_nFixedRowCount = fixedRowCount;
    d->m_verticalHeader->setFixedCount(fixedRowCount);

    d->m_verticalHeader->viewport()->update();
    d->viewport->update();
}

int GLDTableView::fixedRowCount() const
{
    Q_D(const GLDTableView);
    return d->m_nFixedRowCount;
}

bool GLDTableView::isCustomStyle() const
{
    Q_D(const GLDTableView);
    return d->m_bIsCustomStyle;
}

void GLDTableView::setIsCustomStyle(bool value)
{
    Q_D(GLDTableView);

    if (d->m_bIsCustomStyle != value)
    {
        horizontalHeader()->setFixedCellEvent(value);
        verticalHeader()->setFixedCellEvent(value);
        d->m_bIsCustomStyle = value;
    }
}

void GLDTableView::zoomTableView(double factor)
{
    Q_D(GLDTableView);

    int nPrevSize = horizontalHeader()->sectionSize(horizontalHeader()->count() - 1);
    double dPrevWidth = horizontalHeader()->width();

    horizontalHeader()->zoomInSection(factor);
    verticalHeader()->zoomInSection(factor);

    horizontalHeader()->setInZoomingSection(true);

    if (dPrevWidth > 1)
    {
        GlodonDefaultItemDelegate *delegate = itemDelegate();
        double dRate = horizontalHeader()->width() / dPrevWidth;

        if (delegate->m_factor > 0.1)
        {
            dRate = factor / delegate->m_factor;
        }

        delegate->m_factor = factor;

        horizontalHeader()->resizeSection(horizontalHeader()->count() - 1, dRate * nPrevSize);
    }

    horizontalHeader()->setInZoomingSection(false);

    //        QTimer::singleShot(0, horizontalHeader(), SLOT(resizeSections()));
    //        d->m_horizontalHeader->setInZoomingSection(true);
    //        qDebug()<< QMetaObject::invokeMethod(d->m_horizontalHeader, "resizeSections");
    //        d->m_horizontalHeader->setInZoomingSection(false);
    if (d->m_model->rowCount() <= 0)
    {
        return;
    }

    d->viewport->update(d->viewport->rect());
}

void GLDTableView::setAlwaysShowRowSelectedBgColor(bool isShow)
{
    Q_D(GLDTableView);
    d->m_alwaysShowRowSelectedColor = isShow;
}

bool GLDTableView::alwaysShowRowSelectedBgColor()
{
    Q_D(GLDTableView);
    return d->m_alwaysShowRowSelectedColor;
}

QRect GLDTableView::visualRect(const QModelIndex &index) const
{
    Q_D(const GLDTableView);

    if (!d->isIndexValid(index)
            || (!d->hasSpans() && isIndexHidden(index)))
    {
        return QRect();
    }

    d->executePostedLayout();

    if (d->hasSpans())
    {
        GSpanCollection::GSpan span = d->span(index.row(), index.column());
        return d->visualSpanRect(span);
    }

    QVariant oBoundLine = index.data(gidrBoundLineRole);

    QRect oVisualRect = d->visualRectIncludeBoundingLineWidth(index);
    oVisualRect.setWidth(oVisualRect.width() - borderLineSubtractGridLineWidth(oBoundLine, true));
    oVisualRect.setHeight(oVisualRect.height() - borderLineSubtractGridLineWidth(oBoundLine, false));
    return oVisualRect;
}

QRect GLDTableView::visualRectIncludeBoundingLineWidth(const QModelIndex &index) const
{
    Q_D(const GLDTableView);

    if (!d->isIndexValid(index)
            || (!d->hasSpans() && isIndexHidden(index)))
    {
        return QRect();
    }

    d->executePostedLayout();

    if (d->hasSpans())
    {
        GSpanCollection::GSpan span = d->span(index.row(), index.column());
        return d->visualSpanRectIncludeBoundingLineWidth(span);
    }

    return d->visualRectIncludeBoundingLineWidth(index);
}

int GLDTableView::getBottomVisibleRow()
{
    Q_D(GLDTableView);
    int nBottom = d->m_model->rowCount(d->m_root) - 1;
    while (nBottom >= 0 && isRowHidden(d->logicalRow(nBottom)))
    {
        --nBottom;
    }

    return nBottom;
}

int GLDTableView::getRightVisibleCol()
{
    Q_D(GLDTableView);
    int nRight = d->m_model->columnCount(d->m_root) - 1;
    while (nRight >= 0 && isColumnHidden(d->logicalColumn(nRight)))
    {
        --nRight;
    }

    return nRight;
}

int GLDTableView::getLastVisualRow(int nBottom)
{
    Q_D(GLDTableView);
    int nVisualRow = d->m_visualCursor.y();
    if (nVisualRow > nBottom)
    {
        nVisualRow = nBottom;
    }
    Q_ASSERT(nVisualRow != -1);

    return nVisualRow;
}

int GLDTableView::getLastVisualCol(int nRight)
{
    Q_D(GLDTableView);
    int nVisualColumn = d->m_visualCursor.x();
    if (nVisualColumn > nRight)
    {
        nVisualColumn = nRight;
    }
    Q_ASSERT(nVisualColumn != -1);

    return nVisualColumn;
}

QModelIndex GLDTableView::leftTopVisualIndex(int nRight, int nBottom)
{
    Q_D(GLDTableView);

    // 找出最左边的可见列
    int nLeftColumn = 0;
    while (nLeftColumn < nRight && isColumnHidden(d->logicalColumn(nLeftColumn)))
    {
        ++nLeftColumn;
    }

    // 找出最上面的可见行
    int nTopRow = 0;
    while (isRowHidden(d->logicalRow(nTopRow)) && nTopRow < nBottom)
    {
        ++nTopRow;
    }

    d->m_visualCursor = QPoint(nLeftColumn, nTopRow);
    return d->m_model->index(d->logicalRow(nTopRow), d->logicalColumn(nLeftColumn), d->m_root);
}

void GLDTableView::updateVisualCursor()
{
    Q_D(GLDTableView);

    QModelIndex oCurIndex = currentIndex();
    QPoint oVisualCurrent(d->visualColumn(oCurIndex.column()), d->visualRow(oCurIndex.row()));
    if (oVisualCurrent != d->m_visualCursor)
    {
        // 如果当前焦点Index处于合并格中，并且原来的m_visualCursor不在合并格中
        // 直接将m_visualCursor设置为Current
        // 否则改变m_visualCursor
        if (d->hasSpans())
        {
            GSpanCollection::GSpan span = d->span(oCurIndex.row(), oCurIndex.column());
            if (span.top() > d->m_visualCursor.y() || d->m_visualCursor.y() > span.bottom()
                    || span.left() > d->m_visualCursor.x() || d->m_visualCursor.x() > span.right())
            {
                d->m_visualCursor = oVisualCurrent;
            }
        }
        else
        {
            d->m_visualCursor = oVisualCurrent;
        }
    }
}

void GLDTableView::adjustRightToLeftCursorAction(GlodonAbstractItemView::CursorAction cursorAction)
{
    if (isRightToLeft())
    {
        if (cursorAction == MoveLeft)
        {
            cursorAction = MoveRight;
        }
        else if (cursorAction == MoveRight)
        {
            cursorAction = MoveLeft;
        }
    }
}

void GLDTableView::dealWithMoveUp(int &nLastVisualRow, int &nLastVisualCol)
{
    Q_D(GLDTableView);

    int nOriginalRow = nLastVisualRow;

#ifdef QT_KEYPAD_NAVIGATION
    if (QApplication::keypadNavigationEnabled() && nLastVisualRow == 0)
    {
        nLastVisualRow = d->visualRow(model()->rowCount() - 1) + 1;
    }
    // FIXME? visualRow = bottom + 1;
#endif

    int nLogicalRow = d->logicalRow(nLastVisualRow);
    int nLogicalCol = d->logicalColumn(nLastVisualCol);
    if (nLogicalRow != -1 && d->hasSpans())
    {
        GSpanCollection::GSpan span = d->span(nLogicalRow, nLogicalCol);
        if (span.width() > 1 || span.height() > 1)
        {
            nLastVisualRow = d->visualRow(span.top());
        }
    }

    // 找到向上最近的可见的并且可选择的行
    while (nLastVisualRow >= 0)
    {
        --nLastVisualRow;
        nLogicalRow = d->logicalRow(nLastVisualRow);
        nLogicalCol = d->logicalColumn(nLastVisualCol);
        if (nLogicalRow == -1 || (!isRowHidden(nLogicalRow) && d->isCellEnabledAndSelectabled(nLogicalRow, nLogicalCol)))
        {
            break;
        }
    }

    if (nLastVisualRow < 0)
    {
        nLastVisualRow = nOriginalRow;
    }
}

void GLDTableView::dealWithMoveDown(int &nLastVisualRow, int &nLastVisualCol, int nBottom)
{
    Q_D(GLDTableView);

    int nOriginalRow = nLastVisualRow;
    QModelIndex oCurIndex = currentIndex();
    if (d->hasSpans())
    {
        GSpanCollection::GSpan span = d->span(oCurIndex.row(), oCurIndex.column());
        nLastVisualRow = d->visualRow(d->rowSpanEndLogical(span.top(), span.height()));
    }

#ifdef QT_KEYPAD_NAVIGATION
    if (QApplication::keypadNavigationEnabled() && nLastVisualRow >= nBottom)
    {
        nLastVisualRow = -1;
    }
#endif

    int nLogicalRow = d->logicalRow(nLastVisualRow);
    int nLogicalCol = d->logicalColumn(nLastVisualCol);
    if (nLogicalRow != -1 && d->hasSpans())
    {
        GSpanCollection::GSpan span = d->span(nLogicalRow, nLogicalCol);
        if (span.width() > 1 || span.height() > 1)
        {
            nLastVisualRow = d->visualRow(d->rowSpanEndLogical(span.top(), span.height()));
        }
    }

    // 找到向下的最近一个可见，并且可选中的行
    while (nLastVisualRow <= nBottom)
    {
        ++nLastVisualRow;
        nLogicalRow = d->logicalRow(nLastVisualRow);
        nLogicalCol = d->logicalColumn(nLastVisualCol);
        if (nLogicalRow == -1 || (!isRowHidden(nLogicalRow) && d->isCellEnabledAndSelectabled(nLogicalRow, nLogicalCol)))
        {
            break;
        }
    }

    if (nLastVisualRow > nBottom)
    {
        nLastVisualRow = nOriginalRow;
    }
}

QModelIndex GLDTableView::getLeftPageCursorIndex()
{
    Q_D(GLDTableView);
    QModelIndex oCurIndex = currentIndex();
    int nNewColumn = columnAt(visualRect(oCurIndex).right() - d->viewport->width());
    if (nNewColumn == -1)
    {
        nNewColumn = d->logicalRow(0);
    }
    return d->m_model->index(oCurIndex.row(), nNewColumn, d->m_root);
}

void GLDTableView::dealWithMoveLeft(int &nLastVisualRow, int &nLastVisualCol, int nRight, int nBottom, CursorAction cursorAction)
{
    Q_D(GLDTableView);

    int nOriginalRow = nLastVisualRow;
    int nOriginalColumn = nLastVisualCol;
    bool bFirstTime = true;
    bool bLooped = false;
    bool bWrapped = false;

    do
    {
        int nRow = d->logicalRow(nLastVisualRow);
        int nCol = d->logicalColumn(nLastVisualCol);
        if (bFirstTime && nCol != -1 && d->hasSpans())
        {
            bFirstTime = false;
            GSpanCollection::GSpan span = d->span(nRow, nCol);
            if (span.width() > 1 || span.height() > 1)
            {
                nLastVisualCol = d->visualColumn(span.left());
            }
        }

        while (nLastVisualCol >= 0)
        {
            --nLastVisualCol;
            nRow = d->logicalRow(nLastVisualRow);
            nCol = d->logicalColumn(nLastVisualCol);
            if (nRow == -1 || nCol == -1
                    || (!isRowHidden(nRow) && !isColumnHidden(nCol)
                        && d->isCellEnabledAndSelectabled(nRow, nCol)))
            {
                break;
            }

            if (bWrapped && (nOriginalRow < nLastVisualRow
                             || (nOriginalRow == nLastVisualRow && nOriginalColumn <= nLastVisualCol)))
            {
                bLooped = true;
                break;
            }
        }

        if (cursorAction == MoveLeft || nLastVisualCol >= 0)
        {
            break;
        }

        nLastVisualCol = nRight + 1;
        if (nLastVisualRow == 0)
        {
            bWrapped = true;
            nLastVisualRow = nBottom;
        }
        else
        {
            --nLastVisualRow;
        }
    } while (!bLooped);

    if (nLastVisualCol < 0)
    {
        nLastVisualCol = nOriginalColumn;
    }
}

QModelIndex GLDTableView::getRightPageCursorIndex(int nRight)
{
    Q_D(GLDTableView);
    QModelIndex oCurIndex = currentIndex();
    int nNewColumn = columnAt(visualRect(oCurIndex).left() + d->viewport->width());
    if (nNewColumn == -1)
    {
        nNewColumn = d->logicalRow(nRight);
    }
    return d->m_model->index(oCurIndex.row(), nNewColumn, d->m_root);
}

void GLDTableView::dealWithMoveRight(int &nLastVisualRow, int &nLastVisualCol, int nRight, int nBottom, GlodonAbstractItemView::CursorAction cursorAction)
{
    Q_D(GLDTableView);

    int nOriginalRow = nLastVisualRow;
    int nOriginalColumn = nLastVisualCol;
    bool bFirstTime = true;
    bool bLooped = false;
    bool bWrapped = false;
    do
    {
        int nRow = d->logicalRow(nLastVisualRow);
        int nCol = d->logicalColumn(nLastVisualCol);
        if (bFirstTime && nCol != -1 && d->hasSpans())
        {
            bFirstTime = false;
            GSpanCollection::GSpan span = d->span(nRow, nCol);
            if (span.width() > 1 || span.height() > 1)
            {
                nLastVisualCol = d->visualColumn(d->columnSpanEndLogical(span.left(), span.width()));
            }
        }

        while (nLastVisualCol <= nRight)
        {
            ++nLastVisualCol;
            nRow = d->logicalRow(nLastVisualRow);
            nCol = d->logicalColumn(nLastVisualCol);
            if (nRow == -1 || nCol == -1
                    || (!isRowHidden(nRow) && !isColumnHidden(nCol) && d->isCellEnabledAndSelectabled(nRow, nCol)))
            {
                break;
            }

            if (bWrapped && (nOriginalRow > nLastVisualRow
                        || (nOriginalRow == nLastVisualRow && nOriginalColumn >= nLastVisualCol))
                    && (!isRowHidden(nRow) && !isColumnHidden(nCol)
                        && d->isCellEnabledAndSelectabled(nRow, nCol)))
            {
                bLooped = true;
                break;
            }
        }

        if (cursorAction == MoveRight || nLastVisualCol <= nRight)
        {
            break;
        }

        nLastVisualCol = -1;
        if (nLastVisualRow == nBottom)
        {
            bWrapped = true;
            nLastVisualRow = 0;
        }
        else
        {
            ++nLastVisualRow;
        }
    } while (!bLooped);

    if (nLastVisualCol > nRight)
    {
        nLastVisualCol = nOriginalColumn;
    }
}

void GLDTableView::dealWithMoveHome(int &nLastVisualRow, int &nLastVisualCol, int nRight, int nBottom, Qt::KeyboardModifiers modifiers)
{
    Q_D(GLDTableView);

    nLastVisualCol = 0;
    while (nLastVisualCol < nRight
           && d->isVisualColumnHiddenOrDisabled(nLastVisualRow, nLastVisualCol))
    {
        ++nLastVisualCol;
    }

    if (modifiers & Qt::ControlModifier)
    {
        nLastVisualRow = 0;
        while (nLastVisualRow < nBottom
               && d->isVisualRowHiddenOrDisabled(nLastVisualRow, nLastVisualCol))
        {
            ++nLastVisualRow;
        }
    }
}

QModelIndex GLDTableView::getUpPageCursorIndex()
{
    Q_D(GLDTableView);
    QModelIndex oCurIndex = currentIndex();
    int nNewRow = rowAt(visualRect(oCurIndex).bottom() - d->viewport->height());
    if (nNewRow == -1)
    {
        nNewRow = d->logicalRow(0);
    }
    return d->m_model->index(nNewRow, oCurIndex.column(), d->m_root);
}

QModelIndex GLDTableView::getDownPageCursorIndex(int nBottom)
{
    Q_D(GLDTableView);
    QModelIndex oCurIndex = currentIndex();
    int nNewRow = rowAt(visualRect(oCurIndex).top() + d->viewport->height());
    if (nNewRow == -1)
    {
        nNewRow = d->logicalRow(nBottom);
    }
    return d->m_model->index(nNewRow, oCurIndex.column(), d->m_root);
}

void GLDTableView::dealWithMoveEnd(int &nLastVisualRow, int &nLastVisualCol, int nRight, int nBottom, Qt::KeyboardModifiers modifiers)
{
    nLastVisualCol = nRight;
    if (modifiers & Qt::ControlModifier)
    {
        nLastVisualRow = nBottom;
    }
}

QModelIndex GLDTableView::getCursorIndex(int nLastVisualRow, int nLastVisualCol)
{
    Q_D(GLDTableView);

    int nLogicalRow = d->logicalRow(nLastVisualRow);
    int nLogicalColumn = d->logicalColumn(nLastVisualCol);
    if (!d->m_model->hasIndex(nLogicalRow, nLogicalColumn, d->m_root))
    {
        return QModelIndex();
    }

    QModelIndex oResultIndex = d->m_model->index(nLastVisualRow, nLogicalColumn, d->m_root);
    if (!d->isRowHidden(nLastVisualRow) && !d->isColumnHidden(nLogicalColumn) && d->isIndexEnabled(oResultIndex))
    {
        return oResultIndex;
    }

    return QModelIndex();
}

bool GLDTableView::copyOrPastOperation(QKeyEvent *event)
{
#if !defined(QT_NO_CLIPBOARD) && !defined(QT_NO_SHORTCUT)
    Q_D(GLDTableView);
    if (event->matches(QKeySequence::Copy) && (d->m_allowCopy))
    {
        copyTextFromSelections();
        event->accept();
        return true;
    }
    else if (event->matches(QKeySequence::Paste) && (d->m_allowPaste))
    {
        pasteFromClipboard();
        event->accept();
        return true;
    }
#endif
    return false;
}

bool GLDTableView::openEditOrScrollContent(QKeyEvent *event)
{
    // alt + down，进入编辑状态，有下拉的弹出下拉
    if (event->modifiers() & Qt::AltModifier)
    {
        edit(currentIndex(), GlodonAbstractItemView::AnyKeyPressed, event);
        return true;
    }
    else
    {
        resetEnterJumpPreState();
        // ctrl + down，纵向滚动条向下滚动一行
        if (event->modifiers() & Qt::ControlModifier)
        {
            int nChangeValue = event->key() == Qt::Key_Down ? 1 : -1;
            verticalScrollBar()->setValue(verticalScrollBar()->value() + nChangeValue);
            return true;
        }
        return false;
    }
}

void GLDTableView::dealWithKeyEnterPress(QKeyEvent *event)
{
    Q_D(GLDTableView);
    QPersistentModelIndex oNewCurrent;
    // if control is pressed and just get focus(not in edit state) ,do nothing
    if (event->modifiers() & Qt::ControlModifier)
    {
        return;
    }

    CursorAction direction = MoveNext;
    bool bIndexValide = currentIndex().isValid();

    GlodonAbstractItemView::EditTrigger oEditTrigger = GlodonAbstractItemView::AllEditTriggers;
    if (editTriggers() == GlodonAbstractItemView::NoEditTriggers)
    {
        oEditTrigger = GlodonAbstractItemView::NoEditTriggers;
    }

    // 回车跳格没有开启时
    if (!d->m_bEnterJump)
    {
        if (bIndexValide)
        {
            // 说明不是从delegate中的edit传送过来的enter消息
            if (EditingState != state())
            {
                edit(currentIndex());
            }
        }
        return;
    }
    else if (d->isEnterJump())
    {
        if (d->m_prevState == GlodonAbstractItemView::LastFocus)
        {
            emit onPressEnter(direction);
            oNewCurrent = moveCursor(direction, event->modifiers());
            d->m_prevState = GlodonAbstractItemView::FirstFocus;
        }
        else if (d->m_prevState == GlodonAbstractItemView::FirstFocus)
        {
            if (bIndexValide)
            {
                if (!edit(currentIndex(), oEditTrigger, event))
                {
                    //未能进入编辑状态
                    emit onPressEnter(direction);
                    oNewCurrent = moveCursor(direction, event->modifiers());
                }
            }
        }
    }
    else if (d->isEnterJumpPro())
    {
        bool bIsFromClose = false;

        // LastFocus只会在CloseEditor的时候设置
        // 因此这里通过LastFocus，判断是否是通过关闭编辑状态执行到此处的
        // 如果是，则在下一个格子不能进入编辑状态时，不再移动焦点
        if (d->m_prevState == GlodonAbstractItemView::LastFocus)
        {
            bIsFromClose = true;
            d->m_prevState = GlodonAbstractItemView::FirstFocus;
        }

        if (d->m_prevState == GlodonAbstractItemView::FirstFocus)
        {
            if (bIndexValide && !edit(currentIndex(), oEditTrigger, event))
            {
                // 未能进入编辑状态，则不改变焦点
                if (bIsFromClose)
                {
                    event->accept();
                }
                else
                {
                    emit onPressEnter(direction);
                    oNewCurrent = moveCursor(direction, event->modifiers());
                    GlodonAbstractItemView::setCurrentIndex(oNewCurrent);
                    edit(currentIndex(), oEditTrigger, event);
                }
                return;
            }
        }
    }
    setNewCurrentSelection(event, oNewCurrent);
}

void GLDTableView::clickEditorButtonAccordingToEditStyleDrawType(QModelIndex index, QMouseEvent *event)
{
    // 如果当前格子是处于非编辑状态显示编辑方式（是指并没有进入编辑状态，但是将编辑方式的按钮画上）
    // 只要显示编辑方式，再次单击编辑方式区域时，应该进入编辑状态，并且触发对应的编辑方式
    // 因此这里只需要触发编辑方式，而不需要处理SelectionModel
    if (!shouldDoEditorDraw(index))
    {
        return;
    }

    GlodonDefaultItemDelegate *pDelegate = itemDelegate();

    if (pDelegate->isInSubControlRect(index, event->pos()))
    {
        edit(index, DoubleClicked, event);
        pDelegate->clickSubControl(index);
    }
}

void GLDTableView::processEnterJumpAfterCloseEdit(GlodonDefaultItemDelegate::EndEditHint hint)
{
    Q_D(GLDTableView);
    bool bFlagForTabMove = false;
    switch (hint)
    {
    case GlodonDefaultItemDelegate::EditNextItem:
    {
        if (currentIndex().isValid())
        {
            if ((currentIndex().flags() & Qt::ItemIsEditable) == Qt::ItemIsEditable
                    && (!(editTriggers() & GlodonAbstractItemView::CurrentChanged)))
            {
                bFlagForTabMove = true;
            }
        }

        break;
    }

    case GlodonDefaultItemDelegate::EditPreviousItem:
    {
        if (currentIndex().isValid())
        {
            if ((currentIndex().flags() & Qt::ItemIsEditable) == Qt::ItemIsEditable
                    && (!(editTriggers() & GlodonAbstractItemView::CurrentChanged)))
            {
                bFlagForTabMove = true;
            }
        }

        break;
    }
    case GlodonDefaultItemDelegate::RevertModelCache:
    {
        //如果是回车跳格状态，按ESC键时，要把状态置为FirstFocus，否则会不能进入编辑状态，直接跳到下一格
        resetEnterJumpPreState();
        break;
    }
    }

    if (d->isEnterJump())
    {
        if (bFlagForTabMove)
        {
            d->m_prevState = GlodonAbstractItemView::FirstFocus;
        }
        else
        {
            d->m_prevState = GlodonAbstractItemView::LastFocus;
        }
    }
    else if (d->isEnterJumpPro() && hint != GlodonDefaultItemDelegate::RevertModelCache)
    {
        QEvent *pEvent = d->m_itemDelegate.data()->m_closeEditEvent;
        if (pEvent == NULL || pEvent->type() != QEvent::KeyPress)
        {
            resetEnterJumpPreState();
            return;
        }

        QKeyEvent *pKeyEvent = static_cast<QKeyEvent *>(pEvent);
        if (pKeyEvent->key() == Qt::Key_Enter || pKeyEvent->key() == Qt::Key_Return)
        {
            CursorAction direction = MoveNext;
            emit onPressEnter(direction);
            d->m_prevState = GlodonAbstractItemView::LastFocus;
            QModelIndex index = moveCursor(direction, Qt::NoModifier);

            if (index.isValid())
            {
                QPersistentModelIndex persistent(index);
                GlodonAbstractItemView::setCurrentIndex(persistent);
            }
        }
    }
}

void GLDTableView::resetEnterJumpPreState()
{
    Q_D(GLDTableView);
    if (d->m_bEnterJump)
    {
        d->m_prevState = FirstFocus;
    }
}

bool GLDTableView::shouldDoEditorDraw(const QModelIndex &testIndex) const
{
    Q_D(const GLDTableView);

    if (!testIndex.isValid())
    {
        return false;
    }

    bool bResult = false;

    switch (d->m_editStyleDrawType)
    {
    case GlodonAbstractItemView::SdtCurrentCell:
        if (currentIndex().isValid() && testIndex == currentIndex())
        {
            bResult = true;
        }
        break;
    case GlodonAbstractItemView::SdtCurrentRow:
        if (currentIndex().isValid() && testIndex.row() == currentIndex().row())
        {
            bResult = true;
        }
        break;
    case GlodonAbstractItemView::SdtCurrentCol:
        if (currentIndex().isValid() && testIndex.column() == currentIndex().column())
        {
            bResult = true;
        }
        break;
    case GlodonAbstractItemView::SdtAll:
        bResult = true;
        break;
    default:
        bResult = false;
        break;
    }

    return bResult;
}

int GLDTableView::firstVisualIndexAfterHorizontalScroll(const QModelIndex &index, GlodonAbstractItemView::ScrollHint &hint, int nCellWidth)
{
    Q_D(GLDTableView);

    int nHorizontalIndex = d->m_horizontalHeader->visualIndex(index.column());
    int nViewportWidth = d->viewport->width();
    int nHorizontalOffset = d->m_horizontalHeader->offset();
    int nHorizontalPosition = d->m_horizontalHeader->sectionPosition(index.column());

    bool bPositionAtRight = (nHorizontalPosition - nHorizontalOffset + nCellWidth > nViewportWidth);
    if (hint == PositionAtCenter || bPositionAtRight)
    {
        int nW = (hint == PositionAtCenter) ? nViewportWidth / 2 : nViewportWidth;
        int nX = nCellWidth + currentGridLineWidth() + d->m_nFixedColWidth;

        while (nHorizontalIndex > 0)
        {
            if (d->m_oSuitColWidthCols.contains(nHorizontalIndex - 1))
            {
                nX += d->calcSuitWidth(nHorizontalIndex - 1) + currentGridLineWidth();
            }
            else
            {
                nX += columnWidth(d->m_horizontalHeader->logicalIndex(nHorizontalIndex - 1)) + currentGridLineWidth();
            }

            // 当nX>nW时，说明当前的列如果被显示的话，需要滚动到的列就显示不全了
            // 因此应该多滚动一个格子，让需要滚动到的格子显示完整
            if (nX > nW)
            {
                break;
            }
            nHorizontalIndex--;
        }
    }

    return nHorizontalIndex;
}

int GLDTableView::getHorizontalHiddenSectionCountBeforeIndex(int nHorizontalIndex)
{
    Q_D(GLDTableView);

    int nHiddenSections = 0;
    if (d->m_horizontalHeader->sectionsHidden())
    {
        for (int nHiter = nHorizontalIndex - 1; nHiter >= 0; --nHiter)
        {
            int nColumn = d->m_horizontalHeader->logicalIndex(nHiter);

            if (d->m_horizontalHeader->isSectionHidden(nColumn))
            {
                ++nHiddenSections;
            }
        }
    }

    return nHiddenSections;
}

bool GLDTableView::isPositionAtTopForRowExchange(int nVerticalPosition, int nVerticalOffset)
{
    Q_D(GLDTableView);

    bool bPositionAtTopForRowExchange = (0 == nVerticalPosition - nVerticalOffset);

    if (bPositionAtTopForRowExchange)
    {
        bool bHeaderIndicatorExist = d->m_horizontalHeader
                && d->m_horizontalHeader->d_func()
                && d->m_horizontalHeader->d_func()->sectionIndicator;
        if (bHeaderIndicatorExist)
        {
            bPositionAtTopForRowExchange = !d->m_horizontalHeader->d_func()->sectionIndicator->isHidden();
        }
    }

    return bPositionAtTopForRowExchange;
}

int GLDTableView::firstVisualIndexAfterVerticalScroll(const QModelIndex &index, GlodonAbstractItemView::ScrollHint &hint, int nCellHeight)
{
    Q_D(GLDTableView);

    int nVerticalIndex = d->m_verticalHeader->visualIndex(index.row());
    int nViewportHeight = d->viewport->height();

    if (hint == PositionAtCenter || hint == PositionAtBottom)
    {
        int nH = (hint == PositionAtCenter ? nViewportHeight / 2 : nViewportHeight);
        int nY = nCellHeight + currentGridLineWidth() + d->m_nFixedRowHeight;

        QList<int> oNeedCalcHeightCols = needCalcSuitRowHeightColsInViewport();

        while (nVerticalIndex > 0)
        {
            if (oNeedCalcHeightCols.count() == 0)
            {
                nY += rowHeight(d->m_verticalHeader->logicalIndex(nVerticalIndex - 1)) + currentGridLineWidth();
            }
            else
            {
                nY += d->calcSuitHeight(oNeedCalcHeightCols, nVerticalIndex - 1) + currentGridLineWidth();
            }

            // 当nY>nW时，说明当前的列如果被显示的话，需要滚动到的列就显示不全了
            // 因此应该多滚动一个格子，让需要滚动到的格子显示完整
            if (nY > nH)
            {
                break;
            }
            nVerticalIndex--;
        }
    }

    return nVerticalIndex;
}

int GLDTableView::getVerticalHiddenSectionCountBeforeIndex(int nVerticalIndex)
{
    Q_D(GLDTableView);

    int nHiddenSections = 0;
    if (d->m_verticalHeader->sectionsHidden())
    {
        for (int nSIndex = nVerticalIndex - 1; nSIndex >= 0; --nSIndex)
        {
            int row = d->m_verticalHeader->logicalIndex(nSIndex);
            if (d->m_verticalHeader->isSectionHidden(row))
            {
                ++nHiddenSections;
            }
        }
    }

    return nHiddenSections;
}

void GLDTableView::setSpan(int row, int column, int rowSpan, int columnSpan)
{
    Q_D(GLDTableView);

    if (row < 0 || column < 0 || rowSpan < 0 || columnSpan < 0)
    {
        return;
    }

    d->setSpan(row, column, rowSpan, columnSpan);
    d->viewport->update();
}

void GLDTableView::clearSpans()
{
    Q_D(GLDTableView);
    d->m_spans.clear();
    d->viewport->update();
}

void GLDTableView::processScrollBarAfterCanNotCloseEditor()
{
    Q_D(GLDTableView);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonRelease, QPointF(),
                                         Qt::LeftButton, 0, 0);
#else
    QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonRelease, QPoint(),
                                         Qt::LeftButton, 0, 0);
#endif
    QCoreApplication::postEvent(d->vbar, event);
    d->vbar->setEnabled(false);
    d->hbar->setEnabled(false);
    GLDEvent *pScrollBarStep = new GLDEvent(GM_SetScrollBarStep, (int)gsbtAll, 1);
    QCoreApplication::postEvent(this, pScrollBarStep);
}

void GLDTableView::scrollHorizontalContentsBy(int dx)
{
    Q_D(GLDTableView);

    dx = isRightToLeft() ? -dx : dx;

    if (0 != dx)
    {
        if (horizontalScrollMode() == GlodonAbstractItemView::ScrollPerItem)
        {
            int nOldOffset = d->m_horizontalHeader->offset();
//            d->m_horizontalHeader->d_func()->setScrollOffset(horizontalScrollBar(), horizontalScrollMode());
            d->m_horizontalHeader->setOffsetToSectionPosition(horizontalScrollBar()->value());

            int nNewOffset = d->m_horizontalHeader->offset();
            dx = isRightToLeft() ? nNewOffset - nOldOffset : nOldOffset - nNewOffset;
        }
        else
        {
            d->m_horizontalHeader->setOffset(horizontalScrollBar()->value());
        }

        if (isEditorInFixedColAfterHScroll())
        {
            return;
        }

        d->scrollContentsBy(dx, 0);
    }
}

void GLDTableView::scrollVerticalContentsBy(int dy)
{
    Q_D(GLDTableView);

    if (0 != dy)
    {
        if (verticalScrollMode() == GlodonAbstractItemView::ScrollPerItem)
        {
            int nOldOffset = d->m_verticalHeader->offset();
//            d->m_verticalHeader->d_func()->setScrollOffset(verticalScrollBar(), verticalScrollMode());
            d->m_verticalHeader->setOffsetToSectionPosition(verticalScrollBar()->value());

            int nNewOffset = d->m_verticalHeader->offset();
            dy = nOldOffset - nNewOffset;
        }
        else
        {
            d->m_verticalHeader->setOffset(verticalScrollBar()->value());
        }

        if (isEditorInFixedRowAfterVScroll())
        {
            return;
        }

        d->scrollContentsBy(0, dy);
    }
}

void GLDTableView::updateFirstLine(int dx, int dy)
{
    Q_D(GLDTableView);

    if (d->m_showGrid)
    {
        //we need to update the first line of the previous top item in the view
        //because it has the grid drawn if the header is invisible.
        //It is strictly related to what's done at then end of the paintEvent
        if (dy > 0 && d->m_horizontalHeader->isHidden() && d->m_verticalScrollMode == ScrollPerItem)
        {
            d->viewport->update(0, dy, d->viewport->width(), dy);
        }

        if (dx > 0 && d->m_verticalHeader->isHidden() && d->m_horizontalScrollMode == ScrollPerItem)
        {
            d->viewport->update(dx, 0, dx, d->viewport->height());
        }
    }
}

bool GLDTableView::commitDataAndCloseEditorInIngnoreFocusOut()
{
    if (!closeEditorOnFocusOut())
    {
        return commitDataAndCloseEditor();
    }

    return true;
}

void GLDTableView::dealWithSpecialMousePressOperations(const QModelIndex &index, QMouseEvent *event)
{
    Q_D(GLDTableView);

    // 处理拖拉复制
    if (shouldDoRangeFill(event))
    {
        initRangeAction();
        d->m_state = RangeFillingState;
    }
    // 处理超链接单元格操作
    else if (inLinkCell(event->pos()))
    {
        QString strvalue = index.data().toString();
        QUrl url(strvalue);
        QDesktopServices::openUrl(url);
    }
    // 处理框选拖拽
    else if (shouldDoRangeMove(event))
    {
        initRangeAction();
        d->m_state = RangeMovingState;
    }
    // 处理拖拽格线改变行高列宽
    else if (shoudDoResizeCellByDragGridLine(event))
    {
        dealWithResizeCellWhenPressGridLine(event);
    }
    // 处理Bool单元格操作
    else if (inBoolCell(event->pos()))
    {
        onBoolCellPress(event);
    }
}

void GLDTableView::dealWithResizeCellWhenPressGridLine(QMouseEvent *event)
{
    Q_D(GLDTableView);

    d->m_pressedPosition = event->pos();
    d->m_state = DragResizingState;

    if (cursor().shape() == Qt::SplitHCursor)
    {
        d->m_horizontalHeader->setMousePosition(event->pos());
        showSectionResizingInfoFrame(event->screenPos().toPoint(), Qt::Horizontal, GlodonHeaderView::Press);
    }
    else if (cursor().shape() == Qt::SplitVCursor)
    {
        d->m_verticalHeader->setMousePosition(event->pos());
        showSectionResizingInfoFrame(event->screenPos().toPoint(), Qt::Vertical, GlodonHeaderView::Press);
    }
}

bool GLDTableView::shouldDoRangeFill(QMouseEvent *event)
{
    Q_D(GLDTableView);
    return d->m_state == NoState
            && doCanRangeFill(visualRectForSelection())
            && d->isRangeFillingHandleSelected(event->pos());
}

bool GLDTableView::shouldDoRangeMove(QMouseEvent *event)
{
    Q_D(GLDTableView);
    return d->m_state == NoState
            && d->m_bRangeMoving
            && inRangeMovingRegion(event->pos());
}

bool GLDTableView::shoudDoResizeCellByDragGridLine(QMouseEvent *event)
{
    Q_D(GLDTableView);
    return d->m_allowToResizeCellByDragGridLine
            && event->button() == Qt::LeftButton
            && (cursor().shape() == Qt::SplitHCursor || cursor().shape() == Qt::SplitVCursor);
}

bool GLDTableView::shouldSetRangeFillMouseShape(const QPoint &pos)
{
    Q_D(GLDTableView);
    // 非编辑状态、非多选状态，鼠标移动到拖拉复制的黑点时，设置鼠标形状
    return d->m_state != EditingState
            && !d->isInMutiSelect()
            && doCanRangeFill(visualRectForSelection())
            && d->isRangeFillingHandleSelected(pos);
}

void GLDTableView::setRangeFillMouseShape(bool bHasCursor)
{
    // 鼠标没有设置过形状，或者设置过调整大小的形状时，将其设置为拖拉复制形状
    if (!bHasCursor || cursor().shape() == Qt::SplitHCursor || cursor().shape() == Qt::SplitVCursor)
    {
        QPixmap cursorPixmap(":/icons/rangeFillingCursor");

        if (!cursorPixmap.isNull())
        {
            setCursor(QCursor(cursorPixmap));
        }
        else
        {
            setCursor(Qt::CrossCursor);
        }
    }
}

void GLDTableView::getRangeFillingCurRowAndCol(int nOldRow, int nOldColumn, const QPoint &pos)
{
    Q_D(GLDTableView);

    setCurRangeRowAndCol(pos);

    switch (d->m_rangeFillingStyle) {
    case rfsVertical:
    {
        if (m_nRangeCurCol != nOldColumn)
        {
            m_nRangeCurCol = nOldColumn;
        }
        break;
    }
    case rfsHorizontal:
    {
        if (m_nRangeCurRow != nOldRow)
        {
            m_nRangeCurRow = nOldRow;
        }
        break;
    }
    default:
        break;
    }
}

bool GLDTableView::shouldSetRangeMoveMouseShape(const QPoint &pos)
{
    Q_D(GLDTableView);
    // 如果处于非编辑状态，并且可以框选拖拽
    return d->m_state != EditingState
            && d->m_bRangeMoving
            && inRangeMovingRegion(pos);
}

void GLDTableView::setRangeMoveMouseShape(bool bHasCursor)
{
    if (!bHasCursor)
    {
        QPixmap cursorPixmap(":/icons/rangeMoveCursor");

        if (!cursorPixmap.isNull())
        {
            setCursor(QCursor(cursorPixmap));
        }
        else
        {
            setCursor(Qt::SizeAllCursor);
        }
    }
}

void GLDTableView::updateTempRangeRect()
{
    Q_D(GLDTableView);

    if (m_nRangeCurRow != -1 && m_nRangeCurCol != -1)
    {
        m_preTempRangeRect = m_tempRangeRect;
        m_tempRangeRect = calcRangeMovingRegion();
        QRect updateRect = m_tempRangeRect.united(m_preTempRangeRect);
        updateRect.adjust(-3, -3, 3, 3);

        d->viewport->update(updateRect);
    }
}

bool GLDTableView::shouldSetResizeCellByDragGridLineMouseShape(QMouseEvent *event)
{
    Q_D(GLDTableView);
    // 如果没有点击鼠标键，如果是在格线附近，要改变鼠标形状为可拖拽
    return d->m_state != EditingState
            && d->m_allowToResizeCellByDragGridLine
            && event->buttons() == Qt::NoButton
            && (d->isNearHorizontalGridLine(event) || d->isNearVerticalGridLine(event));
}

void GLDTableView::setResizeCellByDragGridLineMouseShape(QMouseEvent *event)
{
    Q_D(GLDTableView);

    bool bInTableView = d->isInTableView(event);

    if (d->isNearHorizontalGridLine(event) && bInTableView)
    {
        setCursor(Qt::SplitHCursor);
    }
    else if (d->isNearVerticalGridLine(event) && bInTableView)
    {
        setCursor(Qt::SplitVCursor);
    }

    if (!d->viewport->rect().contains(event->pos()) || !bInTableView)
    {
        unsetCursor();
    }
}

void GLDTableView::dealWithResizeCellWhenDragGridLine(QMouseEvent *event)
{
    Q_D(GLDTableView);

    if (cursor().shape() == Qt::SplitHCursor)
    {
        d->m_horizontalHeader->setMousePosition(event->pos());
        showSectionResizingInfoFrame(event->windowPos().toPoint(), Qt::Horizontal, GlodonHeaderView::Move);
    }
    else if (cursor().shape() == Qt::SplitVCursor)
    {
        d->m_verticalHeader->setMousePosition(event->pos());
        showSectionResizingInfoFrame(event->windowPos().toPoint(), Qt::Vertical, GlodonHeaderView::Move);
    }
}

void GLDTableView::recoverCursor()
{
    unsetCursor();
    // If no cursor has been set, or after a call to unsetCursor(), the parent's cursor is used
    QWidget *widget = dynamic_cast<QWidget * >(this->parent());

    if (widget->cursor().shape() != Qt::ArrowCursor)
    {
        setCursor(Qt::ArrowCursor);
    }
}

void GLDTableView::dealWithCurCommet(const QPoint &pos)
{
    Q_D(GLDTableView);

    QPersistentModelIndex oCurIndex = indexAt(pos);
    bool showComment = true;
    emit canShowComment(oCurIndex, showComment);

    if (showComment)
    {
        d->drawComment(oCurIndex);
    }
}

void GLDTableView::dealWithRangeFillingOnMouseMove(bool bHasCursor, const QPoint &pos)
{
    doAutoScroll();

    if (!bHasCursor)
    {
        setCursor(Qt::CrossCursor);
    }

    int nOldRow = m_nRangeCurRow;
    int nOldColumn = m_nRangeCurCol;
    getRangeFillingCurRowAndCol(nOldRow, nOldColumn, pos);
    updateRangeFillingRect(nOldRow, nOldColumn);
}

void GLDTableView::dealWithRangeMovingOnMouseMove(const QPoint &pos)
{
    //选择移动时，先计算出移动的区域，再刷新计算出的区域和之前的区域
    doAutoScroll();
    setCurRangeRowAndCol(pos);
    updateTempRangeRect();
}

bool GLDTableView::isInRangeFilling()
{
    Q_D(GLDTableView);
    // 如果正在拖拉复制，且不是多选状态
    return d->m_state == RangeFillingState && !d->isInMutiSelect();
}

bool GLDTableView::isInRangeMoving()
{
    Q_D(GLDTableView);
    // 如果处于框选拖拽状态，并且不是在多选
    return d->m_state == RangeMovingState && !d->isInMutiSelect();
}

bool GLDTableView::isInResizingCellByDragGridLine(QMouseEvent *event)
{
    Q_D(GLDTableView);
    // 如果是拖拽
    return d->m_allowToResizeCellByDragGridLine
            && d->m_state == DragResizingState
            && event->buttons() == Qt::LeftButton;
}

void GLDTableView::updateRangeFillingRect(int nOldRow, int nOldColumn)
{
    Q_D(GLDTableView);

    if (m_nRangeCurRow == -1 || m_nRangeCurCol == -1)
    {
        return;
    }
    if (m_nRangeCurRow == nOldRow && m_nRangeCurCol == nOldColumn)
    {
        return;
    }

    QRect rect = m_oRangeSrc;
    rect.setTop(qMin(m_nRangeCurRow, qMin(nOldRow, m_oRangeSrc.top())));
    rect.setBottom(qMax(m_nRangeCurRow, qMax(nOldRow, m_oRangeSrc.bottom())));
    rect.setLeft(qMin(m_nRangeCurCol, qMin(nOldColumn, m_oRangeSrc.left())));
    rect.setRight(qMax(m_nRangeCurCol, qMax(nOldColumn, m_oRangeSrc.right())));

    d->viewport->update(visualRectForRowColNo(rect).adjusted(-3, -3, 3, 3));
}

void GLDTableView::completeRangeFilling()
{
    initRangeFillingDest();

    try
    {
        doRangeFill();
    }
    catch (...)
    {
        initRangeValuesAndUpdateViewport();
        throw;
    }

    QRect oSelectionRect = m_oRangeSrc.united(m_oRangeDest);
    setSelectionByIndex(topLeftIndex(oSelectionRect),
                        bottomRightIndex(oSelectionRect),
                        QItemSelectionModel::SelectCurrent);
    initRangeValuesAndUpdateViewport();
}

void GLDTableView::completeRangeMoving()
{
    Q_D(GLDTableView);

    if (initRangeMovingDest())
    {
        doRangeMoving(m_oRangeSrc, m_oRangeDest);
    }
    else
    {
        m_oRangeDest = m_oRangeSrc;
    }

    setSelectionByIndex(topLeftIndex(m_oRangeDest),
                        bottomRightIndex(m_oRangeDest), QItemSelectionModel::SelectCurrent);
    d->viewport->update();
    d->m_state = NoState;
}

void GLDTableView::completeResizeCellByDragGridLine(QMouseEvent *event)
{
    Q_D(GLDTableView);

    if (cursor().shape() == Qt::SplitHCursor)
    {
        int nColumn = columnAt(d->m_pressedPosition.x() - c_nResizeHandlerOffset);
        int nColWidth = event->pos().x() - d->m_pressedPosition.x() + columnWidth(nColumn);
        int nMinColWidth = d->m_horizontalHeader->minimumSectionSize();

        if (nColWidth < nMinColWidth)
        {
            nColWidth = nMinColWidth;
        }

        d->m_horizontalHeader->resizeSection(nColumn, nColWidth, true, true);
        showSectionResizingInfoFrame(event->pos(), Qt::Horizontal, GlodonHeaderView::Finish);
    }
    else if (cursor().shape() == Qt::SplitVCursor)
    {
        int nRow = rowAt(d->m_pressedPosition.y() - c_nResizeHandlerOffset);
        int nRowHeight = event->pos().y() - d->m_pressedPosition.y() + rowHeight(nRow);
        int nMinRowHeight = d->m_verticalHeader->minimumSectionSize();

        if (nRowHeight < nMinRowHeight)
        {
            nRowHeight = nMinRowHeight;
        }

        d->m_verticalHeader->resizeSection(nRow, nRowHeight, true, true);
        showSectionResizingInfoFrame(event->pos(), Qt::Vertical, GlodonHeaderView::Finish);
    }

    unsetCursor();
    d->m_state = NoState;
}

void GLDTableView::initRangeValuesAndUpdateViewport()
{
    Q_D(GLDTableView);

    m_oRangeSrc = QRect();
    m_nRangeCurRow = -1;
    m_nRangeCurCol = -1;
    d->m_state = NoState;
    d->viewport->update();
}

bool GLDTableView::doRangeFillHandled()
{
    bool bHandled = false;
    emit rangeFill(m_oRangeSrc, m_oRangeDest, bHandled);

    int srcRowCount = -1;
    int srcColumnCount = -1;
    int destRowCount = -1;
    int destColumnCount = -1;
    QModelIndexList srcModelIndexList = indexesFromRect(m_oRangeSrc, srcRowCount, srcColumnCount);
    QModelIndexList destModelIndexList = indexesFromRect(m_oRangeDest, destRowCount, destColumnCount);
    emit rangeFill(srcModelIndexList, destModelIndexList, srcRowCount, destRowCount, bHandled);

    if (bHandled)
    {
        return true;
    }

    return false;
}

void GLDTableView::doRangeFillToTop()
{
    Q_D(GLDTableView);

    for (int nDestRow = m_oRangeSrc.top() - 1; nDestRow >= m_nRangeCurRow; --nDestRow)
    {
        for (int nDestCol = m_oRangeSrc.left(); nDestCol <= m_oRangeSrc.right(); ++nDestCol)
        {
            QModelIndex oDestLogicalIndex = logicalIndex(d->m_model->index(nDestRow, nDestCol));

            int nSrcRow = m_oRangeSrc.top() + (m_oRangeSrc.top() - nDestRow) % m_oRangeSrc.height();
            QModelIndex oSrcLogicalIndex = logicalIndex(d->m_model->index(nSrcRow, nDestCol));

            setSrcIndexDataToDestIndex(oSrcLogicalIndex, oDestLogicalIndex);
        }
    }
}

void GLDTableView::doRangeFillToBottom()
{
    Q_D(GLDTableView);

    for (int nDestRow = m_oRangeSrc.bottom() + 1; nDestRow <= m_nRangeCurRow; ++nDestRow)
    {
        for (int nDestCol = m_oRangeSrc.left(); nDestCol <= m_oRangeSrc.right(); ++nDestCol)
        {
            QModelIndex oDestLogicalIndex = logicalIndex(d->m_model->index(nDestRow, nDestCol));

            int nSrcRow = m_oRangeSrc.top() + (nDestRow - m_oRangeSrc.bottom() - 1) % m_oRangeSrc.height();
            QModelIndex oSrcLogicalIndex = logicalIndex(d->m_model->index(nSrcRow, nDestCol));

            setSrcIndexDataToDestIndex(oSrcLogicalIndex, oDestLogicalIndex);
        }
    }
}

void GLDTableView::doRangeFillToLeft()
{
    Q_D(GLDTableView);

    for (int nDestCol = m_oRangeSrc.right() - 1; nDestCol >= m_nRangeCurCol; --nDestCol)
    {
        for (int nDestRow = m_oRangeSrc.top(); nDestRow <= m_oRangeSrc.bottom(); ++nDestRow)
        {
            QModelIndex oDestLogicalIndex = logicalIndex(d->m_model->index(nDestRow, nDestCol));

            int nSrcColumn = m_oRangeSrc.left() + (m_oRangeSrc.left() - nDestCol) % m_oRangeSrc.width();
            QModelIndex oSrcLogicalIndex = logicalIndex(d->m_model->index(nDestRow, nSrcColumn));

            setSrcIndexDataToDestIndex(oSrcLogicalIndex, oDestLogicalIndex);
        }
    }
}

void GLDTableView::doRangeFillToRight()
{
    Q_D(GLDTableView);

    for (int nDestCol = m_oRangeSrc.right() + 1; nDestCol <= m_nRangeCurCol; ++nDestCol)
    {
        for (int nDestRow = m_oRangeSrc.top(); nDestRow <= m_oRangeSrc.bottom(); ++nDestRow)
        {
            QModelIndex oDestLogicalIndex = logicalIndex(d->m_model->index(nDestRow, nDestCol));

            int nSrcColumn = m_oRangeSrc.left() + (nDestCol - m_oRangeSrc.right() - 1) % m_oRangeSrc.width();
            QModelIndex oSrcLogicalIndex = logicalIndex(d->m_model->index(nDestRow, nSrcColumn));

            setSrcIndexDataToDestIndex(oSrcLogicalIndex, oDestLogicalIndex);
        }
    }
}

void GLDTableView::setSrcIndexDataToDestIndex(const QModelIndex &oSrcLogicalIndex, QModelIndex &oDestLogicalIndex)
{
    Q_D(GLDTableView);

    QRect rect(oDestLogicalIndex.column(), oDestLogicalIndex.row(), 1, 1);

    if(!canRangeFill(rect))
    {
        return;
    }
    
    if (d->m_cellFillEditField)
    {
        d->m_model->setData(oDestLogicalIndex, d->m_model->data(oSrcLogicalIndex, Qt::EditRole));
    }
    else
    {
        d->m_model->setData(oDestLogicalIndex, d->m_model->data(oSrcLogicalIndex));
    }
}

void GLDTableView::initSectionResizingInfoFrame(const QPoint &mousePostion, Qt::Orientation direction)
{
    Q_D(GLDTableView);

    if (direction == Qt::Vertical)
    {
        //此时的mousePosition是鼠标在屏幕中的绝对位置
        d->m_infoFrame->move(mousePostion);
        setResizeStartPosition(Qt::Vertical);
        d->m_infoFrame->setText(resizeInfoText(QPoint(mousePostion.x(), d->m_nResizeCellEndPosition), Qt::Vertical));
    }
    else
    {
        d->m_infoFrame->move(mousePostion);
        setResizeStartPosition(Qt::Horizontal);
        d->m_infoFrame->setText(resizeInfoText(QPoint(d->m_nResizeCellEndPosition, mousePostion.y()), Qt::Horizontal));

    }
}

void GLDTableView::updateSectionResizingInfoFrameText(Qt::Orientation direction)
{
    Q_D(GLDTableView);

    if (direction == Qt::Vertical)
    {
        d->m_infoFrame->setText(resizeInfoText(d->m_verticalHeader->mousePosition(), Qt::Vertical));
    }
    else
    {
        d->m_infoFrame->setText(resizeInfoText(d->m_horizontalHeader->mousePosition(), Qt::Horizontal));
    }
}

void GLDTableView::adjustVisualRect(int &nFirstVisualRow, int &nFirstVisualCol, int &nLastVisualRow, int &nLastVisualCol)
{
    adjustVisualRow(nFirstVisualRow, nLastVisualRow);
    adjustVisualCol(nFirstVisualCol, nLastVisualCol);
}

void GLDTableView::adjustVisualRow(int &nFirstVisualRow, int &nLastVisualRow) const
{
    Q_D(const GLDTableView);

    if (nFirstVisualRow > 0)
    {
        nFirstVisualRow--;
    }

    if (nLastVisualRow < d->m_verticalHeader->count() - 1)
    {
        nLastVisualRow++;
    }
}

void GLDTableView::adjustVisualCol(int &nFirstVisualCol, int &nLastVisualCol) const
{
    Q_D(const GLDTableView);

    if (nFirstVisualCol > 0)
    {
        nFirstVisualCol--;
    }

    if (nLastVisualCol < d->m_horizontalHeader->count() - 1)
    {
        nLastVisualCol++;
    }
}

void GLDTableView::setMergeCellState(int nRow, int nSpanRow, int nCol, int nSpanCol, QVector<QBitArray> &oCellMergeState)
{
    for (int nSpanMergedRow = nRow; nSpanMergedRow <= nSpanRow; ++nSpanMergedRow)
    {
        for (int nSpanMergedCol = nCol; nSpanMergedCol <= nSpanCol; ++nSpanMergedCol)
        {
            oCellMergeState[nSpanMergedRow].setBit(nSpanMergedCol, true);
        }
    }
}

void GLDTableView::initMergeCellState(
        const int nFirstVisualRow, const int nLastVisualRow, const int nFirstVisualCol,
        const int nLastVisualCol, QVector<QBitArray> &oCellMergeState)
{
    for (int i = nFirstVisualRow; i <= nLastVisualRow; ++i)
    {
        QBitArray row;
        row.resize(nLastVisualCol - nFirstVisualCol + 1);
        row.fill(false);
        oCellMergeState.append(row);
    }
}

void GLDTableView::calcAndSetSpan(
        const int nFirstVisualRow, const int nLastVisualRow, const int nFirstVisualCol, const int nLastVisualCol)
{
    Q_D(GLDTableView);

    if ((nFirstVisualRow > nLastVisualRow) || (nFirstVisualCol > nLastVisualCol))
        return;

    QVector<QBitArray> oCellMergeState;
    initMergeCellState(nFirstVisualRow, nLastVisualRow, nFirstVisualCol, nLastVisualCol, oCellMergeState);

    QAbstractItemModel *pDataModel = itemModel();

    for (int nRow = nFirstVisualRow; nRow <= nLastVisualRow; ++nRow)
    {
        int nCurVisualRow = nRow;
        int nCurLogicalRow = d->logicalRow(nCurVisualRow);

        int nCol = nFirstVisualCol;
        while (nCol <= nLastVisualCol)
        {
            int nCurVisualCol = nCol;
            int nCurLogicalCol = d->logicalColumn(nCurVisualCol);

            // 如果当前格子被标识为true，说明已经处理过，不需要再做处理
            if (oCellMergeState.at(nRow - nFirstVisualRow).at(nCol - nFirstVisualCol))
            {
                ++nCol;
                continue;
            }

            QModelIndex index = dataIndex(d->m_model->index(nCurLogicalRow, nCurLogicalCol));

            int nCurMergeID = pDataModel->data(index, gidrMergeIDRole).toInt();
            // MergeID是0，说明不需要合并
            if (nCurMergeID == 0)
            {
                oCellMergeState[nRow - nFirstVisualRow].setBit(nCol - nFirstVisualCol, true);
            }
            else
            {
                // 如果MergeID不是0，则需要向后找到第一个与MergeID不同的格子
                int nLastSpanRow = findLastSpanRow(nCurMergeID, nRow, nLastVisualRow, nCurLogicalCol);
                int nLastSpanCol = findLastSpanCol(nCurMergeID, nCol, nLastVisualCol, nCurLogicalRow);

                // 找到后，要判断合并格的个数是大于1的，并且将最大的矩形区域合并
                if (nLastSpanRow - nRow < 1 && nLastSpanCol - nCol < 1)
                {
                    ++nCol;
                    continue;
                }

                setMergeCellState(
                            nRow - nFirstVisualRow, nLastSpanRow - nFirstVisualRow,
                            nCol - nFirstVisualCol, nLastSpanCol - nFirstVisualCol, oCellMergeState);

                // 合并单元格
                setSpan(nCurVisualRow, nCurVisualCol, nLastSpanRow - nRow + 1, nLastSpanCol - nCol + 1);

                // 遍历过的格子，不需要再遍历
                nCol = nLastSpanCol;
            }
            ++nCol;
        }
    }
}

int GLDTableView::findLastSpanRow(
        const int nCurMergeID, const int nRow, const int nLastVisualRow,
        const int nCurLogicalCol)
{
    Q_D(GLDTableView);

    QAbstractItemModel *pDataModel = itemModel();

    int nSpanRow = nRow + 1;
    while (true)
    {
        if (nSpanRow > nLastVisualRow)
        {
            --nSpanRow;
            break;
        }

        QModelIndex oRowSpanIndex = dataIndex(
                    d->m_model->index(d->logicalRow(nSpanRow), nCurLogicalCol));

        int nSpanRowMergeID = pDataModel->data(oRowSpanIndex, gidrMergeIDRole).toInt();

        if (nSpanRowMergeID != nCurMergeID)
        {
            --nSpanRow;
            break;
        }

        ++nSpanRow;
    }

    return nSpanRow;
}

int GLDTableView::findLastSpanCol(
        const int nCurMergeID, const int nCol, const int nLastVisualCol, const int nCurLogicalRow)
{
    Q_D(GLDTableView);

    QAbstractItemModel *pDataModel = itemModel();

    int nSpanCol = nCol + 1;

    while (true)
    {
        if (nSpanCol > nLastVisualCol)
        {
            --nSpanCol;
            break;
        }

        QModelIndex spancIndex = dataIndex(
                    d->m_model->index(nCurLogicalRow, d->logicalColumn(nSpanCol)));

        int nSpanColMergeID = pDataModel->data(spancIndex, gidrMergeIDRole).toInt();

        if (nSpanColMergeID != nCurMergeID)
        {
            --nSpanCol;
            break;
        }

        ++nSpanCol;
    }

    return nSpanCol;
}

void GLDTableView::initResizeInfoFrames(Qt::Orientation direction)
{
    Q_D(GLDTableView);

    if (NULL == d->m_pResizeInfoLineFrame)
    {
        d->m_pResizeInfoLineFrame = new QFrame(this);

        // 去掉Frame的外边框
        d->m_pResizeInfoLineFrame->setFrameShape(Panel);
        // 去掉最小化，关闭按钮
        d->m_pResizeInfoLineFrame->setWindowFlags(Qt::ToolTip);

        // 设置背景Frame背景色
        QPalette oPalette = d->m_pResizeInfoLineFrame->palette();
        oPalette.setColor(QPalette::Window, QColor(255, 255, 225));

        d->m_pResizeInfoLineFrame->setPalette(oPalette);
    }

    if (direction == Qt::Horizontal)
    {
        d->m_pResizeInfoLineFrame->resize(1, viewport()->height());
    }
    else
    {
        d->m_pResizeInfoLineFrame->resize(viewport()->width(), 1);
    }

    if (d->m_infoFrame == NULL)
    {
        d->m_infoFrame = new GInfoFrame(this);
    }

    d->m_infoFrame->setOrientation(direction);
}

void GLDTableView::updateVerticalHeaderGeometry(int nVerticalHeaderWidth)
{
    Q_D(GLDTableView);

    QRect oViewportGeometry = d->viewport->geometry();
    int nVerticalLeft = oViewportGeometry.left() - nVerticalHeaderWidth;

    d->m_verticalHeader->setGeometry(
                nVerticalLeft, oViewportGeometry.top(),
                nVerticalHeaderWidth, oViewportGeometry.height());

    if (d->m_verticalHeader->isHidden())
    {
        QMetaObject::invokeMethod(d->m_verticalHeader, "updateGeometries");
    }
}

void GLDTableView::updateHorizontalHeaderGeometry(int nHorizontalHeaderHeight)
{
    Q_D(GLDTableView);

    QRect oViewportGeometry = d->viewport->geometry();
    int nHorizontalTop = oViewportGeometry.top() - nHorizontalHeaderHeight;

    d->m_horizontalHeader->setGeometry(
                oViewportGeometry.left(), nHorizontalTop,
                oViewportGeometry.width(), nHorizontalHeaderHeight);

    if (d->m_horizontalHeader->isHidden())
    {
        QMetaObject::invokeMethod(d->m_horizontalHeader, "updateGeometries");
    }
}

void GLDTableView::updateConrnerWidgetGeometry(int nVerticalHeaderWidth, int nHorizontalHeaderHeight)
{
    Q_D(GLDTableView);

    QRect oViewportGeometry = d->viewport->geometry();
    int nHorizontalTop = oViewportGeometry.top() - nHorizontalHeaderHeight;
    int nVerticalLeft = oViewportGeometry.left() - nVerticalHeaderWidth;

    if (d->m_horizontalHeader->isHidden() || d->m_verticalHeader->isHidden())
    {
        d->m_cornerWidget->setHidden(true);
    }
    else
    {
        d->m_cornerWidget->setHidden(false);
        d->m_cornerWidget->setGeometry(nVerticalLeft, nHorizontalTop, nVerticalHeaderWidth, nHorizontalHeaderHeight);
    }
}

void GLDTableView::updateHorizontalScrollBar(const QSize &oViewportSize)
{
    Q_D(GLDTableView);

    //there must be always at least 1 column
    int nColumnsInViewport = qMax(colCountBackwardsInViewPort(oViewportSize), 1);

    if (horizontalScrollMode() == GlodonAbstractItemView::ScrollPerItem)
    {
        const int c_nColumnCount = d->m_horizontalHeader->count();
        const int c_visibleColumns = c_nColumnCount - d->m_horizontalHeader->hiddenSectionCount();

        int nFixHiddenColCount = 0;
        for (int nIndex = 0; nIndex < d->m_nFixedColCount; ++nIndex)
        {
            if (this->horizontalHeader()->isSectionHidden(nIndex))
            {
                ++nFixHiddenColCount;
            }
        }

        horizontalScrollBar()->setRange(0, c_visibleColumns - nColumnsInViewport - (d->m_nFixedColCount - nFixHiddenColCount));
        horizontalScrollBar()->setPageStep(nColumnsInViewport);

        if (nColumnsInViewport >= c_visibleColumns && c_nColumnCount != 0)
        {
            d->m_horizontalHeader->setOffset(0);
        }
        horizontalScrollBar()->setSingleStep(1);
    }
    else // ScrollPerPixel
    {
        uint nHorizontalHeaderLength = d->m_horizontalHeader->length();

        horizontalScrollBar()->setPageStep(oViewportSize.width());
        horizontalScrollBar()->setRange(0, nHorizontalHeaderLength - oViewportSize.width());
        horizontalScrollBar()->setSingleStep(qMax(oViewportSize.width() / (nColumnsInViewport + 1), 2));
    }
}

void GLDTableView::updateVerticalScrollBar(const QSize &oViewportSize)
{
    Q_D(GLDTableView);

    //there must be always at least 1 row
    int nRowsInViewport = qMax(rowCountBackwardsInViewPort(oViewportSize), 1);

    if (verticalScrollMode() == GlodonAbstractItemView::ScrollPerItem)
    {
        const int c_nRowCount = d->m_verticalHeader->count();
        const int c_nVisibleRows = c_nRowCount - d->m_verticalHeader->hiddenSectionCount();

        int nFixHiddenRowCount = 0;
        for (int nIndex = 0; nIndex < d->m_nFixedRowCount; ++nIndex)
        {
            if (this->verticalHeader()->isSectionHidden(nIndex))
            {
                ++nFixHiddenRowCount;
            }
        }

        if (d->m_showEllipsisButton && verticalScrollBar()->isVisible())
        {
            verticalScrollBar()->setRange(0, c_nVisibleRows - nRowsInViewport - (d->m_nFixedRowCount - nFixHiddenRowCount) + 1);
        }
        else
        {
            verticalScrollBar()->setRange(0, c_nVisibleRows - nRowsInViewport - (d->m_nFixedRowCount - nFixHiddenRowCount));
        }

        verticalScrollBar()->setPageStep(nRowsInViewport);
        if (nRowsInViewport >= c_nVisibleRows && c_nRowCount != 0)
        {
            d->m_verticalHeader->setOffset(0);
        }
        verticalScrollBar()->setSingleStep(1);
    }
    else // ScrollPerPixel
    {
        uint nVerticalHeaderLength = d->m_verticalHeader->length();
        verticalScrollBar()->setPageStep(oViewportSize.height());

        if (d->m_showEllipsisButton && verticalScrollBar()->isVisible())
        {
            verticalScrollBar()->setRange(0, nVerticalHeaderLength - oViewportSize.height() + d->m_ellipsisButton->height());
        }
        else
        {
            verticalScrollBar()->setRange(0, nVerticalHeaderLength - oViewportSize.height());
        }

        verticalScrollBar()->setSingleStep(qMax(oViewportSize.height() / (nRowsInViewport + 1), 2));
    }
}

int GLDTableView::colCountBackwardsInViewPort(const QSize &oViewportSize)
{
    Q_D(GLDTableView);

    // 滚动条控制的应该是除了固定列区域之外的部分
    d->calculateFixedColumnWidth();

    // 计算range时，应该是【总列数-固定列数-最后一屏的列数】
    const int c_nColumnCount = d->m_horizontalHeader->count();
    const int c_nViewportWidth = oViewportSize.width() - d->m_nFixedColWidth;

    int nWidth = 0;
    int nColumnsInViewport = 0;
    // 计算最后一屏的列数
    for (int nColumn = c_nColumnCount - 1; nColumn >= 0; --nColumn)
    {
        int nLogical = d->m_horizontalHeader->logicalIndex(nColumn);
        if (!d->m_horizontalHeader->isSectionHidden(nLogical))
        {
            if (d->m_oSuitColWidthCols.contains(nLogical))
            {
                nWidth += calcSuitWidth(nLogical) + currentGridLineWidth();
            }
            else
            {
                nWidth += d->m_horizontalHeader->sectionSize(nLogical) + currentGridLineWidth();
            }

            if (nWidth > c_nViewportWidth)
            {
                break;
            }
            ++nColumnsInViewport;
        }
    }

    return nColumnsInViewport;
}

int GLDTableView::rowCountBackwardsInViewPort(const QSize &oViewportSize)
{
    Q_D(GLDTableView);

    // 滚动条控制的应该是除了固定行区域之外的部分
    d->calculateFixedRowHeight();

    // vertical scroll bar
    const int c_nRowCount = d->m_verticalHeader->count();
    const int c_nViewportHeight = oViewportSize.height() - d->m_nFixedRowHeight;

    GIntList oNeedCalcSuitRowHeightCols = needCalcSuitRowHeightColsInViewport();

    // 计算最后一屏的行数
    int nHeight = 0;
    int nRowsInViewport = 0;
    for (int nRow = c_nRowCount - 1; nRow >= 0; --nRow)
    {
        int nLogical = d->m_verticalHeader->logicalIndex(nRow);
        if (!d->m_verticalHeader->isSectionHidden(nLogical))
        {
            if (oNeedCalcSuitRowHeightCols.count() > 0)
            {
                nHeight += d->calcSuitHeight(oNeedCalcSuitRowHeightCols, nRow) + currentGridLineWidth();
            }
            else
            {
                nHeight += d->m_verticalHeader->sectionSize(nLogical) + currentGridLineWidth();
            }

            if (nHeight > c_nViewportHeight)
            {
                break;
            }
            ++nRowsInViewport;
        }
    }

    return nRowsInViewport;
}

void GLDTableView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_D(GLDTableView);

    // check if we really need to do anything
    if (!d->isIndexValid(index)
            || (d->m_model->parent(index) != d->m_root)
            || isRowHidden(index.row())
            || isColumnHidden(index.column()))
    {
        return;
    }

    d->calculateFixedRegion();

    GSpanCollection::GSpan span;
    if (d->hasSpans())
    {
        span = d->span(index.row(), index.column());
    }

    int nCellWidth = d->hasSpans()
                     ? d->columnSpanWidth(index.column(), span.width())
                     : d->m_horizontalHeader->sectionSize(index.column());

    if (d->m_oSuitColWidthCols.contains(index.column()))
    {
        nCellWidth = d->calcSuitWidth(index.column());
    }

    // Adjust horizontal position
    scrollHorizontalTo(index, hint, nCellWidth);

    int nCellHeight = d->hasSpans() ? d->rowSpanHeight(index.row(), span.height())
                                    : d->m_verticalHeader->sectionSize(index.row());

    QList<int> oNeedCalcHeightCols = needCalcSuitRowHeightColsInViewport();
    if (oNeedCalcHeightCols.count() == 0)
    {
        nCellHeight = d->calcSuitHeight(oNeedCalcHeightCols, index.row()) + currentGridLineWidth();
    }

    // Adjust vertical position
    scrollVerticalTo(index, hint, nCellHeight);
    update(index);
}

void GLDTableView::scrollHorizontalTo(const QModelIndex &index, ScrollHint &hint, int nCellWidth)
{
    if (horizontalScrollMode() == GlodonAbstractItemView::ScrollPerItem)
    {
        scrollHorizontalPerItemTo(index, hint, nCellWidth);
    }
    else // ScrollPerPixel
    {
        scrollHorizontalPerPixelTo(index, hint, nCellWidth);
    }
}

void GLDTableView::scrollHorizontalPerItemTo(const QModelIndex &index, ScrollHint &hint, int nCellWidth)
{
    Q_D(GLDTableView);

    // 当需要滚动到固定列时，由于固定列从来没改变过位置，因此不需要
    if (index.column() < d->m_nFixedColCount)
    {
        return;
    }

    int nViewportWidth = d->viewport->width();
    int nHorizontalOffset = d->m_horizontalHeader->offset();
    int nHorizontalPosition = d->m_horizontalHeader->sectionPosition(index.column());

    bool bPositionAtLeft = (nHorizontalPosition - nHorizontalOffset < d->m_nFixedColWidth);
    bool bPositionAtRight = (nHorizontalPosition - nHorizontalOffset + nCellWidth > nViewportWidth);

    int nHorizontalIndex = firstVisualIndexAfterHorizontalScroll(index, hint, nCellWidth);

    // 下面是做滚动操作
    if (bPositionAtRight || hint == PositionAtCenter || bPositionAtLeft)
    {
        int nHiddenSections = getHorizontalHiddenSectionCountBeforeIndex(nHorizontalIndex);
        // 因为固定可编辑列不需要滚动，也就是不占滚动步数
        // 所以设置的滚动步数时，应该是【前面计算出来的步数 - 隐藏的列数 - 固定可编辑列数】
        int nValue = nHorizontalIndex - nHiddenSections - d->m_nFixedColCount;
        horizontalScrollBar()->setValue(nValue);
        // 解决updateHorizontalScrollBar等地方中setOffset(0)而没有将滚动条复位而导致的滚动条和水平表头不能同步刷新问题
        if ((0 == d->m_horizontalHeader->offset()) && (0 != nValue))
        {
            d->m_horizontalHeader->setOffsetToSectionPosition(horizontalScrollBar()->value());
        }
    }
}

void GLDTableView::scrollHorizontalPerPixelTo(const QModelIndex &index, GlodonAbstractItemView::ScrollHint &hint, int nCellWidth)
{
    Q_D(GLDTableView);

    int nViewportWidth = d->viewport->width();
    int nHorizontalOffset = d->m_horizontalHeader->offset();
    int nHorizontalPosition = d->m_horizontalHeader->sectionPosition(index.column());

    if (hint == PositionAtCenter)
    {
        horizontalScrollBar()->setValue(nHorizontalPosition - ((nViewportWidth - nCellWidth) / 2));
    }
    else
    {
        if (nHorizontalPosition - nHorizontalOffset < 0 || nCellWidth > nViewportWidth)
        {
            horizontalScrollBar()->setValue(nHorizontalPosition);
        }
        else if (nHorizontalPosition - nHorizontalOffset + nCellWidth > nViewportWidth)
        {
            horizontalScrollBar()->setValue(nHorizontalPosition - nViewportWidth + nCellWidth);
        }
    }
}

bool GLDTableView::checkSelectionCopyEnable(const QModelIndexList &indexList) const
{
    //判断选择部分是否可以复制----同excel
    int nRowNumber = indexList.last().row() - indexList.first().row() + 1;
    int nColNumber = indexList.last().column() - indexList.first().column() + 1;

    bool bIsColumnSelectEnable = (indexList.count() % nRowNumber != 0);
    bool bHasRowSpan = (indexList.first().row() + nRowNumber - 1 < indexList.last().row());
    bool bHasColSpan = (indexList.first().column() + nColNumber - 1 < indexList.last().column());
    bool bHasMultiRow = nRowNumber > 1;
    bool bHasMultiCol = nColNumber > 1;

    return !(bIsColumnSelectEnable || (bHasMultiRow && bHasRowSpan) || (bHasMultiCol && bHasColSpan));
}

GString GLDTableView::getContentsFromOrderedIndexList(const QModelIndexList &indexList)
{
    Q_D(GLDTableView);

    GString result = "";

    for(int i = 0; i < indexList.count(); ++i)
    {
        QVariant variant = d->m_model->data(indexList.at(i));
        GString cellText = "";

        if (!variant.isNull())
        {
            try
            {
                if (variant.type() == GVariant::DateTime)
                {
                    GDateTime datatime = variant.toDateTime();
                    cellText = dateTimeToStr(datatime);
                }
                else
                {
                    cellText = variant.toString();
                }
            }
            catch (...)
            {
                cellText = "";
            }
        }

        cellText = stringReplace(cellText, "\t", " ");

        result += cellText;

        if(indexList.at(i).column() < indexList.last().column())
        {
            result += "\t";
        }
        else if(indexList.at(i).column() == indexList.last().column()
                && indexList.at(i).row() < indexList.last().row())
        {
            result += sLineBreak;
        }
    }

    return result;
}

void GLDTableView::fillCellsOnClipBoardText(const QModelIndexList &indexList, GString &sClipBoardText,
                                            int &nPasteRowCount, int &nPasteColCount)
{
    Q_D(GLDTableView);

    // 从Excel粘贴过来的文本是以"\n"结尾
    // 暂时不考虑从excel粘贴出来的格子里有自动换行的情况,相当复杂 // todo
    if (sClipBoardText.endsWith("\n"))
    {
        sClipBoardText = stringReplace(leftStr(sClipBoardText, sClipBoardText.length() - 1), "\n", sLineBreak);
    }

    QStringList stringList = sClipBoardText.split(sLineBreak);

    for (int i = 0; i < stringList.count(); ++i)
    {
        int nRow = i + indexList.first().row();

        if (d->m_model->rowCount() <= nRow)
        {
            break;
        }

        QStringList oCells = stringList.at(i).split("\t");
        nPasteColCount = 0;

        for (int j = 0; j < oCells.size(); ++j)
        {
            int nCol = j + indexList.first().column();

            if (d->m_model->columnCount() <= nCol)
            {
                break;
            }

            QModelIndex curIndex = d->m_model->index(nRow, nCol, d->m_root);
            setCellData(curIndex, oCells.at(j));
            ++nPasteColCount;
        }

        ++nPasteRowCount;
    }
}

void GLDTableView::setNewSelection(const QModelIndexList &indexList, int &nPasteRowCount, int &nPasteColCount)
{
    Q_D(GLDTableView);

    QItemSelection newSelection;

    QModelIndex tl = d->m_model->index(indexList.first().row(),
                                       indexList.first().column(), d->m_root);

    QModelIndex br = d->m_model->index(tl.row() + nPasteRowCount - 1,
                                       tl.column() + nPasteColCount - 1,
                                       d->m_root);

    newSelection.append(QItemSelectionRange(tl, br));
    d->m_selectionModel->select(newSelection, QItemSelectionModel::ClearAndSelect);
}

void GLDTableView::setCellData(const QModelIndex &index, const QVariant &variant)
{
    Q_D(GLDTableView);

    GlodonDefaultItemDelegate *pGlodonDelegate = d->delegateForIndex(index);
    QModelIndex oDataIndex = pGlodonDelegate->dataIndex(index);

    if (pGlodonDelegate->editable(oDataIndex))
    {
        bool bReadOnly = false;
        GEditStyle editStyle = pGlodonDelegate->editStyle(oDataIndex, bReadOnly);

        if ((d->m_editTriggers != NoEditTriggers) && !bReadOnly && editStyle != esNone)
        {
            d->m_model->setData(index, variant, Qt::EditRole);
        }
    }
}

void GLDTableView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_D(GLDTableView);

    if (topLeft == bottomRight && topLeft.isValid())
    {
        QModelIndex oVisualIndex = visualIndex(topLeft);
        if (!d->isVisibleIndex(oVisualIndex))
            return;

        refreshDisplayColRow();

        d->updateNoStaticEditorData(topLeft);

        if (isVisible() && !d->m_delayedPendingLayout)
        {
            // otherwise the items will be update later anyway
            update(topLeft);
        }
    }
    else
    {
        GlodonAbstractItemView::dataChanged(topLeft, bottomRight, roles);
    }
}

void GLDTableView::onHeaderDataChanged(Qt::Orientation orientation, int, int)
{
    if (orientation == Qt::Horizontal)
    {
        GlodonMultiHeaderView *pMultiHoriHeader = dynamic_cast<GlodonMultiHeaderView *>(horizontalHeader());
        if (NULL != pMultiHoriHeader)
        {
            pMultiHoriHeader->initTitles();
        }
    }
    else
    {
        GlodonMultiHeaderView *pMultiVertHeader = dynamic_cast<GlodonMultiHeaderView *>(verticalHeader());
        if (NULL != pMultiVertHeader)
        {
            pMultiVertHeader->initTitles();
        }
    }
}

void GLDTableView::scrollVerticalTo(const QModelIndex &index, ScrollHint &hint, int nCellHeight)
{
    Q_D(GLDTableView);

    // 当需要滚动到固定列时，由于固定列从来没改变过位置，因此不需要
    if (index.row() < d->m_nFixedRowCount)
    {
        return;
    }

    int nViewportHeight = d->viewport->height();
    int nVerticalOffset = d->m_verticalHeader->offset();
    int nVerticalPosition = d->m_verticalHeader->sectionPosition(index.row());

    bool bPositionAtTop = (nVerticalPosition - nVerticalOffset < d->m_nFixedRowHeight);
    bool bPositionAtBottom = (nVerticalPosition - nVerticalOffset + nCellHeight > nViewportHeight);

    if (bPositionAtTop)
    {
        if (hint == EnsureVisible)
        {
            hint = PositionAtTop;
        }
    } else if (bPositionAtBottom)
    {
        if (hint == EnsureVisible)
        {
            hint = PositionAtBottom;
        }
    }

    if (verticalScrollMode() == GlodonAbstractItemView::ScrollPerItem)
    {
        scrollVerticalPerItemTo(index, hint, nCellHeight);
    }
    else // ScrollPerPixel
    {
        scrollVerticalPerPixelTo(index, hint, nCellHeight);
    }
}

void GLDTableView::scrollVerticalPerItemTo(const QModelIndex &index, GlodonAbstractItemView::ScrollHint &hint, int nCellHeight)
{
    Q_D(GLDTableView);

    // 下面是做滚动操作
    int nVerticalIndex = firstVisualIndexAfterVerticalScroll(index, hint, nCellHeight);
    if (hint == PositionAtBottom || hint == PositionAtCenter || hint == PositionAtTop)
    {
        int nHiddenSections = getVerticalHiddenSectionCountBeforeIndex(nVerticalIndex);
        // 因为固定可编辑列不需要滚动，也就是不占滚动步数
        // 所以设置的滚动步数时，应该是【前面计算出来的步数 - 隐藏的行数 - 固定可编辑行数】
        int nValue = nVerticalIndex - nHiddenSections - d->m_nFixedRowCount;
        verticalScrollBar()->setValue(nValue);
    }
}

void GLDTableView::scrollVerticalPerPixelTo(const QModelIndex &index, GlodonAbstractItemView::ScrollHint &hint, int nCellHeight)
{
    Q_D(GLDTableView);

    int nViewportHeight = d->viewport->height();
    int nVerticalPosition = d->m_verticalHeader->sectionPosition(index.row());

    if (hint == PositionAtTop)
    {
        verticalScrollBar()->setValue(nVerticalPosition);
    }
    else if (hint == PositionAtBottom)
    {
        verticalScrollBar()->setValue(nVerticalPosition - nViewportHeight + nCellHeight);
    }
    else if (hint == PositionAtCenter)
    {
        verticalScrollBar()->setValue(nVerticalPosition - ((nViewportHeight - nCellHeight) / 2));
    }
}

/*!
    This slot is called to change the height of the given \a row. The
    old height is specified by \a oldHeight, and the new height by \a
    newHeight.

    \sa columnResized()
*/
void GLDTableView::rowResized(int row, int, int newHeight, bool isManual)
{
    Q_D(GLDTableView);

    if (isManual)
    {
        afterManualChangedRowHeight(row, newHeight);
        resetCommentPosition();
    }

    d->m_rowsToUpdate.append(row);

    if (d->m_rowResizeTimerID == 0)
    {
        d->m_rowResizeTimerID = startTimer(0);
    }

    resetEllipsisButtonLocation();
}

void GLDTableView::columnResized(int column, int oldWidth, int newWidth, bool isManual)
{
    Q_D(GLDTableView);

    if (d->m_bIsInAdjustFitCols)
    {
        return;
    }

    if (isManual)
    {
        afterManualChangedColWidth(column, newWidth);
        resetCommentPosition();
    }

    d->m_currResizeWidth = newWidth;

    // 自动列宽处理
    if (d->m_oSuitColWidthCols.contains(column) && (newWidth < oldWidth))
    {
        oldWidth = sizeHintForColumn(column);

        if (newWidth < oldWidth && isManual)
        {
            d->m_horizontalHeader->resizeSection(column, oldWidth);
            return;
        }
    }

    // 合适列宽处理
    if (d->m_oFitColWidthCols.size() != 0)
    {
        adjustColWidths(column);

        //设置完合适列宽后，将不是当前调整的合适列宽列手动刷新
        for (int i = 0; i < d->m_oFitColWidthCols.size(); ++i)
        {
            int nupdateCol = d->m_oFitColWidthCols.at(i);

            if (nupdateCol != column)
            {
                d->m_columnsToUpdate.append(nupdateCol);
            }
        }
    }

    // 自动行高处理
    if (d->m_oSuitRowHeightAccordingCols.size() != 0)
    {
        bindMergeCell();

        setGridDisplayRowHeights();
    }

    // Timer 刷新处理
    d->m_columnsToUpdate.append(column);

    if (d->m_columnResizeTimerID == 0)
    {
        d->m_columnResizeTimerID = startTimer(0);
    }

    // 更多按钮位置处理
    resetEllipsisButtonLocation();
}

void GLDTableView::timerEvent(QTimerEvent *event)
{
    Q_D(GLDTableView);

    if (event->timerId() == d->m_columnResizeTimerID)
    {
        updateGeometries();
        killTimer(d->m_columnResizeTimerID);
        d->m_columnResizeTimerID = 0;

        QRect rect;
        int nviewportHeight = d->viewport->height();
        int nviewportWidth = d->viewport->width();

        if (d->hasSpans())
        {
            rect = QRect(0, 0, nviewportWidth, nviewportHeight);
        }
        else
        {
            for (int i = d->m_columnsToUpdate.size() - 1; i >= 0; --i)
            {
                int ncolumn = d->m_columnsToUpdate.at(i);
                int nX = columnViewportPosition(ncolumn);

                if (isRightToLeft())
                {
                    rect |= QRect(0, 0, nX + columnWidth(ncolumn), nviewportHeight);
                }
                else
                {
                    rect |= QRect(nX, 0, nviewportWidth - nX, nviewportHeight);
                }
            }
        }

        d->viewport->update(rect.normalized());
        d->m_columnsToUpdate.clear();
        refreshDisplayRows();
    }

    if (event->timerId() == d->m_rowResizeTimerID)
    {
        updateGeometries();
        killTimer(d->m_rowResizeTimerID);
        d->m_rowResizeTimerID = 0;

        int nviewportHeight = d->viewport->height();
        int nviewportWidth = d->viewport->width();
        int ntop;

        if (d->hasSpans())
        {
            ntop = 0;
        }
        else
        {
            ntop = nviewportHeight;

            for (int i = d->m_rowsToUpdate.size() - 1; i >= 0; --i)
            {
                int nY = rowViewportPosition(d->m_rowsToUpdate.at(i));
                ntop = qMin(ntop, nY);
            }
        }

        d->viewport->update(QRect(0, ntop, nviewportWidth, nviewportHeight - ntop));
        d->m_rowsToUpdate.clear();
        refreshDisplayRows();
    }

    GlodonAbstractItemView::timerEvent(event);
}

void GLDTableView::onBoolCellPress(QMouseEvent *event)
{
    // TODO: delete
    G_UNUSED(event);
}

void GLDTableView::onMousePress(QMouseEvent *event)
{
    GlodonAbstractItemView::mousePressEvent(event);
}

void GLDTableView::mousePressEvent(QMouseEvent *event)
{
    Q_D(GLDTableView);

    // 单击表格CommitData
    if ((event->button() & Qt::LeftButton) && !commitDataAndCloseEditorInIngnoreFocusOut())
    {
        return;
    }

    //如果当前正在编辑的格子数据commitData时失败，则单击无效
    if (!d->m_legalData)
    {
        return;
    }

    QModelIndex index = indexAt(event->pos());

    clickEditorButtonAccordingToEditStyleDrawType(index, event);
    resetEnterJumpPreState();

    // todo: 需要先调整选择模型，然后才能保证填充和选区移动计算时，参考选区正确计算。
    onMousePress(event);

    dealWithSpecialMousePressOperations(index, event);
}

bool GLDTableView::viewportEvent(QEvent *event)
{
    Q_D(GLDTableView);

    if (event->type() == QEvent::Leave)
    {
        d->hideCommentFrame();
    }

    return GlodonAbstractItemView::viewportEvent(event);
}

void GLDTableView::mouseMoveEvent(QMouseEvent *event)
{
    bool bHasCursor = testAttribute(Qt::WA_SetCursor);
    QPoint pos = event->pos();

    // 在超链接格子时，鼠标需要变为小手
    if (inLinkCell(pos))
    {
        if (!bHasCursor)
        {
            setCursor(Qt::PointingHandCursor);
        }
    }
    else if (shouldSetRangeFillMouseShape(pos))
    {
        setRangeFillMouseShape(bHasCursor);
    }
    else if (isInRangeFilling())
    {
        dealWithRangeFillingOnMouseMove(bHasCursor, pos);
    }
    else if (shouldSetRangeMoveMouseShape(pos))
    {
        setRangeMoveMouseShape(bHasCursor);
    }
    else if (isInRangeMoving())
    {
        dealWithRangeMovingOnMouseMove(pos);
    }
    else if (shouldSetResizeCellByDragGridLineMouseShape(event))
    {
        setResizeCellByDragGridLineMouseShape(event);
    }
    else if (isInResizingCellByDragGridLine(event))
    {
        dealWithResizeCellWhenDragGridLine(event);
    }
    else if (bHasCursor)
    {
        recoverCursor();
    }
    else
    {
        GlodonAbstractItemView::mouseMoveEvent(event);
    }

    // 批注框
    dealWithCurCommet(pos);
}

void GLDTableView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (inBoolCell(event->pos()))
    {
        mousePressEvent(event);
    }
    else
    {
        GlodonAbstractItemView::mouseDoubleClickEvent(event);
    }
}

void GLDTableView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(GLDTableView);

    // 结束拖拉复制
    if (d->m_state == RangeFillingState)
    {
        completeRangeFilling();
    }
    // 结束框选拖拽
    else if (d->m_state == RangeMovingState)
    {
        completeRangeMoving();
    }
    // 结束拖拽格线调整行高列宽
    else if (d->m_allowToResizeCellByDragGridLine
             && event->button() == Qt::LeftButton
             && d->m_state == DragResizingState)
    {
        completeResizeCellByDragGridLine(event);
    }
    else
    {
        GlodonAbstractItemView::mouseReleaseEvent(event);
    }

    d->hideCommentFrame();
}

void GLDTableView::keyPressEvent(QKeyEvent *event)
{
    Q_D(GLDTableView);

    if (copyOrPastOperation(event))
        return;

    switch (event->key())
    {
    case Qt::Key_Down:
    case Qt::Key_Up:
    {
        if (openEditOrScrollContent(event))
            return;
        break;
    }
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Escape:
    {
        resetEnterJumpPreState();
        break;
    }
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        dealWithKeyEnterPress(event);
        return;
    }
    case Qt::Key_Backspace:
    {
        if (currentIndex().isValid() && state() != EditingState && (NoEditTriggers != d->m_editTriggers))
        {
            QKeyEvent keyEventBackspace(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
            edit(currentIndex(), AnyKeyPressed, &keyEventBackspace);
            event->accept();
            return;
        }
        break;
    }
    case Qt::Key_F2:
    case Qt::Key_Space:
    {
        QModelIndex index = currentIndex();
        if (index.isValid() && index.data(Qt::DisplayRole).type() == QVariant::Bool)
        {
            setBoolEditValue(index);
            return;
        }
        break;
    }
    }
    GlodonAbstractItemView::keyPressEvent(event);
}

void GLDTableView::resizeEvent(QResizeEvent *event)
{
    GlodonAbstractItemView::resizeEvent(event);

    const bool isToggleHScrollBar = event->size().height() - event->oldSize().height() ==  horizontalScrollBar()->height();
    const bool isToggleVScrollBar = event->size().width() - event->oldSize().width() == verticalScrollBar()->width();

    if (!isToggleHScrollBar && !isToggleVScrollBar)
    {
        refreshDisplayColRow();
    }
}

void GLDTableView::leaveEvent(QEvent *event)
{
    Q_D(GLDTableView);

    d->hideCommentFrame();

    GlodonAbstractItemView::leaveEvent(event);
}

void GLDTableView::wheelEvent(QWheelEvent *event)
{
    Q_D(GLDTableView);

    QPersistentModelIndex index = indexAt(event->pos());

    // TODO liurx 逻辑有点问题，待整理comment功能时再详细看
    if ((d->m_pCommentFrame == NULL) || (state() != DragSelectingState))
    {
        d->drawComment(index);
    }
    else
    {
        d->m_pCommentFrame->hide();
    }

    GlodonAbstractItemView::wheelEvent(event);
}

void GLDTableView::focusOutEvent(QFocusEvent *event)
{
    Q_D(GLDTableView);

    d->hideCommentFrame();

    GlodonAbstractItemView::focusOutEvent(event);
}

void GLDTableView::showEvent(QShowEvent *event)
{
    GlodonAbstractItemView::showEvent(event);
    refreshDisplayColRow();
}

QRect GLDTableView::visualRectForRowColNo(const QRect &rowColNo)
{
    Q_D(GLDTableView);

    if (!rowColNo.isValid())
        return QRect();

    QModelIndex oTopLeftIndex = d->m_model->index(rowColNo.top(), rowColNo.left());
    QModelIndex oBottomRightIndex = d->m_model->index(rowColNo.bottom(), rowColNo.right());
    QRect oTopLeftRect = visualRect(logicalIndex(oTopLeftIndex));
    QRect oBottomRightRect = visualRect(logicalIndex(oBottomRightIndex));
    return QRect(oTopLeftRect.left(),
                 oTopLeftRect.top(),
                 oBottomRightRect.right() - oTopLeftRect.left(),
                 oBottomRightRect.bottom() - oTopLeftRect.top());
}

void GLDTableView::closeEditor(QWidget *editor, bool &canCloseEditor, GlodonDefaultItemDelegate::EndEditHint hint)
{
    Q_D(GLDTableView);

    //如果commitData失败，就不能退出编辑状态
    if (d->m_inCommitData
            || ((!d->m_legalData) && (hint != GlodonDefaultItemDelegate::RevertModelCache)))
    {
        return;
    }

    // Close the editor
    //关闭编辑控件之前也需要判断控件是否为静态
    QModelIndex index = d->indexForEditor(editor);

    if (editor && !d->m_indexEditorHash.value(index).isStatic)
    {
        GlodonDefaultItemDelegate *pDelegate = d->delegateForIndex(index);

        QVariant data = pDelegate->currentEditorData(index, editor);
        emit onCloseEditor(dataIndex(index), data, canCloseEditor);

        if (!canCloseEditor)
        {
            d->m_legalData = false;
            editor->setFocus();
            return;
        }

        d->m_legalData = true;
        pDelegate->setCurEditor(NULL);

        //退出编辑时，如果设置了行选，把固定行的格子选中－byHuangp
        if (d->m_selectionModel->isRowSelected(index.row(), QModelIndex()))
        {
            GlodonMultiHeaderView *oMultiHeaderView = dynamic_cast<GlodonMultiHeaderView *>(d->m_verticalHeader);

            if (NULL != oMultiHeaderView)
            {
                oMultiHeaderView->setSpansSelected(index.row(), true);
            }
        }
    }

    GlodonAbstractItemView::closeEditor(editor, canCloseEditor, hint);

    processEnterJumpAfterCloseEdit(hint);
}

void GLDTableView::commitData(QWidget *editor, bool &canCloseEditor)
{
    Q_D(GLDTableView);

    if (!editor || !d->m_itemDelegate || d->m_pCurCommittingEditor || d->m_inCommitData)
    {
        return;
    }

    d->m_inCommitData = true;

    QModelIndex index = d->indexForEditor(editor);

    if (!index.isValid())
    {
        d->m_inCommitData = false;
        return;
    }

    GlodonDefaultItemDelegate *pGlodonDelegate = d->delegateForIndex(index);

    try
    {
        QVariant data = pGlodonDelegate->currentEditorData(index, editor);
        emit onCommitEditor(dataIndex(index), data, canCloseEditor);

        if (!canCloseEditor)
        {
            d->m_legalData = false;
            d->m_inCommitData = false;
            editor->setFocus();
            processScrollBarAfterCanNotCloseEditor();
            return;
        }
        else
        {
            d->m_legalData = true;
        }

        GlodonAbstractItemView::commitData(editor, canCloseEditor);
    }
    catch (...)
    {
        if (pGlodonDelegate->m_bCloseEditorOnFocusOut)
        {
            pGlodonDelegate->m_bRepeatCommit = true;
        }
        d->m_inCommitData = false;

        throw;
    }

    d->m_inCommitData = false;
}

void GLDTableView::rowMoved(int, int oldIndex, int newIndex)
{
    Q_D(GLDTableView);

    updateGeometries();
    int nlogicalOldIndex = d->m_verticalHeader->logicalIndex(oldIndex);
    int nlogicalNewIndex = d->m_verticalHeader->logicalIndex(newIndex);

    if (d->hasSpans())
    {
        d->viewport->update();
    }
    else
    {
        int noldTop = rowViewportPosition(nlogicalOldIndex);
        int newTop = rowViewportPosition(nlogicalNewIndex);
        int noldBottom = noldTop + rowHeight(nlogicalOldIndex);
        int newBottom = newTop + rowHeight(nlogicalNewIndex);
        int ntop = qMin(noldTop, newTop);
        int nbottom = qMax(noldBottom, newBottom);
        int nheight = nbottom - ntop;
        d->viewport->update(0, ntop, d->viewport->width(), nheight);
    }

//    QMap<QPersistentModelIndex, GCustomCommentFrame*>::iterator it = d->m_commentFrame.begin();
//    while (it != d->m_commentFrame.end())
//    {
//        if (it.key().column() == oldIndex)
//        {
//            it.value()->setFramePosition(visualRect(it.key()).topRight());
//        }
//        ++it;
//    }
}

void GLDTableView::canColumnMoved(int from, int to, bool &canMove)
{
    Q_D(GLDTableView);

    for (int row = 0; row < verticalHeader()->count(); ++row)
    {
        GSpanCollection::GSpan *fromSpan = d->m_spans.spanAt(from, row);
        GSpanCollection::GSpan *toSpan = d->m_spans.spanAt(to, row);

        if ((NULL != fromSpan) || (NULL != toSpan))
        {
            canMove = false;
        }
    }

    if (!canMove)
    {
        d->viewport->update();
    }
}

void GLDTableView::columnMoved(int, int oldIndex, int newIndex)
{
    Q_D(GLDTableView);

    updateGeometries();
//    int logicalOldIndex = d->horizontalHeader->logicalIndex(oldIndex);
//    int logicalNewIndex = d->horizontalHeader->logicalIndex(newIndex);
//    if (d->hasSpans())
//    {
    //列移动时，算刷新区域的效率，和刷全部区域的效率差不多，而且算出来的区域偏小，开启填充功能时，会有黑点
    d->viewport->update();
//    }
//    else
//    {
//        int oldLeft = columnViewportPosition(logicalOldIndex);
//        int newLeft = columnViewportPosition(logicalNewIndex);
//        int oldRight = oldLeft + columnWidth(logicalOldIndex);
//        int newRight = newLeft + columnWidth(logicalNewIndex);
//        int left = qMin(oldLeft, newLeft);
//        int right = qMax(oldRight, newRight);
//        int width = right - left;
//        d->viewport->update(left, 0, width, d->viewport->height());
//    }
//    QMap<QPersistentModelIndex, GCustomCommentFrame*>::iterator it = d->m_commentFrame.begin();
//    while (it != d->m_commentFrame.end())
//    {
//        if (it.key().column() == oldIndex || it.key().column() == newIndex)
//        {
//            QModelIndex visual = visualIndex(it.key());
//            it.value()->setFramePosition(visualRect(visual).topRight());
//        }
//        ++it;
//    }
    Q_UNUSED(oldIndex);
    Q_UNUSED(newIndex);
}

void GLDTableView::selectRow(int row)
{
    Q_D(GLDTableView);

    if (d->m_allowSelectRow)
    {
        d->selectRow(row, true);
    }
}

void GLDTableView::selectColumn(int column)
{
    Q_D(GLDTableView);

    if (d->m_allowSelectCol)
    {
        d->selectColumn(column, true);
    }
}

void GLDTableView::hideRow(int row)
{
    Q_D(GLDTableView);

    d->m_verticalHeader->hideSection(row);
}

void GLDTableView::hideColumn(int column)
{
    Q_D(GLDTableView);
    d->m_horizontalHeader->hideSection(column);
}

void GLDTableView::showRow(int row)
{
    Q_D(GLDTableView);
    d->m_verticalHeader->showSection(row);
}

void GLDTableView::showColumn(int column)
{
    Q_D(GLDTableView);
    d->m_horizontalHeader->showSection(column);
}

void GLDTableView::resizeRowToContents(int row)
{
    Q_D(GLDTableView);

    int nContent = sizeHintForRow(row);
    int nHeaderSize = d->m_verticalHeader->sectionSizeHint(row);

    int nNewHeight = qMax(nContent, nHeaderSize);
    d->m_verticalHeader->resizeSection(row, nNewHeight);

    afterManualChangedRowHeight(row, nNewHeight);
    resetCommentPosition();
}

void GLDTableView::resizeRowsToContents()
{
    Q_D(GLDTableView);

    QList<int> oAllColumns;
    for (int i = 0; i < model()->columnCount(); ++i)
    {
        oAllColumns.append(i);
    }

    QMap<int, int> oResizeSections;
    for (int i = 0; i < model()->rowCount(); ++i)
    {
        oResizeSections.insert(i, qMax(d->calcSizeHintForRow(i, oAllColumns), d->m_verticalHeader->sectionSizeHint(i)));
    }
    d->m_verticalHeader->batchResizeSection(oResizeSections);
    updateAll();

    resetCommentPosition();
}

void GLDTableView::resizeColumnToContents(int column)
{
    Q_D(GLDTableView);

    int nContent = sizeHintForColumn(column);
    int nHeaderSize = d->m_horizontalHeader->sectionSizeHint(column);

    int nNewWidth = qMax(nContent, nHeaderSize);
    d->m_horizontalHeader->resizeSection(column, nNewWidth);

    afterManualChangedColWidth(column, nNewWidth);

    resetCommentPosition();
}

void GLDTableView::resizeColumnsToContents()
{
    Q_D(GLDTableView);

    QMap<int, int> oResizeSections;
    for (int i = 0; i < model()->columnCount(); ++i)
    {
        oResizeSections.insert(i, qMax(d->sizeHintForColumn(i, true), d->m_horizontalHeader->sectionSizeHint(i)));
    }

    d->m_horizontalHeader->batchResizeSection(oResizeSections);
    updateAll();
}

void GLDTableView::setSuitRowHeightForAll(bool value, bool calcAllColumns)
{
    Q_D(GLDTableView);
    d->m_bCalcAllColumns = calcAllColumns;
    d->m_bAllRowsResizeToContents = value;
    d->fillSuitRowHeightCols();
}

void GLDTableView::setSuitColWidthForAll(bool value, bool calcAllRows)
{
    Q_D(GLDTableView);
    d->m_bCalcAllRows = calcAllRows;
    d->m_bAllColumnsResizeToContents = value;

    d->fillSuitColWidthCols();
}

void GLDTableView::setAllowResizeCellByDragGridLine(bool canResize)
{
    Q_D(GLDTableView);
    d->m_allowToResizeCellByDragGridLine = canResize;
}

bool GLDTableView::allowResizeCellByDragGridLine()
{
    Q_D(GLDTableView);
    return d->m_allowToResizeCellByDragGridLine;
}

bool GLDTableView::enterJump() const
{
    Q_D(const GLDTableView);
    return d->m_bEnterJump;
}

void GLDTableView::setEnterJump(bool value)
{
    Q_D(GLDTableView);

    if (!value)
    {
        setEnterJumpPro(value);
    }

    if (d->m_bEnterJump != value)
    {
        d->m_itemDelegate.data()->setIsEnterJump(value);
        d->m_bEnterJump = value;
    }
}

bool GLDTableView::enterJumpPro() const
{
    Q_D(const GLDTableView);
    return d->m_bEnterJumpPro;
}

void GLDTableView::setEnterJumpPro(bool value)
{
    Q_D(GLDTableView);

    // 开启高级回车跳格，默认开启回车跳格
    if (value)
    {
        setEnterJump(true);
    }

    if (d->m_bEnterJumpPro != value)
    {
        d->m_itemDelegate.data()->setIsEnterJumpPro(value);
        d->m_bEnterJumpPro = value;
    }
}

void GLDTableView::setUseBlendColor(bool value)
{
    Q_D(GLDTableView);

    if (d->m_bUseBlendColor != value)
    {
        d->m_itemDelegate.data()->setUseBlendColor(value);
        d->m_bUseBlendColor = value;

        d->viewport->update();
    }
}

bool GLDTableView::useBlendColor() const
{
    Q_D(const GLDTableView);
    return d->m_bUseBlendColor;
}

QColor GLDTableView::selectedCellBackgroundColor() const
{
    Q_D(const GLDTableView);
    return d->m_oSelectedCellBackgroundColor;
}

void GLDTableView::setSelectedCellBackgroundColor(QColor value)
{
    Q_D(GLDTableView);

    if (d->m_oSelectedCellBackgroundColor != value)
    {
        d->m_itemDelegate.data()->setSelectedCellBackgroundColor(value);
        d->m_oSelectedCellBackgroundColor = value;

        d->viewport->update();
    }
}

void GLDTableView::setSelectedBoundLineColor(QColor value)
{
    Q_D(GLDTableView);
    d->m_selectBoundLineColor = value;
}

QColor GLDTableView::selectedBoundLineColor() const
{
    Q_D(const GLDTableView);
    return d->m_selectBoundLineColor;
}

void GLDTableView::setSelectedBoundLineWidth(SelectBorderWidth lineWidth)
{
    Q_D(GLDTableView);
    d->m_selectBoundLineWidth = (int)lineWidth;
}

int GLDTableView::selectedBoundLineWidth() const
{
    Q_D(const GLDTableView);
    return d->m_selectBoundLineWidth;
}

void GLDTableView::setNoFocusSelectedBoundLineColor(QColor value)
{
    Q_D(GLDTableView);
    d->m_oNoFocusSelectedBoundLineColor = value;
}

QColor GLDTableView::NoFocusSelectedBoundLineColor() const
{
    Q_D(const GLDTableView);
    return d->m_oNoFocusSelectedBoundLineColor;
}

void GLDTableView::sortByColumn(int column)
{
    Q_D(GLDTableView);

    if (column == -1)
    {
        return;
    }

    d->m_horizontalHeader->setSortIndicatorSection(column);

    d->m_model->sort(column, d->m_horizontalHeader->sortIndicatorOrder());
}

void GLDTableView::sortByColumn(int column, Qt::SortOrder order)
{
    Q_D(GLDTableView);
    d->m_horizontalHeader->setSortIndicator(column, order);
    sortByColumn(column);
}

void GLDTableView::verticalScrollbarAction(int action)
{
    GlodonAbstractItemView::verticalScrollbarAction(action);
}

void GLDTableView::horizontalScrollbarAction(int action)
{
    GlodonAbstractItemView::horizontalScrollbarAction(action);
}

bool GLDTableView::isIndexHidden(const QModelIndex &index) const
{
    Q_D(const GLDTableView);
    Q_ASSERT(d->isIndexValid(index));

    if (isRowHidden(index.row()) || isColumnHidden(index.column()))
    {
        return true;
    }

    if (d->hasSpans())
    {
        GSpanCollection::GSpan span = d->span(index.row(), index.column());
        return !((span.top() == index.row()) && (span.left() == index.column()));
    }

    return false;
}

bool GLDTableView::inBoolCell(const QPoint pos) const
{
    Q_D(const GLDTableView);
    QModelIndex index = indexAt(pos);

    if (index.isValid())
    {
        GlodonDefaultItemDelegate *pIndexDelegate = d->delegateForIndex(index);

        pIndexDelegate->setCurrTreeEditting(index);
        return index.data().type() == QVariant::Bool;
    }

    return false;
}

int GLDTableView::rowSpan(int row, int column) const
{
    Q_D(const GLDTableView);
    return d->rowSpan(row, column);
}

int GLDTableView::columnSpan(int row, int column) const
{
    Q_D(const GLDTableView);
    return d->columnSpan(row, column);
}

QModelIndex GLDTableView::spanAt(int row, int column)
{
    Q_D(const GLDTableView);
    GSpanCollection::GSpan *span = d->m_spans.spanAt(column, row);

    if (NULL == span)
    {
        return QModelIndex();
    }
    else
    {
        return d->m_model->index(span->left(), span->top());
    }
}

void GLDTableViewPrivate::_q_selectRow(int row)
{
    selectRow(row, false);
}

void GLDTableViewPrivate::_q_selectColumn(int column)
{
    selectColumn(column, false);
}

void GLDTableViewPrivate::_q_selectRows(int left, int top, int right, int bottom)
{
    Q_UNUSED(left);
    Q_UNUSED(right);
    selectRows(top, bottom, true);
}

void GLDTableViewPrivate::_q_selectColumns(int left, int top, int right, int bottom)
{
    Q_UNUSED(top);
    Q_UNUSED(bottom);
    selectColumns(left, right, true);
}

void GLDTableViewPrivate::doSelect(
        const QModelIndex &tl, const QModelIndex &br, QItemSelectionModel::SelectionFlags command, bool)
{
    QItemSelection selection(tl, br);

    m_selectionModel->select(selection, command);
}

void GLDTableViewPrivate::selectRow(int row, bool anchor)
{
    Q_Q(GLDTableView);

    if (selectRows(row, row, anchor))
    {
        QModelIndex currentScrollToIndex = m_model->index(row, q->currentIndex().column(), m_root);
        q->scrollTo(currentScrollToIndex);
    }
}

void GLDTableViewPrivate::selectColumn(int column, bool anchor)
{
    Q_Q(GLDTableView);

    if (selectColumns(column, column, anchor))
    {
        QModelIndex currentScrollToIndex = m_model->index(q->currentIndex().row(), column, m_root);
        q->scrollTo(currentScrollToIndex);
    }
}

bool GLDTableViewPrivate::selectColumns(int start, int end, bool anchor)
{
    Q_Q(GLDTableView);

    if ((start > end) || (start < 0) || (end >= m_horizontalHeader->count()))
        return false;

    if (q->selectionBehavior() == GLDTableView::SelectRows
            || (q->selectionMode() == GLDTableView::SingleSelection
                && q->selectionBehavior() == GLDTableView::SelectItems))
    {
        return false;
    }

    m_rangeFillHandlePositon = RightTop;

    int row = m_verticalHeader->logicalIndexAt(0);
    QModelIndex index = m_model->index(row, start, m_root);

    QItemSelectionModel::SelectionFlags command = q->selectionCommand(index);

    m_bIsInMultiSelect = command & QItemSelectionModel::Toggle ? true : false;

    bool bShouldSelect = true;

    if (anchor)
    {
        if (q->currentIndex().isValid())
        {
            if (!q->commitDataAndCloseEditorInIngnoreFocusOut())
                return false;

            bShouldSelect = q->moveCurrent(q->currentIndex(),
                                           m_model->index(q->currentIndex().row(), start, m_root),
                                           command, mrProgram);
        }
        else
        {
            bShouldSelect = q->moveCurrent(q->currentIndex(), index, command, mrProgram);
        }
    }

    if ((anchor && !(command & QItemSelectionModel::Current))
            || (q->selectionMode() == GLDTableView::SingleSelection))
    {
        m_columnSectionAnchor = start;
    }

    if (q->selectionMode() != GLDTableView::SingleSelection
            && command.testFlag(QItemSelectionModel::Toggle))
    {
        if (anchor)
        {
            m_ctrlDragSelectionFlag = m_verticalHeader->selectionModel()->selectedColumns().contains(index)
                                      ? QItemSelectionModel::Deselect : QItemSelectionModel::Select;
        }

        command &= ~QItemSelectionModel::Toggle;
        command |= m_ctrlDragSelectionFlag;

        if (!anchor)
        {
            command |= QItemSelectionModel::Current;
        }
    }

    QModelIndex tl = m_model->index(0, qMin(m_columnSectionAnchor, start), m_root);
    QModelIndex br = m_model->index(m_model->rowCount(m_root) - 1,
                                    qMax(m_columnSectionAnchor, end), m_root);


    if (bShouldSelect)
    {
        if ((m_horizontalHeader->sectionsMoved() && tl.column() != br.column())
            || m_verticalHeader->sectionsMoved())
        {
            m_oTopLeftSelectIndex = q->visualIndex(tl);
            m_oBottomRightSelectIndex = q->visualIndex(br);
            q->setSelectionByIndex(m_oTopLeftSelectIndex, m_oBottomRightSelectIndex, command);
        }
        else
        {
            doSelect(tl, br, command, false);
        }
        return true;
    }

    return false;
}

bool GLDTableViewPrivate::selectRows(int start, int end, bool anchor)
{
    Q_Q(GLDTableView);

    if ((start > end) || (start < 0) || end >= m_verticalHeader->count())
        return false;

    if (q->selectionBehavior() == GLDTableView::SelectColumns
            || (q->selectionMode() == GLDTableView::SingleSelection
                && q->selectionBehavior() == GLDTableView::SelectItems))
    {
        return false;
    }

    m_rangeFillHandlePositon = LeftBottom;

    int ncolumn = m_horizontalHeader->logicalIndexAt(q->isRightToLeft() ? viewport->width() : 0);
    QModelIndex index = m_model->index(start, ncolumn, m_root);

    QItemSelectionModel::SelectionFlags command = q->selectionCommand(index);

    m_bIsInMultiSelect = command & QItemSelectionModel::Toggle ? true : false;

    bool bShouldSelect = true;

    if (anchor)
    {
        if (q->currentIndex().isValid())
        {
            if (!q->commitDataAndCloseEditorInIngnoreFocusOut())
            {
                return false;
            }

            bShouldSelect = q->moveCurrent(q->currentIndex(),
                                           m_model->index(start, q->currentIndex().column(), m_root),
                                           command, mrProgram);
        }
        else
        {
            bShouldSelect = q->moveCurrent(q->currentIndex(), index, command, mrProgram);
        }
    }

    if ((anchor && !(command & QItemSelectionModel::Current))
            || (q->selectionMode() == GLDTableView::SingleSelection))
    {
        m_rowSectionAnchor = start;
    }

    if (q->selectionMode() != GLDTableView::SingleSelection
            && command.testFlag(QItemSelectionModel::Toggle))
    {
        if (anchor)
        {
            m_ctrlDragSelectionFlag = m_verticalHeader->selectionModel()->selectedRows().contains(index)
                                      ? QItemSelectionModel::Deselect : QItemSelectionModel::Select;
        }

        command &= ~QItemSelectionModel::Toggle;
        command |= m_ctrlDragSelectionFlag;

        if (!anchor)
        {
            command |= QItemSelectionModel::Current;
        }
    }

    QModelIndex tl = m_model->index(qMin(m_rowSectionAnchor, start), 0, m_root);
    QModelIndex br = m_model->index(qMax(m_rowSectionAnchor, end), m_model->columnCount(m_root) - 1, m_root);

    if (bShouldSelect)
    {
        if ((m_verticalHeader->sectionsMoved() && tl.row() != br.row())
            || m_horizontalHeader->sectionsMoved())
        {
            m_oTopLeftSelectIndex = q->visualIndex(tl);
            m_oBottomRightSelectIndex = q->visualIndex(br);
            q->setSelectionByIndex(m_oTopLeftSelectIndex, m_oBottomRightSelectIndex, command);
        }
        else
        {
            doSelect(tl, br, command, true);
        }
        return true;
    }

    return false;
}

void GLDTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    GlodonAbstractItemView::currentChanged(current, previous);

    afterCurrentChanged(dataIndex(current), dataIndex(previous));
}

bool GLDTableView::isLegalData()
{
    Q_D(GLDTableView);
    return d->m_legalData;
}

void GLDTableView::setLegalData(bool value)
{
    Q_D(GLDTableView);
    d->m_legalData = value;
}

bool GLDTableView::event(QEvent *event)
{
    bool result = GlodonAbstractItemView::event(event);

    switch (event->type())
    {
        case QEvent::StyleChange:
        {
            if (isCustomStyle())
            {
                const_cast<QPalette &>(palette()).setColor(QPalette::Window, gridColor());
                setBackgroundRole(QPalette::Window);
                setGridColor(gridColor());
            }

            break;
        }

        default:
            break;
    }

    return result;
}

int GLDTableView::firstVisualRow() const
{
    Q_D(const GLDTableView);

    if (d->m_nFixedRowCount > 0)
    {
        d->calculateFixedRowHeight();
        // 为了获取固定行的后一行，所以增加一个像素
        return d->m_verticalHeader->visualIndexAt(d->m_nFixedRowHeight + 1);
    }

    return qMax(d->m_verticalHeader->visualIndexAt(0), 0);
}

int GLDTableView::lastVisualRow() const
{
    Q_D(const GLDTableView);

    int nLastVisualRow = d->m_verticalHeader->visualIndexAt(d->viewport->height());

    if (nLastVisualRow == -1)
    {
        nLastVisualRow = d->m_verticalHeader->count() - 1;
    }

    return nLastVisualRow;
}

int GLDTableView::firstVisualCol() const
{
    Q_D(const GLDTableView);

    if (d->m_nFixedColCount > 0)
    {
        d->calculateFixedColumnWidth();
        // 为了获取固定列的后一列，所以增加一个像素
        return d->m_horizontalHeader->visualIndexAt(d->m_nFixedColWidth + 1);
    }

    return d->m_horizontalHeader->visualIndexAt(0);
}

int GLDTableView::lastVisualCol() const
{
    Q_D(const GLDTableView);

    int nViewPortWidth = viewport()->width();
    int nlastVisualCol = d->m_horizontalHeader->visualIndexAt(nViewPortWidth);

    if (nlastVisualCol != -1)
    {
        int nLastVisualColPosition = d->m_horizontalHeader->sectionVisualPosition(nlastVisualCol);

        if (nLastVisualColPosition + columnWidth(nlastVisualCol) > nViewPortWidth)
        {
            nlastVisualCol = nlastVisualCol - 1;
        }
    }
    else
    {
        if (firstVisualCol() != -1)
        {
            nlastVisualCol = d->m_horizontalHeader->count() - 1;
        }
    }

    return nlastVisualCol;
}

GIntList GLDTableView::needCalcSuitRowHeightColsInViewport()
{
    Q_D(GLDTableView);

    QSet<int> oNeedCalcHeightCols;
    for (int i = 0; i < d->m_nFixedColCount; ++i)
    {
        if (d->m_oSuitRowHeightAccordingCols.contains(i))
        {
            oNeedCalcHeightCols.insert(i);
        }
    }

    int nLeft;
    int nRight;

    if (d->m_bCalcAllColumns)
    {
        nLeft = 0;
        nRight = model()->columnCount();
    }
    else
    {
        nLeft = firstVisualCol();
        nRight = lastVisualCol();

        adjustVisualCol(nLeft, nRight);
    }

    for (int i = nLeft; i <= nRight; ++i)
    {
        if (d->m_oSuitRowHeightAccordingCols.contains(i))
        {
            oNeedCalcHeightCols.insert(i);
        }
    }
    return oNeedCalcHeightCols.toList();
}

QModelIndex GLDTableView::topLeftIndex(const QRect &rowColRect) const
{
    Q_D(const GLDTableView);
    return d->m_model->index(rowColRect.top(), rowColRect.left(), d->m_root);
}

QModelIndex GLDTableView::bottomRightIndex(const QRect &rowColRect) const
{
    Q_D(const GLDTableView);
    return d->m_model->index(rowColRect.bottom(), rowColRect.right(), d->m_root);
}

void GLDTableView::bindMergeCell()
{
    Q_D(GLDTableView);

    QAbstractItemModel *pDataModel = itemModel();
    if ((pDataModel == NULL) || (pDataModel->rowCount() == 0))
    {
        return;
    }

    clearSpans();

    int nFirstVisualCol = firstVisualCol();
    int nLastVisualCol = lastVisualCol();
    int nFirstVisualRow = firstVisualRow();
    int nLastVisualRow = lastVisualRow();

    // 把可见行的区域都扩大，防止，出现在计算过程中，出现本来是合并个却因这一行或这一列未完全显示，导致没有计算情况
    adjustVisualRect(nFirstVisualRow, nFirstVisualCol, nLastVisualRow, nLastVisualCol);
    if (nFirstVisualRow == -1 || nLastVisualRow == -1 || nFirstVisualCol == -1 || nLastVisualCol == -1)
    {
        return;
    }

    // 对于可编辑固定行和可编辑固定列，需特殊处理，因此，不支持一个合并格跨了固定区域和非固定区域
    calcAndSetSpan(nFirstVisualRow, nLastVisualRow, 0, d->m_nFixedColCount - 1);
    calcAndSetSpan(0, d->m_nFixedRowCount - 1, nFirstVisualCol, nLastVisualCol);
    calcAndSetSpan(0, d->m_nFixedRowCount - 1, 0, d->m_nFixedColCount - 1);

    calcAndSetSpan(nFirstVisualRow, nLastVisualRow, nFirstVisualCol, nLastVisualCol);
}

void GLDTableView::afterCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    // do nothing
}

void GLDTableView::afterExpandedChanged(int index, bool expand, bool emitSignal)
{
    if (emitSignal)
    {
        if (expand)
        {
            emit expanded(index);
        }
        else
        {
            emit collapsed(index);
        }
    }
}

void GLDTableView::afterManualChangedRowHeight(int index, int rowHeight)
{
    G_UNUSED(index);
    G_UNUSED(rowHeight);
}

void GLDTableView::afterManualChangedColWidth(int index, int colWidth)
{
    G_UNUSED(index);
    G_UNUSED(colWidth);
}

void GLDTableView::selectionChanged(
        const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_D(GLDTableView);
    G_UNUSED(selected);

    if (d->m_alwaysShowRowSelectedColor && isVisible() && updatesEnabled())
    {
        d->viewport->update();
    }
    else
    {
        if (isVisible() && updatesEnabled())
        {
            // 按着ctrl键进行选择时，需要把原来的选择区域的边框线刷掉，所以此处不能在用selected参数
            d->viewport->update(visualRegionForSelectionWithSelectionBounding(deselected)
                        | visualRegionForSelectionWithSelectionBounding(d->m_selectionModel.data()->selection()));
        }
    }
}

QRect GLDTableView::initRangeFillingDest()
{
    QRect oRangeFillingRect = m_oRangeSrc;
    m_oRangeDest = m_oRangeSrc;

    // 向上拖拽
    if (m_nRangeCurRow < m_oRangeSrc.top())
    {
        oRangeFillingRect.setTop(m_nRangeCurRow);
        m_oRangeDest.setTop(m_nRangeCurRow);
        m_oRangeDest.setBottom(m_oRangeSrc.top() - 1);
    }
    // 向下拖拽
    else if (m_nRangeCurRow > m_oRangeSrc.bottom())
    {
        oRangeFillingRect.setBottom(m_nRangeCurRow);
        m_oRangeDest.setBottom(m_nRangeCurRow);
        m_oRangeDest.setTop(m_oRangeSrc.bottom() + 1);
    }
    // 向左拖拽
    else if (m_nRangeCurCol < m_oRangeSrc.left())
    {
        oRangeFillingRect.setLeft(m_nRangeCurCol);
        m_oRangeDest.setLeft(m_nRangeCurCol);
        m_oRangeDest.setRight(m_oRangeSrc.left() - 1);
    }
    // 向右拖拽
    else if (m_nRangeCurCol > m_oRangeSrc.right())
    {
        oRangeFillingRect.setRight(m_nRangeCurCol);
        m_oRangeDest.setRight(m_nRangeCurCol);
        m_oRangeDest.setLeft(m_oRangeSrc.right() + 1);
    }

    return oRangeFillingRect;
}

bool GLDTableView::inRangeMovingRegion(const QPoint &p)
{
    Q_D(GLDTableView);

    if (d->isInMutiSelect())//如果处于多选， 则不允许移动
    {
        return false;
    }

    QRect srcRect = visualRectForRowColNo(visualRectForSelection());

    if (srcRect.isValid())
    {
        QRect bigRect = srcRect;
        bigRect.adjust(-1, -1, 1, 1);
        QRect smallRect = srcRect;
        smallRect.adjust(3, 3, -3, -3);
        QRect rangeMovingHandleRect(QPoint(bigRect.right() - 5, bigRect.bottom() - 5), QSize(5, 5));
        return bigRect.contains(p) && !smallRect.contains(p) && !rangeMovingHandleRect.contains(p);
    }

    return false;
}

bool GLDTableView::initRangeMovingDest()
{
    Q_D(GLDTableView);
    int noffsetRow = 0;
    int noffsetCol = 0;
    m_oRangeDest = m_oRangeSrc;

    if (m_nRangeCurRow < m_oRangeSrc.top())
    {
        noffsetRow = m_nRangeCurRow - m_oRangeSrc.top();
        m_oRangeDest.setTop(m_nRangeCurRow);
        m_oRangeDest.setBottom(m_oRangeSrc.bottom() + noffsetRow);
    }
    else if (m_nRangeCurRow >= m_oRangeSrc.bottom())
    {
        noffsetRow = m_nRangeCurRow - m_oRangeSrc.bottom();
        m_oRangeDest.setTop(noffsetRow + m_oRangeSrc.top());
        m_oRangeDest.setBottom(m_nRangeCurRow);
    }

    if (m_nRangeCurCol < m_oRangeSrc.left())
    {
        noffsetCol = m_nRangeCurCol - m_oRangeSrc.left();
        m_oRangeDest.setLeft(m_nRangeCurCol);
        m_oRangeDest.setRight(m_oRangeSrc.right() + noffsetCol);
    }
    else if (m_nRangeCurCol > m_oRangeSrc.right())
    {
        noffsetCol = m_nRangeCurCol - m_oRangeSrc.right();
        m_oRangeDest.setLeft(m_oRangeSrc.left() + noffsetCol);
        m_oRangeDest.setRight(m_nRangeCurCol);
    }

    bool bcanRangeMoving = true;

    for (int row = m_oRangeSrc.top(); row <= m_oRangeSrc.bottom(); ++row)
    {
        for (int col = m_oRangeSrc.left(); col <= m_oRangeSrc.right(); ++col)
        {
            GSpanCollection::GSpan *srcSpan = d->m_spans.spanAt(col, row);
            GSpanCollection::GSpan *destSpan = d->m_spans.spanAt(col + noffsetCol, row + noffsetRow);

            //当src和dest都存在合并个且合并格的高度宽度相等时，或则都不含有合并个时，允许选择移动,此外其他情况都不允许
            if (((NULL != srcSpan) && (NULL != destSpan) && (srcSpan->width() == destSpan->width())
                    && (srcSpan->height() == destSpan->height())) || (NULL == srcSpan && NULL == destSpan))
            {
                continue;
            }
            else
            {
                bcanRangeMoving = false;
            }
        }
    }

    return bcanRangeMoving;
}

QRect GLDTableView::visualRectForSelection()
{
    Q_D(GLDTableView);
    if (!d->m_oTopLeftSelectIndex.isValid() || !d->m_oBottomRightSelectIndex.isValid())
        return QRect();

    int nTop = d->m_oTopLeftSelectIndex.row();
    int nLeft = d->m_oTopLeftSelectIndex.column();
    int nBottom = d->m_oBottomRightSelectIndex.row();
    int nRight = d->m_oBottomRightSelectIndex.column();

    return QRect(nLeft, nTop, nRight - nLeft + 1, nBottom - nTop + 1);
}

void GLDTableView::initRangeAction()
{
    Q_D(GLDTableView);
    QItemSelection selection = d->m_selectionModel.data()->selection();

    if (selection.count() > 0)
    {
        m_oRangeSrc = visualRectForSelection();
        m_nRangeCurRow = m_oRangeSrc.bottom();
        m_nRangeCurCol = m_oRangeSrc.right();
    }
}

QRect GLDTableView::calcRangeMovingRegion()
{
    Q_D(GLDTableView);

    const QPoint c_offset = d->m_scrollDelayOffset;

    int nSrcTop = rowVisualPosition(m_oRangeSrc.top()) + c_offset.y();
    int nSrcBottom = rowVisualPosition(m_oRangeSrc.bottom()) + c_offset.y() + visualRowHeight(m_oRangeSrc.bottom());
    int nSrcLeft = columnVisualPosition(m_oRangeSrc.left()) + c_offset.x();
    int nSrcRight = columnVisualPosition(m_oRangeSrc.right()) + c_offset.x() + visualColumnWidth(m_oRangeSrc.right());

    int nDestTop = nSrcTop;
    int nDestBottom = nSrcBottom;
    int nDestLeft = nSrcLeft;
    int nDestRight = nSrcRight;

    // CurrentRow和Col是指离原始位置最远的那个格子
    // 向上拖拽
    if (m_nRangeCurRow < m_oRangeSrc.top())
    {
        int nDestBottomRow = m_nRangeCurRow + m_oRangeSrc.height() - 1;
        nDestTop = rowVisualPosition(m_nRangeCurRow) + c_offset.y();
        nDestBottom = rowVisualPosition(nDestBottomRow) + visualRowHeight(nDestBottomRow) + c_offset.y();
    }
    // 向下拖拽
    else if (m_nRangeCurRow >= m_oRangeSrc.bottom())
    {
        int nDestTopRow = m_nRangeCurRow - m_oRangeSrc.height() + 1;
        nDestTop = rowVisualPosition(nDestTopRow) + c_offset.y();
        nDestBottom = rowVisualPosition(m_nRangeCurRow) + visualRowHeight(m_nRangeCurRow) + c_offset.y();
    }

    // 向左拖拽
    if (m_nRangeCurCol < m_oRangeSrc.left())
    {
        int nDestRightCol = m_nRangeCurCol + m_oRangeSrc.width() - 1;
        nDestLeft = columnVisualPosition(m_nRangeCurCol) + c_offset.x();
        nDestRight = columnVisualPosition(nDestRightCol) + visualColumnWidth(nDestRightCol) + c_offset.x();
    }
    // 向右拖拽
    else if (m_nRangeCurCol > m_oRangeSrc.right())
    {
        int nDestLeftCol = m_nRangeCurCol - m_oRangeSrc.width() + 1;
        nDestLeft = columnVisualPosition(nDestLeftCol) + c_offset.x();
        nDestRight = columnVisualPosition(m_nRangeCurCol) + visualColumnWidth(m_nRangeCurCol) + c_offset.x();
    }

    return QRect(QPoint(nDestLeft, nDestTop), QPoint(nDestRight, nDestBottom));
}

void GLDTableView::setCurRangeRowAndCol(QPoint pos)
{
    Q_D(GLDTableView);

    m_nRangeCurRow = visualRowAt(pos.y());
    if (m_nRangeCurRow == -1)
    {
        if (pos.y() < 0)
        {
            m_nRangeCurRow = 0;
        }
        else
        {
            m_nRangeCurRow = d->m_verticalHeader->count() - 1;
        }
    }

    m_nRangeCurCol = visualColumnAt(pos.x());
    if (m_nRangeCurCol == -1)
    {
        if (pos.x() < 0)
        {
            m_nRangeCurCol = 0;
        }
        else
        {
            m_nRangeCurCol = d->m_horizontalHeader->count() - 1;
        }
    }
}

void GLDTableView::addNewRow(int newRowCount)
{
    Q_D(GLDTableView);
    QModelIndexList indexList = selectionModel()->selectedIndexes();
    QModelIndex index1 = indexList.at(0);
    QModelIndex index2 = indexList.at(indexList.size() - 1);

    d->m_model->insertRows(qMax(index1.row(), index2.row()), newRowCount);
}

QString GLDTableView::resizeInfoText(QPoint mousePosition, Qt::Orientation direction)
{
    Q_D(GLDTableView);

    QString sInfoText = "";

    const QString sWidth = tr("width:");
    const QString sHeight = tr("height:");
    const QString sPixel = tr("pixel(");
    const QString sMilliMetre = tr("millimetre)");

    if (direction == Qt::Horizontal)
    {
        int nResizeOffset = mousePosition.x() - d->m_nResizeCellStartPosition;
        //通过logicalDpiX获取dpi,把像素转换为厘米
        float fMillimetre = nResizeOffset * 1.0 / logicalDpiX() * 25.4;
        sInfoText = sWidth + QString::number(nResizeOffset, 10) + sPixel
              + QString::number(fMillimetre, 'f', 1) + sMilliMetre;
    }
    else
    {
        int nResizeOffset = mousePosition.y() - d->m_nResizeCellStartPosition;
        float fMillimetre = nResizeOffset * 1.0 / logicalDpiX() * 25.4;
        sInfoText = sHeight + QString::number(nResizeOffset, 10) + sPixel
              + QString::number(fMillimetre, 'f', 1) + sMilliMetre;
    }

    return sInfoText;
}

void GLDTableView::setResizeStartPosition(Qt::Orientation direction)
{
    Q_D(GLDTableView);

    int nResizeIndex;
    if (direction == Qt::Horizontal)
    {
        int nBeforeIndex = d->m_horizontalHeader->visualIndexAt(d->m_horizontalHeader->mousePosition().x() - c_nResizeHandlerOffset);
        int nAfterIndex = d->m_horizontalHeader->visualIndexAt(d->m_horizontalHeader->mousePosition().x() + c_nResizeHandlerOffset);

        if (nAfterIndex == -1)
        {
            nAfterIndex = nBeforeIndex;
        }

        nResizeIndex = qMin(nBeforeIndex, nAfterIndex);
        d->m_nResizeCellStartPosition = d->m_horizontalHeader->sectionViewportPosition(nResizeIndex);
        d->m_nResizeCellEndPosition = d->m_nResizeCellStartPosition + columnWidth(nResizeIndex);
    }
    else
    {
        int nBeforeIndex = d->m_verticalHeader->visualIndexAt(d->m_verticalHeader->mousePosition().y() - c_nResizeHandlerOffset);
        int nAfterIndex = d->m_verticalHeader->visualIndexAt(d->m_verticalHeader->mousePosition().y() + c_nResizeHandlerOffset);

        if (nAfterIndex == -1)
        {
            nAfterIndex = nBeforeIndex;
        }

        nResizeIndex = qMin(nBeforeIndex, nAfterIndex);
        d->m_nResizeCellStartPosition = d->m_verticalHeader->sectionViewportPosition(nResizeIndex);
        d->m_nResizeCellEndPosition = d->m_nResizeCellStartPosition + rowHeight(nResizeIndex);
    }
}

void GLDTableView::doRangeFill()
{
    if (doRangeFillHandled())
    {
        return;
    }

    // 向上拖拽
    if (m_nRangeCurRow < m_oRangeSrc.top())
    {
        doRangeFillToTop();
    }
    // 向下拖拽
    else if (m_nRangeCurRow > m_oRangeSrc.bottom())
    {
        doRangeFillToBottom();
    }
    // 向左拖拽
    else if (m_nRangeCurCol < m_oRangeSrc.left())
    {
        doRangeFillToLeft();
    }
    // 向右拖拽
    else if (m_nRangeCurCol > m_oRangeSrc.right())
    {
        doRangeFillToRight();
    }
}

void GLDTableView::doRangeMoving(QRect src, QRect dest)
{
    Q_D(GLDTableView);
    bool bIsSetCurrentIndex = false;

    for (int sc = src.left(), dc = dest.left(); sc <= src.right() && dc <= dest.right(); ++sc, ++dc)
    {
        for (int sr = src.top(), dr = dest.top(); sr <= src.bottom() && dr <= dest.bottom(); ++sr, ++dr)
        {
            QModelIndex scurr = logicalIndex(d->m_model->index(sr, sc));
            QModelIndex dcurr = logicalIndex(d->m_model->index(dr, dc));

            if (scurr == dcurr)
            {
                continue;
            }

            if (!bIsSetCurrentIndex && scurr == currentIndex())
            {
                bIsSetCurrentIndex = true;
                setCurrentIndex(dataIndex(dcurr));
            }

            d->m_model->setData(dcurr, d->m_model->data(scurr));
            d->m_model->setData(scurr, QVariant());
        }
    }
}

// todo litz-a 此方法可能会在拖拽填充功能中给外部使用，暂不删除，后期修改易用性时再做处理
QList<QModelIndex> GLDTableView::rangeToList(QRect rect)
{
    Q_D(GLDTableView);

    QList<QModelIndex> modelIndexList;

    for (int i = rect.top(); i <= rect.bottom(); ++i)
    {
        for (int j = rect.left(); j <= rect.right(); ++j)
        {
            modelIndexList.append((d->m_model->index(i, j)));
        }
    }

    return modelIndexList;
}

void GLDTableView::setGridDisplayColWidths()
{
    Q_D(GLDTableView);

    if (d->m_oSuitColWidthCols.size() == 0)
    {
        return;
    }

    bool bShouldResetCommentPosition = false;

    d->calcFixedColSuitWidth(bShouldResetCommentPosition);
    d->calcNormalColSuitWidth(bShouldResetCommentPosition);

    if (d->m_columnResizeTimerID == 0)
    {
        d->m_columnResizeTimerID = startTimer(0);
    }

    if (bShouldResetCommentPosition)
    {
        resetCommentPosition();
    }
}

void GLDTableView::refreshDisplayRows()
{
    GLDList<QModelIndex> arrRows;
    refreshDisplayRows(arrRows);
    rebuildGridRows(arrRows);
}

void GLDTableView::refreshDisplayColRow()
{
    if (!isVisible())
    {
        return;
    }

    bindMergeCell();

    // 自动列宽可能会导致可见列的变化，影响自动行高计算
    setGridDisplayColWidths();

    setGridDisplayRowHeights();

    // 合适列宽影响最大，先计算合适列宽
    adjustColWidths(-1);

    refreshDisplayRows();
    resetEllipsisButtonLocation();
}

void GLDTableView::refreshDisplayRows(GLDList<QModelIndex> &arrRows)
{
    Q_D(GLDTableView);

    int nFirstVisualRow = firstVisualRow();
    int nLastVisualRow = lastVisualRow();

    if (NULL == model() || nLastVisualRow == -1)
    {
        return;
    }

    for (int i = 0; i < d->m_nFixedRowCount; ++i)
    {
        if (isRowHidden(i))
        {
            continue;
        }

        QModelIndex modelIndex = model()->index(i, 0);
        arrRows.append(dataIndex(modelIndex));
    }

    for (int i = nFirstVisualRow; i <= nLastVisualRow; ++i)
    {
        if (isRowHidden(i))
        {
            continue;
        }

        QModelIndex modelIndex = model()->index(i, 0);
        arrRows.append(dataIndex(modelIndex));
    }
}

void GLDTableView::rebuildGridRows(const GLDList<QModelIndex> &arrRows)
{
    // donothing
    G_UNUSED(arrRows)
}

int GLDTableView::treeCellDisplayHorizontalOffset(int row, int col, bool isOldMinTextHeightCalWay) const
{
    G_UNUSED(isOldMinTextHeightCalWay);
    G_UNUSED(row);
    G_UNUSED(col);
    return 0;
}

void GLDTableView::doMoveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
                                 QItemSelectionModel::SelectionFlags command,
                                 MoveReason moveReason)
{
    Q_D(GLDTableView);
    GlodonAbstractItemView::doMoveCurrent(oldIndex, newIndex, command, moveReason);

    if (newIndex.isValid() && d->hasSpans())
    {
        GSpanCollection::GSpan oSpan = d->span(newIndex.row(), newIndex.column());
        QModelIndex topLeftIndex = newIndex.model()->index(oSpan.top(), oSpan.left(), newIndex.parent());
        QModelIndex bottomRightIndex = newIndex.model()->index(oSpan.bottom(), oSpan.right(), newIndex.parent());
        QItemSelection itemSelection(topLeftIndex, bottomRightIndex);
        d->m_selectionModel->select(itemSelection, QItemSelectionModel::SelectCurrent);
    }
}

void GLDTableView::setGridDisplayRowHeights()
{
    Q_D(GLDTableView);

    if (d->m_oSuitRowHeightAccordingCols.size() == 0)
        return;

    QList<int> oNeedCalcHeightCols = needCalcSuitRowHeightColsInViewport();
    if (oNeedCalcHeightCols.count() == 0)
        return;

    bool bShouldResetCommentPosition = false;

    d->calcFixedRowSuitHeight(oNeedCalcHeightCols, bShouldResetCommentPosition);
    d->calcNormalRowSuitHeight(oNeedCalcHeightCols, bShouldResetCommentPosition);

    if (d->m_rowResizeTimerID == 0)
    {
        d->m_rowResizeTimerID = startTimer(0);
    }

    //滚动条滚动的时候，会触发自动行高，但是这时候，如果发生行号的改变，会错误重置备注框的位置
    if (bShouldResetCommentPosition)
    {
        resetCommentPosition();
    }
}

// col不为-1时，说明是在调整列宽
void GLDTableView::adjustColWidths(int currentResizeCol)
{
    Q_D(GLDTableView);

    if (d->m_oFitColWidthCols.size() == 0)
    {
        return;
    }

    d->m_bIsInAdjustFitCols = true;

    d->doAdjustColWidths(currentResizeCol);

    d->m_bIsInAdjustFitCols = false;

    resetCommentPosition();
}

int GLDTableView::cellMargin(const QModelIndex &modelIndex, Margin margin) const
{
    switch (margin)
    {
        case TopMargin:
        {
            return (model()->data(modelIndex, gidrCellMargin).toInt() & 0x000000FF);
        }

        case BottomMargin:
        {
            return (model()->data(modelIndex, gidrCellMargin).toInt() & 0x0000FF00) >> 8;
        }

        case LeftMargin:
        {
            return (model()->data(modelIndex, gidrCellMargin).toInt() & 0x00FF0000) >> 16;
        }

        case RightMargin:
        {
            return (model()->data(modelIndex, gidrCellMargin).toInt() & 0xFF000000) >> 24;
        }

        default:
            return 0;
    }
}

void GLDTableView::resetEllipsisButtonLocation()
{
    Q_D(GLDTableView);

    if (d->m_showEllipsisButton && NULL != d->m_ellipsisButton)
    {
        int nX = d->m_horizontalHeader->length() - d->m_horizontalHeader->offset();
        int nY = d->m_verticalHeader->length() - d->m_verticalHeader->offset();

        nX = nX - d->m_ellipsisButton->width() + d->m_verticalHeader->drawWidth() + 2;
        nY = nY + d->m_horizontalHeader->drawWidth();
        d->m_ellipsisButton->move(nX, nY);
    }
}

GCustomCommentFrame *GLDTableView::findOrCreateCommentFrame(const QModelIndex &index, bool createCommentIfNotFind)
{
    Q_D(GLDTableView);
    GCustomCommentFrame *commentFrame = d->m_oCommentFrames.value(index);

    if (NULL == commentFrame && createCommentIfNotFind)
    {
        commentFrame = new GCustomCommentFrame(viewport(), visualRect(index).topRight());

        connect(commentFrame, SIGNAL(textChanged(QString)), this, SLOT(writeComment(QString)));
        d->m_oCommentFrames.insert(index, commentFrame);
    }

    return commentFrame;
}

int GLDTableView::borderLineWidth(const QVariant& borderLine, bool isRightBorderLine) const
{
    int nBorderLineWidth = currentGridLineWidth();

    if (borderLine.isValid())
    {
        typedef GBorderLineWidthHelper BorderLine;
        BorderLine brdLine;
        brdLine.widths = borderLine.toInt();

        if (isRightBorderLine)
        {
            nBorderLineWidth = brdLine.lineWiths[BorderLine::right];
        }
        else
        {
            nBorderLineWidth = brdLine.lineWiths[BorderLine::bottom];
        }

        return nBorderLineWidth;
    }

    return nBorderLineWidth;
}

int GLDTableView::borderLineSubtractGridLineWidth(const QVariant &boundLine, bool isRightBoundLine) const
{
    Q_D(const GLDTableView);

    if (!d->m_bDrawBoundLine)
        return 0;

    int nBorderLineWidth = borderLineWidth(boundLine, isRightBoundLine);
    int nGridLineWidth = currentGridLineWidth();

    // 如果边框线宽度大于格线宽度时，计算边框线的宽度为 边框线宽度 - 格线宽度
    if (d->m_bShowVerticalGridLine && isRightBoundLine)
    {
        if (nBorderLineWidth >= nGridLineWidth)
        {
            nBorderLineWidth = nBorderLineWidth - nGridLineWidth;
        }
    }
    else if (d->m_bShowHorizontalGridLine && !isRightBoundLine)
    {
        if (nBorderLineWidth >= nGridLineWidth)
        {
            nBorderLineWidth = nBorderLineWidth - nGridLineWidth;
        }
    }

    return nBorderLineWidth;
}

void GLDTableView::_q_selectRow(int row)
{
    d_func()->_q_selectRow(row);
}

void GLDTableView::_q_selectColumn(int column)
{
    d_func()->_q_selectColumn(column);
}

void GLDTableView::_q_selectRows(int left, int top, int right, int bottom)
{
    d_func()->_q_selectRows(left, top, right, bottom);
}

void GLDTableView::_q_selectColumns(int left, int top, int right, int bottom)
{
    d_func()->_q_selectColumns(left, top, right, bottom);
}

void GLDTableView::_q_updateSpanInsertedRows(const QModelIndex &parent, int start, int end)
{
    d_func()->_q_updateSpanInsertedRows(parent, start, end);
    resetEllipsisButtonLocation();
    resetCommentPosition();
}

void GLDTableView::_q_updateSpanInsertedColumns(const QModelIndex &parent, int start, int end)
{
    d_func()->_q_updateSpanInsertedColumns(parent, start, end);
    resetEllipsisButtonLocation();
    resetCommentPosition();
}

void GLDTableView::_q_updateSpanRemovedRows(const QModelIndex &parent, int start, int end)
{
    d_func()->_q_updateSpanRemovedRows(parent, start, end);
    resetEllipsisButtonLocation();
    resetCommentPosition();
}

void GLDTableView::_q_updateSpanRemovedColumns(const QModelIndex &parent, int start, int end)
{
    d_func()->_q_updateSpanRemovedColumns(parent, start, end);
    resetEllipsisButtonLocation();
//    resetCommentPosition();
}

void GLDTableView::writeComment(const QString &value)
{
    Q_D(GLDTableView);
    GCustomCommentFrame *pCurrentFrame = dynamic_cast<GCustomCommentFrame *>(QObject::sender());
    d->m_model->setData(d->m_oCommentFrames.key(pCurrentFrame), value, gidrCommentRole);
}

void GLDTableView::onDrawComment(const QModelIndex &index, bool &canShow)
{
    GCustomCommentFrame *pCommentFrame = findOrCreateCommentFrame(index, false);

    if (NULL != pCommentFrame && pCommentFrame->isVisible())
    {
        canShow = false;
    }
}

void GLDTableView::setCommentsUpdatesEnabled(bool enable)
{
    Q_D(GLDTableView);
    QMap<QPersistentModelIndex, GCustomCommentFrame *>::iterator it = d->m_oCommentFrames.begin();

    while (it != d->m_oCommentFrames.end())
    {
        it.value()->setUpdatesEnabled(enable);
        ++it;
    }

    if (enable)
    {
        resetCommentPosition();
    }
}

bool GLDTableView::closeEditorOnFocusOut()
{
    Q_D(GLDTableView);
    return d->m_bCloseEditorOnFocusOut;
}

void GLDTableView::setCloseEditorOnFocusOut(bool value, bool ignoreActiveWindowFocusReason)
{
    Q_D(GLDTableView);

    if (d->m_bCloseEditorOnFocusOut != value)
    {
        d->m_itemDelegate.data()->setCloseEditorOnFocusOut(value, ignoreActiveWindowFocusReason);
        d->m_bCloseEditorOnFocusOut = value;
        d->m_ignoreActiveWindowFocusReason = ignoreActiveWindowFocusReason;
    }
}

void GLDTableView::setUseComboBoxCompleter(bool value)
{
    Q_D(GLDTableView);

    if (d->m_bUseComboBoxCompleter != value)
    {
        d->m_itemDelegate.data()->setUseComboBoxCompleter(value);
        d->m_bUseComboBoxCompleter = value;
    }
}

bool GLDTableView::useComboBoxCompleter() const
{
    Q_D(const GLDTableView);
    return d->m_bUseComboBoxCompleter;
}

void GLDTableView::resetCommentPosition(bool isMove, int dx, int dy)
{
    Q_D(GLDTableView);

    if (d->m_bIsInReset)
        return;

    QMap<QPersistentModelIndex, GCustomCommentFrame *>::iterator it = d->m_oCommentFrames.begin();

    while (it != d->m_oCommentFrames.end())
    {
        if (isMove)
        {
            it.value()->move(it.value()->pos().x() + dx, it.value()->pos().y() + dy);
        }
        else
        {
            QPoint point = visualRect(it.key()).topRight();
            it.value()->moveTo(point);
        }

        ++it;
    }
}

void GLDTableView::setResizeDelay(bool delay)
{
    Q_D(GLDTableView);
    d->m_verticalHeader->setResizeDelay(delay);
    d->m_horizontalHeader->setResizeDelay(delay);
}

void GLDTableView::setAllowRangeMoving(bool value)
{
    Q_D(GLDTableView);

    d->m_bRangeMoving = value;
}

bool GLDTableView::allowRangeMoving() const
{
    Q_D(const GLDTableView);
    return d->m_bRangeMoving;
}

QModelIndex GLDTableView::logicalIndex(QModelIndex visualIndex)
{
    Q_D(GLDTableView);
    return d->m_model->index(d->logicalRow(visualIndex.row()),
                             d->logicalColumn(visualIndex.column()), visualIndex.parent());
}

QModelIndex GLDTableView::visualIndex(const QModelIndex &logicalIndex)
{
    Q_D(GLDTableView);
    return d->m_model->index(d->visualRow(logicalIndex.row()),
                             d->visualColumn(logicalIndex.column()), logicalIndex.parent());
}

void GLDTableView::updateModelIndex(const QModelIndex &logicIndex)
{
    update(logicIndex);
}

void GLDTableView::update(const QModelIndex &logicalIndex)
{
    Q_D(GLDTableView);

    if (!logicalIndex.isValid())
    {
        return;
    }

    // 非编辑状态，显示当前行、当前列编辑方式，需要刷新整行整列
    QRect oUpdateRect;
    switch (d->m_editStyleDrawType)
    {

    case GlodonAbstractItemView::SdtCurrentRow:
        oUpdateRect = QRect(0,
                            d->m_verticalHeader->sectionViewportPosition(logicalIndex.row()),
                            viewport()->width(),
                            rowHeight(logicalIndex.row()));
        break;
    case GlodonAbstractItemView::SdtCurrentCol:
        oUpdateRect = QRect(d->m_horizontalHeader->sectionViewportPosition(logicalIndex.column()),
                            0,
                            columnWidth(logicalIndex.column()),
                            viewport()->width());
        break;

    case GlodonAbstractItemView::SdtAll:
        oUpdateRect = viewport()->rect();
        break;
    case GlodonAbstractItemView::SdtCurrentCell:
    default:
        oUpdateRect = visualRect(logicalIndex);
        break;
    }

    const QRect c_rect = oUpdateRect.adjusted(-3, -3, 3, 3);
    if (d->viewport->rect().intersects(c_rect))
    {
        d->viewport->update(c_rect);
    }
}

void GLDTableView::updateCol(int col)
{
    Q_D(GLDTableView);

    QRect oUpdateRect = QRect(columnVisualPosition(col), 0,
                              columnWidth(col), d->m_verticalHeader->height());
    QRect oAdjustRect = oUpdateRect.adjusted(-3, -3, 3, 3);
    d->viewport->update(oAdjustRect);

    if (GlodonMultiHeaderView *horizonHeader = dynamic_cast<GlodonMultiHeaderView *>(d->m_horizontalHeader))
    {
        horizonHeader->updateSpan(col);
    }
    else
    {
        d->m_horizontalHeader->updateSection(col);
    }
}

void GLDTableView::updateRow(int row)
{
    Q_D(GLDTableView);
    QRect oUpdateRect = QRect(0, rowVisualPosition(row),
                              d->m_horizontalHeader->width(), rowHeight(row));
    QRect oAdjustRect = oUpdateRect.adjusted(-3, -3, 3, 3);
    d->viewport->update(oAdjustRect);

    if (GlodonMultiHeaderView *verticalHeader = dynamic_cast<GlodonMultiHeaderView *>(d->m_verticalHeader))
    {
        verticalHeader->updateSpan(row);
    }
    else
    {
        d->m_verticalHeader->updateSection(row);
    }
}

void GLDTableView::updateAll()
{
    Q_D(GLDTableView);
    d->viewport->update();

    if (GlodonMultiHeaderView *verticalHeader = dynamic_cast<GlodonMultiHeaderView *>(d->m_verticalHeader))
    {
        verticalHeader->resetSpan();
    }
    else
    {
        d->m_verticalHeader->viewport()->update();
    }

    if (GlodonMultiHeaderView *horizonHeader = dynamic_cast<GlodonMultiHeaderView *>(d->m_horizontalHeader))
    {
        horizonHeader->resetSpan();
    }
    else
    {
        d->m_verticalHeader->viewport()->update();
    }
}

void GLDTableView::updateHeaderView(const UpdateHeaderViewType updateType)
{
    Q_D(GLDTableView);

    switch (updateType)
    {
        case uhVertical:
        {
            d->m_verticalHeader->viewport()->update();
            break;
        }

        case uhHorizontal:
        {
            d->m_horizontalHeader->viewport()->update();
            break;
        }

        case uhBoth:
        {
            d->m_verticalHeader->viewport()->update();
            d->m_horizontalHeader->viewport()->update();
            break;
        }

        default:
        {
        }
    }
}

QModelIndex GLDTableView::treeIndex(const QModelIndex &index) const
{
    return index;
}

QModelIndex GLDTableView::dataIndex(const QModelIndex &index) const
{
    return index;
}

GString GLDTableView::copyTextFromSelections(bool *bCanCopyText)
{
    Q_D(GLDTableView);

    if (!d->m_model || 0 == selectionModel()->selection().size())
    {
        return GString();
    }

    QModelIndexList indexList = selectionModel()->selectedIndexes();
    qSort(indexList.begin(), indexList.end());

    bool bCanCopy = checkSelectionCopyEnable(indexList);

    if (bCanCopyText)
    {
        bCanCopyText = &bCanCopy;
    }

    if (!bCanCopy)
    {
        return "";
    }

    GString result = getContentsFromOrderedIndexList(indexList);

    if (result.length() > 0)
    {
        QApplication::clipboard()->clear();
        QApplication::clipboard()->setText(result);
    }

    return result;
}

bool GLDTableView::pasteFromClipboard()
{
    Q_D(GLDTableView);

    if (!d->m_model || 1 != selectionModel()->selection().size())
    {
        return false;
    }

    GString sClipBoardText = QApplication::clipboard()->text();

    if (sClipBoardText.length() == 0)
    {
        return false;
    }

    QModelIndexList indexList = selectionModel()->selectedIndexes();
    qSort(indexList.begin(), indexList.end());

    int nPasteRowCount = 0;
    int nPasteColCount = 0;

    fillCellsOnClipBoardText(indexList, sClipBoardText, nPasteRowCount, nPasteColCount);

    setNewSelection(indexList, nPasteRowCount, nPasteColCount);

    return true;
}


GlodonScrollBar::GlodonScrollBar(QWidget *parent) : QScrollBar(parent),
    m_pParent(parent)
{

}

GlodonScrollBar::GlodonScrollBar(Qt::Orientation orientation, QWidget *parent)
    : QScrollBar(orientation, parent),
      m_pParent(parent)
{

}

void GlodonScrollBar::enterEvent(QEvent *event)
{
    QScrollBar::enterEvent(event);
    bool bHasCursor = m_pParent->testAttribute(Qt::WA_SetCursor);
    if (bHasCursor)
    {
        m_pParent->unsetCursor();
    }
}
