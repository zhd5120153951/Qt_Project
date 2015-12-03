#include "GLDListView.h"
#include "GLDShellWidgets.h"
#include "GLDScrollStyle.h"
#include "GLDFileSystemModel.h"
#include "GLDStrUtils.h"
//#include <private/qabstractitemview_p.h>

#include <QAccessible>
#include <QAbstractItemView>
#include <QListView>
#include <QPaintEvent>
#include <QScroller>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QToolTip>

const int c_nSizeUnitLength = 2;

const GString c_sKB = "KB";
const GString c_sMB = "MB";
const GString c_sGB = "GB";
const GString c_sTB = "TB";

const quint64 c_kb = 1024;
const quint64 c_mb = 1024 * c_kb;
const quint64 c_gb = 1024 * c_mb;
const quint64 c_tb = 1024 * c_gb;

GLDListView::GLDListView(QWidget *parent)
    : QListView(parent), stopDelayedLayoutTimer(false), m_bShowToolTip(true), m_bDragEnabled(true),
      m_bVsModeSetted(false), m_viewMode(VSSmallIconMode), m_shellTree(0)
{
    bool bUseMyDelegateForStyleDraw = false;
    if (bUseMyDelegateForStyleDraw)
        setItemDelegate(new GLDListStyledItemDelegate(this));

    stopDelayedLayoutTime = QTime::currentTime();
    stopDelayedLayoutTime.start();

    if (bUseMyDelegateForStyleDraw)
        setStyle(new GLDWindowsVistaStyle(this));
    init();
}

void GLDListView::init()
{
    connect(this, SIGNAL(pressed(const QModelIndex&)), this, SIGNAL(itemJustBeSelected(const QModelIndex&)));
    QTimer::singleShot(10, this, SLOT(initVsViewState()));
}

GLDListView::~GLDListView()
{
}

void GLDListView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);
    if (m_shellTree)
        m_shellTree->setModel(model);
}

void GLDListView::setRootIndex(const QModelIndex &index)
{
    QListView::setRootIndex(index);
    if (m_shellTree)
        m_shellTree->setRootIndex(m_shellTree->proxyIndex(index));
}

QRect GLDListView::visualRect(const QModelIndex &index) const
{
    QRect tmp = QListView::visualRect(index);
    QSize grid = gridSize();
    if (VSIconMode == vsViewMode())
    {
        if (tmp.width() + 1 < grid.width())
        {
            int nhalfWidth = (grid.width() - tmp.width() - 1 ) / 2.0;
            tmp.adjust(-nhalfWidth, 0, nhalfWidth, 0);
        }
        if (tmp.height() + 1 < grid.height())
        {
            GLDWindowsVistaStyle *pStyle = dynamic_cast<GLDWindowsVistaStyle *>(this->style());
            bool badjusted = false;
            if (pStyle && pStyle->m_textLineHeight > 0)
            {
                double distance = grid.height() - tmp.height();
                int nlineCount = distance / pStyle->m_textLineHeight;
                if (nlineCount > 0)
                {
                    tmp.adjust(0, 0, 0, qMin(( nlineCount * pStyle->m_textLineHeight - 1), distance));
                    badjusted = true;
                }
            }
            if (!badjusted)
                tmp.adjust(0, 0, 0,grid.height() - tmp.height());
        }
    }
    else if (VSSmallIconMode == vsViewMode() || VSListMode == vsViewMode())
    {
        if (tmp.width() != grid.width())
        {
            tmp.adjust(0,0,grid.width() - tmp.width(),0);
        }
    }
    return tmp;
}

