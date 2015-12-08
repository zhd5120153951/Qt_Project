#include "GLDHeaderView.h"
#include "GLDHeaderView_p.h"

#include "GLDTableView.h"
#include "GLDAbstractItemModel.h"

#include <qbitarray.h>
#include <qbrush.h>
#include <qevent.h>
#include <qpainter.h>
#include <qscrollbar.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qvector.h>
#include <qapplication.h>
#include <qvarlengtharray.h>
#include <qabstractitemdelegate.h>
#include <qvariant.h>
#include <private/qabstractitemmodel_p.h>
#include <QStyleFactory>
#include "GLDFilterTableView.h"
#include "GLDMultiHeaderView.h"

#ifndef QT_NO_DATASTREAM
#include <qdatastream.h>
#endif

#ifndef QT_NO_DATASTREAM

//排序列表头的小三角与文字的间距
const int c_nSpacingTextToArrow = 10;
//排序列表头的小三角的size
const QSize c_sortArrow(7, 4);

QDataStream &operator<<(QDataStream &out, const GlodonHeaderViewPrivate::SectionItem &section)
{
    section.write(out);
    return out;
}

QDataStream &operator>>(QDataStream &in, GlodonHeaderViewPrivate::SectionItem &section)
{
    section.read(in);
    return in;
}

#endif // QT_NO_DATASTREAM

static const int maxSizeSection = 1048575; // since section size is in a bitfield (uint 20).

GlodonHeaderView::GlodonHeaderView(Qt::Orientation orientation, QWidget *parent)
    : GlodonAbstractItemView(*new GlodonHeaderViewPrivate, parent),
      m_isSortArrowOnTextTop(false)
{
    Q_D(GlodonHeaderView);
    d->setDefaultValues(orientation);
    initialize();
    setAttribute(Qt::WA_Hover);
}

GlodonHeaderView::GlodonHeaderView(GlodonHeaderViewPrivate &dd,
                                   Qt::Orientation orientation, QWidget *parent)
    : GlodonAbstractItemView(dd, parent)
{
    Q_D(GlodonHeaderView);
    d->setDefaultValues(orientation);
    initialize();
}

GlodonHeaderView::~GlodonHeaderView()
{
}

void GlodonHeaderView::initialize()
{
    Q_D(GlodonHeaderView);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(NoFrame);
    setFocusPolicy(Qt::NoFocus);

    d->viewport->setMouseTracking(true);
    d->m_textElideMode = Qt::ElideNone;

    delete d->m_itemDelegate;
}

void GlodonHeaderView::setModel(QAbstractItemModel *model)
{
    if (model == this->model())
    {
        return;
    }

    Q_D(GlodonHeaderView);

    if (d->m_model && d->m_model != QAbstractItemModelPrivate::staticEmptyModel())
    {
        if (d->orientation == Qt::Horizontal)
        {
            QObject::disconnect(d->m_model, SIGNAL(columnsInserted(QModelIndex, int, int)),
                                this, SLOT(sectionsInserted(QModelIndex, int, int)));
            QObject::disconnect(d->m_model, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)),
                                this, SLOT(sectionsAboutToBeRemoved(QModelIndex, int, int)));
            QObject::disconnect(d->m_model, SIGNAL(columnsRemoved(QModelIndex, int, int)),
                                this, SLOT(_q_sectionsRemoved(QModelIndex, int, int)));
        }
        else
        {
            QObject::disconnect(d->m_model, SIGNAL(rowsInserted(QModelIndex, int, int)),
                                this, SLOT(sectionsInserted(QModelIndex, int, int)));
            QObject::disconnect(d->m_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)),
                                this, SLOT(sectionsAboutToBeRemoved(QModelIndex, int, int)));
            QObject::disconnect(d->m_model, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                                this, SLOT(_q_sectionsRemoved(QModelIndex, int, int)));
        }

        QObject::disconnect(d->m_model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
                            this, SLOT(headerDataChanged(Qt::Orientation, int, int)));
        QObject::disconnect(d->m_model, SIGNAL(layoutAboutToBeChanged()),
                            this, SLOT(_q_layoutAboutToBeChanged()));
    }

    if (model && model != QAbstractItemModelPrivate::staticEmptyModel())
    {
        if (d->orientation == Qt::Horizontal)
        {
            QObject::connect(model, SIGNAL(columnsInserted(QModelIndex, int, int)),
                             this, SLOT(sectionsInserted(QModelIndex, int, int)));
            QObject::connect(model, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)),
                             this, SLOT(sectionsAboutToBeRemoved(QModelIndex, int, int)));
            QObject::connect(model, SIGNAL(columnsRemoved(QModelIndex, int, int)),
                             this, SLOT(_q_sectionsRemoved(QModelIndex, int, int)));
        }
        else
        {
            QObject::connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)),
                             this, SLOT(sectionsInserted(QModelIndex, int, int)));
            QObject::connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)),
                             this, SLOT(sectionsAboutToBeRemoved(QModelIndex, int, int)));
            QObject::connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                             this, SLOT(_q_sectionsRemoved(QModelIndex, int, int)));
        }

        QObject::connect(model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
                         this, SLOT(headerDataChanged(Qt::Orientation, int, int)));
        QObject::connect(model, SIGNAL(layoutAboutToBeChanged()),
                         this, SLOT(_q_layoutAboutToBeChanged()));
    }

    d->m_state = GlodonHeaderViewPrivate::NoClear;
    GlodonAbstractItemView::setModel(model);
    d->m_state = GlodonHeaderViewPrivate::NoState;

    const int nLogicalIndicesCount = d->logicalIndices.count();
    for (int i = 0; i < nLogicalIndicesCount; ++i)
    {
        d->logicalIndices[i] = i;
    }

    const int nVisualIndicesCount = d->visualIndices.count();
    for (int i = 0; i < nVisualIndicesCount; ++i)
    {
        d->visualIndices[i] = i;
    }

    // Users want to set sizes and modes before the widget is shown.
    // Thus, we have to initialize when the model is set,
    // and not lazily like we do in the other views.
    initializeSections();
}

Qt::Orientation GlodonHeaderView::orientation() const
{
    Q_D(const GlodonHeaderView);
    return d->orientation;
}

int GlodonHeaderView::offset() const
{
    Q_D(const GlodonHeaderView);
    return d->offset;
}

void GlodonHeaderView::setOffset(int newOffset)
{
    Q_D(GlodonHeaderView);

    if (d->offset == (int)newOffset)
    {
        return;
    }

    int ndelta = d->offset - newOffset;
    d->offset = newOffset;

    if (d->orientation == Qt::Horizontal)
    {
        if (fixedCount() > 0)
        {
            QRect rect = d->viewportScrollArea();
            d->viewport->scroll(isRightToLeft() ? -ndelta : ndelta, 0, rect);
        }
        else
        {
            d->viewport->scroll(isRightToLeft() ? -ndelta : ndelta, 0);
        }
    }
    else
    {
        if (fixedCount() > 0)
        {
            QRect rect = d->viewportScrollArea();
            d->viewport->scroll(0, ndelta, rect);
        }
        else
        {
            d->viewport->scroll(0, ndelta);
        }
    }

    if (d->m_state == GlodonHeaderViewPrivate::ResizeSection && !d->preventCursorChangeInSetOffset)
    {
        QPoint cursorPos = QCursor::pos();

        if (d->orientation == Qt::Horizontal)
        {
            QCursor::setPos(cursorPos.x() + ndelta, cursorPos.y());
        }
        else
        {
            QCursor::setPos(cursorPos.x(), cursorPos.y() + ndelta);
        }

        d->firstPos += ndelta;
        d->lastPos += ndelta;
    }

    d->viewport->update();

    emit scrolled(newOffset);
}

void GlodonHeaderView::setOffsetToSectionPosition(int visualSectionNumber)
{
    Q_D(GlodonHeaderView);

    if (visualSectionNumber > -1 && visualSectionNumber < d->sectionCount())
    {
        int nAdjustedVisualIndex = d->adjustedVisualIndex(visualSectionNumber);
        int nPosition = 0;

        if (d->fixedCount < 1)
        {
            nPosition = d->headerSectionPosition(nAdjustedVisualIndex);
        }
        else
        {
            // 有固定行列存在时每次移动固定行列后面的格子大小
            nPosition = d->headerSectionPosition(nAdjustedVisualIndex + d->fixedCount);
            nPosition -= d->headerSectionPosition(d->fixedCount);
        }

        if (nPosition < 0)
        {
            return;
        }

        setOffset(nPosition);
    }
}

void GlodonHeaderView::setOffsetToLastSection()
{
    Q_D(const GlodonHeaderView);
    int nsize = (d->orientation == Qt::Horizontal ? viewport()->width() : viewport()->height());
    int nposition = length() - nsize;
    setOffset(nposition);
}

int GlodonHeaderView::length() const
{
    Q_D(const GlodonHeaderView);
    d->executePostedLayout();
    return d->length;
}

int GlodonHeaderView::drawWidth() const
{
    Q_D(const GlodonHeaderView);
    int ndrawWidth = 0;

    if (isAllSectionHidden())
    {
        return 0;
    }

    if (!isHidden())
    {
        if (d->orientation == Qt::Horizontal)
        {
            ndrawWidth = qMax(minimumHeight(), sizeHint().height());
            ndrawWidth = qMin(ndrawWidth, maximumHeight());
        }
        else
        {
            ndrawWidth = qMax(minimumWidth(), sizeHint().width());
            ndrawWidth = qMin(ndrawWidth, maximumWidth());
        }
    }

    return qMax(ndrawWidth, d->drawWidth);
}

int GlodonHeaderView::sectionCount() const
{
    Q_D(const GlodonHeaderView);
    return d->sectionCount();
}

QSize GlodonHeaderView::sizeHint() const
{
    Q_D(const GlodonHeaderView);

    if (d->cachedSizeHint.isValid())
    {
        return d->cachedSizeHint;
    }

    d->cachedSizeHint = QSize(0, 0); //reinitialize the cached size hint
    const int c_sectionCount = count();

    // get size hint for the first n sections
    int nIndex = 0;

    for (int i = 0; i < 100 && nIndex < c_sectionCount; ++nIndex)
    {
        if (isSectionHidden(nIndex))
        {
            continue;
        }
        else
        {
            // 为了提高计算SizeHint的效率，只计算第一个和后十个非隐藏section的值
            i++;
            QSize hint = sectionSizeFromContents(nIndex);
            d->cachedSizeHint = d->cachedSizeHint.expandedTo(hint);
            break;
        }
    }

    // get size hint for the last n sections
    nIndex = qMax(nIndex, c_sectionCount - 100);
    int nchecked = 0;
//    int nCalcCount = 0;

    for (int j = c_sectionCount - 1; j >= nIndex && nchecked < 100; --j)
    {
        if (isSectionHidden(j))
        {
            continue;
        }
        else
        {
            nchecked++;

            QString oHeaderText;

            if (d->autoSectionData)
            {
                oHeaderText = QString::number(j + 1);
            }
            else
            {
                oHeaderText = d->m_model->headerData(j, d->orientation,
                                                  Qt::DisplayRole).toString();
            }

            if (oHeaderText.isEmpty())
            {
                continue;
            }

            QSize hint = sectionSizeFromContents(j);
            d->cachedSizeHint = d->cachedSizeHint.expandedTo(hint);

//            if (nCalcCount++ >= 10)
//            {
//                break;
//            }
        }
    }

    return d->cachedSizeHint;
}

void GlodonHeaderView::setVisible(bool v)
{
    bool actualChange = (v != isVisible());

    GlodonAbstractItemView::setVisible(v);

    if (actualChange)
    {
        QAbstractScrollArea *parent = qobject_cast<QAbstractScrollArea *>(parentWidget());

        if (parent)
        {
            parent->updateGeometry();
        }
    }
}

int GlodonHeaderView::sectionSizeHint(int logicalIndex) const
{
    Q_D(const GlodonHeaderView);

    if (isSectionHidden(logicalIndex))
    {
        return 0;
    }

    if (isInValidLogicalIndex(logicalIndex))
    {
        return -1;
    }

    QSize size;
    QVariant value = d->m_model->headerData(logicalIndex, d->orientation, Qt::SizeHintRole);

    if (value.isValid())
    {
        if (d->autoSectionData)
        {
            d->m_model->setHeaderData(logicalIndex, d->orientation, logicalIndex + 1);
        }
        else
        {
            size = qvariant_cast<QSize>(value);
        }
    }
    else
    {
        size = sectionSizeFromContents(logicalIndex);
    }

    int nhint = d->orientation == Qt::Horizontal ? size.width() : size.height();
    return qMax(minimumSectionSize(), nhint);
}

int GlodonHeaderView::visualIndexAt(int position) const
{
    Q_D(const GlodonHeaderView);

    d->executePostedLayout();

    const int count = sectionCount();

    if (count < 1)
    {
        return -1;
    }

    // TODO: refactoring fixed column by separating method
    for (int i = 0, nvposition = position; i < d->fixedCount; i++)
    {
        if (!isSectionHidden(i))
        {
            nvposition = nvposition - sectionSize(i) - d->gridLineWidth;
        }

        if (nvposition <= 0)
        {
            return i;
        }
    }

    if (d->reverse())
    {
        position = d->viewport->width() - position;
    }

    position += d->offset;

    // TODO: refactoring mouse moving state check by separating method
    GlodonAbstractItemView *pParent = dynamic_cast<GlodonAbstractItemView *>(parentWidget());

    if (pParent && pParent->dragSelectingState())
    {
        if (position > d->length)
        {
            return count - 1;
        }
        else if (position < 0)
        {
            return 0;
        }
    }

    if (position > d->length)
    {
        return -1;
    }

    int nVisual = d->headerVisualIndexAt(position);

    if (nVisual < 0)
    {
        return -1;
    }

    while (d->isVisualIndexHidden(nVisual))
    {
        ++nVisual;

        if (nVisual >= count)
        {
            return -1;
        }
    }

    return nVisual;
}

int GlodonHeaderView::logicalIndexAt(int position) const
{
    const int c_visual = visualIndexAt(position);

    if (c_visual > -1)
    {
        return logicalIndex(c_visual);
    }

    return -1;
}



int GlodonHeaderView::sectionSize(int logicalIndex) const
{
    if (isSectionHidden(logicalIndex))
    {
        return 0;
    }

    if (isInValidLogicalIndex(logicalIndex))
    {
        return 0;
    }

    int nVisualIndex = visualIndex(logicalIndex);
//    if (visual == -1)
//        return 0;
//    d->executePostedResize();
//    if (GlodonAbstractItemModel *model = dynamic_cast<GlodonAbstractItemModel *>(d->model))
//    {
//        int size = -1;
//        if (d->orientation == Qt::Horizontal)
//        {
//            QModelIndex index = model->index(0, logicalIndex);
//            size = model->data(index, gidrColWidthRole).toInt();
//        }
//        else
//        {
//            QModelIndex index = model->index(logicalIndex, 0);
//            size = model->data(index, gidrRowHeightRole).toInt();
//        }
//    }
//    return d->headerSectionSize(visual);
    return visualSectionSize(nVisualIndex);
}

int GlodonHeaderView::visualSectionSize(int visualIndex) const
{
    Q_D(const GlodonHeaderView);

    if (visualIndex == -1)
    {
        return 0;
    }

    return d->headerSectionSize(visualIndex);
}

int GlodonHeaderView::sectionPosition(int logicalIndex) const
{
    int nVisualIndex = visualIndex(logicalIndex);
    return sectionVisualPosition(nVisualIndex);
//    // in some cases users may change the selections
//    // before we have a chance to do the layout
//    if (visual == -1)
//        return -1;
//    d->executePostedResize();
//    return d->headerSectionPosition(visual);
}

