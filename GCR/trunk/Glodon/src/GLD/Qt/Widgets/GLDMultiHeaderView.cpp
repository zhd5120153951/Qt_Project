#include "GLDMultiHeaderView.h"
#include "GLDMultiHeaderView_p.h"
#include "GLDAbstractItemModel.h"
#include "GLDTableView.h"

GlodonMultiHeaderView::GlodonMultiHeaderView(Qt::Orientation orientation, QWidget *parent)
    : GlodonHeaderView(orientation, parent)
{
    headerData = new GHeaderSpanCollection();

    pressedHeaderSpan = -1;
    headerSpanIndicator = 0;
    headerSpanIndicatorOffset = 0;
    headerSpanIndicatorDrawPos = -1;
    moveStartSpan = -1;
}

GlodonMultiHeaderView::~GlodonMultiHeaderView()
{
    delete headerData;
}

void GlodonMultiHeaderView::resizeSection(int logicalIndex, int size, bool update, bool isManual)
{
    GlodonHeaderView::resizeSection(logicalIndex, size, update, isManual);

    for (int i = 0; i < headerList.count(); i++)
    {
        headerList[i]->resizeSection(logicalIndex, size, update, isManual);
    }

    updateSection(logicalIndex);
}

void GlodonMultiHeaderView::setModel(QAbstractItemModel *model)
{
    headerData->spans.clear();
    headerList.clear();
    GlodonHeaderView::setModel(model);
    initTitles();

    const int nCount = headerList.count();
    for (int i = 0; i < nCount; i++)
    {
        headerList.at(i)->setModel(model);
    }
}

int GlodonMultiHeaderView::extraCount() const
{
    return headerList.count();
}

void GlodonMultiHeaderView::setExtraCount(int count)
{
    Q_D(GlodonMultiHeaderView);
    headerList.clear();

    if (count == -1)
    {
        return;
    }

    headerList.resize(count);

    for (int i = 0; i < count; i++)
    {
        headerList[i] = new GlodonHeaderView(orientation(), this);
        headerList[i]->setHidden(false);

        if (d->m_model)
        {
            headerList[i]->setModel(d->m_model);
        }
    }
}

int GlodonMultiHeaderView::drawWidth() const
{
    int nwidth = 0;

    if (orientation() == Qt::Horizontal)
    {
        for (int i = 0; i < headerList.count(); i++)
        {
            if (headerList[i]->minimumHeight() == 0)
            {
                nwidth += headerList[i]->drawWidth();
            }
            else
            {
                nwidth += headerList[i]->minimumHeight();
            }
        }

        if (minimumHeight() == 0)
        {
            nwidth += GlodonHeaderView::drawWidth();
        }
        else
        {
            nwidth += minimumHeight();
        }
    }
    else
    {
        for (int i = 0; i < headerList.count(); i++)
        {
            nwidth += headerList[i]->drawWidth();
        }

        nwidth += GlodonHeaderView::drawWidth();
    }

    return nwidth;
}

void GlodonMultiHeaderView::updateGeometries()
{
    for (int i = 0; i < headerList.count(); i++)
    {
        headerList[i]->updateGeometries();
    }

    GlodonHeaderView::updateGeometries();
}

void GlodonMultiHeaderView::paintEvent(QPaintEvent *e)
{
    Q_D(GlodonMultiHeaderView);
    QPainter painter(d->viewport);
    const QPoint c_offset = d->m_scrollDelayOffset;
    QRect translatedEventRect = e->rect();
    translatedEventRect.translate(c_offset);

    int nstart = -1;
    int nend = -1;

    if (d->orientation == Qt::Horizontal)
    {
        nstart = visualIndexAt(translatedEventRect.left());
        nend = visualIndexAt(translatedEventRect.right());
    }
    else
    {
        nstart = visualIndexAt(translatedEventRect.top());
        nend = visualIndexAt(translatedEventRect.bottom());
    }

    int ntmp = nstart;
    nstart = qMin(nstart, nend);
    nend = qMax(ntmp, nend);

    QRect fixedRect(0, 0, 0, 0);
    QRect scrollRect = d->viewport->rect();

    if (d->fixedCount > 0)
    {
        if (d->orientation == Qt::Vertical)
        {
            int nheight = 0;

            for (int i = 0; i < d->fixedCount; i++)
            {
                nheight += sectionSize(i) + d->gridLineWidth;
            }

            fixedRect.adjust(0, 0, d->viewport->width(), nheight);
            scrollRect.adjust(0, nheight, 0, 0);
        }
        else
        {
            int nwidth = 0;

            for (int i = 0; i < d->fixedCount; i++)
            {
                nwidth += sectionSize(i) + d->gridLineWidth;
            }

            fixedRect.adjust(0, 0, nwidth, d->viewport->height());
            scrollRect.adjust(nwidth, 0, 0, 0);
        }

        painter.setClipRect(scrollRect);
    }

    QRect currentSectionRect(0, 0, 0, 0);

    for (int i = 0; i < headerData->spans.count(); i++)
    {
        painter.save();
        bool bPaint = true;
        currentSectionRect = headerSpanRect(i);

        if (d->orientation == Qt::Vertical)
        {
            if (headerData->spans[i]->bottom() >= d->fixedCount)
            {
                painter.setClipRect(scrollRect);

                if (currentSectionRect.bottom() <= fixedRect.bottom())
                {
                    bPaint = false;
                }
            }
            else
            {
                painter.setClipRect(fixedRect);
            }
        }
        else
        {
            if (headerData->spans[i]->right() >= d->fixedCount)
            {
                painter.setClipRect(scrollRect);

                if (currentSectionRect.right() <= fixedRect.right())
                {
                    bPaint = false;
                }
            }
            else
            {
                painter.setClipRect(fixedRect);
            }
        }

        if (bPaint)
        {
            paintSpanHeader(&painter, currentSectionRect, headerData->spans.at(i));
        }

        painter.restore();
    }
}

void GlodonMultiHeaderView::deselectedHeaderSpans()
{
    for (int i = 0; i < headerData->spans.count(); i++)
    {
        if (i == pressedHeaderSpan || inTopHeaderRange(pressedHeaderSpan, i))
        {
            continue;
        }

        if (headerData->spans[i]->selected())
        {
            headerData->spans[i]->setSelected(false);
            updateHeaderSpan(i);
        }
    }
}

void GlodonMultiHeaderView::mousePressEvent(QMouseEvent *e)
{
    Q_D(GlodonMultiHeaderView);

    d->m_isMouseDoubleClick = false;

    if (d->m_state != GlodonHeaderViewPrivate::NoState || e->button() != Qt::LeftButton)
    {
        return;
    }

    int npos = d->orientation == Qt::Horizontal ? e->x() : e->y();
    int nindex = headerSpanAt(e->pos());

    if (nindex != -1)
    {
        d->pressed = logicalIndexAt(npos);

        pressedHeaderSpan = nindex;

        if (d->clickableSections)
        {
            deselectedHeaderSpans();
            selectHeaderSpan(pressedHeaderSpan, true);

//            emit sectionPressed(d->pressed); //执行功能跟headerSpanPressed类似，暂去掉-By Huangp
            GLDHeaderSpan span = *headerData->spans[pressedHeaderSpan];
            emit headerSpanPressed(span.left(), span.top(), span.right(), span.bottom());
        }

        if (d->movableSections && headerData->spans.count() > nindex && headerData->spans[nindex]->selected())
        {
            moveStartSpan = pressedHeaderSpan;

            d->section = d->target = d->pressed;

            if (d->section == -1 || d->section < d->fixedCount)
            {
                return;
            }

            d->m_state = GlodonHeaderViewPrivate::MoveSection;
            setupHeaderSpanIndicator(npos);
        }
        else if (d->clickableSections && pressedHeaderSpan != -1)
        {
            d->m_state = GlodonHeaderViewPrivate::SelectSections;
        }
    }
    else
    {
        int nhandle = d->sectionHandleAt(npos);

        if ((resizeMode(nhandle) == Interactive) && (npos < length()))
        {
            d->m_mousePosition = e->pos();
            //当第一次按下时，获取的是鼠标所在屏幕的位置，方便tableview中的InfoFrame的正确移动
            QPoint point = e->screenPos().toPoint();
            emit sectionResizing(point, orientation(), Press);
            d->viewport->update(d->m_mousePosition.x(), 0, 1, height());

            d->originalSize = sectionSize(nhandle);
            d->m_state = GlodonHeaderViewPrivate::ResizeSection;
            d->section = nhandle;
        }
    }

    d->firstPos = npos;
    d->lastPos = npos;

    d->clearCascadingSections();
}