void GLDListView::paintEvent(QPaintEvent *e)
{
    Q_D(GLDListView);
    if (currentReportMode())
        return;

    if (!d->itemDelegate)
        return;
    QStyleOptionViewItem option = d->viewOptions();
    QPainter painter(d->viewport);

    const QVector<QModelIndex> c_toBeRendered = d->intersectingSet(e->rect().translated(horizontalOffset(),
                                                                                      verticalOffset()), false);

    const QModelIndex c_current = currentIndex();
    const QModelIndex c_hover = d->hover;
    const QAbstractItemModel *c_itemModel = d->model;
    const QItemSelectionModel *c_selections = d->selectionModel;
    const bool c_focus = (hasFocus() || d->viewport->hasFocus()) && c_current.isValid();
    const bool c_alternate = d->alternatingColors;
    const QStyle::State c_state = option.state;
    const QAbstractItemView::State c_viewState = this->state();
    const bool c_enabled = (c_state & QStyle::State_Enabled) != 0;

    bool balternateBase = false;
    int npreviousRow = -2; // trigger the alternateBase adjustment on first pass

    int nmaxSize = (flow() == TopToBottom)
        ? qMax(viewport()->size().width(), d->contentsSize().width()) - 2 * d->spacing()
        : qMax(viewport()->size().height(), d->contentsSize().height()) - 2 * d->spacing();

    int nitemsCount = c_toBeRendered.count();
    for (int nindex = nitemsCount - 1; nindex > -1; --nindex)
    {
        QModelIndex it = c_toBeRendered.at(nindex);
        Q_ASSERT((it).isValid());
        option.rect = visualRect(it);

        if (flow() == TopToBottom)
            option.rect.setWidth(qMin(nmaxSize, option.rect.width()));
        else
            option.rect.setHeight(qMin(nmaxSize, option.rect.height()));

        option.state = c_state;
        if (c_selections && c_selections->isSelected(it))
            option.state |= QStyle::State_Selected;
        if (c_enabled)
        {
            QPalette::ColorGroup cg;
            if ((c_itemModel->flags(it) & Qt::ItemIsEnabled) == 0)
            {
                option.state &= ~QStyle::State_Enabled;
                cg = QPalette::Disabled;
            }
            else
            {
                cg = QPalette::Normal;
            }
            option.palette.setCurrentColorGroup(cg);
        }
        if (c_focus && c_current == it)
        {
            option.state |= QStyle::State_HasFocus;
            if (c_viewState == QAbstractItemView::EditingState)
                option.state |= QStyle::State_Editing;
        }
        if (it == c_hover)
            option.state |= QStyle::State_MouseOver;
        else
            option.state &= ~QStyle::State_MouseOver;

        if (c_alternate)
        {
            int row = (it).row();
            if (row != npreviousRow + 1)
            {
                // adjust alternateBase according to rows in the "gap"
                if (!d->hiddenRows.isEmpty())
                {
                    for (int r = qMax(npreviousRow + 1, 0); r < row; ++r)
                    {
                        if (!d->isHidden(r))
                            balternateBase = !balternateBase;
                    }
                }
                else
                {
                    balternateBase = (row & 1) != 0;
                }
            }
            if (balternateBase)
            {
                option.features |= QStyleOptionViewItem::Alternate;
            }
            else
            {
                option.features &= ~QStyleOptionViewItem::Alternate;
            }

            // draw background of the item (only alternate row). rest of the background
            // is provided by the delegate
            QStyle::State oldState = option.state;
            option.state &= ~QStyle::State_Selected;
            style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &option, &painter, this);
            option.state = oldState;
            balternateBase = !balternateBase;
            npreviousRow = row;
        }

        option.features |= QStyleOptionViewItem::WrapText;
        d->delegateForIndex(it)->paint(&painter, option, it);
    }

#ifndef QT_NO_DRAGANDDROP
    d->commonListView->paintDragDrop(&painter);
#endif

#ifndef QT_NO_RUBBERBAND
    // #### move this implementation into a dynamic class
    if (d->showElasticBand && d->elasticBand.isValid()) {
        QStyleOptionRubberBand opt;
        opt.initFrom(this);
        opt.shape = QRubberBand::Rectangle;
        opt.opaque = false;
        opt.rect = d->mapToViewport(d->elasticBand, false).intersected(
            d->viewport->rect().adjusted(-16, -16, 16, 16));
        painter.save();
        style()->drawControl(QStyle::CE_RubberBand, &opt, &painter);
        painter.restore();
    }
#endif

}

void GLDListView::timerEvent(QTimerEvent *e)
{
    if (false == stopDelayedLayoutTimer)
    {
        stopDelayedLayoutTimer = true;
        if (stopDelayedLayoutTime.elapsed() < 100)
        {
            stopDelayedLayoutTime.restart();
            return;
        }
    }
    return QListView::timerEvent(e);
}