int GlodonHeaderView::sectionViewportPosition(int logicalIndex) const
{
    Q_D(const GlodonHeaderView);

    if (logicalIndex >= count())
    {
        return -1;
    }

    int nposition = sectionPosition(logicalIndex);

    if (nposition < 0)
    {
        return nposition;    // the section was hidden
    }

    int noffsetPosition;

    if (logicalIndex < d->fixedCount)
    {
        noffsetPosition = nposition;
    }
    else
    {
        noffsetPosition = nposition - d->offset;
    }

    return noffsetPosition;
}

int GlodonHeaderView::sectionVisualPosition(int visualIndex) const
{
    Q_D(const GlodonHeaderView);

    if (visualIndex == -1)
    {
        return -1;
    }

    return d->headerSectionPosition(visualIndex);
}

void GlodonHeaderView::moveSection(int from, int to)
{
    Q_D(GlodonHeaderView);

    d->executePostedLayout();

    if (from < 0 || from >= d->sectionCount() || to < 0 || to >= d->sectionCount())
    {
        return;
    }

    if (from == to)
    {
        int nLogical = logicalIndex(from);
        Q_ASSERT(nLogical != -1);
        updateSection(nLogical);
        return;
    }

    //int oldHeaderLength = length(); // ### for debugging; remove later
    d->initializeIndexMapping();

    QBitArray sectionHidden = d->sectionHidden;
    int *visualIndices = d->visualIndices.data();
    int *logicalIndices = d->logicalIndices.data();
    int nlogical = logicalIndices[from];
    int nVisualIndex = from;

    int naffected_count = qAbs(to - from) + 1;
    QVarLengthArray<int> sizes(naffected_count);
    QVarLengthArray<ResizeMode> modes(naffected_count);

    // move sections and indices
    if (to > from)
    {
        sizes[to - from] = d->headerSectionSize(from);
        modes[to - from] = d->headerSectionResizeMode(from);

        while (nVisualIndex < to)
        {
            sizes[nVisualIndex - from] = d->headerSectionSize(nVisualIndex + 1);
            modes[nVisualIndex - from] = d->headerSectionResizeMode(nVisualIndex + 1);

            if (!sectionHidden.isEmpty())
            {
                sectionHidden.setBit(nVisualIndex, sectionHidden.testBit(nVisualIndex + 1));
            }

            visualIndices[logicalIndices[nVisualIndex + 1]] = nVisualIndex;
            logicalIndices[nVisualIndex] = logicalIndices[nVisualIndex + 1];
            ++nVisualIndex;
        }
    }
    else
    {
        sizes[0] = d->headerSectionSize(from);
        modes[0] = d->headerSectionResizeMode(from);

        while (nVisualIndex > to)
        {
            sizes[nVisualIndex - to] = d->headerSectionSize(nVisualIndex - 1);
            modes[nVisualIndex - to] = d->headerSectionResizeMode(nVisualIndex - 1);

            if (!sectionHidden.isEmpty())
            {
                sectionHidden.setBit(nVisualIndex, sectionHidden.testBit(nVisualIndex - 1));
            }

            visualIndices[logicalIndices[nVisualIndex - 1]] = nVisualIndex;
            logicalIndices[nVisualIndex] = logicalIndices[nVisualIndex - 1];
            --nVisualIndex;
        }
    }

    if (!sectionHidden.isEmpty())
    {
        sectionHidden.setBit(to, d->sectionHidden.testBit(from));
        d->sectionHidden = sectionHidden;
    }

    visualIndices[nlogical] = to;
    logicalIndices[to] = nlogical;

    if (to > from)
    {
        for (nVisualIndex = from; nVisualIndex <= to; ++nVisualIndex)
        {
            int nsize = sizes[nVisualIndex - from];
            ResizeMode mode = modes[nVisualIndex - from];
            d->createSectionItems(nVisualIndex, nVisualIndex, nsize, mode);
        }
    }
    else
    {
        for (nVisualIndex = to; nVisualIndex <= from; ++nVisualIndex)
        {
            int nsize = sizes[nVisualIndex - to];
            ResizeMode mode = modes[nVisualIndex - to];
            d->createSectionItems(nVisualIndex, nVisualIndex, nsize, mode);
        }
    }

    d->viewport->update();

    emit sectionMoved(nlogical, from, to);
}

void GlodonHeaderView::swapSections(int first, int second)
{
    Q_D(GlodonHeaderView);

    if (first == second)
    {
        return;
    }

    d->executePostedLayout();

    if (first < 0 || first >= d->sectionCount() || second < 0 || second >= d->sectionCount())
    {
        return;
    }

    int nFirstSize = d->headerSectionSize(first);
    ResizeMode firstMode = d->headerSectionResizeMode(first);
    int nFirstLogical = d->logicalIndex(first);

    int nSecondSize = d->headerSectionSize(second);
    ResizeMode secondMode = d->headerSectionResizeMode(second);
    int nSecondLogical = d->logicalIndex(second);

    if (d->m_state == GlodonHeaderViewPrivate::ResizeSection)
    {
        d->preventCursorChangeInSetOffset = true;
    }

    d->createSectionItems(second, second, nFirstSize, firstMode);
    d->createSectionItems(first, first, nSecondSize, secondMode);

    d->initializeIndexMapping();

    d->visualIndices[nFirstLogical] = second;
    d->logicalIndices[second] = nFirstLogical;

    d->visualIndices[nSecondLogical] = first;
    d->logicalIndices[first] = nSecondLogical;

    if (!d->sectionHidden.isEmpty())
    {
        bool bfirstHidden = d->sectionHidden.testBit(first);
        bool bsecondHidden = d->sectionHidden.testBit(second);
        d->sectionHidden.setBit(first, bsecondHidden);
        d->sectionHidden.setBit(second, bfirstHidden);
    }

    d->viewport->update();
    emit sectionMoved(nFirstLogical, first, second);
    emit sectionMoved(nSecondLogical, second, first);
}

void GlodonHeaderView::resizeSection(int logical, int size, bool update, bool isManual)
{
    Q_D(GlodonHeaderView);

    if (logical < 0 || logical >= count() || size < 0 ||  size > maxSizeSection)
    {
        return;
    }

    if (isSectionHidden(logical))
    {
        d->hiddenSectionSize.insert(logical, size);
        return;
    }

    int nVisual = visualIndex(logical);

    if (nVisual == -1)
    {
        return;
    }

    if (d->m_state == GlodonHeaderViewPrivate::ResizeSection && !d->cascadingResizing && logical != d->section)
    {
        d->preventCursorChangeInSetOffset = true;
    }

    int nOldSize = d->headerSectionSize(nVisual);

    if (nOldSize == size)
    {
        return;
    }

    d->executePostedLayout();
    d->invalidateCachedSizeHint();

    GlodonHeaderView::ResizeMode mode = d->headerSectionResizeMode(nVisual);

    d->createSectionItems(nVisual, nVisual, size, mode);

    if (!updatesEnabled())
    {
        emit sectionResized(logical, nOldSize, size, isManual);
        return;
    }

    int nw = d->viewport->width();
    int nh = d->viewport->height();
    int npos = sectionViewportPosition(logical);
    QRect tmpRect;

    if (d->orientation == Qt::Horizontal)
    {
        if (isRightToLeft())
        {
            tmpRect.setRect(0, 0, npos + size, nh);
        }
        else
        {
            tmpRect.setRect(npos, 0, nw - npos, nh);
        }
    }
    else
    {
        tmpRect.setRect(0, npos, nw, nh - npos);
    }

    if (update)
    {
        d->viewport->update(tmpRect.normalized());
        emit sectionResized(logical, nOldSize, size, isManual);
    }
}

void GlodonHeaderView::setSectionSpanSeparate(bool value)
{
    Q_D(GlodonHeaderView);
    d->m_createSectionSpanSeparate = value;
}

bool GlodonHeaderView::isSectionSpanSeparate()
{
    Q_D(GlodonHeaderView);
    return d->m_createSectionSpanSeparate;
}

bool GlodonHeaderView::isSectionHidden(int logicalIndex) const
{
    Q_D(const GlodonHeaderView);
    d->executePostedLayout();

    if (logicalIndex >= d->sectionHidden.count() || logicalIndex < 0 || logicalIndex >= d->sectionCount())
    {
        return false;
    }

    int nVisualIndex = visualIndex(logicalIndex);
    Q_ASSERT(nVisualIndex != -1);
    return d->sectionHidden.testBit(nVisualIndex);
}

GlodonHeaderView::VisibleState GlodonHeaderView::visibleState(int logicalIndex) const
{
    Q_D(const GlodonHeaderView);

    d->executePostedLayout();

    QRect rect = this->geometry();
    int nleft = sectionViewportPosition(logicalIndex);
    int nright = nleft + sectionSize(logicalIndex);
    int nrectLeft = rect.left();
    int nrectRight = rect.right();

    if (d->orientation == Qt::Vertical)
    {
        nrectLeft = rect.top();
        nrectRight = rect.bottom();
    }

    int nstate = 3;

    if (nleft > nrectLeft && nright < nrectRight)
    {
        nstate = 2;    //  whole section is visible
    }
    else if (nleft < nrectLeft && nright >= nrectLeft)
    {
        nstate = -1;
    }
    else if (nright > nrectRight && nleft <= nrectRight)
    {
        nstate = 1;
    }
    else if (nleft > nrectRight)
    {
        nstate = 0;
    }
    else if (nright < nrectLeft)
    {
        nstate = 0;
    }
    else        //非常可怕的情况，一个section占据了整个宽度/高度
    {
        Q_ASSERT(false);
    }

    return GlodonHeaderView::VisibleState(nstate);
}

int GlodonHeaderView::hiddenSectionCount() const
{
    Q_D(const GlodonHeaderView);
    return d->hiddenSectionSize.count();
}

void GlodonHeaderView::setSectionHidden(int logicalIndex, bool hide)
{
    Q_D(GlodonHeaderView);

    if (isInValidLogicalIndex(logicalIndex))
    {
        return;
    }

    d->executePostedLayout();
    int nVisualIndex = visualIndex(logicalIndex);
    Q_ASSERT(nVisualIndex != -1);

    if (hide == d->isVisualIndexHidden(nVisualIndex))
    {
        return;
    }

    if (hide)
    {
        int nsize = d->headerSectionSize(nVisualIndex);
        if (nsize == 0)
        {
            d->hiddenSectionSize.insert(logicalIndex, d->defaultSectionSize);
        }
        else
        {
            d->hiddenSectionSize.insert(logicalIndex, nsize);
        }

        resizeSection(logicalIndex, 0);

        if (d->sectionHidden.count() < count())
        {
            d->sectionHidden.resize(count());
        }

        d->sectionHidden.setBit(nVisualIndex, true);
    }
    else
    {
        int nsize = d->hiddenSectionSize.value(logicalIndex, d->defaultSectionSize);

        d->hiddenSectionSize.remove(logicalIndex);

        if (d->hiddenSectionSize.isEmpty())
        {
            d->sectionHidden.clear();
        }
        else
        {
            Q_ASSERT(nVisualIndex <= d->sectionHidden.count());
            d->sectionHidden.setBit(nVisualIndex, false);
        }

        if (nsize == 0)
        {
            nsize = d->defaultSectionSize;
        }

        resizeSection(logicalIndex, nsize);
    }
}

void GlodonHeaderView::batchSetSectionHidden(QSet<int> logicalIndexs, bool hide)
{
    Q_D(GlodonHeaderView);

    d->executePostedLayout();

    QSet<QPair<int, int>> oNeedHideLogicalAndVisualIndexs;
    d->excludeNotNeedHideSections(logicalIndexs, hide, oNeedHideLogicalAndVisualIndexs);

    if (oNeedHideLogicalAndVisualIndexs.isEmpty())
        return;

    QMap<int, int> oNeedResizeSections;

    if (hide)
    {
        if (d->sectionHidden.count() < count())
        {
            d->sectionHidden.resize(count());
        }

        QSet<QPair<int, int>>::iterator oNeedHideIndexIte = oNeedHideLogicalAndVisualIndexs.begin();
        while (oNeedHideIndexIte != oNeedHideLogicalAndVisualIndexs.end())
        {
            QPair<int, int> nCurIndexPair = *oNeedHideIndexIte;

            oNeedResizeSections.insert(nCurIndexPair.first, 0);
            d->sectionHidden.setBit(nCurIndexPair.second, true);
            d->hiddenSectionSize.insert(nCurIndexPair.first, d->headerSectionSize(nCurIndexPair.second));

            oNeedHideIndexIte++;
        }
    }
    else
    {
        QSet<QPair<int, int>>::iterator oNeedHideIndexIte = oNeedHideLogicalAndVisualIndexs.begin();
        while (oNeedHideIndexIte != oNeedHideLogicalAndVisualIndexs.end())
        {
            QPair<int, int> nCurIndexPair = *oNeedHideIndexIte;

            int nCurIndexSize = d->hiddenSectionSize.value(nCurIndexPair.first, d->defaultSectionSize);
            oNeedResizeSections.insert(nCurIndexPair.first, nCurIndexSize);

            d->sectionHidden.setBit(nCurIndexPair.second, false);
            d->hiddenSectionSize.remove(nCurIndexPair.first);
            oNeedHideIndexIte++;
        }

        if (d->hiddenSectionSize.isEmpty())
        {
            d->sectionHidden.clear();
        }
    }

    d->doBatchResizeSection(oNeedResizeSections, true);
}

int GlodonHeaderView::count() const
{
    Q_D(const GlodonHeaderView);
    d->executePostedLayout();
    return d->sectionCount();
}

int GlodonHeaderView::visualIndex(int logicalIndex) const
{
    Q_D(const GlodonHeaderView);

    if (logicalIndex < 0)
    {
        return -1;
    }

    d->executePostedLayout();

    if (d->visualIndices.isEmpty())
    {
        // nothing has been moved, so we have no mapping
        if (logicalIndex < d->sectionCount())
        {
            return logicalIndex;
        }
    }
    else if (logicalIndex < d->visualIndices.count())
    {
        int nVisual = d->visualIndices.at(logicalIndex);
        Q_ASSERT(nVisual < d->sectionCount());
        return nVisual;
    }

    return -1;
}

int GlodonHeaderView::logicalIndex(int visualIndex) const
{
    Q_D(const GlodonHeaderView);

    if (visualIndex < 0 || visualIndex >= d->sectionCount())
    {
        return -1;
    }

    return d->logicalIndex(visualIndex);
}

// ### Qt 5: change to setSectionsMovable()
void GlodonHeaderView::setMovable(bool movable)
{
    Q_D(GlodonHeaderView);

    if ((d->isTree) && (d->orientation == Qt::Vertical))
    {
        return;
    }

    d->movableSections = movable;
}

// ### Qt 5: change to sectionsMovable()
bool GlodonHeaderView::isMovable() const
{
    Q_D(const GlodonHeaderView);
    return d->movableSections;
}

// ### Qt 5: change to setSectionsClickable()
void GlodonHeaderView::setClickable(bool clickable)
{
    Q_D(GlodonHeaderView);
    d->clickableSections = clickable;
}

// ### Qt 5: change to sectionsClickable()
bool GlodonHeaderView::isClickable() const
{
    Q_D(const GlodonHeaderView);
    return d->clickableSections;
}

void GlodonHeaderView::setHighlightSections(bool highlight)
{
    Q_D(GlodonHeaderView);
    d->highlightSelected = highlight;
}

bool GlodonHeaderView::highlightSections() const
{
    Q_D(const GlodonHeaderView);
    return d->highlightSelected;
}

void GlodonHeaderView::setFixedCellColor(const QColor &value, int index)
{
    if (index == -1)
    {
        for (int i = 0; i < count(); i++)
        {
            inserFixedCellColor(value, i);
        }
    }
    else
    {
        Q_ASSERT((index >= 0) && (index < count()));
        inserFixedCellColor(value, index);
    }
}

