#include "GLDFileUtils.h"
#include "GLDFilterTableView.h"
#include "GLDFilterTableView_p.h"
#include "GLDMultiHeaderView.h"
#include "GLDScrollStyle.h"

#include <QScrollBar>
#include <QPushButton>

#define CON_NO_RESULT "没有合适的筛选结果"

GlodonFilterTableView::GlodonFilterTableView(QWidget *parent)
    : GlodonTableView(*new GlodonFilterTableViewPrivate(), parent)
{
    d_func()->init();
}

void GlodonFilterTableView::setModel(QAbstractItemModel *model)
{
    if (m_bDisplayFilter)
    {
        filter()->setModel(model);
    }

    GlodonTableView::setModel(model);
}

void GlodonFilterTableView::paintEvent(QPaintEvent *e)
{
    GlodonTableView::paintEvent(e);
}

void GlodonFilterTableView::setHorizontalHeader(GlodonHeaderView *header)
{
    Q_D(GlodonFilterTableView);

    if (!header || header == d->m_horizontalHeader)
    {
        return;
    }

    GlodonTableView::setHorizontalHeader(header);
    d->connectFilterSignal();
}

void GlodonFilterTableView::setIsDisplayFilter(bool show)
{
    Q_D(GlodonFilterTableView);

    if ((show == m_bDisplayFilter) || isGroupMode())//过滤行与合计行互斥
    {
        return;
    }

    m_bDisplayFilter = show;

    if (!d->filter)
    {
        d->initFilter();
    }

    if (show)
    {
        // TODO wangdd-a：使用dataModel，在没有SetIsTree的时候，会崩溃
        // 鉴于现在这个功能没人使用，因此先修改为直接设置Model，树形显示可能存在BUG，没有详细测试
        // 后面过滤表格需要重新实现为与Excel一致
        d->filter->setModel(model());

        for (int nCol = 0; nCol < d->m_horizontalHeader->sectionCount(); ++nCol)
        {
            d->filter->resizeSection(nCol, 0, d->m_horizontalHeader->sectionSize(nCol));
        }

        d->filter->show();
    }
    else
    {
        d->filter->hide();
    }

    updateGeometries();
}

bool GlodonFilterTableView::isDisplayFilter()
{
    return m_bDisplayFilter;
}

void GlodonFilterTableView::updateGeometries()
{
    Q_D(GlodonFilterTableView);

    if (d->m_geometryRecursionBlock)
    {
        return;
    }

    d->m_geometryRecursionBlock = true;

    bool bFilterHide = isFilterHide();
    int nVerticalHeaderWidth = d->m_verticalHeader->drawWidth();
    int nHorizontalHeaderHeight = d->m_horizontalHeader->drawWidth();
    int nFilterViewHeight = bFilterHide ? 0 : filter()->height();

    int nAboveGridHeight = nHorizontalHeaderHeight + nFilterViewHeight;

    setViewportMargins(nVerticalHeaderWidth, nAboveGridHeight, 0, 0);

    // update headers
    updateVerticalHeaderGeometry(nVerticalHeaderWidth);
    updateHorizontalHeaderGeometry(nHorizontalHeaderHeight, nFilterViewHeight);

    // 更新过滤行
    updateFilterViewGeometry(nFilterViewHeight);

    // update cornerWidget
    updateConrnerWidgetGeometry(nVerticalHeaderWidth, nAboveGridHeight);

    QSize oViewportSize = d->viewport->size();
    QSize oViewportMaxSize = maximumViewportSize();
    uint nVerticalHeaderLength = d->m_verticalHeader->length();
    uint nHorizontalHeaderLength = d->m_horizontalHeader->length();

    if ((uint)oViewportMaxSize.width() >= nHorizontalHeaderLength
            && (uint)oViewportMaxSize.height() >= nVerticalHeaderLength)
    {
        oViewportSize = oViewportMaxSize;
    }

    updateHorizontalScrollBar(oViewportSize);
    updateVerticalScrollBar(oViewportSize);

    d->m_geometryRecursionBlock = false;
    GlodonAbstractItemView::updateGeometries();
}

void GlodonFilterTableView::setFixedColCount(int fixedColCount)
{
    if (!isFilterHide())
    {
        filter()->setFixedCount(fixedColCount);
    }

    GlodonTableView::setFixedColCount(fixedColCount);
}

GFilterView *GlodonFilterTableView::filter()
{
    Q_D(GlodonFilterTableView);
    return d->filter;
}

void GlodonFilterTableView::setGridColor(QColor value)
{
    Q_D(GlodonFilterTableView);

    if (isCustomStyle())
    {
        if (d->filter)
        {
            d->filter->setGridColor(value);
        }

        GlodonTableView::setGridColor(value);
    }
}

void GlodonFilterTableView::setGridLineColor(QColor value)
{
    Q_D(GlodonFilterTableView);

    if (isCustomStyle())
    {
        if (d->filter)
        {
            d->filter->setGridLineColor(value);
        }

        GlodonTableView::setGridLineColor(value);
    }
}

void GlodonFilterTableView::changeViewItemsForFilter(int column)
{
    Q_UNUSED(column);
    Q_D(GlodonFilterTableView);

    for (int row = 0; row < d->m_model->rowCount(); row++)
    {
        bool bHide = false;

        for (int col = 0; col < d->m_model->columnCount(); col++)
        {
            QString strFilterData = filter()->filterData(col);

            if (strFilterData == "NULL")
            {
                continue;
            }

            QString strModelData = d->m_model->data(d->m_model->index(row, col)).toString();

            if (!strModelData.contains(strFilterData))
            {
                bHide = true;
                break;
            }
        }

        setRowHidden(row, bHide);
    }
}

