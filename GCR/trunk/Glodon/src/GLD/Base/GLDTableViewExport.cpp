#include <QMultiHash>
#include <QPaintDevice>
#include "GLDDir.h"
#include "GLDPaperWidget.h"
#include "GLDTableViewExport.h"
#include "GLDGlobal.h"
#include "GLDFile.h"
#include "GLDXLS.h"
#include "GLDStrUtils.h"
#include "GLDUITypes.h"
#include "GLDAbstractItemModel.h"
#include "GLDMultiHeaderView.h"
#include "GLDUIUtils.h"
#include "GLDException.h"
#include "GLDFooterTableView.h"
#include "GLDDrawSymbol.h"
#include "GLDPaperTableView.h"

// 微软官方说：一般取的时标准字体宽度值 7.55，但是对比Excel得到的比例大概是7.2
const int c_nPixToExcelUnit = 7.2;

class GlodonTableViewToExcelPrivate
{
public:
  GlodonTableViewToExcelPrivate(GlodonTableViewToExcel *parent)
    : q_ptr(parent), m_sheet(NULL), m_book(NULL), m_format(NULL), m_titleFormat(NULL),
      m_tableView(NULL), m_horzRowCount(0),
      m_vertColCount(0), m_exportIcon(true),
      m_nRowCount(0), m_nStartRow(0),
      m_bIsToSingleSheet(false), m_bIsOverWrite(true),
      m_showPrompt(true), m_groupCollapsed(false),
      m_borderStyle(BORDERSTYLE_THIN)
  {
  }

private:
  GlodonTableViewToExcel * const q_ptr;
  Q_DECLARE_PUBLIC(GlodonTableViewToExcel);

  libxl::Sheet *m_sheet;
  libxl::Book *m_book;
  libxl::Format *m_format;
  libxl::Format *m_titleFormat;

  GlodonTableView *m_tableView;

  QHash<TableCellLocation *, TableCellFormat *> m_diffFormat;
  QHash<int, TableCellFormat *> m_diffTitleFormat;

  int m_horzRowCount;
  int m_vertColCount;
  bool m_exportIcon;
  int m_nStartRow;
  bool m_bIsOverWrite;
  bool m_bIsToSingleSheet;
  int m_nRowCount;
  bool m_showPrompt;
  bool m_groupCollapsed;

  BorderStyle m_borderStyle;
};

GlodonTableViewToExcel::GlodonTableViewToExcel() :
    d_ptr(new GlodonTableViewToExcelPrivate(this))
{
}

GlodonTableViewToExcel::~GlodonTableViewToExcel()
{
    Q_D(GlodonTableViewToExcel);
    if (NULL != d->m_book)
    {
        d->m_book->release();
    }

    //释放表单元格申请资源
    QHash<TableCellLocation *, TableCellFormat *>::const_iterator allCellFormat = d->m_diffFormat.constBegin();

    while (allCellFormat != d->m_diffFormat.constEnd())
    {
        TableCellLocation *perCellLocation = allCellFormat.key();
        TableCellFormat *perCellFormat = allCellFormat.value();
        freeAndNil(perCellLocation);
        freeAndNil(perCellFormat);
        allCellFormat++;
    }

    //释放表标题栏单元格申请资源
    QHash<int, TableCellFormat *>::const_iterator allTitleCellFormat = d->m_diffTitleFormat.constBegin();

    while (allTitleCellFormat != d->m_diffTitleFormat.constEnd())
    {
        TableCellFormat *perTitleCellFormat = allTitleCellFormat.value();
        freeAndNil(perTitleCellFormat);
        allTitleCellFormat++;
    }

    freePtr(d);
}

ExportState GlodonTableViewToExcel::execute(GlodonTableView *tableView,
                                     const GString &fileName,
                                     const GString &sheetName,
                                     bool isCollapsed,
                                     bool isToSingleSheet,
                                     bool isOverWrite,
                                     bool showPrompt)
{
    GlodonTableViewToExcel *exporter = new GlodonTableViewToExcel;

    ExportState exportState;
    try
    {
        exporter->setGroupCollapsed(isCollapsed);
        exporter->setTableView(tableView);
        exporter->setShowPrompt(showPrompt);
        exporter->setIsToSingleSheet(isToSingleSheet);
        exporter->setIsOverWrite(isOverWrite);
        exporter->load();
        exporter->doExport(sheetName);
        exportState = exporter->save(fileName);
    }
    catch (...)
    {
        freeAndNil(exporter);
        throw;
    }

    freeAndNil(exporter);
    return exportState;
}

ExportState GlodonTableViewToExcel::save(const GString &fileName)
{
    bool bResult = false;
    GString strError = "ok";

    Q_D(GlodonTableViewToExcel);
    try
    {
        GString fileNameDir = fileName;

        if (fileNameDir.contains("/"))
        {
            fileNameDir.replace("/", "\\");
        }

        GString fileDirPath = fileNameDir.left(fileNameDir.lastIndexOf("\\"));
        QDir fileDir(fileDirPath);

        if (!fileDir.exists())
        {
            fileDir.mkpath(fileDirPath);
        }

        bResult = d->m_book->save((const wchar_t *)fileName.constData());
        strError = GString::fromLocal8Bit(d->m_book->errorMessage());
    }
    catch (...)
    {
        //把异常吃掉
    }

    if (d->m_showPrompt)
    {
        if (bResult)
        {
            showPrompt(tr("success export to excel"));
        }
        else if (!bResult && strError == GString::fromLocal8Bit("can't open file for writing"))
        {
            gldError(tr("can't open file for writing"));
        }
        else
        {
            gldError(tr("error on saving"));
        }
    }
    else
    {
        if (bResult)
        {
            return EXPORT_SUCCESS;
        }
        else if (!bResult && strError == GString::fromLocal8Bit("can't open file for writing"))
        {
            return EXPORT_NOTWRITING;
        }
    }

    return EXPORT_ERRSAVE;
}

void GlodonTableViewToExcel::load(const GString &fileName)
{
    Q_D(GlodonTableViewToExcel);
    d->m_book = createXLSBook(true);

    if (GFile::exists(fileName))
    {
        if (d->m_bIsOverWrite)
        {
            GFile::remove(fileName);
        }
        else
        {
            d->m_book->load((const wchar_t *)fileName.constData());
        }
    }
}

void GlodonTableViewToExcel::setTableView(GlodonTableView *tableView)
{
    Q_D(GlodonTableViewToExcel);
    d->m_tableView = tableView;
}

void GlodonTableViewToExcel::setExportIcon(bool value)
{
    Q_D(GlodonTableViewToExcel);
    d->m_exportIcon = value;
}

void GlodonTableViewToExcel::setIsOverWrite(bool isOverWrite)
{
    Q_D(GlodonTableViewToExcel);
    d->m_bIsOverWrite = isOverWrite;
}

void GlodonTableViewToExcel::setIsToSingleSheet(bool isToSingleSheet)
{
    Q_D(GlodonTableViewToExcel);
    d->m_bIsToSingleSheet = isToSingleSheet;
}

void GlodonTableViewToExcel::setShowPrompt(bool value)
{
    Q_D(GlodonTableViewToExcel);
    d->m_showPrompt = value;
}