void GlodonHeaderView::inserFixedCellColor(const QColor &value, int index)
{
    Q_D(GlodonHeaderView);
    GlodonCellAttr attr;
    attr.orientation = d->orientation;
    attr.background = value;
    d->fixedCellsColor[index] = attr;
}

void GlodonHeaderView::setFixedCellEvent(bool value)
{
    Q_D(GlodonHeaderView);

    if (d->fixedCellEvent != value)
    {
        d->fixedCellEvent = value;
    }
}

bool GlodonHeaderView::isFixedcellEvent() const
{
    Q_D(const GlodonHeaderView);
    return d->fixedCellEvent;
}

void GlodonHeaderView::setGridLineWidth(int value)
{
    Q_D(GlodonHeaderView);

    if (d->fixedCellEvent)
    {
        // 设置格线之后，需要重新计算length和每个Section的StartPos
        d->length += (value - d->gridLineWidth) * sectionCount();
        d->gridLineWidth = value;

        d->recalcSectionStartPos();
        d->viewport->update();
    }
}

void GlodonHeaderView::setGridLineColor(QColor value)
{
    Q_D(GlodonHeaderView);

    if (d->gridLineColor != value)
    {
        d->gridLineColor = value;
        d->viewport->update();
    }
}

void GlodonHeaderView::setResizeMode(ResizeMode mode)
{
    Q_D(GlodonHeaderView);

    initializeSections();

    d->setGlobalHeaderResizeMode(mode);
}

// ### Qt 5: change to setSectionResizeMode()
void GlodonHeaderView::setResizeMode(int logicalIndex, ResizeMode mode)
{
    Q_D(GlodonHeaderView);

    int nVisualIndex = visualIndex(logicalIndex);
    Q_ASSERT(nVisualIndex != -1);

    d->setHeaderSectionResizeMode(nVisualIndex, mode);
}

GlodonHeaderView::ResizeMode GlodonHeaderView::resizeMode() const
{
    Q_D(const GlodonHeaderView);
    return d->globalResizeMode;
}

GlodonHeaderView::ResizeMode GlodonHeaderView::resizeMode(int logicalIndex) const
{
    Q_D(const GlodonHeaderView);
    int nVisualIndex = visualIndex(logicalIndex);

    if (nVisualIndex == -1)
    {
        return Fixed;    //the default value
    }

    return d->headerSectionResizeMode(nVisualIndex);
}

void GlodonHeaderView::setSortIndicatorShown(bool show)
{
    Q_D(GlodonHeaderView);

    if (d->sortIndicatorShown == show)
    {
        return;
    }

    d->sortIndicatorShown = show;

    if (sortIndicatorSection() < 0 || sortIndicatorSection() > count())
    {
        return;
    }

    d->viewport->update();
}

bool GlodonHeaderView::isSortIndicatorShown() const
{
    Q_D(const GlodonHeaderView);
    return d->sortIndicatorShown;
}

void GlodonHeaderView::setSortIndicator(int logicalIndex, Qt::SortOrder order)
{
    Q_D(GlodonHeaderView);

    // This is so that people can set the position of the sort indicator before the fill the model
    int nold = d->sortIndicatorSection;
    d->sortIndicatorSection = logicalIndex;
    d->sortIndicatorOrder = order;

    if (logicalIndex >= d->sectionCount())
    {
        emit sortIndicatorChanged(logicalIndex, order);
        return; // nothing to do
    }

    if (nold >= 0 && nold != logicalIndex)
    {
        updateSection(nold);
    }

    if (logicalIndex >= 0)
    {
        updateSection(logicalIndex);
    }

    emit sortIndicatorChanged(logicalIndex, order);
}

int GlodonHeaderView::sortIndicatorSection() const
{
    Q_D(const GlodonHeaderView);
    return d->sortIndicatorSection;
}

void GlodonHeaderView::setSortIndicatorSection(int logicalIndex)
{
    Q_D(GlodonHeaderView);
    d->sortIndicatorSection = logicalIndex;
}

Qt::SortOrder GlodonHeaderView::sortIndicatorOrder() const
{
    Q_D(const GlodonHeaderView);
    return d->sortIndicatorOrder;
}

void GlodonHeaderView::setSortIndicatorOrder(Qt::SortOrder order)
{
    Q_D(GlodonHeaderView);
    d->sortIndicatorOrder = order;
}

bool GlodonHeaderView::cascadingSectionResizes() const
{
    Q_D(const GlodonHeaderView);
    return d->cascadingResizing;
}

void GlodonHeaderView::setCascadingSectionResizes(bool enable)
{
    Q_D(GlodonHeaderView);
    d->cascadingResizing = enable;
}

int GlodonHeaderView::defaultSectionSize() const
{
    Q_D(const GlodonHeaderView);
    return d->defaultSectionSize;
}

void GlodonHeaderView::setDefaultSectionSize(int size)
{
    Q_D(GlodonHeaderView);

    if (size < 0 || size > maxSizeSection)
    {
        return;
    }

    d->setDefaultSectionSize(size);
}

int GlodonHeaderView::minimumSectionSize() const
{
    Q_D(const GlodonHeaderView);

    if (d->minimumSectionSize == -1)
    {
        QSize strut = QApplication::globalStrut();
        int nMargin = style()->pixelMetric(QStyle::PM_HeaderMargin, 0, this);

        if (d->orientation == Qt::Horizontal)
        {
            return qMax(strut.width(), (fontMetrics().maxWidth() + nMargin));
        }

        return qMax(strut.height(), (fontMetrics().height() + nMargin));
    }

    return d->minimumSectionSize;
}

void GlodonHeaderView::setMinimumSectionSize(int size)
{
    Q_D(GlodonHeaderView);

    if (size < -1 || size > maxSizeSection)
    {
        return;
    }

    d->minimumSectionSize = size;
}

bool GlodonHeaderView::isTree() const
{
    Q_D(const GlodonHeaderView);
    return d->isTree;
}

void GlodonHeaderView::setIsTree(bool showTree)
{
    Q_D(GlodonHeaderView);

    if (showTree != d->isTree)
    {
        d->isTree = showTree;

        if (d->orientation == Qt::Vertical)
        {
            d->clear();
            initializeSections();
        }

        if (showTree)
        {
            d->movableSections = !showTree;
        }
    }
}

bool GlodonHeaderView::autoSectionData() const
{
    Q_D(const GlodonHeaderView);
    return d->autoSectionData;
}

void GlodonHeaderView::setAutoSectionData(bool autoData)
{
    Q_D(GlodonHeaderView);

    if (d->autoSectionData != autoData)
    {
        d->autoSectionData = autoData;
        d->viewport->update();
    }
}

int GlodonHeaderView::fixedCount() const
{
    Q_D(const GlodonHeaderView);
    return d->fixedCount;
}

void GlodonHeaderView::setFixedCount(int fixedCount)
{
    Q_D(GlodonHeaderView);
    d->fixedCount = fixedCount;
}

Qt::Alignment GlodonHeaderView::defaultAlignment() const
{
    Q_D(const GlodonHeaderView);
    return d->defaultAlignment;
}

void GlodonHeaderView::setDefaultAlignment(Qt::Alignment alignment)
{
    Q_D(GlodonHeaderView);

    if (d->defaultAlignment == alignment)
    {
        return;
    }

    d->defaultAlignment = alignment;
    d->viewport->update();
}

void GlodonHeaderView::doItemsLayout()
{
    initializeSections();
    GlodonAbstractItemView::doItemsLayout();
}

bool GlodonHeaderView::sectionsMoved() const
{
    Q_D(const GlodonHeaderView);
    return !d->visualIndices.isEmpty();
}

void GlodonHeaderView::clearSectionMove()
{
    Q_D(const GlodonHeaderView);
    d->visualIndices.clear();
    d->logicalIndices.clear();
}

bool GlodonHeaderView::sectionsHidden() const
{
    Q_D(const GlodonHeaderView);
    return !d->hiddenSectionSize.isEmpty();
}

int GlodonHeaderView::getSectionHandleAt(int position)
{
    Q_D(GlodonHeaderView);
    return d->sectionHandleAt(position);
}

#ifndef QT_NO_DATASTREAM

QByteArray GlodonHeaderView::saveState() const
{
    Q_D(const GlodonHeaderView);
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << GlodonHeaderViewPrivate::VersionMarker;
    stream << 0; // current version is 0
    d->write(stream);
    return data;
}

bool GlodonHeaderView::restoreState(const QByteArray &state)
{
    Q_D(GlodonHeaderView);

    if (state.isEmpty())
    {
        return false;
    }

    QByteArray data = state;
    QDataStream stream(&data, QIODevice::ReadOnly);
    int nmarker;
    int nver;
    stream >> nmarker;
    stream >> nver;

    if (stream.status() != QDataStream::Ok
            || nmarker != GlodonHeaderViewPrivate::VersionMarker
            || nver != 0) // current version is 0
    {
        return false;
    }

    if (d->read(stream))
    {
        emit sortIndicatorChanged(d->sortIndicatorSection, d->sortIndicatorOrder);
        d->viewport->update();
        return true;
    }

    return false;
}

#endif // QT_NO_DATASTREAM

void GlodonHeaderView::reset()
{
    GlodonAbstractItemView::reset();

    // it would be correct to call clear, but some apps rely
    // on the header keeping the sections, even after calling reset
    //d->clear();
    initializeSections();
}

void GlodonHeaderView::resetAfterTreeBuild(const QVector<int> &showRows)
{
    Q_D(GlodonHeaderView);
    int nOldCount = d->sectionCount();
    d->resetAfterTreeBuild(showRows);
    int nNewCount = d->sectionCount();

    emit sectionCountChanged(nOldCount, nNewCount);
}

void GlodonHeaderViewPrivate::recalcSectionStartPos() const // linear (but fast)
{
    int pixelpos = 0;
    int nPreSize = 0;

    for (QVector<SectionItem>::const_iterator i = sectionItems.constBegin(); i != sectionItems.constEnd(); ++i)
    {
        if (nPreSize != 0)
        {
            pixelpos += gridLineWidth;
        }

        i->calculated_startpos = pixelpos; // write into const mutable

        pixelpos += i->size;
        nPreSize = i->size;
    }

    sectionStartposRecalc = false;
}

void GlodonHeaderView::resetSectionItems()
{
    Q_D(GlodonHeaderView);
    GlodonAbstractItemView::reset();
    d->clear();
    initializeSections();
}

void GlodonHeaderView::zoomInSection(double factor)
{
    Q_D(GlodonHeaderView);

    if (d->sectionItems.count() <= 0)
    {
        return;
    }

    if (d->m_srcFontPointSize == -1)
    {
        d->m_srcFontPointSize = font().pointSize();
    }

    QFont currentFont = font();
    currentFont.setPointSize(d->m_srcFontPointSize * factor);
    setFont(currentFont);

    int nPreSrcLength = 0;
    int npreLength = 0;

    for (int i = 0; i < d->sectionItems.count(); ++i)
    {
        GlodonHeaderViewPrivate::SectionItem curSpan = d->sectionItems.at(i);

        if (curSpan.srcSize == -1)
        {
            curSpan.srcSize = curSpan.size;
        }

        nPreSrcLength += curSpan.srcSize;
        curSpan.size = nPreSrcLength * factor - npreLength;
        npreLength += curSpan.size;
        d->sectionItems.replace(i, curSpan);
    }

    if (d->m_srcLength == -1 || d->m_srcLength == 0)
    {
        d->m_srcLength = d->length;
    }

    d->length = factor * d->m_srcLength;
    d->viewport->update();
}

void GlodonHeaderView::setInZoomingSection(bool zooming)
{
    d_func()->m_inZoomForceResize = zooming;
}

bool GlodonHeaderView::inZoomingSection() const
{
    return d_func()->m_inZoomForceResize;
}

void GlodonHeaderView::setLogicalSectionMovable(bool isManual)
{
    d_func()->m_logicalMovable = isManual;
}

bool GlodonHeaderView::isLogicalSectionMovable() const
{
    return d_func()->m_logicalMovable;
}

bool GlodonHeaderView::drawBorder() const
{
    return d_func()->m_drawBorder;
}

void GlodonHeaderView::setDrawBorder(bool value)
{
    d_func()->m_drawBorder = value;
}

bool GlodonHeaderView::allowDoubleClicked() const
{
    return d_func()->m_allowDoubleClicked;
}

void GlodonHeaderView::setAllowDoubleClicked(bool value)
{
    d_func()->m_allowDoubleClicked = value;
}

bool GlodonHeaderView::boldWithSelected()
{
    return d_func()->m_boldWithSelected;
}

void GlodonHeaderView::setBoldWithSelected(bool value)
{
    d_func()->m_boldWithSelected = value;
}

bool GlodonHeaderView::isFontShadow() const
{
    return d_func()->m_bFontShadow;
}

void GlodonHeaderView::setFontShadow(bool fontShadow)
{
    d_func()->m_bFontShadow = fontShadow;
}

QColor GlodonHeaderView::fontShadowColor() const
{
    return d_func()->m_clrFontShadow;
}

void GlodonHeaderView::setFontShadowColor(const QColor &clr)
{
    d_func()->m_clrFontShadow = clr;
}

bool GlodonHeaderView::isHeaderLinearGradient() const
{
    return d_func()->m_bLinearGradient;
}

void GlodonHeaderView::setHeaderLinearGradient(bool value)
{
    d_func()->m_bLinearGradient = value;
}

void GlodonHeaderView::headerDataChanged(Qt::Orientation orientation, int logicalFirst, int logicalLast)
{
    Q_D(GlodonHeaderView);

    if (d->orientation != orientation)
    {
        return;
    }

    if (logicalFirst < 0 || logicalLast < 0 || logicalFirst >= count() || logicalLast >= count())
    {
        return;
    }

    d->invalidateCachedSizeHint();

    int nfirstVisualIndex = INT_MAX;
    int nlastVisualIndex = -1;

    for (int i = logicalFirst; i <= logicalLast; ++i)
    {
        const int c_visual = visualIndex(i);
        nfirstVisualIndex = qMin(nfirstVisualIndex, c_visual);
        nlastVisualIndex = qMax(nlastVisualIndex,  c_visual);
    }

    const int c_first = d->headerSectionPosition(nfirstVisualIndex);
    const int c_last = d->headerSectionPosition(nlastVisualIndex)
                       + d->headerSectionSize(nlastVisualIndex);

    if (orientation == Qt::Horizontal)
    {
        d->viewport->update(c_first, 0, c_last - c_first, d->viewport->height());
    }
    else
    {
        d->viewport->update(0, c_first, d->viewport->width(), c_last - c_first);
    }
}

void GlodonHeaderView::setDrawWidth(int drawWidth)
{
    Q_D(GlodonHeaderView);

    if (drawWidth >= 0 && drawWidth != d->drawWidth)
    {
        d->drawWidth = drawWidth;
        d->viewport->update();
        emit drawWidthChanged(drawWidth);
    }
}

void GlodonHeaderView::updateSection(int index)
{
    Q_D(GlodonHeaderView);

    if (d->orientation == Qt::Horizontal)
    {
        d->viewport->update(QRect(sectionViewportPosition(index),
                                  0, sectionSize(index), d->viewport->height()));
    }
    else
    {
        d->viewport->update(QRect(0, sectionViewportPosition(index),
                                  d->viewport->width(), sectionSize(index)));
    }
}