GlodonFilterTableView::GlodonFilterTableView(GlodonFilterTableViewPrivate &dd, QWidget *parent)
    : GlodonTableView(dd, parent)
{
    d_func()->init();
}

void GlodonFilterTableView::timerEvent(QTimerEvent *event)
{
//    Q_D(GlodonFilterTableView);

//    if (event->timerId() == d->m_columnResizeTimerID)
//    {
//        updateGeometries();
//        killTimer(d->m_columnResizeTimerID);
//        d->m_columnResizeTimerID = 0;

//        QRect rect;
//        int nviewportHeight = d->viewport->height();
//        int nviewportWidth = d->viewport->width();

//        if (d->hasSpans())
//        {
//            rect = QRect(0, 0, nviewportWidth, nviewportHeight);
//        }
//        else
//        {
//            for (int i = d->m_columnsToUpdate.size() - 1; i >= 0; --i)
//            {
//                int column = d->m_columnsToUpdate.at(i);
//                int x = columnViewportPosition(column);

//                if (isRightToLeft())
//                {
//                    rect |= QRect(0, 0, x + columnWidth(column), nviewportHeight);
//                }
//                else
//                {
//                    rect |= QRect(x, 0, nviewportWidth - x, nviewportHeight);
//                }
//            }
//        }

//        d->viewport->update(rect.normalized());

//        if (!isFilterHide())
//        {
//            d->filter->viewport()->update(rect.normalized());
//        }

//        d->m_columnsToUpdate.clear();
//    }
//    else
//    {
        return GlodonTableView::timerEvent(event);
//    }
}

void GlodonFilterTableView::resetModel(QAbstractItemModel *dataModel)
{
    if (m_bDisplayFilter)
    {
        filter()->setModel(dataModel);
    }
}

bool GlodonFilterTableView::isFilterHide()
{
    if (!filter())
    {
        return true;
    }
    else
    {
        return filter()->isHidden();
    }
}

void GlodonFilterTableView::updateFilterViewGeometry(int nFilterViewHeight)
{
    Q_D(GlodonFilterTableView);

    QRect oViewportGeometry = d->viewport->geometry();

    if (!isFilterHide())
    {
        int nFilterTop = oViewportGeometry.top() - nFilterViewHeight;
        filter()->setGeometry(oViewportGeometry.left(), nFilterTop, oViewportGeometry.width(), nFilterViewHeight);
    }
}

void GlodonFilterTableView::updateHorizontalHeaderGeometry(int nHorizontalHeaderHeight, int nFilterViewHeight)
{
    Q_D(GLDTableView);

    QRect oViewportGeometry = d->viewport->geometry();
    int nHorizontalTop = oViewportGeometry.top() - (nHorizontalHeaderHeight + nFilterViewHeight);

    d->m_horizontalHeader->setGeometry(
                oViewportGeometry.left(), nHorizontalTop,
                oViewportGeometry.width(), nHorizontalHeaderHeight);

    if (d->m_horizontalHeader->isHidden())
    {
        QMetaObject::invokeMethod(d->m_horizontalHeader, "updateGeometries");
    }
}

void GlodonFilterTableView::resetEllipsisButtonLocation()
{
    Q_D(GlodonFilterTableView);

    if (d->m_showEllipsisButton && NULL != d->m_ellipsisButton)
    {
        GlodonTableView::resetEllipsisButtonLocation();

        if (NULL == d->filter)
        {
            return;
        }

        QPoint posWithoutFilter = d->m_ellipsisButton->pos();
        d->m_ellipsisButton->move(posWithoutFilter + QPoint(0, filter()->height()));
    }
}

void GlodonFilterTableView::columnResized(int column, int oldWidth, int newWidth, bool isManual)
{
    if (!isFilterHide())
    {
        filter()->resizeSection(column, oldWidth, newWidth);
    }

    GlodonTableView::columnResized(column, oldWidth, newWidth, isManual);
}

GlodonFilterTableViewPrivate::GlodonFilterTableViewPrivate() : filter(NULL)
{
}

void GlodonFilterTableViewPrivate::init()
{
    Q_Q(GlodonFilterTableView);
    q->m_bDisplayFilter = false;

    q->setProperty("TableView", "GLDTableView");
    q->setStyleSheet(loadQssFile(c_sTableViewQssFile));
    GLDScrollStyle *scrollStyle = new GLDScrollStyle(q, false);
    scrollStyle->setIsShowArrow(false);
    q->horizontalScrollBar()->setStyle(scrollStyle);
    q->verticalScrollBar()->setStyle(scrollStyle);
}

void GlodonFilterTableViewPrivate::initFilter()
{
    Q_Q(GlodonFilterTableView);
    filter = new GFilterView(q);
    filter->hide();
    QObject::connect(filter, SIGNAL(onFilterChanged(int)), q, SLOT(changeViewItemsForFilter(int)));
    QObject::connect(hbar, SIGNAL(valueChanged(int)), filter, SLOT(hideHiddenSection(int)));
    connectFilterSignal();
}

void GlodonFilterTableViewPrivate::connectFilterSignal()
{
//    QObject::connect(m_horizontalHeader, SIGNAL(sectionResized(int,int,int)),
//            filter, SLOT(resizeSection(int,int,int)));
    if (NULL != filter)
    {
        QObject::connect(m_horizontalHeader, SIGNAL(sectionMoved(int, int, int)),
                         filter, SLOT(moveSection(int, int, int)));
        QObject::connect(m_horizontalHeader, SIGNAL(scrolled(int)), filter, SLOT(setOffset(int)));
    }
}