void GlodonTableViewToExcel::setGroupCollapsed(bool collapse)
{
    Q_D(GlodonTableViewToExcel);
    d->m_groupCollapsed = collapse;
}

void GlodonTableViewToExcel::doExport(const GString &sheetName)
{
    Q_D(GlodonTableViewToExcel);
    initExcel(sheetName);
    initSheet();

    transTitleAllMergeCells();
    transMergeCells();
    transTitleData();
    transCellDatas();
    transFootData();

    groupRows();

    if (d->m_bIsToSingleSheet)
    {
        // 最后一行被折叠，应该是合计行之类的高度计算有问题，先加1处理
        d->m_nStartRow = d->m_nStartRow + d->m_nRowCount + 1;
    }
}

void GlodonTableViewToExcel::initExcel(const GString &sheetName)
{
    Q_D(GlodonTableViewToExcel);
    G_ASSERT(d->m_book != NULL);

    if ("" == sheetName)
    {
        d->m_sheet = d->m_book->addSheet(L"sheet1");
    }
    else
    {
        // 不管是不是覆盖，都只需要获取每个页签一次，即第一次写入时
        if (0 == d->m_nStartRow)
        {
            const wchar_t *pWchar = (const wchar_t *)sheetName.constData();
            d->m_sheet = d->m_book->addSheet(pWchar);

            if (NULL == d->m_sheet)
            {
                gldError(tr("current tag page existing"));//当前标签页已经存在"));    //
            }
        }
    }
}

void GlodonTableViewToExcel::initSheet()
{
    Q_D(GlodonTableViewToExcel);
    // 设置打印的时候有格线
    d->m_sheet->setPrintGridlines(true);

    if (!d->m_bIsToSingleSheet)
    {
        d->m_sheet->clear();
    }

    // 列表头如果可见，获取列表头行数
    GlodonMultiHeaderView *pHorzHeader = dynamic_cast<GlodonMultiHeaderView *>(d->m_tableView->horizontalHeader());

    if (NULL != pHorzHeader && !pHorzHeader->isHidden())
    {
        d->m_horzRowCount = pHorzHeader->rowCount();
    }
    else if (d->m_tableView->horizontalHeader() && !d->m_tableView->horizontalHeader()->isHidden())
    {
        d->m_horzRowCount = 1;
    }
    else
    {
        d->m_horzRowCount = 0;
    }

    int nRowCount = d->m_horzRowCount + d->m_tableView->model()->rowCount();
    d->m_nRowCount = nRowCount;
    d->m_sheet->insertRow(d->m_nStartRow, nRowCount);

    // 行表头如果可见，获取行表头行数
    GlodonMultiHeaderView *pVertHeader = dynamic_cast<GlodonMultiHeaderView *>(d->m_tableView->verticalHeader());

    if (NULL != pVertHeader && !pVertHeader->isHidden())
    {
        d->m_vertColCount = pVertHeader->colCount();
    }
    else if (d->m_tableView->verticalHeader() && !d->m_tableView->verticalHeader()->isHidden())
    {
        d->m_vertColCount = 1;
    }
    else
    {
        d->m_vertColCount = 0;
    }

    int nColCount = d->m_vertColCount + d->m_tableView->model()->columnCount();

    // 设置表头的高
    if (NULL != pHorzHeader)
    {
        if (d->m_horzRowCount == 1)
        {
            double dHeight = pHorzHeader->height();

            if (dHeight <= 0)  //如果取出是0的话，给个默认行高
            {
                dHeight = 30;
            }

            dHeight *= 72.0 / 96;
            d->m_sheet->setRow(d->m_nStartRow, dHeight);
        }
        else
        {
            double dHeight;

            for (int i = 0; i < pHorzHeader->headerList.count(); i++)
            {
                dHeight = pHorzHeader->headerList.at(i)->minimumHeight();

                if (dHeight <= 0)
                {
                    dHeight = 30;
                }

                dHeight *= 72.0 / 96;
                d->m_sheet->setRow(i + d->m_nStartRow, dHeight);
            }

            dHeight = pHorzHeader->minimumHeight();

            if (dHeight <= 0)
            {
                dHeight = 30;
            }

            dHeight *= 72.0 / 96;
            d->m_sheet->setRow(pHorzHeader->headerList.count() + d->m_nStartRow,
                            dHeight);
        }
    }
    else
    {
        d->m_sheet->setRow(d->m_nStartRow, d->m_tableView->horizontalHeader()->height());
    }

    const int nViewPortRowCount = nRowCount - d->m_horzRowCount;

    GIntList oSuitHeightCols = d->m_tableView->suitRowHeights();
    for (int i = 0; i < nViewPortRowCount; i++)
    {
        if (d->m_tableView->isRowHidden(i))
        {
            d->m_sheet->setRowHidden(i + d->m_horzRowCount + d->m_nStartRow, true);
            continue;
        }
		
        QModelIndex index = d->m_tableView->model()->index(i, 0);
        const int nManualRowHeight = d->m_tableView->model()->data(index, gidrRowHeightRole).toInt();

        int nCalcRowHeight = d->m_tableView->calcSuitHeight(oSuitHeightCols, i);

        if (0 == nCalcRowHeight)
        {
            nCalcRowHeight = d->m_tableView->rowHeight(i);
        }
        const int nMaxRowHeight = qMax(nManualRowHeight, nCalcRowHeight);

        // 96 是一般的分辨率；1/72 是 1皮克 = 1/72 英寸；
        d->m_sheet->setRow(i + d->m_horzRowCount + d->m_nStartRow, nMaxRowHeight * 72.0 / 96);
    }

    // 设置每列的宽
    if (d->m_vertColCount > 0)
    {
        const int nWidth = d->m_tableView->verticalHeader()->width() / c_nPixToExcelUnit;
        d->m_sheet->setCol(0, 1, nWidth);
    }

    GIntList oSuitColWidths = d->m_tableView->suitColWidths();
    const int nViewPortColumnCount = nColCount - d->m_vertColCount;

    // 合适列宽不处理
    for (int i = 0; i < nViewPortColumnCount; i++)
    {
        int nWidth = 0;

        if (oSuitColWidths.contains(i))
        {
            nWidth = d->m_tableView->calcSuitWidth(i);
        }
        else
        {
            nWidth = d->m_tableView->columnWidth(i) / c_nPixToExcelUnit;
        }

        d->m_sheet->setCol(i + d->m_vertColCount, i + d->m_vertColCount + 1, nWidth);
    }

    //设置导出excel是否绘制格线
    initBorderFormat();
}

