#include "GLDDocView.h"
#include "GLDPaperWidget.h"
#include "GLDMultiHeaderView.h"
#include "GLDPaperTableView.h"

void GLDPaperWidget::createTableView()
{
    if (m_tableViewFactory != NULL)
    {
        m_tableView = m_tableViewFactory->createTableView(this);
    }
    else
    {
        m_tableView = new GlodonPaperTableView(this);
    }
}

void GLDPaperWidget::initTableView()
{
    m_tableView->setFrameShape(QFrame::NoFrame);
    m_tableView->setUseBlendColor(true);

    m_tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//    m_tableView->setEnterJumpPro(true);
    m_tableView->setFrameStyle(QFrame::NoFrame);
    m_tableView->setAllowSelectCol(false);
    m_tableView->setAllowRangeFilling(true);
}

void GLDPaperWidget::initHeaderView()
{
    m_tableView->verticalHeader()->setHidden(true);

    if (!m_docView->isShowHorizontalHeaderView())
    {
        m_tableView->horizontalHeader()->setVisible(false);
    }
    else if (m_docView->isMutilHeaderViewEnable())
    {
        GlodonMultiHeaderView *mHeaderView = new GlodonMultiHeaderView(Qt::Horizontal);
        mHeaderView->setDrawBorder(true);
        mHeaderView->setObjectName(c_GLDDocView_HHeader);
        m_tableView->setHorizontalHeader(mHeaderView);
    }

    m_tableView->horizontalHeader()->setMovable(false);
    m_tableView->horizontalHeader()->setDragEnabled(false);
    m_tableView->horizontalHeader()->setSectionSpanSeparate(true);
    m_tableView->horizontalHeader()->setResizeMode(m_resizeMode);
    m_tableView->horizontalHeader()->setObjectName(c_GLDDocView_HHeader);
    m_tableView->horizontalHeader()->setDrawBorder(true);
    m_tableView->horizontalHeader()->setAllowDoubleClicked(false);
    m_tableView->horizontalHeader()->setFixedHeight(m_docView->tableViewHorizontalHeaderHeight());

    connect(m_tableView->horizontalHeader(), SIGNAL(sectionResized(int, int, int, bool)),
            this, SLOT(columnResized(int, int, int, bool)));

}

void GLDPaperWidget::initHeaderFooter()
{
    const int headerHeight = m_docView->headerHeight();
    const int footerHeight = m_docView->footerHeight();

    GLDBaseWidgetFactory *factory = m_docView->registeredWidgetFactory();

    // 把页眉页脚控件拆出去

    if (factory != NULL)
    {
        const int pageWidth = m_docView->paperWidth();
        const int leftMargin = m_docView->leftMargin();
        const int rightMargin = m_docView->rightMargin();
        const ZoomFactor zoomFactor = m_docView->factor();

        this->setHeaderWidget(factory->createHeaderWidget(pageWidth,
                              headerHeight,
                              leftMargin,
                              rightMargin,
                              zoomFactor,
                              m_curPageNo,
                              m_pageCount));
        this->setFooterWidget(factory->createFooterWidget(pageWidth,
                              footerHeight,
                              leftMargin,
                              rightMargin,
                              zoomFactor,
                              m_curPageNo,
                              m_pageCount));
    }

    this->setHeaderHeight(headerHeight);
    this->setFooterHeight(footerHeight);
}

void GLDPaperWidget::initPageGeometry()
{
    const int pageWidth = m_docView->paperWidth();
    const int pageHeight = m_docView->paperHeight();

    const int headerHeight = m_docView->headerHeight();
    const int footerHeight = m_docView->footerHeight();

    const int leftMargin = m_docView->leftMargin();
    const int rightMargin = m_docView->rightMargin();

    const ZoomFactor zoomFactor = m_docView->factor();

    const int c_FixedWidth = pageWidth - leftMargin - rightMargin;
    this->setTableViewWidth(c_FixedWidth);

    const int c_FixedHeight = pageHeight - headerHeight - footerHeight;
    this->setTableViewHeight(c_FixedHeight);

    this->setLeftMargin(leftMargin);
    this->setRightMargin(rightMargin);

    this->setFactor(zoomFactor);
}