void GlodonMultiHeaderView::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(GlodonMultiHeaderView);
    int npos = d->orientation == Qt::Horizontal ? e->x() : e->y();

    if (npos < 0)
    {
        return;
    }

    if (e->buttons() == Qt::NoButton)
    {
        d->m_state = GlodonHeaderViewPrivate::NoState;
        pressedHeaderSpan = -1;
        d->pressed = -1;
    }

    switch (d->m_state)
    {
        case GlodonHeaderViewPrivate::ResizeSection:
        {
            Q_ASSERT(d->originalSize != -1);
            emit sectionResizing(d->m_mousePosition, orientation(), Move);
            d->m_mousePosition = e->pos();
            d->viewport->update();

            if (!d->resizeDelay)
            {
                if (d->cascadingResizing)
                {
                    int ndelta = d->reverse() ? d->lastPos - npos : npos - d->lastPos;
                    int nVisualIndex = visualIndex(d->section);
                    d->cascadingResize(nVisualIndex, d->headerSectionSize(nVisualIndex) + ndelta, true);
                }
                else
                {
                    int ndelta = d->reverse() ? d->firstPos - npos : npos - d->firstPos;
                    resizeSection(d->section, qMax(d->originalSize + ndelta, minimumSectionSize()), true, true);
                }
            }

            d->lastPos = npos;
            return;
        }

        case GlodonHeaderViewPrivate::MoveSection:
        {
            if (qAbs(npos - d->firstPos) >= QApplication::startDragDistance()
                    || !headerSpanIndicator->isHidden())
            {
                int nVisualIndex = visualIndexAt(npos);

                if (nVisualIndex == -1)
                {
                    return;
                }

                int nposThreshold = sectionViewportPosition(logicalIndexAt(npos)) +
                                    d->headerSectionSize(nVisualIndex) / 2;
                int nmoving = visualIndex(d->section);

                if (nVisualIndex < nmoving)
                {
                    if (npos < nposThreshold)
                    {
                        d->target = d->logicalIndex(nVisualIndex);
                    }
                    else
                    {
                        d->target = d->logicalIndex(nVisualIndex + 1);
                    }
                }
                else if (nVisualIndex > nmoving)
                {
                    if (npos > nposThreshold)
                    {
                        d->target = d->logicalIndex(nVisualIndex);
                    }
                    else
                    {
                        d->target = d->logicalIndex(nVisualIndex - 1);
                    }
                }
                else
                {
                    d->target = d->section;
                }

                updateHeaderSpanIndicator(npos);

                bool bhasCursor = testAttribute(Qt::WA_SetCursor);

                if (!checkCanMove(d->target))
                {
                    if (!bhasCursor)
                    {
                        setCursor(Qt::ForbiddenCursor);
                    }
                }
                else if (bhasCursor)
                {
                    unsetCursor();
                }
            }

            return;
        }

        case GlodonHeaderViewPrivate::SelectSections:
        {
            int nlogical = logicalIndexAt(npos);

            if (nlogical == d->pressed)
            {
                return;
            }
            else if (d->pressed != -1)
            {
                updateSection(d->pressed);
            }

            d->pressed = nlogical;

            if (d->clickableSections && nlogical != -1)
            {
                emit sectionEntered(d->pressed);

                if (d->orientation == Qt::Horizontal)
                {
                    int nindex = headerData->spanAt(extraCount(), d->pressed);
                    selectHeaderSpan(nindex, d->isColumnSelected(d->pressed));
                    updateHeaderSpan(nindex);
                }
                else
                {
                    int nindex = headerData->spanAt(d->pressed, extraCount());
                    selectHeaderSpan(nindex, d->isRowSelected(d->pressed));
                    updateHeaderSpan(nindex);
                }
            }

            return;
        }

        case GlodonHeaderViewPrivate::NoState:
        {
            int nsectionhandle = -1;
            int nhandle = headerSpanAt(e->pos());

            if (nhandle == -1)
            {
                nsectionhandle = d->sectionHandleAt(npos);
            }

            bool bhasCursor = testAttribute(Qt::WA_SetCursor);

            if (nsectionhandle != -1 && (resizeMode(nsectionhandle) == Interactive))
            {
                if (!bhasCursor)
                {
                    setCursor(d->orientation == Qt::Horizontal ? Qt::SplitHCursor : Qt::SplitVCursor);
                }
            }
            else if (bhasCursor)
            {
                unsetCursor();
            }

            return;
        }

        default:
            break;
    }
}