void GLDListView::switchDragDropState()
{
    if (!currentReportMode())
    {
        if (m_bDragEnabled)
        {
            setDragDropMode(DragOnly);
        }
        else
        {
            setDragDropMode(NoDragDrop);
        }
    }
}

void GLDListView::switchChildTreeDragDropState()
{
    if (m_bDragEnabled)
    {
        m_shellTree->setDragDropMode(DragOnly);
    }
    else
    {
        m_shellTree->setDragDropMode(NoDragDrop);
    }
}

innerGLD::innerShellTreeView *GLDListView::createTreeView()
{
    return new innerGLD::innerShellTreeView(this);
}

void GLDListView::onInnerShellTreeViewClick(const QModelIndex &index)
{
    QModelIndex oDataIndex = m_shellTree->dataIndex(index);
    QModelIndex oCurrentIndex = model()->index(oDataIndex.row(), 0, oDataIndex.parent());

    setCurrentIndex(oCurrentIndex);
    emit clicked(oCurrentIndex);
}

void GLDListView::setVsViewMode(GLDListView::GLDViewMode mode)
{
    if (m_viewMode == mode)
        return;

    m_bVsModeSetted = true;
    if (currentReportMode())
    {
        if (m_shellTree)
        {
            m_shellTree->hide();
        }

        viewport()->show();
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
    else if (!currentReportMode() && mode == GLDListView::VSReportMode)
    {
        if (m_shellTree != NULL)
        {
            m_shellTree->show();
        }

        viewport()->hide();
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    setWrapping(true);
    setUniformItemSizes(true);
    switch (mode)
    {
    case VSListMode:    //OK
        QListView::setViewMode(QListView::ListMode);
        setGridSize(QSize(230,17));//GSP-528，与TcxShellListView保持一致。原生200,但考虑到图标文字间距以及字体差别
        setFlow(TopToBottom);
        break;
    case VSIconMode:
    {
        QListView::setViewMode(QListView::IconMode);
        QSize size(100,70);
        setGridSize(size);
        setFlow(LeftToRight);
        setLayoutMode(Batched);
    }
        break;
    case VSSmallIconMode:
    {
        setGridSize(QSize(118,17));
        setFlow(LeftToRight);
        QListView::setViewMode(QListView::ListMode);
    }
        break;
    case VSReportMode:
        //to do ,switch between list/tree views
    {
        if (!m_shellTree)
        {
            m_shellTree = createTreeView();//(parentWidget());
            connect(m_shellTree, SIGNAL(clicked(QModelIndex)), this, SLOT(onInnerShellTreeViewClick(QModelIndex)));
            if (!layout())
            {
                QVBoxLayout *lay = new QVBoxLayout;
                lay->addWidget(m_shellTree);
                lay->setContentsMargins(0,0,0,0);
                setLayout(lay);
            }
            m_shellTree->setStyleSheet("QTreeView{border:0}");
            //inherited init action
            m_shellTree->setAnimated(false);
            m_shellTree->setIndentation(0);
            m_shellTree->setUniformRowHeights(true);
            m_shellTree->setSortingEnabled(true);
        }
        if (m_shellTree->dataModel() != model())
        {
            m_shellTree->setModel(model());
        }
        QModelIndex rootIndex = this->rootIndex();
        if (rootIndex.isValid())
        {
            m_shellTree->setRootIndex(m_shellTree->proxyIndex(rootIndex));
        }

        QModelIndex currentIndex = this->currentIndex();
        if (currentIndex.isValid())
        {
            m_shellTree->setCurrentIndex(m_shellTree->proxyIndex(currentIndex));
        }
        switchChildTreeDragDropState();

        m_shellTree->setGeometry(geometry());
        m_shellTree->showNormal();
    }
        break;
    default:
        break;
    }
    switchDragDropState();
    m_viewMode = mode;
}

GLDListView::GLDViewMode GLDListView::vsViewMode() const
{
    return m_viewMode;
}

void GLDListView::enterEdit(const QModelIndex &index)
{
    if (VSReportMode == m_viewMode)
    {
        m_shellTree->edit(m_shellTree->proxyIndex(index));
    }
    else
    {
        edit(index);
    }
}

void GLDListView::setSectionResizeMode(int logicalIndex, QHeaderView::ResizeMode mode)
{
    if (VSReportMode == m_viewMode)
    {
        m_shellTree->header()->setSectionResizeMode(logicalIndex, mode);
    }
}

void GLDListView::setDragActionEnable(bool enable)
{
    m_bDragEnabled = enable;
}

bool GLDListView::dragActionEnabled() const
{
    return m_bDragEnabled;
}

QString GLDListView::currentVSViewStateName() const
{
    switch (m_viewMode)
    {
    case GLDListView::VSListMode:
        return tr("List");
    case GLDListView::VSIconMode:
        return tr("Icon");
    case GLDListView::VSSmallIconMode:
        return tr("Small Icon");
    case GLDListView::VSReportMode:
        return tr("Report");
    default:
        return tr("Error");
    }
}

innerGLD::innerShellTreeView *GLDListView::shellTreeView()
{
    return m_shellTree;
}

void GLDListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (m_bShowToolTip)
    {
        if (1 == selected.size())
        {
            if (1 == selected.at(0).indexes().size())
            {
                QModelIndex  focusIndex = selected.at(0).indexes().at(0);
                if (focusIndex.isValid())
                {
                    setToolTip(focusIndex.data().toString());
                }
            }
        }
    }
    return QListView::selectionChanged(selected, deselected);
}

void GLDListView::initVsViewState()
{
    if (!m_bVsModeSetted)
    {
        setVsViewMode(VSListMode);
    }
}

bool GLDListView::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
        QModelIndex index = indexAt(helpEvent->pos());
        if (index.isValid())
        {
            QToolTip::showText(helpEvent->globalPos(), index.data().toString());
        }
        else
        {
            QToolTip::hideText();
            e->ignore();
        }

        return true;
    }
    return inherit::event(e);
}