void GLDPaperWidget::initDelegate()
{
    GlodonDefaultItemDelegateFactory *delegateFactory = m_docView->registeredDelegateFactory();

    if (delegateFactory != NULL)
    {
        GlodonDefaultItemDelegate *delegate = delegateFactory->createDelegate();
        delegate->setParent(m_tableView);

        m_tableView->setItemDelegate(delegate);

        connect(delegate, SIGNAL(onQueryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)),
                m_docView, SLOT(queryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)));
    }
}

void GLDPaperWidget::initColWidths()
{
    for (int i = 0; i < m_docView->columnCount(); ++i)
    {
        int ncolWidth = m_docView->columnWidth(i);

        if (ncolWidth != -1)
        {
            m_tableView->setColumnWidth(i, ncolWidth);
        }
    }

    if (m_docView->hasFitColumn())
    {
        m_tableView->setFitColWidths(m_docView->fitColWidths());
    }
}

void GLDPaperWidget::initRowHeights()
{
    for (int nRow = 0; nRow < m_tableView->model()->rowCount(); ++nRow)
    {
        QVariant variant = m_tableView->model()->index(nRow, 0).data(gidrRowHeightRole);

        if (variant.isValid())
        {
            m_tableView->setRowHeight(nRow, variant.toInt(), false);
        }
    }


    if (m_docView->suitRowHeight())
    {
        GIntList oList;
        oList.push_back(0);
        m_tableView->setSuitRowHeights(oList);
    }
}

void GLDPaperWidget::installEventFilter()
{
    m_tableView->installEventFilter(m_docView);
    m_tableView->viewport()->installEventFilter(m_docView);
}

void GLDPaperWidget::initDataModel()
{
    const int nPageStartRow = m_docView->docInfo().startRowList[m_curPageNo];
    const int nPageRowCount = m_docView->docInfo().rowCountPerPageList[m_curPageNo];

    GLDPaperWidgetModel *paperModel = new GLDPaperWidgetModel(
        m_docView->model(),
        m_curPageNo,
        nPageStartRow,
        nPageRowCount,
        m_tableView
    );

    m_tableView->setModel(paperModel);

    connect(paperModel, SIGNAL(rowHeightChanged(QModelIndex, int)),
            m_docView, SLOT(postUpdatePaperEvent()));
}

void GLDPaperWidget::initTableViewMisc()
{
    m_tableView->setAllowResizeCellByDragGridLine(m_docView->allowToResize());
    m_tableView->setAlternatingRowColors(false);
}

void GLDPaperWidget::initColumnHidden()
{
    if (m_docView->hasColumnHidden())
    {
        QMap<int, bool> &map = m_docView->columnsVisableState();

        QMapIterator<int, bool> i(map);

        while (i.hasNext())
        {
            i.next();

            const int column = i.key();
            const bool visable = i.value();

            if (m_tableView->isColumnHidden(column) != visable)
            {
                m_tableView->setColumnHidden(column, visable);
            }

        }
    }
}

void GLDPaperWidget::configGridLine()
{
    m_tableView->setShowVerticalGridLine(m_docView->isShowVerticalLine());
    m_tableView->setShowHorizontalGridLine(m_docView->isShowHorizontalLine());
    m_tableView->setGridLineWidth(m_docView->gridLineWidth());
    m_tableView->setIsCustomStyle(true);
    m_tableView->setGridLineColor(Qt::black);
    m_tableView->setDrawTopAndLeftBorderLine(m_docView->drawFrameLine());
    m_tableView->setFrameLineColor(Qt::black);
}