void GlodonMultiHeaderView::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(GlodonMultiHeaderView);
    int npos = d->orientation == Qt::Horizontal ? e->x() : e->y();

    switch (d->m_state)
    {
        case GlodonHeaderViewPrivate::MoveSection:
        {
            if (checkCanMove(d->target)
                    && !headerSpanIndicator->isHidden())
            {
                int ntargetSection = visualIndex(d->target);
                int nmovingSectoin = visualIndex(d->section);
                GLDHeaderSpan visualHeaderSpan = visualSpan(moveStartSpan);

                bool bcanMove = true;

                int ntargetSpanIndex = headerSpanAt(e->pos());

                if (ntargetSpanIndex >= d->fixedCount && ntargetSpanIndex >= 0 && ntargetSpanIndex < headerData->spans.count()
                        && moveStartSpan >= 0 && moveStartSpan < headerData->spans.count())
                {
                    HeaderSpanRelation relation =
                        headerData->spans[ntargetSpanIndex]->spanRelation(*(headerData->spans[moveStartSpan]));

                    if (relation == AdjacentIrrelevant || relation == Contain || relation == Overlapping)
                    {
                        bcanMove = false;
                    }
                    else
                    {
                        emit canSectionMove(d->section, d->target, bcanMove);
                        if (isLogicalSectionMovable())
                        {
                            d->section = d->target = -1;
                            updateHeaderSpanIndicator(npos);
                            break;
                        }
                    }
                }
                else
                {
                    bcanMove = false;
                }

                if (bcanMove)
                {
                    if (ntargetSection >= nmovingSectoin)
                    {
                        if (d->orientation == Qt::Horizontal)
                        {
                            for (int i = visualHeaderSpan.left(); i <= visualHeaderSpan.right(); i++)
                            {
                                moveSection(visualHeaderSpan.left(), ntargetSection);
                            }
                        }
                        else
                        {
                            for (int i = visualHeaderSpan.top(); i <= visualHeaderSpan.bottom(); i++)
                            {
                                moveSection(visualHeaderSpan.top(), ntargetSection);
                            }
                        }
                    }
                    else
                    {
                        if (d->orientation == Qt::Horizontal)
                        {
                            for (int i = visualHeaderSpan.right(); i >= visualHeaderSpan.left(); i--)
                            {
                                moveSection(visualHeaderSpan.right(), ntargetSection);
                            }
                        }
                        else
                        {
                            for (int i = visualHeaderSpan.bottom(); i >= visualHeaderSpan.top(); i--)
                            {
                                moveSection(visualHeaderSpan.bottom(), ntargetSection);
                            }
                        }
                    }
                }

                d->section = d->target = -1;
                updateHeaderSpanIndicator(npos);
                break;
            }
            else
            {
                d->target = -1;
                updateHeaderSpanIndicator(npos);
            }
        }

        case GlodonHeaderViewPrivate::SelectSections:
        {
            if (!d->clickableSections)
            {
                int nsection = logicalIndexAt(npos);
                updateSection(nsection);
            }

//        break;
        }    // fall through

        case GlodonHeaderViewPrivate::NoState:
        {
            if (d->clickableSections)
            {
                int nsection = logicalIndexAt(npos);

                if (nsection != -1 && nsection == d->pressed)
                {
                    d->flipSortIndicator(nsection);
                    emit sectionClicked(nsection);
                }

                if (d->pressed != -1)
                {
                    updateSection(d->pressed);
                }
            }

            break;
        }

        case GlodonHeaderViewPrivate::ResizeSection:
        {
            if (d->resizeDelay && !d->m_isMouseDoubleClick)
            {
                if (d->cascadingResizing)
                {
                    int ndelta = d->reverse() ? d->lastPos - npos : npos - d->lastPos;
                    int nVisualIndex = visualIndex(d->section);
                    d->cascadingResize(nVisualIndex, d->headerSectionSize(nVisualIndex) + ndelta, true);
                }
                else
                {
                    int ndelta = d->reverse() ? d->firstPos - npos : npos - d->firstPos;
                    resizeSection(d->section, qMax(d->originalSize + ndelta, minimumSectionSize()), true, true);
                }
            }

            emit sectionResizing(d->m_mousePosition, orientation(), Finish);

            if (orientation() == Qt::Horizontal)
            {
                d->viewport->update(d->m_mousePosition.x(), 0, 1, height());
            }
            else
            {
                d->viewport->update(0, d->m_mousePosition.y(), width(), 1);
            }

            d->originalSize = -1;
            d->clearCascadingSections();
            break;
        }

        default:
            break;
    }

    d->m_state = GlodonHeaderViewPrivate::NoState;
    d->pressed = -1;
}

void GlodonMultiHeaderView::initTitles()
{
    Q_D(GlodonMultiHeaderView);
    headerList.clear();
    resetSpan();

    recoverMinimumHeights();

    d->viewport->update();
}

int GlodonMultiHeaderView::headerSpanAt(QPoint point)
{
    Q_D(GlodonMultiHeaderView);

    int ncolPos = point.x();
    int nrowPos = point.y();

    if (d->orientation == Qt::Vertical)
    {
        int row = logicalIndexAt(nrowPos);
        int col = 0;

        for (col = 0; col < headerList.count(); col++)
        {
            ncolPos -= headerList[col]->drawWidth();

            if (ncolPos <= 0)
            {
                break;
            }
        }

        int nindex = headerData->spanAt(row, col);
        QRect rect = headerSpanRect(nindex);
        int ngrip = style()->pixelMetric(QStyle::PM_HeaderGripMargin, 0, this);

        if (qAbs(rect.top() - nrowPos) <= ngrip || qAbs(rect.bottom() - nrowPos) <= ngrip)
        {
            return -1;
        }
        else
        {
            return nindex;
        }
    }
    else
    {
        int col = logicalIndexAt(ncolPos);
        int row = 0;

        for (row = 0; row < headerList.count(); row++)
        {
            nrowPos -= headerList[row]->drawWidth();

            if (nrowPos <= 0)
            {
                break;
            }
        }

        int nIndex = headerData->spanAt(row, col);
        QRect rect = headerSpanRect(nIndex);
        int nGrip = style()->pixelMetric(QStyle::PM_HeaderGripMargin, 0, this);

        if (qAbs(rect.left() - ncolPos) <= nGrip || qAbs(rect.right() - ncolPos) <= nGrip)
        {
            return -1;
        }
        else
        {
            return nIndex;
        }
    }
}

QRegion GlodonMultiHeaderView::visualRegionForSelection(const QItemSelection &selection) const
{
    Q_D(const GlodonMultiHeaderView);
    const int c_max = d->modelSectionCount();

    if (d->orientation == Qt::Horizontal)
    {
        int nLeft = c_max;
        int nRight = 0;
        int nRangeLeft(0);
        int nRangeRight(0);

        for (int i = 0; i < selection.count(); ++i)
        {
            QItemSelectionRange tmpRange = selection.at(i);

            if (tmpRange.parent().isValid() || !tmpRange.isValid())
            {
                continue;
            }

            // FIXME an item inside the range may be the leftmost or rightmost
            nRangeLeft = visualIndex(tmpRange.left());

            if (nRangeLeft == -1) // in some cases users may change the selections
            {
                continue;    // before we have a chance to do the layout
            }

            nRangeRight = visualIndex(tmpRange.right());

            if (nRangeRight == -1) // in some cases users may change the selections
            {
                continue;    // before we have a chance to do the layout
            }

            if (nRangeLeft < nLeft)
            {
                nLeft = nRangeLeft;
            }

            if (nRangeRight > nRight)
            {
                nRight = nRangeRight;
            }
        }

        int nlogicalLeft = logicalIndex(nLeft);
        int nlogicalRight = logicalIndex(nRight);

        if (nlogicalLeft < 0 || nlogicalLeft >= count()
                || nlogicalRight < 0 || nlogicalRight >= count())
        {
            return QRegion();
        }

        int nleftPos = sectionViewportPosition(nlogicalLeft);
        int nrightPos = sectionViewportPosition(nlogicalRight);
        nrightPos += sectionSize(nlogicalRight);
        return QRect(nleftPos, 0, nrightPos - nleftPos, drawWidth());
    }
    else
    {
        int nTop = c_max;
        int nBottom = 0;
        int nRangeTop(0);
        int nRangeBottom(0);

        for (int i = 0; i < selection.count(); ++i)
        {
            QItemSelectionRange tmpRange = selection.at(i);

            if (tmpRange.parent().isValid() || !tmpRange.isValid())
            {
                continue;    // we only know about toplevel items
            }

            // FIXME an item inside the range may be the leftmost or rightmost
            nRangeTop = visualIndex(tmpRange.top());

            if (nRangeTop == -1) // in some cases users may change the selections
            {
                continue;    // before we have a chance to do the layout
            }

            nRangeBottom = visualIndex(tmpRange.bottom());

            if (nRangeBottom == -1) // in some cases users may change the selections
            {
                continue;    // before we have a chance to do the layout
            }

            if (nRangeTop < nTop)
            {
                nTop = nRangeTop;
            }

            if (nRangeBottom > nBottom)
            {
                nBottom = nRangeBottom;
            }
        }

        int nlogicalTop = logicalIndex(nTop);
        int nlogicalBottom = logicalIndex(nBottom);

        if (nlogicalTop == -1 || nlogicalBottom == -1)
        {
            return QRect();
        }

        int ntopPos = sectionViewportPosition(nlogicalTop);
        int nbottomPos = sectionViewportPosition(nlogicalBottom) + sectionSize(nlogicalBottom);

        return QRect(0, ntopPos, drawWidth(), nbottomPos - ntopPos);
    }
}