void GlodonTableViewToExcel::transTitleData()
{
    Q_D(GlodonTableViewToExcel);
    QAbstractItemModel *oModel = d->m_tableView->model();

    if (!d->m_tableView->horizontalHeader()->isHidden())
    {
        int nMaxRowCount = 0;

        for (int i = 0; i < oModel->columnCount(); i++)
        {
            GString strTitle = oModel->headerData(i, Qt::Horizontal).toString();
            GStringList titles = strTitle.split("|");
            nMaxRowCount = qMax(nMaxRowCount, titles.count());

            for (int j = 0; j < titles.count(); j++)
            {
                Format *oFormat = getTitleFormat(oModel, i, Qt::Horizontal);
                const wchar_t *pWchar = (const wchar_t *)(titles.at(j).constData());
                d->m_sheet->writeStr(j + d->m_nStartRow,
                                  i + d->m_vertColCount, pWchar, oFormat);
            }
        }

        //补齐合并格的右边线
        for (int i = 0; i < nMaxRowCount; i++)
        {
            Format *oFormat = getTitleFormat(oModel, oModel->columnCount() - 1, Qt::Horizontal);
            d->m_sheet->setCellFormat(i, oModel->columnCount() + d->m_vertColCount - 1, oFormat);
        }
    }

    if (!d->m_tableView->verticalHeader()->isHidden())
    {
        int nMaxColumnCount = 0;

        for (int i = 0; i < oModel->rowCount(); i++)
        {
            GString strTitle = oModel->headerData(i, Qt::Vertical).toString();
            GStringList titles = strTitle.split("|");
            nMaxColumnCount = qMax(nMaxColumnCount, titles.count());

            for (int j = 0; j < titles.count(); j++)
            {
                Format *oFormat = getTitleFormat(oModel, i, Qt::Vertical);
                const wchar_t *pWchar = (const wchar_t *)(titles.at(j).constData());
                d->m_sheet->writeStr(i + d->m_horzRowCount + d->m_nStartRow,
                                  j, pWchar, oFormat);
            }
        }

        //补齐合并格的下边线
        for (int i = 0; i < nMaxColumnCount; i++)
        {
            Format *oFormat = getTitleFormat(oModel, oModel->rowCount() - 1, Qt::Vertical);
            d->m_sheet->setCellFormat(oModel->rowCount() + d->m_horzRowCount - 1, i, oFormat);
        }
    }
}

void GlodonTableViewToExcel::transCellDatas()
{
    Q_D(GlodonTableViewToExcel);
    QAbstractItemModel *oModel = d->m_tableView->model();

    for (int i = 0; i < oModel->rowCount(); i++)
    {
        for (int j = 0; j < oModel->columnCount(); j++)
        {
            Format *oFormat = getFormat(oModel, i, j);
            QModelIndex oIndex = oModel->index(i, j);
            GVariant variant = oModel->data(oIndex);

            int nRow = i + d->m_horzRowCount + d->m_nStartRow;
            int nCol = j + d->m_vertColCount;

            // 判断是否是钢筋符号

            GlodonDefaultItemDelegate *gldDelegate = dynamic_cast<GlodonDefaultItemDelegate *>(d->m_tableView->itemDelegate(oIndex));

            Q_ASSERT_X(gldDelegate, "transCellDatas", "Must use GlodonDefaultItemDelegate");

            bool bReadOnly = false;

            GEditStyle edtStyle = gldDelegate->editStyle(
                                      gldDelegate->dataIndex(oIndex),
                                      bReadOnly);

            GObjectList<GlodonSymbolPos *> *pSymbolList = createIndexOfDiameterSymbolList(variant.toString());

            if (pSymbolList->count() > 0)
            {
                writeCellSymbol(i, j);
            }
            else if (esVectorGraph == edtStyle)
            {
                writeCellSymbol(i, j);
            }
            else
            {
                if ((GVariant::Image == variant.type()) || (GVariant::Icon == variant.type()))
                {
                    writeCellIcon(nRow, nCol, variant, oFormat);
                }
                else
                {
                    if (variant.isNull())
                    {
                        writeCellValue("", oFormat, nRow, nCol);
                    }
                    else
                    {
                        writeCellValue(variant, oFormat, nRow, nCol);
                    }
                }
            }

            freeAndNil(pSymbolList);
        }
    }
}

struct GMegerSpan
{
    int top;
    int bottom;
    int left;
    int right;
};

void GlodonTableViewToExcel::transTitleAllMergeCells()
{
    Q_D(GlodonTableViewToExcel);
    QString strTitle;
    QAbstractItemModel *oModel = d->m_tableView->model();

    //水平表头
    QList<QStringList> titles;
    int nColCount = oModel->columnCount();
    int nMaxCount = 0;

    for (int i = 0; i < nColCount; i++)
    {
        strTitle = oModel->headerData(i, Qt::Horizontal).toString();
        titles.append(strTitle.split('|'));
        nMaxCount = qMax(nMaxCount, strTitle.split('|').count());
    }

    //垂直表头
    int nRowCount = oModel->rowCount();
    QList<QStringList> verTitles;
    int nVerMaxCount = 0;

    for (int i = 0; i < nRowCount; i++)
    {
        strTitle = oModel->headerData(i, Qt::Vertical).toString();
        verTitles.append(strTitle.split('|'));
        nVerMaxCount = qMax(nVerMaxCount, strTitle.split('|').count());
    }

    //参考parseSpan代码
    GHeaderSpanCollection *spanCollection = new GHeaderSpanCollection();
    GHeaderSpanCollection *verSpanCollection = new GHeaderSpanCollection();

    try
    {
        //水平表头合并
        for (int i = 0; i < titles.count(); i++)
        {
            int nCurrentCount = titles.at(i).count();

            for (int j = 0; j < nCurrentCount - 1; j++)
            {
                GLDHeaderSpan *newspan
                    = new GLDHeaderSpan(j, i, 1, 1, titles.at(i).at(j), i);
                spanCollection->addSpan(newspan);
            }

            GLDHeaderSpan *newspan = new GLDHeaderSpan(nCurrentCount - 1, i,
                    nMaxCount - nCurrentCount + 1, 1,
                    titles.at(i).at(nCurrentCount - 1), i);
            spanCollection->addSpan(newspan);
        }

        for (int i = 0; i < spanCollection->spans.count(); i++)
        {
            GLDHeaderSpan *span = (GLDHeaderSpan *)(spanCollection->spans.at(i));

            if ((span->top() != span->bottom()) || (span->left() != span->right()))
            {
                d->m_sheet->setMerge(span->top() ,
                                  span->bottom(),
                                  span->left() + d->m_vertColCount,
                                  span->right() + d->m_vertColCount);
            }
        }

        //垂直表头合并
        for (int i = 0; i < verTitles.count(); i++)
        {
            int nCurrentCount = verTitles.at(i).count();

            for (int j = 0; j < nCurrentCount - 1; j++)
            {
                GLDHeaderSpan *newSpan = new GLDHeaderSpan(i, j, 1, 1, verTitles.at(i).at(j), i);
                verSpanCollection->addSpan(newSpan);
            }

            GLDHeaderSpan *newSpan = new GLDHeaderSpan(i, nCurrentCount - 1,
                    1, nVerMaxCount - nCurrentCount + 1,
                    verTitles.at(i).at(nCurrentCount - 1), i);
            verSpanCollection->addSpan(newSpan);
        }

        for (int i = 0; i < verSpanCollection->spans.count(); i++)
        {
            GLDHeaderSpan *span = (GLDHeaderSpan *)(verSpanCollection->spans.at(i));

            if ((span->top() != span->bottom()) || (span->left() != span->right()))
            {
                d->m_sheet->setMerge(span->top() + d->m_horzRowCount,
                                  span->bottom() + d->m_horzRowCount,
                                  span->left(),
                                  span->right());
            }
        }
    }
    catch (...)
    {
        freeAndNil(spanCollection);
        freeAndNil(verSpanCollection);
        throw;
    }

    freeAndNil(spanCollection);
    freeAndNil(verSpanCollection);

    //合并左上角
    if ((d->m_vertColCount > 1) && (d->m_horzRowCount > 1))
    {
        d->m_sheet->setMerge(d->m_nStartRow, d->m_horzRowCount - 1, d->m_nStartRow, d->m_vertColCount - 1);
    }
}