void GlodonHeaderView::sectionsInserted(const QModelIndex &parent,
                                        int logicalFirst, int logicalLast)
{
    Q_D(GlodonHeaderView);

    if (parent != d->m_root)
    {
        return;    // we only handle changes in the top level
    }

    int nOldCount = d->sectionCount();

    d->invalidateCachedSizeHint();

    if (d->m_state == GlodonHeaderViewPrivate::ResizeSection)
    {
        d->preventCursorChangeInSetOffset = true;
    }

    // add the new sections
    int nInsertAt = logicalFirst;
    int nInsertCount = logicalLast - logicalFirst + 1;

    GlodonHeaderViewPrivate::SectionItem span(d->defaultSectionSize, d->globalResizeMode);

    d->sectionStartposRecalc = true;

    d->length += (d->defaultSectionSize + d->gridLineWidth) * nInsertCount;

    if (d->sectionItems.isEmpty() || nInsertAt >= d->sectionItems.count())
    {
        nInsertAt = d->sectionItems.count(); // append
    }

    d->sectionItems.insert(nInsertAt, nInsertCount, span);

    // update sorting column
    if (d->sortIndicatorSection >= logicalFirst)
    {
        d->sortIndicatorSection += nInsertCount;
    }

    // clear selection cache
    d->sectionSelected.clear();

    // update mapping
    if (!d->visualIndices.isEmpty() && !d->logicalIndices.isEmpty())
    {
        Q_ASSERT(d->visualIndices.count() == d->logicalIndices.count());
        int nmappingCount = d->visualIndices.count();

        for (int i = 0; i < nmappingCount; ++i)
        {
            if (d->visualIndices.at(i) >= logicalFirst)
            {
                d->visualIndices[i] += nInsertCount;
            }

            if (d->logicalIndices.at(i) >= logicalFirst)
            {
                d->logicalIndices[i] += nInsertCount;
            }
        }

        for (int j = logicalFirst; j <= logicalLast; ++j)
        {
            d->visualIndices.insert(j, j);
            d->logicalIndices.insert(j, j);
        }
    }

    // insert sections into sectionsHidden
    if (!d->sectionHidden.isEmpty())
    {
        QBitArray sectionHidden(d->sectionHidden);
        const int c_hiddenSize = sectionHidden.count();
        sectionHidden.resize(c_hiddenSize + nInsertCount);
        sectionHidden.fill(false, logicalFirst, logicalLast + 1);

        for (int j = logicalLast + 1; j < c_hiddenSize; ++j)
            //here we simply copy the old sectionHidden
        {
            sectionHidden.setBit(j, d->sectionHidden.testBit(j - nInsertCount));
        }

        d->sectionHidden = sectionHidden;
    }

    // insert sections into hiddenSectionSize
    QHash<int, int> newHiddenSectionSize; // from logical index to section size

    for (int i = 0; i < logicalFirst; ++i)
    {
        if (isSectionHidden(i))
        {
            newHiddenSectionSize[i] = d->hiddenSectionSize[i];
        }
    }

    for (int j = logicalLast + 1; j < d->sectionCount(); ++j)
    {
        if (isSectionHidden(j))
        {
            newHiddenSectionSize[j] = d->hiddenSectionSize[j - nInsertCount];
        }
    }

    d->hiddenSectionSize = newHiddenSectionSize;

    emit sectionCountChanged(nOldCount, count());

    d->viewport->update();
}

void GlodonHeaderView::sectionsAboutToBeRemoved(const QModelIndex &parent,
        int logicalFirst, int logicalLast)
{
    Q_UNUSED(parent);
    Q_UNUSED(logicalFirst);
    Q_UNUSED(logicalLast);
}

void GlodonHeaderViewPrivate::updateHiddenSections(int logicalFirst, int logicalLast)
{
    Q_Q(GlodonHeaderView);
    const int c_changeCount = logicalLast - logicalFirst + 1;

    // remove sections from hiddenSectionSize
    QHash<int, int> newHiddenSectionSize; // from logical index to section size

    for (int i = 0; i < logicalFirst; ++i)
    {
        if (q->isSectionHidden(i))
        {
            newHiddenSectionSize[i] = hiddenSectionSize[i];
        }
    }

    for (int j = logicalLast + 1; j < sectionCount(); ++j)
    {
        if (q->isSectionHidden(j))
        {
            newHiddenSectionSize[j - c_changeCount] = hiddenSectionSize[j];
        }
    }

    hiddenSectionSize = newHiddenSectionSize;

    // remove sections from sectionsHidden
    if (!sectionHidden.isEmpty())
    {
        const int c_hiddenSize = sectionHidden.size();
        const int c_newsize = qMin(sectionCount() - c_changeCount, c_hiddenSize);
        QBitArray newSectionHidden(c_newsize);
        int nk = 0;

        for (int j = 0; j < c_hiddenSize; ++j)
        {
            const int c_logical = logicalIndex(j);

            if (c_logical < logicalFirst || c_logical > logicalLast)
            {
                newSectionHidden[nk++] = sectionHidden[j];
            }
        }

        sectionHidden = newSectionHidden;
    }
}

void GlodonHeaderViewPrivate::_q_sectionsRemoved(const QModelIndex &parent, int logicalFirst, int logicalLast)
{
    Q_Q(GlodonHeaderView);

    if (parent != m_root)
    {
        return;    // we only handle changes in the top level
    }

    if (qMin(logicalFirst, logicalLast) < 0
            || qMax(logicalLast, logicalFirst) >= sectionItems.count())
    {
        return;
    }

    int noldCount = q->count();
    int nchangeCount = logicalLast - logicalFirst + 1;

    if (m_state == GlodonHeaderViewPrivate::ResizeSection)
    {
        preventCursorChangeInSetOffset = true;
    }

    updateHiddenSections(logicalFirst, logicalLast);

    if (visualIndices.isEmpty() && logicalIndices.isEmpty())
    {
        //Q_ASSERT(headerSectionCount() == sectionSpans.count());
        removeSectionsFromSectionItems(logicalFirst, logicalLast);
    }
    else
    {
        if (logicalFirst == logicalLast)   // Remove just one index.
        {
            int i = logicalFirst;
            int nVisualIndex = visualIndices.at(i);

            for (int j = 0; j < sectionItems.count(); ++j)
            {
                if (j >= logicalIndices.count())
                {
                    continue;    // the section doesn't exist
                }

                if (j > nVisualIndex)
                {
                    int nlogical = logicalIndices.at(j);
                    --(visualIndices[nlogical]);
                }

                if (logicalIndex(j) > i) // no need to move the positions before l
                {
                    --(logicalIndices[j]);
                }
            }

            logicalIndices.remove(nVisualIndex);
            visualIndices.remove(i);
            //Q_ASSERT(headerSectionCount() == sectionSpans.count());
            removeSectionsFromSectionItems(nVisualIndex, nVisualIndex);
        }
        else
        {
            sectionStartposRecalc = true; // We will need to recalc positions after removing items

            for (int u = 0; u < sectionItems.count(); ++u)  // Store spans info

            {
                sectionItems.at(u).tmpLogIdx = logicalIndices.at(u);
            }

            for (int v = sectionItems.count() - 1; v >= 0; --v)    // Remove the sections
            {

                if (logicalFirst <= sectionItems.at(v).tmpLogIdx && sectionItems.at(v).tmpLogIdx <= logicalLast)

                {
                    removeSectionsFromSectionItems(v, v);    // Invalidates the spans variable
                }

            }

            visualIndices.resize(sectionItems.count());

            logicalIndices.resize(sectionItems.count());

            int *visual_data = visualIndices.data();

            int *logical_data = logicalIndices.data();

            for (int w = 0; w < sectionItems.count(); ++w)   // Restore visual and logical indexes
            {

                int logindex = sectionItems.at(w).tmpLogIdx;

                if (logindex > logicalFirst)

                {
                    logindex -= nchangeCount;
                }

                visual_data[logindex] = w;

                logical_data[w] = logindex;

            }

        }

        // ### handle sectionSelection (sectionHidden is handled by updateHiddenSections)
    }

    // update sorting column
    if (sortIndicatorSection >= logicalFirst)
    {
        if (sortIndicatorSection <= logicalLast)
        {
            sortIndicatorSection = -1;
        }
        else
        {
            sortIndicatorSection -= nchangeCount;
        }
    }

    // if we only have the last section (the "end" position) left, the header is empty
    if (sectionItems.count() <= 0)
    {
        clear();
    }

    invalidateCachedSizeHint();

    emit q->sectionCountChanged(noldCount, q->count());

    viewport->update();
}

void GlodonHeaderViewPrivate::_q_layoutAboutToBeChanged()
{
    //if there is no row/column we can't have mapping for columns
    //because no QModelIndex in the model would be valid
    // ### this is far from being bullet-proof and we would need a real system to
    // ### map columns or rows persistently
    if ((orientation == Qt::Horizontal && m_model->rowCount(m_root) == 0)
            || m_model->columnCount(m_root) == 0)
    {
        return;
    }

    for (int i = 0; i < sectionHidden.count(); ++i)
    {
        if (sectionHidden.testBit(i)) // ### note that we are using column or row 0
        {
            persistentHiddenSections.append(orientation == Qt::Horizontal
                                            ? m_model->index(0, logicalIndex(i), m_root)
                                            : m_model->index(logicalIndex(i), 0, m_root));
        }
    }
}

void GlodonHeaderViewPrivate::_q_layoutChanged()
{
    Q_Q(GlodonHeaderView);

    viewport->update();

    if (persistentHiddenSections.isEmpty() || modelIsEmpty())
    {
        if (modelSectionCount() != sectionItems.count())
        {
            q->initializeSections();
        }

        persistentHiddenSections.clear();

        return;
    }

    QBitArray oldSectionHidden = sectionHidden;

    bool bsectionCountChanged = false;

    for (int i = 0; i < persistentHiddenSections.count(); ++i)
    {
        QModelIndex index = persistentHiddenSections.at(i);

        if (index.isValid())
        {
            const int c_logical = (orientation == Qt::Horizontal
                                   ? index.column()
                                   : index.row());
            q->setSectionHidden(c_logical, true);
            oldSectionHidden.setBit(c_logical, false);
        }
        else if (!bsectionCountChanged && (modelSectionCount() != sectionItems.count()))
        {
            bsectionCountChanged = true;
            break;
        }
    }

    persistentHiddenSections.clear();

    for (int i = 0; i < oldSectionHidden.count(); ++i)
    {
        if (oldSectionHidden.testBit(i))
        {
            q->setSectionHidden(i, false);
        }
    }

    // the number of sections changed; we need to reread the state of the model
    if (bsectionCountChanged)
    {
        q->initializeSections();
    }
}

void GlodonHeaderView::initializeSections()
{
    Q_D(GlodonHeaderView);

    const int c_oldCount = d->sectionCount();
    const int c_newCount = d->modelSectionCount();

    if (c_newCount <= 0)
    {
        d->clear();

        emit sectionCountChanged(c_oldCount, 0);
    }
    else if (c_newCount != c_oldCount)
    {
        const int c_min = qBound(0, c_oldCount, c_newCount - 1);

        initializeSections(c_min, c_newCount - 1);

        //make sure we update the hidden sections
        if (c_newCount < c_oldCount)
        {
            d->updateHiddenSections(0, c_newCount - 1);
        }
    }
}