void GLDListView::setViewMode(QListView::ViewMode mode)
{
    Q_UNUSED(mode);
    Q_ASSERT(false);
}

void GLDListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QListView::currentChanged(current, previous);
    emit currentItemJustChanged(current, previous);
//    if (m_viewMode != VSReportMode)
//        emit itemJustBeSelected(current);
}

void GLDListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    const QModelIndex c_clickedIndex = indexAt(event->pos());
    if (!currentReportMode())
    {
        if (c_clickedIndex.isValid())
            emit itemJustBeDoubleClicked(c_clickedIndex);
    }
}

void GLDListView::dropEvent(QDropEvent *e)
{
    //IconMode时，一直是DragDrop，只有在此处不响应，否则会放乱。
    if (viewMode() == IconMode)
        return;
    QListView::dropEvent(e);
}

QRect GLDListViewPrivate::mapToViewport(const QRect &rect, bool extend) const
{
    Q_Q(const GLDListView);
    if (!rect.isValid())
        return rect;

    QRect result = extend ? commonListView->mapToViewport(rect) : rect;
    int ndx = -q->horizontalOffset();
    int ndy = -q->verticalOffset();
    return result.adjusted(ndx, ndy, ndx, ndy);
}

QStyleOptionViewItem GLDListViewPrivate::viewOptions() const
{
    Q_Q(const GLDListView);
    QStyleOptionViewItem option = abstractViewOptions();//QAbstractItemViewPrivate::viewOptions();
    if (!iconSize.isValid()) { // otherwise it was already set in abstractitemview
        int pm = (viewMode == GLDListView::ListMode
                  ? q->style()->pixelMetric(QStyle::PM_ListViewIconSize, 0, q)
                  : q->style()->pixelMetric(QStyle::PM_IconViewIconSize, 0, q));
        option.decorationSize = QSize(pm, pm);
    }
    if (viewMode == GLDListView::IconMode) {
        option.showDecorationSelected = false;
        option.decorationPosition = QStyleOptionViewItem::Top;
        option.displayAlignment = Qt::AlignCenter;
    } else {
        option.decorationPosition = QStyleOptionViewItem::Left;
    }
    return option;
}

