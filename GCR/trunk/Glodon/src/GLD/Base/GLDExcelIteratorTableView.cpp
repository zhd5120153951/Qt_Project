#include "GLDExcelIteratorTableView.h"
#include "GLDTableView_p.h"
#include "GLDStrUtils.h"
#include "GLDStrings.h"

const char *c_hideAllMatchdRow          = QT_TRANSLATE_NOOP("GLD", "hide all identified rows");//"隐藏已识别行");
const char *c_showAllRow                = QT_TRANSLATE_NOOP("GLD", "display all rows"); //"显示所有行");
const char *c_selectAll                 = QT_TRANSLATE_NOOP("GLD", "select all"); //"全部选择");
const char *c_cancelAll                 = QT_TRANSLATE_NOOP("GLD", "cancel all"); //"全部取消");
const char *c_markAs                    = QT_TRANSLATE_NOOP("GLD", "mark as");  //"识别为");
const char *c_clearMatchRowIndentity    = QT_TRANSLATE_NOOP("GLD", "clear all matched records of types"); //"清除已匹配的记录类型");

class GlodonTableViewPrivate;
class GLDExcelIteratorTableViewPrivate : public GlodonTableViewPrivate
{
public:
  GLDExcelIteratorTableViewPrivate(GLDExcelIteratorTableView *parent)
    : q_ptr(parent), m_excelModel(NULL), m_clearRowRule(NULL), m_clearColRule(NULL),
      m_rowPopMenu(NULL), m_colPopMenu(NULL), m_selectPopMenu(NULL),
      m_contextMenu(NULL), m_markMenu(NULL)
  {
  }

private:
  GLDExcelIteratorTableView * const q_ptr;
  Q_DECLARE_PUBLIC(GLDExcelIteratorTableView);

  GEGExcelModel *m_excelModel;

  QAction *m_clearRowRule;
  QAction *m_clearColRule;
  // popu menu
  QMenu *m_rowPopMenu;
  QMenu *m_colPopMenu;
  QMenu *m_selectPopMenu;

  QMenu *m_contextMenu;
  QMenu *m_markMenu;
  int m_contextRow;
  QVector<int> m_selectedRows;
};

GLDExcelIteratorTableView::GLDExcelIteratorTableView(QWidget *parent):
    GlodonTableView(*new GLDExcelIteratorTableViewPrivate(this), parent)
{
    setItemDelegate(new GLDExcellIteratorDelegate(this));
    setSelectionMode(GlodonAbstractItemView::ExtendedSelection);
}

void GLDExcelIteratorTableView::setModel(GEGExcelModel *model)
{
    Q_D(GLDExcelIteratorTableView);
    GlodonTableView::setModel(model);
    d->m_excelModel = model;
    createPopupMenus();

    bindColumSetting();
    bindRowSetting();

    //设置右键
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(doGridContextMenuRequest(QPoint)));
}

GEGExcelModel *GLDExcelIteratorTableView::model() const
{
    Q_D(const GLDExcelIteratorTableView);
    return d->m_excelModel;
}

IGEGExcelGrid *GLDExcelIteratorTableView::getExcelGridIterator()
{
    Q_D(GLDExcelIteratorTableView);
    return new CGEGExcelGrid(d->m_excelModel);
}

void GLDExcelIteratorTableView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(GLDExcelIteratorTableView);
    GlodonTableView::mouseReleaseEvent(event);
    if ((d->m_selectedRows.count() > 1) && (Qt::NoButton != (event->button() & Qt::RightButton)))
    {
        d->m_selectedRows.clear();
        return;
    }
    QModelIndex currentIndex = indexAt(event->pos());
    if (d->m_excelModel->inColMatchedResultRow(currentIndex))
    {
        d->m_colPopMenu->popup(event->globalPos());
    }
    if (d->m_excelModel->inRowMatchedResultCol(currentIndex))
    {
        d->m_rowPopMenu->popup(event->globalPos());
    }
}

void GLDExcelIteratorTableView::createPopupMenus()
{
    Q_D(GLDExcelIteratorTableView);
    if (NULL == d->m_excelModel)
        return;

    d->m_colPopMenu = new QMenu(this);
    d->m_rowPopMenu = new QMenu(this);
    d->m_selectPopMenu = new QMenu(this);

    d->m_contextMenu = new QMenu(this);
}

void GLDExcelIteratorTableView::bindColumSetting()
{
    Q_D(GLDExcelIteratorTableView);
    for (int col = 0; col < this->model()->columnCount(); ++col)
    {
        int nWidth = d->m_excelModel->getColWidth(col);
        setColumnWidth(col, nWidth);
    }
}

void GLDExcelIteratorTableView::bindRowSetting()
{
    Q_D(GLDExcelIteratorTableView);
    for (int row = 0; row < this->model()->rowCount(); ++row)
    {
        int nHeight = d->m_excelModel->getRowHeight(row);
        setRowHeight(row, nHeight, false);
    }
}

void GLDExcelIteratorTableView::hideMatchedRows()
{
    Q_D(GLDExcelIteratorTableView);
    GString sIdentity;
    for (int i = 2; i <= d->m_excelModel->rowCount(); i++)
    {
        if (d->m_excelModel->rowIsIdentity(i - 2, sIdentity))
        {
            this->setRowHidden(i, true);
        }
    }
    refresh();
}

