#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QMessageBox>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QProgressDialog>

#include "GLDPaperWidget.h"
#include "GLDMultiHeaderView.h"
#include "GLDDocView.h"
#include "GLDPaperTableView.h"

const GString c_GLDDocView_HHeader = "GLDDocView_HHeader";

/* GLDDocInfo */
int GLDDocInfo::pageOf(int nValue)
{
    if(nValue < 0)
    {
        return -1;
    }

    const int nPageGeoCount = pageGeometryInfoList.count();

    if(nPageGeoCount == 0)
    {
        return -1;
    }

    if (nValue > pageGeometryInfoList[nPageGeoCount - 1].bottom() + nPageSplitterHeight)
    {
        return -1;
    }

    for (int i = 0; i < nPageGeoCount; ++i)
    {
        const QRect pageGeometryInfo = pageGeometryInfoList.at(i);

        if ((nValue >= pageGeometryInfo.top() - nPageSplitterHeight) && ((nValue < pageGeometryInfo.bottom())))
        {
            return i;
        }
    }

    return nPageGeoCount - 1;
}

GIntList GLDDocInfo::pageOf(int nValue, const QRect &rc)
{
    GIntList pageInRect;

    const int nCurPage = pageOf(nValue);

    if (nCurPage < 0)
    {
        return pageInRect;
    }

    const int visableAreaHScrollPos = rc.top() + nValue;

    QRect nextVisibleAreaRc(0, visableAreaHScrollPos, 20000, rc.height());

    for (int i = (nCurPage - 8 > 0) ? nCurPage - 8 : 0; (i < (nCurPage + 8)); ++i)
    {
        if(i >= pageGeometryInfoList.count())
        {
            break;
        }

        if (nextVisibleAreaRc.intersects(pageGeometryInfoList[i]))
        {
            pageInRect.push_back(i);
        }
    }

    return pageInRect;
}

/* GLDScrollArea */
GLDScrollArea::GLDScrollArea(GLDDocView *parent):
    QScrollArea(parent), m_parent(parent)
{

}

bool GLDScrollArea::event(QEvent *e)
{
    // 不调用基类
    return QAbstractScrollArea::event(e);
}

bool GLDScrollArea::eventFilter(QObject *o, QEvent *e)
{
    // 不调用基类
    return QAbstractScrollArea::eventFilter(o, e);
}

void GLDScrollArea::resizeEvent(QResizeEvent *)
{
    // 不调用基类
}

void GLDScrollArea::wheelEvent(QWheelEvent *event)
{
    QScrollArea::wheelEvent(event);
    m_parent->wheelEvent(event);
}

/* GLDDocView */
void GLDDocView::initScrollArea()
{
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(180, 180, 180));

    m_scrollArea->setAutoFillBackground(true);
    m_scrollArea->setPalette(palette);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setAlignment(Qt::AlignHCenter);
    m_scrollArea->setUpdatesEnabled(true);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->installEventFilter(this);
    m_scrollArea->verticalScrollBar()->setFocusProxy(m_scrollArea);
    m_scrollArea->horizontalScrollBar()->setFocusProxy(m_scrollArea);
    setFocusProxy(m_scrollArea);

    connect(m_scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(verticalScrollBarValueChanged(int)));
    connect(m_scrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(horizontalScrollBarValueChanged(int)));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_scrollArea);
    setLayout(layout);
}

void GLDDocView::initPageGeometry()
{
    m_nInitMaxPageNo = 16;

    m_nInitPageLabelFontSize = 10;

    m_nCurrentDPI = logicalDpiX();

    // 以下计算数字单位为毫米
    m_dInitPageWidth = POINT_NUMBER_FROM_MM(210, m_nCurrentDPI);
    m_dInitPageHeight = POINT_NUMBER_FROM_MM(297, m_nCurrentDPI);

    m_dInitHeaderHeight = POINT_NUMBER_FROM_MM(31.8, m_nCurrentDPI);
    m_dInitFooterHeight = POINT_NUMBER_FROM_MM(31.8, m_nCurrentDPI);

    m_dInitLeftMargin = POINT_NUMBER_FROM_MM(19.1, m_nCurrentDPI);
    m_dInitRightMargin = POINT_NUMBER_FROM_MM(19.1, m_nCurrentDPI);

    m_nInitPageSplitterHeight = POINT_NUMBER_FROM_MM(2, m_nCurrentDPI);
    m_nInitTableViewHHeaderHeight = POINT_NUMBER_FROM_MM(10, m_nCurrentDPI);

    m_nPageWidth = m_dInitPageWidth;
    m_nPageHeight = m_dInitPageHeight;

    m_nHeaderHeight = m_dInitHeaderHeight;
    m_nFooterHeight = m_dInitFooterHeight;

    m_nLeftMargin = m_dInitLeftMargin;
    m_nRightMargin = m_dInitRightMargin;

    m_nPageSplitterHeight = m_nInitPageSplitterHeight;
    m_nTableViewHHeaderHeight = m_nInitTableViewHHeaderHeight;
}



GLDDocView::GLDDocView(QWidget *parent, Qt::WindowFlags f):
    QWidget(parent, f),
    m_allowToResize(false),
    m_allowZoom(false),
    m_isHorizontalMutilHeaderView(false),
    m_isInClearOtherIndex(false),
    m_isInManualControlScrollBar(false),
    m_suitRowHeight(false),
    m_headerAndFooterWidgetFactory(NULL),
    m_tableViewFactory(NULL),
    m_delegateFactory(NULL),
    m_resizeMode(GlodonHeaderView::Fixed),
    m_treeModel(NULL),
    m_nCellHeight(30),
    m_nCurPageNo(0),
    m_nCurrentDPI(0),
    m_nSettedWidthsSum(0),
    m_dataModel(NULL),
    m_model(NULL),
    m_scrollArea(new GLDScrollArea(this)),
    m_zoomFactor(percent_100),
    m_bDrawFrameLine(true),
    m_bShowHHeaderView(true),
    m_bHorzLine(true),
    m_bVertLine(true),
    m_nGridLineWidth(1),
    m_printerOrientation(QPrinter::Portrait)
{
    initPageGeometry();

    setContentsMargins(0, 0, 0, 0);

    initScrollArea();
}

GLDDocView::~GLDDocView()
{
    freeAndNil(m_headerAndFooterWidgetFactory);
    freeAndNil(m_delegateFactory);
    freeAndNil(m_tableViewFactory);
}

GLDPaperWidget *GLDDocView::newPaper(int nPageNo)
{
    return new GLDPaperWidget(this,
                              nPageNo,
                              m_docInfo.nPageCount,
                              m_resizeMode,
                              m_tableViewFactory,
                              m_scrollArea->viewport());
}

GLDPaperWidget *GLDDocView::findPaper(QModelIndex &tableViewIndex)
{
    QAbstractItemModel *model = const_cast<QAbstractItemModel *>(tableViewIndex.model());

    GLDPaperWidgetModel *pModel = NULL;

    if ((pModel = dynamic_cast<GLDPaperWidgetModel *>(model)) != NULL)
    {
        return findPaper(pModel->curPageNo());
    }

    return NULL;
}

void GLDDocView::removePaper(int nPageNo)
{
    int nIndex = indexPaper(nPageNo);

    if (nIndex >= 0)
    {
        GLDPaperWidget *paper = m_papers[nIndex];
        m_papers.removeAt(nIndex);
        freeAndNil(paper);
    }
}

void GLDDocView::clearPagers()
{
    for (int i = 0; i < m_papers.count(); ++i)
    {
        GLDPaperWidget *paper = m_papers[i];
        freeAndNil(paper);
    }

    m_papers.clear();
}

bool GLDDocView::paperVisible(int nPageNo)
{
    for (int i = 0; i < m_visiblePageNoList.count(); ++i)
    {
        if (m_visiblePageNoList[i] == nPageNo)
        {
            return true;
        }
    }

    return false;
}

// 布局显示页面
int GLDDocView::updateLeftLayout(int nHpos, const QRect &rc, GLDPaperWidget *pPaperWidget)
{
    int nPosX = rc.left() + rc.width() / 2 - pPaperWidget->width() - m_nPageSplitterHeight - nHpos;

    if (nPosX < 0)
    {
        nPosX = -nHpos;
        m_scrollArea->horizontalScrollBar()->setRange(0, pPaperWidget->width() * 2 - rc.width());
        m_scrollArea->horizontalScrollBar()->setPageStep(rc.width());
    }
    else
    {
        m_scrollArea->horizontalScrollBar()->setRange(0, 0);
        m_scrollArea->horizontalScrollBar()->setPageStep(rc.width());
    }

    return nPosX;
}

int GLDDocView::updateRightLayout(int nHpos, const QRect &rc, GLDPaperWidget *pPaperWidget)
{
    int nPosX  = rc.left() + rc.width() / 2 - pPaperWidget->width() - m_nPageSplitterHeight - nHpos;

    if (nPosX < 0)
    {
        nPosX = -nHpos + m_nPageSplitterHeight + pPaperWidget->width();
    }
    else
    {
        nPosX = nPosX + m_nPageSplitterHeight + pPaperWidget->width();
    }

    return nPosX;
}