void GLDListViewPrivate::interruptDelayedItemsLayout() const
{
    Q_Q(GLDListView const);
    q->stopDelayedLayoutTime.restart();
    q->stopDelayedLayoutTimer = true;
    delayedPendingLayout = false;
}

QStyleOptionViewItem GLDListViewPrivate::abstractViewOptions() const
{
    Q_Q(const GLDListView);
    QStyleOptionViewItem option;
    option.init(q);
    option.state &= ~QStyle::State_MouseOver;
    option.font = q->font();

#ifndef Q_WS_MAC
    // On mac the focus appearance follows window activation
    // not widget activation
    if (!q->hasFocus())
        option.state &= ~QStyle::State_Active;
#endif

    option.state &= ~QStyle::State_HasFocus;
    if (iconSize.isValid()) {
        option.decorationSize = iconSize;
    } else {
        int pm = q->style()->pixelMetric(QStyle::PM_SmallIconSize, 0, q);
        option.decorationSize = QSize(pm, pm);
    }
    option.decorationPosition = QStyleOptionViewItem::Left;
    option.decorationAlignment = Qt::AlignCenter;
    option.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    option.textElideMode = textElideMode;
    option.rect = QRect();
    option.showDecorationSelected = (0 != q->style()->styleHint(QStyle::SH_ItemView_ShowDecorationSelected, 0, q));
    if (wrapItemText)
        option.features = QStyleOptionViewItem::WrapText;
    option.locale = q->locale();
    option.locale.setNumberOptions(QLocale::OmitGroupSeparator);
    option.widget = q;
    return option;
}

void GLDListStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QWidget *widget = option.widget;///QStyledItemDelegatePrivate::widget(option);
    QStyle *style = widget ? widget->style() : QApplication::style();
    if (dynamic_cast<GLDWindowsVistaStyle *>(style))
    {
        const QStyleOptionViewItem *vopt = &opt;
        const QAbstractItemView *view = dynamic_cast<const QAbstractItemView *>(widget);
        if (view)
        {
            QPalette palette = vopt->palette;
            palette.setColor(QPalette::All, QPalette::HighlightedText, palette.color(QPalette::Active, QPalette::Text));
            // Note that setting a saturated color here results in ugly XOR colors in the focus rect
            palette.setColor(QPalette::All, QPalette::Highlight, palette.base().color().darker(108));
            QStyleOptionViewItem adjustedOption = *vopt;
            adjustedOption.palette = palette;
            // We hide the  focusrect in singleselection as it is not required
            if ((view->selectionMode() == QAbstractItemView::SingleSelection)
                    && !(vopt->state & QStyle::State_KeyboardFocusChange))
                adjustedOption.state &= ~QStyle::State_HasFocus;
        }
    }
//    opt.rect.adjust(0,0,0,20);
    opt.features |= QStyleOptionViewItem::WrapText;
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

