#include "GLDFileUtils.h"
#include "GLDFooterTableView.h"
#include "GLDFooterTableView_p.h"
#include "GLDMultiHeaderView.h"
#include "GLDScrollStyle.h"
#include "GLDTreeModel.h"

#include <QStyleFactory>
#include <QScrollBar>
#include <QPushButton>

GlodonFooterTableView::GlodonFooterTableView(QWidget *parent)
    : GlodonFilterTableView(*new GlodonFooterTableViewPrivate(), parent)
{
    d_func()->init();
}

void GlodonFooterTableView::setHorizontalHeader(GlodonHeaderView *header)
{
    Q_D(GlodonFooterTableView);

    if (!header || header == d->m_horizontalHeader)
    {
        return;
    }

    GlodonFilterTableView::setHorizontalHeader(header);

    d->connectFooterSignal();
}

void GlodonFooterTableView::setModel(QAbstractItemModel *model)
{
    Q_D(GlodonFooterTableView);
    d->m_dataModel = model;

    if (d->totalRowAtFooter)
    {
        createModel();
        resetModel();
    }
    else
    {
        GlodonFilterTableView::setModel(model);
    }

    changeVerticalHeaderDrawWidth();
}

bool GlodonFooterTableView::totalRowAtFooter() const
{
    Q_D(const GlodonFooterTableView);
    return d->totalRowAtFooter;
}

void GlodonFooterTableView::setTotalRowAtFooter(bool value)
{
    Q_D(GlodonFooterTableView);

    if ((d->totalRowAtFooter != value) && (!isGroupMode()))
    {
        d->totalRowAtFooter = value;

        if (value)
            connect(d->footer, SIGNAL(currentIndexChanged(const QModelIndex &)),
                    this, SLOT(setCurrentIndexForFooter(const QModelIndex &)));
        else
            disconnect(d->footer, SIGNAL(currentIndexChanged(const QModelIndex &)),
                       this, SLOT(setCurrentIndexForFooter(const QModelIndex &)));
    }
}

void GlodonFooterTableView::setTotalRowCount(int value)
{
    Q_D(GlodonFooterTableView);

    if (d->footerRowCount != value)
    {
        d->footerRowCount = value;
        createModel();
        resetModel();
    }
}

void GlodonFooterTableView::paintEvent(QPaintEvent *e)
{
    GlodonFilterTableView::paintEvent(e);
}

void GlodonFooterTableView::setGridColor(QColor value)
{
    Q_D(GlodonFooterTableView);

    if (isCustomStyle())
    {
        if (d->footer)
        {
            d->footer->setIsCustomStyle(true);
            d->footer->setGridColor(value);
        }

        GlodonFilterTableView::setGridColor(value);
    }
}

void GlodonFooterTableView::setGridLineColor(QColor value)
{
    Q_D(GlodonFooterTableView);

    if (isCustomStyle())
    {
        if (d->footer)
        {
            d->footer->setIsCustomStyle(true);
            d->footer->setGridLineColor(value);
        }

        GlodonFilterTableView::setGridLineColor(value);
    }
}

void GlodonFooterTableView::setFixedColCount(int fixedColCount)
{
    footer()->setFixedColCount(fixedColCount);
    GlodonFilterTableView::setFixedColCount(fixedColCount);
}

GFooterView *GlodonFooterTableView::footer()
{
    return d_func()->footer;
}

void GlodonFooterTableView::beforeReset()
{
    Q_D(GlodonFooterTableView);

    // 将resetSectionSpan方法从GSPTableView中提到FooterTableView中进行设置
    // 解决删除树节点时，界面显示问题（sectionSpan）
    if (NULL != d->m_verticalHeader)
    {
        d->m_verticalHeader->resetSectionItems();
    }

    GlodonFilterTableView::beforeReset();
}

void GlodonFooterTableView::doReset()
{
    Q_D(GlodonFooterTableView);

    if (NULL != d->m_dataModel)
    {
        GlodonFilterTableView::resetModel(d->m_dataModel);
        changeVerticalHeaderDrawWidth();
    }

    GlodonFilterTableView::doReset();
}

void GlodonFooterTableView::afterReset()
{
    GlodonFilterTableView::afterReset();
}

GlodonFooterTableView::GlodonFooterTableView(GlodonFooterTableViewPrivate &dd, QWidget *parent)
    : GlodonFilterTableView(dd, parent)
{
    d_func()->init();
}

void GlodonFooterTableView::createModel()
{
    Q_D(GlodonFooterTableView);
    freeAndNil(d->footerModel);
    freeAndNil(d->footBodyModel);
    // footBodyModel和footerModel的创建顺序会影响modelReset等槽的调用顺序
    // footBodyModel应该在footerModel之前调用modelReset对应的槽, treeModel
    // 中的缓存才会先更新
    d->footBodyModel = new GlodonFooterBodyModel(d->m_dataModel, this);
    d->footBodyModel->setFooterRowCount(d->footerRowCount);
    d->footerModel = new GlodonFooterModel(d->m_dataModel, this);
    d->footerModel->setFooterRowCount(d->footerRowCount);
}

void GlodonFooterTableView::updateFooterGeometry(int nVerticalHeaderWidth)
{
    Q_D(GlodonFooterTableView);

    QRect oViewportGeometry = d->viewport->geometry();
    int nFooterHeight = footer()->height();
    int nVerticalLeft = oViewportGeometry.left() - nVerticalHeaderWidth;
    int nHorzScrollbarHeight = horizontalScrollBar()->isVisibleTo(this) ? horizontalScrollBar()->height() : 0;
    int nFooterTop = rect().bottom() - nFooterHeight - nHorzScrollbarHeight;

    footer()->setGeometry(
                nVerticalLeft, nFooterTop,
                oViewportGeometry.width() + nVerticalHeaderWidth, nFooterHeight);
}