int GLDDocView::updatePageLayout(int nHpos, const QRect &rc, GLDPaperWidget *pPaperWidget)
{
    int nPosX  = rc.left() + rc.width() / 2 - pPaperWidget->width() / 2 - nHpos;

    if (nPosX < 0)
    {
        nPosX = -nHpos;
        m_scrollArea->horizontalScrollBar()->setRange(0, pPaperWidget->width() - rc.width());
        m_scrollArea->horizontalScrollBar()->setPageStep(rc.width());
    }
    else
    {
        m_scrollArea->horizontalScrollBar()->setRange(0, 0);
        m_scrollArea->horizontalScrollBar()->setPageStep(rc.width());
    }

    return nPosX;
}

void GLDDocView::updatePaperLayout(int nVpos, int nHpos, const QRect &rc)
{
    for (int i = 0; i < m_visiblePageNoList.count(); ++i)
    {
        GLDPaperWidget *pPaperWidget = findPaper(m_visiblePageNoList[i]);

        int nPosX = 0;
        int nPosY = 0;

        if (m_zoomFactor == percent_50)
        {
            if (i % 2 == 0)    // 左边
            {
                nPosX = updateLeftLayout(nHpos, rc, pPaperWidget);
            }
            else    // 右边
            {
                nPosX = updateRightLayout(nHpos, rc, pPaperWidget);
            }
        }
        else
        {
            nPosX = updatePageLayout(nHpos, rc, pPaperWidget);
        }

        nPosY = m_docInfo.pageGeometryInfoList[m_visiblePageNoList[i]].top() - nVpos;
        pPaperWidget->move(nPosX, nPosY);

        pPaperWidget->setVisible(true);
    }
}

void GLDDocView::updateFocusWidget()
{
    if (GLDPaperWidget *pPaperWidget = findPaper(m_nCurFocusPageNo))
    {
        GlodonTableView *pTableView = pPaperWidget->tableView();

        if ((NULL != pTableView) && (pTableView->isEditing()))
        {
            GlodonDefaultItemDelegate *pItemDelegate = NULL;

            pItemDelegate = dynamic_cast<GlodonDefaultItemDelegate *>(pTableView->itemDelegate());

            if ((NULL != pItemDelegate) && (NULL != pItemDelegate->curEditor()))
            {
                pItemDelegate->curEditor()->setFocus();
            }
        }
    }
}

void GLDDocView::setColumnHidden(int column, bool hide)
{
    this->columnsVisableMap[column] = hide;

    for (int i = 0; i < m_papers.count(); ++i)
    {
        GlodonTableView *pTableView = m_papers[i]->tableView();
        pTableView->setColumnHidden(column, hide);
    }
}

bool GLDDocView::columnHidden(int column) const
{
    return columnsVisableMap[column];
}

bool GLDDocView::hasColumnHidden() const
{
    return !columnsVisableMap.empty();
}

QMap<int, bool> &GLDDocView::columnsVisableState()
{
    return columnsVisableMap;
}

bool GLDDocView::isPaperCacheFull()
{
    return m_papers.count() + m_visiblePageNoList.count() > m_nInitMaxPageNo;
}





void GLDDocView::clearPaperCache()
{
    for (int i = 0; i < 4; ++i)
    {
        GLDPaperWidget *paper = m_papers.dequeue();
        freeAndNil(paper);
    }
}

void GLDDocView::hidePapersOutOfView()
{
    for (int i = 0; i < m_papers.count(); ++i)
    {
        if (!paperVisible(m_papers[i]->curPageNo()))
        {
            GLDPaperWidget *paper = m_papers[i];

            if (paper)
            {
                paper->move(0, -2000);//->setVisible(false);
            }
        }
    }
}

void GLDDocView::cacheNewPapers()
{
    for (int i = 0; i < m_visiblePageNoList.count(); ++i)
    {
        int nPageNo = m_visiblePageNoList[i];
        int nIndex = indexPaper(nPageNo);

        if (nIndex < 0)
        {
            m_papers.insert(nPageNo, newPaper(nPageNo));;
        }
        else
        {
            // 根据最近显示页面调整位置
            GLDPaperWidget *paper = m_papers[nIndex];
            m_papers.takeAt(nIndex);
            m_papers.append(paper);
        }
    }
}

void GLDDocView::updateVisablePapersList()
{
    const QRect scrollAreaRect = m_scrollArea->rect();
    int verticalScrollBarPos = m_scrollArea->verticalScrollBar()->value();
    m_visiblePageNoList = m_docInfo.pageOf(verticalScrollBarPos, scrollAreaRect);

    //如果当前只有一页，数据刚好占满，插入一行，需要先把新分的页创建出来，再定位焦点，调整滚动条位置
    bool b1 = m_docInfo.nPageCount > m_docFocusInfo.nFocusPageNo;
    bool b2 = m_docFocusInfo.nFocusPageNo > 0;
    bool b3 = m_visiblePageNoList.contains(m_docFocusInfo.nFocusPageNo);
    bool b4 = m_visiblePageNoList.count() + 1 <= m_docInfo.nPageCount;

    if (b1 && b2 && !b3 && b4)
    {
        m_visiblePageNoList.push_back(m_docFocusInfo.nFocusPageNo);
    }
}

void GLDDocView::updateScrollArea()
{
    const int vValue = m_scrollArea->verticalScrollBar()->value();
    const int hValue = m_scrollArea->horizontalScrollBar()->value();
    const QRect scrollAreaRect = m_scrollArea->rect();
    updatePaperLayout(vValue, hValue, scrollAreaRect);

    updateFocusWidget();
}

void GLDDocView::updatePapers()
{
    // 刷新可见页面列表
    updateVisablePapersList();

    // 如果缓存已满，则清空
    if (isPaperCacheFull())
    {
        clearPaperCache();
    }

    // 从可见列表创建新页面并加入缓存
    cacheNewPapers();

    // 隐藏不在视图内的页面
    hidePapersOutOfView();

    // 滚动显示页面
    updateScrollArea();
}

int GLDDocView::rowPosition(QModelIndex dataIndex)
{
    return verticalPosition(index(dataIndex));
}

int GLDDocView::columnPosition(QModelIndex dataIndex)
{
    return horizontalPosition(index(dataIndex));
}

QRect GLDDocView::visualRect(const QModelIndex dataIndex)
{
    QModelIndex tableViewIndex = index(dataIndex);

    int nCurPaperNo = curPageNo(tableViewIndex);

    GLDPaperWidget *curPaperWidget = findPaper(nCurPaperNo, true);

    GlodonTableView *curPaperTableView = curPaperWidget->tableView();

    QRect tableViewRect = curPaperTableView->visualRect(tableViewIndex);

    const int nLeft = tableViewRect.left();
    const int nTop = tableViewRect.top() + curPaperTableView->horizontalHeader()->height();
    const int nWidth = tableViewRect.width();
    const int nHeight = tableViewRect.height();
    QRect tableViewAndHeaderViewRect(nLeft, nTop, nWidth, nHeight);

    QPoint globalTopLeft = curPaperTableView->mapToGlobal(tableViewAndHeaderViewRect.topLeft());

    const int nX = globalTopLeft.x();
    const int nY = globalTopLeft.y();
    const int width = tableViewAndHeaderViewRect.width();
    const int height = tableViewAndHeaderViewRect.height();

    return QRect(nX, nY, width, height);
}

void GLDDocView::scrollTo(QModelIndex dataIndex, ScrollHint hint)
{
    if (!dataIndex.isValid())
    {
        return;
    }

    GLDPaperWidget *pScrollToPage = findPaper(dataIndexPageNo(dataIndex), true);

    if (NULL == pScrollToPage)
    {
        return;
    }

    // todo liurx 竖直方向暂时没有处理
    QModelIndex destTableViewIndex = index(dataIndex);
    int nDestPosition = verticalPosition(destTableViewIndex);

    try
    {
        m_isInManualControlScrollBar = true;
        clearOtherIndex();
        setCurrentIndex(dataIndex);
        m_scrollArea->verticalScrollBar()->setValue(nDestPosition);
        m_isInManualControlScrollBar = false;
    }
    catch (...)
    {
        m_isInManualControlScrollBar = false;
    }

    G_UNUSED(hint)
}

QQueue<GLDPaperWidget *> &GLDDocView::paperWidgetList()
{
    return m_papers;
}

QModelIndexList GLDDocView::selectedIndexes()
{
    GLDPaperWidget *paperWidget = findPaper(m_nCurFocusPageNo);

    if (paperWidget != NULL)
    {
        QModelIndexList modelIndexList = paperWidget->tableView()->selectedIndexes();
        QModelIndexList result;

        for (int i = 0; i < modelIndexList.count(); ++i)
        {
            result.append(dataIndex(m_nCurFocusPageNo, modelIndexList.at(i)));
        }

        return result;
    }

    return QModelIndexList();
}

void GLDDocView::verticalScrollBarValueChanged(int value)
{
    updatePapers();

    // 计算当前页码
    if (m_zoomFactor == percent_50)
    {
        m_nCurPageNo = 2 * qRound((float)value / (m_nPageHeight + m_nPageSplitterHeight));
    }
    else
    {
        m_nCurPageNo = qRound((float)value / (m_nPageHeight + m_nPageSplitterHeight));
    }

    emit currPageChanged(m_nCurPageNo);
}