QRect GLDWindowsVistaStyle::subElementRect(QStyle::SubElement sr, const QStyleOption *opt, const QWidget *widget) const
{
    QRect tmp = QCommonStyle::subElementRect(sr, opt, widget);
    if (SE_ItemViewItemText == sr)
    {
        if (m_custom)
        {
            QSize grid = m_custom->gridSize();
            if (grid.isValid())
            {
                double dFontHeight = opt->fontMetrics.height();
                m_textLineHeight = dFontHeight;
                int nAdjustedHeight = grid.height() - tmp.height();
                int nHalfWidth = (grid.width() - tmp.width() - 1) / 2.0;
                if (dFontHeight > 5 && nAdjustedHeight > 0)
                {
                    int nlineCount = nAdjustedHeight / dFontHeight;
                    tmp.adjust(-nHalfWidth, 0, nHalfWidth, (nlineCount - 1) * dFontHeight);
                }
            }
        }
    }
    else if (SE_ItemViewItemFocusRect == sr)
    {
        if (m_custom)
        {
            QSize grid = m_custom->gridSize();
            double dFontHeight = opt->fontMetrics.height();
            int nadjustedHeight = grid.height() - tmp.height();
            int nhalfWidth = (grid.width() - tmp.width() - 1) / 2.0;
            if (dFontHeight > 5 && nadjustedHeight > 0)
            {
                int nlineCount = nadjustedHeight / dFontHeight;
                tmp.adjust(-nhalfWidth, 0, nhalfWidth, (nlineCount - 1) * dFontHeight);
            }
        }
    }
    else if (static_cast<int>(PE_PanelItemViewItem) == static_cast<int>(sr))
    {
        if (m_custom)
        {
            QSize grid = m_custom->gridSize();
            double dFontHeight = opt->fontMetrics.height();
            int nadjustedHeight = grid.height() - tmp.height();
            int nhalfWidth = (grid.width() - tmp.width() - 1) / 2.0;
            if (dFontHeight > 5 && nadjustedHeight > 0)
            {
                int nlineCount = nadjustedHeight / dFontHeight;
                tmp.adjust(-nhalfWidth, 0, nhalfWidth, (nlineCount - 1) * dFontHeight);
            }
        }
    }
    else
    {
        if (m_custom)
        {
            QSize grid = m_custom->gridSize();
            double dfontHeight = opt->fontMetrics.height();
            int nadjustedHeight = grid.height() - tmp.height();
            int nhalfWidth = (grid.width() - tmp.width() - 1) / 2.0;
            if (dfontHeight > 5 && nadjustedHeight > 0)
            {
                int nlineCount = nadjustedHeight / dfontHeight;
                tmp.adjust(-nhalfWidth, 0, nhalfWidth, (nlineCount - 1) * dfontHeight);
            }
        }
    }
    return tmp;
}

void GLDWindowsVistaStyle::drawPrimitive(QStyle::PrimitiveElement element,
                                         const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == PE_PanelItemViewItem)
    {
//        painter->setClipping(false);
        QStyleOption * opt = const_cast<QStyleOption *>(option);
        if (opt && opt->type == QStyleOption::SO_ViewItem)
        {
            QStyleOptionViewItem *pOpt = static_cast<QStyleOptionViewItem *>(opt);
            if (pOpt->index.isValid())
            {
                if (/*GLDListView *pList = */dynamic_cast<GLDListView *>(m_custom))
                {
//                    if (pList->selectionModel()->isSelected(pOpt->index))
//                    {
//                        opt->rect.adjust(-20, 0, 20, 0);
//                    }
//                    QRect tmp = QCommonStyle::subElementRect(element, option, widget);
                }
            }
        }
    }
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void GLDListViewPrivate::updateStyledFrameWidths()
{
    Q_Q(const QFrame);
    QStyleOptionFrameV3 opt;
    opt.initFrom(q);
    opt.lineWidth = lineWidth;
    opt.midLineWidth = midLineWidth;
    opt.frameShape = QFrame::Shape(frameStyle & QFrame::Shape_Mask);

    QRect cr = q->style()->subElementRect(QStyle::SE_ShapedFrameContents, &opt, q);
    leftFrameWidth = cr.left() - opt.rect.left();
    topFrameWidth = cr.top() - opt.rect.top();
    rightFrameWidth = opt.rect.right() - cr.right(),
    bottomFrameWidth = opt.rect.bottom() - cr.bottom();
    frameWidth = qMax(qMax(leftFrameWidth, rightFrameWidth),
                      qMax(topFrameWidth, bottomFrameWidth));
}

innerGLD::innerShellTreeView::innerShellTreeView(QWidget *parent)
    : QTreeView(parent), m_bShowBranches(false), m_pDataModel(NULL), m_pModel(NULL)
{
    setItemsExpandable(false);
    connect(this, SIGNAL(pressed(QModelIndex)), this, SLOT(onItemPressed(QModelIndex)));

    if (GLDListView* list = dynamic_cast<GLDListView *>(parent))
    {
        //技术平台右键菜单关联
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(QPoint)),
                list, SIGNAL(customContextMenuRequested(QPoint)));

        connect(this, SIGNAL(doubleClicked(QModelIndex)),
                list, SIGNAL(itemJustBeDoubleClicked(const QModelIndex &)));

        connect(this, SIGNAL(itemPressed(QModelIndex)), list, SIGNAL(itemJustBeSelected(const QModelIndex &)));
        connect(this, SIGNAL(currentItemJustChanged(QModelIndex,QModelIndex)),
                        list, SIGNAL(currentItemJustChanged(const QModelIndex &, const QModelIndex &)));
    }

    setStyle(new GLDScrollStyle(this));
}