void GLDPaperWidget::connectTableViewSignalSlot()
{
    connect(m_tableView, SIGNAL(columnNewWidths(GIntList *)),
            m_docView, SLOT(doNewWidthsToPapers(GIntList *)));

    connect(m_tableView, SIGNAL(enableFitColWidths(bool)),
            m_docView, SLOT(enableFitColWidths(bool)));

    connect(m_tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            m_docView, SLOT(onCurrentFocusChanged(QModelIndex, QModelIndex)));

    connect(m_tableView,
            SIGNAL(onbeforeMoveCurrent(QModelIndex, QModelIndex, QItemSelectionModel::SelectionFlags &, MoveReason, bool &)),
            m_docView,
            SLOT(onbeforeMoveCurrent(QModelIndex, QModelIndex, QItemSelectionModel::SelectionFlags &, MoveReason, bool &)));

    connect(m_tableView, SIGNAL(onCommitEditor(QModelIndex, QVariant, bool &)),
            m_docView, SLOT(commitEditor(QModelIndex, QVariant, bool &)));

    connect(m_tableView, SIGNAL(rangeFill(QModelIndexList, QModelIndexList, int, int, bool &)),
            m_docView, SLOT(onTableViewRangeFill(QModelIndexList, QModelIndexList, int, int, bool &)));

    connect(m_tableView->verticalHeader(), SIGNAL(sectionResized(int, int, int, bool)),
            this, SLOT(rowHeightResized(int, int, int, bool)));
}

GLDPaperWidget::GLDPaperWidget(GLDDocView *docView,
                               int curPageNo,
                               int pageCount,
                               GlodonHeaderView::ResizeMode resizeMode,
                               GLDDocViewTableViewFactory *tableViewFactory,
                               QWidget *parent,
                               Qt::WindowFlags f):
    QWidget(parent , f),
    m_headerWidget(NULL),
    m_factor(NoneFactor),
    m_footerWidget(NULL),
    m_headerHeight(docView->m_nHeaderHeight),
    m_footerHeight(docView->m_nFooterHeight),
    m_leftMargin(docView->m_nLeftMargin),
    m_rightMargin(docView->m_nRightMargin),
    m_docView(docView),
    m_resizeMode(resizeMode),
    m_curPageNo(curPageNo),
    m_pageCount(pageCount),
    m_tableViewFactory(tableViewFactory)
{
    m_dPageWidth = m_docView->m_dInitPageWidth;
    m_dPageHeight = m_docView->m_dInitPageHeight;

    this->setFixedHeight(m_dPageHeight);
    this->setFixedWidth(m_dPageWidth);

    setAutoFillBackground(true);
    setBackgroundRole(QPalette::BrightText);

    createTableView();

    initTableView();
    initHeaderView();
    initDelegate();
    initDataModel();
    initColWidths();
    initRowHeights();
    installEventFilter();
    initTableViewMisc();
    initColumnHidden();

    configGridLine();
    connectTableViewSignalSlot();

    initHeaderFooter();
    initPageGeometry();
}

GLDPaperWidget::~GLDPaperWidget()
{

}

int GLDPaperWidget::curPageNo()
{
    return m_curPageNo;
}

void GLDPaperWidget::rowHeightResized(int row, int oldSize, int newSize, bool isManual)
{
    if (!isManual)
    {
        return;
    }

    GlodonHeaderView *pHeaderView = dynamic_cast<GlodonHeaderView *>(sender());

    if (NULL != pHeaderView)
    {
        GLDPaperWidgetModel *pModel = NULL;

        if ((pModel = dynamic_cast<GLDPaperWidgetModel *>(pHeaderView->model())) != NULL)
        {
            QModelIndex tableViewIndex = pModel->index(row, 0);
            pModel->setData(tableViewIndex, newSize, gidrRowHeightRole);
        }
    }

    G_UNUSED(oldSize);
}

void GLDPaperWidget::columnResized(int column, int oldSize, int newSize, bool isManual)
{
    if (!isManual)
    {
        return;
    }

    GlodonHeaderView *pHeaderView = dynamic_cast<GlodonHeaderView *>(sender());

    if (NULL != pHeaderView)
    {
        GLDPaperWidgetModel *pModel = NULL;

        if ((pModel = dynamic_cast<GLDPaperWidgetModel *>(pHeaderView->model())) != NULL)
        {
            QModelIndex tableViewIndex = pModel->index(column, 0);
            pModel->setData(tableViewIndex, newSize, gidrColWidthRole);
        }
    }

    G_UNUSED(oldSize);
}

ZoomFactor GLDPaperWidget::factor() const
{
    return m_factor;
}