void GLDDocView::horizontalScrollBarValueChanged(int value)
{
    const int nOffSet = m_scrollArea->verticalScrollBar()->value();
    const QRect scrollAreaRect = m_scrollArea->rect();
    updatePaperLayout(nOffSet, value, scrollAreaRect);

    updateFocusWidget();
}

void GLDDocView::printDocument(QPrinter *printer)
{
    QPainter painter;

    if (!painter.begin(printer))
    {
        return;
    }

    const int nFromPage = printer->fromPage();
    const int nToPage = printer->toPage();

    QProgressDialog progress(this);
    progress.setWindowModality(Qt::ApplicationModal);
    progress.setMinimum(nFromPage - 1);
    progress.setMaximum(nToPage);

    bool firstPage = true;

    for (int page = nFromPage; page <= nToPage; ++page)
    {

        if (!firstPage)
        {
            printer->newPage();
        }

        qApp->processEvents();

        if (progress.wasCanceled())
        {
            break;
        }

        printPage(page - 1, painter, printer);
        progress.setValue(page);
        firstPage = false;
    }

    painter.end();
}

void GLDDocView::printDocument(QPrinter *printer, QPrintDialog *printDialog)
{
    int fromPage = printDialog->fromPage();
    int toPage = printDialog->toPage();

    if (fromPage > toPage)
    {
        return;
    }

    QAbstractPrintDialog::PrintRange printRange = printDialog->printRange();

    switch (printRange)
    {
        case QAbstractPrintDialog::AllPages:
        {
            printer->setFromTo(1, m_docInfo.nPageCount);
            break;
        };

        case QAbstractPrintDialog::PageRange:
        {
            if (fromPage <= 0)
            {
                fromPage = 1;
            }

            if (toPage > m_docInfo.nPageCount)
            {
                toPage = m_docInfo.nPageCount;
            }

            printer->setFromTo(fromPage, toPage);
            break;
        };

        case QAbstractPrintDialog::CurrentPage:
        {
            printer->setFromTo(m_nCurFocusPageNo, m_nCurFocusPageNo);
            break;
        };

        default:
            break;
    }

    printDocument(printer);
}

void GLDDocView::printPage(int index, QPainter &painter, QPrinter *printer)
{
    GLDPaperWidget *paper = newPaper(index);
    QPixmap image = QPixmap::grabWidget(paper, 0, 0, paper->width(), paper->height());
    QRect rect = printer->paperRect();
    painter.drawPixmap(rect, image);
    freeAndNil(paper);
}

void GLDDocView::printPage(int index, QPainter &painter)
{
    GLDPaperWidget *paper = newPaper(index);
    QPixmap image = QPixmap::grabWidget(paper, 0, 0, paper->width(), paper->height());
    QRect rect = painter.viewport();
    QSize size = image.size();
    size.scale(rect.size(), Qt::KeepAspectRatio);     // 此处保证图片显示完整
    painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
    painter.setWindow(image.rect());
    painter.drawPixmap(0, 0, image);
    freeAndNil(paper);
}

void GLDDocView::printALLDocument(QPrinter *printer)
{
    QPainter painter(printer);

    for (int i = 0; i < m_docInfo.nPageCount; ++i)
    {
        if (i > 0)
        {
            printer->newPage();
        }

        printPage(i, painter);
    }
}

void GLDDocView::setModel(QAbstractItemModel *model)
{
    if (model == m_dataModel)
    {
        return;
    }
    else
    {
        freeAndNil(m_dataModel);
        m_dataModel = model;
    }

    freeAndNil(m_treeModel);
    m_treeModel = new GlodonTreeModel(m_dataModel, this);
    m_treeModel->setObjectName("m_treeModel");

    GlodonTreeDrawInfo *pDrawInfo = new GlodonTreeDrawInfo(this);
    pDrawInfo->setModel(m_dataModel);
    m_treeModel->drawInfo = pDrawInfo;

    freeAndNil(m_model);
    m_model = m_treeModel;

    connect(m_dataModel, SIGNAL(modelReset()), this, SLOT(calculatePages()), Qt::QueuedConnection);
    connect(m_dataModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(calculatePages()), Qt::QueuedConnection);
    connect(m_dataModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(calculatePages()), Qt::QueuedConnection);
    connect(m_dataModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(dataChanged(QModelIndex, QModelIndex)));

    if (m_colWidth.count() > m_model->columnCount())
    {
        bool bChanged = false;

        for (int i = m_colWidth.count() - 1; i >= m_model->columnCount(); --i)
        {
            m_colWidth.remove(i);
            bChanged = true;
        }

        if (bChanged)
        {
            m_nSettedWidthsSum = 0;
        }
    }

    //    calculatePages();
}

QAbstractItemModel *GLDDocView::dataModel()
{
    return m_dataModel;
}

void GLDDocView::print()
{
    if (this->pageCount() <= 0)
    {
        return;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOrientation(m_printerOrientation);
    printer.setOutputFormat(QPrinter::NativeFormat);
    printer.setPageMargins(0, 0, 0, 0, QPrinter::Millimeter);

    QPrintDialog printDlg(&printer, this);

    if (printDlg.exec() != QDialog::Accepted)
    {
        return;
    }

    printDocument(&printer, &printDlg);
}

void GLDDocView::printPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOrientation(m_printerOrientation);

    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), this, SLOT(printALLDocument(QPrinter *)));

    if (m_zoomFactor == percent_400)
    {
        preview.setMinimumSize(QSize(1200, 800));
    }
    else
    {
        preview.setMinimumSize(m_papers[0]->size());
    }

    preview.exec();
}

void GLDDocView::exportToPdf()
{
    printPreview();
}

void GLDDocView::exportToPdf(QString fileName)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOrientation(m_printerOrientation);

    printer.setOutputFileName(fileName);
    QPainter painter;

    painter.begin(&printer);

    for (int i = 0; i < m_docInfo.nPageCount; ++i)
    {
        if (i > 0)
        {
            printer.newPage();
        }

        printPage(i, painter);
    }

    painter.end();
}

void GLDDocView::fistPage()
{
    if (m_nCurPageNo == 0)
    {
        return;
    }

    gotoPage(0);
    m_nCurPageNo = 0;
}

void GLDDocView::lastPage()
{
    if (m_nCurPageNo == pageCount() - 1)
    {
        return;
    }

    gotoPage(pageCount() - 1);
    m_nCurPageNo = pageCount() - 1;
}

void GLDDocView::prevPage()
{
    int nGotoPage = m_nCurPageNo;

    if (m_zoomFactor == percent_50)
    {
        if (nGotoPage <= 1)
        {
            return;
        }

        nGotoPage -= 2;
    }
    else
    {
        if (nGotoPage == 0)
        {
            return;
        }

        nGotoPage -= 1;
    }

    gotoPage(nGotoPage);
    m_nCurPageNo = nGotoPage;
}

void GLDDocView::nextPage()
{
    int nGotoPage = m_nCurPageNo;

    if (m_zoomFactor == percent_50)
    {
        if (nGotoPage >= pageCount() - 2)
        {
            return;
        }

        nGotoPage += 2;
    }
    else
    {
        if (nGotoPage == pageCount() - 1)
        {
            return;
        }

        nGotoPage += 1;
    }

    gotoPage(nGotoPage);
    m_nCurPageNo = nGotoPage;
}

void GLDDocView::gotoPage(int value)
{
    if (m_nCurPageNo == value)
    {
        return;
    }

    int nValue = 0;

    if (m_zoomFactor == percent_50)
    {
        nValue = (value) * (m_nPageHeight + m_nPageSplitterHeight) / 2 + m_nPageSplitterHeight;
    }
    else
    {
        nValue = (value) * (m_nPageHeight + m_nPageSplitterHeight) + m_nPageSplitterHeight;
    }

    m_scrollArea->verticalScrollBar()->setValue(nValue);
}

void GLDDocView::calculatePageGeometryInfo()
{
    m_docInfo.nPageSplitterHeight = m_nPageSplitterHeight;
    m_docInfo.pageGeometryInfoList.clear();

    for (int i = 0; i < m_docInfo.nPageCount; ++i)
    {
        int x = 0;
        int y = 0;
        int width = 2000;
        int height = m_nPageHeight + 1;

        if (m_zoomFactor == percent_50)
        {
            y = (m_nPageHeight + m_nPageSplitterHeight) * (i / 2) + m_nPageSplitterHeight;
        }
        else
        {
            y = (m_nPageHeight + m_nPageSplitterHeight) * i + m_nPageSplitterHeight;
        }

        QRect pageRect(x, y, width, height);

        m_docInfo.pageGeometryInfoList.push_back(pageRect);

        if ((m_zoomFactor == percent_50) && (i < m_docInfo.nPageCount - 1))
        {
            m_docInfo.pageGeometryInfoList.push_back(pageRect);
            ++i;
        }
    }
}