void GlodonHeaderView::initializeSections(int start, int end)
{
    Q_D(GlodonHeaderView);

    Q_ASSERT(start >= 0);
    Q_ASSERT(end >= 0);

    d->invalidateCachedSizeHint();

    int nOldCount = d->sectionCount();
    int nNewCount = end + 1;

    if (end + 1 < d->sectionCount())
    {
        d->removeSectionsFromSectionItems(nNewCount, d->sectionCount() - 1);

        if (!d->hiddenSectionSize.isEmpty())
        {
            if (d->sectionCount() - nNewCount > d->hiddenSectionSize.count())
            {
                for (int i = end + 1; i < d->sectionCount(); ++i)
                {
                    d->hiddenSectionSize.remove(i);
                }
            }
            else
            {
                QHash<int, int>::iterator it = d->hiddenSectionSize.begin();

                while (it != d->hiddenSectionSize.end())
                {
                    if (it.key() > end)
                    {
                        it = d->hiddenSectionSize.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }
    }

    if (!d->logicalIndices.isEmpty())
    {
        if (nOldCount <= nNewCount)
        {
            d->logicalIndices.resize(nNewCount);
            d->visualIndices.resize(nNewCount);

            for (int i = nOldCount; i < nNewCount; ++i)
            {
                d->logicalIndices[i] = i;
                d->visualIndices[i] = i;
            }
        }
        else
        {
            int j = 0;

            for (int i = 0; i < nOldCount; ++i)
            {
                int v = d->logicalIndices.at(i);

                if (v < nNewCount)
                {
                    d->logicalIndices[j] = v;
                    d->visualIndices[v] = j;
                    j++;
                }
            }

            d->logicalIndices.resize(nNewCount);
            d->visualIndices.resize(nNewCount);
        }
    }

    if (!d->sectionHidden.isEmpty())
    {
        d->sectionHidden.resize(nNewCount);
    }

    if (nNewCount > nOldCount)
    {
        if (d->m_createSectionSpanSeparate)
        {
            for (int i = start; i <= end; ++i)
            {
                d->createSectionItems(i, i, d->defaultSectionSize, d->globalResizeMode);
            }
        }
        else
        {
            d->createSectionItems(start, end, (end - start + 1) * d->defaultSectionSize, d->globalResizeMode);
        }
    }

    if (d->sectionCount() != nOldCount)
    {
        emit sectionCountChanged(nOldCount,  d->sectionCount());
    }

    d->viewport->update();
}

void GlodonHeaderView::currentChanged(const QModelIndex &current, const QModelIndex &old)
{
    Q_D(GlodonHeaderView);

    if (d->orientation == Qt::Horizontal && current.column() != old.column())
    {
        if (old.isValid() && old.parent() == d->m_root)
        {
            d->viewport->update(QRect(sectionViewportPosition(old.column()), 0,
                                      sectionSize(old.column()), d->viewport->height()));
        }

        if (current.isValid() && current.parent() == d->m_root)
        {
            d->viewport->update(QRect(sectionViewportPosition(current.column()), 0,
                                      sectionSize(current.column()), d->viewport->height()));
        }
    }
    else if (d->orientation == Qt::Vertical && current.row() != old.row())
    {
        if (old.isValid() && old.parent() == d->m_root)
        {
            d->viewport->update(QRect(0, sectionViewportPosition(old.row()),
                                      d->viewport->width(), sectionSize(old.row())));
        }

        if (current.isValid() && current.parent() == d->m_root)
        {
            d->viewport->update(QRect(0, sectionViewportPosition(current.row()),
                                      d->viewport->width(), sectionSize(current.row())));
        }
    }
}

QPoint GlodonHeaderView::mousePosition()
{
    Q_D(GlodonHeaderView);
    return d->m_mousePosition;
}

void GlodonHeaderView::setMousePosition(QPoint pos)
{
    Q_D(GlodonHeaderView);
    d->m_mousePosition = pos;
}

void GlodonHeaderView::setResizeDelay(bool delay)
{
    Q_D(GlodonHeaderView);
    d->resizeDelay = delay;
}

bool GlodonHeaderView::event(QEvent *e)
{
    Q_D(GlodonHeaderView);

    switch (e->type())
    {
        case QEvent::HoverEnter:
        {
            QHoverEvent *he = static_cast<QHoverEvent *>(e);
            d->m_hover = logicalIndexAt(he->pos());

            if (d->m_hover != -1)
            {
                updateSection(d->m_hover);
            }

            break;
        }

        case QEvent::Leave:
        case QEvent::HoverLeave:
        {
            if (d->m_hover != -1)
            {
                updateSection(d->m_hover);
            }

            d->m_hover = -1;
            break;
        }

        case QEvent::HoverMove:
        {
            QHoverEvent *he = static_cast<QHoverEvent *>(e);

            int nOldHoverIndex = d->m_hover;

            d->m_hover = logicalIndexAt(he->pos());

            bool bChangedIndex = (d->m_hover != nOldHoverIndex);

            if (bChangedIndex)
            {
                if (nOldHoverIndex != -1)
                {
                    updateSection(nOldHoverIndex);
                }

                if (d->m_hover != -1)
                {
                    updateSection(d->m_hover);
                }
            }
            else
            {
                if (d->m_state == GlodonHeaderViewPrivate::MoveSection)
                {
                    setCursor(Qt::ArrowCursor);
                }
            }
        }
        default:
            break;
    }

    return GlodonAbstractItemView::event(e);
}

void GlodonHeaderView::drawImageInDesignatedLogicalIndex(QPainter *painter, QStyleOptionHeader &supopt) const
{
    Q_D(const GlodonHeaderView);
    if (d->m_logicalIndexHash.keys().contains(supopt.section))
    {
        QImage image(d->m_logicalIndexHash.value(supopt.section));
        int nTextWidth = painter->fontMetrics().width(supopt.text); //表头文本的宽度
        int nTextHeight = painter->fontMetrics().height();
        QRect rectImage((supopt.rect.left() + (supopt.rect.width() - nTextWidth) / 2) - image.width() - 3,
                        supopt.rect.top() + (supopt.rect.height() - nTextHeight) / 2, image.width(), image.height());
        painter->drawImage(rectImage, image);
    }
}

void GlodonHeaderView::paintEvent(QPaintEvent *e)
{
    Q_D(GlodonHeaderView);

    if (count() == 0)
    {
        return;
    }

    QPainter painter(d->viewport);

    const QPoint c_offset = d->m_scrollDelayOffset;
    QRect translatedEventRect = e->rect();
    translatedEventRect.translate(c_offset);

    QRect currentSectionRect;
    const int c_width = d->viewport->width();
    const int c_height = d->viewport->height();

    if (isAllSectionHidden() && d->orientation == Qt::Vertical)
    {
        currentSectionRect.setRect(0, 0, c_width, 22);
        paintEmptySection(&painter, currentSectionRect);
        return;
    }

    int nlogical;
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

    if (d->reverse())
    {
        nstart = (nstart == -1 ? count() - 1 : nstart);
        nend = (nend == -1 ? 0 : nend);
    }
    else
    {
        nstart = (nstart == -1 ? 0 : nstart);
        nend = (nend == -1 ? count() - 1 : nend);
    }

    int ntmp = nstart;
    nstart = qMin(nstart, nend);
    nend = qMax(ntmp, nend);

    d->prepareSectionSelected(); // clear and resize the bit array

    QRect fixedRect(0, 0, 0, 0);
    QRect scrollRect = d->viewport->rect();
//    if (d->fixedCount > 0)
    {
        int nfixSize = d->fixedSectionsSize();

        if (d->orientation == Qt::Vertical)
        {
            fixedRect.adjust(0, 0, d->viewport->width(), nfixSize);
            scrollRect.adjust(0, nfixSize, 0, 0);
        }
        else
        {
            fixedRect.adjust(0, 0, nfixSize, d->viewport->height());
            scrollRect.adjust(nfixSize, 0, 0, 0);
        }

        painter.setClipRect(scrollRect);
    }

    GFilterView *filter = dynamic_cast<GFilterView *>(this);
    bool bIsFilter = (filter != NULL);

    for (int i = nstart; i <= nend; ++i)
    {
        if (d->isVisualIndexHidden(i))
        {
            continue;
        }

        painter.save();
        nlogical = logicalIndex(i);

        if (d->orientation == Qt::Horizontal)
        {
            currentSectionRect.setRect(sectionViewportPosition(nlogical), 0,
                                       sectionSize(nlogical) + d->gridLineWidth, c_height);
        }
        else
        {
            currentSectionRect.setRect(0, sectionViewportPosition(nlogical),
                                       c_width, sectionSize(nlogical) + d->gridLineWidth);
        }

        currentSectionRect.translate(c_offset);

        QVariant variant = d->m_model->headerData(nlogical, d->orientation, Qt::FontRole);

        if (variant.isValid() && variant.canConvert<QFont>())
        {
            QFont sectionFont = qvariant_cast<QFont>(variant);
            painter.setFont(sectionFont);
        }

        paintSection(&painter, currentSectionRect, nlogical);
        painter.restore();
    }

    if (bIsFilter && 0 == nstart && nend > nstart && nend < d->sectionCount() - 1)
        //&& e->rect().width() > sectionSize(start) + sectionSize(end) )
    {
        paintSection(nend + 1);
    }

//    if (d->fixedCount > 0)
    {
        painter.setClipRect(fixedRect);
        int ncurrentPos = 0;

        for (int i = 0; i < d->fixedCount; i++)
        {
            if (d->isVisualIndexHidden(i))
            {
                continue;
            }

            painter.save();

            if (d->orientation == Qt::Horizontal)
            {
                currentSectionRect.setRect(ncurrentPos , 0, sectionSize(i) + d->gridLineWidth, c_height);
            }
            else
            {
                currentSectionRect.setRect(0, ncurrentPos , c_width, sectionSize(i) + d->gridLineWidth);
            }

            QVariant variant = d->m_model->headerData(i, d->orientation, Qt::FontRole);

            if (variant.isValid() && variant.canConvert<QFont>())
            {
                QFont sectionFont = qvariant_cast<QFont>(variant);
                painter.setFont(sectionFont);
            }

            paintSection(&painter, currentSectionRect, i);
            painter.restore();
            ncurrentPos += (sectionSize(i) + d->gridLineWidth);
        }
    }
}

void GlodonHeaderView::mousePressEvent(QMouseEvent *e)
{
    Q_D(GlodonHeaderView);

    d->m_isMouseDoubleClick = false;

    if (d->m_state != GlodonHeaderViewPrivate::NoState || e->button() != Qt::LeftButton)
    {
        return;
    }

    int npos = d->orientation == Qt::Horizontal ? e->x() : e->y();
    int nhandle = d->sectionHandleAt(npos);

    d->originalSize = -1; // clear the stored original size

    if (nhandle == -1)
    {
        d->pressed = logicalIndexAt(npos);

        if (d->clickableSections)
        {
            emit sectionPressed(d->pressed);
        }

        if (d->movableSections)
        {
            d->section = d->target = d->pressed;

            //现不做限制，即使是显示树形结构的列，也可以移动
            if (d->section == -1 || d->section < d->fixedCount)
            {
                return;
            }

            d->m_state = GlodonHeaderViewPrivate::MoveSection;
            d->setupSectionIndicator(d->section, npos);
        }
        else if (d->clickableSections && d->pressed != -1)
        {
            updateSection(d->pressed);
            d->m_state = GlodonHeaderViewPrivate::SelectSections;
        }
    }
    else if ((resizeMode(nhandle) == Interactive) && (npos < length()))
    {
        d->m_mousePosition = e->pos();
        //当第一次按下时，获取的是鼠标所在屏幕的位置，方便tableview中的InfoFrame的正确移动
        QPoint point = e->screenPos().toPoint();
        emit sectionResizing(point, orientation(), Press);

        d->viewport->update(d->m_mousePosition.x(), 0, 1, height());
        d->originalSize = sectionSize(nhandle);
        d->m_state = GlodonHeaderViewPrivate::ResizeSection;
        d->section = nhandle;
        d->preventCursorChangeInSetOffset = false;
    }

    d->firstPos = npos;
    d->lastPos = npos;

    d->clearCascadingSections();
}

void GlodonHeaderView::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(GlodonHeaderView);
    int npos = d->orientation == Qt::Horizontal ? e->x() : e->y();

//    if (pos < 0)
//    {
//        return;
//    }
    if (e->buttons() == Qt::NoButton)
    {
        d->m_state = GlodonHeaderViewPrivate::NoState;
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
            if (d->shouldAutoScroll(e->pos()))
            {
                d->startAutoScroll();
            }

            if (qAbs(npos - d->firstPos) >= QApplication::startDragDistance()
                    || !d->sectionIndicator->isHidden())
            {
                updateTargetSectionRecord(npos);
                d->updateSectionIndicator(d->section, npos);
            }

            return;
        }

        case GlodonHeaderViewPrivate::SelectSections:
        {
            int nlogical = logicalIndexAt(npos);

            if (nlogical == d->pressed)
            {
                return; // nothing to do
            }
            else if (d->pressed != -1)
            {
                updateSection(d->pressed);
            }

            d->pressed = nlogical;

            if (d->clickableSections && nlogical != -1)
            {
                emit sectionEntered(d->pressed);
                updateSection(d->pressed);
            }

            return;
        }

        case GlodonHeaderViewPrivate::NoState:
        {
#ifndef QT_NO_CURSOR
            int nhandle = d->sectionHandleAt(npos);
            bool bhasCursor = testAttribute(Qt::WA_SetCursor);

            if (nhandle != -1 && resizeMode(nhandle) == Interactive && npos < length())
            {
                bool bInState = getSectionHandleAt(npos) != -1;

                if (bInState)
                {
                    setCursor(d->orientation == Qt::Horizontal ? Qt::SplitHCursor : Qt::SplitVCursor);
                }
            }
            else if (bhasCursor)
            {
                unsetCursor();
            }

#endif
            return;
        }

        default:
            break;
    }
}

void GlodonHeaderView::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(GlodonHeaderView);
    int npos = d->orientation == Qt::Horizontal ? e->x() : e->y();

    switch (d->m_state)
    {
        case GlodonHeaderViewPrivate::MoveSection:
        {
            if (!d->sectionIndicator->isHidden())
            {
                // moving
                bool bcanMove = true;

                int nfrom = visualIndex(d->section);
                Q_ASSERT(nfrom != -1);
                int nto = visualIndex(d->target);
                Q_ASSERT(nto != -1);

                if (d->target < d->fixedCount)
                {
                    d->updateSectionIndicator(-1, npos);
                    break;
                }

                emit canSectionMove(d->section, d->target, bcanMove);

                if (d->m_logicalMovable)
                {
                    d->section = d->target = -1;
                    d->updateSectionIndicator(d->section, npos);
                    break;
                }

                if (bcanMove)
                {
                    moveSection(nfrom, nto);
                    d->section = d->target = -1;
                    d->updateSectionIndicator(d->section, npos);
                    break;
                }
                else
                {
                    d->sectionIndicator->hide();
                    break;
                }
            } // not moving

//        break;
        }

        case GlodonHeaderViewPrivate::SelectSections:
        {
            if (!d->clickableSections)
            {
                int nsection = logicalIndexAt(npos);
                updateSection(nsection);
            }

//        break;
            // fall through
        }

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

void GlodonHeaderView::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_D(GlodonHeaderView);

    if (!d->m_allowDoubleClicked)
    {
        return;
    }

    int npos = d->orientation == Qt::Horizontal ? e->x() : e->y();
    int nhandle = d->sectionHandleAt(npos);
    d->m_isMouseDoubleClick = true;

    if (nhandle > -1 && resizeMode(nhandle) == Interactive)
    {
        emit sectionHandleDoubleClicked(nhandle);
#ifndef QT_NO_CURSOR
        Qt::CursorShape splitCursor = (d->orientation == Qt::Horizontal)
                                      ? Qt::SplitHCursor : Qt::SplitVCursor;

        if (cursor().shape() == splitCursor)
        {
            // signal handlers may have changed the section size
            nhandle = d->sectionHandleAt(npos);

            if (!(nhandle > -1 && resizeMode(nhandle) == Interactive))
            {
                setCursor(Qt::ArrowCursor);
            }
        }

#endif
    }
    else
    {
        emit sectionDoubleClicked(logicalIndexAt(e->pos()));
    }
}

bool GlodonHeaderView::viewportEvent(QEvent *e)
{
    Q_D(GlodonHeaderView);

    switch (e->type())
    {
#ifndef QT_NO_TOOLTIP

        case QEvent::ToolTip:
        {
            QHelpEvent *he = static_cast<QHelpEvent *>(e);
            int nlogical = logicalIndexAt(he->pos());

            if (nlogical != -1)
            {
                QVariant variant = d->m_model->headerData(nlogical, d->orientation, Qt::ToolTipRole);

                if (variant.isValid())
                {
                    QToolTip::showText(he->globalPos(), variant.toString(), this);
                    return true;
                }
            }

            break;
        }

#endif
#ifndef QT_NO_WHATSTHIS

        case QEvent::QueryWhatsThis:
        {
            QHelpEvent *he = static_cast<QHelpEvent *>(e);
            int nlogical = logicalIndexAt(he->pos());

            if (nlogical != -1
                    && d->m_model->headerData(nlogical, d->orientation, Qt::WhatsThisRole).isValid())
            {
                return true;
            }

            break;
        }

        case QEvent::WhatsThis:
        {
            QHelpEvent *he = static_cast<QHelpEvent *>(e);
            int nlogical = logicalIndexAt(he->pos());

            if (nlogical != -1)
            {
                QVariant whatsthis = d->m_model->headerData(nlogical, d->orientation,
                                     Qt::WhatsThisRole);

                if (whatsthis.isValid())
                {
                    QWhatsThis::showText(he->globalPos(), whatsthis.toString(), this);
                    return true;
                }
            }

            break;
        }

#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_STATUSTIP

        case QEvent::StatusTip:
        {
            QHelpEvent *he = static_cast<QHelpEvent *>(e);
            int nlogical = logicalIndexAt(he->pos());

            if (nlogical != -1)
            {
                QString statustip = d->m_model->headerData(nlogical, d->orientation,
                                    Qt::StatusTipRole).toString();

                if (!statustip.isEmpty())
                {
                    setStatusTip(statustip);
                }
            }

            return true;
        }

#endif // QT_NO_STATUSTIP

        case QEvent::Hide:
        {
            d->invalidateCachedSizeHint();
            emit geometriesChanged();

            break;
        }

        case QEvent::Show:
        case QEvent::FontChange:
        case QEvent::StyleChange:
            d->invalidateCachedSizeHint();
            emit geometriesChanged();
            break;

        case QEvent::ContextMenu:
        {
            d->m_state = GlodonHeaderViewPrivate::NoState;
            d->pressed = d->section = d->target = -1;
            d->updateSectionIndicator(d->section, -1);
            break;
        }

        case QEvent::Wheel:
        {
            QAbstractScrollArea *asa = dynamic_cast<QAbstractScrollArea *>(parentWidget());

            if (asa)
            {
                return QApplication::sendEvent(asa->viewport(), e);
            }

            break;
        }

        default:
            break;
    }

    return GlodonAbstractItemView::viewportEvent(e);
}

void GlodonHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    Q_D(const GlodonHeaderView);

    if (!rect.isValid())
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
        if (logicalIndex == d->m_hover)
        {
            state |= QStyle::State_MouseOver;
        }

        if (logicalIndex == d->pressed)
        {
            state |= QStyle::State_Sunken;
        }
        else if (d->highlightSelected)
        {
            if (d->isSectionSelected(logicalIndex))
            {
                state |= QStyle::State_Sunken;
            }
            else if (d->sectionIntersectsSelection(logicalIndex))
            {
                if (d->m_boldWithSelected)
                {
                    state |= QStyle::State_On;
                }

                if (!(state & QStyle::State_Sunken))
                {
                    bDrawHighLight = true;
                }
            }
        }
    }

    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex)
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

    // setup the style options structure
    QVariant textAlignment = d->m_model->headerData(logicalIndex, d->orientation,
                             Qt::TextAlignmentRole);
    opt.rect = rect;
    opt.section = logicalIndex;
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
    QVariant iconAlignment = d->m_model->headerData(logicalIndex, d->orientation,
                             Qt::DecorationPropertyRole);

    if (iconAlignment.isValid())
    {
        opt.iconAlignment = (Qt::Alignment)iconAlignment.toInt();
    }

    if (d->autoSectionData)
    {
        opt.text = QString::number(visualIndex(logicalIndex) + 1);
    }
    else
    {
        opt.text = d->m_model->headerData(logicalIndex, d->orientation,
                                          Qt::DisplayRole).toString();
    }

    if (d->m_textElideMode != Qt::ElideNone)
    {
        opt.text = opt.fontMetrics.elidedText(opt.text, d->m_textElideMode , rect.width() - 4);
    }

    QVariant variant = d->m_model->headerData(logicalIndex, d->orientation,
                       Qt::DecorationRole);
    opt.icon = qvariant_cast<QIcon>(variant);

    if (opt.icon.isNull())
    {
        opt.icon = qvariant_cast<QPixmap>(variant);
    }

    QVariant foregroundBrush = d->m_model->headerData(logicalIndex, d->orientation,
                               Qt::ForegroundRole);

    if (foregroundBrush.canConvert<QBrush>())
    {
        opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));
    }

    QPointF oldBO = painter->brushOrigin();
    QVariant backgroundBrush = d->m_model->headerData(logicalIndex, d->orientation,
                               Qt::BackgroundRole);

    if (backgroundBrush.canConvert<QBrush>())
    {
        opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
        opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
        painter->setBrushOrigin(opt.rect.topLeft());
    }

    // the section position
    int nVisual = visualIndex(logicalIndex);
    Q_ASSERT(nVisual != -1);

    bool first = d->isFirstVisibleSection(nVisual);
    bool last = d->isLastVisibleSection(nVisual);

    if (first && last)
    {
        opt.position = QStyleOptionHeader::OnlyOneSection;
    }
    else if (first)
    {
        opt.position = QStyleOptionHeader::Beginning;
    }
    else if (last)
    {
        opt.position = QStyleOptionHeader::End;
    }
    else
    {
        opt.position = QStyleOptionHeader::Middle;
    }

    opt.orientation = d->orientation;
    // the selected position
    bool bPreviousSelected = d->isSectionSelected(this->logicalIndex(nVisual - 1));
    bool bNextSelected =  d->isSectionSelected(this->logicalIndex(nVisual + 1));

    if (bPreviousSelected && bNextSelected)
    {
        opt.selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;
    }
    else if (bPreviousSelected)
    {
        opt.selectedPosition = QStyleOptionHeader::PreviousIsSelected;
    }
    else if (bNextSelected)
    {
        opt.selectedPosition = QStyleOptionHeader::NextIsSelected;
    }
    else
    {
        opt.selectedPosition = QStyleOptionHeader::NotAdjacent;
    }

    if (bDrawHighLight)
    {
        drawCellBorder(painter, opt);

        if (d->fixedCellEvent)
        {
            paintHighLightSection(painter, opt, rect);
        }
        else
        {
            paintSectionOnQt(painter, opt, this, true);
        }
    }
    else
    {
        // drawHighlight为false时，只将表头字体加粗
        if (d->sectionIntersectsSelection(logicalIndex))
        {
            QFont fnt = painter->font();

            if (d->m_boldWithSelected)
            {
                opt.state |= QStyle::State_On;
                fnt.setBold(true);
            }

            painter->setFont(fnt);
        }

        paintSectionOnQt(painter, opt, this, false);

        if (d->fixedCellEvent)
        {
            fillFixedCell(painter, opt, rect, logicalIndex);
        }

        drawCellBorder(painter, opt);
    }

    painter->setBrushOrigin(oldBO);

    if (Qt::Horizontal == d->orientation)
    {
        drawImageInDesignatedLogicalIndex(painter, opt);
    }
}