void GlodonTableViewToExcel::transMergeCells()
{
    Q_D(GlodonTableViewToExcel);
    QMultiHash<int, int> oDirtyRect;
    QAbstractItemModel *oModel = d->m_tableView->model();
    GMegerSpan span;
    int nRowCount = oModel->rowCount();
    int nColCount = oModel->columnCount();

    for (int k = 0; k < nRowCount; k++)
    {
        int nCounter = 0;

        while (nCounter < nColCount)
        {
            QMultiHash<int, int>::const_iterator it = oDirtyRect.find(k, nCounter);

            if (it != oDirtyRect.end())
            {
                nCounter++;
                continue;
            }

            span.bottom = k;
            span.top = k;
            span.left = nCounter;
            span.right = nCounter;
            bool bSetMerge = false;
            QModelIndex oIndex = oModel->index(k, nCounter);
            GVariant value = oModel->data(oIndex, gidrMergeIDRole);
            int nMergeID = value.toInt();

            if (nMergeID > 0)
            {
                int nRow = k + 1;

                while (nRow < nRowCount)
                {
                    oIndex = oModel->index(nRow, nCounter);
                    int nID = oModel->data(oIndex, gidrMergeIDRole).toInt();

                    if (nID == nMergeID)
                    {
                        span.bottom = nRow;
                        oDirtyRect.insert(nRow, nCounter);
                        bSetMerge = true;
                    }
                    else
                    {
                        break;
                    }

                    nRow++;
                }

                int nCol = nCounter + 1;

                while (nCol < nColCount)
                {
                    oIndex = oModel->index(k, nCol);
                    int nID = oModel->data(oIndex, gidrMergeIDRole).toInt();

                    if (nID == nMergeID)
                    {
                        span.right = nCol;
                        oDirtyRect.insert(k, nCol);
                        bSetMerge = true;
                    }
                    else
                    {
                        break;
                    }

                    nCol++;
                }

                for (int i = k + 1; i < nRow; i++)
                {
                    for (int j = nCounter + 1; j < nCol; j++)
                    {
                        oDirtyRect.insert(i, j);
                    }
                }
            }

            if (bSetMerge)
            {
                d->m_sheet->setMerge(span.top + d->m_horzRowCount + d->m_nStartRow,
                                  span.bottom + d->m_horzRowCount + d->m_nStartRow,
                                  span.left + d->m_vertColCount,
                                  span.right + d->m_vertColCount);
            }

            nCounter++;
        }
    }
}

void GlodonTableViewToExcel::transFootData()
{
    Q_D(GlodonTableViewToExcel);
    GlodonFooterTableView *pFooterTableView = dynamic_cast<GlodonFooterTableView *>(d->m_tableView);

    if (NULL == pFooterTableView)
    {
        return;
    }

    GFooterView *pFootView = pFooterTableView->footer();
    QAbstractItemModel *pModel = pFootView->model();

    if (NULL == pModel)
    {
        return;
    }

    int nRow = d->m_horzRowCount + pFooterTableView->model()->rowCount();

    for (int i = 0; i < pModel->rowCount(); i++)
    {
        int nHeight = 30;
        d->m_sheet->setRow(i + nRow + d->m_nStartRow, nHeight);
    }

    for (int i = 0; i < pModel->rowCount(); i++)
    {
        GString strTitle = pModel->headerData(i, Qt::Horizontal).toString();
        GStringList titles = strTitle.split("|");

        for (int j = 0; j < titles.count(); j++)
        {
            Format *oFormat = getTitleFormat(pModel, i, Qt::Vertical);
            const wchar_t *pWchar = (const wchar_t *)(titles.at(j).constData());
            d->m_sheet->writeStr(nRow + i + d->m_nStartRow, j, pWchar, oFormat);
        }
    }

    for (int i = 0; i < pModel->rowCount(); i++)
    {
        for (int j = 0; j < pModel->columnCount(); j++)
        {
            Format *oFormat = getFormat(pModel, i, j);
            QModelIndex oIndex = pModel->index(i, j);
            GVariant value = pModel->data(oIndex, Qt::DisplayRole);
            int nRow = d->m_horzRowCount + pFooterTableView->model()->rowCount() + i + d->m_nStartRow;
            int nCol = d->m_vertColCount + j;
            writeCellValue(value, oFormat, nRow, nCol);
        }
    }

}

/**
 * @brief GlodonTableViewToExcel::groupRows
 * 有两点需要注意：1，分组必须从最小到最大分组，即 调用按照m_sheet->groupRows(2, 17); m_sheet->groupRows(3, 7);这个顺序，
 *              如果按照m_sheet->groupRows(3, 7);m_sheet->groupRows(2, 17);就会出现显示问题
 *         2，从excel的操作来看,分组开始是第一个孩子的rownumber，而不是本身
 *  对于以上两点的处理，第一个，就是用map存放，然后，按照从0到rowcount来获取。
 *              第二个，就是在处理的时候每次都加1。
 */
void GlodonTableViewToExcel::groupRows()
{
    Q_D(GlodonTableViewToExcel);
    if (d->m_tableView->isTree())
    {
        int rowcount = d->m_tableView->model()->rowCount();
        GMap<int, GlodonExcelGroupStruct> groups;

        try
        {
            for (int i = 0; i < rowcount; ++i)
            {
                i += recursiveGroupRowsParent(i, &groups);
            }

            // 设置组
            for (GMap<int, GlodonExcelGroupStruct>::Iterator it = groups.begin();
                    it != groups.end(); ++it)
            {
                GlodonExcelGroupStruct parent = getLastParentGroup(it.value(), &groups);

                if (parent.start != -1 && !parent.visible)
                {
                    it.value().visible = false;
                }

                d->m_sheet->groupRows(it.value().start + d->m_horzRowCount, it.value().end  + d->m_horzRowCount, !it.value().visible);
            }

//            for (int i = 0; i < rowcount; ++i)
//            {
//                // 判断i的子是否是group
//                if (groups.find(i+1) != groups.end())
//                {
//                    bool isRowExpanded = d->m_tableView->isRowExpanded(i);
//                    d->m_sheet->groupRows(i + 1 + d->m_horzRowCount, groups.value(i + 1)  + d->m_horzRowCount, !isRowExpanded);
//                }
//            }
        }
        catch (...)
        {
            throw;
        }

        d->m_sheet->setGroupSummaryBelow(false);
        d->m_sheet->setGroupSummaryRight(true);
    }
}

//int GlodonTableViewToExcel::recursiveGroupRows(int currentIndex, int childCount, GMap<int, int> *groups)
//{
//    if (0 == childCount)
//    {
//        return 1;
//    }
//    int startindex = currentIndex;