GLDHeaderSpan GlodonMultiHeaderView::visualSpan(int index)
{
    Q_D(GlodonMultiHeaderView);
    GLDHeaderSpan *logicalSpan = headerData->spans[index];

    if (d->orientation == Qt::Horizontal)
    {
        int nleft = d->sectionCount();
        int nright = 0;

        for (int i = logicalSpan->left(); i <= logicalSpan->right(); i++)
        {
            int nVisual = visualIndex(i);

            if (nVisual < nleft)
            {
                nleft = nVisual;
            }

            if (nVisual > nright)
            {
                nright = nVisual;
            }
        }

        GLDHeaderSpan visualHeader;
        visualHeader.setLeft(nleft);
        visualHeader.setRight(nright);
        visualHeader.setTop(logicalSpan->top());
        visualHeader.setBottom(logicalSpan->bottom());
        return visualHeader;
    }
    else
    {
        int nTop = d->sectionCount();
        int nBottom = 0;

        for (int i = logicalSpan->top(); i <= logicalSpan->bottom(); i++)
        {
            int nVisual = visualIndex(i);

            if (nVisual < nTop)
            {
                nTop = nVisual;
            }

            if (nVisual > nBottom)
            {
                nBottom = nVisual;
            }
        }

        GLDHeaderSpan visualHeader;
        visualHeader.setLeft(logicalSpan->left());
        visualHeader.setRight(logicalSpan->right());
        visualHeader.setTop(nTop);
        visualHeader.setBottom(nBottom);
        return visualHeader;
    }
}

void GlodonMultiHeaderView::setOffset(int offset)
{
    Q_D(GlodonMultiHeaderView);

    for (int i = 0; i < headerList.count(); i++)
    {
        headerList[i]->setOffset(offset);
    }

    GlodonHeaderView::setOffset(offset);

    d->viewport->update();
}

void GlodonMultiHeaderView::selectionChanged(const QItemSelection &selected,
        const QItemSelection &deselected)
{
    Q_D(GlodonMultiHeaderView);

    if (isVisible() && updatesEnabled())
    {
        QRegion deRegion = visualRegionForSelection(deselected);
        QRegion selRegion = visualRegionForSelection(selected);

        for (int i = 0; i < headerData->spans.count(); i++)
        {
            if (d->orientation == Qt::Horizontal)
            {
                QRect span = headerSpanRect(i);
                QRect spanLeftTop = QRect(span.topLeft(), span.topLeft());

                if (deRegion.contains(spanLeftTop))
                {
                    deRegion = deRegion.united(span);
                }

                if (selRegion.contains(spanLeftTop))
                {
                    selRegion = selRegion.united(span);
                }

                if (deRegion.contains(span) || selRegion.contains(span)) // in one's subarea
                {
                    if (d->isColumnSelected(headerData->spans[i]->left()))
                    {
                        selectHeaderSpan(i, true, false, false);
                    }
                    else
                    {
                        selectHeaderSpan(i, false, false);
                    }
                }
            }
            else
            {
                QRect span = headerSpanRect(i);
                QRect spanTopLeft = QRect(span.topLeft(), span.topLeft());

                if (deRegion.contains(spanTopLeft))
                {
                    deRegion = deRegion.united(span);
                }

                if (selRegion.contains(spanTopLeft))
                {
                    selRegion = selRegion.united(spanTopLeft);
                }

                if (deRegion.contains(span) || selRegion.contains(span))
                {
                    if (d->isRowSelected(headerData->spans[i]->top()))
                    {
                        selectHeaderSpan(i, true);
                    }
                    else
                    {
                        selectHeaderSpan(i, false);
                    }
                }
            }
        }

        d->viewport->update(deRegion | selRegion);
    }
}

int GlodonMultiHeaderView::rowCount() const
{
    Q_D(const GlodonMultiHeaderView);

    if (d->orientation == Qt::Horizontal)
    {
        return extraCount() + 1;
    }
    else
    {
        return count();
    }
}

int GlodonMultiHeaderView::colCount() const
{
    Q_D(const GlodonMultiHeaderView);

    if (d->orientation == Qt::Horizontal)
    {
        return count();
    }
    else
    {
        return extraCount() + 1;
    }
}

void GlodonMultiHeaderView::setupHeaderSpanIndicator(int position)
{
    Q_D(GlodonMultiHeaderView);

    /*因为一个目前不知道为什么会出现的bug而不得不采用的方式
      bug现象：不断的进行几次窗口最大最小化，或者调整窗体大小后
      再进行拖拽移动时会崩溃*/
    if (headerSpanIndicator != NULL)
    {
        headerSpanIndicator = NULL;
    }

    headerSpanIndicator = new QLabel(d->viewport);

    int nwidth = 0;
    int nheight = 0;
    int nP = 0;
    QRect start = headerSpanRect(moveStartSpan);
    GLDHeaderSpan *moveStart = headerData->spans[moveStartSpan];
    QPoint point = start.topLeft();

    if (d->orientation == Qt::Horizontal)
    {
        nwidth = start.width();

        for (int i = headerData->spans[moveStartSpan]->top(); i <= extraCount() - 1; i++)
        {
            nheight += headerList[i]->drawWidth();
        }

        nheight += lastDrawWidth();
    }
    else
    {
        nheight = start.height();

        for (int i = headerData->spans[moveStartSpan]->top(); i <= extraCount() - 1; i++)
        {
            nwidth += headerList[i]->drawWidth();
        }

        nwidth += lastDrawWidth();
    }

    headerSpanIndicator->resize(nwidth, nheight);

    QPixmap pm(nwidth, nheight);
    pm.fill(QColor(0, 0, 0, 45));
    QRect rect(0, 0, 0, 0);

    QPainter painter(&pm);
    painter.setOpacity(0.75);

    if (d->orientation == Qt::Horizontal)
    {
        nP = start.left();

        for (int i = 0; i < headerData->spans.count(); i++)
        {
            if (headerData->spans[i]->left() >= moveStart->left()
                    && headerData->spans[i]->right() <= moveStart->right())
            {
                rect = headerSpanRect(i);
                rect.translate(-point.x(), -point.y());
                paintSpanHeader(&painter, rect, headerData->spans[i]);
            }
        }

        headerSpanIndicatorDrawPos = start.top();
    }
    else
    {
        nP = start.top();

        for (int i = 0; i < headerData->spans.count(); i++)
        {
            if (headerData->spans[i]->top() >= moveStart->top()
                    && headerData->spans[i]->bottom() <= moveStart->bottom())
            {
                rect = headerSpanRect(i);
                rect.translate(-point.x(), -point.y());
                paintSpanHeader(&painter, rect, headerData->spans[i]);
            }
        }

        headerSpanIndicatorDrawPos = start.left();
    }

    painter.end();

    headerSpanIndicator->setPixmap(pm);
    headerSpanIndicatorOffset = position - qMax(nP, 0);
}

void GlodonMultiHeaderView::updateHeaderSpanIndicator(int position)
{
    Q_D(GlodonMultiHeaderView);

    if (!headerSpanIndicator)
    {
        return;
    }

    if (d->section == -1 || d->target == -1)
    {
        headerSpanIndicator->hide();
        return;
    }

    if (d->orientation == Qt::Horizontal)
    {
        headerSpanIndicator->move(position - headerSpanIndicatorOffset, headerSpanIndicatorDrawPos);
    }
    else
    {
        headerSpanIndicator->move(headerSpanIndicatorDrawPos, position - headerSpanIndicatorOffset);
    }

    headerSpanIndicator->show();
}