void innerGLD::innerShellTreeView::setModel(QAbstractItemModel *model)
{
    if (m_pDataModel == model)
        return;

    m_pDataModel = model;
    m_pModel = createSortModel();
    m_pModel->setSortLocaleAware(true);
    m_pModel->setSourceModel(m_pDataModel);

    QTreeView::setModel(m_pModel);
    adjustColumnWidth(QModelIndex());

    header()->setSectionsMovable(true);
    //    setHeaderHidden(true);
}

QSortFilterProxyModel *innerGLD::innerShellTreeView::createSortModel()
{
    return new GLDFileSortModel(this);
}

QAbstractItemModel *innerGLD::innerShellTreeView::dataModel()
{
    return m_pDataModel;
}

QModelIndex innerGLD::innerShellTreeView::dataIndex(const QModelIndex &index)
{
    if (m_pModel != NULL)
    {
        return m_pModel->mapToSource(index);
    }
    return QModelIndex();
}

QModelIndex innerGLD::innerShellTreeView::proxyIndex(const QModelIndex &index)
{
    if (m_pModel != NULL)
    {
        return m_pModel->mapFromSource(index);
    }

    return QModelIndex();
}

void innerGLD::innerShellTreeView::adjustColumnWidth(const QModelIndex &)
{
    header()->setSectionsMovable(true);
//    if (-1 != header()->visualIndex(0))
//        header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
//    resizeColumnToContents(0);
}

void innerGLD::innerShellTreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (GLDListView* list = dynamic_cast<GLDListView *>(parent()))
    {
        if (list->m_bShowToolTip)
        {
            if (1 == selected.size())
            {
                if (1 == selected.at(0).indexes().size())
                {
                    QModelIndex  focusIndex = selected.at(0).indexes().at(0);
                    if (focusIndex.isValid())
                    {
                        setToolTip(focusIndex.data().toString());
                    }
                }
            }
        }
    }
    QTreeView::selectionChanged(selected, deselected);
}

bool innerGLD::innerShellTreeView::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
        QPoint ps = helpEvent->pos();
        if (header())
        {
            ps.setY(ps.y() - header()->height());
        }
        QModelIndex index = indexAt(ps);
        if (index.isValid())
        {
            QToolTip::showText(helpEvent->globalPos(), index.data().toString());
        }
        else
        {
            QToolTip::hideText();
            e->ignore();
        }

        return true;
    }
    return inherit::event(e);
}

void innerGLD::innerShellTreeView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
    if (m_bShowBranches)
    {
        QTreeView::drawBranches(painter, rect, index);
    }
}

void innerGLD::innerShellTreeView::mousePressEvent(QMouseEvent *event)
{
    QTreeView::mousePressEvent(event);
    QModelIndex pressedIndex = indexAt(event->pos());
    setCurrentIndex(pressedIndex);
}

void innerGLD::innerShellTreeView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex tmp = indexAt(event->pos());
    if (tmp.isValid())
    {
        if (0 != tmp.column())
        {
            tmp = tmp.sibling(tmp.row(), 0);
            if (!tmp.isValid())
                return;
        }

        bool bcanIntoIndex = true;
        if (this->model())
        {
            if (GLDCustomFileSystemModel *pModel = dynamic_cast<GLDCustomFileSystemModel *>(m_pDataModel))
            {
                if (!pModel->isDir(dataIndex(tmp)))
                    bcanIntoIndex = false;
            }
        }
        emit doubleClicked(dataIndex(tmp));
    }
}

void innerGLD::innerShellTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    emit currentItemJustChanged(dataIndex(current), dataIndex(previous));
    QTreeView::currentChanged(current, previous);
}

void innerGLD::innerShellTreeView::onItemPressed(const QModelIndex &index)
{
    emit itemPressed(dataIndex(index));
}

GLDFileSortModel::GLDFileSortModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_oSizeUnits.insert(c_sKB, c_kb);
    m_oSizeUnits.insert(c_sMB, c_mb);
    m_oSizeUnits.insert(c_sGB, c_gb);
    m_oSizeUnits.insert(c_sTB, c_tb);
}