//    for (int i = 0; i < childCount; i++)
//    {
//        QModelIndex modeindex = d->m_tableView->model()->index(currentIndex, 0);
//        QModelIndex dataIndex = d->m_tableView->dataIndex(modeindex);
//        int childcont =  dataIndex.model()->rowCount(dataIndex);
//        currentIndex += recursiveGroupRows(currentIndex + 1, childcont, groups);
//    }

//    if (currentIndex > startindex)
//    {
//        groups->insert(startindex, currentIndex);
//    }
//    // + 1:包含本身
//    return currentIndex - startindex + 1;
//}

int GlodonTableViewToExcel::recursiveGroupRowsParent(int parentIndex, GMap<int, GlodonExcelGroupStruct> *groups)
{
    Q_D(GlodonTableViewToExcel);
    QModelIndex modeindex = d->m_tableView->model()->index(parentIndex, 0);
    QModelIndex dataIndex = d->m_tableView->dataIndex(modeindex);
    int childCount = dataIndex.model()->rowCount(dataIndex);

    int endIndex = parentIndex;

    for (int i = 0; i < childCount; ++i)
    {
        // 探测下一个节点
        ++endIndex;
        endIndex += recursiveGroupRowsParent(endIndex, groups);
    }

    if (childCount > 0)
    {
        GlodonExcelGroupStruct group;
        group.start = parentIndex + 1;
        group.end = endIndex;
        group.visible = d->m_tableView->isRowExpanded(parentIndex);
        groups->insert(group.start, group);
        return endIndex - parentIndex;
    }
    else
    {
        return 0;
    }
}

GlodonTableViewToExcel::GlodonExcelGroupStruct
GlodonTableViewToExcel::getLastParentGroup(
    const GlodonTableViewToExcel::GlodonExcelGroupStruct &group,
    GMap<int, GlodonExcelGroupStruct> *groups)
{
    GlodonExcelGroupStruct result;

    for (GMap<int, GlodonExcelGroupStruct>::Iterator it = groups->begin();
            it != groups->end(); ++it)
    {
        if (it.key() >= group.start)
        {
            break;
        }

        if (it.value().start <= group.start && it.value().end >= group.end)
        {
            if (result.start <= it.value().start && result.end >= it.value().end)
            {
                result = it.value();
            }
        }
    }

    return result;
}

void GlodonTableViewToExcel::writeCellValue(const GVariant &value, Format *format, int row, int col)
{
    Q_D(GlodonTableViewToExcel);
    if (GVariant::String == value.type() || GVariant::DateTime == value.type()
            || GVariant::Date == value.type())
    {
        const wchar_t *pWchar;
        GString strDateTime;

        if (GVariant::String == value.type())
        {
            pWchar = (const wchar_t *)value.toString().constData();
        }
        else if (GVariant::DateTime == value.type())
        {
            strDateTime = value.value<QDateTime>().toString("yyyy-MM-dd");
            pWchar = (const wchar_t *)(strDateTime.constData());
        }
        else if (GVariant::Date == value.type())
        {
            strDateTime = value.value<QDate>().toString("yyyy-MM-dd");
            pWchar = (const wchar_t *)(strDateTime.constData());
        }

        if (!value.isNull() && (pWchar[0] == L'='))
        {
            pWchar++;
            d->m_sheet->writeFormula(row, col, pWchar, format);
        }
        else
        {
            d->m_sheet->writeStr(row, col, pWchar, format);
        }
    }
    else if (GVariant::Bool == value.type())
    {
        d->m_sheet->writeBool(row, col, value.toBool(), format);
    }
    else if (variantTypeIsNumeric(value.type()))
    {
        d->m_sheet->writeNum(row, col, value.toDouble(), format);
    }
    else if (GVariant::Color == value.type())
    {
        QColor color = qvariant_cast<QColor>(value);
        Color xlsColorFont = d->m_book->colorPack(color.red(), color.green(), color.blue());

        if (COLOR_WHITE != xlsColorFont)
        {
            format->setFillPattern(libxl::FILLPATTERN_SOLID);
            format->setPatternForegroundColor(xlsColorFont);
        }
    }

    format->setBorderTop(d->m_borderStyle);
    format->setBorderBottom(d->m_borderStyle);
    format->setBorderLeft(d->m_borderStyle);
    format->setBorderRight(d->m_borderStyle);
    d->m_sheet->setCellFormat(row, col, format);
}
/**
 * @brief GlodonTableViewToExcel::writeIcon
 * 现在只是支撑icon和string在一个格子里面，别的现在还不支持
 * @param model
 * @param oIndex
 * @param format
 * @param row
 * @param col
 */
void GlodonTableViewToExcel::writeIcon(const QAbstractItemModel *model,  QModelIndex oIndex, Format *format, int row, int col)
{
    Q_D(GlodonTableViewToExcel);
    GVariant valueIcon = model->data(oIndex);

    if ((GVariant::Icon != valueIcon.type()) && (GVariant::Image != valueIcon.type()))
    {
        return;
    }

    GString fileName;
    QSize c_size;
    int picId;

    if (GVariant::Image == valueIcon.type())
    {
        QImage oImage = qvariant_cast<QImage>(valueIcon);
        fileName = oImage.text("Image");
        picId = d->m_book->addPicture((const wchar_t *)(fileName.constData()));
        c_size = oImage.size();
    }
    else if (GVariant::Icon == valueIcon.type())
    {
        GIcon iconConetct = qvariant_cast<GIcon>(valueIcon);
        fileName = iconConetct.themeName();
        QStyleOptionViewItem item4 = ((GlodonAbstractItemView *)d->m_tableView)->StyleOptionViewItem();
        c_size  = iconConetct.actualSize(item4.decorationSize);

        picId = d->m_book->addPicture((const wchar_t *)fileName.constData());
    }

    d->m_sheet->setPicture2(row, col, picId, c_size.width(), c_size.height());

    GString text = model->data(oIndex, Qt::DisplayRole).toString();
    const wchar_t *wchar = (const wchar_t *)(text.constData());

    if (!text.isNull() && (wchar[0] == L'='))
    {
        wchar++;
        format->setIndent(4);
        setFomatIndentSize(format, c_size.width());
        d->m_sheet->writeFormula(row, col, wchar, format);
    }
    else
    {
        setFomatIndentSize(format, c_size.width());
        d->m_sheet->writeStr(row, col, wchar, format);
    }
}