void GlodonHeaderView::paintSection(int beginToHideIndex) const
{
    Q_UNUSED(beginToHideIndex);
}

QSize GlodonHeaderView::sectionSizeFromContents(int logicalIndex) const
{
    Q_D(const GlodonHeaderView);
    Q_ASSERT(logicalIndex >= 0);

    ensurePolished();

    // use SizeHintRole
    QVariant variant = d->m_model->headerData(logicalIndex, d->orientation, Qt::SizeHintRole);

    if (variant.isValid())
    {
        if (!d->autoSectionData)
        {
            return qvariant_cast<QSize>(variant);
        }
    }

    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.section = logicalIndex;
    QVariant var = d->m_model->headerData(logicalIndex, d->orientation,
                                          Qt::FontRole);
    QFont fnt;

    if (var.isValid() && var.canConvert<QFont>())
    {
        fnt = qvariant_cast<QFont>(var);
    }
    else
    {
        fnt = font();
    }

    fnt.setBold(true);
    opt.fontMetrics = QFontMetrics(fnt);

    if (d->autoSectionData)
    {
        opt.text = QString::number(logicalIndex + 1);
    }
    else
    {
        opt.text = d->m_model->headerData(logicalIndex, d->orientation,
                                          Qt::DisplayRole).toString();
    }

    variant = d->m_model->headerData(logicalIndex, d->orientation, Qt::DecorationRole);
    opt.icon = qvariant_cast<QIcon>(variant);

    if (opt.icon.isNull())
    {
        opt.icon = qvariant_cast<QPixmap>(variant);
    }

    QSize size = style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), this);

    if (isSortIndicatorShown())
    {
        int nMargin = style()->pixelMetric(QStyle::PM_HeaderMargin, &opt, this);

        if (d->orientation == Qt::Horizontal)
        {
            size.rwidth() += size.height() + nMargin;
        }
        else
        {
            size.rheight() += size.width() + nMargin;
        }
    }

    return size;
}

void GlodonHeaderView::paintEmptySection(QPainter *painter, const QRect &rect)
{
    Q_D(const GlodonHeaderView);

    if (!rect.isValid())
    {
        return;
    }

    QStyleOptionHeader opt;
    initStyleOption(&opt);
    QStyle::State state = QStyle::State_None;

    if (isEnabled())
    {
        state |= QStyle::State_Enabled;
    }

    if (window()->isActiveWindow())
    {
        state |= QStyle::State_Active;
    }

    opt.rect = rect;
    opt.section = 0;
    opt.state |= state;

    QPointF oldBO = painter->brushOrigin();

    opt.position = QStyleOptionHeader::OnlyOneSection;
    opt.orientation = d->orientation;

    opt.selectedPosition = QStyleOptionHeader::NotAdjacent;

    style()->drawControl(QStyle::CE_Header, &opt, painter, this);

    painter->setBrushOrigin(oldBO);
}

void GlodonHeaderView::setSortArrowOnTextTop(bool bIsSortArrowOnTextTop)
{
    m_isSortArrowOnTextTop = bIsSortArrowOnTextTop;
}

void GlodonHeaderView::setLogicalIndexImage(QMultiHash<int, QString> logicalIndexHash, int spanTop)
{
    Q_D(GlodonHeaderView);
    d->m_logicalIndexHash = logicalIndexHash;
    d->m_spanTop = spanTop;
}

void GlodonHeaderView::batchResizeSection(const QMap<int, int> &logicalIndexSizes)
{
    Q_D(GlodonHeaderView);
    d->doBatchResizeSection(logicalIndexSizes, false);
}

void GlodonHeaderView::paintSectionSortArrow(QPainter *painter, QStyleOptionHeader &opt, const QWidget *widget) const
{
    Q_D(const GlodonHeaderView);
    QStyleOptionHeader supopt = opt;

    if (supopt.sortIndicator != QStyleOptionHeader::None && Qt::Horizontal == d->orientation)
    {
        supopt.rect = style()->subElementRect(QStyle::SE_HeaderArrow, &opt, widget);
        if (!m_isSortArrowOnTextTop)
        {
            int nTextWidth = painter->fontMetrics().width(supopt.text); //表头文本的宽度

            //如果当前列宽大于文本宽度加上两倍的c_nSpacingTextToArrow再加上箭头的宽度再加上2px，则执行排序小三角的绘制。
            //最后加的2px，1px是文本本身有1px偏移，1px是右边线的宽度
            const int nOffect = 2;
            const int nTextMargins = 2 * c_nSpacingTextToArrow + c_sortArrow.width() + nOffect;

            if (opt.rect.width() > nTextWidth + nTextMargins)
            {
                //箭头水平偏移为文本宽度的一半加上c_nSpacingTextToArrow
                int nOffsetX = nTextWidth / 2 + c_nSpacingTextToArrow;
                //箭头垂直偏移为表头高度的一半减去箭头高度的一半
                int nOffsetY = (opt.rect.height() - c_sortArrow.height()) / 2;

                supopt.rect.adjust(nOffsetX, nOffsetY, nOffsetX, nOffsetY);
                style()->proxy()->drawPrimitive(QStyle::PE_IndicatorHeaderArrow, &supopt, painter, widget);
            }
        }
        else
        {
            style()->proxy()->drawPrimitive(QStyle::PE_IndicatorHeaderArrow, &supopt, painter, widget);
        }
    }
}

void GlodonHeaderView::paintSectionOnQt(QPainter *painter, QStyleOptionHeader &opt, const QWidget *widget, bool bHighLight) const
{
    Q_D(const GlodonHeaderView);

    style()->proxy()->drawControl(QStyle::CE_HeaderSection, &opt, painter, this);

    QStyleOptionHeader supopt = opt;

    //获取文本内容有效绘制区(通过Qss等 已经计算完 margin, padding的rect)
    //  supopt.rect = style()->subElementRect(QStyle::SE_HeaderLabel, &opt, widget);

    if (supopt.rect.isValid())
    {
        if (d->m_bFontShadow)
        {
            painter->save();
            painter->setPen(d->m_clrFontShadow);
            QFont oldFont = painter->font();
            QFont newFont;

            if (bHighLight)
            {
                newFont = widget->font();
                newFont.setBold(bHighLight);
            }
            else
            {
                newFont = painter->font(); //注意 获取widget的字体 可能不会使用到qss中设置字体(包括字体大小) 暂时没有好方法从QStyle中获取应该使用的字体
            }

            painter->setFont(newFont);
            QRect rect = supopt.rect;
            painter->drawText(rect.adjusted(1, 1, 1, 1), supopt.textAlignment, supopt.text);
            painter->setFont(oldFont);
            painter->restore();
        }

        style()->proxy()->drawControl(QStyle::CE_HeaderLabel, &supopt, painter, widget);
//        painter->drawText(supopt.rect.adjusted(1, 1, 1, 1), supopt.textAlignment | Qt::TextWordWrap | Qt::TextWrapAnywhere, supopt.text);
    }

    paintSectionSortArrow(painter, supopt, widget);   //绘制排序列的表头小三角
}

void GlodonHeaderView::paintHighLightSection(QPainter *painter, QStyleOptionHeader &opt, const QRect &rect) const
{
    Q_D(const GlodonHeaderView);
    painter->save();

    int nGridLineWidth = 0;
    QStyleOptionHeader supopt = opt;

    if (d->m_drawBorder)
    {
        nGridLineWidth = 1;
    }

    int nWw = rect.width() - 1;
    int nHh = rect.height() - 1 - nGridLineWidth;
    int nlineWidth = 1;
    int nXx = rect.x();
    int nYy = rect.y() + nGridLineWidth;

    if (d->m_drawBorder && (rect.x() == 0))
    {
        nXx = nXx + nGridLineWidth;
        nWw = nWw - nGridLineWidth;
    }

    QBrush *fill = const_cast<QBrush *>(&opt.palette.brush(QPalette::Button));

    if (nWw == 0 || nHh == 0)
    {
        return;
    }

    QPen oldPen = painter->pen();

    if (fill && d->m_bLinearGradient)
    {
        QLinearGradient linearGradient(nXx + nWw / 2 + nlineWidth, nYy, nXx + nWw / 2 + nlineWidth,
                                       nYy + nHh - nlineWidth * 2 - 1);

        linearGradient.setColorAt(0.3, QColor(248, 215, 153));
        linearGradient.setColorAt(0.6, QColor(245, 203, 123));
        linearGradient.setColorAt(1, QColor(242, 196, 103));

        painter->setBrush(linearGradient);
        painter->fillRect(nXx, nYy, nWw, nHh, QBrush(linearGradient));
    }
    else
    {
        painter->fillRect(nXx, nYy, nWw, nHh, QBrush(QColor("#ffefb5")));
    }

    painter->setPen(oldPen);

    QRect headerRect = opt.rect;
    int npixw = 0;

    if (!opt.icon.isNull())
    {
        QPixmap pixmap = opt.icon.pixmap(style()->proxy()->pixelMetric(QStyle::PM_SmallIconSize),
                                         (QStyle::State_Enabled == (opt.state & QStyle::State_Enabled))
                                         ? QIcon::Normal : QIcon::Disabled);
        npixw = pixmap.width();

        QRect aligned = style()->alignedRect(opt.direction, QFlag(opt.iconAlignment), pixmap.size(), headerRect);
        QRect inter = aligned.intersected(headerRect);

        if (d->orientation == Qt::Vertical)
        {
            painter->drawPixmap(inter.x() + 4, inter.y(), pixmap, inter.x() - aligned.x(), inter.y() - aligned.y(),
                                inter.width(), inter.height());
        }
        else
        {
            painter->drawPixmap(inter.x(), inter.y(), pixmap, inter.x() - aligned.x(), inter.y() - aligned.y(),
                                inter.width(), inter.height());
        }
    }

    if (QStyle::State_On == (opt.state & QStyle::State_On))
    {
        QFont fnt = painter->font();

        if (d->m_boldWithSelected)
        {
            fnt.setBold(true);
        }
        else
        {
            opt.state &= ~QStyle::State_On;
        }

        painter->setFont(fnt);
    }

    if (npixw != 0 && d->orientation == Qt::Vertical)//如果是垂直表头，需要加上偏移，不然选中后的问题绘制不正确
    {
        headerRect.setLeft(headerRect.left() + npixw + 2);
    }

    opt.rect = textRect(rect);
    style()->proxy()->drawItemText(painter, headerRect, opt.textAlignment, opt.palette,
                                   QStyle::State_Enabled == (opt.state & QStyle::State_Enabled),
                                   opt.text, QPalette::ButtonText);
    opt.rect = rect;

    //如果当前列为排序指示列，则画上小三角
    if (isSortIndicatorShown() && sortIndicatorSection() == opt.section)
    {
        QRegion clipRegion = painter->clipRegion();

        paintSectionSortArrow(painter, supopt, this);

        painter->setClipRegion(clipRegion);
    }

    painter->restore();
}

//自定义固定格背景色填充
void GlodonHeaderView::fillFixedCell(QPainter *painter, QStyleOptionHeader &opt, QRect rect, int logicalIndex) const
{
    Q_D(const GlodonHeaderView);

    if (d->fixedCellsColor.contains(logicalIndex)
            && d->fixedCellsColor.value(logicalIndex).orientation == d->orientation)
    {
        rect.adjust(0, 0, -1, -1);
        QColor bkGround = d->fixedCellsColor.value(logicalIndex).background;

        painter->fillRect(rect, bkGround);

        style()->proxy()->drawItemText(painter, opt.rect, opt.textAlignment, opt.palette,
                                       (QStyle::State_Enabled == (opt.state & QStyle::State_Enabled)),
                                       opt.text, QPalette::ButtonText);
    }
}

int GlodonHeaderView::horizontalOffset() const
{
    Q_D(const GlodonHeaderView);

    if (d->orientation == Qt::Horizontal)
    {
        return d->offset;
    }

    return 0;
}

int GlodonHeaderView::verticalOffset() const
{
    Q_D(const GlodonHeaderView);

    if (d->orientation == Qt::Vertical)
    {
        return d->offset;
    }

    return 0;
}

void GlodonHeaderView::updateGeometries()
{
    Q_D(GlodonHeaderView);
    d->layoutChildren();
}

void GlodonHeaderView::scrollContentsBy(int dx, int dy)
{
    Q_D(GlodonHeaderView);
    d->scrollDirtyRegion(dx, dy);
}

void GlodonHeaderView::rowsInserted(const QModelIndex &, int, int)
{
    // do nothing
}