void GlodonMultiHeaderView::currentChanged(const QModelIndex &current, const QModelIndex &old)
{
    for (int i = 0; i < headerList.count(); i++)
    {
        headerList[i]->currentChanged(current, old);
    }

    GlodonHeaderView::currentChanged(current, old);
}

void GlodonMultiHeaderView::updateSection(int logicalIndex)
{
    Q_D(GlodonMultiHeaderView);

    if (d->orientation == Qt::Horizontal)
    {
        int nindex = headerData->spanAt(extraCount(), logicalIndex);

        if (nindex == -1)
        {
            return;
        }

        for (int i = 0; i < headerData->spans.count(); i++)
        {
            if (headerData->spans[i]->left() <= headerData->spans[nindex]->left()
                    && headerData->spans[i]->right() >= headerData->spans[nindex]->right())
            {
                nindex = i;
            }
        }

        int nwidth = 0;

        for (int i = headerData->spans[nindex]->left(); i <= headerData->spans[nindex]->right(); i++)
        {
            nwidth += sectionSize(i);
        }

        d->viewport->update(QRect(sectionViewportPosition(headerData->spans[nindex]->left()),
                                  0, nwidth, d->viewport->height()));
    }
    else
    {
        int nindex = headerData->spanAt(logicalIndex, headerList.count() + 1);

        if (nindex == -1)
        {
            return;
        }

        for (int i = 0; i < headerData->spans.count(); i++)
        {
            if (headerData->spans[i]->top() <= headerData->spans[nindex]->top()
                    && headerData->spans[i]->bottom() >= headerData->spans[nindex]->bottom())
            {
                nindex = i;
            }
        }

        int nheight = 0;

        for (int i = headerData->spans[nindex]->top(); i <= headerData->spans[nindex]->bottom(); i++)
        {
            nheight += sectionSize(i);
        }

        d->viewport->update(QRect(0, sectionViewportPosition(headerData->spans[nindex]->top()),
                                  d->viewport->width(), nheight));
    }
}

int GlodonMultiHeaderView::lastDrawWidth()
{
    Q_D(GlodonMultiHeaderView);
    int nWidth = 0;

    for (int i = 0; i < headerList.count(); i++)
    {
        nWidth += headerList[i]->drawWidth();
    }

    if (d->orientation == Qt::Horizontal)
    {
        return (viewport()->height() - nWidth);
    }
    else
    {
        return (viewport()->width() - nWidth);
    }
}

void GlodonMultiHeaderView::paintSpanHeader(QPainter *painter, const QRect &rect, GLDHeaderSpan *span) const
{
    Q_D(const GlodonMultiHeaderView);

    if (!rect.isValid() || rect.height() == 1 || rect.width() == 1) //高度和宽度为1的时候不应该绘制，不然会出现多余的线
    {
        return;
    }

    QStyleOptionHeader opt;
    initStyleOption(&opt);
    QStyle::State state = QStyle::State_None;
    bool bDrawHighLight = false;

    if (isEnabled())
    {
        state |= QStyle::State_Enabled;
    }

    if (window()->isActiveWindow())
    {
        state |= QStyle::State_Active;
    }

    if (d->clickableSections)
    {
        if (span->selected())
        {
            state |= QStyle::State_Sunken;
        }

        if (d->highlightSelected)
        {
            bool bstate_on = false;
            int nStart = d->orientation == Qt::Horizontal ? span->left() : span->top();

            if (d->sectionIntersectsSelection(nStart))
            {
                bstate_on = true;
            }

            if (bstate_on)
            {
                if (d->m_boldWithSelected)
                {
                    state |= QStyle::State_On;
                }

                if (!span->selected())
                {
                    bDrawHighLight = true;
                }
            }
        }
    }

    if (isSortIndicatorShown() && sortIndicatorSection() == span->section())
    {
        if (sortIndicatorOrder() == Qt::AscendingOrder)
        {
            opt.sortIndicator = QStyleOptionHeader::SortDown;
        }
        else if (sortIndicatorOrder() == Qt::DescendingOrder)
        {
            opt.sortIndicator = QStyleOptionHeader::SortUp;
        }
        else
        {
            opt.sortIndicator = QStyleOptionHeader::None;
        }
    }

    QVariant textAlignment = d->m_model->headerData(span->section(), d->orientation,
                             Qt::TextAlignmentRole);

    opt.rect = rect;
    opt.section = span->section();
    opt.state |= state;

//    opt.textAlignment = Qt::Alignment(textAlignment.isValid()
//                                      ? Qt::Alignment(textAlignment.toInt())
//                                      : d->defaultAlignment);
    if (textAlignment.isValid() && ((textAlignment.toInt() & Qt::AlignHorizontal_Mask) != 0))
    {
        opt.textAlignment = Qt::Alignment(textAlignment.toInt());
    }
    else
    {
        opt.textAlignment = Qt::AlignCenter;
    }

    opt.textAlignment = Qt::AlignmentFlag(int(opt.textAlignment) | int(Qt::TextWordWrap));
    opt.iconAlignment = Qt::AlignVCenter;

    QVariant var = d->m_model->headerData(span->section(), Qt::Horizontal, Qt::FontRole);
    QFont fnt;

    if (var.isValid() && var.canConvert<QFont>())
    {
        fnt = qvariant_cast<QFont>(var);
    }
    else
    {
        fnt = font();
    }

    painter->setFont(fnt);

    QVariant variant = d->m_model->headerData(span->section(), d->orientation,
                       Qt::DecorationRole);
    opt.icon = qvariant_cast<QIcon>(variant);

    if (opt.icon.isNull())
    {
        opt.icon = qvariant_cast<QPixmap>(variant);
    }

    variant = d->m_model->headerData(span->section(), d->orientation,
                                     Qt::DecorationPropertyRole);

    if (variant.isValid())
    {
        opt.iconAlignment = (Qt::Alignment)variant.toInt();
    }

    QVariant foregroundBrush = d->m_model->headerData(span->section(), d->orientation,
                               Qt::ForegroundRole);

    if (foregroundBrush.canConvert<QBrush>())
    {
        opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));
    }

    QPointF oldBO = painter->brushOrigin();
    QVariant backgroundBrush = d->m_model->headerData(span->section(), d->orientation,
                               Qt::BackgroundRole);

    if (backgroundBrush.canConvert<QBrush>())
    {
        opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
        opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
        painter->setBrushOrigin(opt.rect.topLeft());
    }

    if (d->orientation == Qt::Horizontal)
    {
        if (count() == 1)
        {
            opt.position = QStyleOptionHeader::OnlyOneSection;
        }
        else if (span->left() == 0)
        {
            opt.position = QStyleOptionHeader::Beginning;
        }
        else if (span->right() == count() - 1)
        {
            opt.position = QStyleOptionHeader::End;
        }
        else
        {
            opt.position = QStyleOptionHeader::Middle;
        }
    }
    else
    {
        if (count() == 1)
        {
            opt.position = QStyleOptionHeader::OnlyOneSection;
        }
        else if (span->top() == 0)
        {
            opt.position = QStyleOptionHeader::Beginning;
        }
        else if (span->bottom() == count() - 1)
        {
            opt.position = QStyleOptionHeader::End;
        }
        else
        {
            opt.position = QStyleOptionHeader::Middle;
        }
    }

    opt.orientation = d->orientation;
    drawCellBorder(painter, opt);

    if (bDrawHighLight)
    {
        opt.text = span->text();

        if (d->m_textElideMode != Qt::ElideNone)
        {
            opt.text = opt.fontMetrics.elidedText(opt.text, d->m_textElideMode , rect.width() - 4);
        }

//        paintHighLightSection(painter, opt, rect);

        if (isFixedcellEvent())
        {
            paintHighLightSection(painter, opt, rect);
        }
        else
        {
//            style()->drawControl(QStyle::CE_Header, &opt, painter, this);
            paintSectionOnQt(painter, opt, this, true);
        }
    }
    else
    {
//        style()->drawControl(QStyle::CE_Header, &opt, painter, this);

        opt.text = span->text();

        if (d->m_textElideMode != Qt::ElideNone)
        {
            opt.text = opt.fontMetrics.elidedText(opt.text, d->m_textElideMode , rect.width() - 4);
        }

//        opt.rect = textRect(rect);
//        opt.icon = QIcon();

        paintSectionOnQt(painter, opt, this, false);
//        style()->drawControl(QStyle::CE_HeaderLabel, &opt, painter, this);

        opt.rect = rect;
    }

    painter->setBrushOrigin(oldBO);
    if (span->top() == d->m_spanTop)
    {
        if (Qt::Horizontal == d->orientation)
        {
            drawImageInDesignatedLogicalIndex(painter, opt);
        }
    }
}