int GLDDocView::curPageNo(const QModelIndex& currentIndex) const
{
    QAbstractItemModel *itemModel = const_cast<QAbstractItemModel *>(currentIndex.model());
    GLDPaperWidgetModel *paperModel = dynamic_cast<GLDPaperWidgetModel *>(itemModel);

    if (paperModel != NULL)
    {
        return paperModel->curPageNo();
    }
    else
    {
        return -1;
    }
}

int GLDDocView::dataIndexPageNo(QModelIndex dataIndex)
{
    if (NULL == m_treeModel)
    {
        return -1;
    }

    if (!dataIndex.isValid())
    {
        return -1;
    }

    QModelIndex treeIndex = m_treeModel->treeIndex(dataIndex);

    GIntList startRows = m_docInfo.startRowList;
    int nTreeIndexPaperNo = 0;
    int nStartRow = 0;
    bool bHasDataIndexPageNo = false;

    for (int i = 0; i < startRows.count(); ++i)
    {
        if (startRows.at(i) > treeIndex.row())
        {
            if (0 == i)
            {
                break;
            }

            nTreeIndexPaperNo = i - 1;
            nStartRow = startRows.at(i - 1);
            bHasDataIndexPageNo = true;
            break;
        }
        else if (startRows.at(i) <= treeIndex.row() &&
                 treeIndex.row() < m_docInfo.rowCountPerPageList.at(i) + startRows.at(i))
        {
            nTreeIndexPaperNo = i;
            nStartRow = startRows.at(i);
            bHasDataIndexPageNo = true;
            break;
        }
    }

    if (bHasDataIndexPageNo)
    {
        return nTreeIndexPaperNo;
    }
    else
    {
        return -1;
    }
}

QModelIndex GLDDocView::index(QModelIndex dataIndex)
{
    int nDataIndexPageNo = dataIndexPageNo(dataIndex);

    if (nDataIndexPageNo != -1)
    {
        QModelIndex treeIndex = m_treeModel->treeIndex(dataIndex);
        GLDPaperWidget *paper = findPaper(nDataIndexPageNo);

        if (paper != NULL)
        {
            const int row = treeIndex.row() - m_docInfo.startRowList.at(nDataIndexPageNo);
            return paper->tableView()->model()->index(row, treeIndex.column());
        }
    }

    return QModelIndex();
}

void GLDDocView::updateModelIndex(QModelIndex dataIndex)
{
    QModelIndex tableViewIndex = index(dataIndex);
    int nPageNo = curPageNo(tableViewIndex);

    if (GLDPaperWidget *pPageWidget = findPaper(nPageNo))
    {
        if (GlodonTableView *pTableView = pPageWidget->tableView())
        {
            pTableView->updateModelIndex(tableViewIndex);
        }
    }
}

void GLDDocView::updateCol(QModelIndex dataIndex)
{
    QModelIndex tableViewIndex = index(dataIndex);
    int nPageNo = curPageNo(tableViewIndex);

    if (GLDPaperWidget *pPageWidget = findPaper(nPageNo))
    {
        if (GlodonTableView *pTableView = pPageWidget->tableView())
        {
            pTableView->updateCol(tableViewIndex.column());
        }
    }
}

void GLDDocView::updateRow(QModelIndex dataIndex)
{
    QModelIndex tableViewIndex = index(dataIndex);
    int nPageNo = curPageNo(tableViewIndex);

    if (GLDPaperWidget *pPageWidget = findPaper(nPageNo))
    {
        if (GlodonTableView *pTableView = pPageWidget->tableView())
        {
            pTableView->updateRow(tableViewIndex.row());
        }
    }
}

void GLDDocView::updateAll()
{
    for (int i = 0; i < m_papers.count(); ++i)
    {
        GlodonTableView *pTableView = m_papers[i]->tableView();

        if (pTableView)
        {
            pTableView->updateAll();
        }
    }
}

void GLDDocView::clearOtherIndex()
{
    m_isInClearOtherIndex = true;

    for (int i = 0; i < m_papers.count(); ++i)
    {
        GlodonTableView *pTableView = m_papers[i]->tableView();

        if (pTableView->isEditing())
        {
            pTableView->forceCloseEditor();
        }

        pTableView->clearSelection();
        pTableView->selectionModel()->clearCurrentIndex();
    }

    m_isInClearOtherIndex = false;
}

double GLDDocView::colWidthRate(int col) const
{
    if (col < 0 || col >= m_colWidth.count())
    {
        return 0.0;
    }

    int nSum = 0;

    if (0 == m_nSettedWidthsSum)
    {
        for (int i = 0; i < m_colWidth.count(); ++i)
        {
            nSum += m_colWidth[i];
        }
    }
    else
    {
        nSum = m_nSettedWidthsSum;
    }

    double dRate = m_colWidth[col] / (double)nSum;
    return dRate;
}

int GLDDocView::paperVerticalPosition(const QModelIndex &tableViewIndex)
{
    const int nCurPaperNo = curPageNo(tableViewIndex);

    GLDPaperWidget *curPaperWidget = findPaper(nCurPaperNo, true);
    GlodonTableView *curPaperTableView = curPaperWidget->tableView();

    const int nPageHeaderHeight = curPaperWidget->headerHeight();
    const int nTableViewHeaderHeight = curPaperTableView->horizontalHeader()->height();
    const int nCurPaperHeaderHeight = nPageHeaderHeight + nTableViewHeaderHeight;
    const int nVerticalPosition = curPaperTableView->rowViewportPosition(tableViewIndex.row());
    const int nIndexPositionWithTableView = nVerticalPosition + nCurPaperHeaderHeight;

    return nIndexPositionWithTableView;
}

int GLDDocView::paperHorizontalPosition(const QModelIndex &tableViewIndex)
{
    const int nCurPaperNo = curPageNo(tableViewIndex);

    GLDPaperWidget *curPaperWidget = findPaper(nCurPaperNo, true);
    GlodonTableView *curPaperTableView = curPaperWidget->tableView();

    const int nHorizentalPosition = curPaperTableView->columnViewportPosition(tableViewIndex.column());
    const int nLeftPaperMargin = curPaperWidget->leftMargin();

    return nHorizentalPosition + nLeftPaperMargin;
}

int GLDDocView::verticalPosition(const QModelIndex &tableViewIndex)
{
    int nCurPaperNo = curPageNo(tableViewIndex);
    return paperVerticalPosition(tableViewIndex) + m_docInfo.pageGeometryInfoList.at(nCurPaperNo).top();
}

int GLDDocView::horizontalPosition(const QModelIndex &tableViewIndex)
{
    int nCurPaperNo = curPageNo(tableViewIndex);
    return paperHorizontalPosition(tableViewIndex) + m_docInfo.pageGeometryInfoList.at(nCurPaperNo).left();
}

int GLDDocView::rowHeight(const QModelIndex &tableViewIndex)
{
    int ncurPaperNo = curPageNo(tableViewIndex);
    GLDPaperWidget *curPaperWidget = findPaper(ncurPaperNo, true);
    GlodonTableView *curPaperTableView = curPaperWidget->tableView();
    return curPaperTableView->rowHeight(tableViewIndex.row());
}

void GLDDocView::postUpdatePaperEvent()
{
    QEvent *pEvent = new QEvent((QEvent::Type)GM_DocViewCalculatePage);
    QApplication::postEvent(this, pEvent);
}

QModelIndex GLDDocView::indexAt(QPoint pos)
{
    int nPageNo = m_docInfo.pageOf(pos.y());
    GLDPaperWidget *pPaperWidget = findPaper(nPageNo, true);

    if (NULL == pPaperWidget)
    {
        return QModelIndex();
    }

    GlodonTableView *pTableView = pPaperWidget->tableView();
    QPoint tableViewPos(pPaperWidget->leftMargin() + pTableView->verticalHeader()->width(),
                        pPaperWidget->headerHeight() + pTableView->horizontalHeader()->height());
    QPoint paperWidgetPos = QPoint(m_docInfo.pageGeometryInfoList.at(nPageNo).left(), m_docInfo.pageGeometryInfoList.at(nPageNo).top());
    QPoint posInTableView = pos - tableViewPos - paperWidgetPos;

    if (posInTableView.y() > pTableView->verticalHeader()->length())
    {
        posInTableView.setY(pTableView->verticalHeader()->length() - 1);
    }
    else if (posInTableView.y() < 0)
    {
        posInTableView.setY(0);
    }

    if (posInTableView.x() > pTableView->horizontalHeader()->length())
    {
        posInTableView.setX(pTableView->horizontalHeader()->length() - 1);
    }
    else if (posInTableView.x() < 0)
    {
        posInTableView.setX(0);
    }

    return dataIndex(nPageNo, pTableView->indexAt(posInTableView));
}

void GLDDocView::setMutilHeaderView(bool value, Qt::Orientation orientation)
{
    if (m_bShowHHeaderView && orientation == Qt::Horizontal)
    {
        m_isHorizontalMutilHeaderView = value;
    }
}

bool GLDDocView::isMutilHeaderViewEnable() const
{
    return m_isHorizontalMutilHeaderView;
}

QScrollArea *GLDDocView::scrollArea()
{
    return m_scrollArea;
}