QRect GlodonHeaderView::visualRect(const QModelIndex &) const
{
    return QRect();
}

void GlodonHeaderView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_UNUSED(index);
    Q_UNUSED(hint);
}

QModelIndex GlodonHeaderView::indexAt(const QPoint &) const
{
    return QModelIndex();
}

bool GlodonHeaderView::isIndexHidden(const QModelIndex &) const
{
    return true; // the header view has no items, just sections
}

bool GlodonHeaderView::isAllSectionHidden() const
{
    for (int i = 0; i < count(); i++)
    {
        if (!isSectionHidden(i))
        {
            return false;
        }
    }

    return true;
}

QModelIndex GlodonHeaderView::moveCursor(CursorAction, Qt::KeyboardModifiers)
{
    return QModelIndex();
}

void GlodonHeaderView::setSelection(const QRect &, QItemSelectionModel::SelectionFlags)
{
    // do nothing
}

QRegion GlodonHeaderView::visualRegionForSelection(const QItemSelection &selection) const
{
    Q_D(const GlodonHeaderView);
    const int c_max = d->modelSectionCount();
    const int c_selectionCount = selection.count();

    if (d->orientation == Qt::Horizontal)
    {
        int nLeft = c_max;
        int nRight = 0;
        int nrangeLeft(0);
        int nrangeRight(0);

        for (int i = 0; i < c_selectionCount; ++i)
        {
            QItemSelectionRange tmpRange = selection.at(i);

            if (tmpRange.parent().isValid() || !tmpRange.isValid())
            {
                continue;    // we only know about toplevel items and we don't want invalid ranges
            }

            // FIXME an item inside the range may be the leftmost or rightmost
            nrangeLeft = visualIndex(tmpRange.left());

            if (nrangeLeft == -1) // in some cases users may change the selections
            {
                continue;    // before we have a chance to do the layout
            }

            nrangeRight = visualIndex(tmpRange.right());

            if (nrangeRight == -1) // in some cases users may change the selections
            {
                continue;    // before we have a chance to do the layout
            }

            if (nrangeLeft < nLeft)
            {
                nLeft = nrangeLeft;
            }

            if (nrangeRight > nRight)
            {
                nRight = nrangeRight;
            }
        }

        int nLogicalLeft = logicalIndex(nLeft);
        int nLogicalRight = logicalIndex(nRight);

        if (nLogicalLeft < 0 || nLogicalLeft >= count()
                || nLogicalRight < 0 || nLogicalRight >= count())
        {
            return QRegion();
        }

        int nLeftPos = sectionViewportPosition(nLogicalLeft);
        int nRightPos = sectionViewportPosition(nLogicalRight);
        nRightPos += sectionSize(nLogicalRight);
        return QRect(nLeftPos, 0, nRightPos - nLeftPos, height());
    }

    // orientation() == Qt::Vertical
    int nTop = c_max;
    int nBottom = 0;
    int nRangeTop(0);
    int nRangeBottom(0);

    for (int i = 0; i < c_selectionCount; ++i)
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

    return QRect(0, ntopPos, width(), nbottomPos - ntopPos);
}

// private implementation

int GlodonHeaderViewPrivate::sectionHandleAt(int position)
{
    Q_Q(GlodonHeaderView);
    int nVisualIndex = q->visualIndexAt(position);

    if (nVisualIndex == -1)
    {
        return -1;
    }

    int nlog = logicalIndex(nVisualIndex);
    int npos = q->sectionViewportPosition(nlog);
    int ngrip = q->style()->pixelMetric(QStyle::PM_HeaderGripMargin, 0, q);

    bool batLeft = position < npos + ngrip;
    bool batRight = (position > npos + q->sectionSize(nlog) - ngrip);

    if (reverse())
    {
        qSwap(batLeft, batRight);
    }

    if (batLeft)
    {
        //grip at the beginning of the section
        while (nVisualIndex > -1)
        {
            int nlogical = q->logicalIndex(--nVisualIndex);

            if (!q->isSectionHidden(nlogical))
            {
                return nlogical;
            }
        }
    }
    else if (batRight)
    {
        //grip at the end of the section
        return nlog;
    }

    return -1;
}

void GlodonHeaderViewPrivate::setupSectionIndicator(int section, int position)
{
    Q_Q(GlodonHeaderView);

    if (!sectionIndicator)
    {
        sectionIndicator = new QLabel(viewport);
    }

    int nWidth(0);
    int nHeight(0);
    int np = q->sectionViewportPosition(section);

    if (orientation == Qt::Horizontal)
    {
        nWidth = q->sectionSize(section);
        nHeight = viewport->height();
    }
    else
    {
        nWidth = viewport->width();
        nHeight = q->sectionSize(section);
    }

    sectionIndicator->resize(nWidth, nHeight);

    QPixmap pm(nWidth, nHeight);
    pm.fill(QColor(0, 0, 0, 45));
    QRect rect(0, 0, nWidth, nHeight);

    QPainter painter(&pm);
    painter.setOpacity(0.75);
    q->paintSection(&painter, rect, section);
    painter.end();

    sectionIndicator->setPixmap(pm);
    sectionIndicatorOffset = position - qMax(np, 0);
}

void GlodonHeaderViewPrivate::updateSectionIndicator(int section, int position)
{
    if (!sectionIndicator)
    {
        return;
    }

    if (section == -1 || target == -1)
    {
        sectionIndicator->hide();
        return;
    }

    if (orientation == Qt::Horizontal)
    {
        sectionIndicator->move(position - sectionIndicatorOffset, 0);
    }
    else
    {
        sectionIndicator->move(0, position - sectionIndicatorOffset);
    }

    sectionIndicator->show();
}

void GlodonHeaderView::initStyleOption(QStyleOptionHeader *option) const
{
    Q_D(const GlodonHeaderView);

    option->initFrom(this);
    option->state = QStyle::State_None | QStyle::State_Raised;
    option->orientation = d->orientation;

    if (d->orientation == Qt::Horizontal)
    {
        option->state |= QStyle::State_Horizontal;
    }

    if (isEnabled())
    {
        option->state |= QStyle::State_Enabled;
    }

    option->section = 0;
}

void GlodonHeaderView::_q_sectionsRemoved(const QModelIndex &parent, int logicalFirst, int logicalLast)
{
    d_func()->_q_sectionsRemoved(parent, logicalFirst, logicalLast);
}

void GlodonHeaderView::_q_layoutAboutToBeChanged()
{
    d_func()->_q_layoutAboutToBeChanged();
}

void GlodonHeaderView::updateTargetSectionRecord(int pos)
{
    Q_D(GlodonHeaderView);
    int nVisualIndex = visualIndexAt(pos);

    if (nVisualIndex == -1)
    {
        return;
    }

    int nposThreshold = d->headerSectionPosition(nVisualIndex) + d->headerSectionSize(nVisualIndex) / 2;
    int nmoving = visualIndex(d->section);

    if (nVisualIndex < nmoving)
    {
        if (pos < nposThreshold)
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
        if (pos > nposThreshold)
        {
            d->target = d->logicalIndex(nVisualIndex - 1);
        }
        else
        {
            d->target = d->logicalIndex(nVisualIndex);
        }
    }
    else
    {
        d->target = d->section;
    }
}

void GlodonHeaderView::drawCellBorder(QPainter *painter, QStyleOptionHeader &opt) const
{
    Q_D(const GlodonHeaderView);
    // m_drawBorder为true, 手动画边线, 否则调用Qt中画HeaderSection的方法画边线
    if (d->m_drawBorder)
    {
        QRect topBorderRect(QRect(QPoint(opt.rect.left(), opt.rect.top()), QSize(opt.rect.width(), 1)));
        QRect leftBorderRect(QRect(QPoint(opt.rect.left(), opt.rect.top()), QSize(1, opt.rect.height())));
        QRect bottemBorderRect(QRect(QPoint(opt.rect.left(), opt.rect.bottom()), QSize(opt.rect.width(), 1)));
        QRect rightBorderRect(QRect(QPoint(opt.rect.right(), opt.rect.top()), QSize(1, opt.rect.height())));
        // 画上边线和左边线, 水平表头只有第一个section画左边线, 垂直表头只有第一个section画上边线
        if (Qt::Horizontal == d->orientation)
        {
            painter->fillRect(topBorderRect, QColor(0, 0, 0));

            if (opt.rect.left() < 2)
            {
                painter->fillRect(leftBorderRect, QColor(0, 0, 0));
            }
        }
        else
        {
            if (opt.rect.top() < 2)
            {
                painter->fillRect(topBorderRect, QColor(0, 0, 0));
            }

            painter->fillRect(leftBorderRect, QColor(0, 0, 0));
        }

        painter->fillRect(bottemBorderRect, QColor(0, 0, 0));

        painter->fillRect(rightBorderRect, QColor(0, 0, 0));
    }
}

QRect GlodonHeaderView::textRect(const QRect &rect) const
{
    Q_D(const GlodonHeaderView);
    QVariant margin = d->m_model->headerData(0, d->orientation,
                      gidrCellMargin);
    QRect newRect = rect;

    if (margin.isValid())
    {
        int nMargin = margin.toInt();
//        newRect.adjust((nMargin & 0x00FF0000) >> 16, (nMargin & 0x000000FF) / 2,
//                        -(nMargin & 0x00FF0000) >> 16, -(nMargin & 0x000000FF) / 2);
        newRect.adjust((nMargin & 0x00FF0000) >> 16, 0,
                       -(nMargin & 0x00FF0000) >> 16, 0);

    }

    return newRect;
}

bool GlodonHeaderViewPrivate::isSectionSelected(int section) const
{
    int nIndex = section * 2;

    if (nIndex < 0 || nIndex >= sectionSelected.count())
    {
        return false;
    }

    if (sectionSelected.testBit(nIndex)) // if the value was cached
    {
        return sectionSelected.testBit(nIndex + 1);
    }

    bool bStat = false;

    if (orientation == Qt::Horizontal)
    {
        bStat = isColumnSelected(section);
    }
    else
    {
        bStat = isRowSelected(section);
    }

    sectionSelected.setBit(nIndex + 1, bStat); // selection state
    sectionSelected.setBit(nIndex, true); // cache state
    return bStat;
}

bool GlodonHeaderViewPrivate::isFirstVisibleSection(int section) const
{
    if (sectionStartposRecalc)
    {
        recalcSectionStartPos();
    }

    const SectionItem &item = sectionItems.at(section);

    return item.size > 0 && item.calculated_startpos == 0;
}

bool GlodonHeaderViewPrivate::isLastVisibleSection(int section) const
{
    if (sectionStartposRecalc)
    {
        recalcSectionStartPos();
    }

    const SectionItem &item = sectionItems.at(section);

    return item.size > 0 && item.calculatedEndPos() == length;
}

int GlodonHeaderViewPrivate::modelSectionCount() const
{
    if (orientation == Qt::Horizontal)
    {
        return m_model->columnCount(m_root);
    }
    else
    {
        return m_model->rowCount(m_root);
    }
}

int GlodonHeaderViewPrivate::lastVisibleVisualIndex() const
{
    Q_Q(const GlodonHeaderView);

    for (int i = q->count() - 1; i >= 0; --i)
    {
        if (!q->isSectionHidden(q->logicalIndex(i)))
        {
            return i;
        }
    }

    //default value if no section is actually visible
    return -1;
}

void GlodonHeaderViewPrivate::resizeSectionItem(int newSize, SectionItem &oCurrentSectionItem)
{
    if (newSize != 0 && oCurrentSectionItem.size == 0)
    {
        length += gridLineWidth;
    }
    else if (newSize == 0 && oCurrentSectionItem.size != 0)
    {
        length -= gridLineWidth;
    }

    length += (newSize - oCurrentSectionItem.size);
    oCurrentSectionItem.size = newSize;
}

void GlodonHeaderViewPrivate::excludeNotNeedHideSections(
        const QSet<int> &logicalIndexs, bool hide, QSet<QPair<int, int>> &oNeedHideLogicalAndVisualIndexs)
{
    Q_Q(GlodonHeaderView);

    foreach (int nCurLogicalIndex, logicalIndexs)
    {
        if (q->isInValidLogicalIndex(nCurLogicalIndex))
            continue;

        int nVisualIndex = visualIndex(nCurLogicalIndex);
        Q_ASSERT(nVisualIndex != -1);
        if (hide == isVisualIndexHidden(nVisualIndex))
            continue;

        oNeedHideLogicalAndVisualIndexs.insert(QPair<int, int>(nCurLogicalIndex, nVisualIndex));
    }
}

void GlodonHeaderViewPrivate::doBatchResizeSection(const QMap<int, int> &logicalIndexSizes, bool isInHiddenSection)
{
    Q_Q(GlodonHeaderView);

    QMap<int, int>::const_iterator oSizeIte = logicalIndexSizes.begin();

    GlodonHeaderViewPrivate::SectionItem *sectiondata = sectionItems.data();

    while (oSizeIte != logicalIndexSizes.end())
    {
        if (!isInHiddenSection && q->isSectionHidden(oSizeIte.key()))
        {
            hiddenSectionSize.insert(oSizeIte.key(), oSizeIte.value());
        }
        else
        {
            resizeSectionItem(oSizeIte.value(), sectiondata[oSizeIte.key()]);
        }
        oSizeIte++;
    }

    invalidateCachedSizeHint();
    recalcSectionStartPos();
    viewport->update();
}

void GlodonHeaderViewPrivate::createSectionItems(int start, int end, int size, GlodonHeaderView::ResizeMode mode)
{
    int sizePerSection = size / (end - start + 1);

    if (end >= sectionItems.count())
    {
        sectionItems.resize(end + 1);
    }

    SectionItem *sectiondata = sectionItems.data();

    for (int i = start; i <= end; ++i)
    {
        sectionStartposRecalc |= (sectiondata[i].size != sizePerSection);
        resizeSectionItem(sizePerSection, sectiondata[i]);
        sectiondata[i].resizeMode = mode;
    }
}

void GlodonHeaderViewPrivate::removeSectionsFromSectionItems(int start, int end)
{
    // remove sections
    sectionStartposRecalc |= (end != sectionItems.count() - 1);

    int removedlength = 0;

    for (int u = start; u <= end; ++u)
    {
        removedlength += sectionItems.at(u).size;

        if (sectionItems.at(u).size != 0)
        {
            removedlength += gridLineWidth;
        }
    }

    length -= removedlength;

    sectionItems.remove(start, end - start + 1);
}

void GlodonHeaderViewPrivate::clear()
{
    if (m_state != NoClear)
    {
        length = 0;
        visualIndices.clear();
        logicalIndices.clear();
        sectionSelected.clear();
        sectionHidden.clear();
        hiddenSectionSize.clear();
        sectionItems.clear();
    }
}

void GlodonHeaderViewPrivate::flipSortIndicator(int section)
{
    Q_Q(GlodonHeaderView);
    Qt::SortOrder sortOrder;

    if (sortIndicatorSection == section)
    {
        sortOrder = (sortIndicatorOrder == Qt::DescendingOrder) ? Qt::AscendingOrder : Qt::DescendingOrder;
    }
    else
    {
        //         const QVariant value = model->headerData(section, orientation, Qt::InitialSortOrderRole);
        //         if (value.canConvert(QVariant::Int))
        //         {
        //             sortOrder = static_cast<Qt::SortOrder>(value.toInt());
        //         }
        //         else
        //        {
        sortOrder = Qt::AscendingOrder;
        //        }
    }

    q->setSortIndicator(section, sortOrder);
}