bool GlodonMultiHeaderView::checkLogicalTitles(int fixedCount)
{
    bool blogical = true;

    for (int i = 0; i < headerData->spans.count(); i++)
    {
        if (headerData->spans[i]->left() < fixedCount && fixedCount < headerData->spans[i]->right())
        {
            blogical = false;
        }
    }

    return blogical;
}

bool GlodonMultiHeaderView::inTopHeaderRange(int topHeaderIndex, int index)
{
    Q_D(GlodonMultiHeaderView);
    int nMin(0);
    int nMax = -1;

    if (d->orientation == Qt::Horizontal)
    {
        if (headerData->spans[topHeaderIndex]->top() == 0)
        {
            nMin = headerData->spans[topHeaderIndex]->left();
            nMax = headerData->spans[topHeaderIndex]->right();
        }
        else
        {
            for (int i = 0; i < headerData->spans.count(); i++)
            {
                if (headerData->spans[i]->left() <= headerData->spans[topHeaderIndex]->left()
                        && headerData->spans[i]->right() >= headerData->spans[topHeaderIndex]->right()
                        && headerData->spans[i]->top() == 0)
                {
                    nMin = headerData->spans[i]->left();
                    nMax = headerData->spans[i]->right();
                    break;
                }
            }
        }

        return (headerData->spans[index]->left() >= nMin && headerData->spans[index]->right() <= nMax);
    }
    else
    {
        if (headerData->spans[topHeaderIndex]->left() == 0)
        {
            nMin = headerData->spans[topHeaderIndex]->top();
            nMax = headerData->spans[topHeaderIndex]->bottom();
        }
        else
        {
            for (int i = 0; i < headerData->spans.count(); i++)
            {
                if (headerData->spans[i]->top() <= headerData->spans[topHeaderIndex]->top()
                        && headerData->spans[i]->bottom() >= headerData->spans[topHeaderIndex]->bottom()
                        && headerData->spans[i]->left() == 0)
                {
                    nMin = headerData->spans[i]->top();
                    nMax = headerData->spans[i]->bottom();
                    break;
                }
            }
        }

        return (headerData->spans[index]->top() >= nMin && headerData->spans[index]->bottom() <= nMax);
    }
}