void GlodonTableViewToExcel::writeCellIcon(int row, int col, const GVariant value, const Format *format)
{
    Q_D(GlodonTableViewToExcel);
    if ((GVariant::Icon != value.type()) && (GVariant::Image != value.type()))
    {
        return;
    }

    GString fileName;
    QSize c_size;
    int picId;

    if (GVariant::Image == value.type())
    {
        QImage oImage = qvariant_cast<QImage>(value);
        QStringList imageKeys = oImage.textKeys();

        for (int i = 0; i < imageKeys.count(); i++)
        {
            QString key = imageKeys.at(i);
        }

        fileName = oImage.text("Image");
        picId = d->m_book->addPicture((const wchar_t *)(fileName.constData()));
        c_size = oImage.size();
    }
    else if (GVariant::Icon == value.type())
    {
        GIcon iconConetct = qvariant_cast<GIcon>(value);
        fileName = iconConetct.themeName();
        QStyleOptionViewItem item4 = ((GlodonAbstractItemView *)d->m_tableView)->StyleOptionViewItem();
        c_size  = iconConetct.actualSize(item4.decorationSize);

        picId = d->m_book->addPicture((const wchar_t *)fileName.constData());
    }

    int nOffsetX;

    if ((format->alignH() & ALIGNH_LEFT) == ALIGNH_LEFT)
    {
        nOffsetX = 1;
    }
    else if ((format->alignH() & ALIGNH_RIGHT) == ALIGNH_RIGHT)
    {
        nOffsetX = d->m_sheet->colWidth(col) * 7.55 - c_size.width();
    }
    else
    {
        nOffsetX = (d->m_sheet->colWidth(col) * 7.55 - c_size.width()) / 2;
    }

    int nOffsetY;

    if ((format->alignV() & ALIGNV_TOP) == ALIGNV_TOP)
    {
        nOffsetY = 0;
    }
    else if ((format->alignV() & ALIGNV_BOTTOM) == ALIGNV_BOTTOM)
    {
        nOffsetY = d->m_sheet->rowHeight(row) * 96.0 / 72 - c_size.height();
    }
    else
    {
        nOffsetY = (d->m_sheet->rowHeight(row) * 96.0 / 72 - c_size.height()) / 2;
    }

    d->m_sheet->setPicture2(row, col, picId, c_size.width(),
                         c_size.height(), nOffsetX, nOffsetY);

    Format *pFormat = d->m_book->addFormat();
    pFormat->setBorderTop(d->m_borderStyle);
    pFormat->setBorderBottom(d->m_borderStyle);
    pFormat->setBorderLeft(d->m_borderStyle);
    pFormat->setBorderRight(d->m_borderStyle);
    d->m_sheet->setCellFormat(row, col, pFormat);
}

void GlodonTableViewToExcel::writeCellSymbol(int row, int col)
{
    Q_D(GlodonTableViewToExcel);
    QModelIndex index = d->m_tableView->model()->index(row, col);
    int nCellRow = row + d->m_horzRowCount + d->m_nStartRow;
    int nCellCol = col + d->m_vertColCount;

    QRect cellRect(-1, -1, -1, -1);
    cellRect = d->m_tableView->visualRect(index);

    QPixmap pixmap = d->m_tableView->viewport()->grab(cellRect);

    int nWidth = d->m_tableView->columnWidth(col);

    d->m_sheet->setCol(nCellCol, nCellCol, nWidth / c_nPixToExcelUnit);

    d->m_sheet->setRow(nCellRow, d->m_sheet->rowHeight(nCellRow) + 2);

    QString sFileName = QDir::tempPath() + "/Symbol.jpg";
    pixmap.save(sFileName);
    int picId = d->m_book->addPicture((const wchar_t *)sFileName.constData());

    int nFirstRow = -1;
    int nLastRow = -1;
    int nFirstCol = -1;
    int nLastCol = -1;
    d->m_sheet->getMerge(nCellRow, nCellCol, &nFirstRow, &nLastRow,
                      &nFirstCol, &nLastCol);

    if (-1 == nFirstRow)
    {
        nFirstRow = nCellRow;
        nFirstCol = nCellCol;
    }

    d->m_sheet->setPicture(nFirstRow, nFirstCol, picId, 1, 1, 1);

    Format *format = d->m_book->addFormat();
    format->setBorderTop(d->m_borderStyle);
    format->setBorderBottom(d->m_borderStyle);
    format->setBorderLeft(d->m_borderStyle);
    format->setBorderRight(d->m_borderStyle);
    d->m_sheet->setCellFormat(nCellRow, nCellCol, format);

    GFile::remove(sFileName);
}

/**
 * @brief GlodonTableViewToExcel::setFomatIndentSize
 *  excel的缩进不是按照像素来的，是按照字体大小来缩进的，所以需要把像素点转换到相应的字体大小。
 *
 */
void GlodonTableViewToExcel::setFomatIndentSize(Format *format, int pixMaSize)
{
    int fontSize = format->font()->size();
    int indentSize = (pixMaSize / fontSize) ;
    format->setIndent(indentSize);
}

Format *GlodonTableViewToExcel::getFormat(const QAbstractItemModel *model, int row, int col)
{
    Q_D(GlodonTableViewToExcel);
    if (d->m_diffFormat.isEmpty())
    {
        d->m_format = addDiffFormat(model, row, col);
    }
    else
    {
        if (!hasMatchedFormat(model, row, col))
        {
            d->m_format = addDiffFormat(model, row, col);
        }
    }

    return d->m_format;
}

Format *GlodonTableViewToExcel::getTitleFormat(const QAbstractItemModel *model, int section,
        Qt::Orientation orientation)
{
    Q_D(GlodonTableViewToExcel);
    if (d->m_diffTitleFormat.isEmpty())
    {
        d->m_titleFormat = addDiffTitleFormat(model, section, orientation);
    }
    else
    {
        if (!hasMatchedTitleFormat(model, section, orientation))
        {
            d->m_titleFormat = addDiffTitleFormat(model, section, orientation);
        }
    }

    return d->m_titleFormat;
}

Format *GlodonTableViewToExcel::addDiffTitleFormat(const QAbstractItemModel *model, int section,
        Qt::Orientation orientation)
{
    Q_D(GlodonTableViewToExcel);
    TableCellFormat *titleCellFormat = new TableCellFormat;
    Format *format = d->m_book->addFormat();
    GVariant value = model->headerData(section, orientation, Qt::FontRole);
    GFont font = qvariant_cast<GFont>(value);
    Font *pFont = d->m_book->addFont();
    pFont->setName((const wchar_t *)font.family().constData());
    titleCellFormat->fontName = const_cast<QChar *>(font.family().constData());
    pFont->setSize(font.pointSize());
    titleCellFormat->fontSize = font.pointSize();
    pFont->setBold(font.bold());
    titleCellFormat->fontIsBold = font.bold();

    if (font.underline())
    {
        pFont->setUnderline(libxl::UNDERLINE_SINGLE);
    }

    titleCellFormat->fontIsUnderline = font.underline();
    pFont->setStrikeOut(font.strikeOut());
    titleCellFormat->fontIsStrikeOut = font.strikeOut();
    pFont->setItalic(font.italic());
    titleCellFormat->fontIsItalic = font.italic();

    value = model->headerData(section, orientation, Qt::ForegroundRole);
    QColor oColor = qvariant_cast<QColor>(value);
    Color pColor = d->m_book->colorPack(oColor.red(), oColor.green(), oColor.blue());
    pFont->setColor(pColor);
    titleCellFormat->foregroundRed = oColor.red();
    titleCellFormat->foregroundGreen = oColor.green();
    titleCellFormat->foregroundBlue = oColor.blue();
    format->setFont(pFont);

    value = model->headerData(section, orientation, Qt::TextAlignmentRole);
    Qt::Alignment align = Qt::AlignCenter;

    if (GVariant() != value)
    {
        align = (Qt::Alignment)(qvariant_cast<unsigned int>(value));
    }

    AlignH horizontal = QtAlignmentToXLSHorzAlignment(align);
    AlignV vertical = QtAlignmentToXLSVertAlignment(align);
    format->setAlignH(horizontal);
    format->setAlignV(vertical);
    titleCellFormat->textAlignment = align;

    value = model->headerData(section, orientation, Qt::BackgroundColorRole);

    if (GVariant() != value)
    {
        oColor = qvariant_cast<QColor>(value);
        pColor = d->m_book->colorPack(oColor.red(), oColor.green(), oColor.blue());
        format->setFillPattern(libxl::FILLPATTERN_SOLID);
        titleCellFormat->backColorRed = oColor.red();
        titleCellFormat->backColorGreen = oColor.green();
        titleCellFormat->backColorBlue = oColor.blue();

        if (pColor == Color(-1))
        {
            format->setPatternForegroundColor(COLOR_WHITE);
        }
        else
        {
            format->setPatternForegroundColor(pColor);
        }
    }

    format->setWrap(d->m_tableView->wordWrap());
    format->setBorderTop(d->m_borderStyle);
    format->setBorderBottom(d->m_borderStyle);
    format->setBorderLeft(d->m_borderStyle);
    format->setBorderRight(d->m_borderStyle);

    titleCellFormat->diffFormat = format;
    d->m_diffTitleFormat.insert(section, titleCellFormat);
    return format;
}