void GLDDocView::recoverFocusState()
{
    if (GLDPaperWidget *pPaperWidget = findPaper(m_docFocusInfo.nFocusPageNo))
    {
        if (GlodonTableView *pTableView = pPaperWidget->tableView())
        {
            QModelIndex curIndex = pTableView->model()->index(m_docFocusInfo.nFocusRow, m_docFocusInfo.nFocusColumn);
            pTableView->setCurrentIndex(curIndex);
            pTableView->setFocus();
        }
    }
}

GLDPaperWidget *GLDDocView::editingPaper()
{
    for (int i = 0; i < m_papers.count(); ++i)
    {
        if (GLDPaperWidget *pPaperWidget = m_papers.at(i))
        {
            if (GlodonTableView *pTableView = pPaperWidget->tableView())
            {
                if (pTableView->isEditing())
                {
                    return pPaperWidget;
                }
            }
        }
    }

    return NULL;
}

void GLDDocView::prepareLoadPages()
{
    calculatePageGeometryInfo();

    clearPagers();

    resizeEvent(NULL);
}

void GLDDocView::calculatePages()
{
    m_docInfo.rowCountPerPageList.clear();
    m_docInfo.startRowList.clear();
    m_docInfo.startRowList.push_back(0);
    m_docInfo.nPageCount = 1;

    calculatePageCount();

    prepareLoadPages();

    if (m_model->rowCount() == 0)
    {
        m_model->insertRows(0, 1);
    }
    else
    {
        recoverFocusState();
    }
}

void GLDDocView::onCurrentFocusChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (!m_isInClearOtherIndex)
    {
        m_nCurFocusPageNo = curPageNo(current);
        m_currentIndex = dataIndex(m_nCurFocusPageNo, current);
        m_docFocusInfo.nFocusPageNo = m_nCurFocusPageNo;
        m_docFocusInfo.nFocusRow = current.row();
        m_docFocusInfo.nFocusColumn = current.column();

        if (GlodonAbstractItemModel *pModel = dynamic_cast<GlodonAbstractItemModel *>(m_model))
        {
            pModel->afterCurrentChanged(m_currentIndex);
        }

        emit currentFocusChanged(dataIndex(curPageNo(previous), previous),
                                 m_currentIndex);

        onAfterCurrentFocusChanged(current, previous);
    }
}

void GLDDocView::onAfterCurrentFocusChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (!previous.isValid() && current.isValid())
    {
        if (GLDPaperWidget *pPaperWidget = findPaper(m_nCurFocusPageNo))
        {
            pPaperWidget->setFocusProxy(pPaperWidget->tableView());
            QWidget *pParentWidget = pPaperWidget->parentWidget();

            while (pParentWidget)
            {
                pParentWidget->setFocusProxy(pPaperWidget);
                pParentWidget = pParentWidget->parentWidget();
            }

            m_scrollArea->setFocusProxy(pPaperWidget);
        }
    }
}

void GLDDocView::onbeforeMoveCurrent(const QModelIndex &oldIndex,
                                     const QModelIndex &newIndex,
                                     QItemSelectionModel::SelectionFlags &command,
                                     MoveReason moveReason, bool &canMove)
{
    if (oldIndex.isValid())
    {
        m_preIndex = dataIndex(oldIndex);
    }

    if (newIndex.isValid())
    {
        emit onbeforeMoveCursor(m_preIndex, dataIndex(newIndex), command, moveReason, canMove);
    }

    if (m_isInManualControlScrollBar)
    {
        return;
    }

    //控制在整个分页表格中，有且只有一页处于焦点状态
    if (!oldIndex.isValid() && newIndex.isValid())
    {
        clearOtherIndex();

        if (moveReason == mrProgram || moveReason == mrKey)
        {
            int nCurPaperNo = curPageNo(newIndex);

            if (nCurPaperNo > m_nCurFocusPageNo)
            {
                m_scrollArea->verticalScrollBar()->setValue(m_docInfo.pageGeometryInfoList.at(nCurPaperNo).top());
            }
            else if (nCurPaperNo < m_nCurFocusPageNo)
            {
                GlodonTableView *pCurTableView = findPaper(nCurPaperNo)->tableView();
                int nPosition = m_docInfo.pageGeometryInfoList.at(nCurPaperNo).bottom() - m_nFooterHeight - m_nPageSplitterHeight
                                - pCurTableView->rowHeight(newIndex.row()) * 2;
                m_scrollArea->verticalScrollBar()->setValue(nPosition);
            }
        }
    }

    if (GLDPaperWidgetModel * pModel
            = dynamic_cast<GLDPaperWidgetModel *>(const_cast<QAbstractItemModel *>(oldIndex.model())))
    {
        if (oldIndex.column() == pModel->columnCount() - 1 && newIndex.column() == 0
                && newIndex.row() < pModel->rowCount() && oldIndex.row() < newIndex.row() && moveReason != mrMouse)
        {
            emit focusToNextRow(dataIndex(pModel->curPageNo(), oldIndex),
                                dataIndex(pModel->curPageNo(), newIndex), canMove);
        }
    }

    if (oldIndex.isValid() && newIndex.isValid())
    {
        //如果已经是处于一页的最后一个格子，那么就跳到下一个格子
        if ((oldIndex.row() == (oldIndex.model()->rowCount() - 1))
                && (oldIndex.column() == (oldIndex.model()->columnCount() - 1))
                && (newIndex.row() == 0) && (newIndex.column() == 0) && (moveReason == mrProgram || moveReason == mrKey))
        {
            canMove = false;
            int nprePaperNo = curPageNo(oldIndex);

            int ncurPaperNo = nprePaperNo + 1;

            if (ncurPaperNo >= m_docInfo.nPageCount)
            {
                m_docFocusInfo.nFocusPageNo = ncurPaperNo;
                m_docFocusInfo.nFocusRow = 0;
                m_docFocusInfo.nFocusColumn = 0;
                emit focusToNextRow(dataIndex(oldIndex), QModelIndex(), canMove);
                canMove = false;
                return;
            }

            m_scrollArea->verticalScrollBar()->setValue(m_docInfo.pageGeometryInfoList.at(ncurPaperNo).top());

            GlodonTableView *pCurTableView = findPaper(ncurPaperNo)->tableView();
            QModelIndex currentIndex = pCurTableView->model()->index(0, 0);
            setCurrentIndex(dataIndex(currentIndex));
            pCurTableView->edit(currentIndex);
        }
        else if ((newIndex.row() < newIndex.model()->rowCount()) && (moveReason == mrKey || moveReason == mrProgram))
        {
            int ncurPaperNo = curPageNo(newIndex);
            int nNewIndexPosition = paperVerticalPosition(newIndex);

            int nOldIndexPosition = paperVerticalPosition(oldIndex);
            int nCurPaperOffset = m_docInfo.pageGeometryInfoList.at(ncurPaperNo).top()
                                  - m_scrollArea->verticalScrollBar()->value();

            if (nOldIndexPosition + nCurPaperOffset < 0)
            {
                m_scrollArea->verticalScrollBar()->setValue(m_docInfo.pageGeometryInfoList.at(ncurPaperNo).top()
                        + nNewIndexPosition);
            }
            else
            {
                if (nNewIndexPosition + nCurPaperOffset + rowHeight(newIndex) > m_scrollArea->height())
                {
                    m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->value()
                            + rowHeight(newIndex));
                }
                else if (nNewIndexPosition + nCurPaperOffset < 0)
                {
                    m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->value()
                            - rowHeight(newIndex));
                }
            }
        }

    }

    G_UNUSED(command);
}

void GLDDocView::doNewWidthsToPapers(GIntList *widths)
{
    const int c_headerCount = widths->count();

    for (int i = 0; i < c_headerCount; ++i)
    {
        setColumnWidthPixel(i, widths->at(i));
    }
}

void GLDDocView::enableFitColWidths(bool enable)
{
    if (enable)
    {
        if (m_fitColWidths.count() > 0)
        {
            GIntList oList;

            for (int i = 0; i < m_fitColWidths.count(); ++i)
            {
                oList.push_back(m_fitColWidths[i]);
            }

            foreach(GLDPaperWidget * paper, m_papers)
            {
                if (paper != NULL)
                {
                    paper->tableView()->setFitColWidths(oList);
                }
            }
        }
    }
    else
    {
        foreach(GLDPaperWidget * paper, m_papers)
        {
            if (paper != NULL)
            {
                GIntList oList;
                paper->tableView()->setFitColWidths(oList);
            }
        }
    }
}

void GLDDocView::queryIndexDataType(const QModelIndex &index, GlodonDefaultItemDelegate::GDataType &dataType)
{
    emit onQueryIndexDataType(dataIndex(index), dataType);
}

void GLDDocView::commitEditor(const QModelIndex &index, QVariant data, bool &commit)
{
    emit onCommitEditor(dataIndex(index), data, commit);
}