bool GLDFileSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    GLDCustomFileSystemModel *pCustomFileSystemModel = dynamic_cast<GLDCustomFileSystemModel*>(sourceModel());
    G_ASSERT(pCustomFileSystemModel != NULL);

    QVariant oLeftData = (left.model() ? left.model()->data(left, sortRole()) : QVariant());
    QVariant oRightData = (right.model() ? right.model()->data(right, sortRole()) : QVariant());

    GDateTime oLeftDateTime;
    GDateTime oRightDateTime;

    // 保证在有桌面的情况下，按照升序排序，显示的顺序为 桌面,C: D: E:
    if (oLeftData.type() == QVariant::String)
    {
        GString sLeftData = oLeftData.toString();
        GString sRightData = oRightData.toString();

        // 当字段是日期时间类型时， data里面存储的是字符串， 先把字符串转成日期再排序，当前只支持系统默认日期格式
        oLeftDateTime = QDateTime::fromString(sLeftData, "yyyy/M/d H:m");
        oRightDateTime = QDateTime::fromString(sRightData, "yyyy/M/d H:m");

        if (sLeftData.compare(tr("desktop"), Qt::CaseInsensitive) == 0 &&
                sRightData.compare(tr("desktop"), Qt::CaseInsensitive) == 0)
        {
            return false;
        }
        else if (sLeftData.compare(tr("desktop"), Qt::CaseInsensitive) == 0 &&
                 sRightData.compare(tr("desktop"), Qt::CaseInsensitive) != 0)
        {
            return true;
        }
        else if (sLeftData.compare(QObject::tr("desktop"), Qt::CaseInsensitive) != 0 &&
                 sRightData.compare(QObject::tr("desktop"), Qt::CaseInsensitive) == 0)
        {
            return false;
        }
    }

    bool bLeftIsDir = pCustomFileSystemModel->isDir(left);
    bool bRightIsDir = pCustomFileSystemModel->isDir(right);

    // 先按照节点的类型比较，分为目录和文件，如果都是文件情况下，排序的列为“大小”列，走特性的大小比较逻辑
    if (bLeftIsDir && bRightIsDir)
    {
        if (oLeftDateTime.isValid() || oRightDateTime.isValid())
        {
            if (oLeftDateTime < oRightDateTime)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return QSortFilterProxyModel::lessThan(left, right);
        }
    }

    else if (bLeftIsDir && !bRightIsDir)
        return true;
    else if (!bLeftIsDir && bRightIsDir)
        return false;
    else if (!bLeftIsDir && !bRightIsDir)
    {
        if (left.model()->headerData(left.column(), Qt::Horizontal).toString() == tr("Size"))
        {
            return sizeCompare(oLeftData, oRightData);
        }
        else
        {
            if (oLeftDateTime.isValid() || oRightDateTime.isValid())
            {
                if (oLeftDateTime < oRightDateTime)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return QSortFilterProxyModel::lessThan(left, right);
            }
        }
    }
    else
    {
        return true;
    }
}

bool GLDFileSortModel::sizeCompare(const QVariant &oLeftData, const QVariant &oRightData) const
{
    GString sLeftData = oLeftData.toString().trimmed();
    GString sRightData = oRightData.toString().trimmed();

    if (sLeftData.isEmpty())
        return true;
    else if (sRightData.isEmpty())
        return false;

    GString sLeftSizeUnit = sLeftData.right(c_nSizeUnitLength).toUpper();
    GString sRightSizeUnit = sRightData.right(c_nSizeUnitLength).toUpper();

    double dRelativeLeftSize = sLeftData.left(sLeftData.size() - c_nSizeUnitLength).trimmed().toDouble();
    double dRelativeRightSize = sRightData.left(sRightData.size() - c_nSizeUnitLength).trimmed().toDouble();

    quint64 nAbsoluteLeftSize = dRelativeLeftSize * m_oSizeUnits.value(sLeftSizeUnit);
    quint64 nAbsoluteRightSize = dRelativeRightSize * m_oSizeUnits.value(sRightSizeUnit);

    return nAbsoluteLeftSize < nAbsoluteRightSize;
}
