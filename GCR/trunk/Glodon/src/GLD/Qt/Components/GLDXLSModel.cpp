#include "GLDXLSModel.h"
#include "GLDGlobal.h"
#include "GLDStrUtils.h"
#include <QColor>
#include <QFont>
#include <QList>
#include "GLDFileInfo.h"
#include "GLDXLS.h"

using namespace libxl;

/* GlodonXLSModel */

GlodonXLSModel::GlodonXLSModel(QObject *parent) : GlodonAbstractItemModel(parent),
    m_book(NULL), m_bakBook(NULL), m_sheet(NULL), m_editable(false)
{
}

GlodonXLSModel::~GlodonXLSModel()
{
    while(m_listBook.count() != NULL)
    {
        libxl::Book *listBook = m_listBook.takeFirst();
        if (listBook != NULL)
        {
            listBook->release();
        }
    }
}

bool GlodonXLSModel::load(const GString &fileName)
{
    GFileInfo fi(fileName);

    if (fi.suffix() == "xls")
    {
        m_book = createXLSBook(true);

        if (!m_listBook.contains(m_book))
        {
            m_listBook.append(m_book);
        }
    }
    else
    {
        m_book = createXLSBook(false);
    }

    m_book->setRgbMode(true);

    m_bakBook = createXLSBook(true);

    if (!m_listBook.contains(m_bakBook))
    {
        m_listBook.append(m_bakBook);
    }

    m_bakBook->setRgbMode(true);
    return m_book->load((const wchar_t *)fileName.constData());
}

bool GlodonXLSModel::save(const GString &fileName)
{
    return m_book->save((const wchar_t *)fileName.constData());
}

bool GlodonXLSModel::setSheetIndex(int index)
{
    if (m_book && (m_book->sheetCount() > 0) && (index >= 0) && (index < m_book->sheetCount()))
    {
        m_sheet = m_book->getSheet(index);
        return m_sheet != NULL;
    }

    return false;
}

void GlodonXLSModel::setXlsBook(Book *oBook)
{
    m_book = oBook;

//    if (NULL == m_bakBook)
//    {
//        m_bakBook = createXLSBook(true);
//        m_bakBook->setRgbMode(true);
//    }
}

void GlodonXLSModel::setXlsBakBook(Book *oBakBook)
{
    m_bakBook = oBakBook;
}

GString GlodonXLSModel::getSheetName() const
{
    if (NULL != m_sheet)
    {
        return GString::fromWCharArray(m_sheet->name());
    }

    return GString("");
}

QModelIndex GlodonXLSModel::index(int row, int column, const QModelIndex &) const
{
    return createIndex(row, column);
}

QModelIndex GlodonXLSModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int GlodonXLSModel::rowCount(const QModelIndex &parent) const
{
    if (!m_sheet)
    {
        return 0;
    }

    if (parent.isValid())
    {
        return 0;
    }
    else if (m_sheet->lastRow() >= 0)
    {
        return m_sheet->lastRow();
    }
    else
    {
        return 0;
    }
}

int GlodonXLSModel::columnCount(const QModelIndex &parent) const
{
    if (!m_sheet)
    {
        return 0;
    }

    if (parent.isValid())
    {
        return 0;
    }
    else
    {
        return m_sheet->lastCol();
    }
}

bool GlodonXLSModel::hasChildren(const QModelIndex &parent) const
{
    if (!m_sheet)
    {
        return false;
    }

    if (parent.isValid())
    {
        return false;
    }
    else
    {
        return m_sheet->lastRow() >= 0;
    }
}