void GLDDocView::onTableViewRangeFill(QModelIndexList src,
                                      QModelIndexList dest,
                                      int rowCount,
                                      int columnCount,
                                      bool &handled)
{
    QModelIndexList destModelIndexList;
    QModelIndexList srcModelIndexList;

    for (int i = 0; i < m_papers.count(); ++i)
    {
        GLDPaperWidget *pPaperWidget = m_papers.at(i);

        if (pPaperWidget->curPageNo() == m_nCurFocusPageNo)
        {
            if (NULL != pPaperWidget->tableView())
            {
                foreach(const QModelIndex& tableViewIndex, src)
                {
                    srcModelIndexList.push_back(dataIndex(tableViewIndex));
                }

                foreach(const QModelIndex& tableViewIndex, dest)
                {
                    destModelIndexList.push_back(dataIndex(tableViewIndex));
                }
            }
        }
    }

    emit onRangeFill(srcModelIndexList, destModelIndexList, rowCount, columnCount, handled);
}

void GLDDocView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    updateModelIndex(topLeft);
    updateModelIndex(bottomRight);
}

void GLDDocView::registerTableViewFactory(GLDDocViewTableViewFactory *tableViewFactory)
{
    m_tableViewFactory = tableViewFactory;
}

void GLDDocView::setAllowToResize(bool value)
{
    m_allowToResize = value;
}

bool GLDDocView::allowToResize() const
{
    return m_allowToResize;
}

QModelIndex GLDDocView::editingIndex()
{
    if (GLDPaperWidget *pPaperWidget = editingPaper())
    {
        return dataIndex(pPaperWidget->tableView()->currentIndex());
    }

    return QModelIndex();
}

void GLDDocView::forceCloseEditor()
{
    if (GLDPaperWidget *pPaperWidget = editingPaper())
    {
        pPaperWidget->tableView()->forceCloseEditor();
    }
}

void GLDDocView::setShowHorizontalHeaderView(bool value)
{
    m_bShowHHeaderView = value;

    if (!value)
    {
        setTableViewHorizontalHeaderHeight(0);
    }
}

bool GLDDocView::isShowHorizontalHeaderView()
{
    return m_bShowHHeaderView;
}

void GLDDocView::setDrawFrameLine(bool value)
{
    m_bDrawFrameLine = value;
}

bool GLDDocView::drawFrameLine()
{
    return m_bDrawFrameLine;
}

void GLDDocView::setShowVerticalLine(bool value)
{
    m_bVertLine = value;
}

bool GLDDocView::isShowVerticalLine()
{
    return m_bVertLine;
}

void GLDDocView::setShowHorizontalLine(bool value)
{
    m_bHorzLine = value;
}

bool GLDDocView::isShowHorizontalLine()
{
    return m_bHorzLine;
}

void GLDDocView::setGridLineWidth(int value)
{
    m_nGridLineWidth = value;
}

int GLDDocView::gridLineWidth()
{
    return m_nGridLineWidth;
}

void GLDDocView::setPrintOrientation(QPrinter::Orientation oritation)
{
    m_printerOrientation = oritation;
}

QPrinter::Orientation GLDDocView::printOrientation()
{
    return m_printerOrientation;
}

bool GLDDocView::suitRowHeight() const
{
    return m_suitRowHeight;
}

void GLDDocView::setSuitRowHeight(bool suitRowHeight)
{
    m_suitRowHeight = suitRowHeight;
}

int GLDDocView::cellHeight() const
{
    return m_nCellHeight;
}

void GLDDocView::setCellHeight(int cellHeight)
{
    m_nCellHeight = cellHeight;
}

bool GLDDocView::allowZoom() const
{
    return m_allowZoom;
}

void GLDDocView::setAllowZoom(bool bAllowZoom)
{
    m_allowZoom = bAllowZoom;
}

GlodonHeaderView::ResizeMode GLDDocView::resizeMode() const
{
    return m_resizeMode;
}

void GLDDocView::setResizeMode(const GlodonHeaderView::ResizeMode &resizeMode)
{
    m_resizeMode = resizeMode;
}

GIntList GLDDocView::fitColWidths() const
{
    return m_fitColWidths;
}

void GLDDocView::setFitColWidths(const GIntList &fitColWidths)
{
    m_fitColWidths = fitColWidths;
}

bool GLDDocView::hasFitColumn() const
{
    return !m_fitColWidths.empty();
}

void GLDDocView::setCurrentIndex(QModelIndex dataIndex)
{
    QModelIndex tableViewIndex = index(dataIndex);
    GLDPaperWidget *pPaperWidget = findPaper(tableViewIndex);

    if (NULL != pPaperWidget)
    {
        GlodonTableView *pTableView = pPaperWidget->tableView();
        pTableView->setCurrentIndex(tableViewIndex);

        if (!(pTableView->hasFocus()))
        {
            pTableView->setFocus();
        }
    }
}

void GLDDocView::setIndexEdit(QModelIndex dataIndex)
{
    if (dataIndex.isValid() == false)
    {
        return;
    }

    QModelIndex tableViewIndex = index(dataIndex);
    GLDPaperWidget *pPaperWidget = findPaper(tableViewIndex);

    if (NULL != pPaperWidget)
    {
        GlodonTableView *pTableView = pPaperWidget->tableView();

        if (tableViewIndex != pTableView->currentIndex())
        {
            pTableView->setCurrentIndex(tableViewIndex);
        }

        pPaperWidget->tableView()->edit(tableViewIndex);
    }
}

void GLDDocView::reLayoutPaperWidget()
{
    for (int i = 0; i < m_papers.count(); ++i)
    {
        GLDPaperWidget *pPaperWidget = m_papers[i];
        pPaperWidget->setPaperWidth(m_dInitPageWidth);
        pPaperWidget->setPaperHeight(m_dInitPageHeight);
        pPaperWidget->setHeaderHeight(m_nHeaderHeight);
        pPaperWidget->setFooterHeight(m_nFooterHeight);
        pPaperWidget->setTableViewHeight(m_nPageHeight - m_nHeaderHeight - m_nFooterHeight);
        pPaperWidget->setLeftMargin(m_nLeftMargin);
        pPaperWidget->setRightMargin(m_nRightMargin);
        pPaperWidget->setTableViewWidth(m_nPageWidth - m_nLeftMargin - m_nRightMargin);
        pPaperWidget->tableView()->horizontalHeader()->setFixedHeight(m_nTableViewHHeaderHeight);
        pPaperWidget->setFactor(m_zoomFactor);
        pPaperWidget->reLayout();
    }
}

int GLDDocView::paperWidth() const
{
    return m_nPageWidth;
}

void GLDDocView::setInitPageWidth(int value)
{
    m_dInitPageWidth = value;
}

void GLDDocView::calculatePageWidth()
{
    double dZoomFactor = zoomFactorAsDouble();

    m_nPageWidth = m_dInitPageWidth * dZoomFactor;
}

void GLDDocView::setPaperWidth(int value)
{
    setInitPageWidth(value);
    calculatePageWidth();
}

int GLDDocView::paperHeight() const
{
    return m_nPageHeight;
}

void GLDDocView::setInitPageHeight(int value)
{
    m_dInitPageHeight = value;
}

void GLDDocView::calculatePageHeight()
{
    double dZoomFactor = zoomFactorAsDouble();

    m_nPageHeight = m_dInitPageHeight * dZoomFactor;
}

void GLDDocView::setPaperHeight(int value)
{
    setInitPageHeight(value);
    calculatePageHeight();
}

void GLDDocView::setBackgroundPalette(QPalette palette)
{
    m_scrollArea->setPalette(palette);
}

int GLDDocView::rightMargin() const
{
    return m_nRightMargin;
}

void GLDDocView::setRightMargin(int rightMargin)
{
    m_dInitRightMargin = POINT_NUMBER_FROM_MM(rightMargin, m_nCurrentDPI);
    m_nRightMargin = m_dInitRightMargin;
    reLayoutPaperWidget();
}

QModelIndex GLDDocView::dataIndex(int pageNo, const QModelIndex &index)
{
    if (pageNo >= 0 && pageNo < m_docInfo.startRowList.count())
    {
        int ndataRow = m_docInfo.startRowList[pageNo] + index.row();

        if (GlodonTreeModel *pTreeModel = dynamic_cast<GlodonTreeModel *>(m_model))
        {
            return pTreeModel->dataIndex(ndataRow, index.column());
        }
        else
        {
            return m_dataModel->index(ndataRow, index.column(), index.parent());
        }
    }
    else
    {
        return QModelIndex();
    }
}

void GLDDocView::registerWidgetFactory(GLDBaseWidgetFactory *factory)
{
    if (m_headerAndFooterWidgetFactory == factory)
    {
        return;
    }

    freeAndNil(m_headerAndFooterWidgetFactory);
    m_headerAndFooterWidgetFactory = factory;
}

GLDBaseWidgetFactory *GLDDocView::registeredWidgetFactory() const
{
    return m_headerAndFooterWidgetFactory;
}

void GLDDocView::registerDelegateFactory(GlodonDefaultItemDelegateFactory *factory)
{
    if (m_delegateFactory == factory)
    {
        return;
    }

    freeAndNil(m_delegateFactory);
    m_delegateFactory = factory;
}

GlodonDefaultItemDelegateFactory *GLDDocView::registeredDelegateFactory() const
{
    return m_delegateFactory;
}

int GLDDocView::curPageNo()
{
    return m_nCurPageNo;    // 从0开始
}