void GlodonFooterTableView::resetModel()
{
    Q_D(GlodonFooterTableView);
    setFooterModel(d->footerModel);
    //重新绑定
    d->footer->horizontalHeader()->reset();
    d->footer->verticalHeader()->reset();
    GlodonFilterTableView::setModel(d->footBodyModel);
}

void GlodonFooterTableView::changeVerticalHeaderDrawWidth()
{
    Q_D(GlodonFooterTableView);
    d->footer->setVerticalHeaderDrawWidth(d->m_verticalHeader->sizeHint().width());
    updateGeometries();
}

void GlodonFooterTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_D(GlodonFooterTableView);
    GlodonTableView::currentChanged(current, previous);

    if (d->totalRowAtFooter && d->footerRowCount != 0)
    {
        d->footer->resetCurrentIndex(current);
    }
}

void GlodonFooterTableView::columnResized(int column, int oldWidth, int newWidth, bool isManual)
{
    footer()->resizeSection(column, oldWidth, newWidth);
    GlodonFilterTableView::columnResized(column, oldWidth, newWidth, isManual);
}

void GlodonFooterTableView::setCurrentIndex(const QModelIndex &dataIndex)
{
    Q_D(GlodonFooterTableView);

    if (totalRowAtFooter())
    {
        GlodonTableView::setCurrentIndex(d->footBodyModel->index(dataIndex));
    }
    else
    {
        GlodonTableView::setCurrentIndex(dataIndex);
    }
}

void GlodonFooterTableView::setCurrentIndexForFooter(const QModelIndex &footIndex)
{
    QModelIndex current = currentIndex();

    if (footIndex.column() != current.column())
    {
        QAbstractItemModel *curModel = model();
        GlodonTreeModel *treeModel = dynamic_cast<GlodonTreeModel *>(curModel);
        QModelIndex newIndex = curModel->index(current.row(), footIndex.column(), current.parent());

        if (treeModel)
        {
            newIndex = treeModel->dataIndex(newIndex);
        }

        setCurrentIndex(newIndex);
    }
}

void GlodonFooterTableView::updateGeometries()
{
    Q_D(GlodonFooterTableView);

    if (d->m_geometryRecursionBlock)
    {
        return;
    }

    bool bFilterHide = isFilterHide();

    if (verticalHeader()->isHidden())
    {
        d->footer->verticalHeader()->hide();
        d->footer->verticalHeader()->setDrawWidth(0);
    }

    d->m_geometryRecursionBlock = true;

    int nVerticalHeaderWidth = d->m_verticalHeader->drawWidth();
    int nHorizontalHeaderHeight = d->m_horizontalHeader->drawWidth();
    int nFilterViewHeight = bFilterHide ? 0 : filter()->height();
    int nAboveGridHeight = nHorizontalHeaderHeight + nFilterViewHeight;

    int nFooterHeight = footer()->height();

    setViewportMargins(nVerticalHeaderWidth, nAboveGridHeight, 0, qMax(0, nFooterHeight - frameWidth()));

    // update headers
    updateVerticalHeaderGeometry(nVerticalHeaderWidth);
    updateHorizontalHeaderGeometry(nHorizontalHeaderHeight, nFilterViewHeight);

    // 更新过滤行
    updateFilterViewGeometry(nFilterViewHeight);

    // update cornerWidget
    updateConrnerWidgetGeometry(nVerticalHeaderWidth, nAboveGridHeight);

    // 更新合计行
    updateFooterGeometry(nVerticalHeaderWidth);

    QSize oViewportSize = d->viewport->size();
    QSize oViewportMaxSize = maximumViewportSize();
    uint nHorizontalHeaderLength = d->m_horizontalHeader->length();
    uint nVerticalHeaderLength = d->m_verticalHeader->length();

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

void GlodonFooterTableView::setFooterModel(QAbstractItemModel *model)
{
    footer()->setModel(model);
    changeVerticalHeaderDrawWidth();
}

void GlodonFooterTableViewPrivate::init()
{
    Q_Q(GlodonFooterTableView);
    footer = new GFooterView(q);
    connectFooterSignal();
    q->changeVerticalHeaderDrawWidth();
    footer->setItemDelegate(new GFooterViewDelegate(q));

    q->setProperty("TableView", "GLDTableView");
    q->setStyleSheet(loadQssFile(c_sTableViewQssFile));
    GLDScrollStyle *scrollStyle = new GLDScrollStyle(q, false);
    scrollStyle->setIsShowArrow(false);
    q->horizontalScrollBar()->setStyle(scrollStyle);
    q->verticalScrollBar()->setStyle(scrollStyle);
}

void GlodonFooterTableViewPrivate::connectFooterSignal()
{
//    QObject::connect(horizontalHeader, SIGNAL(sectionResized(int,int,int)),
//            footer, SLOT(resizeSection(int,int,int)));
    QObject::connect(m_horizontalHeader, SIGNAL(sectionMoved(int, int, int)),
                     footer, SLOT(moveSection(int, int, int)));
    QObject::connect(m_horizontalHeader, SIGNAL(scrolled(int)),
                     footer, SLOT(setOffset(int)));
    QObject::connect(footer->horizontalHeader(), SIGNAL(scrolled(int)),
                     m_horizontalHeader, SLOT(setOffset(int)));
    QObject::connect(footer->verticalHeader(), SIGNAL(drawWidthChanged(int)),
                     m_verticalHeader, SLOT(setDrawWidth(int)));
}