QVariant GlodonXLSModel::data(const QModelIndex &index, int role) const
{
    if (!m_sheet)
    {
        return QVariant();
    }

    if (!index.isValid())
    {
        return QVariant();
    }

//    switch (m_sheet->cellType(index.row(), index.column()))
//    {
//        case CELLTYPE_EMPTY:
//        case CELLTYPE_BLANK:
//        case CELLTYPE_ERROR:
//            return QVariant();

//        default:
//            break;
//    }

    switch (role)
    {
        case Qt::DisplayRole:
            return displayData(index.row(), index.column());

        case Qt::EditRole:
            return editData(index.row(), index.column());

        case gidrCommentRole:
            return QString((const QChar *)m_sheet->readComment(index.row(), index.column()));

        case gidrRowExpandedRole:
            return true;

        case gidrRowHeightRole:
            return m_sheet->rowHeight(index.row());

        case gidrColWidthRole:
        {
            // 微软官方说：一般取的时标准字体宽度值 8.43
            return m_sheet->colWidth(index.column()) * 8.5;
        }

        case gidrRowHeaderData:
            return QVariant();

        case gidrMergeIDRole:
        {
            int nrowFirst = -1;
            int nrowLast = -1;
            int ncolFirst = -1;
            int ncolLast = -1;
            m_sheet->getMerge(index.row(), index.column(), &nrowFirst, &nrowLast, &ncolFirst, &ncolLast);

            if (nrowFirst >= 0 && ncolFirst >= 0)
            {
                return nrowFirst * 1000 + ncolFirst + 1;
            }
            else
            {
                return 0;
            }
        }

        default:
            break;
    }

    Format *format = NULL;

    switch (role)
    {
        case Qt::FontRole:
        case Qt::TextAlignmentRole:
        case Qt::BackgroundColorRole:
        case Qt::ForegroundRole:
            format = m_sheet->cellFormat(index.row(), index.column());

        default:
            break;
    }

    if (!format)
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::FontRole:
        {
            Font *pFont = format->font();

            if (!pFont)
            {
                return QVariant();
            }

            QFont result(QString((const QChar *)pFont->name()), pFont->size(),
                         -1, pFont->italic());
            result.setStrikeOut(pFont->strikeOut());
            result.setBold(pFont->bold());
            result.setUnderline(UNDERLINE_NONE != pFont->underline());
            return result;
        }

        case Qt::TextAlignmentRole:
        {
            int result = XLSAlignmentToQtAlignment(format->alignH(), format->alignV());
            return result;
        }

        case Qt::BackgroundColorRole:
        {
            Color color = format->patternForegroundColor();

            //修改导入excel颜色不正确的bug
            if (color >= COLOR_DEFAULT_FOREGROUND || color == Color(-1))
            {
                return QColor(255, 255, 255);
            }

            int nRed = 255;
            int nGreen = 255;
            int nBlue = 255;
            m_bakBook->colorUnpack(color, &nRed, &nGreen, &nBlue);
            return QColor(nRed, nGreen, nBlue);
        }

        case Qt::ForegroundRole:
        {
            Font *pFont = format->font();

            if (!pFont)
            {
                return QVariant();
            }

            Color color = pFont->color();

            if (color == Color(-1))
            {
                color = COLOR_BLACK;
            }

            if (color >= COLOR_DEFAULT_FOREGROUND)
            {
                return QVariant();
            }

            int nRed = 0;
            int nGreen = 0;
            int nBlue = 0;
            m_bakBook->colorUnpack(color, &nRed, &nGreen, &nBlue);
            return QColor(nRed, nGreen, nBlue);
        }

        default:
            return QVariant();
    }
}

bool GlodonXLSModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!m_sheet)
    {
        return false;
    }

    if (!m_editable)
    {
        return false;
    }

    switch (role)
    {
        case Qt::EditRole:
            return doSetEditData(index.row(), index.column(), value);
            break;

        default:
            break;
    }

    return false;
}

QVariant GlodonXLSModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!m_sheet)
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            if (orientation == Qt::Vertical)
            {
                return section + 1;
            }
            else
            {
                return QChar(QLatin1Char('A' + section));
            }
        }

        default:
            return QVariant();
    }
}

bool GlodonXLSModel::setHeaderData(int, Qt::Orientation, const QVariant &, int)
{
    return false;
}

Qt::ItemFlags GlodonXLSModel::flags(const QModelIndex &index) const
{
    if (!m_sheet)
    {
        return inherited::flags(index);
    }

    if (!index.isValid())
    {
        return 0;
    }

    Qt::ItemFlags result = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if (m_editable)
    {
        result |= Qt::ItemIsEditable;
    }

    return result;
}

QVariant GlodonXLSModel::displayData(int row, int col) const
{
    switch (m_sheet->cellType(row, col))
    {
        case CELLTYPE_EMPTY:
            return QString();

        case CELLTYPE_NUMBER:
            return m_sheet->readNum(row, col);

        case CELLTYPE_STRING:
        {
            return QString((const QChar *)m_sheet->readStr(row, col));
        }

        case CELLTYPE_BOOLEAN:
            return m_sheet->readBool(row, col);

        case CELLTYPE_BLANK:
            return QString();

        case CELLTYPE_ERROR:
        default:
            return QVariant();
    }
}

QVariant GlodonXLSModel::editData(int row, int col) const
{
    if (m_sheet->isFormula(row, col))
    {
        return QString("=") + QString((const QChar *)m_sheet->readFormula(row, col));
    }
    else
    {
        return displayData(row, col);
    }
}

bool GlodonXLSModel::doSetEditData(int row, int col, const QVariant &value)
{
    if (value.type() == QVariant::String)
    {
        const wchar_t *pWChar = (const wchar_t *)value.toString().constData();

        if (!value.isNull() && (pWChar[0] == L'='))
        {
            pWChar++;
            m_sheet->writeFormula(row, col, pWChar);
        }
        else
        {
            m_sheet->writeStr(row, col, pWChar);
        }

        return true;
    }
    else if (variantTypeIsNumeric(value.type()))
    {
        m_sheet->writeNum(row, col, value.toDouble());
        return true;
    }
    else if (value == QVariant())//字符串长度为0的情况
    {
        return true;
    }

    return false;
}

Book *loadxlsFile(const GString &fileName)
{
    GFileInfo fi(fileName);
    libxl::Book *oBook;

    if (fi.suffix() == "xls")
    {
        oBook = createXLSBook(true);
    }
    else
    {
        oBook = createXLSBook(false);
    }

    oBook->setRgbMode(true);
    oBook->load((const wchar_t *)fileName.constData());
    return oBook;
}