bool GlodonTableViewToExcel::hasMatchedTitleFormat(const QAbstractItemModel *model, int section, Qt::Orientation orientation)
{
    Q_D(GlodonTableViewToExcel);
    GFont font = qvariant_cast<GFont>(model->headerData(section, orientation, Qt::FontRole));
    QColor oForeColor = qvariant_cast<QColor>(model->headerData(section, orientation, Qt::ForegroundRole));
    QVariant value = model->headerData(section, orientation, Qt::TextAlignmentRole);
    Qt::Alignment align = Qt::AlignCenter;

    if (GVariant() != value)
    {
        align = (Qt::Alignment)(qvariant_cast<unsigned int>(value));
    }

    value = model->headerData(section, orientation, Qt::BackgroundColorRole);
    QColor oBackColor;

    if (GVariant() != value)
    {
        oBackColor = qvariant_cast<QColor>(value);
    }

    QHash<int, TableCellFormat *>::const_iterator tableTitleCellIter = d->m_diffTitleFormat.constBegin();

    while (tableTitleCellIter != d->m_diffTitleFormat.constEnd())
    {
        //Hash中存储的Format样式
        TableCellFormat *matchCellFormat = tableTitleCellIter.value();

        //Qt::FontRole比较结果
        bool isSameFontRole = false;

        if (matchCellFormat->fontName == font.family().constData()
                && matchCellFormat->fontSize == font.pointSize()
                && matchCellFormat->fontIsBold == font.bold()
                && matchCellFormat->fontIsUnderline == font.underline()
                && matchCellFormat->fontIsStrikeOut == font.strikeOut()
                && matchCellFormat->fontIsItalic == font.italic())
        {
            isSameFontRole = true;
        }

        //Qt::ForegroundRole比较结果
        bool isSameForegroundRole = false;

        if (matchCellFormat->foregroundRed == oForeColor.red()
                && matchCellFormat->foregroundGreen == oForeColor.green()
                && matchCellFormat->foregroundBlue == oForeColor.blue())
        {
            isSameForegroundRole = true;
        }

        //Qt::TextAlignmentRole比较结果
        bool isSameTextAligRole = false;

        if (matchCellFormat->textAlignment == align)
        {
            isSameTextAligRole = true;
        }

        //Qt::BackgroundColorRole比较结果
        bool isSameBackColorRole = false;

        if (matchCellFormat->backColorRed == oBackColor.red()
                && matchCellFormat->backColorGreen == oBackColor.green()
                && matchCellFormat->backColorBlue == oBackColor.blue())
        {
            isSameBackColorRole = true;
        }

        if (isSameFontRole
                && isSameForegroundRole
                && isSameTextAligRole
                && isSameBackColorRole)
        {
            d->m_titleFormat = matchCellFormat->diffFormat;
            return true;
        }

        tableTitleCellIter++;
    }

    return false;
}

void GlodonTableViewToExcel::initBorderFormat()
{
    Q_D(GlodonTableViewToExcel);
    if (d->m_tableView->gridLineWidth() == 0)
    {
        d->m_borderStyle = BORDERSTYLE_NONE;
    }
}

Format *GlodonTableViewToExcel::addDiffFormat(const QAbstractItemModel *model, int row, int col)
{
    Q_D(GlodonTableViewToExcel);
    TableCellLocation *cellLocation = new TableCellLocation(row, col);
    TableCellFormat *cellFormat = new TableCellFormat;
    Format *format = d->m_book->addFormat();
    QModelIndex oIndex = model->index(row, col);
    GVariant value = model->data(oIndex, Qt::FontRole);
    GFont font = qvariant_cast<GFont>(value);
    Font *pFont = d->m_book->addFont();
    pFont->setName((const wchar_t *)font.family().constData());
    cellFormat->fontName = const_cast<QChar *>(font.family().constData());
    pFont->setSize(font.pointSize());
    cellFormat->fontSize = font.pointSize();
    pFont->setBold(font.bold());
    cellFormat->fontIsBold = font.bold();

    if (font.underline())
    {
        pFont->setUnderline(libxl::UNDERLINE_SINGLE);
    }

    cellFormat->fontIsUnderline = font.underline();
    pFont->setStrikeOut(font.strikeOut());
    cellFormat->fontIsStrikeOut = font.strikeOut();
    pFont->setItalic(font.italic());
    cellFormat->fontIsItalic = font.italic();

    value = model->data(oIndex, Qt::ForegroundRole);
    QColor oColor = qvariant_cast<QColor>(value);
    Color pColor = d->m_book->colorPack(oColor.red(), oColor.green(), oColor.blue());
    pFont->setColor(pColor);
    cellFormat->foregroundRed = oColor.red();
    cellFormat->foregroundGreen = oColor.green();
    cellFormat->foregroundBlue = oColor.blue();
    format->setFont(pFont);

    value = model->data(oIndex, Qt::TextAlignmentRole);

    if (GVariant() != value)
    {
        Qt::Alignment align = (Qt::Alignment)(qvariant_cast<unsigned short>(value));
        AlignH horizontal = QtAlignmentToXLSHorzAlignment(align);
        AlignV vertical = QtAlignmentToXLSVertAlignment(align);
        format->setAlignH(horizontal);
        format->setAlignV(vertical);
        cellFormat->textAlignment = align;
    }

    value = model->data(oIndex, Qt::BackgroundColorRole);

    if (GVariant() != value)
    {
        oColor = qvariant_cast<QColor>(value);
        pColor = d->m_book->colorPack(oColor.red(), oColor.green(), oColor.blue());

        format->setFillPattern(libxl::FILLPATTERN_SOLID);

        if (pColor == Color(-1))
        {
            format->setPatternForegroundColor(COLOR_WHITE);
        }
        else
        {
            format->setPatternForegroundColor(pColor);
        }

        cellFormat->backColorRed = oColor.red();
        cellFormat->backColorGreen = oColor.green();
        cellFormat->backColorBlue = oColor.blue();
    }

    format->setWrap(d->m_tableView->wordWrap());
    format->setBorderTop(d->m_borderStyle);
    format->setBorderBottom(d->m_borderStyle);
    format->setBorderLeft(d->m_borderStyle);
    format->setBorderRight(d->m_borderStyle);
    cellFormat->diffFormat = format;
    d->m_diffFormat.insert(cellLocation, cellFormat);
    return format;
}