void GLDExcelIteratorTableView::showAllRows()
{
    Q_D(GLDExcelIteratorTableView);
    for (int i = 1; i <= d->m_excelModel->rowCount(); i++)
    {
        if (this->isRowHidden(i))
        {
            this->setRowHidden(i, false);
        }
    }
    refresh();
}

void GLDExcelIteratorTableView::refresh()
{
    refreshDisplayColRow();
    this->updateAll();
}

void GLDExcelIteratorTableView::prevUnmatchedRow()
{
    Q_D(GLDExcelIteratorTableView);
    QModelIndex curIndex = this->currentIndex();
    QModelIndex desIndex = d->m_excelModel->getPrevUnmatchedRow(curIndex);
    if (desIndex.isValid())
        setCurrentIndex(desIndex);
}

void GLDExcelIteratorTableView::nextUnmatchedRow()
{
    Q_D(GLDExcelIteratorTableView);
    QModelIndex curIndex = this->currentIndex();
    QModelIndex desIndex = d->m_excelModel->getNextUnmatchedRow(curIndex);
    if (desIndex.isValid())
        setCurrentIndex(desIndex);
}

void GLDExcelIteratorTableView::selectAllClick()
{
    Q_D(GLDExcelIteratorTableView);
    d->m_excelModel->setAllRowsCheckData(true);
}

void GLDExcelIteratorTableView::cancelAllClick()
{
    Q_D(GLDExcelIteratorTableView);
    d->m_excelModel->setAllRowsCheckData(false);
}

void GLDExcelIteratorTableView::clearMatchRowIndentity()
{
    Q_D(GLDExcelIteratorTableView);
    if (NULL != d->m_excelModel)
    {
        d->m_excelModel->removeRowIdentity(d->m_contextRow);
    }
}

QMenu *GLDExcelIteratorTableView::colPopMenu()
{
    Q_D(GLDExcelIteratorTableView);
    return d->m_colPopMenu;
}

QMenu *GLDExcelIteratorTableView::rowPopMenu()
{
    Q_D(GLDExcelIteratorTableView);
    return d->m_rowPopMenu;
}

QMenu *GLDExcelIteratorTableView::selectPopMenu()
{
    Q_D(GLDExcelIteratorTableView);
    return d->m_selectPopMenu;
}

QMenu *GLDExcelIteratorTableView::contextMenu()
{
    Q_D(GLDExcelIteratorTableView);
    return d->m_contextMenu;
}

void GLDExcelIteratorTableView::autoMatch()
{
    Q_D(GLDExcelIteratorTableView);
    if (NULL != d->m_excelModel)
    {
        d->m_excelModel->autoMatch();
        updateAll();
    }
}

void GLDExcelIteratorTableView::setCurrentIndexValue(const GString &value)
{
    Q_D(GLDExcelIteratorTableView);
    QModelIndex index = currentIndex();
    d->m_excelModel->setData(index, value);
    updateAll();
}

void GLDExcelIteratorTableView::doGridContextMenuRequest(QPoint value)
{
    Q_D(GLDExcelIteratorTableView);
    QModelIndexList indexList = selectedIndexes();
    if (0 != d->m_selectedRows.count())
    {
        d->m_selectedRows.clear();
    }
    for(int i = 0; i < indexList.count(); ++i)
    {
        if (! d->m_selectedRows.contains(indexList[i].row() - c_DATA_BEGIN_ROWNO))
        {
            d->m_selectedRows.push_back(indexList[i].row() - c_DATA_BEGIN_ROWNO);
        }
    }

    QModelIndex currentIndex = indexAt(value);
    if (1 == d->m_selectedRows.count())
    {
        if ((d->m_excelModel->inColMatchedResultRow(currentIndex))
                || (d->m_excelModel->inRowMatchedResultCol(currentIndex))
                || (0 == currentIndex.column())
                || (1 == currentIndex.row()))
        {
            return ;
        }
        d->m_contextMenu->popup(QCursor::pos());
    }
    else
    {
        d->m_selectPopMenu->popup(QCursor::pos());
    }
}

/* GLDExcellIteratorDelegate */

GLDExcellIteratorDelegate::GLDExcellIteratorDelegate(QObject *parent):
    GlodonDefaultItemDelegate(parent)
{

}

GEditStyle GLDExcellIteratorDelegate::editStyle(const QModelIndex &index, bool &readOnly) const
{
    if (index.row() >= c_DATA_BEGIN_ROWNO
            && index.column() == c_IS_EXPORT_COLNO)
    {
        return esBool;
    }
    if (index.row() == 0 || index.column() == 1)
    {
        return esNone;
    }
    else
    {
        return GlodonDefaultItemDelegate::editStyle(index, readOnly);
    }
}

bool GLDExcellIteratorDelegate::editable(const QModelIndex &index) const
{
    if (index.row() >= c_titleRow
            && index.column() >= c_IndexCol)
    {
        return false;
    }
    else
    {
        return true;
    }
}