void GlodonHeaderViewPrivate::cascadingResize(int visual, int newSize, bool isManual)
{
    Q_Q(GlodonHeaderView);
    const int c_minimumSize = q->minimumSectionSize();
    const int c_oldSize = headerSectionSize(visual);
    int ndelta = newSize - c_oldSize;

    if (ndelta > 0)  // larger
    {
        bool bsectionResized = false;

        // restore old section sizes
        for (int i = firstCascadingSection; i < visual; ++i)
        {
            if (cascadingSectionSize.contains(i))
            {
                int ncurrentSectionSize = headerSectionSize(i);
                int noriginalSectionSize = cascadingSectionSize.value(i);

                if (ncurrentSectionSize < noriginalSectionSize)
                {
                    int newSectionSize = ncurrentSectionSize + ndelta;
                    resizeSectionItem(i, ncurrentSectionSize, newSectionSize, isManual);

                    if (newSectionSize >= noriginalSectionSize && false)
                    {
                        cascadingSectionSize.remove(i); // the section is now restored
                    }

                    bsectionResized = true;
                    break;
                }
            }

        }

        // resize the section
        if (!bsectionResized)
        {
            newSize = qMax(newSize, c_minimumSize);

            if (c_oldSize != newSize)
            {
                resizeSectionItem(visual, c_oldSize, newSize, isManual);
            }
        }

        // cascade the section size change
        for (int i = visual + 1; i < sectionItems.count(); ++i)
        {
            if (!sectionIsCascadable(i))
            {
                continue;
            }

            int ncurrentSectionSize = headerSectionSize(i);

            if (ncurrentSectionSize <= c_minimumSize)
            {
                continue;
            }

            int newSectionSize = qMax(ncurrentSectionSize - ndelta, c_minimumSize);
            //qDebug() << "### cascading to" << i << newSectionSize - currentSectionSize << delta;
            resizeSectionItem(i, ncurrentSectionSize, newSectionSize, isManual);
            saveCascadingSectionSize(i, ncurrentSectionSize);
            ndelta = ndelta - (ncurrentSectionSize - newSectionSize);

            //qDebug() << "new delta" << delta;
            //if (newSectionSize != minimumSize)
            if (ndelta <= 0)
            {
                break;
            }
        }
    }
    else // smaller
    {
        bool bsectionResized = false;

        // restore old section sizes
        for (int i = lastCascadingSection; i > visual; --i)
        {
            if (!cascadingSectionSize.contains(i))
            {
                continue;
            }

            int ncurrentSectionSize = headerSectionSize(i);
            int noriginalSectionSize = cascadingSectionSize.value(i);

            if (ncurrentSectionSize >= noriginalSectionSize)
            {
                continue;
            }

            int newSectionSize = ncurrentSectionSize - ndelta;
            resizeSectionItem(i, ncurrentSectionSize, newSectionSize, isManual);

            if (newSectionSize >= noriginalSectionSize && false)
            {
                //qDebug() << "section" << i << "restored to" << originalSectionSize;
                cascadingSectionSize.remove(i); // the section is now restored
            }

            bsectionResized = true;
            break;
        }

        // resize the section
        resizeSectionItem(visual, c_oldSize, qMax(newSize, c_minimumSize), isManual);

        // cascade the section size change
        if (ndelta < 0 && newSize < c_minimumSize)
        {
            for (int i = visual - 1; i >= 0; --i)
            {
                if (!sectionIsCascadable(i))
                {
                    continue;
                }

                int nsectionSize = headerSectionSize(i);

                if (nsectionSize <= c_minimumSize)
                {
                    continue;
                }

                resizeSectionItem(i, nsectionSize, qMax(nsectionSize + ndelta, c_minimumSize), isManual);
                saveCascadingSectionSize(i, nsectionSize);
                break;
            }
        }

        // let the next section get the space from the resized section
        if (!bsectionResized)
        {
            for (int i = visual + 1; i < sectionItems.count(); ++i)
            {
                if (!sectionIsCascadable(i))
                {
                    continue;
                }

                int ncurrentSectionSize = headerSectionSize(i);
                int newSectionSize = qMax(ncurrentSectionSize - ndelta, c_minimumSize);
                resizeSectionItem(i, ncurrentSectionSize, newSectionSize, isManual);
                break;
            }
        }
    }

    viewport->update(viewportScrollArea());
}

void GlodonHeaderViewPrivate::setDefaultSectionSize(int size)
{
    Q_Q(GlodonHeaderView);

    defaultSectionSize = size;

    for (int i = 0; i < sectionItems.count(); ++i)
    {
        GlodonHeaderViewPrivate::SectionItem &span = sectionItems[i];

        if (span.size > 0)
        {
            //we resize it if it is not hidden (ie size > 0)
            const int c_newSize = size;

            if (c_newSize != span.size)
            {
                length += c_newSize - span.size; //the whole length is changed
                const int c_oldSectionSize = span.sectionSize();
                span.size = size;

                emit q->sectionResized(logicalIndex(i), c_oldSectionSize, size, true);
            }
        }
    }
}

void GlodonHeaderViewPrivate::resetTreeSectionItems(const QVector<int> &showRows)
{
    const int nNewSectionCount = modelSectionCount();

    if (showRows.count() == nNewSectionCount)
    {
        SectionItem span(defaultSectionSize, GlodonHeaderView::Interactive);
        sectionItems.insert(sectionItems.count(), nNewSectionCount, span);

        sectionHidden.fill(false);
    }
    else
    {
        for (int i = 0; i < showRows.count(); i++)
        {
            int nNextRowNo;

            if (i == showRows.count() - 1)
            {
                nNextRowNo = nNewSectionCount;
            }
            else
            {
                nNextRowNo = showRows[i + 1];
            }

            int nCurrRowNo = showRows[i];

            for (int j = nCurrRowNo + 1; j < nNextRowNo; j++)
            {
                hiddenSectionSize.insert(j, defaultSectionSize);
            }

            sectionHidden.fill(true, nCurrRowNo + 1, nNextRowNo);
            sectionHidden.setBit(nCurrRowNo, false);

            SectionItem showRowSpan(defaultSectionSize, GlodonHeaderView::Interactive);
            sectionItems.append(showRowSpan);

            SectionItem hideRowSpan(0, GlodonHeaderView::Interactive);
            sectionItems.insert(sectionItems.count(), nNextRowNo - nCurrRowNo - 1, hideRowSpan);
        }
    }

    sectionStartposRecalc = true;

    length = showRows.count() * (defaultSectionSize + gridLineWidth);
}

void GlodonHeaderViewPrivate::resetAfterTreeBuild(const QVector<int> &showRows)
{
    const int nNewSectionCount = modelSectionCount();

    sectionHidden.resize(nNewSectionCount);
    logicalIndices.resize(nNewSectionCount);
    visualIndices.resize(nNewSectionCount);

    sectionItems.clear();

    for (int i = 0; i < nNewSectionCount; ++i)
    {
        logicalIndices[i] = i;
        visualIndices[i] = i;
    }

    if (!hiddenSectionSize.isEmpty())
    {
        hiddenSectionSize.clear();
    }

    resetTreeSectionItems(showRows);
    viewport->update();
}

int GlodonHeaderViewPrivate::headerLength() const
{
    int len = 0;

    for (int i = 0; i < sectionItems.count(); ++i)
    {
        len += sectionItems.at(i).size;
    }

    return len;
}

int GlodonHeaderViewPrivate::fixedSectionsSize() const
{
    Q_Q(const GlodonHeaderView);

    int nsectionsize = 0;

    for (int i = 0; i < fixedCount; i++)
    {
        nsectionsize += q->sectionSize(i) + gridLineWidth;
    }

    return nsectionsize;
}

void GlodonHeaderViewPrivate::resizeSectionItem(int visualIndex, int oldSize, int newSize, bool isManual)
{
    Q_Q(GlodonHeaderView);

    GlodonHeaderView::ResizeMode mode = headerSectionResizeMode(visualIndex);

    createSectionItems(visualIndex, visualIndex, newSize, mode);

    emit q->sectionResized(logicalIndex(visualIndex), oldSize, newSize, isManual);
}

int GlodonHeaderViewPrivate::headerSectionSize(int visual) const
{
    if (visual < sectionItems.count() && visual >= 0)
    {
        return sectionItems.at(visual).sectionSize();
    }

    return -1;
}

int GlodonHeaderViewPrivate::headerSectionPosition(int visual) const
{
    if (visual < sectionItems.count() && visual >= 0)
    {
        if (sectionStartposRecalc)
        {
            recalcSectionStartPos();
        }

        return sectionItems.at(visual).calculated_startpos;
    }

    return -1;
}

int GlodonHeaderViewPrivate::headerVisualIndexAt(int position) const
{
    if (sectionStartposRecalc)
    {
        recalcSectionStartPos();
    }

    int startidx = 0;
    int endidx = sectionItems.count() - 1;

    while (startidx <= endidx)
    {
        int middle = (endidx + startidx) / 2;

        if (sectionItems.at(middle).calculated_startpos > position)
        {
            endidx = middle - 1;
        }
        else
        {
            if (sectionItems.at(middle).calculatedEndPos() < position)
            {
                startidx = middle + 1;
            }
            else // we found it.
            {
                return middle;
            }
        }
    }

    return -1;
}

void GlodonHeaderViewPrivate::setHeaderSectionResizeMode(int visual, GlodonHeaderView::ResizeMode mode)
{
    int nsize = headerSectionSize(visual);
    createSectionItems(visual, visual, nsize, mode);
}

GlodonHeaderView::ResizeMode GlodonHeaderViewPrivate::headerSectionResizeMode(int visual) const
{
    if (visual < 0 || visual >= sectionItems.count())
    {
        return globalResizeMode;
    }

    return static_cast<GlodonHeaderView::ResizeMode>(sectionItems.at(visual).resizeMode);
}

void GlodonHeaderViewPrivate::setGlobalHeaderResizeMode(GlodonHeaderView::ResizeMode mode)
{
    globalResizeMode = mode;

    for (int i = 0; i < sectionItems.count(); ++i)
    {
        sectionItems[i].resizeMode = mode;
    }
}

int GlodonHeaderViewPrivate::viewSectionSizeHint(int logical) const
{
    if (GlodonAbstractItemView *view = dynamic_cast<GlodonAbstractItemView *>(parent))
    {
        return (orientation == Qt::Horizontal
                ? view->sizeHintForColumn(logical)
                : view->sizeHintForRow(logical));
    }

    return 0;
}

int GlodonHeaderViewPrivate::adjustedVisualIndex(int visualIndex) const
{
    if (hiddenSectionSize.count() > 0)
    {
        int nAdjustedVisualIndex = visualIndex;
        int nCurrentVisualIndex = 0;

        for (int i = 0; i < sectionItems.count(); ++i)
        {
            // 在考虑是否有隐藏section时，如果隐藏的section是在固定section，就不需要考虑，因为滚动的区域是固定section之后的区域
            if ((i >= fixedCount) && (sectionItems.at(i).size == 0))
            {
                ++nAdjustedVisualIndex;
            }
            else
            {
                ++nCurrentVisualIndex;
            }

            // 由于currentVisualIndex实际是count，所以不应在等于的时候break // TODO liurx Qt的是>= ，整理headerView时，再详细考虑
            if (nCurrentVisualIndex > visualIndex)
            {
                break;
            }
        }

        visualIndex = nAdjustedVisualIndex;
    }

    return visualIndex;
}

void GlodonHeaderViewPrivate::setScrollOffset(const QScrollBar *scrollBar, GlodonAbstractItemView::ScrollMode scrollMode)
{
    Q_Q(GlodonHeaderView);
    if (scrollMode == GlodonAbstractItemView::ScrollPerItem) {
        if (scrollBar->maximum() > 0 && scrollBar->value() == scrollBar->maximum())
            q->setOffsetToLastSection();
        else
            q->setOffsetToSectionPosition(scrollBar->value());
    } else {
        q->setOffset(scrollBar->value());
    }
}

QRect GlodonHeaderViewPrivate::viewportScrollArea()
{
    Q_Q(GlodonHeaderView);
    int nfixedSize = fixedSectionsSize();

    if (orientation == Qt::Horizontal)
    {
        return QRect(nfixedSize, 0, viewport->width() - nfixedSize, q->drawWidth());
    }
    else
    {
        return QRect(0, nfixedSize, q->drawWidth(), viewport->height() - nfixedSize);
    }
}

#ifndef QT_NO_DATASTREAM
void GlodonHeaderViewPrivate::write(QDataStream &out) const
{
    out << int(orientation);
    out << int(sortIndicatorOrder);
    out << sortIndicatorSection;
    out << sortIndicatorShown;

    out << visualIndices;
    out << logicalIndices;

    out << sectionHidden;
    out << hiddenSectionSize;

    out << length;
    out << sectionItems.count();
    out << movableSections;
    out << clickableSections;
    out << highlightSelected;
    out << cascadingResizing;
    out << defaultSectionSize;
    out << minimumSectionSize;

    out << int(defaultAlignment);
    out << int(globalResizeMode);

    out << sectionItems;
}

bool GlodonHeaderViewPrivate::read(QDataStream &in)
{
    int nOrient(0);
    int nOrder(0);
    int nAlign(0);
    int nGlobal(0);
    in >> nOrient;
    orientation = (Qt::Orientation)nOrient;

    in >> nOrder;
    sortIndicatorOrder = (Qt::SortOrder)nOrder;

    in >> sortIndicatorSection;
    in >> sortIndicatorShown;

    in >> visualIndices;
    in >> logicalIndices;

    in >> sectionHidden;
    in >> hiddenSectionSize;

    in >> length;
    int unusedSectionCount; // For compability
    in >> unusedSectionCount;
    in >> movableSections;
    in >> clickableSections;
    in >> highlightSelected;
    in >> cascadingResizing;
    in >> defaultSectionSize;
    in >> minimumSectionSize;

    in >> nAlign;
    defaultAlignment = Qt::Alignment(nAlign);

    in >> nGlobal;
    globalResizeMode = (GlodonHeaderView::ResizeMode)nGlobal;

    in >> sectionItems;

    // Spans in Qt5 only contains one element - but for backward compability with Qt4 we do the following

    QVector<SectionItem> newSectionSpans;

    for (int u = 0; u < sectionItems.count(); ++u)
    {
        int count = sectionItems.at(u).tmpDataStreamSectionCount;

        for (int n = 0; n < count; ++n)
        {
            newSectionSpans.append(sectionItems[u]);
        }
    }

    sectionItems = newSectionSpans;

    recalcSectionStartPos();

    return true;
}

GlodonHHeaderView::GlodonHHeaderView(QWidget *parent)
    : GlodonHeaderView(Qt::Horizontal, parent)
{}

GlodonHHeaderView::~GlodonHHeaderView()
{}

GlodonVHeaderView::GlodonVHeaderView(QWidget *parent)
    : GlodonHeaderView(Qt::Vertical, parent)
{}

GlodonVHeaderView::~GlodonVHeaderView()
{}

void GlodonHHeaderViewGraphicsEffect::draw(QPainter *painter)
{
    QPoint offset;
    QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset);
    QPainter p(&pixmap);
    p.setBrush(Qt::NoBrush);
    QRect rt = pixmap.rect();
    int x1 = 0;
    int x2 = m_header->length() - 1;
    int y = rt.height() - m_shadowHeight;
    p.setPen(m_clrStart);
    p.drawLine(QPoint(x1, y), QPoint(x2, y));
    ++y;
    p.setPen(m_clrMid);
    p.drawLine(QPoint(x1, y), QPoint(x2, y));
    ++y;
    p.setPen(m_clrEnd);
    p.drawLine(QPoint(x1, y), QPoint(x2, y));
    painter->drawPixmap(offset, pixmap);
}
QRectF GlodonHHeaderViewGraphicsEffect::boundingRectFor(const QRectF &rect) const
{
    QRectF rectF = QGraphicsEffect::boundingRectFor(rect);
    rectF.setHeight(rectF.height() + m_shadowHeight);
    return rectF;
}
#endif // QT_NO_DATASTREAM