void GLDPaperWidget::setFactor(const ZoomFactor &value)
{
    if (m_factor == value)
    {
        return;
    }

    m_factor = value;
    double dfactor = m_factor;

    if (m_factor == percent_50)
    {
        dfactor = 0.5;
    }

    m_dPageWidth = m_docView->m_dInitPageWidth * dfactor;
    m_dPageHeight = m_docView->m_dInitPageHeight * dfactor;

    m_tableView->zoomTableView(dfactor);

    this->setFixedHeight(m_dPageHeight);
    this->setFixedWidth(m_dPageWidth);

    reLayout();
}

QSize GLDPaperWidget::sizeHint() const
{
    return QWidget::sizeHint();
}

int GLDPaperWidget::headerHeight() const
{
    return m_headerHeight;
}

void GLDPaperWidget::setHeaderHeight(int headerHeight)
{
    m_headerHeight = headerHeight;

    if (m_headerWidget != NULL)
    {
        m_headerWidget->setFixedHeight(headerHeight);
    }
}

int GLDPaperWidget::footerHeight() const
{
    return m_footerHeight;
}

void GLDPaperWidget::setFooterHeight(int footerHeight)
{
    m_footerHeight = footerHeight;

    if (m_footerWidget != NULL)
    {
        m_footerWidget->setFixedHeight(footerHeight);
    }
}

int GLDPaperWidget::tableViewWidth() const
{
    return m_tableViewWidth;
}

void GLDPaperWidget::setTableViewWidth(int tableViewWidth)
{
    m_tableViewWidth = tableViewWidth;
    m_tableView->setFixedWidth(m_tableViewWidth);
}

int GLDPaperWidget::tableViewHeight() const
{
    return m_tableViewHeight;
}

void GLDPaperWidget::setTableViewHeight(int tableViewHeight)
{
    m_tableViewHeight = tableViewHeight;
    m_tableView->setFixedHeight(m_tableViewHeight);
}

void GLDPaperWidget::reLayout()
{
    m_tableView->move(m_leftMargin, m_headerHeight);

    if (m_footerWidget != NULL)
    {
        m_footerWidget->move(0, m_headerHeight + m_tableViewHeight);
        m_footerWidget->setFixedWidth(this->width());
    }

    if (m_headerWidget != NULL)
    {
        m_headerWidget->move(0, 0);
        m_headerWidget->setFixedWidth(this->width());
    }
}

QWidget *GLDPaperWidget::headerWidget() const
{
    return m_headerWidget;
}

void GLDPaperWidget::setHeaderWidget(QWidget *headerWidget)
{
    if (m_headerWidget == headerWidget)
    {
        return;
    }

    freeAndNil(m_headerWidget);
    m_headerWidget = headerWidget;

    if (m_headerWidget != NULL)
    {
        m_headerWidget->setParent(this);
        m_headerWidget->setFixedWidth(this->width());
    }
}

QWidget *GLDPaperWidget::footerWidget() const
{
    return m_footerWidget;
}

void GLDPaperWidget::setFooterWidget(QWidget *footerWidget)
{
    if (m_footerWidget == footerWidget)
    {
        return;
    }

    freeAndNil(m_footerWidget);
    m_footerWidget = footerWidget;

    if (footerWidget != NULL)
    {
        m_footerWidget->setParent(this);
        m_footerWidget->setFixedWidth(this->width());
    }
}

int GLDPaperWidget::rightMargin() const
{
    return m_rightMargin;
}

void GLDPaperWidget::setRightMargin(int rightMargin)
{
    m_rightMargin = rightMargin;
}

int GLDPaperWidget::leftMargin() const
{
    return m_leftMargin;
}

void GLDPaperWidget::setLeftMargin(int leftMargin)
{
    m_leftMargin = leftMargin;
}

void GLDPaperWidget::setPaperWidth(int value)
{
    G_UNUSED(value);

    double dfactor = int(m_factor);

    if (m_factor == percent_50)
    {
        dfactor = 0.5;
    }

    m_dPageWidth = m_docView->m_dInitPageWidth * dfactor;
}

int GLDPaperWidget::paperHeight() const
{
    return m_dPageHeight;
}

void GLDPaperWidget::setPaperHeight(int value)
{
    G_UNUSED(value);

    double dfactor = int(m_factor);

    if (m_factor == percent_50)
    {
        dfactor = 0.5;
    }

    m_dPageHeight = m_docView->m_dInitPageHeight * dfactor;
}