int GLDDocView::curFocusPageNo()
{
    return m_nCurFocusPageNo;    // 从0开始
}

bool GLDDocView::removePageAt(int nPage)
{
    if ((nPage < 0) || (nPage >= m_docInfo.nPageCount))
    {
        return false;
    }

    m_model->removeRows(m_docInfo.startRowList[nPage], m_docInfo.rowCountPerPageList[nPage]);
    return true;
}

bool GLDDocView::removeRows(int row, int count, const QModelIndex &parent)
{
    if (dynamic_cast<QStandardItemModel *>(m_model))
    {
        return m_model->removeRows(row, count, parent);
    }

    return false;
}

bool GLDDocView::insertRows(int row, int count, const QModelIndex &parent)
{
    if (dynamic_cast<QStandardItemModel *>(m_dataModel))
    {
        return m_dataModel->insertRows(row, count, parent);
    }

    return false;
}

bool GLDDocView::appendRows(int count, const QModelIndex &parent)
{
    QStandardItemModel *standardItemModel = dynamic_cast<QStandardItemModel *>(m_dataModel);

    if (standardItemModel != NULL)
    {
        for (int i = 0; i < count; ++i)
        {
            standardItemModel->appendRow(new QStandardItem(""));
            return true;
        }
    }
    else
    {
        return m_dataModel->insertRows(m_dataModel->rowCount(), count, parent);
    }

    return false;
}

void GLDDocView::setColumnWidth(int column, int size)
{
    size = POINT_NUMBER_FROM_MM(size, m_nCurrentDPI);
    setColumnWidthPixel(column, size);
}

void GLDDocView::setColumnWidthPixel(int column, int size)
{
    for (int i = m_colWidth.count(); i <= column; ++i)
    {
        m_colWidth.push_back(-1);
        m_nSettedWidthsSum = 0;
    }

    if ((column >= 0) && (column < m_colWidth.count()) && (m_colWidth.at(column) != size))
    {
        m_colWidth.replace(column, size);
        m_nSettedWidthsSum = 0;

        setPaperColumnWidth(column, size);
    }
}

void GLDDocView::setPaperColumnWidth(int column, int size)
{
    for (int i = 0; i < m_papers.count(); ++i)
    {
        GlodonTableView *pTableView = m_papers[i]->tableView();
        pTableView->setColumnWidth(column, size);
    }
}

int GLDDocView::columnWidth(int column)
{
    if ((column >= 0) && (column < m_colWidth.count()))
    {
        return m_colWidth.at(column);
    }
    else
    {
        return -1;
    }
}

GLDDocInfo &GLDDocView::docInfo()
{
    return m_docInfo;
}

QAbstractItemModel *GLDDocView::model() const
{
    return m_model;
}

int GLDDocView::columnCount() const
{
    return m_colWidth.count();
}

int GLDDocView::leftMargin() const
{
    return m_nLeftMargin;
}

void GLDDocView::setLeftMargin(int leftMargin)
{
    m_dInitLeftMargin = POINT_NUMBER_FROM_MM(leftMargin, m_nCurrentDPI);
    m_nLeftMargin = m_dInitLeftMargin;

    reLayoutPaperWidget();
}

int GLDDocView::tableViewHorizontalHeaderHeight() const
{
    return m_nTableViewHHeaderHeight;
}

void GLDDocView::setTableViewHorizontalHeaderHeight(int tableViewHHeaderHeight)
{
    m_nInitTableViewHHeaderHeight = POINT_NUMBER_FROM_MM(tableViewHHeaderHeight, m_nCurrentDPI);
    m_nTableViewHHeaderHeight = m_nInitTableViewHHeaderHeight;
}

#ifndef QT_NO_WHEELEVENT
void GLDDocView::wheelEvent(QWheelEvent *event)
{
    Qt::KeyboardModifiers modifier = event->modifiers();

    if (m_allowZoom && (Qt::CTRL == (modifier & Qt::CTRL)))
    {
        int nfactor = m_zoomFactor;

        // 缩小
        if (event->delta() < 0)
        {
            if (m_zoomFactor <= percent_100)
            {
                nfactor = 0;
            }
            else
            {
                nfactor = m_zoomFactor / 2;
            }
        }
        // 放大
        else if (event->delta() > 0)
        {
            if (m_zoomFactor == percent_50)
            {
                nfactor = percent_100;
            }
            else if (m_zoomFactor == percent_100)
            {
                nfactor = percent_200;
            }
            else if (m_zoomFactor == percent_200)
            {
                nfactor = percent_400;
            }
        }

        setFactor(ZoomFactor(nfactor));
        event->accept();
    }
    else
    {
        QWidget::wheelEvent(event);
    }
}

#endif // QT_NO_WHEELEVENT

bool GLDDocView::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::InputMethod)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        return keyPressEventFilter(object, keyEvent);
    }
    else if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick)
    {
        QMouseEvent *pMouseEvent = static_cast<QMouseEvent *>(event);
        return mousePressEventFilter(object, pMouseEvent);
    }

    Q_UNUSED(event);
    return false;
}

bool GLDDocView::mousePressEventFilter(QObject *object, QMouseEvent *pMouseEvent)
{
    GlodonTableView *pTableView = NULL;

    if ((pTableView = dynamic_cast<GlodonTableView *>(QApplication::focusWidget())) != NULL)
    {
        QWidget *pScrollArea = dynamic_cast<QWidget *>(object);

        QPoint posInTableView = pMouseEvent->pos();
        QPoint globalPos = pScrollArea->mapToGlobal(pMouseEvent->pos());

        QModelIndex tableViewIndex = pTableView->indexAt(posInTableView);

        bool bHandled = false;

        if (pMouseEvent->type() == QEvent::MouseButtonPress)
        {
            emit onMousePress(dataIndex(tableViewIndex), globalPos, pMouseEvent, bHandled);
        }
        else if (pMouseEvent->type() == QEvent::MouseButtonDblClick)
        {
            emit onMouseButtonDblClick(dataIndex(tableViewIndex), globalPos, pMouseEvent, bHandled);
        }

        if (bHandled)
        {
            pMouseEvent->accept();
            return true;
        }
    }

    Q_UNUSED(pMouseEvent);
    return false;
}

bool GLDDocView::onKeyUp(GlodonTableView *tableView, QKeyEvent *keyEvent)
{
    QModelIndex curIndex = tableView->currentIndex();

    if (curIndex.row() == 0)
    {
        int ncurPageNo = curPageNo(curIndex);

        // todo 如果当前页不是第一页，就会跳到上一页。否则应该跳到最后一页？
        if (ncurPageNo > 0)
        {
            GlodonTableView *preTableView = findPaper(ncurPageNo - 1, true)->tableView();
            QModelIndex preIndex =
                    preTableView->model()->index(preTableView->model()->rowCount() - 1, curIndex.column());
            m_currentIndex = dataIndex(preIndex);
            preTableView->setCurrentIndex(preIndex);

            if (!preTableView->hasFocus())
            {
                preTableView->setFocus();
            }

            keyEvent->accept();

            return true;
        }
    }

    return false;
}

bool GLDDocView::onKeyDown(GlodonTableView *tableView, QKeyEvent *keyEvent)
{
    QModelIndex curIndex = tableView->currentIndex();

    if (curIndex.isValid() && curIndex.row() == curIndex.model()->rowCount() - 1)
    {
        int ncurPageNo = curPageNo(curIndex);

        // todo 如果当前的也不是最后一页，就会跳到下一页。否则应该跳到第一页？
        if (ncurPageNo < m_docInfo.nPageCount - 1)
        {
            GLDPaperWidget *paper = findPaper(ncurPageNo + 1, true);
            GlodonTableView *nextTableView = paper->tableView();

            QModelIndex nextIndex = nextTableView->model()->index(0, curIndex.column());

            m_currentIndex = dataIndex(nextIndex);
            nextTableView->setCurrentIndex(nextIndex);

            if (!nextTableView->hasFocus())
            {
                nextTableView->setFocus();
            }

            keyEvent->accept();

            return true;
        }
    }

    return false;
}

bool GLDDocView::onKeyPress(GlodonTableView *tableView, QKeyEvent *keyEvent)
{
    switch (keyEvent->key())
    {
        case Qt::Key_Down:
        {
            return onKeyDown(tableView, keyEvent);
        }
            break;

        case Qt::Key_Up:
        {
            return onKeyUp(tableView, keyEvent);
        }
            break;

        case Qt::Key_PageDown:
        {
            return onKeyPageDown(tableView, keyEvent);
        }
            break;

        case Qt::Key_PageUp:
        {
            return onKeyPageUp(tableView, keyEvent);
        }
            break;
    }

    return false;
}

bool GLDDocView::onKeyPageUp(GlodonTableView *tableView, QKeyEvent *keyEvent)
{
    QModelIndex curIndex = tableView->currentIndex();

    int nextPosition = m_nPageSplitterHeight + m_scrollArea->verticalScrollBar()->value()
            - m_scrollArea->height();

    if (nextPosition >= m_docInfo.pageGeometryInfoList.at(0).top())
    {
        scrollTo(indexAt(QPoint(horizontalPosition(curIndex), nextPosition)));
    }
    else
    {
        m_scrollArea->verticalScrollBar()->setValue(0);
        tableView->setCurrentIndex(tableView->model()->index(0, curIndex.column()));
    }

    keyEvent->accept();

    return true;
}