bool GlodonTableViewToExcel::hasMatchedFormat(const QAbstractItemModel *model, int row, int col)
{
    Q_D(GlodonTableViewToExcel);
    QModelIndex oIndex = model->index(row, col);
    GFont font = qvariant_cast<GFont>(model->data(oIndex, Qt::FontRole));
    QColor oForeColor = qvariant_cast<QColor>(model->data(oIndex, Qt::ForegroundRole));
    Qt::Alignment align = (Qt::Alignment)qvariant_cast<unsigned short>(model->data(oIndex, Qt::TextAlignmentRole));
    QColor oBackColor = qvariant_cast<QColor>(model->data(oIndex, Qt::BackgroundColorRole));

    //判断存储在Hash中是否有相同的Format
    QHash<TableCellLocation *, TableCellFormat *>::const_iterator tableTitleCellIter = d->m_diffFormat.constBegin();

    while (tableTitleCellIter != d->m_diffFormat.constEnd())
    {
        //Hash中存储的Format样式
        TableCellFormat *matchCellFormat = tableTitleCellIter.value();

        //Qt::FontRole比较结果
        bool isSameFontRole = false;

        if (matchCellFormat->fontName == font.family().constData()
                && matchCellFormat->fontSize == font.pointSize()
                && matchCellFormat->fontIsBold == font.bold()
                && matchCellFormat->fontIsUnderline == font.underline()
                && matchCellFormat->fontIsStrikeOut == font.strikeOut()
                && matchCellFormat->fontIsItalic == font.italic())
        {
            isSameFontRole = true;
        }

        //Qt::ForegroundRole比较结果
        bool isSameForegroundRole = false;

        if (matchCellFormat->foregroundRed == oForeColor.red()
                && matchCellFormat->foregroundGreen == oForeColor.green()
                && matchCellFormat->foregroundBlue == oForeColor.blue())
        {
            isSameForegroundRole = true;
        }

        //Qt::TextAlignmentRole比较结果
        bool isSameTextAligRole = false;

        if (matchCellFormat->textAlignment == align)
        {
            isSameTextAligRole = true;
        }

        //Qt::BackgroundColorRole比较结果
        bool isSameBackColorRole = false;

        if (matchCellFormat->backColorRed == oBackColor.red()
                && matchCellFormat->backColorGreen == oBackColor.green()
                && matchCellFormat->backColorBlue == oBackColor.blue())
        {
            isSameBackColorRole = true;
        }

        if (isSameFontRole
                && isSameForegroundRole
                && isSameTextAligRole
                && isSameBackColorRole)
        {
            d->m_format = matchCellFormat->diffFormat;
            return true;
        }

        tableTitleCellIter++;
    }

    return false;
}

class GLDDocViewToExcelPrivate
{
public:
  GLDDocViewToExcelPrivate(GLDDocViewToExcel *parent)
    : q_ptr(parent), m_nStartExportPage(0), m_nEndExportPage(0),
      m_sLoadFileName(""), m_sSaveFileName(""),
      m_sSheetName("")
  {
  }

private:
  GLDDocViewToExcel * const q_ptr;
  Q_DECLARE_PUBLIC(GLDDocViewToExcel);

  GLDDocView *m_docView;
  GlodonTableViewToExcel *m_tableViewToXls;
  int m_nStartExportPage;
  int m_nEndExportPage;
  QString m_sLoadFileName;
  QString m_sSaveFileName;
  QString m_sSheetName;
};

GLDDocViewToExcel::GLDDocViewToExcel() : d_ptr(new GLDDocViewToExcelPrivate(this))

{
    Q_D(GLDDocViewToExcel);
    d->m_docView = NULL;
    d->m_tableViewToXls = new GlodonTableViewToExcel();
}

GLDDocViewToExcel::~GLDDocViewToExcel()
{
    Q_D(GLDDocViewToExcel);
    freeAndNil(d->m_docView);
    freeAndNil(d->m_tableViewToXls);
    freePtr(d);
}

bool GLDDocViewToExcel::execute(GLDDocView *docView,
                                const GString &fileName,
                                const GString &sheetName,
                                bool isToSingleSheet,
                                bool isOverWrite,
                                bool showPrompt)
{
    GlodonTableViewToExcel *exporter = new GlodonTableViewToExcel();

    try
    {
        exporter->setIsToSingleSheet(isToSingleSheet);
        exporter->setIsOverWrite(isOverWrite);
        exporter->load(fileName);
        exporter->setShowPrompt(showPrompt);

        for (int i = 0; i < docView->pageCount(); ++i)
        {
            GLDPaperWidget *pPaper = docView->newPaper(i);
            exporter->setTableView(pPaper->tableView());
            exporter->doExport(sheetName);
            freeAndNil(pPaper);
        }

        exporter->save(fileName);
    }
    catch (...)
    {
        freeAndNil(exporter);
        throw;
    }

    freeAndNil(exporter);
    return true;
}

void GLDDocViewToExcel::setDocView(GLDDocView *docView)
{
    Q_D(GLDDocViewToExcel);
    d->m_docView = docView;
    d->m_nEndExportPage = d->m_docView->pageCount() - 1;
}

ExportState GLDDocViewToExcel::exportToExcel(bool isToSingleSheet, bool isOverWrite, bool showPrompt)
{
    Q_D(GLDDocViewToExcel);
    d->m_tableViewToXls->setShowPrompt(showPrompt);
    d->m_tableViewToXls->setIsToSingleSheet(isToSingleSheet);
    d->m_tableViewToXls->setIsOverWrite(isOverWrite);
    d->m_tableViewToXls->load(d->m_sLoadFileName);

    for (int i = d->m_nStartExportPage; i <= d->m_nEndExportPage; ++i)
    {
        GLDPaperWidget *pPaper = d->m_docView->newPaper(i);
        d->m_tableViewToXls->setTableView(pPaper->tableView());
        d->m_tableViewToXls->doExport(d->m_sSheetName);
        freeAndNil(pPaper);
    }

    return d->m_tableViewToXls->save(d->m_sSaveFileName);
}

void GLDDocViewToExcel::setStartExportPage(int startExportPage)
{
    Q_D(GLDDocViewToExcel);
    d->m_nStartExportPage = startExportPage;
}

void GLDDocViewToExcel::setEndExportPage(int endExportPage)
{
    Q_D(GLDDocViewToExcel);
    d->m_nEndExportPage = endExportPage;
}

void GLDDocViewToExcel::setLoadFileName(QString fileName)
{
    Q_D(GLDDocViewToExcel);
    d->m_sLoadFileName = fileName;
}

void GLDDocViewToExcel::setSaveFileName(QString fileName)
{
    Q_D(GLDDocViewToExcel);
    d->m_sSaveFileName = fileName;
}

void GLDDocViewToExcel::setSheetName(QString sheetName)
{
    Q_D(GLDDocViewToExcel);
    d->m_sSheetName = sheetName;
}