bool GlodonMultiHeaderView::checkCanMove(int targetLogicalIndex)
{
    Q_D(GlodonMultiHeaderView);
    int nto = visualIndex(targetLogicalIndex);
    GLDHeaderSpan *moveStart = headerData->spans[moveStartSpan];
    int nfrom = visualIndex(d->section);

    if (d->orientation == Qt::Horizontal)
    {
        int nTargetIndex = headerData->spanAt(moveStart->top(), targetLogicalIndex);
        GLDHeaderSpan *targetSpan = headerData->spans[nTargetIndex];
        GLDHeaderSpan visualTarget = visualSpan(nTargetIndex);

        if (targetSpan->top() == moveStart->top())
        {
            if (moveStart->top() > 0)
            {
                //如果上一层是合并格的话，不在同一个合并格的不让拖动
                int nTop = moveStart->top() - 1;
                int nMoveTopIndex = headerData->spanAt(nTop, moveStart->left());
                int nTargetTopIndex = headerData->spanAt(nTop, targetSpan->left());

                if (nMoveTopIndex != nTargetTopIndex)
                {
                    return false;
                }
            }

            if (nfrom < nto)
            {
                if (visualTarget.left() > nto || visualTarget.right() <= nto)
                {
                    return true;
                }
            }
            else
            {
                if (visualTarget.left() >= nto || visualTarget.right() < nto)
                {
                    return true;
                }
            }
        }
    }
    else
    {
        int nTargetIndex = headerData->spanAt(targetLogicalIndex, moveStart->left());
        GLDHeaderSpan *targetSpan = headerData->spans[nTargetIndex];
        GLDHeaderSpan visualTarget = visualSpan(nTargetIndex);

        if (targetSpan->left() == moveStart->left())
        {
            if (nfrom < nto)
            {
                if (visualTarget.top() > nto || visualTarget.bottom() <= nto)
                {
                    return true;
                }
            }
            else
            {
                if (visualTarget.top() >= nto || visualTarget.bottom() < nto)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

int GlodonMultiHeaderView::parseSpan(int nFirstVisual, int nLastVisual, int nMaxCount)
{
    QString strTitle;
    QList<QStringList> titles;

    for (int i = nFirstVisual; i >= 0 && i < nLastVisual; i++)
    {
        strTitle = model()->headerData(i, orientation()).toString();

        if ("|" == strTitle)
        {
            strTitle = " | ";
        }

        titles.append(strTitle.split('|'));
    }

    for (int i = 0; i < titles.count(); i++)
    {
        int nCurrentCount = titles.at(i).count();

        if (orientation() == Qt::Horizontal)
        {
            for (int j = 0; j < nCurrentCount - 1; j++)
            {
                GLDHeaderSpan *newspan
                    = new GLDHeaderSpan(j, i + nFirstVisual, 1, 1, titles.at(i).at(j), i + nFirstVisual);
                headerData->addSpan(newspan);
            }

            GLDHeaderSpan *newspan = new GLDHeaderSpan(nCurrentCount - 1, i + nFirstVisual,
                    nMaxCount - nCurrentCount + 1, 1,
                    titles.at(i).at(nCurrentCount - 1), i + nFirstVisual);
            headerData->addSpan(newspan);
        }
        else
        {
            for (int j = 0; j < nCurrentCount - 1; j++)
            {
                GLDHeaderSpan *newspan
                    = new GLDHeaderSpan(i + nFirstVisual, j, 1, 1, titles.at(i).at(j), i + nFirstVisual);
                headerData->addSpan(newspan);
            }

            GLDHeaderSpan *newspan = new GLDHeaderSpan(i + nFirstVisual, nCurrentCount - 1,
                    1, nMaxCount - nCurrentCount + 1,
                    titles.at(i).at(nCurrentCount - 1), i + nFirstVisual);
            headerData->addSpan(newspan);
        }
    }

    return nMaxCount;
}

int GlodonMultiHeaderView::maxCount()
{
    Q_D(GlodonMultiHeaderView);
    QString strTitle;
    int nMaxCount = 0;

    for (int i = 0; i >= 0 && i < d->sectionCount(); i++)
    {
        strTitle = model()->headerData(i, orientation()).toString();
        nMaxCount = qMax(nMaxCount, strTitle.split('|').count());
    }

    return nMaxCount;
}

void GlodonMultiHeaderView::recoverMinimumHeights()
{
    Q_D(GlodonMultiHeaderView);

    if (d->orientation == Qt::Horizontal && m_minimumHeights.count() > 0)
    {
        for (int i = 0; i < headerList.count(); i++)
        {
            headerList.at(i)->setMinimumHeight(m_minimumHeights.at(i));
        }

        setMinimumHeight(m_minimumHeights.at(headerList.count()));
    }
}

int GlodonMultiHeaderView::resetSpan()
{
    Q_D(GlodonMultiHeaderView);
    headerData->spans.clear();

    int nMaxCount = maxCount();
    parseSpan(0, d->sectionCount(), nMaxCount);
    Q_ASSERT_X(checkLogicalTitles(d->fixedCount), "resetSpan",
               "cannot resetSpan that makes part of a combined cell fixed");
	// 2015/11/5 chensf 刷新Tableview时 考虑多重表头个数发生改变的情况，重新计算GlodonHeaderView的数量
    setExtraCount(nMaxCount - 1);
    updateGeometries();
    d->viewport->update();
    return nMaxCount;
}

void GlodonMultiHeaderView::updateSpan(int index)
{
    Q_D(GlodonMultiHeaderView);
    QStringList title = model()->headerData(index, orientation()).toString().split("|");

    for (int i = 0; i < headerData->spans.count(); ++i)
    {
        GLDHeaderSpan *currentSpan = headerData->spans.at(i);

        if (orientation() == Qt::Horizontal)
        {
            if (currentSpan->left() == index)
            {
                if (title.count() > 1)
                {
                    for (int row = 0; row < title.count(); ++row)
                    {
                        headerData->spans.at(headerData->spanAt(row, index))->setText(title.at(row));
                    }
                }
                else
                {
                    currentSpan->setText(title.at(0));
                }

                break;
            }
        }
        else
        {
            if (currentSpan->top() == index)
            {
                if (title.count() > 1)
                {
                    for (int col = 0; col < title.count(); ++col)
                    {
                        headerData->spans.at(headerData->spanAt(index, col))->setText(title.at(col));
                    }
                }
                else
                {
                    currentSpan->setText(title.at(0));
                }

                break;
            }
        }
    }

    d->viewport->update();
}

void GlodonMultiHeaderView::setSpansSelected(int index, bool selected)
{
    Q_D(GlodonMultiHeaderView);

    if (Qt::Horizontal == d->orientation)
    {
        for (int i = 0; i < headerData->spans.count(); i++)
        {
            if ((index >= headerData->spans.at(i)->left())
                    && (index <= headerData->spans.at(i)->right()))
            {
                headerData->spans.at(i)->setSelected(selected);
            }
        }
    }
    else
    {
        for (int i = 0; i < headerData->spans.count(); i++)
        {
            if ((index >= headerData->spans.at(i)->top())
                    && (index <= headerData->spans.at(i)->bottom()))
            {
                headerData->spans.at(i)->setSelected(selected);
            }
        }
    }
}

void GlodonMultiHeaderView::setMinimumHeights(QVector<int> &minHs)
{
    Q_D(GlodonMultiHeaderView);

    if (d->orientation == Qt::Horizontal)
    {
        m_minimumHeights = minHs;
    }
}

void GHeaderSpanCollection::addSpan(GLDHeaderSpan *span)
{
    if (parseSpanRelationRecursively(span, false, -1))
    {
        freeAndNil(span);
    }
    else
    {
        spans.append(span);
    }
}

void GHeaderSpanCollection::parseSpanInternal(int index, bool isInRecursion, int i)
{

    if(isInRecursion)
    {
        if(index <= i)
        {
            i--;
        }

        spans.removeAt(index);

    }

    parseSpanRelationRecursively(spans[i], true, i);
}

bool GHeaderSpanCollection::parseSpanRelationRecursively(GLDHeaderSpan *span, bool isInRecursion, int index)
{
    // 先判断上一级标题是否相同
    for (int i = 0; i < spans.count(); ++i)
    {
        if ((spans.at(i)->bottom() == (span->top() - 1))
                && (spans.at(i)->left() == span->left())
                )
        {
            if (spans.at(i)->text() != span->text())
            {
                return false;
            }
        }
    }

    HeaderSpanRelation relation;

    for (int i = spans.count() - 1; i >= 0; i--) // reverse
    {
        relation = spans[i]->spanRelation(*span);
        Q_ASSERT(relation != Overlapping);

        switch (relation)
        {
            case Overlapping:
            case Irrelevant:
            case AdjacentIrrelevant:
            {
                continue;
            }

            case Contain:
            {
                if (!isInRecursion)
                {
                    return true;
                }

                continue;
            }

            case AdjacentRight:
            {
                spans[i]->setLeft(span->left());

                break;
            }

            case AdjacentLeft:
            {
                spans[i]->setRight(span->right());

                break;
            }

            case AdjacentDown:
            {
                spans[i]->setTop(span->top());

                break;
            }

            case AdjacentUp:
            {
                spans[i]->setBottom(span->bottom());

                break;
            }
        }

        parseSpanInternal(index, isInRecursion, i);

        return true;
    }

    return false;
}

HeaderSpanRelation GLDHeaderSpan::spanRelation(GLDHeaderSpan span)
{
    if (m_text != span.text()|| ((m_text == "") && (span.text() == ""))
            || m_left > span.right() + 1 || m_top > span.bottom() + 1
            || m_right < span.left() - 1 || m_bottom < span.top() - 1)
    {
        return Irrelevant;
    }

    if (m_left <= span.left() && m_top <= span.top() && m_bottom >= span.bottom() && m_right >= span.right())
    {
        return Contain;
    }

    if (m_left == span.right() + 1)
    {
        if (height() == span.height())
        {
            return AdjacentRight;
        }
        else
        {
            return AdjacentIrrelevant;
        }
    }

    if (m_right == span.left() - 1)
    {
        if (height() == span.height())
        {
            return AdjacentLeft;
        }
        else
        {
            return AdjacentIrrelevant;
        }
    }

    // Span上下左右是屏幕坐标系
    // 数字的加减是笛卡尔坐标系

    if (m_top == span.bottom() + 1)
    {
        if (width() == span.width())
        {
            return AdjacentDown;
        }
        else
        {
            return AdjacentIrrelevant;
        }
    }

    if (m_bottom == span.top() - 1)
    {
        if (width() == span.width())
        {
            return AdjacentUp;
        }
        else
        {
            return AdjacentIrrelevant;
        }
    }

    return Overlapping;
}

void GlodonMultiHeaderView::setFixedCount(int fixedCount)
{
    //由于动态设置列隐藏的过程中，由可编辑固定都隐藏，到显示一列可编辑固定列，会出现checkLogicalTitles不合法的情况，
    //但是由于不设置GlodonHeaderView中的fixedCol，GlodonMultiHeaderView就不能正确的initTitle，所以不合法校验，不在
    //setFixedCount中进行
//    Q_ASSERT_X(checkLogicalTitles(fixedCount), "setFixedCount",
//               "cannot set fixed count that makes part of a combined cell fixed");
    GlodonHeaderView::setFixedCount(fixedCount);
}

int GHeaderSpanCollection::spanAt(int rowIndex, int colIndex) const
{
    for (int i = 0; i < spans.count(); i++)
    {
        if (spans[i]->left() <= colIndex && spans[i]->right() >= colIndex
                && spans[i]->top() <= rowIndex && spans[i]->bottom() >= rowIndex)
        {
            return i;
        }
    }

    return -1;
}

void GlodonMultiHeaderView::updateHeaderSpan(int index)
{
    Q_D(GlodonMultiHeaderView);
    QRect rect = headerSpanRect(index);
    d->viewport->update(rect);
}

void GlodonMultiHeaderView::selectHeaderSpan(int index, bool select, bool checkParent, bool checkChild)
{
    Q_D(GlodonMultiHeaderView);

    if (index < 0 || index > headerData->spans.count() || headerData->spans[index]->selected() == select)
    {
        return;
    }

    headerData->spans[index]->setSelected(select);
    updateHeaderSpan(index);

    if (d->orientation == Qt::Horizontal)
    {
        if (!checkChild && headerData->spans[index]->top() == 0)
        {
            return;
        }

        if (!checkParent && headerData->spans[index]->bottom() == extraCount())
        {
            return;
        }

        for (int i = 0; i < headerData->spans.count(); i++)
        {
            if (headerData->spans[i]->left() <= headerData->spans[index]->left()
                    && headerData->spans[i]->right() >= headerData->spans[index]->right()
                    && headerData->spans[i]->bottom() == headerData->spans[index]->top() - 1
                    && checkParent)
            {
                if (headerData->spans[i]->selected() != select)
                {
                    if (select)
                    {
                        bool bAllChildrenSelected = true;

                        for (int j = 0; j < headerData->spans.count(); j++)
                        {
                            if (headerData->spans[i]->left() <= headerData->spans[j]->left()
                                    && headerData->spans[i]->right() >= headerData->spans[j]->right()
                                    && headerData->spans[i]->bottom() == headerData->spans[j]->top() - 1
                                    && !headerData->spans[j]->selected())
                            {
                                bAllChildrenSelected = false;
                                break;
                            }
                        }

                        if (bAllChildrenSelected)
                        {
                            selectHeaderSpan(i, true, true, false);
                        }
                    }
                    else
                    {
                        selectHeaderSpan(i, false, true, false);
                    }
                }
            }
            else if (headerData->spans[i]->left() >= headerData->spans[index]->left()
                     && headerData->spans[i]->right() <= headerData->spans[index]->right()
                     && headerData->spans[i]->top() == headerData->spans[index]->bottom() + 1
                     && checkChild)
            {
                if (headerData->spans[i]->selected() != select)
                {
                    selectHeaderSpan(i, select, false, true);
                }
            }
        }
    }
    else
    {
        if (!checkChild && headerData->spans[index]->left() == 0)
        {
            return;
        }

        if (!checkParent && headerData->spans[index]->right() == extraCount())
        {
            return;
        }

        for (int i = 0; i < headerData->spans.count(); i++)
        {
            if (headerData->spans[i]->top() <= headerData->spans[index]->top()
                    && headerData->spans[i]->bottom() >= headerData->spans[index]->bottom()
                    && headerData->spans[i]->right() == headerData->spans[index]->left() - 1
                    && checkParent)
            {
                if (headerData->spans[i]->selected() != select)
                {
                    if (select)
                    {
                        bool bAllChildrenSelected = true;

                        for (int j = 0; j < headerData->spans.count(); j++)
                        {
                            if (headerData->spans[i]->top() <= headerData->spans[j]->top()
                                    && headerData->spans[i]->bottom() >= headerData->spans[j]->bottom()
                                    && headerData->spans[i]->right() == headerData->spans[j]->left() - 1
                                    && !headerData->spans[j]->selected())
                            {
                                bAllChildrenSelected = false;
                                break;
                            }
                        }

                        if (bAllChildrenSelected)
                        {
                            selectHeaderSpan(i, true, true, false);
                        }
                    }
                    else
                    {
                        selectHeaderSpan(i, false, true, false);
                    }
                }
            }
            else if (headerData->spans[i]->top() >= headerData->spans[index]->top()
                     && headerData->spans[i]->bottom() <= headerData->spans[index]->bottom()
                     && headerData->spans[i]->left() == headerData->spans[index]->right() + 1
                     && checkChild)
            {
                if (headerData->spans[i]->selected() != select)
                {
                    selectHeaderSpan(i, select, false, true);
                }
            }
        }
    }
}

QRect GlodonMultiHeaderView::headerSpanRect(int index)
{
    Q_D(GlodonMultiHeaderView);

    if (index < 0 || index >= headerData->spans.count())
    {
        return QRect();
    }

    QRect currentRect(0, 0, 0, 0);

    if (d->orientation == Qt::Horizontal)
    {
        int nLogical = headerData->spans.at(index)->left();
        currentRect.setLeft(sectionViewportPosition(nLogical));
        int nRectWidth = 0;

        bool bSpanVisualFirst = isSectionHidden(headerData->spans.at(index)->left());

        for (int i = headerData->spans.at(index)->left(); i <= headerData->spans.at(index)->right(); i++)
        {
            if (bSpanVisualFirst)
            {
                if (isSectionHidden(i))
                {
                    currentRect.setLeft(sectionViewportPosition(i) + 1);
                }
                else
                {
                    bSpanVisualFirst = false;
                }
            }

            int nRectLeft = sectionViewportPosition(i);

            if (nRectLeft < currentRect.left() && !bSpanVisualFirst &&
                    !isSectionHidden(i))
            {
                currentRect.setLeft(nRectLeft);
            }

            if (isSectionHidden(i))
            {
                nRectWidth -= d->gridLineWidth;
            }

            nRectWidth += sectionSize(i) + d->gridLineWidth;
        }

        currentRect.setWidth(nRectWidth);

        int nRectTop = 0;

        for (int i = 0; i < headerData->spans.at(index)->top(); i++)
        {
//            rectTop += headerList[i]->drawWidth();
            if (headerList[i]->minimumHeight() == 0)
            {
                nRectTop += headerList[i]->drawWidth();
            }
            else
            {
                nRectTop += headerList[i]->minimumHeight();
            }
        }

        currentRect.setTop(nRectTop);

        int nRectHeight = 0;

        for (int i = headerData->spans.at(index)->top(); i <= headerData->spans.at(index)->bottom(); i++)
        {
            if (i < headerList.count())
            {
//                rectHeight += headerList[i]->drawWidth();
                if (headerList[i]->minimumHeight() == 0)
                {
                    nRectHeight += headerList[i]->drawWidth();
                }
                else
                {
                    nRectHeight += headerList[i]->minimumHeight();
                }
            }
            else
            {
                if (minimumHeight() == 0)
                {
                    nRectHeight += lastDrawWidth();
                }
                else
                {
                    nRectHeight += /*lastDrawWidth()*/minimumHeight();
                }
            }
        }

        currentRect.setHeight(nRectHeight);
    }
    else
    {
        int nLogical = headerData->spans.at(index)->top();
        currentRect.setTop(sectionViewportPosition(nLogical));
        int nRectHeight = 0;

        for (int i = headerData->spans.at(index)->top(); i <= headerData->spans.at(index)->bottom(); i++)
        {
            int nRectTop = sectionViewportPosition(i);

            if (nRectTop < currentRect.top())
            {
                currentRect.setTop(nRectTop);
            }

            nRectHeight += sectionSize(i) + d->lineWidth;
        }

        currentRect.setHeight(nRectHeight);

        int nRectLeft = 0;

        for (int i = 0; headerList.count() > 0 && i < headerData->spans.at(index)->left(); i++)
        {
            nRectLeft += headerList[i]->drawWidth();
        }

        currentRect.setLeft(nRectLeft);

        int nRectWidth = 0;

        for (int i = headerData->spans.at(index)->left(); i <= headerData->spans.at(index)->right(); i++)
        {
            if (i < headerList.count())
            {
                nRectWidth += headerList[i]->drawWidth();
            }
            else
            {
                nRectWidth += lastDrawWidth();
            }
        }

        currentRect.setWidth(nRectWidth);
    }

    return currentRect;
}