bool GLDDocView::onKeyPageDown(GlodonTableView *tableView, QKeyEvent *keyEvent)
{
    QModelIndex curIndex = tableView->currentIndex();

    int nextPosition = m_scrollArea->verticalScrollBar()->value() + m_scrollArea->height();

    if (m_scrollArea->horizontalScrollBar()->isVisible())
    {
        nextPosition -= m_scrollArea->horizontalScrollBar()->height();
    }

    if (nextPosition <= m_docInfo.pageGeometryInfoList.at(m_docInfo.nPageCount - 1).bottom())
    {
        scrollTo(indexAt(QPoint(horizontalPosition(curIndex), nextPosition)));
    }
    else
    {
        m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->maximumHeight());

        const int row = tableView->model()->rowCount() - 1;
        const int col = curIndex.column();
        QModelIndex nextIndex = tableView->model()->index(row, col);

        tableView->setCurrentIndex(nextIndex);
    }

    keyEvent->accept();

    return true;
}

bool GLDDocView::keyPressEventFilter(QObject *object, QKeyEvent *keyEvent)
{
    bool bHandle = false;

    emit onKeyPress(m_currentIndex, *keyEvent, bHandle);

    if (bHandle)
    {
        keyEvent->accept();
        return true;
    }

    GlodonTableView *currentTableView = dynamic_cast<GlodonTableView *>(object);

    if (NULL == currentTableView)
    {
        return false;
    }

    return onKeyPress(currentTableView, keyEvent);

}

int GLDDocView::headerHeight() const
{
    return m_nHeaderHeight;
}

void GLDDocView::setHeaderHeight(int headerHeight)
{
    m_dInitHeaderHeight = POINT_NUMBER_FROM_MM(headerHeight, m_nCurrentDPI);
    m_nHeaderHeight = m_dInitHeaderHeight;

    reLayoutPaperWidget();
}

int GLDDocView::footerHeight() const
{
    return m_nFooterHeight;
}

void GLDDocView::setFooterHeight(int footerHeight)
{
    m_dInitFooterHeight = POINT_NUMBER_FROM_MM(footerHeight, m_nCurrentDPI);;
    m_nFooterHeight = m_dInitFooterHeight;

    reLayoutPaperWidget();
}

ZoomFactor GLDDocView::factor() const
{
    return m_zoomFactor;
}

void GLDDocView::setFactor(const ZoomFactor &value)
{
    if (m_zoomFactor == value)
    {
        return;
    }

    m_zoomFactor = value;

    double dZoomFactor = zoomFactorAsDouble();

    m_nPageWidth = m_dInitPageWidth * dZoomFactor;
    m_nPageHeight = m_dInitPageHeight * dZoomFactor;
    m_nHeaderHeight = m_dInitHeaderHeight * dZoomFactor;
    m_nFooterHeight = m_dInitFooterHeight * dZoomFactor;
    m_nLeftMargin = m_dInitLeftMargin * dZoomFactor;
    m_nRightMargin = m_dInitRightMargin * dZoomFactor;

    m_nTableViewHHeaderHeight = m_nInitTableViewHHeaderHeight * dZoomFactor;
    m_nPageSplitterHeight = m_nInitPageSplitterHeight * dZoomFactor;

    m_scrollArea->horizontalScrollBar()->setValue(0);

    prepareLoadPages();
}

QSize GLDDocView::sizeHint() const
{
    return QWidget::sizeHint();
}

void GLDDocView::resizeEvent(QResizeEvent *)
{
    int nPageRemainedHeight = 0;

    QRect scrollAreaRect = m_scrollArea->rect();

    if (m_zoomFactor == percent_50)
    {
        const int nCurPageNo = int(m_docInfo.nPageCount + 1) / 2;
        const int nPageContentHeight = (m_nPageHeight + m_nPageSplitterHeight) * (nCurPageNo) + m_nPageSplitterHeight;
        nPageRemainedHeight = nPageContentHeight - scrollAreaRect.height();
    }
    else
    {
        const int nPageContentHeight = (m_nPageHeight + m_nPageSplitterHeight) * (m_docInfo.nPageCount) + m_nPageSplitterHeight;
        nPageRemainedHeight =  nPageContentHeight - scrollAreaRect.height();
    }

    m_scrollArea->verticalScrollBar()->setRange(0, nPageRemainedHeight);
    m_scrollArea->verticalScrollBar()->setPageStep(scrollAreaRect.height());

    verticalScrollBarValueChanged(m_scrollArea->verticalScrollBar()->value());
}

void GLDDocView::customEvent(QEvent *event)
{
    if (event->type() == (int)GM_DocViewCalculatePage)
    {
        calculatePages();
    }
}

bool GLDDocView::hasPager(int nPageNo)
{
    return indexPaper(nPageNo) != -1;
}

int GLDDocView::indexPaper(int nPageNo)
{
    for (int i = 0; i < m_papers.count(); ++i)
    {
        if (nPageNo == m_papers[i]->curPageNo())
        {
            return i;
        }
    }

    return -1;
}

GLDPaperWidget *GLDDocView::findPaper(int nPageNo, bool createIfNotFound)
{
    for (int i = 0; i < m_papers.count(); ++i)
    {
        if (nPageNo == m_papers[i]->curPageNo())
        {
            return m_papers[i];
        }
    }

    if (createIfNotFound)
    {
        if (nPageNo >= 0 && nPageNo < m_docInfo.nPageCount)
        {
            GLDPaperWidget *pPaperWidget = newPaper(nPageNo);
            m_papers.insert(nPageNo, pPaperWidget);
            return pPaperWidget;
        }
    }

    return NULL;
}

int GLDDocView::pageCount() const
{
    return m_docInfo.nPageCount;
}

struct PageCountCalculater
{

};

double GLDDocView::zoomFactorAsDouble() const
{
    double dZoomFactor = 0.5;

    if (m_zoomFactor != percent_50)
    {
        dZoomFactor = m_zoomFactor;
    }

    return dZoomFactor;
}

void GLDDocView::calculatePageCount()
{
    int nModelRowCount = m_model->rowCount();

    if(nModelRowCount == 0)
    {
        m_docInfo.rowCountPerPageList.push_back(28);

        return;
    }

    int nPageHeight = 0;
    int nRowCountPerPage = 0;

    double dZoomFactor = zoomFactorAsDouble();

    const int nCellHeight = m_nCellHeight * dZoomFactor;

    int nFirstRowIndex = 0;
    int nNextRowIndex = 0;

    int nTableViewHeight = 0;
    int nRemainHeight = 0;

    while (nFirstRowIndex < nModelRowCount || nRowCountPerPage != 0)
    {
        QVariant firstRowHeight;
        QVariant nextRowHeight;

        if (nFirstRowIndex < nModelRowCount)
        {
            firstRowHeight = m_model->index(nFirstRowIndex, 0).data(gidrRowHeightRole);
        }

        if (nFirstRowIndex + 1 < nModelRowCount)
        {
            nextRowHeight = m_model->index(nFirstRowIndex + 1, 0).data(gidrRowHeightRole);
        }

        // 以下都加格线的宽度的原因是现在tableView的格线不再占用格子的宽度

        if (firstRowHeight.isValid())
        {
            nPageHeight += firstRowHeight.toInt();
        }
        else
        {
            nPageHeight += nCellHeight; // ?
        }

        nPageHeight += m_nGridLineWidth;

        if (nextRowHeight.isValid())
        {
            nextRowHeight = nextRowHeight.toInt();
        }
        else
        {
            nextRowHeight = nCellHeight;
        }

        nextRowHeight.setValue(nextRowHeight.toInt() + m_nGridLineWidth);

        nRowCountPerPage++;

        nTableViewHeight = m_nPageHeight - m_nHeaderHeight - m_nFooterHeight;
        nRemainHeight = nTableViewHeight - m_nTableViewHHeaderHeight - nPageHeight;

        if (nRemainHeight < nextRowHeight.toInt())
        {
            m_docInfo.rowCountPerPageList.push_back(nRowCountPerPage);

            nPageHeight = 0;
            nRowCountPerPage = 0;

            if (nFirstRowIndex + 1 < nModelRowCount)
            {
                m_docInfo.startRowList.push_back(nFirstRowIndex + 1);
                m_docInfo.nPageCount++;
            }
        }
        else
        {
            if (nFirstRowIndex + 1 >= nModelRowCount)
            {
                nNextRowIndex++;
            }
        }

        nFirstRowIndex++;
    }

    if (nFirstRowIndex >= m_model->rowCount() + 1)
    {
        if (nNextRowIndex != 0)
        {
            m_model->insertRows(m_model->rowCount(), nNextRowIndex);
            //  return;
        }
    }
}

/* CBaseWidgetFactory */
GLDBaseWidgetFactory::~GLDBaseWidgetFactory()
{

}

/* GLDDocViewTableViewFactory */
GLDDocViewTableViewFactory::~GLDDocViewTableViewFactory()
{

}
